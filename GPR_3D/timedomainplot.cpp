#include "timedomainplot.h"

TimeDomainPlot::TimeDomainPlot(QWidget *parent)
        : QWidget{parent} {
    auto layout = new QVBoxLayout(this);
    m_custom_plot = new QCustomPlot();
    m_custom_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    m_custom_plot->rescaleAxes();
    m_custom_plot->xAxis->setVisible(false);
    m_custom_plot->xAxis2->setVisible(true);
    m_custom_plot->yAxis->setRangeReversed(true);
    m_custom_plot->xAxis2->setTickLength(0, 3);
    m_custom_plot->yAxis->setTickLength(0, 3);
    m_custom_plot->xAxis2->setSubTickLength(0, 1);
    m_custom_plot->yAxis->setSubTickLength(0, 1);
    m_custom_plot->xAxis2->grid()->setVisible(true);
    m_custom_plot->axisRect()->setRangeDragAxes(m_custom_plot->xAxis2, nullptr);
    m_custom_plot->axisRect()->setRangeZoomAxes(m_custom_plot->xAxis2, m_custom_plot->yAxis);
    auto graph = m_custom_plot->addGraph(m_custom_plot->yAxis, m_custom_plot->xAxis2);
    graph->setPen(QPen(Qt::red, 2));
    m_custom_plot->replot(QCustomPlot::rpQueuedReplot);
//    connect(m_custom_plot, &QCustomPlot::mouseMove, this, &TimeDomainPlot::on_mouse_move);
    layout->addWidget(m_custom_plot);
}

void TimeDomainPlot::set_x_axis_label(const QString& label) {
    m_custom_plot->xAxis2->setLabel(label);
}

void TimeDomainPlot::set_y_axis_label(const QString& label) {
    m_custom_plot->yAxis->setLabel(label);
}

void TimeDomainPlot::clear() {
    m_custom_plot->graph()->data()->clear();
    m_custom_plot->replot(QCustomPlot::rpQueuedReplot);
}

void TimeDomainPlot::on_data_ready(const QVector<double>& x, const QVector<double>& y) {
    m_custom_plot->graph()->setData(x, y, true);
    m_custom_plot->rescaleAxes();
    m_custom_plot->xAxis2->setRange(-2,2);
    m_custom_plot->replot(QCustomPlot::rpQueuedReplot);
}

//void TimeDomainPlot::on_mouse_move(QMouseEvent* event) {
//    auto x = m_custom_plot->xAxis2->pixelToCoord(event->pos().x());
//    int y = m_custom_plot->yAxis->pixelToCoord(event->pos().y());
//    setToolTip(QString("[%1, %2]").arg(x).arg(y));
//}
