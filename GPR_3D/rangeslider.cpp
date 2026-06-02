#include "rangeslider.h"

#include <QMouseEvent>
#include <QPainter>
#include <QtMath>

RangeSlider::RangeSlider(QWidget* parent)
    : QWidget(parent),
      m_minimum(0),
      m_maximum(100),
      m_lower(25),
      m_upper(75),
      m_drag_mode(DragMode::None),
      m_last_pos(0)
{
    setMouseTracking(true);
    setMinimumWidth(24);
}

void RangeSlider::setRange(int minimum, int maximum)
{
    if (minimum > maximum) {
        return;
    }
    m_minimum = minimum;
    m_maximum = maximum;
    clamp_values();
    update();
}

void RangeSlider::setValues(int lower, int upper)
{
    m_lower = lower;
    m_upper = upper;
    clamp_values();
    update();
    emit_range_changed();
}

int RangeSlider::lowerValue() const
{
    return m_lower;
}

int RangeSlider::upperValue() const
{
    return m_upper;
}

void RangeSlider::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const int r = handle_radius();
    const int x = width() / 2;
    const int top = r;
    const int bottom = height() - r;

    painter.setPen(QPen(Qt::gray, 2));
    painter.drawLine(x, top, x, bottom);

    const int y_lower = pos_from_value(m_lower);
    const int y_upper = pos_from_value(m_upper);

    painter.setPen(QPen(QColor(80, 160, 240), 4));
    painter.drawLine(x, y_lower, x, y_upper);

    painter.setBrush(QColor(240, 240, 240));
    painter.setPen(QPen(Qt::black, 1));
    painter.drawEllipse(QPointF(x, y_lower), r, r);
    painter.drawEllipse(QPointF(x, y_upper), r, r);
}

void RangeSlider::mousePressEvent(QMouseEvent* event)
{
    const int r = handle_radius();
    const int y = event->pos().y();
    const int y_lower = pos_from_value(m_lower);
    const int y_upper = pos_from_value(m_upper);

    if (qAbs(y - y_lower) <= r) {
        m_drag_mode = DragMode::Lower;
    } else if (qAbs(y - y_upper) <= r) {
        m_drag_mode = DragMode::Upper;
    } else if (y >= y_lower && y <= y_upper) {
        m_drag_mode = DragMode::Range;
        m_last_pos = y;
    } else {
        if (qAbs(y - y_lower) < qAbs(y - y_upper)) {
            m_drag_mode = DragMode::Lower;
            m_lower = value_from_pos(y);
        } else {
            m_drag_mode = DragMode::Upper;
            m_upper = value_from_pos(y);
        }
        clamp_values();
        emit_range_changed();
        update();
    }
}

void RangeSlider::mouseMoveEvent(QMouseEvent* event)
{
    if (m_drag_mode == DragMode::None) {
        return;
    }
    const int y = event->pos().y();
    if (m_drag_mode == DragMode::Lower) {
        m_lower = value_from_pos(y);
    } else if (m_drag_mode == DragMode::Upper) {
        m_upper = value_from_pos(y);
    } else if (m_drag_mode == DragMode::Range) {
        const int delta = y - m_last_pos;
        const int delta_value = value_from_pos(y) - value_from_pos(m_last_pos);
        m_lower += delta_value;
        m_upper += delta_value;
        m_last_pos = y;
        if (delta == 0) {
            return;
        }
    }
    clamp_values();
    emit_range_changed();
    update();
}

void RangeSlider::mouseReleaseEvent(QMouseEvent*)
{
    if (m_drag_mode != DragMode::None) {
        emit rangeReleased(m_lower, m_upper);
    }
    m_drag_mode = DragMode::None;
}

void RangeSlider::leaveEvent(QEvent*)
{
    m_drag_mode = DragMode::None;
}

int RangeSlider::value_from_pos(int y) const
{
    const int r = handle_radius();
    const int top = r;
    const int bottom = height() - r;
    if (bottom <= top) {
        return m_minimum;
    }
    const int clamped = qBound(top, y, bottom);
    const double ratio = static_cast<double>(clamped - top) / static_cast<double>(bottom - top);
    const int value = static_cast<int>(qRound(m_minimum + ratio * (m_maximum - m_minimum)));
    return qBound(m_minimum, value, m_maximum);
}

int RangeSlider::pos_from_value(int value) const
{
    const int r = handle_radius();
    const int top = r;
    const int bottom = height() - r;
    if (m_maximum == m_minimum) {
        return top;
    }
    const double ratio = static_cast<double>(value - m_minimum) / static_cast<double>(m_maximum - m_minimum);
    return static_cast<int>(top + ratio * (bottom - top));
}

int RangeSlider::handle_radius() const
{
    return 6;
}

void RangeSlider::clamp_values()
{
    m_lower = qBound(m_minimum, m_lower, m_maximum);
    m_upper = qBound(m_minimum, m_upper, m_maximum);
    if (m_lower > m_upper) {
        std::swap(m_lower, m_upper);
    }
}

void RangeSlider::emit_range_changed()
{
    emit rangeChanged(m_lower, m_upper);
}
