#ifndef IMAGE2DPLOT_H
#define IMAGE2DPLOT_H
#include <QWidget>
#include "filecachedcolormap.h"
#include "qcustomplot.h"
#include <QMenu>
#include <QResizeEvent>

class Image2DPlot : public QWidget
{
    Q_OBJECT
public:
    explicit Image2DPlot(const QString& plot_id,QWidget *parent = nullptr);

    void create(int y_size, double y_axis_lower, double y_axis_upper,int x_axis_width=2048);

    void clear();

    void append(const QVector<double>& data,bool is_roll=true,bool is_replot=true);

    void set_x_axis_label(const QString& label);

    void set_y_axis_label(const QString& label);

    void set_scale_axes(bool x_axis, bool y_axis);

    void set_axis_annotations_visible(bool visible);

    void enable_realtime(bool is_enabled);

    QVector<QVector<double>> get_data(int begin_track, int end_track) const;
    // void onPlotDoubleClick(int x);
    void replot();

    void set_x_axis_range(int x);

    void swapBuffers();

    void setDataInstant(const QVector<QVector<double>>& data);
    void setDataInstant(const QVector<QVector<double>>& data, int begin_track);
    void replaceDataRange(const QVector<QVector<double>>& data, int begin_track, bool is_replot = true);
    void set_overlay_visible(bool visible);
    void rebuild_downsamples(int max_block_index);
    void refresh_from_disk();
    void set_last_block_index(int last_index);
    void set_io_suspended(bool suspended);
    void set_interactions_enabled(bool enabled);

    QCPRange get_x_axis_range() const;

    void setCustomGradient(const QVector<QPair<double, QColor>>& colorStops);
    void set_map_params(double k);
    void axisRectMetrics(int& top, int& bottom, int& height) const;

public slots:
    void sync_x_range(const QCPRange& range);

signals:
    void double_click(int x,int y,QString m_plot_id);
    void x_range_changed(const QCPRange& range);
    void axisRectChanged(int top, int bottom, int height);

private:
    QCustomPlot* m_custom_plot;
    FileCachedColorMap* m_color_map;
    // QCPColorMap* m_backgroundColorMap = nullptr;
    QString m_plot_id;
    int m_data_size;
    bool m_is_display;
    bool m_axis_annotations_visible = true;
    int x_index_timer;
    int y_index_timer;
    QTimer* m_mouse_timer;  // 延迟重绘定时器
    QMenu* m_context_menu;
    QAction* m_toggle_mouse_switch_action;
    bool m_mouse_switch_enabled; // 是否启用鼠标切换图层功能
    void on_double_click(QMouseEvent* event);
    bool eventFilter(QObject* obj, QEvent* event) override;  // 事件过滤器
    void on_mouse_move_timeout();
    void create_context_menu();                              // 创建右键菜单
    void contextMenuEvent(QContextMenuEvent* event) override; // 右键菜单事件
    void toggle_mouse_layer_switch();
    void resizeEvent(QResizeEvent* event) override;
    void emit_axis_rect_changed();
    //    void on_mouse_move(QMouseEvent* event);
};

#endif // IMAGE2DPLOT_H
