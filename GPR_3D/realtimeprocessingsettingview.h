#ifndef REALTIMEPROCESSINGSETTINGVIEW_H
#define REALTIMEPROCESSINGSETTINGVIEW_H
#include <QWidget>
#include <QSettings>
#include "global.h"

namespace Ui {
class RealTimeProcessingSettingView;
}

enum class ScaleOption {
    X_AXIS,
    Y_AXIS,
    XY_AXIS
};

class RealTimeProcessingSettingView : public QWidget
{
    Q_OBJECT

public:
    explicit RealTimeProcessingSettingView(QWidget *parent = nullptr);
    ~RealTimeProcessingSettingView();

    void set_eliminate_background(bool is_enabled);

    bool is_background_elimination_enabled() const;

    bool is_sdk_time_transform_enabled() const;

    bool is_mine_ground_trend_flatten_enabled() const;
    bool is_v2_hyperbola_detector_enabled() const;

    void enable_eliminate_background_button(bool is_enabled);

    void enable_adjust_gain_button(bool is_enabled);

    void enable_time_transform_selector(bool is_enabled);

signals:
    void adjust_gain();
    void eliminate_background(bool is_enabled);
    void change_color_stops(const QVector<QPair<double, QColor>>& colorStops);
    void mine_ground_trend_flatten_changed(bool enabled);
    void v2_hyperbola_detector_changed(bool enabled);

private slots:
    void on_adjust_gain_button_clicked();

    void on_eliminate_background_currentIndexChanged(int index);

    void on_comboBox_currentIndexChanged(int index);
    void on_time_domain_transform_currentIndexChanged(int index);
    void on_mine_ground_trend_flatten_currentIndexChanged(int index);
    void on_v2_hyperbola_detector_currentIndexChanged(int index);

private:
    Ui::RealTimeProcessingSettingView* ui;
    QSettings m_settings;
};

#endif // REALTIMEPROCESSINGSETTINGVIEW_H
