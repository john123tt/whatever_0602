#ifndef FILEREPLAYVIEW_H
#define FILEREPLAYVIEW_H
#include <QSettings>
#include <QWidget>
#include <GPR_Type.h>
#include "global.h"
#include <QElapsedTimer>

class QDialog;

namespace Ui {
class FileReplayView;
}

class FileReplayView : public QWidget
{
    Q_OBJECT

public:
    explicit FileReplayView(QWidget *parent = nullptr);
    ~FileReplayView();

    void realtime_play_start();

    bool is_replaying() const;

signals:
    void data_ready(const QVector<QVector<GPR_Complex>>& data, bool is_last = false);
    void data_batch_ready(const QVector<QVector<QVector<GPR_Complex>>>& data_batch);
    void replay(const GPR_FileHeader& file_header,int channel_number);
    void error_occur(int error_code);
    void replay_state_changed(bool running);
    void display_channel_changed(int channel);

private slots:
    void on_error(int error_code);

    void on_replay_file_browse_clicked();

    void on_replay_button_toggled(bool checked);

    void on_replay_file_path_textChanged(const QString& path);

    void on_launch_channel_combo_box_currentIndexChanged(int index);

    void on_receive_channel_combo_box_currentIndexChanged(int index);

private:
    Ui::FileReplayView *ui;
    QDialog* m_header_dialog = nullptr;
    QSettings m_settings;
    GPR_FileHeader m_header;
    GPR_FileDataHeader m_data_header;
    QVector<QVector<GPR_Complex>> m_all_channel_data;
    QVector<QVector<QVector<GPR_Complex>>> m_replay_batch_buffer;

    void on_replay(const GPR_FileHeader& file_header);
    void reset_replay_state();
    void flush_replay_batch();
    void handle_replay_frame(QVector<QVector<GPR_Complex>> data);
    void update_detected_channel_count(int total_channels);
    int m_sequence_counter;
    QMap<int, QVector<QVector<GPR_Complex>>> m_sequence_buffer;
    QVector<QVector<GPR_Complex>> m_pending_data;
    int m_frame;
    // 当前帧缓存（key = tx 通道）
    QMap<GPR_TxChannel, QVector<QVector<GPR_Complex>>> m_frame_buffer;
    // 当前帧开始时间
    QElapsedTimer m_frame_timer;
    // 是否已开始接收一帧
    bool m_frame_active = false;
    // 期望的 TX 数量（⚠️可配置）
    int m_expected_tx_count = 5;
    // 超时时间（毫秒，⚠️可配置）
    int m_frame_timeout_ms = 30;



    bool m_calibrating = true;
    QSet<GPR_TxChannel> m_seen_tx_once;
    QVector<GPR_TxChannel> m_tx_order;   // 第一帧学到的 TX 顺序（非常关键）
    QSet<GPR_TxChannel> m_tx_set;        // 快速判断是否齐全
    bool m_using_sdk_time_domain = false;
    int m_detected_channel_count = 0;
};

#endif // FILEREPLAYVIEW_H
