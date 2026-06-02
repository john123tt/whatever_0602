#ifndef ADJUSTGAINSETTINGDIALOG_H
#define ADJUSTGAINSETTINGDIALOG_H
#include <QDialog>
#include<qcustomplot.h>
#include<GPR_Type.h>
#include "global.h"
#include "setting.h"


namespace Ui {
class AdjustGainSettingDialog;
}

class AdjustGainSettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AdjustGainSettingDialog(QVector<double> data, QVector<double> gains,
      double gain_parameter, double start_time, double end_time, QWidget* parent = nullptr);
    ~AdjustGainSettingDialog();

signals:
    void submit_gains(const QVector<double>& gains);

private slots:
    void on_gain_type_currentIndexChanged(int index);
    void on_gain_point_valueChanged(int value);
    void on_save_gain_button_clicked();
    void on_load_gain_button_clicked();
    void on_reset_gain_button_clicked();
    void on_clear_gain_button_clicked();
    void on_apply_button_clicked();
    void on_ok_button_clicked();
    void on_cancel_button_clicked();

private:
    Ui::AdjustGainSettingDialog *ui;

    enum class GAIN_TYPE {
      MANUAL_GAIN,
      AUTO_GAIN,
    };

    double time_scale;
    QVector<double> m_data;
    QVector<double> m_original_gains;
    QVector<double> m_base_gains;
    QVector<double> m_manual_gains;
    QVector<double> m_gains;
    double m_gain_parameter;
    double m_start_time;
    double m_end_time;
    QSettings m_settings;
    int m_point_index;
    bool m_has_submitted;

    int hit_gain_point(QMouseEvent* event) const;
    void plot_data();
    void reset_gain_points();
    void reset_gains();
    void clear_gains();
    void update_manual_gains(int gain_point_index);
    void on_mouse_press(QMouseEvent* event);
    void on_mouse_release(QMouseEvent* event);
    void on_mouse_move(QMouseEvent* event);
};

#endif // ADJUSTGAINSETTINGDIALOG_H
