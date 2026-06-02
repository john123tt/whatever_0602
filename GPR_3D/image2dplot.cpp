#include "image2dplot.h"
#include <algorithm>

const auto KEY_AXIS_WIDTH = 2048;

Image2DPlot::Image2DPlot(const QString& plot_id,QWidget *parent)
    : QWidget{parent},
    m_data_size(-1),
    m_is_display(false),
    m_mouse_switch_enabled(false),
    m_plot_id(plot_id){
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    m_custom_plot = new QCustomPlot();
    m_custom_plot->setContentsMargins(0, 0, 0, 0);
    m_custom_plot->plotLayout()->setMargins(QMargins(0, 0, 0, 0));
    m_custom_plot->plotLayout()->setRowSpacing(0);
    m_custom_plot->plotLayout()->setColumnSpacing(0);
    // 启用鼠标跟踪
    m_custom_plot->setMouseTracking(true);
    m_custom_plot->installEventFilter(this);  // 安装事件过滤器
    m_custom_plot->setAttribute(Qt::WA_NoMousePropagation, false);
    m_custom_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    m_custom_plot->rescaleAxes();
    m_custom_plot->xAxis->setVisible(false);
    m_custom_plot->yAxis->setVisible(false);
    m_custom_plot->xAxis2->setVisible(true);
    m_custom_plot->yAxis2->setVisible(true);
    m_custom_plot->yAxis2->setRangeReversed(true);
    m_custom_plot->xAxis2->setTickLength(0, 3);
    m_custom_plot->yAxis2->setTickLength(0, 3);
    m_custom_plot->xAxis2->setSubTickLength(0, 1);
    m_custom_plot->yAxis2->setSubTickLength(0, 1);
    m_custom_plot->xAxis2->grid()->setVisible(false);
    m_custom_plot->yAxis2->grid()->setVisible(false);
    m_custom_plot->axisRect()->setRangeDragAxes(m_custom_plot->xAxis2, nullptr);
    m_custom_plot->axisRect()->setRangeZoomAxes(m_custom_plot->xAxis2, nullptr);
    m_custom_plot->xAxis2->setRange(0, KEY_AXIS_WIDTH);
    connect(m_custom_plot->xAxis2, qOverload<const QCPRange&>(&QCPAxis::rangeChanged),
            [=] (const auto& range) {
                if(range.lower < 0) {
                    m_custom_plot->xAxis2->setRange(0, range.size());
                }
                emit x_range_changed(m_custom_plot->xAxis2->range());
            });
    connect(m_custom_plot, &QCustomPlot::mouseDoubleClick, this, &Image2DPlot::on_double_click);
    m_custom_plot->replot(QCustomPlot::rpQueuedReplot);
    layout->addWidget(m_custom_plot);
    m_color_map = new FileCachedColorMap(*m_custom_plot->xAxis2, *m_custom_plot->yAxis2,
                                         KEY_AXIS_WIDTH, m_plot_id,this);
    // 初始化鼠标移动定时器
    m_mouse_timer = new QTimer(this);
    m_mouse_timer->setSingleShot(true);
    m_mouse_timer->setInterval(100);  // 50fps，可根据性能调整
    connect(m_mouse_timer, &QTimer::timeout, this, &Image2DPlot::on_mouse_move_timeout);
    create_context_menu();
}

void Image2DPlot::set_x_axis_label(const QString& label) {
    m_custom_plot->xAxis2->setLabel(label);
    emit_axis_rect_changed();
}

void Image2DPlot::set_y_axis_label(const QString& label) {
    m_custom_plot->yAxis2->setLabel(label);
    emit_axis_rect_changed();
}

void Image2DPlot::set_scale_axes(bool x_axis, bool y_axis) {
    m_custom_plot->axisRect()->setRangeZoomAxes(x_axis ? m_custom_plot->xAxis2 : nullptr,
                                                y_axis ? m_custom_plot->yAxis2 : nullptr);
    emit_axis_rect_changed();
}

void Image2DPlot::set_axis_annotations_visible(bool visible) {
    m_axis_annotations_visible = visible;
    m_custom_plot->xAxis2->setVisible(visible);
    m_custom_plot->yAxis2->setVisible(visible);
    m_custom_plot->axisRect()->setAutoMargins(visible ? QCP::msAll : QCP::msNone);
    m_custom_plot->axisRect()->setMargins(visible ? QMargins() : QMargins(0, 0, 0, 0));
    m_custom_plot->replot(QCustomPlot::rpQueuedReplot);
    emit_axis_rect_changed();
}

void Image2DPlot::enable_realtime(bool is_enabled) {
    m_is_display=is_enabled;
    m_color_map->enable_realtime(is_enabled);
    // m_color_map->swapBuffers();
}

QVector<QVector<double>> Image2DPlot::get_data(int begin_track, int end_track) const {
    return m_color_map->get_data(begin_track, end_track);
}

void Image2DPlot::create(int y_size, double y_axis_lower, double y_axis_upper,int x_axis_width) {
    m_data_size=y_size;
    if(x_axis_width>0)
    {
        m_custom_plot->xAxis2->setRange(0, x_axis_width);
    }
    m_custom_plot->yAxis2->setRange(y_axis_lower, y_axis_upper);
    m_color_map->set_value_size(y_size);
    m_color_map->set_value_range(y_axis_lower, y_axis_upper);
    emit_axis_rect_changed();
}

void Image2DPlot::clear() {
    m_color_map->clear();
}

void Image2DPlot::append(const QVector<double>& data,bool is_roll,bool is_replot) {
    m_color_map->append(data,is_roll,is_replot);
}

void Image2DPlot::on_double_click(QMouseEvent* event) {
    if(m_is_display||event->button() != Qt::LeftButton||m_data_size==-1) {
        return;
    }
    auto x_coord = m_custom_plot->xAxis2->pixelToCoord(event->pos().x());
    auto y_coord = m_custom_plot->yAxis2->pixelToCoord(event->pos().y());
    int y_index = static_cast<int>((y_coord / (m_custom_plot->yAxis2->range().upper-m_custom_plot->yAxis2->range().lower))*m_data_size);
    int x_index = static_cast<int>(x_coord);
    x_index = qBound(static_cast<int>(m_custom_plot->xAxis2->range().lower), x_index, static_cast<int>(m_custom_plot->xAxis2->range().upper));
    y_index = qBound(0, y_index, m_data_size - 1);
    emit double_click(x_index,y_index,m_plot_id);
}

void Image2DPlot::replot() {
    m_color_map->m_plot->replot();
    emit_axis_rect_changed();
}
// void Image2DPlot::plot_2d_dimension(const QVector<QVector<double>>& m_side_data){
//     m_color_map->append_sideview(m_side_data);


// }
// 实现同步槽函数
void Image2DPlot::sync_x_range(const QCPRange& range) {
    // 避免自身范围变化触发的循环同步（仅当范围不同时才更新）
    if (m_custom_plot->xAxis2->range() != range) {
        m_custom_plot->xAxis2->setRange(range);
        m_custom_plot->replot();
    }
}

void Image2DPlot::set_x_axis_range(int x) {
    if(x>0){
         m_custom_plot->xAxis2->setRange(0, x);
    }
}



// 事件过滤器 - 处理鼠标移动事件
bool Image2DPlot::eventFilter(QObject* obj, QEvent* event)
{
    if(!m_mouse_switch_enabled)
    {
        return QWidget::eventFilter(obj, event);
    }
    if (obj == m_custom_plot && event->type() == QEvent::MouseMove&&m_data_size!=-1) {
        QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);

        auto x_coord = m_custom_plot->xAxis2->pixelToCoord(mouse_event->pos().x());
        auto y_coord = m_custom_plot->yAxis2->pixelToCoord(mouse_event->pos().y());
        y_index_timer = static_cast<int>((y_coord / (m_custom_plot->yAxis2->range().upper-m_custom_plot->yAxis2->range().lower))*m_data_size+ 0.5)-1;
        x_index_timer = static_cast<int>(x_coord);
        x_index_timer = qBound(static_cast<int>(m_custom_plot->xAxis2->range().lower), x_index_timer, static_cast<int>(m_custom_plot->xAxis2->range().upper));
        y_index_timer = qBound(0, y_index_timer, m_data_size - 1);

        if (!m_mouse_timer->isActive()) {
            m_mouse_timer->start();
        }
        return false; // 继续传递事件
    }

    return QWidget::eventFilter(obj, event);
}


void Image2DPlot::on_mouse_move_timeout()
{
    emit double_click(x_index_timer,y_index_timer,m_plot_id);
}

// 创建右键菜单
void Image2DPlot::create_context_menu()
{
    m_context_menu = new QMenu(this);
    m_toggle_mouse_switch_action = new QAction("启用鼠标切换图层", this);

    // 设置初始状态
    if (m_mouse_switch_enabled) {
        m_toggle_mouse_switch_action->setText("禁用鼠标切换图层");
    } else {
        m_toggle_mouse_switch_action->setText("启用鼠标切换图层");
    }
    connect(m_toggle_mouse_switch_action, &QAction::triggered, this, &Image2DPlot::toggle_mouse_layer_switch);
    m_context_menu->addAction(m_toggle_mouse_switch_action);
}

void Image2DPlot::contextMenuEvent(QContextMenuEvent* event)
{
    if (m_context_menu) {
        m_context_menu->exec(event->globalPos());
    }
}

void Image2DPlot::toggle_mouse_layer_switch()
{
    m_mouse_switch_enabled = !m_mouse_switch_enabled;
    if (m_mouse_switch_enabled) {
        m_toggle_mouse_switch_action->setText("禁用鼠标切换图层");
    } else {
        m_toggle_mouse_switch_action->setText("启用鼠标切换图层");
    }
}

void Image2DPlot::swapBuffers(){
    // m_color_map->swapBuffers();
}

void Image2DPlot::setDataInstant(const QVector<QVector<double>>& data)
{
    setDataInstant(data, 0);
}

void Image2DPlot::setDataInstant(const QVector<QVector<double>>& data, int begin_track)
{
    m_color_map->setDataInstant(data, begin_track);
}

void Image2DPlot::replaceDataRange(const QVector<QVector<double>>& data, int begin_track, bool is_replot)
{
    m_color_map->replaceDataRange(data, begin_track, is_replot);
}

void Image2DPlot::set_overlay_visible(bool visible)
{
    m_color_map->setOverlayVisible(visible);
}

void Image2DPlot::rebuild_downsamples(int max_block_index)
{
    m_color_map->rebuild_downsamples(max_block_index);
}

void Image2DPlot::refresh_from_disk()
{
    m_color_map->refresh_from_disk();
}

void Image2DPlot::set_last_block_index(int last_index)
{
    m_color_map->set_last_block_index(last_index);
}

void Image2DPlot::set_io_suspended(bool suspended)
{
    m_color_map->set_io_suspended(suspended);
}

void Image2DPlot::set_interactions_enabled(bool enabled)
{
    m_custom_plot->setInteraction(QCP::iRangeDrag, enabled);
    m_custom_plot->setInteraction(QCP::iRangeZoom, enabled);
}

QCPRange Image2DPlot::get_x_axis_range() const
{
    return m_custom_plot->xAxis2->range();
}
void Image2DPlot::setCustomGradient(const QVector<QPair<double, QColor>>& colorStops)
{
    m_color_map->setCustomGradient(colorStops);
}

void Image2DPlot::set_map_params(double k)
{
    m_color_map->set_map_params(k);
}

void Image2DPlot::axisRectMetrics(int& top, int& bottom, int& height) const
{
    if (!m_custom_plot || !m_custom_plot->axisRect()) {
        top = 0;
        bottom = 0;
        height = this->height();
        return;
    }
    const QRect rect = m_custom_plot->axisRect()->rect();
    const int plot_height = m_custom_plot->height();
    top = rect.top();
    bottom = std::max(0, plot_height - rect.bottom() - 1);
    height = rect.height();
}

void Image2DPlot::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    emit_axis_rect_changed();
}

void Image2DPlot::emit_axis_rect_changed()
{
    int top = 0;
    int bottom = 0;
    int height = 0;
    axisRectMetrics(top, bottom, height);
    emit axisRectChanged(top, bottom, height);
}
