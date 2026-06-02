#ifndef TIMEDOMAINPLOT_H
#define TIMEDOMAINPLOT_H

#include <QWidget>
#include "qcustomplot.h"

class TimeDomainPlot : public QWidget
{
    Q_OBJECT
public:
    explicit TimeDomainPlot(QWidget *parent = nullptr);

    void set_x_axis_label(const QString& label);

    void set_y_axis_label(const QString& label);

    void clear();

public slots:
    void on_data_ready(const QVector<double>& x, const QVector<double>& y);

signals:

private:
    QCustomPlot* m_custom_plot;

//    void on_mouse_move(QMouseEvent* event);
};

#endif // TIMEDOMAINPLOT_H
