#ifndef GPRSDK_H
#define GPRSDK_H
#include "GPR_Type.h"

typedef struct GPR_CalibrationMeasurement* GPR_CalibrationMeasurementHandle;
#define GPR_EMPTY_MEASUREMENT ((GPR_CalibrationMeasurementHandle)0)

/**
 * @brief 探地雷达库初始化，在程序启动时应首先调用该函数
 * @return 错误码
 */
GPR_SDK_API int __stdcall GPR_Init();

/**
 * @brief 退出SDK库，释放资源
 * @return 错误码
 */
GPR_SDK_API int __stdcall GPR_Cleanup();

/**
 * @brief 设置探地雷达的ip地址和端口号
 * @param ip 探地雷达的IP地址
 * @param port 探地雷达的端口号
 * @return 设置结果
 */
GPR_SDK_API int __stdcall GPR_SetDeviceAddress(const char* ip, uint16_t port);

/**
 * @brief 设置本机端口号
 * @param port 本机端口号
 * @return 设置结果
 */
GPR_SDK_API int __stdcall GPR_SetLocalPort(uint16_t port);

/**
 * @brief 将探地雷达复位到初始状态
 * @return 复位操作状态
 */
GPR_SDK_API int __stdcall GPR_Reset();

/**
 * @brief 获取数据采集的触发源
 * @param source [out] 触发源，外部触发或者内部触发
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_GetTriggerSource(GPR_TriggerSource* source);

/**
 * @brief 设置数据采集的触发源，外部触发还是内部触发
 * @param source 触发源
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_SetTriggerSource(GPR_TriggerSource source);

/**
 * @brief 获取触发方式
 * @param mode [out] 触发方式，单次触发或者连续触发
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_GetTriggerMode(GPR_TriggerMode* mode);

/**
 * @brief 设置触发方式，单次触发或者连续触发
 * @param mode 触发方式
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_SetTriggerMode(GPR_TriggerMode mode);

/**
 * @brief 获取内部连续触发的触发间隔
 * @param interval [out] 触发间隔，时间单位为ms
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_GetInternalTriggerInterval(int* interval);

/**
 * @brief 设置内部连续触发的触发间隔
 * @param interval 触发间隔，时间单位为ms
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_SetInternalTriggerInterval(int interval);

/**
 * @brief 获取外部触发的触发间隔
 * @param interval [out] 外部触发间隔，单位为触发信号个数
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_GetExternalTriggerInterval(int* interval);

/**
 * @brief 设置外部触发的触发间隔
 * @param interval 外部触发间隔，单位为触发信号个数
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_SetExternalTriggerInterval(int interval);

/**
 * @brief 设置多音数据选择类型
 * @param type 多音类型，范围 1~10
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_SetMultiToneDataType(int type);

/**
 * @brief 设置相位步进间隔
 * @param interval 步进间隔，范围 0~65535
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_SetPhaseStepInterval(int interval);

/**
 * @brief 启动最佳相位点扫描程序
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_StartBestPhaseScan();

/**
 * @brief 设置发射衰减
 * @param tx1 TX1 衰减，范围 0~359
 * @param tx2 TX2 衰减，范围 0~359
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_SetTxAttenuation(int tx1, int tx2);

/**
 * @brief 设置接收增益
 * @param rx1 RX1 增益，范围 0~76
 * @param rx2 RX2 增益，范围 0~76
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_SetRxGain(int rx1, int rx2);

/**
 * @brief 启动初始化配置
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_StartInitializationConfig();

/**
 * @brief 获取探地雷达工作的起始频率, 终止频率和扫描点数
 * @param frap [out] 频率范围和扫描点数
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_GetFrequencyRangeAndPoints(GPR_FrequencyRangeAndPoints* frap);

/**
 * @brief 设置探地雷达工作的起始频率, 终止频率和扫描点数
 * @param frap 频率范围和扫描点数
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_SetFrequencyRangeAndPoints(GPR_FrequencyRangeAndPoints* frap);

/**
 * @brief 获取探地雷达的单频采样频率和扫描点数
 * @param frequency [out] 起始频率值(单位为MHz)
 * @param points [out] 扫描点数
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_GetSingleFrequency(int* frequency, GPR_ScanPoints* points);

/**
 * @brief 设置探地雷达的单频采样频率和扫描点数
 * @param frequency 起始频率值(单位为MHz)
 * @param points 扫描点数
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_SetSingleFrequency(int frequency, GPR_ScanPoints points);

/**
 * @brief 获取探地雷达的中频带宽
 * @param ifbw [out] 中频带宽
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_GetIFBW(GPR_IFBW* ifbw);

/**
 * @brief 设置探地雷达的中频带宽
 * @param ifbw 中频带宽
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_SetIFBW(GPR_IFBW ifbw);

/**
 * @brief 获取采集的帧数
 * @param frames [out] 触发帧数
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_GetFrameNumbers(int* frames);

/**
 * @brief 设置采集帧数，当触发模式为单次触发GPR_TriggerMode_Once时有效
 * @param frames 触发帧数
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_SetFrameNumbers(int frames);

/**
 * @brief 开始从雷达获取数据
 * @param data_type 数据类型，可能为GPR_RAW_DATA或者GPR_CALIBRATED_DATA
 * @param channels 发射通道对应接收通道数组
 * @param channel_count 数组大小
 * @param callback 回调函数，数据类型为GPR_CALLBACK_RAW_DATA或者GPR_CALLBACK_CALIBRATED_DATA
 * @param user 用户自定义数据。
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_StartCapturingData(GPR_DataType data_type,
  GPR_TxRxChannel* channels, int channel_count, GRR_DataCallback callback, void* user);

/**
 * @brief 停止从雷达采集频域数据
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_StopCapturingData();

/**
 * @brief 开始保存采集的数据到指定文件
 * @param file_name 文件名
 * @param user_info 用户信息(可选)
 * @param antenna_fr 天线频率范围(可选)
 * @param profile 一些参数(可选)
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_StartSavingFile(const char* file_name,
  GPR_UserInfo* user_info, GPR_AntennaFrequencyRange* antenna_freq_range,
  GPR_RadarSignalProfile* profile);

/**
 * @brief 停止保存数据到文件
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_StopSavingFile();

/**
 * @brief 开始从文件中读取数据
 * @param file_name 文件名
 * @param data_type 数据类型，可能为GPR_RAW_DATA或者GPR_CALIBRATED_DATA
 * @param callback 回调函数，数据类型为GPR_CALLBACK_FILE_HEADER, GPR_CALLBACK_FILE_FRAME_HEADER,
 *        GPR_CALLBACK_RAW_DATA或者GPR_CALLBACK_CALIBRATED_DATA或者GPR_CALLBACK_TIME_DOMAIN_DATA
 * @param user 用户自定义数据。
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_StartReadingFile(const char* file_name,
  GPR_DataType data_type, GRR_DataCallback callback, void* user);

/**
 * @brief 停止从文件中读取数据
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_StopReadingFile();

/**
 * @brief 开始从雷达获取时域数据
 * @param type 时域变换算法
 * @param channels 发射通道对应接收通道数组
 * @param channel_count 数组大小
 * @param time_range 转换时域数据所用的时间范围 
 * @param window_parameter 窗口参数
 * @param callback 回调函数, 数据类型为GPR_CALLBACK_TIME_DOMAIN_DATA
 * @param user 用户自定义数据。
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_StartCapturingTimeDomainData(GPR_ConversionType type,
  GPR_TxRxChannel* channels, int channel_count, GPR_TimeRange* time_range,
  double window_parameter, GRR_DataCallback callback, void* user);

/**
 * @brief 停止从雷达获取时域数据
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_StopCapturingTimeDomainData();

/**
 * @brief 开始从文件中读取时域数据
 * @param file_name 文件名
 * @param type 时域变换算法
 * @param time_range 转换时域数据的时间范围
 * @param window_parameter 窗口参数
 * @param callback 回调函数, 数据类型为GPR_CALLBACK_FILE_HEADER, GPR_CALLBACK_FILE_FRAME_HEADER 或者 GPR_CALLBACK_TIME_DOMAIN_DATA
 * @param user 用户自定义数据。
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_StartReadingTimeDomainFile(const char* file_name,
  GPR_ConversionType type, GPR_TimeRange* time_range, double window_parameter,
  GRR_DataCallback callback, void* user);

/**
 * @brief 停止播放从文件中读取的数据
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_StopReadingTimeDomainFile();

/**
 * @brief 创建主机端口双天线测量句柄
 * @param type 测量类型
 * @return 测量句柄，由GPR_DestroyMeasurementHandle销毁
 */
GPR_SDK_API GPR_CalibrationMeasurementHandle __stdcall GPR_CreateHostPortDualAntennaMeasurementHandle(
  GPR_HostPortDualAntennaMeasurementType type);

/**
 * @brief 创建主机端口单天线测量句柄
 * @param type 测量类型
 * @return 测量句柄，由GPR_DestroyMeasurementHandle销毁
 */
GPR_SDK_API GPR_CalibrationMeasurementHandle __stdcall GPR_CreateHostPortSingleAntennaMeasurementHandle(
  GPR_HostPortSingleAntennaMeasurementType type);

/**
 * @brief 创建天线端口测量句柄
 * @param mode 测量模式
 * @param type 测量类型
 * @return 测量句柄，由GPR_DestroyMeasurementHandle销毁
 */
GPR_SDK_API GPR_CalibrationMeasurementHandle __stdcall GPR_CreateAntennaPortMeasurementHandle(
  GPR_CalibrationMode mode, GPR_AntennaPortMeasurementType type);

/**
 * @brief 销毁天线端口测量句柄
 * @param handle 测量句柄
 */
GPR_SDK_API void __stdcall GPR_DestroyMeasurementHandle(GPR_CalibrationMeasurementHandle handle);

/**
 * @brief 校准测量指定通道
 * @param tx_channel 发射通道
 * @param rx_channel 接收通道
 * @param measurement_handle 测量句柄, 由GPR_CreateXXXMeasurementHandle创建
 * @param callback 回调函数，数据类型为GPR_CALLBACK_RAW_DATA
 * @param user 用户自定义数据
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_MeasureCalibration(GPR_TxChannel tx_channel,
  GPR_RxChannel rx_channel, GPR_CalibrationMeasurementHandle handle,
  GRR_DataCallback callback, void* user);

/**
 * @brief 清除指定通道的校准测量数据
 * @param tx_channel 发射通道
 * @param rx_channel 接收通道
 * @param measurement_handle 测量句柄, 由GPR_CreateXXXMeasurementHandle创建
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_ClearMeasuredData(GPR_TxChannel tx_channel,
  GPR_RxChannel rx_channel, GPR_CalibrationMeasurementHandle handle);

/**
 * @brief 保存指定通道的测量数据到指定文件中。
 * @param tx_channel 发射通道
 * @param rx_channel 接收通道
 * @param measurement_handle 测量句柄, 由GPR_CreateXXXMeasurementHandle创建
 * @param file_name 文件名。
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_SaveMeasuredData(GPR_TxChannel tx_channel,
  GPR_RxChannel rx_channel, GPR_CalibrationMeasurementHandle handle, const char* file_name);

/**
 * @brief 从文件中加载测量数据。
 * @param file_name 文件名。
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_LoadMeasuredData(const char* file_name);

/**
 * @brief 返回指定通道是否已被测量。
 * @param tx_channel 发射通道
 * @param rx_channel 接收通道
 * @param handle 测量句柄, 由GPR_CreateXXXMeasurementHandle创建
 * @param is_measured [out] 测量状态 (true=已测量，false=未测量)
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_IsMeasured(GPR_TxChannel tx_channel,
  GPR_RxChannel rx_channel, GPR_CalibrationMeasurementHandle handle, bool* is_measured);

/**
 * @brief 在指定通道上应用校准
 * @param tx_channel 发射通道
 * @param rx_channel 接收通道
 * @param calibration 校准类型和模式
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_ApplyCalibration(GPR_TxChannel tx_channel,
  GPR_RxChannel rx_channel, GPR_Calibration calibration);

/**
 * @brief 保存指定通道的校准数据到指定文件中。
 * @param tx_channel 发射通道
 * @param rx_channel 接收通道
 * @param calibration 校准类型和模式
 * @param file_name 文件名。
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_SaveCalibration(GPR_TxChannel tx_channel,
  GPR_RxChannel rx_channel, GPR_Calibration calibration,
  const char* file_name);

/**
 * @brief 从文件中加载校准数据。
 * @param file_name 文件名。
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_LoadCalibration(const char* file_name);

/**
 * @brief 清除校准数据。
 * @param calibration 校准类型和模式
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_ClearCalibration(GPR_Calibration calibration);

/**
 * @brief 设置校准模式。
 * @param GPR_CalibrationSetup 标定设置。
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_SetCalibration(GPR_Calibration calibration);

/**
 * @brief 获取校准模式。
 * @param GPR_Calibration [out] 标定设置。
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_GetCalibration(GPR_Calibration* calibration);

/**
 * @brief 获取主机端口单天线校准件参数
 * @param parameters 参数
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_GetHostPortSingleCalibrationKitParameters(
  GPR_HostPortSingleCalibrationKitParameter* parameters);

/**
 * @brief 设置主机端口单天线校准件参数
 * @param parameters 参数
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_SetHostPortSingleCalibrationKitParameters(
  GPR_HostPortSingleCalibrationKitParameter* parameters);

/**
 * @brief 获取指定发射通道下已经测量成功的接收通道
 * @param handle [in] 测量句柄。
 * @param tx_channel [in] 发射通道
 * @param rx_channels [out] 接收通道数组
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_GetMeasuredRxChannels(GPR_CalibrationMeasurementHandle handle,
  GPR_TxChannel tx_channel, GPR_RxChannels* rx_channels);

/**
 * @brief 获取已经标定成功的发射通道
 * @param calibration [in] 校准模式。
 * @param tx_channels [out] 发射通道数组
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_GetCalibratedTxChannels(GPR_Calibration calibration,
  GPR_TxChannels* tx_channels);

/**
 * @brief 获取指定发射通道下已经标定成功的接收通道
 * @param calibration [in] 校准模式。
 * @param tx_channel [in] 发射通道
 * @param rx_channels [out] 接收通道数组
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_GetCalibratedRxChannels(GPR_Calibration calibration,
  GPR_TxChannel tx_channel, GPR_RxChannels* rx_channels);
  
/**
 * @brief 计算距离测试中的测试距离
 * @param distance [out] 距离值，单位为米
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_GetCalibrationDistance(double* distance);

/**
 * @brief 根据扫描点数查询时域缩放的最大模值
 * @param type 时域变换算法
 * @param frap 频域范围和扫描点数
 * @param window_parameter 窗口参数
 * @param time_range 时域范围
 * @param max_value [out] 时域缩放的最大模值
 * @return 操作状态, 如果没有找到对应的最大模值，返回GPR_ERROR_NO_DATA, 否则返回GPR_SUCCESS。
 */
GPR_SDK_API int __stdcall GPR_GetMaxModulusValue(GPR_ConversionType type,
  GPR_FrequencyRangeAndPoints* frap, double window_parameter,
  GPR_TimeRange* time_range, double* max_value);

/**
 * @brief 烧写
 * @param type 烧写类型
 * @param file_name 文件路径
 * @param progress_callback 烧写进度回调函数，返回烧写进度
 * @param completion_callback 烧写完成回调函数
 * @return 操作状态
 */
GPR_SDK_API int __stdcall GPR_Burn(GPR_BurningType type, const char* file_name,
  GRR_BurningProgressCallback progress_callback,
  GRR_BurningCompletionCallback completion_callback, void* user);

/**
 * @brief 是否启用触发间隔自动调整
 * @param enabled 是否启用
 */
GPR_SDK_API void __stdcall GPR_EnableAutoAdjustTriggerInterval(BOOL is_enabled);

/**
 * @brief 获取触发间隔自动调整状态
 * @return 是否启用
 */
GPR_SDK_API BOOL __stdcall GPR_IsAutoAdjustTriggerIntervalEnabled();

/**
 * @brief 是否启用中值滤波
 * @param enabled 是否启用
 */
GPR_SDK_API void __stdcall GPR_EnableMedianFilter(BOOL is_enabled);

/**
 * @brief 获取z中值滤波是否启用
 * @return 是否启用
 */
GPR_SDK_API BOOL __stdcall GPR_IsMedianFilerEnabled();

/**
 * @brief 在mask中启用某个接收通道
 * @param mask 接收通道bitmask 
 * @param channel 要启用的接收通道
 * @return 新的接收通道bitmask
 */
GPR_SDK_API uint32_t __stdcall GPR_EnableRxChannelMask(uint32_t mask, GPR_RxChannel channel);

/**
 * @brief 在mask中禁用某个接收通道
 * @param mask 接收通道bitmask 
 * @param channel 要禁用的接收通道
 * @return 新的接收通道bitmask
 */
GPR_SDK_API uint32_t __stdcall GPR_DisableRxChannelMask(uint32_t mask, GPR_RxChannel channel);

/**
 * @brief 检测某个接收通道是否被启用
 * @param mask 接收通道bitmask 
 * @param channel 用于检测的接收通道
 * @return 如果channel启用，返回true
 */
GPR_SDK_API BOOL __stdcall GPR_IsRxChannelMaskEnabled(uint32_t mask, GPR_RxChannel channel);

/**
 * @brief 返回某个接收通道在所有启用的接收通道中的索引，例如GPR_RX_D在GPR_RX_B和GPR_RX_D(0b0101)都启用的情况下，返回1
 * @param mask 接收通道bitmask 
 * @param channel 用于检测的接收通道
 * @return 索引值，如果接收通道channel未启用，返回-1
 */
GPR_SDK_API int __stdcall GPR_GetRxChannelIndex(uint32_t mask, GPR_RxChannel channel);

/**
 * @brief 返回第n个启用的接收通道, 例如GPR_RX_D在GPR_RX_B和GPR_RX_D(0b0101)都启用的情况下，n是1，则返回GPR_RX_D
 * @param mask 接收通道bitmask 
 * @param n 第n个启用的接收通道
 * @return 接收通道, 如果mask中含有的启用接收通道数目小于n，则返回GPR_RX_NONE 
 */
GPR_SDK_API GPR_RxChannel __stdcall GPR_GetNthEnabledRxChannel(uint32_t mask, int n);

/**
 * @brief 返回启用的接收通道的数目，例如GPR_RX_B和GPR_RX_D(0b0101)都启用的情况下，一共有2个启用的接收通道
 * @param mask 接收通道bitmask 
 * @return 启用的接收通道的数目
 */
GPR_SDK_API int __stdcall GPR_GetEnabledRxChannelCount(uint32_t mask);

#endif // GPRSDK_H
