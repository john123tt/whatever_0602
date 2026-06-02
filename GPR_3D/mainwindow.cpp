#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "three_d_volume_view.h"
#include <complex>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <functional>
#include <limits>
#include <thread>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QMenu>
#include <QPushButton>
#include <QToolBar>
#include <QDockWidget>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSignalBlocker>
#include <QDateTime>
#include <QEvent>
#include <QFile>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QShortcut>
#include <QTextStream>
#include "setting.h"

using namespace GPR;

Q_DECLARE_METATYPE(GPR_Complex);


namespace {
const auto S2NS = 1000000000.0;
const auto CALCULATE_BACKGROUND_FRAMES = 50;
const auto REALTIME_PROCESSING_DELAY_FRAMES = 200;
const auto DATA_BLOCK_SIZE = 2000;
const auto TEMP_DIR_PATH = QDir::tempPath() + "/GPR/raw";
const int TRACK_AVG_RADIUS = 2;
constexpr double MINE_GROUND_TREND_MAX_DIFF_PTS = 1.0;
constexpr double MINE_GROUND_TREND_MAX_SLOPE_PTS = 2.0;
constexpr int MINE_GROUND_TREND_INIT_TRACES = 60;
constexpr int MINE_GROUND_TREND_RECOVER_TRACES = 20;
constexpr int MINE_GROUND_TREND_HISTORY = 9;
constexpr int MINE_GROUND_TREND_SMOOTH_TRACES = 5;
constexpr double MINE_GROUND_TREND_MIN_QUALITY = 1.10;
constexpr double MINE_GROUND_TREND_SEARCH_END_NS = 6.0;
auto normalize_value(double value) {
    if(value > 1.0) {
        return 1.0;
    } if(value < -1.0) {
        return 0.0;
    }
    return (value + 1.0) / 2.0;
}

double average_adjusted_window(
    const QVector<double>& data,
    const QVector<double>& background,
    const QVector<double>& gains,
    int center,
    int radius)
{
    const int size = std::min({data.size(), background.size(), gains.size()});
    if (size <= 0) {
        return 0.0;
    }
    const int clamped_center = std::max(0, std::min(center, size - 1));
    const int safe_radius = std::max(0, radius);
    const int start = std::max(0, clamped_center - safe_radius);
    const int end = std::min(size - 1, clamped_center + safe_radius);
    double sum = 0.0;
    for (int i = start; i <= end; ++i) {
        sum += std::abs((data[i] - background[i]) * gains[i]);
    }
    const int count = end - start + 1;
    return (count > 0) ? (sum / count) : 0.0;
}

double average_adjusted_range(
    const QVector<double>& data,
    const QVector<double>& background,
    const QVector<double>& gains,
    const QVector<double>& mean_background,
    int lower,
    int upper)
{
    const int size = std::min({data.size(), background.size(), gains.size()});
    if (size <= 0) {
        return 0.0;
    }
    int start = std::max(0, std::min(lower, upper));
    int end = std::min(size - 1, std::max(lower, upper));
    if (start > end) {
        return 0.0;
    }
    const bool has_mean_background = mean_background.size() >= size;
    double sum = 0.0;
    for (int i = start; i <= end; ++i) {
        const double mean_value = has_mean_background ? mean_background[i] : 0.0;
        sum += std::abs((data[i] - background[i] - mean_value) * gains[i]);
    }
    const int count = end - start + 1;
    return (count > 0) ? (sum / count) : 0.0;
}

double average_adjusted_range(
    const QVector<double>& data,
    const QVector<double>& background,
    const QVector<double>& gains,
    int lower,
    int upper)
{
    const int size = std::min({data.size(), background.size(), gains.size()});
    if (size <= 0) {
        return 0.0;
    }
    int start = std::max(0, std::min(lower, upper));
    int end = std::min(size - 1, std::max(lower, upper));
    if (start > end) {
        return 0.0;
    }
    double sum = 0.0;
    for (int i = start; i <= end; ++i) {
        sum += std::abs((data[i] - background[i]) * gains[i]);
    }
    const int count = end - start + 1;
    return (count > 0) ? (sum / count) : 0.0;
}

double mean_background_value(
    const QVector<double>& data,
    int index)
{
    return (index >= 0 && index < data.size()) ? data[index] : 0.0;
}

double median_finite(QVector<double> values)
{
    values.erase(std::remove_if(values.begin(), values.end(),
        [](double value) { return !std::isfinite(value); }), values.end());
    if (values.isEmpty()) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    std::sort(values.begin(), values.end());
    const int middle = values.size() / 2;
    if (values.size() % 2 == 1) {
        return values[middle];
    }
    return (values[middle - 1] + values[middle]) / 2.0;
}

double mean_finite(const QQueue<double>& values)
{
    double sum = 0.0;
    int count = 0;
    for (double value : values) {
        if (std::isfinite(value)) {
            sum += value;
            ++count;
        }
    }
    return count > 0 ? (sum / static_cast<double>(count)) : std::numeric_limits<double>::quiet_NaN();
}

void push_limited(QQueue<double>& values, double value, int limit)
{
    values.enqueue(value);
    while (values.size() > limit) {
        values.dequeue();
    }
}

void push_limited(QQueue<bool>& values, bool value, int limit)
{
    values.enqueue(value);
    while (values.size() > limit) {
        values.dequeue();
    }
}

double sign_value(double value)
{
    if (value > 0.0) {
        return 1.0;
    }
    if (value < 0.0) {
        return -1.0;
    }
    return 0.0;
}

QVector<double> average_columns(const QVector<QVector<double>>& columns, int start, int end) {
    if (columns.isEmpty()) {
        return {};
    }
    const int clamped_start = std::max(0, std::min(start, end));
    const int clamped_end = std::min<qsizetype>(columns.size() - 1, std::max(start, end));
    if (clamped_start > clamped_end) {
        return {};
    }
    const int channel_count = columns[0].size();
    QVector<double> averaged(channel_count, 0.0);
    const int count = clamped_end - clamped_start + 1;
    for (int i = clamped_start; i <= clamped_end; ++i) {
        const auto& column = columns[i];
        for (int ch = 0; ch < channel_count; ++ch) {
            averaged[ch] += column[ch];
        }
    }
    if (count > 1) {
        for (int ch = 0; ch < channel_count; ++ch) {
            averaged[ch] /= count;
        }
    }
    return averaged;
}

void track_average_push(
    QVector<QVector<double>>& buffer,
    const QVector<double>& column,
    int radius,
    const std::function<void(const QVector<double>&)>& emit_column)
{
    if (radius <= 0) {
        emit_column(column);
        return;
    }
    buffer.push_back(column);
    const int window = radius * 2 + 1;
    if (buffer.size() < window) {
        return;
    }
    emit_column(average_columns(buffer, 0, window - 1));
    buffer.remove(0);
}

void track_average_flush(
    QVector<QVector<double>>& buffer,
    int radius,
    const std::function<void(const QVector<double>&)>& emit_column)
{
    if (radius <= 0) {
        for (const auto& column : buffer) {
            emit_column(column);
        }
        buffer.clear();
        return;
    }
    for (int i = 0; i < buffer.size(); ++i) {
        const int start = std::max(0, i - radius);
        const int end = std::min<qsizetype>(buffer.size() - 1, i + radius);
        emit_column(average_columns(buffer, start, end));
    }
    buffer.clear();
}

struct RawBlockInfo {
    QString path;
    int x_size;
    int y_size;
    int z_size;
    int y_offset;
};

QVector<RawBlockInfo> list_raw_blocks(const QString& plot_id) {
    QVector<RawBlockInfo> blocks;
    auto dir = QDir(TEMP_DIR_PATH);
    if (!dir.exists()) {
        return blocks;
    }
    auto file_list = dir.entryInfoList(QDir::Files, QDir::Name);
    QCollator collator;
    collator.setNumericMode(true);
    collator.setCaseSensitivity(Qt::CaseInsensitive);
    std::sort(file_list.begin(), file_list.end(),
        [&] (const QFileInfo& a, const QFileInfo& b) {
            return collator.compare(a.fileName(), b.fileName()) < 0;
        });

    int y_offset = 0;
    const QString prefix = QString("raw_%1_").arg(plot_id);
    for (const auto& file_info : file_list) {
        if (!file_info.fileName().startsWith(prefix)) {
            continue;
        }
        QFile file(file_info.absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly)) {
            continue;
        }
        QDataStream in(&file);
        in.setByteOrder(QDataStream::LittleEndian);
        int x_size = 0;
        int y_size = 0;
        int z_size = 0;
        in >> x_size >> y_size >> z_size;
        if (x_size <= 0 || y_size <= 0 || z_size <= 0) {
            continue;
        }
        blocks.push_back({file_info.absoluteFilePath(), x_size, y_size, z_size, y_offset});
        y_offset += y_size;
    }
    return blocks;
}

void append_topview_columns_from_blocks(
    const QVector<RawBlockInfo>& blocks,
    int begin,
    int end,
    int lower,
    int upper,
    const QVector<QVector<double>>& background,
    const QVector<double>& gains,
    const QVector<double>& zero_background,
    int channel_count,
    QVector<char>& filled,
    double gain_parameter,
    QVector<QVector<double>>& out_columns)
{
    for (const auto& block : blocks) {
        const int block_begin = block.y_offset;
        const int block_end = block.y_offset + block.y_size;
        if (end <= block_begin || begin >= block_end) {
            continue;
        }
        const int local_begin = std::max(begin, block_begin) - block_begin;
        const int local_end = std::min(end, block_end) - block_begin;
        QFile file(block.path);
        if (!file.open(QIODevice::ReadOnly)) {
            continue;
        }
        const qint64 header_size = static_cast<qint64>(sizeof(qint32)) * 3;
        const qint64 row_size_bytes =
            static_cast<qint64>(block.x_size) * block.z_size * static_cast<qint64>(sizeof(double));
        QVector<double> channel(block.z_size);
        for (int t = local_begin; t < local_end; ++t) {
            const qint64 offset = header_size + row_size_bytes * t;
            if (!file.seek(offset)) {
                break;
            }
            QVector<double> column(channel_count);
            const int block_channels = block.x_size;
            for (int ch = 0; ch < block_channels && ch < channel_count; ++ch) {
                const qint64 bytes = static_cast<qint64>(block.z_size) * sizeof(double);
                if (file.read(reinterpret_cast<char*>(channel.data()), bytes) != bytes) {
                    break;
                }
                if (ch < background.size() && background[ch].size() == gains.size()) {
                    column[ch] = average_adjusted_range(
                        channel, background[ch], gains, lower, upper) * gain_parameter;
                } else {
                    column[ch] = average_adjusted_range(
                        channel, zero_background, gains, lower, upper) * gain_parameter;
                }
            }
            const int global_index = block_begin + t;
            const int out_index = global_index - begin;
            if (out_index >= 0 && out_index < out_columns.size()) {
                out_columns[out_index] = column;
                filled[out_index] = 1;
            }
        }
    }
}

void export_data_as_csv(const QVector<GPR_Complex>& data, std::ostream& out) {
    for(auto i = 0; i < data.size(); ++i) {
        if(i != 0) {
            out << std::endl;
        }
        out << data[i].real << ", " << data[i].imag;
    }
}

void export_data_as_csv(const QVector<QVector<double>>& data, std::ostream& out) {
    for(auto y = 0; y < data[0].size(); ++y) {
        if(y != 0) {
            out << std::endl;
        }
        for(auto x = 0; x < data.size(); ++x) {
            out << data[x][y];
            if(x != data.size() - 1) {
                out << ", ";
            }
        }
    }
}

QString get_cache_filename(int index, int res) {
    return QString("block_%1_res%2.dat").arg(index).arg(res);
}

void cache(const QVector<QVector<QVector<double>>>& data, int index,const QString& plot_id) {
    if(data.isEmpty() || data[0].isEmpty()||data[0][0].isEmpty()) {
        return;
    }
    QDir().mkpath(TEMP_DIR_PATH);
    auto file = QFile(QString("%1/raw_%2_%3.dat").arg(TEMP_DIR_PATH).arg(plot_id).arg(index));
    if(file.open(QIODevice::WriteOnly)) {
        auto out = QDataStream(&file);
        out.setByteOrder(QDataStream::LittleEndian);
        out << static_cast<int>(data.size()) << static_cast<int>(data[data.size()-1].size())<<static_cast<int>(data[0][0].size());
        qDebug()<<static_cast<int>(data[data.size()-1].size());
        auto block_size = data[0][0].size()* sizeof(double);
        for(auto y = 0; y < data[data.size()-1].size(); ++y) {
            for(auto x = 0; x < data.size(); ++x) {
            out.writeRawData(reinterpret_cast<const char*>(data[x][y].data()), block_size);
            }
        }
    }
}

void load_cache_plot(
    const QString& plot_id,
    std::function<void(const QVector<QVector<double>>&, bool)> callback,
    int count)
{
    auto dir = QDir(TEMP_DIR_PATH);
    if(!dir.exists()) {
        return;
    }
    int load_count = 0;
    QVector<QVector<double>> data_multichannel;
    auto file_list = dir.entryInfoList(QDir::Files, QDir::Name);
    QCollator collator;
    collator.setNumericMode(true);
    collator.setCaseSensitivity(Qt::CaseInsensitive);
    std::sort(file_list.begin(), file_list.end(),
      [&] (const QFileInfo& a, const QFileInfo& b) {
          return collator.compare(a.fileName(), b.fileName()) < 0;
      });
    const QString prefix = QString("raw_%1_").arg(plot_id);
    int expected_x = -1;
    int expected_z = -1;
    bool stopped_by_count = false;
    for(const auto& file_info : file_list) {
        if(!file_info.fileName().startsWith(prefix)) {
            continue;
        }
        auto file = QFile(file_info.absoluteFilePath());
        if(file.open(QIODevice::ReadOnly)) {
            auto in = QDataStream(&file);
            in.setByteOrder(QDataStream::LittleEndian);
            int x_size = 0;
            int y_size = 0;
            int z_size = 0;
            in >> x_size >> y_size >> z_size;
            if (x_size <= 0 || y_size <= 0 || z_size <= 0) {
                continue;
            }
            if (expected_x < 0) {
                expected_x = x_size;
                expected_z = z_size;
            } else if (x_size != expected_x || z_size != expected_z) {
                static const auto empty_data = QVector<QVector<double>>();
                callback(empty_data, true);
                return;
            }
            auto block_size = z_size * sizeof(double);
            auto data = QVector<double>(z_size);
            for(auto y = 0; y < y_size; ++y) {
                data_multichannel.clear();
                for(auto x = 0; x < x_size; ++x) {
                    in.readRawData(reinterpret_cast<char*>(data.data()), block_size);
                    data_multichannel.append(data);
                }
                callback(data_multichannel, false);
                ++load_count;
                if(load_count >= count && count != -1) {
                    stopped_by_count = true;
                    break;
                }
            }
            if (stopped_by_count) {
                break;
            }
        }
    }
    static const auto empty_data = QVector<QVector<double>>();
    callback(empty_data, true);
}

void load_cache(std::function<void(const QVector<QVector<double>>&, bool)> callback, int count) {
    load_cache_plot("1", std::move(callback), count);
}

void load_cache_all(
    std::function<void(QVector<QVector<QVector<double>>>&)> callback)
{
    QVector<QVector<QVector<double>>> empty_data;
    auto dir = QDir(TEMP_DIR_PATH);
    if (!dir.exists()) {
        callback(empty_data);
        return;
    }

    QVector<QVector<QVector<double>>> all_data;

    auto file_list = dir.entryInfoList(QDir::Files, QDir::Name);
    QCollator collator;
    collator.setNumericMode(true);
    collator.setCaseSensitivity(Qt::CaseInsensitive);
    std::sort(file_list.begin(), file_list.end(),
              [&](const QFileInfo& a, const QFileInfo& b) {
                  return collator.compare(a.fileName(), b.fileName()) < 0;
              });

    bool initialized = false;
    int y_offset = 0;

    for (const auto& file_info : file_list) {
        QFile file(file_info.absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly)) {
            continue;
        }

        QDataStream in(&file);
        in.setByteOrder(QDataStream::LittleEndian);

        int x_size = 0, y_size = 0, z_size = 0;
        in >> x_size >> y_size >> z_size;

        // ===== 第一次：确定 X 维 =====
        if (!initialized) {
            all_data.resize(x_size);
            initialized = true;
        }

        // ===== 扩展 Y 维 =====
        for (int x = 0; x < all_data.size(); ++x) {
            all_data[x].resize(y_offset + y_size);
        }

        QVector<double> channel(z_size);
        const int block_size = z_size * sizeof(double);

        // ===== 构造过程中直接写成 [X][Y][Z] =====
        for (int y = 0; y < y_size; ++y) {
            for (int x = 0; x < x_size; ++x) {
                in.readRawData(reinterpret_cast<char*>(channel.data()), block_size);
                all_data[x][y_offset + y] = channel;
            }
        }

        y_offset += y_size;
    }

    callback(all_data);
}


void clear_cache() {
    auto dir = QDir(TEMP_DIR_PATH);
    if(!dir.exists()) {
        return;
    }
    auto file_list = dir.entryInfoList(QDir::Files);
    for(const auto& file_info : file_list) {
        QFile::remove(file_info.absoluteFilePath());
    }
}
}

static void fft1d(QVector<std::complex<double>>& x, bool inverse)
{
    int N = x.size();
    QVector<std::complex<double>> out(N);
    double sign = inverse ? 1.0 : -1.0;

    for (int k = 0; k < N; ++k) {
        std::complex<double> sum(0, 0);
        for (int n = 0; n < N; ++n) {
            double ang = 2 * M_PI * k * n / N;
            sum += x[n] * std::exp(std::complex<double>(0, sign * ang));
        }
        out[k] = inverse ? sum / (double)N : sum;
    }
    x.swap(out);
}


static void fft2(QVector<QVector<std::complex<double>>>& data, bool inverse)
{
    int NX = data.size();
    int NY = data[0].size();

    // X 方向
    for (int y = 0; y < NY; ++y) {
        QVector<std::complex<double>> col(NX);
        for (int x = 0; x < NX; ++x)
            col[x] = data[x][y];

        fft1d(col, inverse);

        for (int x = 0; x < NX; ++x)
            data[x][y] = col[x];
    }

    // Y 方向
    for (int x = 0; x < NX; ++x)
        fft1d(data[x], inverse);
}

QVector<QVector<double>> removeBackgroundMean(
    const QVector<QVector<double>>& in,
    int kc = 21,   // channel 方向窗口（一定要大）
    int kt = 41    // trace 方向窗口
    )
{
    const int C = in.size();
    const int T = in[0].size();

    QVector<QVector<double>> out(C, QVector<double>(T, 0.0));

    int hc = kc / 2;
    int ht = kt / 2;

    for (int c = 0; c < C; ++c)
    {
        for (int t = 0; t < T; ++t)
        {
            double sum = 0.0;
            int cnt = 0;

            for (int dc = -hc; dc <= hc; ++dc)
            {
                int cc = c + dc;
                if (cc < 0 || cc >= C) continue;

                for (int dt = -ht; dt <= ht; ++dt)
                {
                    int tt = t + dt;
                    if (tt < 0 || tt >= T) continue;

                    sum += in[cc][tt];
                    cnt++;
                }
            }

            double bg = (cnt > 0) ? sum / cnt : 0.0;
            out[c][t] = in[c][t] - bg;
        }
    }

    return out;
}


QVector<QVector<double>> f_k_real(
    const QVector<QVector<double>>& s,
    double dear_x,
    double Lg,
    double Ls
    ) {
    int nr = s.size();
    if (nr == 0) return s;

    int nc = s[0].size();
    if (nc == 0) return s;

    // === 原始异常判断（不动）===
    if (Lg <= Ls || Lg >= dear_x * (nc - 1) / 2.0)
        return s;

    // === 实数 -> 复数 ===
    QVector<QVector<std::complex<double>>> spec(
        nr, QVector<std::complex<double>>(nc)
        );
    for (int r = 0; r < nr; ++r)
        for (int c = 0; c < nc; ++c)
            spec[r][c] = { s[r][c], 0.0 };

    // === FFT ===
    fft2(spec, false);

    // === 理论通带索引 ===
    int p1 = std::floor((nc - 1) * dear_x / Lg) + 1;
    int p2 = std::floor((nc - 1) * dear_x / Ls) + 1;

    // === clamp 到合法范围 ===
    int N1 = std::max(1, std::min(p1, nc / 2));
    int N2 = std::max(1, std::min(p2, nc / 2));

    if (N1 > N2) std::swap(N1, N2);

    // === 频域掩膜 ===
    QVector<QVector<std::complex<double>>> spec_filt(
        nr, QVector<std::complex<double>>(nc, {0.0, 0.0})
        );

    // 正频
    for (int r = 0; r < nr; ++r)
        for (int c = N1; c <= N2; ++c)
            spec_filt[r][c] = spec[r][c];

    // 负频（共轭对称）
    for (int r = 0; r < nr; ++r)
        for (int c = nc - N2; c <= nc - N1; ++c)
            spec_filt[r][c] = spec[r][c];

    // === IFFT ===
    fft2(spec_filt, true);

    // === 输出实部 ===
    QVector<QVector<double>> out(nr, QVector<double>(nc));
    for (int r = 0; r < nr; ++r)
        for (int c = 0; c < nc; ++c)
            out[r][c] = spec_filt[r][c].real();

    return out;
}

QVector<QVector<double>> spatialMedianFilter(
    const QVector<QVector<double>>& in,
    int kc = 3,   // channel 方向窗口
    int kt = 5    // trace 方向窗口
    )
{
    const int C = in.size();
    const int T = in[0].size();

    QVector<QVector<double>> out(C, QVector<double>(T, 0.0));

    QVector<double> window;
    window.reserve(kc * kt);

    int hc = kc / 2;
    int ht = kt / 2;

    for (int c = 0; c < C; ++c)
    {
        for (int t = 0; t < T; ++t)
        {
            window.clear();

            for (int dc = -hc; dc <= hc; ++dc)
            {
                int cc = c + dc;
                if (cc < 0 || cc >= C) continue;

                for (int dt = -ht; dt <= ht; ++dt)
                {
                    int tt = t + dt;
                    if (tt < 0 || tt >= T) continue;

                    window.push_back(in[cc][tt]);
                }
            }

            std::nth_element(
                window.begin(),
                window.begin() + window.size() / 2,
                window.end()
                );

            out[c][t] = window[window.size() / 2];
        }
    }

    return out;
}


QVector<QVector<double>> suppressClutterMADSoft(
    const QVector<QVector<double>>& in,
    int kc = 5,
    int kt = 11,
    double k = 2.5
    )
{
    const int C = in.size();
    const int T = in[0].size();

    QVector<QVector<double>> out(C, QVector<double>(T, 0.0));

    QVector<double> window;
    window.reserve(kc * kt);

    int hc = kc / 2;
    int ht = kt / 2;

    for (int c = 0; c < C; ++c)
    {
        for (int t = 0; t < T; ++t)
        {
            window.clear();

            for (int dc = -hc; dc <= hc; ++dc)
            {
                int cc = c + dc;
                if (cc < 0 || cc >= C) continue;

                for (int dt = -ht; dt <= ht; ++dt)
                {
                    int tt = t + dt;
                    if (tt < 0 || tt >= T) continue;

                    window.push_back(in[cc][tt]);
                }
            }

            if (window.size() < 5)
            {
                out[c][t] = in[c][t];
                continue;
            }

            // median
            auto mid = window.begin() + window.size() / 2;
            std::nth_element(window.begin(), mid, window.end());
            double med = *mid;

            // MAD
            QVector<double> dev(window.size());
            for (int i = 0; i < window.size(); ++i)
                dev[i] = std::abs(window[i] - med);

            std::nth_element(
                dev.begin(),
                dev.begin() + dev.size() / 2,
                dev.end()
                );
            double mad = dev[dev.size() / 2] + 1e-6;

            double v = in[c][t];
            double d = std::abs(v - med);

            // ===== 软权重 =====
            double w;
            if (d <= k * mad)
                w = d / (k * mad);   // 小偏差 -> 强压
            else
                w = 1.0;             // 明显异常 -> 保留

            out[c][t] = w * (v - med);
            if (std::abs(out[c][t]) < 1e-6)
                out[c][t] = 0.0;
            out[c][t] *= 6/511.4;
            // 这里保留原有幅值缩放

        }
    }

    return out;
}



    QVector<QVector<double>> spatialGradientSobel(
        const QVector<QVector<double>>& in,
        int kc = 3,   // channel 方向窗口
        int kt = 5    // trace 方向窗口
        )
{
        {
            const int C = in.size
                          ();
            const int T = in[0].size
                          ();

            QVector<QVector<
                double>> eroded(C, QVector<double>(T, 0.0
                                          ));
            QVector<QVector<
                double>> out(C, QVector<double>(T, 0.0
                                       ));

            int hc = kc / 2
                ;
            int ht = kt / 2
                ;

            // 腐蚀（取最小）
            for (int c = 0
                 ; c < C; ++c)
                for (int t = 0
                     ; t < T; ++t)
                {
                    double
                        v = in[c][t];
                    for (int
                             dc = -hc; dc <= hc; ++dc)
                        for (int
                                 dt = -ht; dt <= ht; ++dt)
                        {
                            int
                                cc = c + dc, tt = t + dt;
                            if (cc < 0 || cc >= C || tt < 0 || tt >= T) continue
                                    ;
                            v = std::
                                min
                                (v, in[cc][tt]);
                        }
                    eroded[c][t] = v;
                }

            // 膨胀（取最大）
            for (int c = 0
                 ; c < C; ++c)
                for (int t = 0
                     ; t < T; ++t)
                {
                    double
                        v = eroded[c][t];
                    for (int
                             dc = -hc; dc <= hc; ++dc)
                        for (int
                                 dt = -ht; dt <= ht; ++dt)
                        {
                            int
                                cc = c + dc, tt = t + dt;
                            if (cc < 0 || cc >= C || tt < 0 || tt >= T) continue
                                    ;
                            v = std::
                                max
                                (v, eroded[cc][tt]);
                        }
                    out[c][t] = v;
                }

            return
                out;
        }
}



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_adjust_gain_setting_dialog(nullptr),
    m_track_number(0),
    ui(new Ui::MainWindow),
    m_display_point(200),
    m_display_channel(0),
    m_gain_parameter(6/511.4),
    m_points(1024),
    m_is_eliminate_background(false),
    m_is_mean_background(false),
    m_is_mine_ground_trend_flatten_enabled(false),
    m_is_v2_hyperbola_detector_enabled(false),
    m_mean_background_count(0),
    m_is_display_data(false),
    m_channel_number(10),
    m_cache_block_index(0),
    m_linear_gain_initial(1.0),
    m_linear_gain_slope_per_ns(0.05)
  {
      qRegisterMetaType<QVector<QVector<QVector<GPR_Complex>>>>("QVector<QVector<QVector<GPR_Complex>>>");
      ui->setupUi(this);
      auto exitFullScreenShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
      exitFullScreenShortcut->setContext(Qt::WindowShortcut);
      connect(exitFullScreenShortcut, &QShortcut::activated, this, [this]() {
          if (isFullScreen()) {
              showNormal();
          }
      });
      ui->eliminate_background_combo_box->setCurrentIndex(1);
      m_settings.setValue(mine_ground_trend_flatten_enabled_key, false);
      m_settings.setValue(v2_hyperbola_detector_enabled_key, false);
      auto mean_background_label = new QLabel(tr("Mean background:"), ui->eliminate_background_group_box);
      auto mean_background_combo_box = new QComboBox(ui->eliminate_background_group_box);
      mean_background_combo_box->setObjectName("mean_background_combo_box");
      mean_background_combo_box->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
      mean_background_combo_box->setStyleSheet(ui->auto_adjust_gain_combo_box->styleSheet());
      mean_background_combo_box->addItem(ui->auto_adjust_gain_combo_box->itemText(0));
      mean_background_combo_box->addItem(ui->auto_adjust_gain_combo_box->itemText(1));
      mean_background_combo_box->setCurrentIndex(1);
      mean_background_label->setMinimumWidth(ui->background_frames_label->sizeHint().width());
      mean_background_label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
      auto linear_gain_initial_label = new QLabel(QString::fromUtf8("线性初始增益:"), ui->eliminate_background_group_box);
      auto linear_gain_initial_spin = new QDoubleSpinBox(ui->eliminate_background_group_box);
      linear_gain_initial_spin->setObjectName("linear_gain_initial_spin");
      linear_gain_initial_spin->setRange(0.0, 500.0);
      linear_gain_initial_spin->setDecimals(3);
      linear_gain_initial_spin->setSingleStep(0.1);
      linear_gain_initial_spin->setValue(m_linear_gain_initial);
      linear_gain_initial_spin->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
      auto linear_gain_slope_label = new QLabel(QString::fromUtf8("线性斜率(1/ns):"), ui->eliminate_background_group_box);
      auto linear_gain_slope_spin = new QDoubleSpinBox(ui->eliminate_background_group_box);
      linear_gain_slope_spin->setObjectName("linear_gain_slope_spin");
      linear_gain_slope_spin->setRange(-500.0, 500.0);
      linear_gain_slope_spin->setDecimals(4);
      linear_gain_slope_spin->setSingleStep(0.01);
      linear_gain_slope_spin->setValue(m_linear_gain_slope_per_ns);
      linear_gain_slope_spin->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
      auto time_transform_label = new QLabel(QString::fromUtf8("时域转换"), ui->eliminate_background_group_box);
      auto time_transform_combo = new QComboBox(ui->eliminate_background_group_box);
      time_transform_combo->setObjectName("main_time_domain_transform_combo");
      time_transform_combo->addItem(QString::fromUtf8("CZT（当前算法）"), 0);
      time_transform_combo->addItem(QString::fromUtf8("SDK 时域缩放（原版）"), 1);
      time_transform_combo->setCurrentIndex(m_settings.value(time_domain_transform_key, 0).toInt() == 1 ? 1 : 0);
      connect(time_transform_combo,
              QOverload<int>::of(&QComboBox::currentIndexChanged),
              this,
              [this](int index) {
                  m_settings.setValue(time_domain_transform_key, index == 1 ? 1 : 0);
                  if(auto combo = m_real_time_processing_setting_view->findChild<QComboBox*>("time_domain_transform_combo")) {
                      const bool blocked = combo->blockSignals(true);
                      combo->setCurrentIndex(index == 1 ? 1 : 0);
                      combo->blockSignals(blocked);
                  }
              });

      ui->eliminate_background_grid_layout->removeWidget(ui->eliminate_background_label);
      ui->eliminate_background_grid_layout->removeWidget(ui->eliminate_background_combo_box);
      ui->eliminate_background_grid_layout->removeWidget(ui->background_frames_label);
      ui->eliminate_background_grid_layout->removeWidget(ui->auto_adjust_gain_combo_box);
      ui->eliminate_background_label->hide();
      ui->eliminate_background_combo_box->hide();
      ui->background_frames_label->hide();
      ui->auto_adjust_gain_combo_box->hide();

      ui->eliminate_background_grid_layout->addWidget(mean_background_label, 1, 0);
      ui->eliminate_background_grid_layout->addWidget(mean_background_combo_box, 1, 1, 1, 2);
      ui->eliminate_background_grid_layout->addWidget(time_transform_label, 9, 0);
      ui->eliminate_background_grid_layout->addWidget(time_transform_combo, 9, 1, 1, 2);
      ui->eliminate_background_grid_layout->addWidget(linear_gain_initial_label, 10, 0);
      ui->eliminate_background_grid_layout->addWidget(linear_gain_initial_spin, 10, 1, 1, 2);
      ui->eliminate_background_grid_layout->addWidget(linear_gain_slope_label, 11, 0);
      ui->eliminate_background_grid_layout->addWidget(linear_gain_slope_spin, 11, 1, 1, 2);
      auto trend_flatten_label = new QLabel(QString::fromUtf8("趋势地面拉直v2:"), ui->eliminate_background_group_box);
      auto trend_flatten_combo = new QComboBox(ui->eliminate_background_group_box);
      trend_flatten_combo->setObjectName("main_mine_ground_trend_flatten_combo");
      trend_flatten_combo->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
      trend_flatten_combo->setStyleSheet(ui->auto_adjust_gain_combo_box->styleSheet());
      trend_flatten_combo->addItem(QString::fromUtf8("开启"), true);
      trend_flatten_combo->addItem(QString::fromUtf8("关闭"), false);
      trend_flatten_combo->setCurrentIndex(m_is_mine_ground_trend_flatten_enabled ? 0 : 1);
      auto v2_detector_label = new QLabel(QString::fromUtf8("V2谷值双曲:"), ui->eliminate_background_group_box);
      auto v2_detector_combo = new QComboBox(ui->eliminate_background_group_box);
      v2_detector_combo->setObjectName("main_v2_hyperbola_detector_combo");
      v2_detector_combo->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
      v2_detector_combo->setStyleSheet(ui->auto_adjust_gain_combo_box->styleSheet());
      v2_detector_combo->addItem(QString::fromUtf8("开启"), true);
      v2_detector_combo->addItem(QString::fromUtf8("关闭"), false);
      v2_detector_combo->setCurrentIndex(m_is_v2_hyperbola_detector_enabled ? 0 : 1);
      ui->eliminate_background_grid_layout->addWidget(trend_flatten_label, 12, 0);
      ui->eliminate_background_grid_layout->addWidget(trend_flatten_combo, 12, 1, 1, 2);
      ui->eliminate_background_grid_layout->addWidget(v2_detector_label, 13, 0);
      ui->eliminate_background_grid_layout->addWidget(v2_detector_combo, 13, 1, 1, 2);
      ui->eliminate_background_grid_layout->removeWidget(ui->map_k_label);
      ui->eliminate_background_grid_layout->removeWidget(ui->map_k_slider);
      ui->eliminate_background_grid_layout->removeWidget(ui->map_k_value_label);
      ui->eliminate_background_grid_layout->removeWidget(ui->max_depth_label);
      ui->eliminate_background_grid_layout->removeWidget(ui->max_depth_slider);
      ui->eliminate_background_grid_layout->removeItem(ui->horizontalLayout_3);
      ui->map_k_label->hide();
      ui->map_k_slider->hide();
      ui->map_k_value_label->hide();
      ui->max_depth_label->hide();
      ui->max_depth_slider->hide();
      ui->max_depth_value_spin_box->hide();
      setStyleSheet(R"(
        QMainWindow {
          background: #f5f7fa;
        }
        QWidget {
          color: #1f2933;
          font-family: "Microsoft YaHei UI", "Segoe UI", "Helvetica Neue", Arial, sans-serif;
          font-size: 12px;
        }
        QGroupBox {
          background: #ffffff;
          border: 1px solid #e3e8ef;
          border-radius: 8px;
          margin-top: 16px;
          padding: 12px;
        }
        QGroupBox::title {
          subcontrol-origin: margin;
          subcontrol-position: top left;
          padding: 0 6px;
          color: #4a5568;
          font-weight: 600;
        }
        QLabel {
          color: #4a5568;
        }
        QPushButton, QToolButton {
          background: #ffffff;
          border: 1px solid #cbd2d9;
          border-radius: 6px;
          padding: 4px 10px;
        }
        QPushButton:hover, QToolButton:hover {
          border-color: #3b82f6;
          color: #1d4ed8;
        }
        QPushButton:pressed, QToolButton:pressed {
          background: #e6f0ff;
        }
        QComboBox, QSpinBox, QDoubleSpinBox, QLineEdit {
          background: #ffffff;
          border: 1px solid #cbd2d9;
          border-radius: 6px;
          padding: 2px 6px;
          min-height: 22px;
        }
        QComboBox:hover, QSpinBox:hover, QDoubleSpinBox:hover, QLineEdit:hover {
          border-color: #3b82f6;
        }
        QComboBox::drop-down {
          border: none;
          width: 18px;
        }
        QSlider::groove:horizontal {
          height: 6px;
          background: #e4e7eb;
          border-radius: 3px;
        }
        QSlider::handle:horizontal {
          width: 14px;
          margin: -4px 0;
          border-radius: 7px;
          background: #3b82f6;
        }
        QToolBar {
          background: #f5f7fa;
          border: none;
          spacing: 6px;
        }
        QStatusBar {
          background: #f5f7fa;
          border-top: 1px solid #e3e8ef;
        }
        QMenu {
          background: #ffffff;
          border: 1px solid #e3e8ef;
        }
        QMenu::item:selected {
          background: #e6f0ff;
        }
      )");
      GPR_Init();
    init_dock_view();
    init_setting_view();
    init_status_bar();
    init_plots();
    // init3DVolumeView();
    // 在开始采集数据前连接信号
    connect(m_real_time_acquisition, &realtimeacquisition::data_ready, this, &MainWindow::on_display_data, Qt::QueuedConnection);
    connect(m_file_replay_view, &FileReplayView::data_ready, this, &MainWindow::on_display_data, Qt::QueuedConnection);
    connect(m_file_replay_view, &FileReplayView::data_batch_ready, this, &MainWindow::on_display_data_batch, Qt::QueuedConnection);
//     connect(this, &MainWindow::display_data_ready, m_image2d_plot, &Image2DPlot::on_data_ready, Qt::QueuedConnection);
//     connect(this, &MainWindow::display_data_ready_topview, m_image2d_plot, &Image2DPlot::on_data_ready, Qt::QueuedConnection);
//     connect(this, &MainWindow::display_data_ready_sideview, m_image2d_plot, &Image2DPlot::on_data_ready, Qt::QueuedConnection);
    connect(this, &MainWindow::processing_data_ready, this, &MainWindow::on_processing_data_ready);
    connect(this, &MainWindow::processing_data_ready_topview, this, &MainWindow::on_processing_data_ready_topview);
    connect(this, &MainWindow::processing_data_ready_sideview, this, &MainWindow::on_processing_data_ready_sideview);
    connect(m_real_time_acquisition, &realtimeacquisition::realtime_play, this, &MainWindow::on_realtime_play);
    connect(m_file_replay_view, &FileReplayView::replay, this, &MainWindow::on_replay);
    connect(m_file_replay_view, &FileReplayView::display_channel_changed,
            this, &MainWindow::on_replay_display_channel_changed);
    connect(m_real_time_acquisition, &realtimeacquisition::capture_state_changed,
            this, [this](bool running) {
                set_time_transform_selector_enabled(!running);
            });
    connect(m_file_replay_view, &FileReplayView::replay_state_changed,
            this, [this](bool running) {
                set_time_transform_selector_enabled(!running);
            });
    connect(m_image2d_plot, &Image2DPlot::double_click, this, &MainWindow::onPlotDoubleClick);
    connect(m_image2d_plot_topview, &Image2DPlot::double_click, this, &MainWindow::onPlotDoubleClick_topview);
    connect(m_image2d_plot_sideview, &Image2DPlot::double_click, this, &MainWindow::onPlotDoubleClick_sideview);
    connect(m_real_time_processing_setting_view, &RealTimeProcessingSettingView::eliminate_background,
            this, &MainWindow::on_eliminate_background);
    connect(m_real_time_processing_setting_view, &RealTimeProcessingSettingView::mine_ground_trend_flatten_changed,
            this, &MainWindow::on_mine_ground_trend_flatten_changed);
    connect(m_real_time_processing_setting_view, &RealTimeProcessingSettingView::v2_hyperbola_detector_changed,
            this, &MainWindow::on_v2_hyperbola_detector_changed);
    connect(m_image2d_plot, &Image2DPlot::x_range_changed, m_image2d_plot_topview, &Image2DPlot::sync_x_range);
    connect(m_image2d_plot_topview, &Image2DPlot::x_range_changed, m_image2d_plot, &Image2DPlot::sync_x_range);
    connect(m_real_time_processing_setting_view, &RealTimeProcessingSettingView::change_color_stops, m_image2d_plot, &Image2DPlot::setCustomGradient);
    connect(m_real_time_processing_setting_view, &RealTimeProcessingSettingView::change_color_stops, m_image2d_plot_topview, &Image2DPlot::setCustomGradient);
    connect(m_real_time_processing_setting_view, &RealTimeProcessingSettingView::change_color_stops, m_image2d_plot_sideview, &Image2DPlot::setCustomGradient);
    connect(m_real_time_processing_setting_view, &RealTimeProcessingSettingView::adjust_gain,
            this, &MainWindow::on_adjust_gain);
    connect(ui->eliminate_background_combo_box,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index) {
                bool enabled =index;
                m_real_time_processing_setting_view->set_eliminate_background(!enabled);
                // on_calculate_background(!enabled);
                m_is_eliminate_background=!enabled;
                if(!m_is_display_data){
                    on_eliminate_background(m_is_eliminate_background);
                }
            });
    connect(mean_background_combo_box,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::on_mean_background_combo_box_currentIndexChanged);
    connect(linear_gain_initial_spin,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,
            &MainWindow::on_linear_gain_changed);
    connect(linear_gain_slope_spin,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,
            &MainWindow::on_linear_gain_changed);
    connect(trend_flatten_combo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this, trend_flatten_combo](int) {
                const bool enabled = trend_flatten_combo->currentData().toBool();
                if (auto combo = m_real_time_processing_setting_view->findChild<QComboBox*>("mine_ground_trend_flatten_combo")) {
                    const QSignalBlocker blocker(combo);
                    combo->setCurrentIndex(enabled ? 0 : 1);
                }
                on_mine_ground_trend_flatten_changed(enabled);
            });
    connect(v2_detector_combo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this, v2_detector_combo](int) {
                const bool enabled = v2_detector_combo->currentData().toBool();
                if (auto combo = m_real_time_processing_setting_view->findChild<QComboBox*>("v2_hyperbola_detector_combo")) {
                    const QSignalBlocker blocker(combo);
                    combo->setCurrentIndex(enabled ? 0 : 1);
                }
                on_v2_hyperbola_detector_changed(enabled);
            });
    // createAndSetVolumeData();

    // QVector<QVector<QVector<float>>> data;
    // int xSize=100;
    // int ySize=1024;
    // int zSize=22;
    // data.resize(xSize);

    // // 创建一些模拟的 3D 形状
    // QVector3D center(xSize/2.0f, ySize/2.0f, zSize/2.0f);
    // float maxRadius = std::min(std::min(xSize, ySize), zSize) / 2.0f;

    // for (int x = 0; x < xSize; ++x) {
    //     data[x].resize(ySize);
    //     for (int y = 0; y < ySize; ++y) {
    //         data[x][y].resize(zSize);
    //         for (int z = 0; z < zSize; ++z) {
    //             // 计算到中心的距离
    //             QVector3D point(x, y, z);
    //             float distance = (point - center).length();

    //             // 创建球体
    //             float sphere = 0.0f;
    //             if (distance < maxRadius * 0.8f) {
    //                 sphere = 1.0f - (distance / (maxRadius * 0.8f));
    //             }

    //             // 添加一些噪声
    //             float noise = (std::sin(x * 0.3f) + std::cos(y * 0.2f) + std::sin(z * 0.4f)) / 6.0f + 0.5f;
    //             noise *= 0.2f; // 降低噪声强度

    //             // 组合结果
    //             data[x][y][z] = sphere + noise;

    //             // 确保值在合理范围内
    //             data[x][y][z] = std::max(0.0f, std::min(1.0f, data[x][y][z]));
    //         }
    //     }
    // }
    QTimer::singleShot(0, this, [this]() {
        showFullScreen();
    });
    // QVector<float> volume;
    // int Ns=100;
    // int Np=1024;
    // int Nc=22;
    // volume.resize(Ns * Np * Nc);

    // for(int x=0; x<Ns; ++x)
    //     for(int y=0; y<Np; ++y)
    //         for(int z=0; z<Nc; ++z)
    //         {
    //             volume[x*Np*Nc + y*Nc + z] = float(data[x][y][z]) / 255.0f;  // 灰度归一化
    //         }
    // auto m_Three_D_Volume_view= new Three_D_Volume_view();
    // m_Three_D_Volume_view->setVolume(volume, Ns, Np, Nc);

}

MainWindow::~MainWindow()
{
    clear_cache();
    GPR_Cleanup();
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape && isFullScreen()) {
        showNormal();
        event->accept();
        return;
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    update_side_panel_toggle_button_position();
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event)
{
    if ((watched == m_track_number_title_label || watched == m_track_number_label) &&
        event->type() == QEvent::MouseButtonRelease) {
        m_bscan_axis_annotations_visible = !m_bscan_axis_annotations_visible;
        if (m_image2d_plot) {
            m_image2d_plot->set_axis_annotations_visible(m_bscan_axis_annotations_visible);
        }
        return true;
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::update_side_panel_toggle_button_position()
{
    if (!m_side_panel_toggle_button) {
        return;
    }

    const int x = (!m_side_panel_collapsed && m_dock_widget && m_dock_widget->isVisible())
        ? std::max(0, m_dock_widget->geometry().right() + 1)
        : 0;
    const int y = std::max(0, (height() - m_side_panel_toggle_button->height()) / 2);
    m_side_panel_toggle_button->move(x, y);
    m_side_panel_toggle_button->raise();
}

void MainWindow::toggle_side_panel()
{
    m_side_panel_collapsed = !m_side_panel_collapsed;
    if (m_dock_widget) {
        m_dock_widget->setVisible(!m_side_panel_collapsed);
    }
    if (m_side_panel_toggle_button) {
        m_side_panel_toggle_button->setText(m_side_panel_collapsed ? QStringLiteral(">") : QStringLiteral("<"));
    }
    QTimer::singleShot(0, this, [this]() {
        update_side_panel_toggle_button_position();
    });
}

void MainWindow::init_dock_view() {
    m_dock_widget = new QDockWidget();
    m_dock_widget->setFeatures(QDockWidget::NoDockWidgetFeatures);
    auto empty_title_bar = new QWidget(m_dock_widget);
    empty_title_bar->setFixedHeight(0);
    m_dock_widget->setTitleBarWidget(empty_title_bar);
    addDockWidget(Qt::LeftDockWidgetArea, m_dock_widget);
    m_dock_widget->setFixedWidth(200);
    m_dock_content_widget = new QWidget(m_dock_widget);
    m_dock_content_layout = new QVBoxLayout(m_dock_content_widget);
    m_dock_content_layout->setContentsMargins(0, 0, 0, 0);
    m_dock_content_layout->setSpacing(8);
    m_dock_content_widget->setLayout(m_dock_content_layout);
    m_dock_widget->setWidget(m_dock_content_widget);
    ui->eliminate_background_group_box->hide();

    m_side_panel_toggle_button = new QToolButton(this);
    m_side_panel_toggle_button->setText(QStringLiteral("<"));
    m_side_panel_toggle_button->setObjectName("side_panel_toggle_button");
    m_side_panel_toggle_button->setToolButtonStyle(Qt::ToolButtonTextOnly);
    m_side_panel_toggle_button->setFixedSize(18, 120);
    m_side_panel_toggle_button->setCursor(Qt::PointingHandCursor);
    m_side_panel_toggle_button->setStyleSheet(R"(
      QToolButton#side_panel_toggle_button {
        background: #edf2f7;
        border: 1px solid #cbd2d9;
        border-radius: 4px;
        padding: 0;
        color: #1f2933;
      }
      QToolButton#side_panel_toggle_button:hover {
        border-color: #3b82f6;
        color: #1d4ed8;
      }
    )");
    connect(m_side_panel_toggle_button, &QToolButton::clicked,
            this, &MainWindow::toggle_side_panel);
    QTimer::singleShot(0, this, [this]() {
        update_side_panel_toggle_button_position();
    });
}

void MainWindow::init_setting_view() {
    m_real_time_acquisition = new realtimeacquisition(this);
    m_file_replay_view = new FileReplayView(this);
    m_real_time_processing_setting_view = new RealTimeProcessingSettingView(this);
    m_real_time_processing_setting_view->hide();

    if (auto layout = qobject_cast<QBoxLayout*>(m_real_time_acquisition->layout())) {
        const int insert_index = std::max(0, layout->count() - 1);
        layout->insertWidget(insert_index, m_file_replay_view);
        layout->insertWidget(std::max(0, layout->count() - 1), ui->eliminate_background_group_box);
        ui->eliminate_background_group_box->setStyleSheet(R"(
          QGroupBox#eliminate_background_group_box {
            background: transparent;
            border: 1px solid #cbd2d9;
            border-radius: 4px;
            margin-top: 1ex;
          }
          QGroupBox#eliminate_background_group_box::title {
            subcontrol-origin: margin;
            left: 8px;
            padding: 0 3px;
          }
        )");
        ui->eliminate_background_group_box->show();
    }
    if (m_dock_content_layout) {
        m_dock_content_layout->addWidget(m_real_time_acquisition, 1);
    }
    if (m_dock_widget) {
        m_dock_widget->show();
    }
    m_file_replay_view->show();
}

void MainWindow::init_status_bar() {
    m_status_info_widget = new QWidget(m_real_time_acquisition);
    auto layout = new QGridLayout(m_status_info_widget);
    layout->setContentsMargins(6, 0, 6, 0);
    m_track_number_title_label = new QLabel(QString::fromUtf8("采集道数:"), m_status_info_widget);
    m_track_number_title_label->setCursor(Qt::PointingHandCursor);
    m_track_number_title_label->installEventFilter(this);
    layout->addWidget(m_track_number_title_label, 0, 0);
    m_track_number_label = new QLabel("0");
    m_track_number_label->setCursor(Qt::PointingHandCursor);
    m_track_number_label->installEventFilter(this);
    layout->addWidget(m_track_number_label, 0, 1);
    m_replay_progress_bar = new QProgressBar(m_status_info_widget);
    m_replay_progress_bar->setRange(0, 100);
    m_replay_progress_bar->setValue(0);
    m_replay_progress_bar->setTextVisible(true);
    m_replay_progress_bar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_replay_progress_bar->hide();
    layout->addWidget(m_replay_progress_bar, 1, 0, 1, 2);
    if (auto panel_layout = qobject_cast<QBoxLayout*>(m_real_time_acquisition->layout())) {
        panel_layout->addWidget(m_status_info_widget);
    }
    statusBar()->hide();
}

QToolButton* MainWindow::make_tool_button(const QIcon& icon, const QString& text, QWidget* connected_view) {
    auto button = new QToolButton();
    button->setIcon(icon);
    button->setText(text);
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    button->setCheckable(true);
    m_button_group.addButton(button);
    connect(button, &QToolButton::clicked, [=] () {
        if(!m_dock_widget->isHidden() && m_current_dock_view == connected_view) {
            m_dock_widget->hide();
        } else {
            if (m_current_dock_view && m_current_dock_view != connected_view) {
                m_dock_content_layout->removeWidget(m_current_dock_view);
                m_current_dock_view->hide();
            }
            m_current_dock_view = connected_view;
            if (m_dock_content_layout->indexOf(connected_view) < 0) {
                m_dock_content_layout->insertWidget(0, connected_view, 1);
            }
            m_dock_content_layout->removeWidget(ui->eliminate_background_group_box);
            m_dock_content_layout->addWidget(ui->eliminate_background_group_box, 0);
            connected_view->show();
            ui->eliminate_background_group_box->show();
            m_dock_widget->setWindowTitle(text);
            m_dock_widget->show();
        }
    });
    return button;
}

void MainWindow::init_plots() {
    // 1. 创建主网格布局，直接挂到 centralwidget
    auto grid = new QGridLayout(ui->centralwidget);
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setSpacing(0);

    m_topview_range_container = new QWidget();
    auto range_layout = new QVBoxLayout(m_topview_range_container);
    range_layout->setContentsMargins(0, 0, 0, 0);
    range_layout->setSpacing(0);

    m_topview_range_top_spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Fixed);
    m_topview_range_bottom_spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Fixed);
    range_layout->addItem(m_topview_range_top_spacer);

    m_topview_range_slider = new RangeSlider();
    m_topview_range_slider->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    m_topview_range_slider = new RangeSlider();
    m_topview_range_slider->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    m_topview_range_slider->setFixedWidth(18);
    range_layout->addWidget(m_topview_range_slider, 1);

    range_layout->addWidget(m_topview_range_slider, 1);

    range_layout->addItem(m_topview_range_bottom_spacer);

    m_image2d_plot = new Image2DPlot("1");
    m_image2d_plot->set_x_axis_label(QString());
    m_image2d_plot->set_y_axis_label(QString());
    m_image2d_plot->set_axis_annotations_visible(false);
    connect(m_image2d_plot, &Image2DPlot::axisRectChanged,
            this, static_cast<void (MainWindow::*)(int, int, int)>(&MainWindow::alignAngleSliderWithYAxis));

    connect(m_image2d_plot, &Image2DPlot::axisRectChanged,
            this, &MainWindow::on_main_axis_rect_changed);


    m_image2d_plot_sideview = new Image2DPlot("2");
    m_image2d_plot_sideview->set_x_axis_label(QString::fromUtf8("道数"));
    m_image2d_plot_sideview->set_y_axis_label(QString::fromUtf8("时间标尺(ns)"));

    m_time_domain_plot = new TimeDomainPlot();
    m_time_domain_plot->set_x_axis_label(QString::fromUtf8("强度"));
    m_time_domain_plot->set_y_axis_label(QString::fromUtf8("时间标尺(ns)"));

    m_image2d_plot_topview = new Image2DPlot("3");
    m_image2d_plot_topview->set_x_axis_label(QString::fromUtf8("道数"));
    m_image2d_plot_topview->set_y_axis_label(QString::fromUtf8("时间标尺(ns)"));

    m_topview_range_container->hide();
    m_image2d_plot_sideview->hide();
    m_time_domain_plot->hide();
    m_image2d_plot_topview->hide();

    grid->addWidget(m_image2d_plot, 0, 0, 1, 4);

    grid->setColumnStretch(0, 6);
    grid->setColumnStretch(1, 0);
    grid->setColumnStretch(2, 0);
    grid->setColumnStretch(3, 0);

    grid->setRowStretch(0, 1);
    connect(m_image2d_plot_topview, &Image2DPlot::x_range_changed,
            this, &MainWindow::on_topview_range_changed);
    connect(m_topview_range_slider, &RangeSlider::rangeChanged,
            this, &MainWindow::on_topview_point_range_changed);
    connect(m_topview_range_slider, &RangeSlider::rangeReleased,
            this, &MainWindow::on_topview_avg_radius_released);

    auto map_k_from_slider = [](int value) {
        return static_cast<double>(value) / 1000.0;
    };
    ui->map_k_slider->setRange(0, 1000);
    ui->map_k_slider->setSingleStep(10);
    ui->map_k_slider->setValue(200);
    connect(ui->map_k_slider, &QSlider::valueChanged, this, [this, map_k_from_slider](int value) {
        const double k = map_k_from_slider(value);
        if (ui->map_k_value_label) {
            ui->map_k_value_label->setText(QString::number(k, 'f', 2));
        }
        on_map_k_changed(k);
    });
    if (ui->map_k_value_label) {
        ui->map_k_value_label->setText(QString::number(map_k_from_slider(ui->map_k_slider->value()), 'f', 2));
    }
    on_map_k_changed(map_k_from_slider(ui->map_k_slider->value()));

    m_topview_avg_timer = new QTimer(this);
    m_topview_avg_timer->setSingleShot(true);
    m_topview_avg_timer->setInterval(16);
    connect(m_topview_avg_timer, &QTimer::timeout,
            this, &MainWindow::update_topview_avg_preview);
    connect(this, &MainWindow::time_domain_data_ready, m_time_domain_plot, &TimeDomainPlot::on_data_ready, Qt::QueuedConnection);
    m_time_domain_plot->set_y_axis_label(QString::fromUtf8("????(ns)"));
    m_image2d_plot_topview->set_scale_axes(true, false);
    QTimer::singleShot(0, this, [this]() {
        alignAngleSliderWithYAxis();
    });

}

void MainWindow::on_display_data(const QVector<QVector<GPR_Complex>>& multichannel_data,bool is_last) {
    if(is_last) {
        if (m_is_v2_hyperbola_detector_enabled && !m_is_replay_loading) {
            apply_realtime_v2_hyperbola_backfill(true);
        }
        if (m_is_replay_loading) {
            cache(m_raw_data, m_cache_block_index, "1");
        } else {
            std::thread([=, raw_data = m_raw_data] () {
                cache(raw_data, m_cache_block_index,"1");
            }).detach();
        }
        m_raw_data.clear();
        ++m_cache_block_index;
        if (m_is_replay_loading) {
            finish_replay_loading();
            return;
        }
        if (m_realtime_processing_delay_pending) {
            set_mine_processing_switches_without_rebuild(
                m_pending_realtime_eliminate_background,
                m_pending_realtime_ground_trend_flatten,
                m_pending_realtime_v2_hyperbola_detector);
            m_is_mean_background = m_pending_realtime_mean_background;
            m_realtime_processing_delay_pending = false;
        }
        track_average_flush(
            m_topview_track_buffer,
            TRACK_AVG_RADIUS,
            [this](const QVector<double>& column) {
                m_image2d_plot_topview->append(column);
            });
        // m_switch_time_interval->setText("");
        m_image2d_plot->enable_realtime(false);
        m_image2d_plot_topview->enable_realtime(false);
        // m_background.clear();
        m_real_time_processing_setting_view->enable_eliminate_background_button(true);
        set_time_transform_selector_enabled(true);
        m_is_display_data=false;
        if (m_process_btn) {
            m_process_btn->setEnabled(true);
        }
        // createAndSetVolumeData();
        // exportDataToCSV();
        // m_background.clear();
        return;
    }
    if(multichannel_data[0].empty()) {
        return;
    }
    // for(int channel_number=0;channel_number<multichannel_data.size();channel_number++)
    // {
    m_channel_number=multichannel_data.size();
    m_points = multichannel_data[0].size();
    if (m_track_number == 0 && m_topview_range_slider) {
        const bool was_blocked = m_topview_range_slider->blockSignals(true);
        const int max_point = std::max(0, m_points - 1);
        int value = (m_display_point >= 0 && m_display_point <= max_point) ? m_display_point : 0;
        m_topview_range_slider->setRange(0, max_point);
        m_topview_range_slider->setValues(value, value);
        m_topview_range_low.store(value);
        m_topview_range_high.store(value);
        m_topview_range_slider->blockSignals(was_blocked);
    }
    if(m_background.isEmpty()){
        if (m_process_btn) {
            m_process_btn->setEnabled(false);
        }
        m_is_display_data=true;
        update_linear_gains();
        m_background = QVector<QVector<double>>(m_channel_number, QVector<double>(m_points, 0.0));
        m_mean_background = QVector<QVector<double>>(m_channel_number, QVector<double>(m_points, 0.0));
        m_mean_background_count = 0;
        m_base_data = QVector<QVector<double>>(m_channel_number, QVector<double>(m_points, 0.0));
    // auto background= QVector<QVector<double>>(m_channel_number, QVector<double>(m_points, 0.0));
    }
    // m_background.clear();

        if(m_track_number < CALCULATE_BACKGROUND_FRAMES) {
            // m_real_time_processing_setting_view->set_eliminate_background(false);
            // QMessageBox::warning(this, tr("GPR"), tr("???50?????"), QMessageBox::Ok);
            // return;

            for(auto i=0;i<multichannel_data.size();++i){
                for(auto j=0;j<multichannel_data[0].size();++j){
                    m_base_data[i][j] += multichannel_data[i][j].real/**5/511.4*/ / CALCULATE_BACKGROUND_FRAMES;
                }
            }
        }
        activate_delayed_realtime_processing_if_ready();
        // QVector<QVector<double>> background;
        if(m_is_eliminate_background){
            m_background= m_base_data;
        }else{
            m_background= QVector<QVector<double>>(m_channel_number, QVector<double>(m_points, 0.0));
        }
    auto strength_data = QVector<double>(m_points);
    auto strength_data_topview=QVector<double>(multichannel_data.size());
    const int safe_display_channel = std::max(0, std::min(m_display_channel, static_cast<int>(multichannel_data.size()) - 1));
    const int low = std::max(0, std::min(m_points - 1, m_topview_range_low.load()));
    const int high = std::max(0, std::min(m_points - 1, m_topview_range_high.load()));
    QVector<double> unit_gains(m_points, 1.0);
    const QVector<double>& gains_use = m_gains.isEmpty() ? unit_gains : m_gains;
    QVector<double> zero_background(m_points, 0.0);
    QVector<QVector<double>> strength_data_sideview(multichannel_data.size());
    auto real_data = QVector<double>(m_points);
    auto real_data_multi = QVector<double>(m_points);
    m_raw_data.resize(multichannel_data.size());
    const bool replay_deferred_processing =
        m_is_replay_loading &&
        (m_is_eliminate_background ||
         m_is_mean_background ||
         m_is_mine_ground_trend_flatten_enabled ||
         m_is_v2_hyperbola_detector_enabled);
    const bool should_buffer_realtime_v2 =
        m_is_v2_hyperbola_detector_enabled ||
        (m_realtime_processing_delay_pending && m_pending_realtime_v2_hyperbola_detector);
    if (should_buffer_realtime_v2 && !m_is_replay_loading &&
        m_v2_hyperbola_history.size() != multichannel_data.size()) {
        m_v2_hyperbola_history = QVector<QVector<QVector<double>>>(multichannel_data.size());
        m_v2_hyperbola_history_start_track = m_track_number;
        m_v2_hyperbola_next_output_track = m_track_number;
    }
    if (m_is_v2_hyperbola_detector_enabled && m_is_replay_loading &&
        !replay_deferred_processing &&
        m_replay_v2_channel_buffer.size() != multichannel_data.size()) {
        m_replay_v2_channel_buffer = QVector<QVector<QVector<double>>>(multichannel_data.size());
    }
    const bool needs_processed_trace =
        m_is_mean_background ||
        m_is_mine_ground_trend_flatten_enabled ||
        m_is_v2_hyperbola_detector_enabled ||
        m_realtime_processing_delay_pending;
    bool updated_mean_background = false;
    // m_raw_data_all_channel.resize(multichannel_data.size());
    // if(!m_is_eliminate_background){
    //     m_background.clear();
    //     m_background.resize(m_channel_number, QVector<double>(m_points));
    //     if(m_track_number < CALCULATE_BACKGROUND_FRAMES) {
    //         m_real_time_processing_setting_view->set_eliminate_background(false);
    //         QMessageBox::warning(this, tr("GPR"), tr("???50?????"), QMessageBox::Ok);
    //         return;
    //     }
    //     load_cache([=] (const QVector<QVector<double>>& data, bool is_eof) {
    //         if(is_eof) {
    //             return;
    //         }
    //         for(auto i = 0; i < data.size(); ++i) {
    //             for(auto j=0;j<data[0].size();++j){
    //                 m_background[i][j] += data[i][j] / CALCULATE_BACKGROUND_FRAMES;
    //             }
    //         }
    //     }, CALCULATE_BACKGROUND_FRAMES);
    // }
    for(auto i=0;i<multichannel_data.size();++i){
        auto data=multichannel_data[i];
        if(data.empty()/* || m_gains.size() != data.size()*/) {
            return;
        }
        for(auto j = 0; j < m_points; ++j) {
            real_data[j] = data[j].real;
        }
        if (i == safe_display_channel) {
            update_display_scale_reference(real_data);
        }
        m_raw_data[i].append(real_data);
        if(m_is_eliminate_background && i < m_background.size()) {
            m_background[i] = m_base_data[i];
        }
        if (!needs_processed_trace) {
            const bool has_background =
                i < m_background.size() && m_background[i].size() == real_data.size();
            const QVector<double>& background_for_display =
                has_background ? m_background[i] : zero_background;
            strength_data_topview[i] = average_adjusted_range(
                real_data, background_for_display, gains_use, low, high) * m_gain_parameter;
            if(i==safe_display_channel){
                for(auto j = 0; j < m_points; ++j) {
                    const double background_value =
                        (j < background_for_display.size()) ? background_for_display[j] : 0.0;
                    const double gain_value = (j < m_gains.size()) ? m_gains[j] : 1.0;
                    strength_data[j] = (real_data[j] - background_value) * gain_value * m_gain_parameter;
                }
                m_previous_frame = real_data;
            }
            continue;
        }
        const bool should_accumulate_mean_background =
            (m_is_mean_background ||
             (m_realtime_processing_delay_pending && m_pending_realtime_mean_background)) &&
            i < m_mean_background.size();
        if (should_accumulate_mean_background) {
            const int next_count = m_mean_background_count + 1;
            for(auto j = 0; j < m_points; ++j) {
                const double background_for_mean =
                    (m_realtime_processing_delay_pending &&
                     m_pending_realtime_eliminate_background &&
                     !m_is_eliminate_background &&
                     i < m_base_data.size() &&
                     j < m_base_data[i].size())
                        ? m_base_data[i][j]
                        : ((i < m_background.size() && j < m_background[i].size()) ? m_background[i][j] : 0.0);
                const double adjusted = real_data[j] -
                    background_for_mean;
                m_mean_background[i][j] =
                    (m_mean_background[i][j] * m_mean_background_count + adjusted) / next_count;
            }
            updated_mean_background = true;
        }
        QVector<double> adjusted_trace(m_points, 0.0);
        for(auto j = 0; j < m_points; ++j) {
            const double background_value =
                (i < m_background.size() && j < m_background[i].size()) ? m_background[i][j] : 0.0;
            const double mean_value =
                (m_is_mean_background && i < m_mean_background.size())
                    ? mean_background_value(m_mean_background[i], j)
                    : 0.0;
            adjusted_trace[j] = real_data[j] - background_value - mean_value;
        }
        QVector<double> output_trace = adjusted_trace;
        QVector<double> v2_trace = output_trace;
        if (m_is_mine_ground_trend_flatten_enabled) {
            output_trace = apply_mine_ground_trend_flatten_to_trace(i, output_trace);
            v2_trace = output_trace;
        } else if (m_realtime_processing_delay_pending &&
                   m_pending_realtime_ground_trend_flatten) {
            QVector<double> warmup_trace = adjusted_trace;
            if (m_pending_realtime_eliminate_background &&
                !m_is_eliminate_background &&
                i < m_base_data.size()) {
                for (int j = 0; j < warmup_trace.size() && j < m_base_data[i].size(); ++j) {
                    warmup_trace[j] = real_data[j] - m_base_data[i][j];
                }
            }
            v2_trace = apply_mine_ground_trend_flatten_to_trace(i, warmup_trace);
        } else if (m_realtime_processing_delay_pending &&
                   m_pending_realtime_eliminate_background &&
                   !m_is_eliminate_background &&
                   i < m_base_data.size()) {
            v2_trace = adjusted_trace;
            for (int j = 0; j < v2_trace.size() && j < m_base_data[i].size(); ++j) {
                v2_trace[j] = real_data[j] - m_base_data[i][j];
            }
        }
        if (should_buffer_realtime_v2 && !m_is_replay_loading &&
            i == safe_display_channel && i < m_v2_hyperbola_history.size()) {
            m_v2_hyperbola_history[i].append(
                m_is_v2_hyperbola_detector_enabled ? output_trace : v2_trace);
        } else if (m_is_v2_hyperbola_detector_enabled && m_is_replay_loading &&
                   !replay_deferred_processing &&
                   i < m_replay_v2_channel_buffer.size()) {
            m_replay_v2_channel_buffer[i].append(output_trace);
        }
        if (!output_trace.isEmpty() && i < m_background.size() && m_background[i].size() == gains_use.size()) {
            strength_data_topview[i] = average_adjusted_range(
                output_trace, zero_background, gains_use, low, high) * m_gain_parameter;
        } else if (!output_trace.isEmpty()) {
            strength_data_topview[i] = average_adjusted_range(
                output_trace, zero_background, gains_use, low, high) * m_gain_parameter;
        }
        if(i==safe_display_channel && !output_trace.isEmpty()){
            for(auto j = 0; j < m_points; ++j) {
                const double gain_value = (j < m_gains.size()) ? m_gains[j] : 1.0;
                strength_data[j] = output_trace[j] * gain_value * m_gain_parameter;
            }
            m_previous_frame = real_data;
        }
    }
    if (updated_mean_background) {
        ++m_mean_background_count;
    }
    if(m_raw_data[multichannel_data.size()-1].size() == DATA_BLOCK_SIZE) {
            if (m_is_replay_loading) {
                cache(m_raw_data, m_cache_block_index, "1");
            } else {
            std::thread([=, this, raw_data = m_raw_data] () {
                cache(raw_data, m_cache_block_index,"1");
            }).detach();
            }
            m_raw_data.clear();
            ++m_cache_block_index;
        }
        if (m_is_v2_hyperbola_detector_enabled && !m_is_replay_loading) {
            apply_realtime_v2_hyperbola_backfill(false);
        }
        if (m_is_replay_loading) {
            if (replay_deferred_processing) {
                m_track_number_label->setText(QString("%1").arg(++m_track_number));
                if (m_replay_progress_bar) {
                    const bool should_update_progress =
                        (m_track_number >= m_replay_total_frames) || (m_track_number % 64 == 0);
                    if (should_update_progress) {
                        m_replay_progress_bar->setValue(std::min(m_track_number, m_replay_total_frames));
                    }
                }
                return;
            }
            if (m_is_v2_hyperbola_detector_enabled) {
                track_average_push(
                    m_topview_track_buffer,
                    strength_data_topview,
                    TRACK_AVG_RADIUS,
                    [this](const QVector<double>& column) {
                        m_replay_topview_buffer.append(column);
                    });
                m_track_number_label->setText(QString("%1").arg(++m_track_number));
                if (m_replay_progress_bar) {
                    const bool should_update_progress =
                        (m_track_number >= m_replay_total_frames) || (m_track_number % 64 == 0);
                    if (should_update_progress) {
                        m_replay_progress_bar->setValue(std::min(m_track_number, m_replay_total_frames));
                    }
                }
                return;
            }
            m_replay_bscan_buffer.append(strength_data);
            track_average_push(
                m_topview_track_buffer,
                strength_data_topview,
                TRACK_AVG_RADIUS,
                [this](const QVector<double>& column) {
                    m_replay_topview_buffer.append(column);
                });
            m_track_number_label->setText(QString("%1").arg(++m_track_number));
            if (m_replay_progress_bar) {
                const bool should_update_progress =
                    (m_track_number >= m_replay_total_frames) || (m_track_number % 64 == 0);
                if (should_update_progress) {
                    m_replay_progress_bar->setValue(std::min(m_track_number, m_replay_total_frames));
                }
            }
            if (m_track_number % 64 == 0 || m_track_number >= m_replay_total_frames) {
                update_range_avg_display();
            }
            return;
        }
        // m_image2d_plot->append(strength_data);
        // if(channel_number == 0) {
            m_image2d_plot->append(strength_data);
            track_average_push(
                m_topview_track_buffer,
                strength_data_topview,
                TRACK_AVG_RADIUS,
                [this](const QVector<double>& column) {
                    m_image2d_plot_topview->append(column);
                });
            // for(int x=0;x<strength_data_sideview.size();++x)
            // {
            //     m_image2d_plot_sideview->append(strength_data_sideview[x]);
            // }
            // m_image2d_plot_sideview->append(strength_data);
        // }
        m_track_number_label->setText(QString("%1").arg(++m_track_number));
        update_range_avg_display();
}

void MainWindow::on_display_data_batch(const QVector<QVector<QVector<GPR_Complex>>>& data_batch)
{
    for (const auto& frame : data_batch) {
        on_display_data(frame, false);
    }
}

void MainWindow::on_processing_data_ready_sideview(const QVector<double>& data, bool is_last) {
    if(is_last) {
        m_image2d_plot_sideview->enable_realtime(false);
        m_real_time_processing_setting_view->enable_eliminate_background_button(true);
        // m_real_time_processing_setting_view->enable_adjust_gain_button(true);
        return;
    }
    if(data.empty()) {
        return;
    }

    m_image2d_plot_sideview->append(data,false);
}

void MainWindow::on_processing_data_ready_topview(const QVector<double>& data, bool is_last) {
    if(is_last) {
        m_image2d_plot_topview->enable_realtime(false);
        m_real_time_processing_setting_view->enable_eliminate_background_button(true);
        // m_real_time_processing_setting_view->enable_adjust_gain_button(true);
        return;
    }
    if(data.empty()) {
        return;
    }

    m_image2d_plot_topview->append(data,false);
}

void MainWindow::on_processing_data_ready(const QVector<double>& data, bool is_last) {
    if(is_last) {
        m_image2d_plot->enable_realtime(false);
        // m_image2d_plot_topview->enable_realtime(false);
        m_real_time_processing_setting_view->enable_eliminate_background_button(true);
        // m_real_time_processing_setting_view->enable_adjust_gain_button(true);
        return;
    }
    if(data.empty()) {
        return;
    }
    m_image2d_plot->append(data,false);
}

void MainWindow::reset_display_scale_reference()
{
    m_display_scale_sample_count = 0;
    m_display_scale_mean = 0.0;
    m_display_scale_m2 = 0.0;
    m_display_scale_clip = 1.0;
    m_display_scale_applied_clip = 1.0;
    m_display_scale_apply_timer.invalidate();
    if (m_image2d_plot) {
        m_image2d_plot->set_map_params(m_display_scale_clip);
    }
    if (m_image2d_plot_topview) {
        m_image2d_plot_topview->set_map_params(m_display_scale_clip);
    }
    if (m_image2d_plot_sideview) {
        m_image2d_plot_sideview->set_map_params(m_display_scale_clip);
    }
}

void MainWindow::apply_display_scale_clip(bool force)
{
    constexpr int apply_interval_ms = 250;
    constexpr double min_relative_change = 0.05;

    const double denominator = std::max(std::abs(m_display_scale_applied_clip), 1e-9);
    const double relative_change =
        std::abs(m_display_scale_clip - m_display_scale_applied_clip) / denominator;
    if (!force && relative_change < min_relative_change) {
        return;
    }
    if (!force) {
        if (!m_display_scale_apply_timer.isValid()) {
            m_display_scale_apply_timer.start();
            return;
        }
        if (m_display_scale_apply_timer.elapsed() < apply_interval_ms) {
            return;
        }
    }

    if (m_display_scale_apply_timer.isValid()) {
        m_display_scale_apply_timer.restart();
    } else {
        m_display_scale_apply_timer.start();
    }
    m_display_scale_applied_clip = m_display_scale_clip;

    if (m_image2d_plot) {
        m_image2d_plot->set_map_params(m_display_scale_clip);
    }
    if (m_image2d_plot_topview) {
        m_image2d_plot_topview->set_map_params(m_display_scale_clip);
    }
    if (m_image2d_plot_sideview) {
        m_image2d_plot_sideview->set_map_params(m_display_scale_clip);
    }
}

void MainWindow::update_display_scale_reference(const QVector<double>& raw_trace)
{
    if (raw_trace.isEmpty()) {
        return;
    }

    const qint64 max_reference_samples =
        static_cast<qint64>(std::max(1, m_channel_number)) *
        static_cast<qint64>(std::max(1, m_points)) *
        CALCULATE_BACKGROUND_FRAMES;
    if (m_display_scale_sample_count >= max_reference_samples) {
        return;
    }

    for (double value : raw_trace) {
        if (!std::isfinite(value)) {
            continue;
        }
        ++m_display_scale_sample_count;
        const double delta = value - m_display_scale_mean;
        m_display_scale_mean += delta / static_cast<double>(m_display_scale_sample_count);
        const double delta2 = value - m_display_scale_mean;
        m_display_scale_m2 += delta * delta2;
        if (m_display_scale_sample_count >= max_reference_samples) {
            break;
        }
    }

    if (m_display_scale_sample_count < 2) {
        return;
    }

    double sd = std::sqrt(m_display_scale_m2 / static_cast<double>(m_display_scale_sample_count - 1));
    if (!std::isfinite(sd) || sd < 1e-9) {
        sd = 1.0;
    }

    constexpr double clip_sigma = 6.0;
    constexpr double default_percent = 0.40;
    constexpr double soft_factor = 1.0 + (1.0 - default_percent) * 2.0;
    m_display_scale_clip = clip_sigma * sd * soft_factor * m_gain_parameter;
    if (!std::isfinite(m_display_scale_clip) || m_display_scale_clip < 1e-9) {
        m_display_scale_clip = 1.0;
    }

    apply_display_scale_clip(m_display_scale_sample_count >= max_reference_samples);
}

void MainWindow::rebuild_display_from_cache()
{
    m_image2d_plot->clear();
    m_image2d_plot->create(m_points, m_settings.value(start_time_key).toInt(),
                           m_settings.value(end_time_key).toInt(), -1);
    m_image2d_plot->enable_realtime(true);

    m_image2d_plot_topview->clear();
    m_image2d_plot_topview->create(m_channel_number, 0, m_channel_number, -1);
    m_image2d_plot_topview->enable_realtime(true);

    reset_display_scale_reference();
    m_topview_track_buffer.clear();
    m_track_number = 0;
    m_cache_block_index = 0;

    const int low = std::max(0, std::min(m_points - 1, m_topview_range_low.load()));
    const int high = std::max(0, std::min(m_points - 1, m_topview_range_high.load()));
    QVector<double> unit_gains(m_points, 1.0);
    const QVector<double>& gains_use = m_gains.isEmpty() ? unit_gains : m_gains;
    QVector<double> zero_background(m_points, 0.0);

    load_cache([&](const QVector<QVector<double>>& data, bool is_eof) {
        if (is_eof) {
            track_average_flush(
                m_topview_track_buffer,
                TRACK_AVG_RADIUS,
                [this](const QVector<double>& column) {
                    m_image2d_plot_topview->append(column);
                });
            m_image2d_plot->enable_realtime(false);
            m_image2d_plot_topview->enable_realtime(false);
            return;
        }
        if (data.isEmpty()) {
            return;
        }

        m_channel_number = static_cast<int>(data.size());
        if (!data[0].isEmpty()) {
            m_points = static_cast<int>(data[0].size());
        }

        const int display_channel = std::max(0, std::min(m_display_channel, static_cast<int>(data.size()) - 1));
        QVector<double> display_trace = data[display_channel];
        update_display_scale_reference(display_trace);
        QVector<double> strength_data(display_trace.size(), 0.0);
        for (int i = 0; i < strength_data.size(); ++i) {
            const double gain_value = (i < m_gains.size()) ? m_gains[i] : 1.0;
            strength_data[i] = display_trace[i] * gain_value * m_gain_parameter;
        }
        m_image2d_plot->append(strength_data);

        QVector<double> topview_column(data.size(), 0.0);
        for (int ch = 0; ch < data.size(); ++ch) {
            topview_column[ch] = average_adjusted_range(
                data[ch], zero_background, gains_use, low, high) * m_gain_parameter;
        }
        track_average_push(
            m_topview_track_buffer,
            topview_column,
            TRACK_AVG_RADIUS,
            [this](const QVector<double>& column) {
                m_image2d_plot_topview->append(column);
            });

        ++m_track_number;
    }, -1);

    if (m_track_number_label) {
        m_track_number_label->setText(QString("%1").arg(m_track_number));
    }
    update_range_avg_display();
}

void MainWindow::on_mine_ground_trend_flatten_changed(bool enabled)
{
    m_is_mine_ground_trend_flatten_enabled = enabled;
    m_settings.setValue(mine_ground_trend_flatten_enabled_key, enabled);

    if (auto combo = findChild<QComboBox*>("main_mine_ground_trend_flatten_combo")) {
        const QSignalBlocker blocker(combo);
        combo->setCurrentIndex(enabled ? 0 : 1);
    }
    if (auto combo = m_real_time_processing_setting_view->findChild<QComboBox*>("mine_ground_trend_flatten_combo")) {
        const QSignalBlocker blocker(combo);
        combo->setCurrentIndex(enabled ? 0 : 1);
    }

    reset_mine_ground_trend_flatten();
    if (!m_is_display_data && m_track_number > 0) {
        on_eliminate_background(m_is_eliminate_background);
    }
}

void MainWindow::on_v2_hyperbola_detector_changed(bool enabled)
{
    m_is_v2_hyperbola_detector_enabled = enabled;
    m_settings.setValue(v2_hyperbola_detector_enabled_key, enabled);

    if (auto combo = findChild<QComboBox*>("main_v2_hyperbola_detector_combo")) {
        const QSignalBlocker blocker(combo);
        combo->setCurrentIndex(enabled ? 0 : 1);
    }
    if (auto combo = m_real_time_processing_setting_view->findChild<QComboBox*>("v2_hyperbola_detector_combo")) {
        const QSignalBlocker blocker(combo);
        combo->setCurrentIndex(enabled ? 0 : 1);
    }

    reset_v2_hyperbola_history();
    if (!m_is_display_data && m_track_number > 0) {
        on_eliminate_background(m_is_eliminate_background);
    }
}

void MainWindow::reset_mine_ground_trend_flatten()
{
    m_mine_ground_trend_flatten_states.clear();
}

void MainWindow::reset_v2_hyperbola_history()
{
    ++m_v2_hyperbola_task_id;
    m_v2_hyperbola_task_in_progress.store(false);
    m_v2_hyperbola_flush_requested = false;
    m_v2_hyperbola_history.clear();
    m_v2_hyperbola_history_start_track = m_track_number;
    m_v2_hyperbola_next_output_track = m_track_number;
    m_v2_hyperbola_last_window_start = -1;
    m_v2_hyperbola_active_targets.clear();
    m_v2_hyperbola_committed_boxes.clear();
}

QVector<double> MainWindow::apply_mine_ground_trend_flatten_to_trace(
    int channel,
    const QVector<double>& trace,
    QVector<MineGroundTrendFlattenState>* state_buffer)
{
    if (trace.isEmpty() || m_points <= 0) {
        return trace;
    }
    auto& states = state_buffer ? *state_buffer : m_mine_ground_trend_flatten_states;
    const int required_state_count = std::max(m_channel_number, channel + 1);
    if (states.size() < required_state_count) {
        states.resize(required_state_count);
    }
    if (channel < 0 || channel >= states.size()) {
        return trace;
    }

    const double time_span_ns =
        m_settings.value(end_time_key).toDouble() - m_settings.value(start_time_key).toDouble();
    const double dt_ns = (m_points > 1 && time_span_ns > 0.0)
        ? time_span_ns / static_cast<double>(m_points - 1)
        : 1.0;
    const int trace_size = static_cast<int>(trace.size());
    const int search_end_idx = std::max(
        3,
        std::min(trace_size, static_cast<int>(std::round(MINE_GROUND_TREND_SEARCH_END_NS / dt_ns)) + 1));
    if (search_end_idx <= 0) {
        return trace;
    }

    QVector<double> score(search_end_idx, 0.0);
    bool valid = false;
    for (int i = 0; i < search_end_idx; ++i) {
        const double value = trace[i];
        if (std::isfinite(value)) {
            score[i] = std::abs(value);
            valid = true;
        }
    }
    if (!valid) {
        return trace;
    }
    if (search_end_idx >= 3) {
        QVector<double> smoothed = score;
        for (int i = 1; i < search_end_idx - 1; ++i) {
            smoothed[i] = (score[i - 1] + 2.0 * score[i] + score[i + 1]) / 4.0;
        }
        score = smoothed;
    }

    int raw_index = 0;
    double raw_amp = score[0];
    double bg_level = 0.0;
    for (int i = 0; i < search_end_idx; ++i) {
        bg_level += score[i];
        if (score[i] > raw_amp) {
            raw_amp = score[i];
            raw_index = i;
        }
    }
    bg_level = bg_level / static_cast<double>(search_end_idx) + std::numeric_limits<double>::epsilon();
    const double quality = raw_amp / bg_level;
    const bool raw_ok = raw_amp > 0.0 && std::isfinite(raw_amp) &&
                        std::isfinite(quality) && quality >= MINE_GROUND_TREND_MIN_QUALITY;
    const double raw_pick = static_cast<double>(raw_index);

    auto& state = states[channel];
    double pick = state.previous_pick;
    if (!state.initialized) {
        if (raw_ok) {
            push_limited(state.init_picks, raw_pick, MINE_GROUND_TREND_INIT_TRACES);
        }
        const double seed = mean_finite(state.init_picks);
        pick = std::isfinite(seed) ? seed : 0.0;
        state.previous_pick = std::max(0.0, std::min(static_cast<double>(search_end_idx - 1), pick));
        state.initialized = state.init_picks.size() >= MINE_GROUND_TREND_INIT_TRACES;
    } else {
        const double prev_pick = std::isfinite(state.previous_pick) ? state.previous_pick : 0.0;
        const double trend_mean = mean_finite(state.accepted_picks);
        double accepted_trend = 0.0;
        if (std::isfinite(trend_mean) && !state.accepted_picks.isEmpty()) {
            accepted_trend = (prev_pick - trend_mean) / static_cast<double>(state.accepted_picks.size());
            accepted_trend = std::max(-MINE_GROUND_TREND_MAX_DIFF_PTS,
                                      std::min(MINE_GROUND_TREND_MAX_DIFF_PTS, accepted_trend));
        }
        const double pred_pick = prev_pick + accepted_trend;
        double cand = prev_pick;

        if (state.hold_state) {
            push_limited(state.recovery_raw_picks, raw_pick, MINE_GROUND_TREND_RECOVER_TRACES);
            push_limited(state.recovery_valid, raw_ok, MINE_GROUND_TREND_RECOVER_TRACES);
            int valid_count = 0;
            QVector<double> valid_recovery;
            for (int i = 0; i < state.recovery_raw_picks.size() && i < state.recovery_valid.size(); ++i) {
                if (state.recovery_valid[i]) {
                    valid_recovery.append(state.recovery_raw_picks[i]);
                    ++valid_count;
                }
            }
            bool recover_ok = false;
            if (valid_count >= std::max(5, static_cast<int>(std::ceil(0.60 * state.recovery_raw_picks.size())))) {
                const int valid_recovery_size = static_cast<int>(valid_recovery.size());
                const double rec_slope = (valid_recovery.last() - valid_recovery.first()) /
                    static_cast<double>(std::max(1, valid_recovery_size - 1));
                QVector<double> rec_steps;
                for (int i = 1; i < valid_recovery.size(); ++i) {
                    rec_steps.append(std::abs(valid_recovery[i] - valid_recovery[i - 1]));
                }
                const double rec_step_med = rec_steps.isEmpty() ? 0.0 : median_finite(rec_steps);
                recover_ok = std::abs(rec_slope) <= MINE_GROUND_TREND_MAX_SLOPE_PTS &&
                             rec_step_med <= MINE_GROUND_TREND_MAX_SLOPE_PTS;
            }
            if (recover_ok && raw_ok) {
                const double delta = raw_pick - prev_pick;
                if (std::abs(delta) > MINE_GROUND_TREND_MAX_SLOPE_PTS) {
                    cand = prev_pick;
                } else if (std::abs(delta) > MINE_GROUND_TREND_MAX_DIFF_PTS) {
                    cand = prev_pick + sign_value(delta) * MINE_GROUND_TREND_MAX_DIFF_PTS;
                } else {
                    cand = raw_pick;
                }
                state.hold_state = false;
                state.hold_count = 0;
                state.recovery_raw_picks.clear();
                state.recovery_valid.clear();
            }
        } else if (raw_ok) {
            const double inst_slope = raw_pick - prev_pick;
            if (std::abs(inst_slope) > MINE_GROUND_TREND_MAX_SLOPE_PTS) {
                state.hold_state = true;
                state.hold_count = 1;
                state.recovery_raw_picks.clear();
                state.recovery_valid.clear();
                push_limited(state.recovery_raw_picks, raw_pick, MINE_GROUND_TREND_RECOVER_TRACES);
                push_limited(state.recovery_valid, true, MINE_GROUND_TREND_RECOVER_TRACES);
                cand = prev_pick;
            } else if (std::abs(inst_slope) > MINE_GROUND_TREND_MAX_DIFF_PTS) {
                cand = prev_pick + sign_value(inst_slope) * MINE_GROUND_TREND_MAX_DIFF_PTS;
            } else {
                cand = raw_pick;
            }
        } else {
            const double pred_delta = pred_pick - prev_pick;
            cand = std::abs(pred_delta) > MINE_GROUND_TREND_MAX_DIFF_PTS
                ? prev_pick + sign_value(pred_delta) * MINE_GROUND_TREND_MAX_DIFF_PTS
                : pred_pick;
        }

        pick = std::max(0.0, std::min(static_cast<double>(search_end_idx - 1), cand));
        state.previous_pick = pick;
    }

    push_limited(state.accepted_picks, state.previous_pick, MINE_GROUND_TREND_HISTORY);
    QQueue<double> smooth_values = state.accepted_picks;
    push_limited(smooth_values, state.previous_pick, MINE_GROUND_TREND_SMOOTH_TRACES);
    const double smooth_pick = std::isfinite(mean_finite(smooth_values)) ? mean_finite(smooth_values) : state.previous_pick;
    const double shift_pts = std::max(0.0, std::min(static_cast<double>(trace_size - 1), smooth_pick));

    QVector<double> output(trace_size, 0.0);
    const int k_int = std::max(0, std::min(trace_size - 1, static_cast<int>(std::floor(shift_pts))));
    const double frac = std::max(0.0, std::min(1.0, shift_pts - static_cast<double>(k_int)));
    for (int dst = 0; dst < trace_size; ++dst) {
        const int src0 = dst + k_int;
        const int src1 = src0 + 1;
        const double v0 = src0 < trace_size ? trace[src0] : 0.0;
        const double v1 = src1 < trace_size ? trace[src1] : 0.0;
        output[dst] = v0 * (1.0 - frac) + v1 * frac;
    }
    return output;
}

double MainWindow::current_dt_ns() const
{
    const double time_span_ns =
        m_settings.value(end_time_key).toDouble() - m_settings.value(start_time_key).toDouble();
    return (m_points > 1 && time_span_ns > 0.0)
        ? time_span_ns / static_cast<double>(m_points - 1)
        : 1.0;
}

QVector<double> MainWindow::build_bscan_trace(const QVector<double>& trace) const
{
    QVector<double> strength_data(m_points, 0.0);
    for (int p = 0; p < m_points && p < trace.size(); ++p) {
        const double gain_value = (p < m_gains.size()) ? m_gains[p] : 1.0;
        strength_data[p] = trace[p] * gain_value * m_gain_parameter;
    }
    return strength_data;
}

QVector<double> MainWindow::build_topview_column(const QVector<QVector<double>>& channel_traces) const
{
    const int channel_count = channel_traces.size();
    QVector<double> column(channel_count, 0.0);
    if (m_points <= 0) {
        return column;
    }
    const int low = std::max(0, std::min(m_points - 1, m_topview_range_low.load()));
    const int high = std::max(0, std::min(m_points - 1, m_topview_range_high.load()));
    QVector<double> unit_gains(m_points, 1.0);
    const QVector<double>& gains_use = m_gains.isEmpty() ? unit_gains : m_gains;
    QVector<double> zero_background(m_points, 0.0);
    for (int ch = 0; ch < channel_count; ++ch) {
        column[ch] = average_adjusted_range(
            channel_traces[ch],
            zero_background,
            gains_use,
            low,
            high) * m_gain_parameter;
    }
    return column;
}

int MainWindow::apply_realtime_v2_hyperbola_backfill(bool flush_all)
{
    if (!m_is_v2_hyperbola_detector_enabled || m_v2_hyperbola_history.isEmpty() || m_is_replay_loading) {
        return 0;
    }
    if (m_v2_hyperbola_task_in_progress.load()) {
        if (flush_all) {
            m_v2_hyperbola_flush_requested = true;
        }
        return 0;
    }
    const int channel_count = m_v2_hyperbola_history.size();
    const int display_channel = std::max(0, std::min(m_display_channel, channel_count - 1));
    if (display_channel < 0 || display_channel >= channel_count || m_points <= 0) {
        return 0;
    }
    const int available_count = static_cast<int>(m_v2_hyperbola_history[display_channel].size());
    if (available_count <= 0) {
        return 0;
    }

    V2HyperbolaParams params = default_v2_hyperbola_params();
    params.radii_tr = {300, 400};
    const int window_back = 1050;
    const int tail_guard = 50;
    const int detection_step = 80;
    const int available_end_track = m_v2_hyperbola_history_start_track + available_count;
    int window_start_track = available_end_track - window_back;
    int window_end_track = available_end_track - tail_guard;
    if (!flush_all && (window_start_track < m_v2_hyperbola_history_start_track ||
                       window_end_track <= window_start_track ||
                       window_start_track - m_v2_hyperbola_last_window_start < detection_step)) {
        return 0;
    }
    if (flush_all) {
        window_start_track = std::max(m_v2_hyperbola_history_start_track, available_end_track - window_back);
        window_end_track = available_end_track;
    }
    const int window_start_offset = window_start_track - m_v2_hyperbola_history_start_track;
    const int window_end_offset = window_end_track - m_v2_hyperbola_history_start_track;
    if (window_start_offset < 0 || window_end_offset > available_count ||
        window_end_offset - window_start_offset < 25) {
        return 0;
    }

    const double dt_ns = current_dt_ns();
    const int task_id = m_v2_hyperbola_task_id.fetch_add(1) + 1;
    QVector<QVector<double>> snapshot;
    snapshot.reserve(window_end_offset - window_start_offset);
    for (int t = window_start_offset; t < window_end_offset; ++t) {
        snapshot.append(m_v2_hyperbola_history[display_channel][t]);
    }

    m_v2_hyperbola_task_in_progress.store(true);
    m_v2_hyperbola_last_window_start = window_start_track;
    std::thread([=, this, snapshot = std::move(snapshot)]() mutable {
        V2HyperbolaResult result = detect_v2_hyperbola(snapshot, dt_ns, params);

        QMetaObject::invokeMethod(this, [this, task_id, window_start_track, window_end_track, flush_all,
                                         boxes = std::move(result.boxes)]() mutable {
            if (task_id != m_v2_hyperbola_task_id.load()) {
                return;
            }
            if (!m_is_v2_hyperbola_detector_enabled || m_is_replay_loading) {
                m_v2_hyperbola_task_in_progress.store(false);
                return;
            }

            QVector<int> matched_targets;
            QVector<int> finalize_targets;
            matched_targets.reserve(boxes.size());
            for (auto box : boxes) {
                box.x += window_start_track;
                const double box_right = box.x + box.width - 1.0;
                const bool clipped_by_left_window =
                    !flush_all && window_start_track > 0 && box.x <= window_start_track + 2.0;
                const bool clipped_by_right_window =
                    !flush_all && box_right >= window_end_track - 2.0;
                if (clipped_by_left_window || clipped_by_right_window) {
                    continue;
                }
                const double box_center = box.x + box.width * 0.5;
                int best_index = -1;
                double best_distance = std::numeric_limits<double>::max();
                for (int i = 0; i < m_v2_hyperbola_active_targets.size(); ++i) {
                    const auto& target = m_v2_hyperbola_active_targets[i];
                    const double target_center = target.last_box.x + target.last_box.width * 0.5;
                    const double center_distance = std::abs(box_center - target_center);
                    const double apex_distance = std::abs(box.apex_ns - target.last_box.apex_ns);
                    const double allowed_distance = std::max(180.0, 0.55 * std::max(box.width, target.last_box.width));
                    if (center_distance <= allowed_distance && apex_distance <= 0.85 &&
                        center_distance < best_distance) {
                        best_distance = center_distance;
                        best_index = i;
                    }
                }

                if (best_index < 0) {
                    V2RealtimeTarget target;
                    target.best_box = box;
                    target.last_box = box;
                    target.union_box = box;
                    target.peak_score = box.score;
                    target.last_score = box.score;
                    target.first_seen_track = window_start_track;
                    target.last_seen_track = window_end_track;
                    target.hit_count = 1;
                    target.miss_count = 0;
                    m_v2_hyperbola_active_targets.append(target);
                    matched_targets.append(m_v2_hyperbola_active_targets.size() - 1);
                    continue;
                }

                auto& target = m_v2_hyperbola_active_targets[best_index];
                const bool score_declined =
                    target.hit_count >= 3 &&
                    box.score < target.peak_score - std::max(0.12, 0.06 * std::abs(target.peak_score));
                const double union_x1 = std::min(target.union_box.x, box.x);
                const double union_x2 = std::max(
                    target.union_box.x + target.union_box.width - 1.0,
                    box.x + box.width - 1.0);
                const double union_y1 = std::min(target.union_box.y_ns, box.y_ns);
                const double union_y2 = std::max(
                    target.union_box.y_ns + target.union_box.height_ns,
                    box.y_ns + box.height_ns);
                target.union_box.x = union_x1;
                target.union_box.y_ns = union_y1;
                target.union_box.width = union_x2 - union_x1 + 1.0;
                target.union_box.height_ns = union_y2 - union_y1;
                target.union_box.score = std::max(target.union_box.score, box.score);
                target.union_box.hyperbola_score = std::max(target.union_box.hyperbola_score, box.hyperbola_score);
                if (box.score > target.peak_score) {
                    target.union_box.apex_ns = box.apex_ns;
                    target.union_box.radius = box.radius;
                    target.union_box.mode = box.mode;
                    target.union_box.edge = box.edge;
                }
                if (box.score > target.peak_score) {
                    target.peak_score = box.score;
                    target.best_box = box;
                }
                target.last_box = box;
                target.last_score = box.score;
                target.last_seen_track = window_end_track;
                target.miss_count = 0;
                ++target.hit_count;
                matched_targets.append(best_index);

                if (score_declined) {
                    if (!finalize_targets.contains(best_index)) {
                        finalize_targets.append(best_index);
                    }
                }
            }

            std::sort(finalize_targets.begin(), finalize_targets.end(), std::greater<int>());
            for (int target_index : finalize_targets) {
                if (target_index >= 0 && target_index < m_v2_hyperbola_active_targets.size()) {
                    apply_realtime_v2_hyperbola_box(m_v2_hyperbola_active_targets[target_index].union_box);
                    m_v2_hyperbola_active_targets.removeAt(target_index);
                    for (int& matched_index : matched_targets) {
                        if (matched_index == target_index) {
                            matched_index = -1;
                        } else if (matched_index > target_index) {
                            --matched_index;
                        }
                    }
                }
            }

            for (int i = m_v2_hyperbola_active_targets.size() - 1; i >= 0; --i) {
                bool matched = false;
                for (int matched_index : matched_targets) {
                    if (matched_index == i) {
                        matched = true;
                        break;
                    }
                }
                auto& target = m_v2_hyperbola_active_targets[i];
                if (!matched && target.last_seen_track < window_end_track) {
                    ++target.miss_count;
                }
                if (target.hit_count >= 2 && target.miss_count >= 2) {
                    apply_realtime_v2_hyperbola_box(target.union_box);
                    m_v2_hyperbola_active_targets.removeAt(i);
                }
            }

            if (flush_all) {
                for (int i = m_v2_hyperbola_active_targets.size() - 1; i >= 0; --i) {
                    if (m_v2_hyperbola_active_targets[i].hit_count > 0) {
                        apply_realtime_v2_hyperbola_box(m_v2_hyperbola_active_targets[i].union_box);
                    }
                    m_v2_hyperbola_active_targets.removeAt(i);
                }
            }

            const int keep_after_track = window_end_track - 2200;
            const int trim_count = std::max(0, keep_after_track - m_v2_hyperbola_history_start_track);
            if (trim_count > 0) {
                for (auto& channel_history : m_v2_hyperbola_history) {
                    const int remove_count = std::min(trim_count, static_cast<int>(channel_history.size()));
                    channel_history.erase(channel_history.begin(), channel_history.begin() + remove_count);
                }
                m_v2_hyperbola_history_start_track += trim_count;
            }

            update_range_avg_display();

            const bool pending_flush = m_v2_hyperbola_flush_requested;
            m_v2_hyperbola_flush_requested = false;
            m_v2_hyperbola_task_in_progress.store(false);
            if (m_is_v2_hyperbola_detector_enabled && !m_is_replay_loading) {
                apply_realtime_v2_hyperbola_backfill(pending_flush);
            }
        }, Qt::QueuedConnection);
    }).detach();
    return window_end_track - window_start_track;
}

void MainWindow::apply_realtime_v2_hyperbola_box(const V2HyperbolaBox& box)
{
    if (m_v2_hyperbola_history.isEmpty() || m_points <= 0 || box.width <= 0.0 || box.height_ns <= 0.0) {
        return;
    }
    V2HyperbolaBox incoming = box;
    int render_begin = static_cast<int>(std::floor(incoming.x));
    int render_end = static_cast<int>(std::ceil(incoming.x + incoming.width));

    int merge_index = -1;
    double best_overlap = 0.0;
    const double in_x1 = incoming.x;
    const double in_x2 = incoming.x + incoming.width - 1.0;
    const double in_center = (in_x1 + in_x2) * 0.5;
    for (int i = 0; i < m_v2_hyperbola_committed_boxes.size(); ++i) {
        const auto& old = m_v2_hyperbola_committed_boxes[i];
        const double old_x1 = old.x;
        const double old_x2 = old.x + old.width - 1.0;
        const double old_center = (old_x1 + old_x2) * 0.5;
        const double inter = std::min(in_x2, old_x2) - std::max(in_x1, old_x1) + 1.0;
        const double union_width = std::max(in_x2, old_x2) - std::min(in_x1, old_x1) + 1.0;
        const double overlap = std::max(0.0, inter) / std::max(1.0, union_width);
        const double center_distance = std::abs(in_center - old_center);
        const double allowed_distance = std::max(220.0, 0.65 * std::max(incoming.width, old.width));
        const double apex_distance = std::abs(incoming.apex_ns - old.apex_ns);
        if ((overlap >= 0.12 || center_distance <= allowed_distance) && apex_distance <= 1.0 &&
            overlap >= best_overlap) {
            best_overlap = overlap;
            merge_index = i;
        }
    }

    if (merge_index >= 0) {
        auto& old = m_v2_hyperbola_committed_boxes[merge_index];
        render_begin = std::min(render_begin, static_cast<int>(std::floor(old.x)));
        render_end = std::max(render_end, static_cast<int>(std::ceil(old.x + old.width)));
        const double x1 = std::min(old.x, incoming.x);
        const double x2 = std::max(old.x + old.width - 1.0, incoming.x + incoming.width - 1.0);
        const double y1 = std::min(old.y_ns, incoming.y_ns);
        const double y2 = std::max(old.y_ns + old.height_ns, incoming.y_ns + incoming.height_ns);
        old.x = x1;
        old.y_ns = y1;
        old.width = x2 - x1 + 1.0;
        old.height_ns = y2 - y1;
        const bool incoming_is_stronger = incoming.score > old.score;
        old.score = std::max(old.score, incoming.score);
        old.hyperbola_score = std::max(old.hyperbola_score, incoming.hyperbola_score);
        old.apex_ns = incoming_is_stronger ? incoming.apex_ns : old.apex_ns;
        old.radius = std::max(old.radius, incoming.radius);
        old.mode = std::max(old.mode, incoming.mode);
        old.edge = old.edge && incoming.edge;
    } else {
        m_v2_hyperbola_committed_boxes.append(incoming);
    }

    render_realtime_v2_hyperbola_range(render_begin, render_end);
}

void MainWindow::render_realtime_v2_hyperbola_range(int begin_track, int end_track)
{
    if (m_v2_hyperbola_history.isEmpty() || m_points <= 0 || begin_track >= end_track) {
        return;
    }
    const int channel_count = m_v2_hyperbola_history.size();
    const int display_channel = std::max(0, std::min(m_display_channel, channel_count - 1));
    if (display_channel < 0 || display_channel >= channel_count) {
        return;
    }
    auto& channel_history = m_v2_hyperbola_history[display_channel];
    if (channel_history.isEmpty()) {
        return;
    }

    const int history_end_track = m_v2_hyperbola_history_start_track + static_cast<int>(channel_history.size()) - 1;
    const int x1 = std::max(m_v2_hyperbola_history_start_track, begin_track);
    const int x2 = std::min(history_end_track, end_track - 1);
    if (x1 > x2) {
        return;
    }

    QVector<QVector<double>> traces;
    traces.reserve(x2 - x1 + 1);
    for (int track = x1; track <= x2; ++track) {
        const int local_index = track - m_v2_hyperbola_history_start_track;
        if (local_index >= 0 && local_index < channel_history.size()) {
            traces.append(channel_history[local_index]);
        }
    }
    if (traces.isEmpty()) {
        return;
    }

    QVector<V2HyperbolaBox> local_boxes;
    for (const auto& committed_box : m_v2_hyperbola_committed_boxes) {
        const double bx1 = committed_box.x;
        const double bx2 = committed_box.x + committed_box.width - 1.0;
        if (bx2 < x1 || bx1 > x2) {
            continue;
        }
        V2HyperbolaBox local_box = committed_box;
        local_box.x = committed_box.x - x1;
        local_boxes.append(local_box);
    }
    if (local_boxes.isEmpty()) {
        return;
    }

    const auto processed = apply_v2_hyperbola_gain(
        traces,
        local_boxes,
        current_dt_ns(),
        5.0,
        2.0,
        0,
        0,
        0,
        0);

    QVector<QVector<double>> bscan_patch;
    bscan_patch.reserve(processed.size());
    for (int i = 0; i < processed.size(); ++i) {
        bscan_patch.append(build_bscan_trace(processed[i]));
    }

    if (!bscan_patch.isEmpty()) {
        m_image2d_plot->replaceDataRange(bscan_patch, x1, true);
    }
}

void MainWindow::clear(int points,int all_channel_count) {
    m_track_number = 0;
    m_cache_block_index = 0;
    ++m_processing_rebuild_task_id;
    reset_display_scale_reference();
    m_raw_data.clear();
    m_raw_data_all_channel.clear();
    m_time_domain_plot->clear();
    m_background.clear();
    m_mean_background.clear();
    m_base_data.clear();
    m_mean_background_count = 0;
    reset_mine_ground_trend_flatten();
    reset_v2_hyperbola_history();
    m_image2d_plot->create(points, m_settings.value(start_time_key).toInt(),m_settings.value(end_time_key).toInt());
    m_image2d_plot_topview->create(all_channel_count,0,all_channel_count);
    m_image2d_plot_sideview->create(points,m_settings.value(start_time_key).toInt(),m_settings.value(end_time_key).toInt(),all_channel_count);
    m_image2d_plot->enable_realtime(true);
    m_image2d_plot_topview->enable_realtime(true);
    m_image2d_plot_sideview->enable_realtime(true);
    m_topview_overlay_enabled = false;
    m_image2d_plot_topview->set_overlay_visible(false);
    m_topview_track_buffer.clear();
    m_topview_track_emitted_first = false;
    if (m_topview_range_slider) {
        const bool was_blocked = m_topview_range_slider->blockSignals(true);
        const int max_point = std::max(0, points - 1);
        m_topview_range_slider->setRange(0, max_point);
        m_topview_range_slider->setValues(0, max_point);
        m_topview_range_low.store(0);
        m_topview_range_high.store(max_point);
        m_topview_range_slider->blockSignals(was_blocked);
    }
    // update_gains(points);
    clear_cache();
}

void MainWindow::set_mine_processing_switches_without_rebuild(bool background_enabled, bool ground_enabled, bool v2_enabled)
{
    m_is_eliminate_background = background_enabled;
    m_is_mine_ground_trend_flatten_enabled = ground_enabled;
    m_is_v2_hyperbola_detector_enabled = v2_enabled;
    m_settings.setValue(mine_ground_trend_flatten_enabled_key, ground_enabled);
    m_settings.setValue(v2_hyperbola_detector_enabled_key, v2_enabled);

    m_real_time_processing_setting_view->set_eliminate_background(background_enabled);
    if (ui && ui->eliminate_background_combo_box) {
        const QSignalBlocker blocker(ui->eliminate_background_combo_box);
        ui->eliminate_background_combo_box->setCurrentIndex(background_enabled ? 0 : 1);
    }
    if (auto combo = findChild<QComboBox*>("main_mine_ground_trend_flatten_combo")) {
        const QSignalBlocker blocker(combo);
        combo->setCurrentIndex(ground_enabled ? 0 : 1);
    }
    if (auto combo = findChild<QComboBox*>("main_v2_hyperbola_detector_combo")) {
        const QSignalBlocker blocker(combo);
        combo->setCurrentIndex(v2_enabled ? 0 : 1);
    }
    if (auto combo = m_real_time_processing_setting_view->findChild<QComboBox*>("mine_ground_trend_flatten_combo")) {
        const QSignalBlocker blocker(combo);
        combo->setCurrentIndex(ground_enabled ? 0 : 1);
    }
    if (auto combo = m_real_time_processing_setting_view->findChild<QComboBox*>("v2_hyperbola_detector_combo")) {
        const QSignalBlocker blocker(combo);
        combo->setCurrentIndex(v2_enabled ? 0 : 1);
    }
}

void MainWindow::activate_delayed_realtime_processing_if_ready()
{
    if (!m_realtime_processing_delay_pending ||
        m_track_number < REALTIME_PROCESSING_DELAY_FRAMES) {
        return;
    }

    set_mine_processing_switches_without_rebuild(
        m_pending_realtime_eliminate_background,
        m_pending_realtime_ground_trend_flatten,
        m_pending_realtime_v2_hyperbola_detector);
    m_is_mean_background = m_pending_realtime_mean_background;
    m_realtime_processing_delay_pending = false;
}

void MainWindow::on_realtime_play(std::vector<GPR_TxRxChannel> m_tx_rx_channels) {
    const bool requested_background = m_is_eliminate_background;
    const bool requested_mean_background = m_is_mean_background;
    const bool requested_ground = m_is_mine_ground_trend_flatten_enabled;
    const bool requested_v2 = m_is_v2_hyperbola_detector_enabled;
    m_pending_realtime_eliminate_background = requested_background;
    m_pending_realtime_mean_background = requested_mean_background;
    m_pending_realtime_ground_trend_flatten = requested_ground;
    m_pending_realtime_v2_hyperbola_detector = requested_v2;
    m_realtime_processing_delay_pending =
        requested_background || requested_mean_background || requested_ground || requested_v2;
    if (m_realtime_processing_delay_pending) {
        m_is_eliminate_background = false;
        m_is_mean_background = false;
        m_is_mine_ground_trend_flatten_enabled = false;
        m_is_v2_hyperbola_detector_enabled = false;
    }
    m_real_time_processing_setting_view->enable_eliminate_background_button(false);
    set_time_transform_selector_enabled(false);
    reset_mine_ground_trend_flatten();
    m_is_replay_loading = false;
    m_cache_block_index = 0;
    m_replay_bscan_buffer.clear();
    m_replay_topview_buffer.clear();
    m_replay_v2_channel_buffer.clear();
    if (m_replay_progress_bar) {
        m_replay_progress_bar->hide();
    }
    m_topview_overlay_enabled = false;
    m_image2d_plot_topview->set_overlay_visible(false);
    int all_channel_count=0;
    for(auto tx_rx_channels:m_tx_rx_channels)
    {
        int channel_count = std::popcount(tx_rx_channels.rx_channels);
        all_channel_count+=channel_count;
    }
    clear(128*std::pow(2,m_settings.value(points_key).toInt()),all_channel_count);
    m_last_session_was_realtime = true;
}

void MainWindow::on_replay(const GPR_FileHeader& file_header,int channel_number) {
    m_realtime_processing_delay_pending = false;
    m_real_time_processing_setting_view->enable_eliminate_background_button(false);
    set_time_transform_selector_enabled(false);
    reset_mine_ground_trend_flatten();
    m_topview_overlay_enabled = false;
    m_image2d_plot_topview->set_overlay_visible(false);
    m_is_replay_loading = true;
    m_cache_block_index = 0;
    m_channel_number = std::max(1, channel_number);
    m_display_channel = std::max(0, m_channel_number - 1);
    m_replay_total_frames = std::max(1, static_cast<int>(file_header.frames));
    m_replay_bscan_buffer.clear();
    m_replay_topview_buffer.clear();
    m_replay_v2_channel_buffer.clear();
    m_replay_bscan_buffer.reserve(m_replay_total_frames);
    m_replay_topview_buffer.reserve(m_replay_total_frames);
    if (m_replay_progress_bar) {
        m_replay_progress_bar->setRange(0, m_replay_total_frames);
        m_replay_progress_bar->setValue(0);
        m_replay_progress_bar->show();
    }
    clear(128*std::pow(2,static_cast<int>(file_header.frequency_range_points.points)),channel_number);
    m_last_session_was_realtime = false;
}

void MainWindow::on_replay_display_channel_changed(int channel)
{
    const int configured_channels = m_settings.value(channel_number, m_channel_number).toInt();
    const int channel_count = std::max({1, m_channel_number, configured_channels});
    const int safe_channel = std::max(0, std::min(channel, channel_count - 1));
    m_channel_number = channel_count;
    if (m_is_replay_loading && m_track_number == 0) {
        m_image2d_plot_topview->create(m_channel_number, 0, m_channel_number);
        m_image2d_plot_sideview->create(m_points, m_settings.value(start_time_key).toInt(),
                                        m_settings.value(end_time_key).toInt(), m_channel_number);
    }
    if (safe_channel == m_display_channel) {
        return;
    }

    m_display_channel = safe_channel;
    reset_v2_hyperbola_history();

    if (m_is_replay_loading || m_last_session_was_realtime || m_track_number <= 0 || m_is_display_data) {
        return;
    }

    on_eliminate_background(m_is_eliminate_background);
}

void MainWindow::onPlotDoubleClick(int x,int y,QString m_plot_id)
{
    if(m_plot_id!="1"||m_track_number==0){
        return;
    }
    m_display_point=y;
    int points=128*std::pow(2,m_settings.value(points_key).toInt());
    if (m_topview_range_slider) {
        const bool was_blocked = m_topview_range_slider->blockSignals(true);
        const int max_point = std::max(0, points - 1);
        const int value = std::max(0, std::min(y, max_point));
        const int span = std::max(0, m_topview_range_high.load() - m_topview_range_low.load());
        int new_low = value - span / 2;
        int new_high = new_low + span;
        if (new_low < 0) {
            new_low = 0;
            new_high = std::min(max_point, span);
        } else if (new_high > max_point) {
            new_high = max_point;
            new_low = std::max(0, max_point - span);
        }
        m_topview_range_slider->setRange(0, max_point);
        m_topview_range_slider->setValues(new_low, new_high);
        m_topview_range_low.store(new_low);
        m_topview_range_high.store(new_high);
        m_topview_range_slider->blockSignals(was_blocked);
    }
    m_image2d_plot_topview->clear();
    m_image2d_plot_topview->create(m_settings.value(channel_number).toInt(), 0, m_settings.value(channel_number).toInt(),-1);
    m_image2d_plot_topview->enable_realtime(true);
    m_image2d_plot_sideview->clear();
    m_image2d_plot_sideview->create(points, m_settings.value(start_time_key).toInt(), m_settings.value(end_time_key).toInt(),m_settings.value(channel_number).toInt());
    m_image2d_plot_sideview->enable_realtime(true);
    m_image2d_plot_sideview->set_x_axis_range(m_settings.value(channel_number).toInt());
    m_time_domain_plot->clear();
    auto data = m_image2d_plot->get_data(x, x + 1);
    if(data.size() != 1 || data[0].isEmpty()) {
        return;
    }
    auto start_time = static_cast<double>(m_settings.value(start_time_key).toInt());
    auto end_time = static_cast<double>(m_settings.value(end_time_key).toInt());
    auto time_step = (end_time - start_time) / (points - 1);
    auto time_domain_x = QVector<double>(points);
    auto time_domain_y = QVector<double>(points);
    for(auto i = 0; i < points; ++i) {
        time_domain_x[i] = start_time + i * time_step;
        time_domain_y[i] = data[0][i];
    }
    time_domain_data_ready(time_domain_x, time_domain_y);
    auto count = std::make_shared<std::atomic<int>>(0);
    const int low = std::max(0, std::min(points - 1, m_topview_range_low.load()));
    const int high = std::max(0, std::min(points - 1, m_topview_range_high.load()));
    // ?? gains????????
    // qDebug()<<x;
    std::thread([=] () mutable { // ?? gains
            QVector<QVector<double>> topview_buffer;
            topview_buffer.reserve(TRACK_AVG_RADIUS * 2 + 2);
            QVector<double> unit_gains(points, 1.0);
            const QVector<double>& gains_use = m_gains.isEmpty() ? unit_gains : m_gains;
            QVector<double> zero_background(points, 0.0);
            load_cache([=, this] (const QVector<QVector<double>>& data, bool is_eof)mutable { // ???? this
                if (is_eof) {
                    track_average_flush(
                        topview_buffer,
                        TRACK_AVG_RADIUS,
                        [this](const QVector<double>& column) {
                            emit processing_data_ready_topview(column);
                        });
                    emit processing_data_ready_topview({}, true);
                    return;
                }
                auto strength_data = QVector<double>(data.size());
                for (int i = 0; i < data.size(); ++i) {
                    if (i < m_background.size() && m_background[i].size() == gains_use.size()) {
                        strength_data[i] = average_adjusted_range(
                            data[i], m_background[i], gains_use, low, high) * m_gain_parameter;
                    } else {
                        strength_data[i] = average_adjusted_range(
                            data[i], zero_background, gains_use, low, high) * m_gain_parameter;
                    }
                }
                track_average_push(
                    topview_buffer,
                    strength_data,
                    TRACK_AVG_RADIUS,
                    [this](const QVector<double>& column) {
                        emit processing_data_ready_topview(column);
                    });
                if((*count)==x){
                            // qDebug()<<(*count)<<x;
                    // for(int channel=0;channel<data.size();channel++){
                    //     auto strength_data_sideview = QVector<double>(data[channel].size());
                    //     for(int point=0;point<data[channel].size();point++){
                    //         strength_data_sideview[point]=data[channel][point]*m_gain_parameter;
                    //     }

                    //     emit processing_data_ready_sideview(strength_data_sideview);
                    // }
                    for(int channel = 0; channel <= data.size(); channel++) {
                        if(channel==0){
                            auto strength_data_sideview = QVector<double>(data[channel].size());
                            for(int point = 0; point < data[channel].size(); point++) {
                                strength_data_sideview[point] = (data[channel][point] - m_background[channel][point])*m_gains[point]* m_gain_parameter;
                            }

                            emit processing_data_ready_sideview(strength_data_sideview);
                        }else if(channel==data.size())
                        {
                            int last_index = data.size() - 1;
                            auto strength_data_sideview = QVector<double>(data[last_index].size());
                            for(int point = 0; point < data[last_index].size(); point++) {
                                strength_data_sideview[point] = (data[last_index][point] - m_background[last_index][point])*m_gains[point] * m_gain_parameter;
                            }

                            emit processing_data_ready_sideview(strength_data_sideview);
                        }else
                        {
                            auto strength_data_sideview = QVector<double>(data[channel].size());
                            for(int point = 0; point < data[channel].size(); point++) {
                                // ???? channel ???? channel ????
                                strength_data_sideview[point] = (data[channel-1][point] + data[channel][point]- m_background[channel-1][point]-m_background[channel][point]) / 2.0*m_gains[point] * m_gain_parameter;
                            }
                            emit processing_data_ready_sideview(strength_data_sideview);
                        }
                    }
                }
            (*count)++;
        }, -1);
    }).detach();
}
void MainWindow::onPlotDoubleClick_sideview(int x,int y,QString m_plot_id)
{
    if(m_plot_id!="2"){
        return;
    }
    // if (m_isProcessing.load()) {
    //     qDebug()<<"多余数据";
    //     return;
    // }
    qDebug()<<"事件触发";
    // ??????
    if(x>=m_settings.value(channel_number).toInt()){
        x=m_settings.value(channel_number).toInt()-1;
    }
    int points=128*std::pow(2,m_settings.value(points_key).toInt());
    if (m_topview_range_slider) {
        const bool was_blocked = m_topview_range_slider->blockSignals(true);
        const int max_point = std::max(0, points - 1);
        const int value = std::max(0, std::min(y, max_point));
        const int span = std::max(0, m_topview_range_high.load() - m_topview_range_low.load());
        int new_low = value - span / 2;
        int new_high = new_low + span;
        if (new_low < 0) {
            new_low = 0;
            new_high = std::min(max_point, span);
        } else if (new_high > max_point) {
            new_high = max_point;
            new_low = std::max(0, max_point - span);
        }
        m_topview_range_slider->setRange(0, max_point);
        m_topview_range_slider->setValues(new_low, new_high);
        m_topview_range_low.store(new_low);
        m_topview_range_high.store(new_high);
        m_topview_range_slider->blockSignals(was_blocked);
    }
    m_display_channel=x;
    m_image2d_plot->clear();
    m_image2d_plot->create(points, m_settings.value(start_time_key).toInt(),m_settings.value(end_time_key).toInt(),-1);
    m_image2d_plot->enable_realtime(true);
    m_image2d_plot_topview->clear();
    m_image2d_plot_topview->create(m_settings.value(channel_number).toInt(), 0, m_settings.value(channel_number).toInt(),-1);
    m_image2d_plot_topview->enable_realtime(true);
    // m_image2d_plot_sideview->set_x_axis_range(m_settings.value(channel_number).toInt());
    // ?? gains????????
    QVector<int> gains(points, 1);
    auto count = std::make_shared<std::atomic<int>>(0);
    const int low = std::max(0, std::min(points - 1, m_topview_range_low.load()));
    const int high = std::max(0, std::min(points - 1, m_topview_range_high.load()));

    // 缓存线程
    // std::thread([=, raw_data = m_all_raw_data] () {
    //     try {
    //         cache(raw_data, m_track_number / DATA_BLOCK_SIZE, "1");
    //         cache_promise->set_value();
    //     } catch (...) {
    //         // ???????????
    //         cache_promise->set_exception(std::current_exception());
    //     }
    // }).detach();

    // ????
    std::thread([=, gains_copy = gains] () mutable { // ?? gains
        QVector<QVector<double>> topview_buffer;
        topview_buffer.reserve(TRACK_AVG_RADIUS * 2 + 2);
        QVector<double> unit_gains(points, 1.0);
        const QVector<double>& gains_use = m_gains.isEmpty() ? unit_gains : m_gains;
        QVector<double> zero_background(points, 0.0);
        try {
            // 等待缓存完成，可能会抛出异常
            load_cache([=, this] (const QVector<QVector<double>>& data, bool is_eof)mutable { // ???? this
                if (is_eof) {
                    track_average_flush(
                        topview_buffer,
                        TRACK_AVG_RADIUS,
                        [this](const QVector<double>& column) {
                            emit processing_data_ready_topview(column);
                        });
                    // ?? Qt ???????????
                    QMetaObject::invokeMethod(this, [this]() {
                            emit processing_data_ready({}, true);
                            emit processing_data_ready_topview({}, true);
                            qDebug()<<"结束";
                        }, Qt::QueuedConnection);
                    return;
                }
                auto new_data = data[x];
                auto strength_data = QVector<double>(new_data.size());
                int index=0;
                std::transform(new_data.begin(), new_data.end(),
                               gains_copy.begin(), strength_data.begin(),
                               [&](auto& value1, auto& value2) {
                                   return (value1-m_background[x][index])*m_gains[index++] * value2*m_gain_parameter; // m_gain_parameter ????
                               });
                // ?? Qt ???????????
                QMetaObject::invokeMethod(this, [this, strength_data]() mutable {
                        emit processing_data_ready(strength_data);
                    }, Qt::QueuedConnection);
                auto strength_data_topview = QVector<double>(data.size());
                for (int i = 0; i < data.size(); ++i) {
                    if (i < m_background.size() && m_background[i].size() == gains_use.size()) {
                        strength_data_topview[i] = average_adjusted_range(
                            data[i], m_background[i], gains_use, low, high) * m_gain_parameter;
                    } else {
                        strength_data_topview[i] = average_adjusted_range(
                            data[i], zero_background, gains_use, low, high) * m_gain_parameter;
                    }
                }
                track_average_push(
                    topview_buffer,
                    strength_data_topview,
                    TRACK_AVG_RADIUS,
                    [this](const QVector<double>& column) {
                        emit processing_data_ready_topview(column);
                    });
            }, -1);
        } catch (const std::exception& e) {
            // ????
            qDebug() << "Error in processing thread:" << e.what();
        } catch (...) {
            qDebug() << "Unknown error in processing thread";
        }
    }).detach();
}



void MainWindow::onPlotDoubleClick_topview(int x,int y,QString m_plot_id) {
    if(m_plot_id!="3"){
        return;
    }
    int points=128*std::pow(2,m_settings.value(points_key).toInt());
    m_display_channel=y;
    m_display_point=x;
    m_image2d_plot->clear();
    m_image2d_plot->create(points, m_settings.value(start_time_key).toInt(),m_settings.value(end_time_key).toInt(),-1);
    m_image2d_plot->enable_realtime(true);
    m_image2d_plot_sideview->clear();
    m_image2d_plot_sideview->create(points, m_settings.value(start_time_key).toInt(), m_settings.value(end_time_key).toInt(),m_settings.value(channel_number).toInt());
    m_image2d_plot_sideview->enable_realtime(true);
    // m_image2d_plot_sideview->set_x_axis_range(m_settings.value(channel_number).toInt());
    // ?? gains????????
    QVector<int> gains(points, 1);
    auto count = std::make_shared<std::atomic<int>>(0);
    std::thread([=, gains_copy = gains] () mutable { // ?? gains
        try {
            // 等待缓存完成，可能会抛出异常
            load_cache([=, this] (const QVector<QVector<double>>& data, bool is_eof) { // ???? this
                if (is_eof) {
                    // ?? Qt ???????????
                    QMetaObject::invokeMethod(this, [this]() {
                            emit processing_data_ready({}, true);
                        }, Qt::QueuedConnection);
                    return;
                }

                auto new_data = data[y];
                auto strength_data = QVector<double>(new_data.size());
                int index=0;
                std::transform(new_data.begin(), new_data.end(),
                gains_copy.begin(), strength_data.begin(),
                    [&](auto& value1, auto& value2) {
                        return (value1-m_background[y][index]) *m_gains[index++]* value2*m_gain_parameter; // m_gain_parameter ????
                    });
                // ?? Qt ???????????
                QMetaObject::invokeMethod(this, [this, strength_data]() mutable {
                        emit processing_data_ready(strength_data);
                    }, Qt::QueuedConnection);
                if((*count)==x){
                    // qDebug()<<(*count)<<x;
                    // for(int channel=0;channel<data.size();channel++){
                    //     auto strength_data_sideview = QVector<double>(data[channel].size());
                    //     for(int point=0;point<data[channel].size();point++){
                    //         strength_data_sideview[point]=data[channel][point]*m_gain_parameter;
                    //     }

                    //     emit processing_data_ready_sideview(strength_data_sideview);
                    // }
                    for(int channel = 0; channel <= data.size(); channel++) {
                        if(channel==0){
                            auto strength_data_sideview = QVector<double>(data[channel].size());
                            for(int point = 0; point < data[channel].size(); point++) {
                                strength_data_sideview[point] = (data[channel][point]-m_background[channel][point]) *m_gains[point]* m_gain_parameter;
                            }
                            emit processing_data_ready_sideview(strength_data_sideview);
                        }else if(channel==data.size())
                        {
                            int last_index = data.size() - 1;
                            auto strength_data_sideview = QVector<double>(data[last_index].size());
                            for(int point = 0; point < data[last_index].size(); point++) {
                                strength_data_sideview[point] = (data[last_index][point]-m_background[last_index][point]) *m_gains[point]* m_gain_parameter;
                            }

                            emit processing_data_ready_sideview(strength_data_sideview);
                        }else
                        {
                            auto strength_data_sideview = QVector<double>(data[channel].size());
                            for(int point = 0; point < data[channel].size(); point++) {
                                // ???? channel ???? channel ????
                                strength_data_sideview[point] = (data[channel-1][point]-m_background[channel-1][point] + data[channel][point]-m_background[channel][point]) / 2.0 *m_gains[point]* m_gain_parameter;
                            }
                            emit processing_data_ready_sideview(strength_data_sideview);
                        }
                    }
                }
                (*count)++;
            }, -1);
        } catch (const std::exception& e) {
            // ????
            qDebug() << "Error in processing thread:" << e.what();
        } catch (...) {
            qDebug() << "Unknown error in processing thread";
        }
    }).detach();
}

void MainWindow::on_topview_avg_radius_changed(int)
{   qDebug()<<"??...........................................................";
    if (!m_topview_avg_timer) {
        return;
    }
    if (m_topview_rebuild_in_progress.load()) {
        return;
    }
    m_topview_overlay_enabled = true;
    ++m_topview_avg_task_id;
    m_topview_avg_timer->start();
}

void MainWindow::on_topview_point_range_changed(int lower, int upper)
{
    const int low = std::min(lower, upper);
    const int high = std::max(lower, upper);
    m_topview_range_low.store(low);
    m_topview_range_high.store(high);
    update_range_avg_display();
    if (!m_topview_avg_timer) {
        return;
    }
    if (m_topview_rebuild_in_progress.load()) {
        return;
    }
    m_topview_overlay_enabled = true;
    ++m_topview_avg_task_id;
    m_topview_avg_timer->start();
}

void MainWindow::on_map_k_changed(double value)
{
    if (m_image2d_plot) {
        m_image2d_plot->set_map_params(value);
    }
    if (m_image2d_plot_topview) {
        m_image2d_plot_topview->set_map_params(value);
    }
    if (m_image2d_plot_sideview) {
        m_image2d_plot_sideview->set_map_params(value);
    }
}

void MainWindow::update_range_avg_display()
{
    if (!ui) {
        return;
    }
    const int low = std::min(m_topview_range_low.load(), m_topview_range_high.load());
    const int high = std::max(m_topview_range_low.load(), m_topview_range_high.load());
    double sum = 0.0;
    long long count = 0;
    auto accumulate_trace = [&](const QVector<double>& trace) {
        if (trace.isEmpty()) {
            return;
        }
        const int start = std::max<qsizetype>(0, std::min<qsizetype>(low, trace.size() - 1));
        const int end = std::max<qsizetype>(0, std::min<qsizetype>(high, trace.size() - 1));
        for (int i = start; i <= end; ++i) {
            sum += std::abs(trace[i]);
            ++count;
        }
    };
    if (!m_raw_data_all_channel.isEmpty()) {
        for (const auto& channel : m_raw_data_all_channel) {
            if (channel.isEmpty()) {
                continue;
            }
            accumulate_trace(channel.last());
        }
    } else if (!m_raw_data.isEmpty()) {
        for (const auto& channel : m_raw_data) {
            if (channel.isEmpty()) {
                continue;
            }
            accumulate_trace(channel.last());
        }
    }
    // const double avg = (count > 0) ? (sum / static_cast<double>(count)) : 0.0;
    // ui->range_avg_value_label->setText(QString::number(avg, 'f', 6));
}

void MainWindow::on_topview_avg_radius_released()
{
    if (m_points <= 0) {
        return;
    }
    if (m_topview_rebuild_in_progress.exchange(true)) {
        return;
    }
    m_topview_overlay_enabled = false;
    ++m_topview_avg_task_id;
    if (m_topview_avg_timer) {
        m_topview_avg_timer->stop();
    }
    m_image2d_plot_topview->set_overlay_visible(false);
    m_image2d_plot_topview->set_io_suspended(true);
    m_image2d_plot_topview->set_interactions_enabled(false);
    const int low = std::max(0, m_topview_range_low.load());
    const int high = std::min(m_points - 1, m_topview_range_high.load());
    const auto background_copy = m_background;
    const auto gains_copy = m_gains;
    const double gain_parameter = m_gain_parameter;
    const int points = m_points;
    const int track_radius = TRACK_AVG_RADIUS;

    std::thread([=, this]() {
        const int block_width = 1024;
        const QString plot_id = "3";
        const QString dir_path = QDir::tempPath() + "/GPR/plot" + plot_id;
        QDir().mkpath(dir_path);

        QDir dir(dir_path);
        auto files = dir.entryInfoList(QDir::Files);
        for (const auto& info : files) {
            if (info.fileName().startsWith(QString("block_%1_").arg(plot_id))) {
                QFile::remove(info.absoluteFilePath());
            }
        }

        QVector<double> unit_gains(points, 1.0);
        const QVector<double>& gains_use = gains_copy.isEmpty() ? unit_gains : gains_copy;
        QVector<double> zero_background(points, 0.0);

        QVector<QVector<double>> block_cache;
        block_cache.reserve(block_width);
        QVector<QVector<double>> track_buffer;
        track_buffer.reserve(track_radius * 2 + 2);
        int block_index = 0;
        int total_traces = 0;
        auto append_block = [&](const QVector<double>& column) {
            block_cache.push_back(column);
            ++total_traces;
            if (block_cache.size() >= block_width) {
                const int columns = block_cache.size();
                const int value_size = column.size();
                const int lower_key = block_index * block_width;
                const int upper_key = lower_key + columns;
                const QString filename = QString("%1/block_%2_%3_res1.dat")
                    .arg(dir_path)
                    .arg(plot_id)
                    .arg(block_index);
                const QString temp_filename = filename + ".tmp";
                QFile temp_file(temp_filename);
                if (temp_file.open(QIODevice::WriteOnly)) {
                    QDataStream out(&temp_file);
                    out << columns << value_size << lower_key << upper_key;
                    const int block_size = value_size * sizeof(double);
                    for (int x = 0; x < columns; ++x) {
                        out.writeRawData(reinterpret_cast<const char*>(block_cache[x].data()), block_size);
                    }
                    temp_file.close();
                    QFile::remove(filename);
                    QFile::rename(temp_filename, filename);
                }
                block_cache.clear();
                ++block_index;
            }
        };

        load_cache([=, &block_cache, &block_index, &total_traces, &track_buffer, &append_block](const QVector<QVector<double>>& data, bool is_eof) mutable {
            if (is_eof) {
                return;
            }
            const int channel_count = data.size();
            QVector<double> column(channel_count);
            for (int ch = 0; ch < channel_count; ++ch) {
                if (ch < background_copy.size() && background_copy[ch].size() == gains_use.size()) {
                    column[ch] = average_adjusted_range(
                        data[ch], background_copy[ch], gains_use, low, high) * gain_parameter;
                } else {
                    column[ch] = average_adjusted_range(
                        data[ch], zero_background, gains_use, low, high) * gain_parameter;
                }
            }
            track_average_push(track_buffer, column, track_radius, append_block);
        }, -1);

        track_average_flush(track_buffer, track_radius, append_block);

        if (!block_cache.isEmpty()) {
            const int columns = block_cache.size();
            const int value_size = block_cache[0].size();
            const int lower_key = block_index * block_width;
            const int upper_key = lower_key + columns;
            const QString filename = QString("%1/block_%2_%3_res1.dat")
                .arg(dir_path)
                .arg(plot_id)
                .arg(block_index);
            const QString temp_filename = filename + ".tmp";
            QFile temp_file(temp_filename);
            if (temp_file.open(QIODevice::WriteOnly)) {
                QDataStream out(&temp_file);
                out << columns << value_size << lower_key << upper_key;
                const int block_size = value_size * sizeof(double);
                for (int x = 0; x < columns; ++x) {
                    out.writeRawData(reinterpret_cast<const char*>(block_cache[x].data()), block_size);
                }
                temp_file.close();
                QFile::remove(filename);
                QFile::rename(temp_filename, filename);
            }
        }

        const int max_block_index = block_cache.isEmpty() ? (block_index - 1) : block_index;
        QMetaObject::invokeMethod(this, [this, max_block_index]() {
            m_image2d_plot_topview->set_last_block_index(max_block_index);
            m_image2d_plot_topview->rebuild_downsamples(max_block_index);
            m_image2d_plot_topview->refresh_from_disk();
            m_image2d_plot_topview->set_overlay_visible(false);
            m_image2d_plot_topview->set_io_suspended(false);
            m_image2d_plot_topview->set_interactions_enabled(true);
            m_topview_rebuild_in_progress.store(false);
        }, Qt::QueuedConnection);
    }).detach();
}

void MainWindow::on_topview_range_changed()
{
    if (!m_topview_overlay_enabled) {
        return;
    }
    if (!m_topview_avg_timer) {
        return;
    }
    if (m_topview_rebuild_in_progress.load()) {
        return;
    }
    m_topview_avg_timer->start();
}

void MainWindow::update_topview_avg_preview()
{
    if (!ui || !m_image2d_plot_topview) {
        return;
    }
    if (!m_topview_overlay_enabled) {
        return;
    }
    if (m_topview_rebuild_in_progress.load()) {
        return;
    }
    if (m_points <= 0) {
        return;
    }
    const int low = std::max(0, std::min(m_points - 1, m_topview_range_low.load()));
    const int high = std::max(0, std::min(m_points - 1, m_topview_range_high.load()));

    const auto range = m_image2d_plot_topview->get_x_axis_range();
    int begin = std::max(0, static_cast<int>(std::floor(range.lower)));
    int end = std::max(begin, static_cast<int>(std::floor(range.upper)) + 1);
    if (end <= begin) {
        end = begin + 1;
    }

    const int task_id = ++m_topview_avg_task_id;
    const auto background_copy = m_background;
    const auto gains_copy = m_gains;
    const auto raw_tail = m_raw_data;
    const double gain_parameter = m_gain_parameter;
    const int points = m_points;

    m_image2d_plot_topview->set_overlay_visible(false);

    std::thread([=]() {
        const auto blocks = list_raw_blocks("1");

        int cached_count = 0;
        for (const auto& block : blocks) {
            cached_count = std::max(cached_count, block.y_offset + block.y_size);
        }

        const int tail_count =
            (!raw_tail.isEmpty() && !raw_tail[0].isEmpty())
                ? static_cast<int>(raw_tail[0].size())
                : 0;

        // ?????????????? "cache + tail" ???????
        const bool has_cached_blocks = !blocks.isEmpty() && cached_count > 0;
        const bool use_tail = !has_cached_blocks; // ????????? tail

        const int total_count = has_cached_blocks ? cached_count : tail_count;

        qDebug() << "[topview_avg][task=" << task_id << "]"
                 << "has_cached_blocks=" << has_cached_blocks
                 << "use_tail=" << use_tail
                 << "cached_count=" << cached_count
                 << "tail_count=" << tail_count
                 << "total_count=" << total_count
                 << "begin/end=" << begin << end;

        if (total_count <= 0) {
            return;
        }
        if (begin >= total_count) {
            return;
        }

        const int adjusted_end = std::min(end, total_count);

        int channel_count = 0;
        if (!blocks.isEmpty()) {
            channel_count = blocks[0].x_size;
        } else if (!raw_tail.isEmpty()) {
            channel_count = raw_tail.size();
        } else if (!background_copy.isEmpty()) {
            channel_count = background_copy.size();
        }
        if (channel_count <= 0 || points <= 0) {
            return;
        }

        QVector<double> zero_background(points, 0.0);
        QVector<double> unit_gains(points, 1.0);
        const QVector<double>& gains_use = gains_copy.isEmpty() ? unit_gains : gains_copy;

        QVector<QVector<double>> columns(adjusted_end - begin,
                                         QVector<double>(channel_count, 0.0));
        QVector<char> filled(columns.size(), 0);

        // ? cache ??? cache ?? [begin, adjusted_end)
        if (has_cached_blocks) {
            append_topview_columns_from_blocks(
                blocks,
                begin,
                adjusted_end,
                low,
                high,
                background_copy,
                gains_use,
                zero_background,
                channel_count,
                filled,
                gain_parameter,
                columns);
        }

        // ?? cache ?? tail??????
        if (use_tail && !raw_tail.isEmpty()) {
            const int tail_begin = begin;
            const int tail_end = adjusted_end;

            const int tail_channel_count = raw_tail.size();
            for (int t = tail_begin; t < tail_end; ++t) {
                QVector<double> column(tail_channel_count);
                for (int ch = 0; ch < tail_channel_count; ++ch) {
                    if (t >= raw_tail[ch].size()) {
                        column[ch] = 0.0;
                        continue;
                    }
                    const auto& channel = raw_tail[ch][t];
                    if (ch < background_copy.size() && background_copy[ch].size() == gains_use.size()) {
                        column[ch] = average_adjusted_range(
                                         channel, background_copy[ch], gains_use, low, high) * gain_parameter;
                    } else {
                        column[ch] = average_adjusted_range(
                                         channel, zero_background, gains_use, low, high) * gain_parameter;
                    }
                }
                const int out_index = t - begin;
                if (out_index >= 0 && out_index < columns.size()) {
                    columns[out_index] = column;
                    filled[out_index] = 1;
                }
            }
        }

        if (columns.isEmpty()) {
            return;
        }

        // ????????????????????????? 0 ????? return
        for (int i = 0; i < filled.size(); ++i) {
            if (!filled[i]) {
                return;
            }
        }

        QVector<QVector<double>> averaged_columns;
        averaged_columns.reserve(columns.size());
        QVector<QVector<double>> track_buffer;
        track_buffer.reserve(TRACK_AVG_RADIUS * 2 + 2);
        for (const auto& column : columns) {
            track_average_push(
                track_buffer,
                column,
                TRACK_AVG_RADIUS,
                [&](const QVector<double>& out_column) {
                    averaged_columns.push_back(out_column);
                });
        }
        track_average_flush(
            track_buffer,
            TRACK_AVG_RADIUS,
            [&](const QVector<double>& out_column) {
                averaged_columns.push_back(out_column);
            });

        QMetaObject::invokeMethod(this, [this, averaged_columns, begin, task_id]() mutable {
                if (task_id != m_topview_avg_task_id.load()) {
                    return;
                }
                if (m_topview_rebuild_in_progress.load() || !m_topview_overlay_enabled) {
                    return;
                }
                m_image2d_plot_topview->setDataInstant(averaged_columns, begin);
            }, Qt::QueuedConnection);
    }).detach();
}



void MainWindow::on_eliminate_background(bool is_enabled) {
    // if(/*m_real_time_acquisition->is_playing() ||*/ m_file_replay_view->is_replaying() ||
    //     m_previous_frame.isEmpty()) {
    //     return;
    // }
    m_is_eliminate_background = is_enabled;
    if (ui && ui->eliminate_background_combo_box) {
        const QSignalBlocker blocker(ui->eliminate_background_combo_box);
        ui->eliminate_background_combo_box->setCurrentIndex(is_enabled ? 0 : 1);
    }
    m_real_time_processing_setting_view->set_eliminate_background(is_enabled);
    m_real_time_processing_setting_view->enable_eliminate_background_button(false);
    const int rebuild_task_id = ++m_processing_rebuild_task_id;
    // m_real_time_processing_setting_view->enable_adjust_gain_button(true);
    m_background.clear();
    m_background.resize(m_channel_number, QVector<double>(m_points));
    m_mean_background.clear();
    m_mean_background.resize(m_channel_number, QVector<double>(m_points, 0.0));
    m_mean_background_count = 0;
    reset_mine_ground_trend_flatten();
    reset_v2_hyperbola_history();
    if (!m_is_display_data) {
        const bool use_mean_background = m_is_mean_background;
        const bool use_trend_flatten = m_is_mine_ground_trend_flatten_enabled;
        const bool use_v2_hyperbola = m_is_v2_hyperbola_detector_enabled;
        const double v2_dt_ns = current_dt_ns();
        const int display_channel = m_display_channel;
        const int low = std::max(0, std::min(m_points - 1, m_topview_range_low.load()));
        const int high = std::max(0, std::min(m_points - 1, m_topview_range_high.load()));
        const QVector<double> gains_copy = m_gains;
        const double gain_parameter = m_gain_parameter;
        const int topview_radius = TRACK_AVG_RADIUS;
        std::thread([=, this]() {
            QVector<QVector<QVector<double>>> all_data;
            load_cache_all([&](QVector<QVector<QVector<double>>>& loaded_data) {
                all_data = loaded_data;
            });
            if (rebuild_task_id != m_processing_rebuild_task_id.load()) {
                return;
            }
            if (all_data.isEmpty() || all_data[0].isEmpty() || all_data[0][0].isEmpty()) {
                QMetaObject::invokeMethod(this, [this, rebuild_task_id]() {
                    if (rebuild_task_id != m_processing_rebuild_task_id.load()) {
                        return;
                    }
                    m_real_time_processing_setting_view->enable_eliminate_background_button(true);
                }, Qt::QueuedConnection);
                return;
            }

            const int channel_count = all_data.size();
            const int trace_count = all_data[0].size();
            const int points = all_data[0][0].size();

            QVector<QVector<double>> background(channel_count, QVector<double>(points, 0.0));
            if (is_enabled && trace_count >= CALCULATE_BACKGROUND_FRAMES) {
                for (int ch = 0; ch < channel_count; ++ch) {
                    for (int t = 0; t < CALCULATE_BACKGROUND_FRAMES && t < all_data[ch].size(); ++t) {
                        for (int p = 0; p < points && p < all_data[ch][t].size(); ++p) {
                            background[ch][p] += all_data[ch][t][p] / CALCULATE_BACKGROUND_FRAMES;
                        }
                    }
                }
            }

            QVector<QVector<double>> mean_background(channel_count, QVector<double>(points, 0.0));
            int mean_count = 0;
            if (use_mean_background) {
                for (int t = 0; t < trace_count; ++t) {
                    for (int ch = 0; ch < channel_count; ++ch) {
                        if (t >= all_data[ch].size()) {
                            continue;
                        }
                        for (int p = 0; p < points && p < all_data[ch][t].size(); ++p) {
                            mean_background[ch][p] += all_data[ch][t][p] - background[ch][p];
                        }
                    }
                    ++mean_count;
                }
                if (mean_count > 0) {
                    for (auto& channel : mean_background) {
                        for (auto& value : channel) {
                            value /= mean_count;
                        }
                    }
                }
            }

            QVector<double> unit_gains(points, 1.0);
            const QVector<double>& gains_use =
                (gains_copy.size() == points) ? gains_copy : unit_gains;
            QVector<QVector<double>> display_line;
            display_line.reserve(trace_count);
            const int safe_display_channel = std::max(0, std::min(display_channel, channel_count - 1));
            QVector<MineGroundTrendFlattenState> display_flatten_states;
            for (int t = 0; t < trace_count && t < all_data[safe_display_channel].size(); ++t) {
                QVector<double> trace = all_data[safe_display_channel][t];
                for (int p = 0; p < trace.size(); ++p) {
                    const double mean_value = use_mean_background ? mean_background[safe_display_channel][p] : 0.0;
                    trace[p] -= background[safe_display_channel][p] + mean_value;
                }
                if (use_trend_flatten) {
                    trace = apply_mine_ground_trend_flatten_to_trace(
                        safe_display_channel, trace, &display_flatten_states);
                }
                display_line.append(trace);
            }
            if (use_v2_hyperbola) {
                display_line = apply_v2_hyperbola_detector(
                    display_line,
                    v2_dt_ns > 0.0 ? v2_dt_ns : 1.0,
                    default_v2_hyperbola_params());
            }
            QVector<QVector<double>> bscan;
            bscan.reserve(display_line.size());
            for (auto trace : display_line) {
                for (int p = 0; p < trace.size(); ++p) {
                    const double gain_value = (p < gains_use.size()) ? gains_use[p] : 1.0;
                    trace[p] = trace[p] * gain_value * gain_parameter;
                }
                bscan.append(std::move(trace));
            }

            const QVector<double> zero_for_topview(points, 0.0);
            QVector<QVector<double>> topview_buffer;
            QVector<QVector<double>> topview_avg_buffer;
            topview_buffer.reserve(trace_count);
            QVector<MineGroundTrendFlattenState> topview_flatten_states;
            for (int t = 0; t < trace_count; ++t) {
                QVector<double> column(channel_count, 0.0);
                for (int ch = 0; ch < channel_count; ++ch) {
                    if (t >= all_data[ch].size()) {
                        continue;
                    }
                    QVector<double> topview_trace = all_data[ch][t];
                    for (int p = 0; p < topview_trace.size(); ++p) {
                        const double bg = (p < background[ch].size()) ? background[ch][p] : 0.0;
                        const double mean = (use_mean_background && p < mean_background[ch].size())
                            ? mean_background[ch][p]
                            : 0.0;
                        topview_trace[p] -= bg + mean;
                    }
                    if (use_trend_flatten) {
                        topview_trace = apply_mine_ground_trend_flatten_to_trace(
                            ch, topview_trace, &topview_flatten_states);
                    }
                    column[ch] = average_adjusted_range(
                        topview_trace, zero_for_topview, gains_use, low, high) * gain_parameter;
                }
                track_average_push(
                    topview_avg_buffer,
                    column,
                    topview_radius,
                    [&topview_buffer](const QVector<double>& averaged_column) {
                        topview_buffer.append(averaged_column);
                    });
            }
            track_average_flush(
                topview_avg_buffer,
                topview_radius,
                [&topview_buffer](const QVector<double>& averaged_column) {
                    topview_buffer.append(averaged_column);
                });

            QMetaObject::invokeMethod(this, [=, this]() {
                if (rebuild_task_id != m_processing_rebuild_task_id.load()) {
                    return;
                }
                m_background = background;
                m_mean_background = mean_background;
                m_mean_background_count = mean_count;
                m_channel_number = channel_count;
                m_points = points;
                m_track_number = trace_count;
                m_image2d_plot->clear();
                m_image2d_plot->create(points, m_settings.value(start_time_key).toInt(),
                                       m_settings.value(end_time_key).toInt(), -1);
                m_image2d_plot->enable_realtime(false);
                m_image2d_plot->set_x_axis_range(std::max(1, trace_count));
                m_image2d_plot->setDataInstant(bscan);
                m_image2d_plot_topview->clear();
                m_image2d_plot_topview->create(channel_count, 0, channel_count, -1);
                m_image2d_plot_topview->enable_realtime(false);
                m_image2d_plot_topview->set_x_axis_range(std::max(1, static_cast<int>(topview_buffer.size())));
                m_image2d_plot_topview->setDataInstant(topview_buffer);
                if (m_track_number_label) {
                    m_track_number_label->setText(QString("%1").arg(m_track_number));
                }
                m_real_time_processing_setting_view->enable_eliminate_background_button(true);
                update_range_avg_display();
            }, Qt::QueuedConnection);
        }).detach();
        return;
    }
    if(is_enabled) {
        if(m_track_number < CALCULATE_BACKGROUND_FRAMES) {
            m_is_eliminate_background = false;
            if (ui && ui->eliminate_background_combo_box) {
                const QSignalBlocker blocker(ui->eliminate_background_combo_box);
                ui->eliminate_background_combo_box->setCurrentIndex(1);
            }
            m_real_time_processing_setting_view->set_eliminate_background(false);
            QMessageBox::warning(this, tr("GPR"), tr("Please collect at least 50 frames of data."), QMessageBox::Ok);
            return;
        }
        load_cache([&] (const QVector<QVector<double>>& data, bool is_eof) {
            if(is_eof) {
                return;
            }
            const auto& processed_data = data;
            for(auto i = 0; i < processed_data.size() && i < m_background.size(); ++i) {
                for(auto j=0;j<processed_data[i].size() && j < m_background[i].size();++j){
                    m_background[i][j] += processed_data[i][j] / CALCULATE_BACKGROUND_FRAMES;
                }
            }
        }, CALCULATE_BACKGROUND_FRAMES);
    }
    if(m_is_mean_background) {
        int mean_count = 0;
        load_cache([&] (const QVector<QVector<double>>& data, bool is_eof) {
            if(is_eof) {
                return;
            }
            const auto& processed_data = data;
            for(auto i = 0; i < processed_data.size() && i < m_mean_background.size(); ++i) {
                for(auto j = 0; j < processed_data[i].size() && j < m_mean_background[i].size(); ++j) {
                    const double background_value =
                        (i < m_background.size() && j < m_background[i].size()) ? m_background[i][j] : 0.0;
                    m_mean_background[i][j] += processed_data[i][j] - background_value;
                }
            }
            ++mean_count;
        }, -1);
        if(mean_count > 0) {
            for(auto& channel : m_mean_background) {
                for(auto& value : channel) {
                    value /= mean_count;
                }
            }
            m_mean_background_count = mean_count;
        }
    }
    m_image2d_plot->clear();
    m_image2d_plot->create(m_points,m_settings.value(start_time_key).toInt(), m_settings.value(end_time_key).toInt(),-1);
    // m_image2d_plot->set_x_axis_range(1024);
    m_image2d_plot->enable_realtime(true);
    if (m_is_mine_ground_trend_flatten_enabled) {
        reset_mine_ground_trend_flatten();
    }
    std::thread([=, this] () {
        load_cache([=, this] (const QVector<QVector<double>>& data, bool is_eof) {
            if(is_eof) {
                emit processing_data_ready({}, true);
                return;
            }
            const auto& processed_data = data;
            if (m_display_channel < 0 || m_display_channel >= processed_data.size()) {
                return;
            }
            auto new_data = processed_data[m_display_channel];
            for(auto i = 0; i < new_data.size(); ++i) {
                const double mean_value = (m_is_mean_background && m_display_channel < m_mean_background.size())
                    ? mean_background_value(m_mean_background[m_display_channel], i)
                    : 0.0;
                const double background_value =
                    (m_display_channel < m_background.size() && i < m_background[m_display_channel].size())
                        ? m_background[m_display_channel][i]
                        : 0.0;
                new_data[i] = new_data[i] - background_value - mean_value;
            }
            if (m_is_mine_ground_trend_flatten_enabled) {
                new_data = apply_mine_ground_trend_flatten_to_trace(m_display_channel, new_data);
            }
            for(auto i = 0; i < new_data.size(); ++i) {
                const double gain_value = (i < m_gains.size()) ? m_gains[i] : 1.0;
                new_data[i] = new_data[i] * gain_value * m_gain_parameter;
            }
            emit processing_data_ready(new_data);
        }, -1);
    }).detach();



    // if(is_enabled) {
    //     std::thread([this]() {

    //         QVector<QVector<double>> frame_data; // [T][C]

    //         load_cache_all([&](auto& all_data) {

    //             const int C = all_data.size();
    //             const int T = all_data[0].size();

    //             QVector<QVector<double>> topview(C, QVector<double>(T));
    //             for (int c = 0; c < C; ++c)
    //                 for (int t = 0; t < T; ++t)
    //                     topview[c][t] = all_data[c][t][m_display_point];

    //             // auto topview_fk = f_k_real(topview, 0.05, 30.0, 0.05);
    //             // auto topview_fk=spatialGradientSobel(topview);
    //             auto step1 = removeBackgroundMean(topview,21,41);
    //             // ??
    //             auto step2 = spatialMedianFilter(step1, 3, 5);
    //             qDebug() << "f_k完成";
    //             frame_data.resize(T);
    //             for (int t = 0; t < T; ++t)
    //             {
    //                 frame_data[t].resize(C);
    //                 for (int c = 0; c < C; ++c)
    //                 {
    //                     frame_data[t][c] =
    //                         (step2[c][t] - m_background[c][m_display_point]) *
    //                         m_gains[m_display_point] *
    //                         m_gain_parameter;
    //                 }
    //             }
    //         });

    //         // === ????????? UI ?? ===
    //         QMetaObject::invokeMethod(
    //             this,
    //             [this, frame_data]() {
    //                 qDebug()<<frame_data.size();
    //                 qDebug()<<frame_data[0].size();
    //                 updateTopViewFrame(frame_data);
    //             },
    //             Qt::QueuedConnection
    //             );

    //     }).detach();

    // }
}

void MainWindow::updateTopViewFrame
    ( const QVector<QVector<double>>& frame)
{
    m_image2d_plot_topview->clear();
    m_image2d_plot_topview->create(m_settings.value(channel_number).toInt(), 0, m_settings.value(channel_number).toInt(),-1);
    m_image2d_plot_topview->enable_realtime(true);
    for (const auto & strength_data : frame)
    {
        emit processing_data_ready_topview(strength_data) ;
    }
}

void MainWindow::on_adjust_gain() {
    if(m_track_number < 50) {
        QMessageBox::warning(this, tr("GPR"), tr("Please collect at least 50 frames of data."), QMessageBox::Ok);
        return;
    }
    if(!m_adjust_gain_setting_dialog) {
        auto strength_data = m_previous_frame;
        if(m_real_time_processing_setting_view->is_background_elimination_enabled()) {
            std::transform(strength_data.begin(), strength_data.end(),
                           m_background[m_display_channel].begin(), strength_data.begin(), [&] (auto& value1, auto& value2) {
                               return value1 - value2;
                           });
        }
        if(m_is_mean_background) {
            const QVector<double> mean_trace =
                (m_display_channel < m_mean_background.size())
                    ? m_mean_background[m_display_channel]
                    : QVector<double>();
            for(int i = 0; i < strength_data.size(); ++i) {
                strength_data[i] -= mean_background_value(mean_trace, i);
            }
        }
        m_adjust_gain_setting_dialog = new AdjustGainSettingDialog(
            strength_data, m_gains, m_gain_parameter,
            m_settings.value(start_time_key).toDouble(),
            m_settings.value(end_time_key).toDouble(), this);
        m_adjust_gain_setting_dialog->setModal(true);
        m_adjust_gain_setting_dialog->setAttribute(Qt::WA_DeleteOnClose);
        m_adjust_gain_setting_dialog->setWindowFlag(Qt::WindowStaysOnTopHint);
        connect(m_adjust_gain_setting_dialog, &AdjustGainSettingDialog::submit_gains,
                this, &MainWindow::on_submit_gains);
        connect(m_adjust_gain_setting_dialog, &AdjustGainSettingDialog::destroyed, [=] {
            m_adjust_gain_setting_dialog = nullptr;
        });
        m_adjust_gain_setting_dialog->show();
    }
}


void MainWindow::on_submit_gains(const QVector<double>& gains) {
    m_gains = gains;
    if(!m_is_display_data) {
        on_eliminate_background(m_is_eliminate_background);
    }
}

// ? MainWindow ?????????????????

void MainWindow::createAndSetVolumeData()
{
    // qDebug() << "??????3D???...";
    // for(int channel=0;channel<m_raw_data_all_channel.size();channel++){
    //     for(int trace=0;trace<m_raw_data_all_channel[channel].size();trace++){
    //         for(int point=0;point<m_raw_data_all_channel[channel][trace].size();point++){
    //             m_raw_data_all_channel[channel][trace][point]=normalize_value((m_raw_data_all_channel[channel][trace][point]-m_background[channel][point])*5/511.4);

    //             // qDebug() << m_raw_data_all_channel[channel][trace][point];
    //         }
    //     }
    // }
    // // 1. ???????
    // // 4. ?????3D??

    // if (m_Three_D_Volume_view) {
    //     m_Three_D_Volume_view->setData(m_raw_data_all_channel);
    //     qDebug() << "????????3D??";
    // } else {
    //     qDebug() << "???3D??????";
    // }
}







#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>

void MainWindow::exportDataToCSV()
{
    // ???????
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "保存数据到CSV",
                                                    QString("data_%1.csv").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")),
                                                    "CSV Files (*.csv)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"), tr("Unable to create file."));
        return;
    }

    QTextStream out(&file);
    // out.setCodec("UTF-8"); // 设置UTF-8编码支持中文

    // ???????
    out << "# ??????: " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "\n";
    out << "# 通道数量: " << m_raw_data_all_channel.size() << "\n";

    int totalTraces = 0;
    for(int channel = 0; channel < m_raw_data_all_channel.size(); channel++) {
        totalTraces += m_raw_data_all_channel[channel].size();
    }
    out << "# 总轨迹数: " << totalTraces << "\n";
    out << "??,??,???,????\n";

    int totalRows = 0;

    // ???????
    for(int channel = 0; channel < m_raw_data_all_channel.size(); channel++) {
        for(int trace = 0; trace < m_raw_data_all_channel[channel].size(); trace++) {
            for(int point = 0; point < m_raw_data_all_channel[channel][trace].size(); point++) {
                // ?????????
                double originalValue = m_raw_data_all_channel[channel][trace][point];
                double normalizedValue = normalize_value(originalValue);

                // 更新数据
                m_raw_data_all_channel[channel][trace][point] = normalizedValue;

                // ?? CSV ?
                out << channel << ","
                    << trace << ","
                    << point << ","
                    // << QString::number(originalValue, 'f', 6) << ","
                    << QString::number(normalizedValue, 'f', 6) << "\n";

                totalRows++;

                // ? 1000 ???????
                if (totalRows % 1000 == 0) {
                    qDebug() << "Exported" << totalRows << "rows";
                }
            }
        }
    }

    file.close();

    QMessageBox::information(this, tr("Export complete"),
                             QString("Data exported to:\n%1\n\nTotal rows: %2")
                                 .arg(fileName)
                                 .arg(totalRows));
}


void MainWindow::on_scale_option_update(ScaleOption option) {
    if(option == ScaleOption::X_AXIS) {
        m_image2d_plot_topview->set_scale_axes(true, false);
    } else if(option == ScaleOption::Y_AXIS) {
        m_image2d_plot_topview->set_scale_axes(false, true);
    } else if(option == ScaleOption::XY_AXIS) {
        m_image2d_plot_topview->set_scale_axes(true, true);
    }
}

void MainWindow::on_set_volume_data(){
    load_cache_all([&](auto& all_data) {
        qDebug() << "total rows:" << all_data.size();
        if(all_data.size()==0){
            return;
        }
            for(int channel=0;channel<all_data.size();channel++){
                for(int trace=0;trace<all_data[channel].size();trace++){
                    for(int point=0;point<all_data[channel][trace].size();point++){
                        // all_data[point]=f_k_real(all_data[0],0.05,60,0.05);
                        all_data[channel][trace][point]=normalize_value((all_data[channel][trace][point]/*-m_background[channel][point]*/)*m_gain_parameter*m_gains[point]);
                    }
                }
            }
        // const int C = all_data.size();              // channel ?
        // if (C == 0) return;

        // const int T = all_data[0].size();           // trace ?
        // if (T == 0) return;

        // const int P = all_data[0][0].size();        // point ?
        // if (P == 0) return;

        // // ===============================
        // // 2. f-k ???????????
        // // ===============================
        // const double dear_x = 0.05;   // ?????? (m)
        // const double Lg     = 60.0;   // 低频背景尺度 (m)
        // const double Ls     = 0.05;   // 高频杂波尺度 (m)

        // // ===============================
        // // 3. ???? point?????? f-k
        // // ===============================
        // for (int point = 0; point < P; ++point)
        // {
        //     // ---- 3.1 ??? point ???? (channel x trace) ----
        //     QVector<QVector<double>> topview(C, QVector<double>(T));

        //     for (int channel = 0; channel < C; ++channel)
        //     {
        //         for (int trace = 0; trace < T; ++trace)
        //         {
        //             topview[channel][trace] =
        //                 all_data[channel][trace][point];
        //         }
        //     }

        //     // ---- 3.2 f-k ????????? 2D ?? ----
        //     QVector<QVector<double>> topview_fk =
        //         f_k_real(topview, dear_x, Lg, Ls);

        //     // ---- 3.3 ?? + ?????/???? ----
        //     for (int channel = 0; channel < C; ++channel)
        //     {
        //         for (int trace = 0; trace < T; ++trace)
        //         {
        //             double v = topview_fk[channel][trace];

        //             // ????????????
        //             v = (v - m_background[channel][point])
        //                 * m_gain_parameter
        //                 * m_gains[point];

        //             all_data[channel][trace][point] =
        //                 normalize_value(v);
        //         }
        //     }
        // }
            m_three_d_volume_dialog = new Three_D_Volume_Dialog();
            // m_three_d_volume_dialog->create_and_set_volume_data(std::move(all_data));

            QMetaObject::invokeMethod(
                this,
                [this, all_data]() {
                    m_three_d_volume_dialog->create_and_set_volume_data(all_data);
                },
                Qt::QueuedConnection
                );
            m_three_d_volume_dialog->setAttribute(Qt::WA_DeleteOnClose);
            connect(m_three_d_volume_dialog, &Three_D_Volume_Dialog::destroyed, [=, this] {
                m_three_d_volume_dialog = nullptr;});
            m_three_d_volume_dialog->show();
        // }
    });
}


void MainWindow::on_auto_adjust_gain_combo_box_currentIndexChanged(int index)
{   if(index){
        std::fill(m_gains.begin(), m_gains.end(), 1.0);
    }else{
        if(m_track_number<CALCULATE_BACKGROUND_FRAMES){
            QMessageBox::warning(this, tr("GPR"), tr("Please collect at least 50 frames of data."), QMessageBox::Ok);
            ui->auto_adjust_gain_combo_box->setCurrentIndex(1);
            return;
        }else{
            rms_gain(m_base_data[0]);
        }
    }
    if(!m_is_display_data){
        on_submit_gains(m_gains);
    }
}

void MainWindow::on_mean_background_combo_box_currentIndexChanged(int index)
{
    m_is_mean_background = (index == 0);
    m_mean_background.clear();
    m_mean_background.resize(m_channel_number, QVector<double>(m_points, 0.0));
    m_mean_background_count = 0;
    if(!m_is_display_data) {
        on_eliminate_background(m_is_eliminate_background);
    }
}

void MainWindow::finish_replay_loading()
{
    const bool rebuild_replay_with_processing =
        m_is_eliminate_background ||
        m_is_mean_background ||
        m_is_mine_ground_trend_flatten_enabled ||
        m_is_v2_hyperbola_detector_enabled;
    if (rebuild_replay_with_processing) {
        m_is_replay_loading = false;
        m_is_display_data = false;
        m_replay_bscan_buffer.clear();
        m_replay_topview_buffer.clear();
        m_replay_v2_channel_buffer.clear();
        m_topview_track_buffer.clear();
        if (m_replay_progress_bar) {
            m_replay_progress_bar->setValue(m_replay_progress_bar->maximum());
        }
        if (m_track_number_label) {
            m_track_number_label->setText(QString("%1").arg(m_track_number));
        }
        set_time_transform_selector_enabled(true);
        if (m_process_btn) {
            m_process_btn->setEnabled(true);
        }
        on_eliminate_background(m_is_eliminate_background);
        return;
    }

    if (m_is_v2_hyperbola_detector_enabled && !m_replay_v2_channel_buffer.isEmpty()) {
        const double dt_ns = current_dt_ns();
        const V2HyperbolaParams params = default_v2_hyperbola_params();
        m_replay_bscan_buffer.clear();
        const int channel_count = m_replay_v2_channel_buffer.size();
        int trace_count = 0;
        if (channel_count > 0) {
            trace_count = m_replay_v2_channel_buffer[0].size();
        }
        const int display_channel = std::max(0, std::min(m_display_channel, channel_count - 1));
        QVector<QVector<double>> display_line;
        if (display_channel < channel_count) {
            display_line = apply_v2_hyperbola_detector(
                m_replay_v2_channel_buffer[display_channel],
                dt_ns,
                params);
        }
        m_replay_bscan_buffer.reserve(trace_count);
        for (int t = 0; t < trace_count && t < display_line.size(); ++t) {
            m_replay_bscan_buffer.append(build_bscan_trace(display_line[t]));
        }
    }

    track_average_flush(
        m_topview_track_buffer,
        TRACK_AVG_RADIUS,
        [this](const QVector<double>& column) {
            m_replay_topview_buffer.append(column);
        });

    if (!m_replay_bscan_buffer.isEmpty()) {
        m_image2d_plot->enable_realtime(false);
        m_image2d_plot->set_x_axis_range(m_replay_bscan_buffer.size());
        m_image2d_plot->setDataInstant(m_replay_bscan_buffer);
    } else {
        m_image2d_plot->enable_realtime(false);
    }

    if (!m_replay_topview_buffer.isEmpty()) {
        m_image2d_plot_topview->enable_realtime(false);
        m_image2d_plot_topview->set_x_axis_range(m_replay_topview_buffer.size());
        m_image2d_plot_topview->setDataInstant(m_replay_topview_buffer);
    } else {
        m_image2d_plot_topview->enable_realtime(false);
    }

    m_image2d_plot_sideview->enable_realtime(false);
    m_real_time_processing_setting_view->enable_eliminate_background_button(true);
    set_time_transform_selector_enabled(true);
    if (m_process_btn) {
        m_process_btn->setEnabled(true);
    }
    if (m_replay_progress_bar) {
        m_replay_progress_bar->setValue(m_replay_progress_bar->maximum());
    }
    if (m_track_number_label) {
        m_track_number_label->setText(QString("%1").arg(m_track_number));
    }
    m_is_display_data = false;
    m_is_replay_loading = false;
    update_range_avg_display();
}

void MainWindow::set_time_transform_selector_enabled(bool enabled)
{
    m_real_time_processing_setting_view->enable_time_transform_selector(enabled);
    if(auto combo = findChild<QComboBox*>("main_time_domain_transform_combo")) {
        combo->setEnabled(enabled);
    }
}

void MainWindow::update_linear_gains()
{
    if (m_points <= 0) {
        m_gains.clear();
        return;
    }
    const double time_span_ns =
        m_settings.value(end_time_key).toDouble() - m_settings.value(start_time_key).toDouble();
    const double dt_ns = (m_points > 1 && time_span_ns > 0.0)
        ? time_span_ns / static_cast<double>(m_points - 1)
        : 0.0;
    m_gains.resize(m_points);
    for (int i = 0; i < m_points; ++i) {
        const double gain = m_linear_gain_initial + m_linear_gain_slope_per_ns * i * dt_ns;
        m_gains[i] = std::max(0.0, std::min(500.0, std::isfinite(gain) ? gain : m_linear_gain_initial));
    }
}

void MainWindow::on_linear_gain_changed(double)
{
    if (auto spin = findChild<QDoubleSpinBox*>("linear_gain_initial_spin")) {
        m_linear_gain_initial = spin->value();
    }
    if (auto spin = findChild<QDoubleSpinBox*>("linear_gain_slope_spin")) {
        m_linear_gain_slope_per_ns = spin->value();
    }
    update_linear_gains();
    if (!m_is_display_data) {
        on_eliminate_background(m_is_eliminate_background);
    }
}

void MainWindow::rms_gain(const QVector<double>& data)
{
    auto gain_rms = QVector<double>(data.size());
    auto half_window_length = 32;
    auto sum = 0.0;
    auto update_rms = [&] (double rms, int index) {
        if(rms <= 0.0) {
            gain_rms[index] = 0;
        } else {
            gain_rms[index] = data[index] / std::sqrt(rms);
        }
    };
    for(int i = 0; i < half_window_length; ++i) {
        sum += data[i] * data[i];
    }
    auto nwin = 2 * half_window_length + 1;
    auto rms = sum / nwin;
    for(int i = 0; i < half_window_length; ++i) {
        sum += data[i + half_window_length] * data[i + half_window_length];
        ++nwin;
        rms = sum / nwin;
        update_rms(rms, i);
    }
    for(int i = half_window_length; i < data.size() - half_window_length; ++i) {
        sum += data[i + half_window_length] * data[i + half_window_length];
        sum -= data[i - half_window_length] * data[i - half_window_length];
        rms = sum / nwin;
        update_rms(rms, i);
    }
    for(int i = data.size() - half_window_length; i < data.size(); ++i) {
        sum -= data[i - half_window_length] * data[i - half_window_length];
        nwin--;
        rms = sum / nwin;
        update_rms(rms, i);
    }
    auto gains = QVector<double>(data.size());
    for(auto i = 0; i < data.size(); ++i) {
        gains[i] = gain_rms[i] * 0.4 / data[i]/m_gain_parameter;
    }
    m_gains=gains;
}






void MainWindow::on_main_axis_rect_changed(int top, int bottom, int height)
{
    if (!m_topview_range_container || !m_topview_range_slider) {
        return;
    }
    if (m_topview_range_top_spacer) {
        m_topview_range_top_spacer->changeSize(0, top, QSizePolicy::Minimum, QSizePolicy::Fixed);
    }
    if (m_topview_range_bottom_spacer) {
        m_topview_range_bottom_spacer->changeSize(0, bottom, QSizePolicy::Minimum, QSizePolicy::Fixed);
    }
    if (height > 0) {
        m_topview_range_slider->setFixedHeight(height);
    }
    if (m_topview_range_container->layout()) {
        m_topview_range_container->layout()->invalidate();
    }
}

void MainWindow::alignAngleSliderWithYAxis()
{
    if (!m_image2d_plot) {
        return;
    }
    int top = 0;
    int bottom = 0;
    int height = 0;
    m_image2d_plot->axisRectMetrics(top, bottom, height);
    alignAngleSliderWithYAxis(top, bottom, height);
}

void MainWindow::alignAngleSliderWithYAxis(int top, int bottom, int height)
{
    if (!m_topview_range_container || !m_topview_range_slider) {
        return;
    }

    top = std::max(0, top);
    bottom = std::max(0, bottom);

    if (m_topview_range_top_spacer) {
        m_topview_range_top_spacer->changeSize(0, top, QSizePolicy::Minimum, QSizePolicy::Fixed);
    }
    if (m_topview_range_bottom_spacer) {
        m_topview_range_bottom_spacer->changeSize(0, bottom, QSizePolicy::Minimum, QSizePolicy::Fixed);
    }

    if (height > 0) {
        m_topview_range_slider->setFixedHeight(height);
    }

    if (auto l = m_topview_range_container->layout()) {
        l->invalidate();
        l->activate();
    }
}




