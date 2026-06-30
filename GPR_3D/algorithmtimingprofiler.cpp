#include "algorithmtimingprofiler.h"

#include <algorithm>
#include <cmath>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QMutexLocker>
#include <QTextStream>

namespace {
double ns_to_us(qint64 ns)
{
    return static_cast<double>(ns) / 1000.0;
}

QString timing_file_timestamp(const QDateTime& dt)
{
    return dt.toString("yyyyMMdd_hhmmss_zzz");
}
} // namespace

void AlgorithmTimingProfiler::reset()
{
    QMutexLocker locker(&m_mutex);
    m_trace_timings.clear();
    m_v2_window_timings.clear();
    m_session_start = QDateTime::currentDateTime();
    m_exported = false;
}

void AlgorithmTimingProfiler::addTraceTiming(
    int trace,
    int channel,
    qint64 background_ns,
    qint64 ground_flatten_ns)
{
    QMutexLocker locker(&m_mutex);
    m_trace_timings.append({
        trace,
        channel,
        background_ns,
        ground_flatten_ns,
        background_ns + ground_flatten_ns
    });
    m_exported = false;
}

void AlgorithmTimingProfiler::addV2WindowTiming(
    int window_start_trace,
    int window_end_trace,
    qint64 valley_detector_ns,
    int seeds,
    int verified,
    int boxes)
{
    const int trace_count = std::max(0, window_end_trace - window_start_trace);
    QMutexLocker locker(&m_mutex);
    m_v2_window_timings.append({
        window_start_trace,
        window_end_trace,
        trace_count,
        valley_detector_ns,
        trace_count > 0 ? static_cast<double>(valley_detector_ns) / trace_count : 0.0,
        seeds,
        verified,
        boxes
    });
    m_exported = false;
}

void AlgorithmTimingProfiler::exportCsv(const QString& directory_path, bool force)
{
    QMutexLocker locker(&m_mutex);
    if (m_exported && !force) {
        return;
    }
    if (m_trace_timings.isEmpty() && m_v2_window_timings.isEmpty()) {
        return;
    }

    QDir dir(directory_path);
    if (!dir.exists() && !dir.mkpath(".")) {
        qWarning() << "AlgorithmTimingProfiler: failed to create timing directory" << directory_path;
        return;
    }

    const QString stamp = timing_file_timestamp(m_session_start);
    const QString trace_path = dir.filePath(QString("trace_timing_%1.csv").arg(stamp));
    QFile trace_file(trace_path);
    if (trace_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&trace_file);
        out << "trace,channel,background_us,ground_flatten_us,total_us\n";
        for (const auto& item : m_trace_timings) {
            out << item.trace << ','
                << item.channel << ','
                << QString::number(ns_to_us(item.background_ns), 'f', 3) << ','
                << QString::number(ns_to_us(item.ground_flatten_ns), 'f', 3) << ','
                << QString::number(ns_to_us(item.total_ns), 'f', 3) << '\n';
        }
    } else {
        qWarning() << "AlgorithmTimingProfiler: failed to write" << trace_path;
    }

    const QString v2_path = dir.filePath(QString("valley_window_timing_%1.csv").arg(stamp));
    QFile v2_file(v2_path);
    if (v2_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&v2_file);
        out << "window_start_trace,window_end_trace,trace_count,valley_detector_us,valley_detector_us_per_trace,seeds,verified,boxes\n";
        for (const auto& item : m_v2_window_timings) {
            out << item.window_start_trace << ','
                << item.window_end_trace << ','
                << item.trace_count << ','
                << QString::number(ns_to_us(item.valley_detector_ns), 'f', 3) << ','
                << QString::number(item.valley_detector_ns_per_trace / 1000.0, 'f', 3) << ','
                << item.seeds << ','
                << item.verified << ','
                << item.boxes << '\n';
        }
    } else {
        qWarning() << "AlgorithmTimingProfiler: failed to write" << v2_path;
    }

    printTraceSummaryLocked();
    printV2SummaryLocked();
    qInfo() << "Algorithm timing CSV exported:" << trace_path << v2_path;
    m_exported = true;
}

AlgorithmTimingProfiler::SummaryStats AlgorithmTimingProfiler::summarize(const QVector<qint64>& values_ns)
{
    SummaryStats stats;
    if (values_ns.isEmpty()) {
        return stats;
    }

    QVector<qint64> sorted = values_ns;
    std::sort(sorted.begin(), sorted.end());

    qint64 sum = 0;
    for (qint64 value : sorted) {
        sum += value;
    }

    const int value_count = static_cast<int>(sorted.size());
    const int p95_index = std::min(
        value_count - 1,
        static_cast<int>(std::ceil(sorted.size() * 0.95)) - 1);
    stats.mean_us = ns_to_us(sum / value_count);
    stats.max_us = ns_to_us(sorted.last());
    stats.p95_us = ns_to_us(sorted[std::max(0, p95_index)]);
    return stats;
}

void AlgorithmTimingProfiler::printTraceSummaryLocked() const
{
    if (m_trace_timings.isEmpty()) {
        return;
    }

    QVector<qint64> bg;
    QVector<qint64> flatten;
    QVector<qint64> total;
    bg.reserve(m_trace_timings.size());
    flatten.reserve(m_trace_timings.size());
    total.reserve(m_trace_timings.size());
    for (const auto& item : m_trace_timings) {
        bg.append(item.background_ns);
        flatten.append(item.ground_flatten_ns);
        total.append(item.total_ns);
    }

    const auto bg_stats = summarize(bg);
    const auto flatten_stats = summarize(flatten);
    const auto total_stats = summarize(total);
    qInfo().noquote()
        << QString("[Timing] traces=%1 bg mean/p95/max=%2/%3/%4 us, flatten mean/p95/max=%5/%6/%7 us, basic total mean/p95/max=%8/%9/%10 us")
               .arg(m_trace_timings.size())
               .arg(bg_stats.mean_us, 0, 'f', 3)
               .arg(bg_stats.p95_us, 0, 'f', 3)
               .arg(bg_stats.max_us, 0, 'f', 3)
               .arg(flatten_stats.mean_us, 0, 'f', 3)
               .arg(flatten_stats.p95_us, 0, 'f', 3)
               .arg(flatten_stats.max_us, 0, 'f', 3)
               .arg(total_stats.mean_us, 0, 'f', 3)
               .arg(total_stats.p95_us, 0, 'f', 3)
               .arg(total_stats.max_us, 0, 'f', 3);
}

void AlgorithmTimingProfiler::printV2SummaryLocked() const
{
    if (m_v2_window_timings.isEmpty()) {
        return;
    }

    QVector<qint64> per_trace;
    QVector<qint64> per_window;
    per_trace.reserve(m_v2_window_timings.size());
    per_window.reserve(m_v2_window_timings.size());
    for (const auto& item : m_v2_window_timings) {
        per_trace.append(static_cast<qint64>(item.valley_detector_ns_per_trace));
        per_window.append(item.valley_detector_ns);
    }

    const auto trace_stats = summarize(per_trace);
    const auto window_stats = summarize(per_window);
    qInfo().noquote()
        << QString("[Timing] valley windows=%1 per-trace mean/p95/max=%2/%3/%4 us, per-window mean/p95/max=%5/%6/%7 us")
               .arg(m_v2_window_timings.size())
               .arg(trace_stats.mean_us, 0, 'f', 3)
               .arg(trace_stats.p95_us, 0, 'f', 3)
               .arg(trace_stats.max_us, 0, 'f', 3)
               .arg(window_stats.mean_us, 0, 'f', 3)
               .arg(window_stats.p95_us, 0, 'f', 3)
               .arg(window_stats.max_us, 0, 'f', 3);
}
