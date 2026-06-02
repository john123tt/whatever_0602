#ifndef SETTING_H
#define SETTING_H

namespace GPR {
    const auto ip_key = "RadarOperation/ip";
    const auto port_key = "RadarOperation/port";
    const auto data_directory_key = "SystemManagementSetting/data_directory";
    const auto local_port_key = "SystemManagementSetting/port";
    const auto save_frame_data_key = "SystemManagementSetting/save_frame_data";
    const auto scale_option_key = "RealTimeProcessingSettingView/scale_option";
    const auto replay_file_key = "Replay/replay_file";
    const auto start_time_key = "Time/start_time";
    const auto end_time_key = "Time/end_time";
    const auto window_parameter_key = "Time/window_parameter";
    const auto max_modulus_value_key = "Time/max_modulus_value";
    const auto gain_parameter1_key = "Time/gain_parameter1";
    const auto gain_parameter2_key = "Time/gain_parameter2";
    const auto company_name_key = "User/company_name";
    const auto testing_location_key = "User/testing_location";
    const auto antenna_start_frequency_key = "Antenna/start_frequency";
    const auto antenna_stop_frequency_key = "Antenna/stop_frequency";
    const auto save_begin_track_key = "Save/begin_track";
    const auto save_end_track_key = "Save/end_track";
    const auto last_loaded_directory_key = "Calibration/loaded_directory";
    const auto gain_point_key = "Gain/gain_point";
    const auto gain_file_key = "Gain/gain_file_key";
    const auto points_key = "realtimeacquisition/points_key";
    const auto channel_number = "realtimeacquisition/channel_number";
    const auto temp_file_key = "Image2DPlot/temp_directory_key";
    const auto time_domain_transform_key = "RealTimeProcessingSettingView/time_domain_transform";
    const auto mine_ground_trend_flatten_enabled_key = "RealTimeProcessingSettingView/mine_ground_trend_flatten_enabled";
    const auto v2_hyperbola_detector_enabled_key = "RealTimeProcessingSettingView/v2_hyperbola_detector_enabled";
}

#endif // SETTING_H
