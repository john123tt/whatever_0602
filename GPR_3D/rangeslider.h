#ifndef RANGESLIDER_H
#define RANGESLIDER_H

#include <QWidget>

class RangeSlider : public QWidget
{
    Q_OBJECT
public:
    explicit RangeSlider(QWidget* parent = nullptr);

    void setRange(int minimum, int maximum);
    void setValues(int lower, int upper);
    int lowerValue() const;
    int upperValue() const;

signals:
    void rangeChanged(int lower, int upper);
    void rangeReleased(int lower, int upper);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    enum class DragMode { None, Lower, Upper, Range };

    int m_minimum;
    int m_maximum;
    int m_lower;
    int m_upper;
    DragMode m_drag_mode;
    int m_last_pos;

    int value_from_pos(int y) const;
    int pos_from_value(int value) const;
    int handle_radius() const;
    void clamp_values();
    void emit_range_changed();
};

#endif // RANGESLIDER_H
