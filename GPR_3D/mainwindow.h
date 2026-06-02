#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <memory>
#include <limits>
#include <QButtonGroup>
#include <QMainWindow>
#include <QSettings>
#include <QToolButton>
#include <QMainWindow>
#include <QTimer>
#include <QElapsedTimer>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QQueue>
#include <atomic>
#include "realtimeacquisition.h"
#include "filereplayview.h"
#include "GPR_SDK.h"
#include "global.h"
#include "image2dplot.h"
#include "timedomainplot.h"
#include "realtimeprocessingsettingview.h"
#include "adjustgainsettingdialog.h"
#include "three_d_volume_view.h"
#include "three_d_volume_dialog.h"
#include "backgroundsubtraction.h"
#include "rangeslider.h"
#include "v2hyperboladetector.h"

class QKeyEvent;
class QResizeEvent;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

struct MineGroundTrendFlattenState {
    bool initialized = false;
    bool hold_state = false;
    int hold_count = 0;
    double previous_pick = 1.0;
    QQueue<double> init_picks;
    QQueue<double> accepted_picks;
    QQueue<double> recovery_raw_picks;
    QQueue<bool> recovery_valid;
};

struct V2RealtimeTarget {
    V2HyperbolaBox best_box;
    V2HyperbolaBox last_box;
    V2HyperbolaBox union_box;
    double peak_score = 0.0;
    double last_score = 0.0;
    int first_seen_track = 0;
    int last_seen_track = 0;
    int hit_count = 0;
    int miss_count = 0;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void init_dock_view();
    void init_tool_bar();
    void init_status_bar();
    void init_setting_view();
    void init_plots();
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

signals:
    void display_data_ready(const QVector<double>& data,bool is_start);
    void display_data_ready_topview(const QVector<double>& data,bool is_start);
    void display_data_ready_sideview(const QVector<double>& data,bool is_start);
    void processing_data_ready(const QVector<double>& data, bool is_last = false);
    void processing_data_ready_topview(const QVector<double>& data, bool is_last = false);
    void processing_data_ready_sideview(const QVector<double>& data, bool is_last = false);
    void time_domain_data_ready(const QVector<double>& x, const QVector<double>& y);

private slots:
    void on_display_data(const QVector<QVector<GPR_Complex>>& data,bool is_last);
    void on_display_data_batch(const QVector<QVector<QVector<GPR_Complex>>>& data_batch);
    void on_processing_data_ready(const QVector<double>& data, bool is_last);
    void on_processing_data_ready_topview(const QVector<double>& data, bool is_last);
    void on_processing_data_ready_sideview(const QVector<double>& data, bool is_last);
    void on_replay(const GPR_FileHeader& file_header,int channel_number);
    void on_replay_display_channel_changed(int channel);
    void onPlotDoubleClick(int x,int y,QString m_plot_id);
    void onPlotDoubleClick_topview(int x,int y,QString m_plot_id);
    void onPlotDoubleClick_sideview(int x,int y,QString m_plot_id);
    void on_eliminate_background(bool is_open);
    void on_adjust_gain();
    void on_set_volume_data();

    void on_auto_adjust_gain_combo_box_currentIndexChanged(int index);
    void on_mean_background_combo_box_currentIndexChanged(int index);
    void on_linear_gain_changed(double value);
    void on_topview_avg_radius_changed(int value);
    void on_topview_avg_radius_released();
    void on_topview_range_changed();
    void on_topview_point_range_changed(int lower, int upper);
    void update_topview_avg_preview();
    void on_main_axis_rect_changed(int top, int bottom, int height);
    void on_map_k_changed(double value);
    void on_mine_ground_trend_flatten_changed(bool enabled);
    void on_v2_hyperbola_detector_changed(bool enabled);
    void update_range_avg_display();
    void alignAngleSliderWithYAxis();
    void alignAngleSliderWithYAxis(int top, int bottom, int height);


private:
    QDockWidget* m_dock_widget;
    QWidget* m_dock_content_widget = nullptr;
    QVBoxLayout* m_dock_content_layout = nullptr;
    QWidget* m_current_dock_view = nullptr;
    QToolButton* make_tool_button(const QIcon& icon, const QString& text, QWidget* connected_view);
    QButtonGroup m_button_group;
    realtimeacquisition* m_real_time_acquisition;
    FileReplayView* m_file_replay_view;
    RealTimeProcessingSettingView* m_real_time_processing_setting_view;
    Image2DPlot* m_image2d_plot;
    Image2DPlot* m_image2d_plot_topview;
    Image2DPlot* m_image2d_plot_sideview;
    TimeDomainPlot* m_time_domain_plot;
    Three_D_Volume_view* m_Three_D_Volume_view;
    AdjustGainSettingDialog* m_adjust_gain_setting_dialog;
    Three_D_Volume_Dialog* m_three_d_volume_dialog;
    BackgroundSubtraction m_background_subtraction;
    QToolButton* m_process_btn = nullptr;
    QToolButton* m_side_panel_toggle_button = nullptr;

    QLabel* m_track_number_label;
    QLabel* m_track_number_title_label = nullptr;
    QWidget* m_status_info_widget = nullptr;
    QProgressBar* m_replay_progress_bar = nullptr;
    int m_track_number;
    Ui::MainWindow *ui;
    // QVector<QVector<double>> m_raw_data;
    QVector<QVector<double>> m_strength_data;
    QVector<QVector<double>> m_background;
    QVector<QVector<double>> m_mean_background;
    QVector<QVector<double>> m_base_data;
    QVector<double> m_gains;
    QVector<QVector<QVector<double>>> m_raw_data;
    QVector<QVector<QVector<double>>> m_raw_data_all_channel;
    QVector<double> m_previous_frame;
    // QVector<double> m_background;
    double m_gain_parameter;
    QSettings m_settings;
    // QLabel* m_track_number_label;
    QLabel* m_switch_time_interval;
    // AdjustGainSettingDialog* m_adjust_gain_setting_dialog;
    // int m_track_number;
    int m_display_tx;
    int m_display_rx;
    int m_display_point;
    int m_display_channel;
    int m_points;
    int m_channel_number;
    qint64 m_display_scale_sample_count = 0;
    double m_display_scale_mean = 0.0;
    double m_display_scale_m2 = 0.0;
    double m_display_scale_clip = 1.0;
    double m_display_scale_applied_clip = 1.0;
    QElapsedTimer m_display_scale_apply_timer;
    bool m_is_eliminate_background;
    bool m_is_mean_background;
    bool m_is_mine_ground_trend_flatten_enabled;
    bool m_is_v2_hyperbola_detector_enabled;
    bool m_realtime_processing_delay_pending = false;
    bool m_pending_realtime_eliminate_background = false;
    bool m_pending_realtime_mean_background = false;
    bool m_pending_realtime_ground_trend_flatten = false;
    bool m_pending_realtime_v2_hyperbola_detector = false;
    double m_linear_gain_initial;
    double m_linear_gain_slope_per_ns;
    int m_mean_background_count;
    bool m_is_display_data;
    QTimer* m_topview_avg_timer = nullptr;
    std::atomic<int> m_topview_avg_task_id{0};
    bool m_topview_overlay_enabled = false;
    std::atomic<bool> m_topview_rebuild_in_progress{false};
    QVector<QVector<double>> m_topview_track_buffer;
    bool m_topview_track_emitted_first = false;
    RangeSlider* m_topview_range_slider = nullptr;
    QWidget* m_topview_range_container = nullptr;
    QSpacerItem* m_topview_range_top_spacer = nullptr;
    QSpacerItem* m_topview_range_bottom_spacer = nullptr;
    std::atomic<int> m_topview_range_low{0};
    std::atomic<int> m_topview_range_high{0};
    bool m_is_replay_loading = false;
    int m_replay_total_frames = 0;
    int m_cache_block_index = 0;
    QVector<QVector<double>> m_replay_bscan_buffer;
    QVector<QVector<double>> m_replay_topview_buffer;
    QVector<QVector<QVector<double>>> m_replay_v2_channel_buffer;
    std::atomic<int> m_processing_rebuild_task_id{0};
    QVector<QVector<QVector<double>>> m_v2_hyperbola_history;
    int m_v2_hyperbola_history_start_track = 0;
    int m_v2_hyperbola_next_output_track = 0;
    std::atomic<bool> m_v2_hyperbola_task_in_progress{false};
    std::atomic<int> m_v2_hyperbola_task_id{0};
    bool m_v2_hyperbola_flush_requested = false;
    int m_v2_hyperbola_last_window_start = -1;
    QVector<V2RealtimeTarget> m_v2_hyperbola_active_targets;
    QVector<V2HyperbolaBox> m_v2_hyperbola_committed_boxes;
    bool m_last_session_was_realtime = false;
    bool m_side_panel_collapsed = false;
    bool m_bscan_axis_annotations_visible = false;
    QVector<MineGroundTrendFlattenState> m_mine_ground_trend_flatten_states;
    // bool m_is_adjust_gain;
    void clear(int points,int all_channel_count);
    void on_realtime_play(std::vector<GPR_TxRxChannel> m_tx_rx_channels);
    void on_submit_gains(const QVector<double>& gains);
    void createAndSetVolumeData();
    void exportDataToCSV();
    void on_scale_option_update(ScaleOption option);
    void rms_gain(const QVector<double>& data);
    void F_K();
    void updateTopViewFrame(const QVector<QVector<double>>& frame);
    void reset_display_scale_reference();
    void update_display_scale_reference(const QVector<double>& raw_trace);
    void apply_display_scale_clip(bool force);
    void rebuild_display_from_cache();
    void finish_replay_loading();
    void set_time_transform_selector_enabled(bool enabled);
    void update_linear_gains();
    void reset_mine_ground_trend_flatten();
    QVector<double> apply_mine_ground_trend_flatten_to_trace(
        int channel,
        const QVector<double>& trace,
        QVector<MineGroundTrendFlattenState>* state_buffer = nullptr);
    void reset_v2_hyperbola_history();
    int apply_realtime_v2_hyperbola_backfill(bool flush_all);
    void apply_realtime_v2_hyperbola_box(const V2HyperbolaBox& box);
    void render_realtime_v2_hyperbola_range(int begin_track, int end_track);
    void set_mine_processing_switches_without_rebuild(bool background_enabled, bool ground_enabled, bool v2_enabled);
    void activate_delayed_realtime_processing_if_ready();
    double current_dt_ns() const;
    QVector<double> build_bscan_trace(const QVector<double>& trace) const;
    QVector<double> build_topview_column(const QVector<QVector<double>>& channel_traces) const;
    void update_side_panel_toggle_button_position();
    void toggle_side_panel();
};
#endif // MAINWINDOW_H
