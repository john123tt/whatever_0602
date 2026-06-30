#ifndef ALGORITHM_TIMING_PROFILER_H
#define ALGORITHM_TIMING_PROFILER_H

#include <QDateTime>
#include <QElapsedTimer>
#include <QMutex>
#include <QString>
#include <QVector>

class AlgorithmTimingProfiler {
public:
    struct TraceTiming {
        int trace = 0;
        int channel = 0;
        qint64 background_ns = 0;
        qint64 ground_flatten_ns = 0;
        qint64 total_ns = 0;
    };

    struct V2WindowTiming {
        int window_start_trace = 0;
        int window_end_trace = 0;
        int trace_count = 0;
        qint64 valley_detector_ns = 0;
        double valley_detector_ns_per_trace = 0.0;
        int seeds = 0;
        int verified = 0;
        int boxes = 0;
    };

    void reset();
    void addTraceTiming(int trace, int channel, qint64 background_ns, qint64 ground_flatten_ns);
    void addV2WindowTiming(
        int window_start_trace,
        int window_end_trace,
        qint64 valley_detector_ns,
        int seeds,
        int verified,
        int boxes);
    void exportCsv(const QString& directory_path, bool force = false);

private:
    struct SummaryStats {
        double mean_us = 0.0;
        double max_us = 0.0;
        double p95_us = 0.0;
    };

    static SummaryStats summarize(const QVector<qint64>& values_ns);
    void printTraceSummaryLocked() const;
    void printV2SummaryLocked() const;

    mutable QMutex m_mutex;
    QVector<TraceTiming> m_trace_timings;
    QVector<V2WindowTiming> m_v2_window_timings;
    QDateTime m_session_start = QDateTime::currentDateTime();
    bool m_exported = false;
};

#endif // ALGORITHM_TIMING_PROFILER_H
