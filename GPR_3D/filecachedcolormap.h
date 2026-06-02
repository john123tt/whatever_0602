#ifndef FILE_CACHED_COLOR_MAP_H
#define FILE_CACHED_COLOR_MAP_H
#include <unordered_set>
#include <QElapsedTimer>
#include "qcustomplot.h"
#include <atomic>

/**
 * A class that manages a color map with file caching for large datasets.
 * Supports real-time updates, data retrieval, and efficient memory usage.
 * Stores color map data in temporary files at multiple resolutions, loading them on demand.
 * Color maps outside three times the viewport range are removed.
 */
class FileCachedColorMap : QObject {
  Q_OBJECT

  public:

    /** 
     * Constructor for FileCachedColorMap.
     * @param key_axis The axis for the keys (x-axis).
     * @param value_axis The axis for the values (y-axis).
     * @param block_width The width of each block in the color map.
     * @param parent The parent.
     */
    FileCachedColorMap(QCPAxis& key_axis, QCPAxis& value_axis,int block_width, const QString& plotId, QObject* parent = nullptr);

    ~FileCachedColorMap();

    /**
     * Toggles real-time updates for the color map. Disables interaction when enabled.
     */
    void enable_realtime(bool is_enabled);

    /** Appends a new block of data.  */
    void append(const QVector<double>& data,bool is_roll,bool is_replot);

    void append_sideview(const QVector<QVector<double>>& data);

    /** Clears all data and temporary files. */
    void clear();

    /** Sets the size of the value axis. */
    void set_value_size(int value_size);

    /** Sets the range of the value axis. */
    void set_value_range(double lower, double upper);

    /** Retrieves data from the given begin track to the end track. */
    QVector<QVector<double>> get_data(int begin_track, int end_track) const;
    QCustomPlot* m_plot;
    void switchToBackgroundBuffer();       // 切换到后台缓冲区
    void swapBuffers();                    // 交换缓冲区
    void setDataInstant(const QVector<QVector<double>>& data);
    void setDataInstant(const QVector<QVector<double>>& data, int begin_track);
    void replaceDataRange(const QVector<QVector<double>>& data, int begin_track, bool is_replot = true);
    void rebuild_downsamples(int max_block_index);
    void refresh_from_disk();
    void set_last_block_index(int last_index);
    void set_io_suspended(bool suspended);
    void setOverlayVisible(bool visible);

    void setColorGradient(const QCPColorGradient& gradient);

    void setCustomGradient(const QVector<QPair<double, QColor>>& colorStops);
    void set_map_params(double k);


  private:
    QCPAxis* m_key_axis;
    QCPAxis* m_value_axis;
    int m_block_width;
    QMap<double, QColor> m_color_stops;


    QCPColorMap* m_current_color_map;
    QCPColorMap* m_overlay_map;
    // QCPColorMap* m_background_color_map;   // 后台准备的颜色地图
    // bool m_using_front_buffer;
    bool m_is_realtime_enabled;
    bool m_is_instant_data_enabled;
    bool m_io_suspended;
    int m_value_size;
    QCPRange m_value_range;
    int m_last_block_index;
    int m_absolute_track_count;
    int m_scroll_update_counter = 0;
    QElapsedTimer m_scroll_timer;
    QElapsedTimer m_replot_timer;
    int m_current_resolution;
    std::unordered_set<QString> m_loaded_blocks;
    QVector<QVector<double>> m_block_cache;

    int get_resolution_for_zoom() const;
    QCPColorMap* create_color_map(int key_size, int value_size,
      const QCPRange& key_range, const QCPRange& value_range) const;
    void create_current_color_map();
    void clear_temporary_files();
    void remove_out_range_color_maps();
    void flush_current_block();
    void build_downsamples_for_block(int index, bool async);
    void request_replot(bool force = false);
    void save_block_to_file(int index, int resolution);
    void downsample_and_save(int index, int resolution);
    void update_viewport();
    void switch_resolution(int resolution);
    void load_active_blocks();
    void load_block(int index, int resolution);
    QVector<QVector<double>> read_block_from_file(int block_index,
      int begin_track, int end_track) const;
    void on_key_range_changed(const QCPRange& range);
    QString make_block_filename(int index, int res) const;
    QString m_plotId;
    QString m_dir_path;
    void optimizeForSoftwareRendering();
};

#endif // FILE_CACHED_COLOR_MAP_H
