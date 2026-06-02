#ifndef GPR_TYPE_H
#define GPR_TYPE_H
#include <stdint.h>
#include <stdbool.h>

#if (defined(_WIN32)) //windows
#ifdef GPRSDKDLL
#define GPR_SDK_API extern "C" __declspec(dllexport)
#else
#define GPR_SDK_API extern "C" __declspec(dllimport)
#endif //GPRSDKDLL
#elif defined(__linux__) || defined(__APPLE__) //linux
#define __stdcall 
#define GPR_SDK_API extern "C"
#endif

#define TRUE 1
#define FALSE 0
typedef int BOOL;

// 错误码
#define GPR_SUCCESS                             0     // 没有错误
#define GPR_ERROR_NOT_INIT                      1     // 没有初始化库，即没有调用GPR_Init
#define GPR_ERROR_INIT_FAILED                   2     // 初始化失败
#define GPR_ERROR_CLEAN_UP_FAILED               3     // 释放资源失败
#define GPR_ERROR_TIMEOUT                       4     // 通信超时
#define GPR_ERROR_COMMUNICATION_FAILED          5     // 通讯失败
#define GPR_ERROR_NO_DATA                       6     // 没有数据
#define GPR_ERROR_INVALID_PARAMETER             7     // 无效参数
#define GPR_ERROR_INVALID_FILE_NAME             8     // 无效文件名
#define GPR_ERROR_OPEN_FILE                     9     // 打开文件失败
#define GPR_ERROR_FILE_ALREADY_OPEN             10    // 文件已经被打开
#define GPR_ERROR_FILE_NOT_FOUND                11    // 没有找到文件
#define GPR_ERROR_READ_END_OF_FILE              12    // 读取到文件结尾
#define GPR_ERROR_READ_FILE                     13    // 读取文件失败
#define GPR_ERROR_INVALID_FILE                  14    // 无效文件
#define GPR_ERROR_INVALID_DATA_FORMAT           15    // 无效数据格式
#define GPR_ERROR_INVALID_CALIBRATION           16    // 无效的标定
#define GPR_ERROR_IS_READING                    17    // 正在读取，需要先停止读取再开始
#define GPR_ERROR_IS_SAVING                     18    // 正在保存，需要先停止保存再开始
#define GPR_ERROR_NOT_MATCH_CALIBRATION         19    // 数据长度与标定数据长度不匹配
#define GPR_ERROR_INVALID_COMMAND               20    // 无效的命令
#define GPR_ERROR_INVALID_DATA                  21    // 无效的数据
#define GPR_ERROR_BUFFER_SIZE_NOT_ENOUGH        22    // 缓冲区不够大
#define GPR_ERROR_READ_DATA_END                 23    // 采集数据结束
#define GPR_ERROR_BURNING_FAILED                24    // 烧写失败
#define GPR_ERROR_IS_BURNING                    25    // 正在烧写，不能进行另一次烧写
#define GPR_ERROR_INCONSISTENT_TX_CHANNEL       26    // 不一致的发射通道 
#define GPR_ERROR_INCONSISTENT_SCAN_POINT       27    // 不一致的扫描点数 
#define GPR_ERROR_INCONSISTENT_FREQUENCY        28    // 不一致的频率 
#define GPR_ERROR_NO_CALIBRATED_CHANNEL         29    // 没有标定的通道
#define GPR_ERROR_SIZE_TOO_LARGE                30    // 读取数据太大
#define GPR_ERROR_SIZE_TOO_SMALL                31    // 读取数据太小
#define GPR_ERROR_CONVERT_DATA                  32    // 转换数据错误
#define GPR_ERROR_PACKET_TOO_SMALL              33    // 接收的数据太小
#define GPR_ERROR_FRAME_TOO_SMALL               34    // 接收的帧数据太小
#define GPR_ERROR_INVALID_CHANNEL_PARAMETER     35    // 没有设置有效的通道
#define GPR_ERROR_WRITE_FILE                    36    // 写文件失败
#define GPR_ERROR_MEASUREMENT_DATA_INCOMPLETE   37    // 标定测量数据不完整
#define GPR_ERROR_INVALID_MEASUREMENT_HANDLE    38    // 无效的测量句柄
#define GPR_ERROR_INVALID_MEASURED_DATA         39    // 无效的测量数据
#define GPR_ERROR_INCONSISTENT_MEASUREMENT      40    // 不一致的测量
#define GPR_ERROR_INVALID_MEASURE_FILE          41    // 无效的测量文件
#define GPR_ERROR_INVALID_CALIBRATION_FILE      42    // 无效的标定文件
#define GPR_ERROR_NO_CALIBRATION                43    // 指定的播放通道没有被标定
#define GPR_ERROR_OTHERS                        100   // 其它错误

const int GPR_TX_CHANNEL_NUMBER = 8;
const int GPR_RX_CHANNEL_NUMBER = 7;
const int GAINS_NUMBER = 2;

// 发射通道
typedef enum {
  GPR_TX_NONE = -1,
  GPR_TX_1 = 0,
  GPR_TX_2,
  GPR_TX_3,
  GPR_TX_4,
  GPR_TX_5,
  GPR_TX_6,
  GPR_TX_7,
  GPR_TX_8
} GPR_TxChannel;

// 接收通道
typedef enum {
  GPR_RX_NONE = -1,
  GPR_RX_B = 0,
  GPR_RX_C,
  GPR_RX_D,
  GPR_RX_E,
  GPR_RX_F,
  GPR_RX_G,
  GPR_RX_H
} GPR_RxChannel;

// 扫描点数
typedef enum {
  GPR_POINT_128,
  GPR_POINT_256,
  GPR_POINT_512,
  GPR_POINT_1024,
} GPR_ScanPoints;

// 中频带宽
typedef enum {
  GPR_IFBW_1HZ = 0,
  GPR_IFBW_2HZ,
  GPR_IFBW_5HZ,
  GPR_IFBW_10HZ,
  GPR_IFBW_20HZ,
  GPR_IFBW_50HZ,
  GPR_IFBW_100HZ,
  GPR_IFBW_200HZ,
  GPR_IFBW_500HZ,
  GPR_IFBW_1KHZ,
  GPR_IFBW_3KHZ,
  GPR_IFBW_5KHZ,
  GPR_IFBW_10KHZ,
  GPR_IFBW_15KHZ,
  GPR_IFBW_20KHZ,
  GPR_IFBW_30KHZ,
  GPR_IFBW_50KHZ,
  GPR_IFBW_100KHZ,
  GPR_IFBW_200KHZ,
  GPR_IFBW_600KHZ,
  GPR_IFBW_1MHZ,
  GPR_IFBW_2MHZ
} GPR_IFBW;

// 采集数据类型
typedef enum {
  GPR_RAW_DATA_TYPE = 1,          // IQ数据
  GPR_CALIBRATED_DATA_TYPE       // 校准后的数据
} GPR_DataType;

// 回调函数数据类型
typedef enum {
  GPR_CALLBACK_NONE_DATA = 0,     // 没有任何数据
  GPR_CALLBACK_DATA_HEADER,       // 数据头，参考数据结构GPR_DataHeader
  GPR_CALLBACK_RAW_DATA,          // 原始数据，参考数据结构GPR_IQ_DATA
  GPR_CALLBACK_CALIBRATED_DATA,   // 校准之后的数据，GPR_ComplexArray数组，参考数据结构GPR_ComplexArray
  GPR_CALLBACK_TIME_DOMAIN_DATA,  // 时域数据，GPR_ComplexArray数组，存储多道时域数据，
                                  // 每一个GPR_ComplexArray存储了一个道时域数据，参考数据结构GPR_ComplexArray
  GPR_CALLBACK_FILE_HEADER,       // 文件头, 参考数据结构GPR_FileHeader 
  GPR_CALLBACK_FILE_DATA_HEADER   // 文件中的帧头, 参考数据结构GPR_FileDataHeader 
} GPR_CallbackDataType;

// 触发源
typedef enum {
  GPR_TRIGGER_SOURCE_INTERNAL = 0,   // 内部触发
  GPR_TRIGGER_SOURCE_EXTERNAL        // 外部触发
} GPR_TriggerSource;

// 内部触发方式
typedef enum {
  GPR_TRIGGER_MODE_ONCE = 0,   // 单次触发，发送命令后只触发一次
  GPR_TRIGGER_MODE_CONTINUE    // 连续触发
} GPR_TriggerMode;

// 变换函数类型
typedef enum {
  GPR_IFFT = 0,        // 逆傅里叶变换
  GPR_TIME_SCALE       // 时域伸缩
} GPR_ConversionType;

// 校准类型
typedef enum {
  GPR_HOST_PORT_CALIBRATION = 0,     // 主机端口校准
  GPR_ANTENNA_PORT_CALIBRATION       // 天线端口校准
} GPR_CalibrationType;

// 校准模式
typedef enum {
  GPR_DUAL_ANTENNA_MODE = 0,      // 双天线模式
  GPR_SINGLE_ANTENNA_MODE        // 单天线模式
} GPR_CalibrationMode;

// 主机端口校准双天线模式的测量类型
typedef enum {
  GPR_ATTENUATION_MEASUREMENT      // 衰减测量
} GPR_HostPortDualAntennaMeasurementType;

// 主机端口校准单天线模式的测量类型
typedef enum {
  GPR_OPEN_MEASUREMENT = 0,        // Open
  GPR_SHORT_MEASUREMENT,           // Short
  GPR_LOAD_MEASUREMENT             // Load
} GPR_HostPortSingleAntennaMeasurementType;

// 天线端口校准的测量类型
typedef enum {
  GPR_DISTANCE_BASED_MEASUREMENT = 0,   // 距离测量
  GPR_NEAR_FIELD_MEASUREMENT,           // 贴近测量
  GPR_SCENARIO_BASED_MEASUREMENT        // 场景测量
} GPR_AntennaPortMeasurementType;

// 烧写类型
typedef enum {
  GPR_INITIAL_BURNING = 0,        // 第一次烧写
  GPR_SIMPLE_BURNING,             // 简单烧写，不含防断电功能
  GPR_NORMAL_BURNING              // 正常烧写，含防掉断电功能
} GPR_BurningType;

typedef enum {
    GPR_VERSION_NONE = 0,        // 无版本号
    GPR_VERSION_V4,             // 9915v4版本
    GPR_VERSION_V5,            // 9915v5版本
}GPR_BoardFlag;

typedef struct {
  int i;               // In-phase component
  int q;               // Quadrature component
} GPR_IQ;

// IQ数据
typedef struct {
  GPR_IQ tx_iq;                     // 发送通道IQ
  GPR_IQ* rx_iqs;                    // 接收通道IQ
  int rx_channel_count;             // 接收通道IQ数量
} GPR_IQData;

// 复数
typedef struct {
  double real;    // 实部
  double imag;    // 虚部
} GPR_Complex;

// 复数数组
typedef struct {
  GPR_Complex* data;   // 复数数组
  int count;          // 复数个数
} GPR_ComplexArray;

// 发射通道数组
typedef struct {
  GPR_TxChannel channels[GPR_TX_CHANNEL_NUMBER]; //发射通道数组，最大长度为GPR_TX_CHANNEL_NUMBER
  int count;         // 发射通道个数
} GPR_TxChannels;

// 接收通道数组
typedef struct {
  GPR_RxChannel channels[GPR_RX_CHANNEL_NUMBER]; //接收通道数组，最大长度为GPR_RX_CHANNEL_NUMBER
  int count;         // 接收通道个数
} GPR_RxChannels;

// 一个发射通道对应的接收通道
typedef struct {
  GPR_TxChannel tx_channel;
  uint32_t rx_channels;   // 使用bitmask, 每一位代表一个通道，从低位到高位依次是GPR_RX_B, GPR_RX_C, ...
                        // 可以使用一系列辅助函数来进行位操作，例如GPR_EnableRxChannelMask，GPR_DisableRxChannelMask
} GPR_TxRxChannel;

// GPS信息
typedef struct {
  uint8_t status;
  int pps_count;
  int64_t timestamp;
} GPR_GPSInfo;

typedef struct {
    double longitude;
    double latitude;
} GPR_Rtk;

// 数据头
typedef struct {
  uint32_t frame_id;            // 道ID
  GPR_TxRxChannel tx_rx_channel;  // 指定数据对应的发射通道和接收通道
  GPR_GPSInfo gps_info;           // 存储GPS相关信息
  bool rx_mark;
  GPR_BoardFlag version;
  GPR_Rtk rtk_info;
} GPR_DataHeader;

// 频率范围和扫描点数
typedef struct {
  int start_frequency;     // 起始频率值(单位为MHz)
  int stop_frequency;      // 终止频率值(单位为MHz)
  GPR_ScanPoints points;   // 扫描点数
} GPR_FrequencyRangeAndPoints;

// 天线频率范围
typedef struct {
  int start_frequency;     // 起始频率值(单位为MHz)
  int stop_frequency;      // 终止频率值(单位为MHz)
} GPR_AntennaFrequencyRange;

// 用户信息
typedef struct {
  char company_name[30];
  char location[30];
} GPR_UserInfo;

// 一些参数 
typedef struct {
  int16_t start_length;     // 起始里程点
  int16_t stop_length;       // 终止里程点
  double dielectric;          // 介电常数
  double max_modulus_value;   // 最大模值
  double gains[GAINS_NUMBER];            // 增益参数数组
} GPR_RadarSignalProfile;

// 时间范围
typedef struct {
  int start_time;             // 起始时间(单位为ns)
  int end_time;               // 结束时间(单位为ns)
} GPR_TimeRange;

// 标定
typedef struct {
  GPR_CalibrationType type;
  GPR_CalibrationMode mode;
} GPR_Calibration;

// Open测试参数
typedef struct {
  double Z0;
  double delay;
  double loss;
  double C0;
  double C1;
  double C2;
  double C3;
} GPR_OpenCalibrationKitParameter;

// Short测试参数
typedef struct {
  double Z0;
  double delay;
  double loss;
  double L0;
  double L1;
  double L2;
  double L3;
} GPR_ShortCalibrationKitParameter;

// Load测试参数
typedef struct {
  double resistance;
  double Z0;
  double delay;
  double C_parallel;
  double L_series;
} GPR_LoadCalibrationKitParameter;

// 主机端口单天线模式的测量校准件
typedef struct {
  GPR_OpenCalibrationKitParameter m_open_parameter;
  GPR_ShortCalibrationKitParameter m_short_parameter;
  GPR_LoadCalibrationKitParameter m_load_parameter;
} GPR_HostPortSingleCalibrationKitParameter;

// 文件头信息
typedef struct {
  GPR_DataType data_type;        // 数据类型
  uint32_t frames;                 // 道数
  GPR_FrequencyRangeAndPoints frequency_range_points;
  GPR_IFBW ifbw;
  GPR_AntennaFrequencyRange antenna_frequency_range;
  GPR_TimeRange time_range;
  GPR_TriggerSource trigger_source;
  GPR_TriggerMode trigger_mode;
  GPR_UserInfo user_info;
  GPR_RadarSignalProfile profile;
  int trigger_interval;
  int trigger_frame_number;
  double window_parameter;
  int64_t create_timestamp;           // 时间戳
  int64_t modify_timestamp;
} GPR_FileHeader;

// 文件数据头信息
typedef struct {
  uint16_t flag;                     // 数据标记
  GPR_TxRxChannel tx_rx_channel;     // 采集数据的相关通道
  GPR_GPSInfo gps_info;              // GPS信息
  bool rx_mark;
} GPR_FileDataHeader;

/**
 * 数据回调函数类型。
 * @param error_code: 错误码
 * @param type: 回调数据类型
 * @param buffer: 数据缓冲区，数据类型由type指定
 * @param buffer_size: 缓冲区长度
 * @param user: 用户数据
 */
typedef void (__stdcall *GRR_DataCallback)(int error_code, GPR_CallbackDataType type,
  void* buffer, int buffer_size, void* user);

/**
 * 烧写进度回调函数类型。
 * @param progress: 进度
 * @param user: 用户数据
 */
typedef void (__stdcall* GRR_BurningProgressCallback)(float progress, void* user);

/**
 * 烧写完成结果回调函数类型。
 * @param error: 是否有错误发生，参考错误码
 * @param user: 用户数据
 */
typedef void (__stdcall* GRR_BurningCompletionCallback)(int error, void* user);

#endif // GPR_TYPE_H
