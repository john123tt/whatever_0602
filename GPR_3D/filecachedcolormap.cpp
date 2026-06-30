#include "filecachedcolormap.h"
#include <algorithm>
#include <atomic>
#include <cmath>
#include <QSet>

/** All available resolutions. */
const auto AVAILABLE_RESOLUTIONS = std::vector<int>({1, 2, 3, 4, 6, 10, 16});
/** The path used for storing temporary files. */
const auto DIR_PATH = QDir::tempPath() + "/GPR/plot";
std::atomic<double> g_map_k{1.0};
constexpr int kRealtimeRefreshIntervalMs = 16;
constexpr int kRealtimeCleanupScrollUpdates = 60;

// QString make_block_filename(int index, int res) {
//   return QString("block_%1_%2_res%3.dat").arg(m_plotId).arg(index).arg(res);
// }

// auto normalize_value(double value) {
//   if(value > 1.0) {
//     return 1.0;
//   } if(value < -1.0) {
//     return 0.0;
//   }
//   return (value + 1.0) / 2.0;
// }
double map_value(double value, double k)
{
    const double span = std::max(k, 1e-6);
    if (value <= 0.0) {
        return 0.5;
    }
    if (value < span) {
        return 0.5 - (value / span) * 0.5;
    }
    return 0.0;
}

double map_signed_value(double value, double k)
{
    const double span = std::max(k, 1e-6);
    if (value >= span) {
        return 1.0;
    }
    if (value <= -span) {
        return 0.0;
    }
    return (value + span) / (2.0 * span);
}

double normalized_map_k()
{
    double k = g_map_k.load();
    if (!std::isfinite(k) || k <= 1e-9) {
        k = 1.0;
    }
    return k;
}

auto normalize_value(double value, double k, bool is_signed_plot)
{
    if(is_signed_plot){
        return map_signed_value(value, k);
    }else{
        value = std::abs(value);
        return map_value(value,k);
    }
}


FileCachedColorMap::FileCachedColorMap(QCPAxis& key_axis, QCPAxis& value_axis,
    int block_width,const QString& plotId, QObject* parent)
    : QObject(parent),
      m_plotId(plotId),
      m_key_axis(&key_axis),
      m_value_axis(&value_axis),
      m_block_width(block_width),
      // m_plot(m_key_axis->parentPlot()),
      m_current_color_map(nullptr),
      m_overlay_map(nullptr),
      m_is_realtime_enabled(true),
      m_is_instant_data_enabled(false),
      m_io_suspended(false),
      m_is_signed_plot(plotId == QStringLiteral("1")),
      m_value_size(0),
      m_last_block_index(-1),
      m_absolute_track_count(0),
      m_current_resolution(1),
      m_dir_path(DIR_PATH+plotId)
{
  QDir().mkpath(m_dir_path);
    if (m_key_axis && m_key_axis->parentPlot()) {
        m_plot = m_key_axis->parentPlot();
    } else {
        // 处理错误情况，例如抛出异常或记录错误日志
        qWarning() << "Key axis does not have a parent QCustomPlot";
        // 或者使用其他适当的错误处理方式
    }
    m_color_stops={{0.0, Qt::black}, {1.0, Qt::white}};
  connect(m_key_axis, qOverload<const QCPRange&>(&QCPAxis::rangeChanged), 
    std::bind_front(&FileCachedColorMap::on_key_range_changed, this));
    // m_plot->setOpenGl(true);
}

FileCachedColorMap::~FileCachedColorMap() {
  clear_temporary_files();
}



void FileCachedColorMap::enable_realtime(bool is_enabled) {
  const bool was_realtime = m_is_realtime_enabled;
  if(!is_enabled && was_realtime) {
    flush_current_block();
  }
  m_is_realtime_enabled = is_enabled;
  m_plot->setInteraction(QCP::iRangeDrag, !is_enabled);
  m_plot->setInteraction(QCP::iRangeZoom, !is_enabled);
  if(!is_enabled) {
    remove_out_range_color_maps();
    request_replot(true);
  }
}

void FileCachedColorMap::append(const QVector<double>& data,bool is_roll,bool is_replot) {
  if(data.size() != m_value_size) {
    return;
  }
  m_is_instant_data_enabled = false;
  if(!m_current_color_map || m_block_cache.size() >= m_block_width) {
    flush_current_block();
    create_current_color_map();
    m_block_cache.clear();
    m_block_cache.reserve(m_block_width);
  }
  auto x = static_cast<int>(m_block_cache.size());
  const double map_k = normalized_map_k();
  const bool is_signed_plot = m_is_signed_plot;
  for(auto y = 0; y < data.size(); ++y) {
    m_current_color_map->data()->setCell(x, y, normalize_value(data[y], map_k, is_signed_plot));
  }
  m_block_cache.push_back(data);

  if(is_roll && m_absolute_track_count >= m_block_width - 1) {
      const double desired_upper = static_cast<double>(m_absolute_track_count + 1);
      if(desired_upper > m_key_axis->range().upper) {
          if(!m_scroll_timer.isValid()) {
              m_scroll_timer.start();
          }
          if(m_scroll_timer.elapsed() >= kRealtimeRefreshIntervalMs) {
              m_scroll_timer.restart();
              const double span = m_key_axis->range().size();
              m_key_axis->blockSignals(true);
              m_key_axis->setRange(desired_upper - span, desired_upper);
              m_key_axis->blockSignals(false);
              if(++m_scroll_update_counter >= kRealtimeCleanupScrollUpdates) {
                  m_scroll_update_counter = 0;
                  remove_out_range_color_maps();
              }
          }
      }
  }
  ++m_absolute_track_count;
  if(is_replot){
      request_replot(false);
  }
}

void FileCachedColorMap::clear() {
  m_plot->clearPlottables();
  m_current_color_map = nullptr;
  m_overlay_map = nullptr;
  m_is_realtime_enabled = true;
  m_is_instant_data_enabled = false;
  m_last_block_index = -1;
  m_absolute_track_count=0;
  m_scroll_update_counter = 0;
  m_scroll_timer.invalidate();
  m_replot_timer.invalidate();
  m_current_resolution = 1;
  m_loaded_blocks.clear();
  m_block_cache.clear();
  clear_temporary_files();
  m_plot->replot(QCustomPlot::rpImmediateRefresh);
}

void FileCachedColorMap::set_value_size(int value_size) {
  clear();
  m_value_size = value_size;
}

void FileCachedColorMap::set_value_range(double lower, double upper) {
  m_value_range = {lower, upper};
}

QVector<QVector<double>> FileCachedColorMap::get_data(int begin_track,
    int end_track) const {
  auto begin_block = static_cast<int>(begin_track / m_block_width);
  auto end_block = static_cast<int>(end_track / m_block_width);
  auto data = QVector<QVector<double>>();
  for(auto i = begin_block; i <= end_block; ++i) {
    auto block_start = i * m_block_width;
    auto begin_track_in_block = std::max(begin_track, block_start) - block_start;
    auto end_track_in_block = std::min(end_track, (i + 1) * m_block_width) - block_start;
    if(auto block_data = read_block_from_file(i, begin_track_in_block, end_track_in_block);
        !block_data.isEmpty()) {
      data.append(block_data);
    }
  }
  return data;
}

int FileCachedColorMap::get_resolution_for_zoom() const {
  auto span = m_key_axis->range().size();
  auto screen_width = m_plot->viewport().width();
  auto minimum_resolution = std::round(span / screen_width);
  for(auto resolution : AVAILABLE_RESOLUTIONS) {
    if(resolution >= minimum_resolution) {
      return resolution;
    }
  }
  return AVAILABLE_RESOLUTIONS.back();
}

QCPColorMap* FileCachedColorMap::create_color_map(int key_size, int value_size,
    const QCPRange& key_range, const QCPRange& value_range) const {
  auto color_map = new QCPColorMap(m_key_axis, m_value_axis);
  color_map->data()->setSize(key_size, value_size);
  color_map->data()->setRange(key_range, value_range);
  color_map->data()->fill(-1.0);
  color_map->setDataRange({0.0, 1.0});
  auto gray_scale_gradient = QCPColorGradient();
  gray_scale_gradient.setLevelCount(256);
  gray_scale_gradient.setColorStops(m_color_stops);
  gray_scale_gradient.setNanHandling(QCPColorGradient::NanHandling::nhNanColor);
  gray_scale_gradient.setNanColor(Qt::white);
  color_map->setGradient(gray_scale_gradient);
  return color_map;
}

// QCPColorMap* FileCachedColorMap::create_color_map(int key_size, int value_size,
//                                                   const QCPRange& key_range, const QCPRange& value_range) const {
//     auto color_map = new QCPColorMap(m_key_axis, m_value_axis);
//     color_map->data()->setSize(key_size, value_size);
//     color_map->data()->setRange(key_range, value_range);
//     color_map->data()->fill(-1.0);
//     color_map->setDataRange({0.0, 1.0});

//     // 创建自定义彩色渐变
//     auto color_gradient = QCPColorGradient();
//     color_gradient.setLevelCount(256);

//     // 定义彩色渐变颜色停止点
//     // 这里使用蓝-青-绿-黄-红的渐变，适合雷达数据显示
//     color_gradient.setColorStops({
//         {0.0, QColor(0, 0, 0)},   // 黄色
//         {0.25, QColor(0, 0, 255)},    // 红色
//         {0.5, QColor(0, 255, 0)},     // 绿色
//         {0.75, QColor(255, 0, 0)},    // 蓝色
//         {1.0, QColor(255, 255, 0)}        // 黑色
//     });

//     color_gradient.setNanHandling(QCPColorGradient::NanHandling::nhNanColor);
//     color_gradient.setNanColor(Qt::white);
//     color_map->setGradient(color_gradient);

//     return color_map;
// }




// QCPColorMap* FileCachedColorMap::create_color_map(int key_size, int value_size,
//                                                   const QCPRange& key_range, const QCPRange& value_range) const {
//     auto color_map = new QCPColorMap(m_key_axis, m_value_axis);
//     color_map->data()->setSize(key_size, value_size);
//     color_map->data()->setRange(key_range, value_range);
//     color_map->data()->fill(-1.0);


//     color_map->setDataRange({0.0, 1.0});

//     auto color_gradient = QCPColorGradient();
//     color_gradient.setLevelCount(256);  // 渐变分层数，越多过渡越平滑

//     // 定义贴合目标图片的颜色停止点
//     color_gradient.setColorStops({
//         {0.0,   QColor(0, 255, 255)},    // 深蓝色（最低值）
//         {0.15,  QColor(0, 50, 200)},   // 亮蓝色
//         {0.3,   QColor(0, 150, 255)},  // 天蓝色
//         {0.45,  QColor(0, 0, 120)},  // 青色
//         {0.6,   QColor(0, 255, 100)},  // 青绿色
//         {0.75,  QColor(0, 255, 0)},    // 绿色
//         {0.85,  QColor(255, 255, 0)},  // 黄色
//         {1.0,   QColor(255, 50, 0)}    // 橙红色（最高值，匹配顶部亮条纹）
//     });

//     color_gradient.setNanHandling(QCPColorGradient::NanHandling::nhNanColor);
//     color_gradient.setNanColor(Qt::white);  // 无效数据（NaN）显示为白色
//     color_map->setGradient(color_gradient);

//     return color_map;
// }



void FileCachedColorMap::create_current_color_map() {
  ++m_last_block_index;
  auto start_key = m_last_block_index * m_block_width;
  m_current_color_map = create_color_map(m_block_width, m_value_size,
    QCPRange(start_key, start_key + m_block_width), m_value_range);
}

void FileCachedColorMap::clear_temporary_files() {
  auto dir = QDir(m_dir_path);
  if(!dir.exists()) {
    return;
  }
  auto file_list = dir.entryInfoList(QDir::Files);
  for(const auto& file_info : file_list) {
    QFile::remove(file_info.absoluteFilePath());
  }
}

void FileCachedColorMap::remove_out_range_color_maps() {
  auto visible_range = m_key_axis->range();
  auto lower = visible_range.lower - visible_range.size();
  auto upper = visible_range.upper + visible_range.size();
  for(int i = m_plot->plottableCount() - 1; i >= 0; --i) {
    auto map = qobject_cast<QCPColorMap*>(m_plot->plottable(i));
    if(!map) {
      continue;
    }
    if(map == m_overlay_map) {
      continue;
    }
    if(map == m_current_color_map) {
      continue;
    }
    auto block_range = map->data()->keyRange();
    if(block_range.upper < lower || block_range.lower > upper) {
      m_plot->removePlottable(map);
      auto index = static_cast<int>(std::floor(block_range.lower / m_block_width));
      m_loaded_blocks.erase(QString("%1@%2").arg(index).arg(m_current_resolution));
    }
  }
}

void FileCachedColorMap::flush_current_block() {
  if(m_last_block_index < 0 || m_block_cache.isEmpty()) {
    return;
  }
  save_block_to_file(m_last_block_index, 1);
  m_loaded_blocks.insert(QString("%1@%2").arg(m_last_block_index).arg(1));
  if(m_is_realtime_enabled) {
    return;
  }
  build_downsamples_for_block(m_last_block_index, true);
}

void FileCachedColorMap::build_downsamples_for_block(int index, bool async) {
  for(auto resolution : AVAILABLE_RESOLUTIONS) {
    if(resolution > 1) {
      if(async) {
        std::thread([=] () {
          downsample_and_save(index, resolution);
        }).detach();
      } else {
        downsample_and_save(index, resolution);
      }
    }
  }
}

void FileCachedColorMap::request_replot(bool force) {
  if(!m_plot) {
    return;
  }
  if(force || !m_is_realtime_enabled) {
    m_plot->replot(QCustomPlot::rpQueuedReplot);
    if(m_replot_timer.isValid()) {
      m_replot_timer.restart();
    } else {
      m_replot_timer.start();
    }
    return;
  }
  if(!m_replot_timer.isValid()) {
    m_replot_timer.start();
    m_plot->replot(QCustomPlot::rpQueuedReplot);
    return;
  }
  if(m_replot_timer.elapsed() >= kRealtimeRefreshIntervalMs) {
    m_replot_timer.restart();
    m_plot->replot(QCustomPlot::rpQueuedReplot);
  }
}

void FileCachedColorMap::save_block_to_file(int index, int resolution) {
  auto filename = make_block_filename(index, resolution);
  auto file = QFile(QString("%1/%2").arg(m_dir_path).arg(filename));
  if(file.open(QIODevice::WriteOnly)) {
    auto out = QDataStream(&file);
    auto column = static_cast<int>(m_block_cache.size());
    out << column << m_value_size << index * m_block_width <<
      index * m_block_width + column;
    auto block_size = m_value_size * sizeof(double);
    for(auto x = 0; x < m_block_cache.size(); ++x) {
      out.writeRawData(reinterpret_cast<char*>(m_block_cache[x].data()), block_size);
    }
  }
}

void FileCachedColorMap::downsample_and_save(int index, int resolution) {
  auto src_path = QString("%1/%2").arg(m_dir_path).arg(make_block_filename(index, 1));
  auto dst_path = QString("%1/%2").arg(m_dir_path).arg(make_block_filename(index, resolution));
  if(!QFile::exists(src_path)) {
    return;
  }
  auto file = QFile(src_path);
  if(!file.open(QIODevice::ReadOnly)) {
    return;
  }
  auto in = QDataStream(&file);
  int columns = 0;
  int value_size = 0;
  int lower_key = 0;
  int upper_key = 0;
  in >> columns >> value_size >> lower_key >> upper_key;
  auto buffer = QVector<QVector<double>>(columns, QVector<double>(value_size));
  auto block_size = value_size * sizeof(double);
  for(auto x = 0; x < columns; ++x) {
    in.readRawData(reinterpret_cast<char*>(buffer[x].data()), block_size);
  }
  auto out_file = QFile(dst_path);
  if(out_file.open(QIODevice::WriteOnly)) {
    auto out = QDataStream(&out_file);
      auto step = resolution;
      const int out_columns = (columns + step - 1) / step;
      out << out_columns << value_size << lower_key << upper_key;

    for(int i = 0; i < out_columns; ++i) {
    const int x = i * step;
      auto downsample = QVector<double>(value_size);
      for(auto y = 0; y < value_size; ++y) {
        auto sum = 0.0;
        auto count = 0;
        for(auto dx = 0; dx < step && x + dx < columns; ++dx) {
          sum += buffer[x + dx][y];
          ++count;
        }
        downsample[y] = count > 0 ? sum / count : 0.0;
      }
      out.writeRawData(reinterpret_cast<char*>(downsample.data()),
        downsample.size() * sizeof(double));
    }
  }
}

void FileCachedColorMap::update_viewport() {
  if (m_is_instant_data_enabled) {
    m_plot->replot(QCustomPlot::rpQueuedReplot);
    return;
  }
  remove_out_range_color_maps();
  if(!m_is_realtime_enabled) {
    load_active_blocks();
    m_plot->replot(QCustomPlot::rpQueuedReplot);
  }
}

void FileCachedColorMap::switch_resolution(int resolution) {
    m_plot->clearPlottables();
    m_loaded_blocks.clear();
    m_overlay_map = nullptr;
    m_current_color_map = nullptr;
    m_current_resolution = resolution;
    m_plot->replot(QCustomPlot::rpQueuedReplot);
}


void FileCachedColorMap::load_active_blocks() {
  if(m_is_realtime_enabled) {
    return;
  }
  auto resolution = get_resolution_for_zoom();
  if(resolution != m_current_resolution) {
    switch_resolution(resolution);
  }
  auto visible_range = m_key_axis->range();
  auto lower = visible_range.lower - visible_range.size();
  auto upper = visible_range.upper + visible_range.size();
  for(auto i = 0; i <= m_last_block_index; ++i) {
    auto block_lower = i * m_block_width;
    if(block_lower + m_block_width - 1 >= lower && block_lower <= upper) {
      if(!m_loaded_blocks.contains(QString("%1@%2").arg(i).arg(resolution))) {
        load_block(i, resolution);
      }
    }
  }
}

void FileCachedColorMap::load_block(int index, int resolution) {
  auto filename = make_block_filename(index, resolution);
  auto file = QFile(QString("%1/%2").arg(m_dir_path).arg(filename));
  if(!file.open(QIODevice::ReadOnly)) {
    if(resolution > 1) {
      downsample_and_save(index, resolution);
      file.setFileName(QString("%1/%2").arg(m_dir_path).arg(filename));
    }
  }
  if(!file.isOpen() && !file.open(QIODevice::ReadOnly)) {
    return;
  }
  auto in = QDataStream(&file);
  int columns = 0;
  int value_size = 0;
  int lower_key = 0;
  int upper_key = 0;
  in >> columns >> value_size >> lower_key >> upper_key;
  auto map = create_color_map(columns, value_size,
    QCPRange(lower_key, upper_key), m_value_range);
  auto block_size = value_size * sizeof(double);
  for(auto x = 0; x < columns; ++x) {
    auto track_data = QVector<double>(value_size);
    in.readRawData(reinterpret_cast<char*>(track_data.data()), block_size);
    const double map_k = normalized_map_k();
    const bool is_signed_plot = m_is_signed_plot;
    for(auto y = 0; y < track_data.size(); ++y) {
      map->data()->setCell(x, y, normalize_value(track_data[y], map_k, is_signed_plot));
    }
  }
  m_loaded_blocks.insert(QString("%1@%2").arg(index).arg(resolution));
}

QVector<QVector<double>> FileCachedColorMap::read_block_from_file(int block_index,
    int begin_track, int end_track) const {
  auto data = QVector<QVector<double>>();
    auto filename = make_block_filename(block_index, 1);
  auto file = QFile(QString("%1/%2").arg(m_dir_path).arg(filename));
  if(!file.open(QIODevice::ReadOnly)) {
    return data;
  }
  auto in = QDataStream(&file);
  int columns = 0;
  int value_size = 0;
  int lower_key = 0;
  int upper_key = 0;
  in >> columns >> value_size >> lower_key >> upper_key;
  if(begin_track >= columns) {
    return data;
  }
  auto track_size_bytes = value_size * sizeof(double);
  auto start_offset = 4 * sizeof(int) + static_cast<qint64>(begin_track) * track_size_bytes;
  file.seek(start_offset);
  auto block_size = value_size * sizeof(double);
  for(auto x = begin_track; x < end_track; ++x) {
    auto track_data = QVector<double>(value_size);
    in.readRawData(reinterpret_cast<char*>(track_data.data()), block_size);
    data.push_back(track_data);
  }
  return data;
}

void FileCachedColorMap::on_key_range_changed(const QCPRange& range) {
  Q_UNUSED(range);
  if (m_io_suspended) {
    return;
  }
  update_viewport();
}

// void FileCachedColorMap::append_sideview(const QVector<QVector<double>>& data) {
//     // if(data.size() != m_value_size) {
//     //     return;
//     // }
//     // if(!m_current_color_map || m_block_cache.size() >= m_block_width) {
//     //     flush_current_block();
//         create_current_color_map();
//     //     m_block_cache.clear();
//     //     m_block_cache.reserve(m_block_width);
//     // }
//     // auto x = static_cast<int>(m_block_cache.size());
//     for(auto x = 0; x < data.size(); ++x){
//         for(auto y = 0; y < data[x].size(); ++y) {
//             m_current_color_map->data()->setCell(x, y, normalize_value(data[x][y]));
//         }
//     }
//     // m_block_cache.push_back(data);
//     // if(m_last_block_index * m_block_width + x > m_key_axis->range().upper) {
//     //     m_key_axis->moveRange(m_key_axis->range().size() / 2);
//     // }
//     m_plot->replot(QCustomPlot::rpQueuedReplot);
// }

QString FileCachedColorMap::make_block_filename(int index, int res) const{
    return QString("block_%1_%2_res%3.dat").arg(m_plotId).arg(index).arg(res);
}

void FileCachedColorMap::swapBuffers() {
    // if (m_background_color_map) {
    //     // 隐藏当前前台缓冲区
    //     if (m_current_color_map && m_current_color_map != m_background_color_map) {
    //         m_current_color_map->setVisible(false);
    //     }

    //     // 显示后台缓冲区
    //     m_background_color_map->setVisible(true);
    //     m_current_color_map = m_background_color_map;

    //     // 重置后台缓冲区指针，等待下次使用
    //     m_background_color_map = nullptr;
    //     m_using_front_buffer = true;
    // }

    // m_plot->replot(QCustomPlot::rpQueuedReplot);
}

void FileCachedColorMap::setDataInstant(const QVector<QVector<double>>& data)
{
    setDataInstant(data, 0);
}

void FileCachedColorMap::setDataInstant(const QVector<QVector<double>>& data, int begin_track)
{
    if (data.isEmpty() || data[0].isEmpty()) {
        return;
    }

    const int xSize = data.size();
    const int ySize = data[0].size();
    m_is_instant_data_enabled = true;

    if (!m_overlay_map) {
        m_overlay_map = create_color_map(xSize, ySize,
            QCPRange(begin_track, begin_track + xSize), m_value_range);
    } else {
        m_overlay_map->data()->setSize(xSize, ySize);
        m_overlay_map->data()->setRange(QCPRange(begin_track, begin_track + xSize), m_value_range);
    }

    const double map_k = normalized_map_k();
    const bool is_signed_plot = m_is_signed_plot;
    for (int x = 0; x < xSize; ++x) {
        for (int y = 0; y < ySize; ++y) {
            m_overlay_map->data()->setCell(x, y, normalize_value(data[x][y], map_k, is_signed_plot));
        }
    }

    setOverlayVisible(true);

    m_plot->replot(QCustomPlot::rpQueuedReplot);
}

void FileCachedColorMap::replaceDataRange(const QVector<QVector<double>>& data, int begin_track, bool is_replot)
{
    if (data.isEmpty() || begin_track < 0) {
        return;
    }
    for (int i = 0; i < data.size(); ++i) {
        if (data[i].size() != m_value_size) {
            return;
        }
    }

    const int end_track = begin_track + data.size();
    int replaced = 0;
    QSet<int> changed_blocks;
    for (int i = 0; i < m_plot->plottableCount(); ++i) {
        auto* map = qobject_cast<QCPColorMap*>(m_plot->plottable(i));
        if (!map || map == m_overlay_map) {
            continue;
        }
        const auto key_range = map->data()->keyRange();
        const int map_begin = static_cast<int>(std::round(key_range.lower));
        const int map_end = static_cast<int>(std::round(key_range.upper));
        const int overlap_begin = std::max(begin_track, map_begin);
        const int overlap_end = std::min(end_track, map_end);
        if (overlap_begin >= overlap_end) {
            continue;
        }
        for (int track = overlap_begin; track < overlap_end; ++track) {
            const int src_x = track - begin_track;
            const int map_x = track - map_begin;
            const auto& trace = data[src_x];
            const double map_k = normalized_map_k();
            const bool is_signed_plot = m_is_signed_plot;
            for (int y = 0; y < trace.size(); ++y) {
                map->data()->setCell(map_x, y, normalize_value(trace[y], map_k, is_signed_plot));
            }
            ++replaced;
        }
    }

    for (int track = begin_track; track < end_track; ++track) {
        const int src_x = track - begin_track;
        changed_blocks.insert(track / m_block_width);
        if (track >= m_absolute_track_count - m_block_cache.size() &&
            track < m_absolute_track_count) {
            const int cache_x = track - (m_absolute_track_count - m_block_cache.size());
            if (cache_x >= 0 && cache_x < m_block_cache.size()) {
                m_block_cache[cache_x] = data[src_x];
            }
        }
    }

    for (int block_index : changed_blocks) {
        const QString path = QString("%1/%2").arg(m_dir_path).arg(make_block_filename(block_index, 1));
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)) {
            continue;
        }

        QDataStream in(&file);
        int columns = 0;
        int value_size = 0;
        int lower_key = 0;
        int upper_key = 0;
        in >> columns >> value_size >> lower_key >> upper_key;
        if (columns <= 0 || value_size != m_value_size) {
            continue;
        }

        QVector<QVector<double>> block(columns, QVector<double>(value_size));
        const auto block_size = value_size * sizeof(double);
        for (int x = 0; x < columns; ++x) {
            in.readRawData(reinterpret_cast<char*>(block[x].data()), block_size);
        }
        file.close();

        const int block_begin = block_index * m_block_width;
        const int overlap_begin = std::max(begin_track, block_begin);
        const int overlap_end = std::min(end_track, block_begin + columns);
        if (overlap_begin >= overlap_end) {
            continue;
        }
        for (int track = overlap_begin; track < overlap_end; ++track) {
            block[track - block_begin] = data[track - begin_track];
        }

        if (!file.open(QIODevice::WriteOnly)) {
            continue;
        }
        QDataStream out(&file);
        out << columns << value_size << lower_key << upper_key;
        for (int x = 0; x < columns; ++x) {
            out.writeRawData(reinterpret_cast<char*>(block[x].data()), block_size);
        }
        file.close();

        for (auto resolution : AVAILABLE_RESOLUTIONS) {
            if (resolution > 1) {
                downsample_and_save(block_index, resolution);
            }
        }
    }

    if (replaced > 0 && is_replot) {
        m_plot->replot(QCustomPlot::rpQueuedReplot);
    }
}

void FileCachedColorMap::rebuild_downsamples(int max_block_index)
{
    for (int index = 0; index <= max_block_index; ++index) {
        for (auto resolution : AVAILABLE_RESOLUTIONS) {
            if (resolution > 1) {
                downsample_and_save(index, resolution);
            }
        }
    }
}

void FileCachedColorMap::refresh_from_disk()
{
    auto previous_realtime = m_is_realtime_enabled;
    m_is_realtime_enabled = false;

    m_plot->clearPlottables();
    m_current_color_map = nullptr;
    m_overlay_map = nullptr;

    m_loaded_blocks.clear();
    m_current_resolution = get_resolution_for_zoom();
    load_active_blocks();
    m_plot->replot(QCustomPlot::rpQueuedReplot);

    m_is_realtime_enabled = previous_realtime;
}

void FileCachedColorMap::set_last_block_index(int last_index)
{
    m_last_block_index = last_index;
}

void FileCachedColorMap::set_io_suspended(bool suspended)
{
    m_io_suspended = suspended;
}

// void FileCachedColorMap::setOverlayVisible(bool visible)
// {
//     if (!m_overlay_map) {
//         return;
//     }
//     for (int i = 0; i < m_plot->plottableCount(); ++i) {
//         if (auto* map = qobject_cast<QCPColorMap*>(m_plot->plottable(i))) {
//             if (visible) {
//                 map->setVisible(map == m_overlay_map);
//             } else {
//                 map->setVisible(map != m_overlay_map);
//             }
//         }
//     }
// }

void FileCachedColorMap::setOverlayVisible(bool visible)
{
    if (!m_overlay_map) {
        return;
    }
    // 只控制 overlay 自己的可见性，不再把其它 map 隐藏
    m_overlay_map->setVisible(visible);
}


void FileCachedColorMap::setColorGradient(const QCPColorGradient& gradient) {
    // 更新所有已存在的颜色映射
    for (int i = 0; i < m_plot->plottableCount(); ++i) {
        if (auto* colorMap = qobject_cast<QCPColorMap*>(m_plot->plottable(i))) {
            colorMap->setGradient(gradient);
        }
    }

    // 更新当前颜色映射
    // if (m_current_color_map) {
    //     m_current_color_map->setGradient(gradient);
    // }

    // 重新绘制
    m_plot->replot(QCustomPlot::rpQueuedReplot);
}

void FileCachedColorMap::setCustomGradient(const QVector<QPair<double, QColor>>& colorStops) {
    auto custom_gradient = QCPColorGradient();
    custom_gradient.setLevelCount(256);

    // 将 QVector<QPair<double, QColor>> 转换为 QMap<double, QColor>
    QMap<double, QColor> stopsMap;
    for (const auto& stop : colorStops) {
        stopsMap[stop.first] = stop.second;
    }
    m_color_stops=stopsMap;
    custom_gradient.setColorStops(stopsMap);
    custom_gradient.setNanHandling(QCPColorGradient::NanHandling::nhNanColor);
    custom_gradient.setNanColor(Qt::white);

    setColorGradient(custom_gradient);
}

void FileCachedColorMap::set_map_params(double k)
{
    g_map_k.store(k);
    const double map_k = normalized_map_k();
    const bool is_signed_plot = m_is_signed_plot;
    if (m_current_color_map && !m_block_cache.isEmpty()) {
        for (int x = 0; x < m_block_cache.size(); ++x) {
            const auto& trace = m_block_cache[x];
            for (int y = 0; y < trace.size(); ++y) {
                m_current_color_map->data()->setCell(x, y, normalize_value(trace[y], map_k, is_signed_plot));
            }
        }
    }
    if (!m_is_realtime_enabled) {
        refresh_from_disk();
        return;
    }
    m_plot->replot(QCustomPlot::rpQueuedReplot);
}


// void FileCachedColorMap::optimizeForSoftwareRendering() {
//     if (!m_plot) return;

//     qDebug() << "应用软件渲染优化";

//     // 关闭抗锯齿以提高性能
//     m_plot->setNotAntialiasedElements(QCP::aeAll);

//     // 启用快速绘制提示
//     m_plot->setPlottingHint(QCP::phFastPolylines, true);
//     m_plot->setPlottingHint(QCP::phImmediateRefresh, false);

//     // 设置适当的重绘策略


//     // 禁用OpenGL（如果之前启用了）
//     m_plot->setOpenGl(false);

//     // 设置缓冲区设备像素比为1，减少内存使用
//     m_plot->setBufferDevicePixelRatio(1.0);
// }

// 添加一个方法来检查当前渲染模式
// QString FileCachedColorMap::getRenderingMode() const {
//     if (!m_plot) return "Unknown";

//     if (m_plot->openGl()) {
//         return "OpenGL";
//     } else {
//         return "Software";
//     }
// }
