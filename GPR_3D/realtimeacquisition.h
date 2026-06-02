#ifndef REALTIMEACQUISITION_H
#define REALTIMEACQUISITION_H

#include <QSettings>
#include <QWidget>
#include <GPR_Type.h>
#include <QQueue>
#include <QMap>
#include <QVector>


namespace Ui {
class realtimeacquisition;
}

class realtimeacquisition : public QWidget
{
    Q_OBJECT

public:
    explicit realtimeacquisition(QWidget *parent = nullptr);
    ~realtimeacquisition();

signals:
    void error_occur(int error_code);
    void data_ready(const QVector<QVector<GPR_Complex>>& data,bool is_last);
    void realtime_play(std::vector<GPR_TxRxChannel> m_tx_rx_channels);
    void raw_frame_arrived(GPR_DataHeader header,QVector<QVector<GPR_Complex>> data);
    void capture_state_changed(bool running);

private slots:
    void on_One_Click_Configuration_button_clicked();
    void on_error(int error_code);
    void on_Start_Capture_Data_button_toggled(bool checked);

    void on_save_file_check_box_clicked(bool checked);
    void on_raw_frame_arrived(const GPR_DataHeader& header,QVector<QVector<GPR_Complex>> data);

private:
    Ui::realtimeacquisition *ui;
    QSettings m_settings;
    GPR_DataHeader m_data_header;
    void update_calibrated_channels();
    int multichannel_number;
    std::vector<GPR_TxRxChannel> m_tx_rx_channels;
    QVector<QVector<GPR_Complex>> m_all_channel_data;
    void enable_view(bool is_enabled);
    int m_track_number;
    // realtimeacquisition.h
    QSet<int> m_seen_tx;
    QMap<int, QVector<QVector<GPR_Complex>>> m_curr_frame;
    QSet<int> m_expected_tx;
    uint32_t m_current_frame_id = UINT32_MAX;

    QQueue<GPR_DataHeader> m_header_queue;
    GPR_FrequencyRangeAndPoints m_frequency_range_points{};
    bool m_using_sdk_time_domain = false;
};

#endif // REALTIMEACQUISITION_H
