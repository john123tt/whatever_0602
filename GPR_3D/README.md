# GPR_3D

频域体制探雷实时目标清晰化算法设计及系统实现。

本项目是面向浅埋危险目标探测的频域探地雷达（GPR）上位机软件。软件围绕“非接触、便携、实时、可复查”的实验需求，打通频域雷达采集板、近地探测天线、工控主机和实时显示处理链路，实现设备配置、实时采集、数据保存、文件回放、B-scan 显示、背景抑制、趋势地面拉直、目标区域高亮和 3D 体数据显示等功能。

当前阶段对应中期答辩材料中的“已完成系统”：已经具备硬件-软件-算法的最小闭环，可用于沙箱和浅埋场景的联调采集、算法验证、结果复核与后续整机装配测试。

## 项目定位

### 研究目标

- 构建一套手持式频域 GPR 探测终端的上位机采集与处理软件。
- 将频域雷达采集数据转换为可解释、可复查的 B-scan 图像。
- 对原始 B-scan 中的稳定背景、地面响应起伏和弱目标响应问题进行实时增强。
- 在上位机中接入背景消除、增益控制、趋势地面拉直和窗口谷值双曲检测算法，形成现场可操作的验证平台。

### 应用场景

- 频域体制探地雷达采集链路联调。
- 浅埋目标、低金属或非金属目标的实验验证。
- B-scan 图像实时显示与处理效果对比。
- 历史采集数据回放、复核与参数调试。
- 毕设、论文、答辩和竞赛材料中的系统展示。

> 注意：本软件面向科研和教学实验，不应替代专业排爆设备或未经授权的现场处置流程。

## 当前完成情况

已完成的核心能力如下：

- 频域雷达板与上位机 SDK 接入。
- 实时采集、停止采集、采集道数统计和状态显示。
- 可选保存采集数据，为后续离线分析保留原始数据来源。
- 历史数据文件选择、文件头读取、通道数设置和连续回放。
- 频域数据到时域数据的转换链路，支持 CZT 当前算法和 SDK 时域缩放原版链路。
- B-scan 主视图、俯视图、侧视图和单道时域曲线显示。
- 背景消除、均值背景、线性增益、自动/手动增益调节。
- 趋势地面拉直 v2，用于缓解扫描过程中天线起伏造成的地面基准漂移。
- V2 谷值双曲检测与目标区域高亮，用于快速提示疑似浅埋目标响应。
- 算法耗时记录，输出 trace 级别和谷值检测窗口级别的性能日志。
- 3D 体数据窗口和 OpenGL 显示基础能力。

## 技术栈

- 语言：C++20
- GUI 框架：Qt Widgets
- 绘图：QCustomPlot
- 3D 显示：Qt OpenGL / QOpenGLWidget
- 雷达接入：`GPR_SDK`
- 构建系统：qmake
- 目标平台：Windows x64

当前工程文件启用了以下 Qt 模块：

```pro
QT += core gui widgets opengl openglwidgets printsupport
CONFIG += c++20
DEFINES += QCUSTOMPLOT_USE_OPENGL
```

## 目录结构

```text
D:\GPR_0602
├── GPR_3D
│   ├── GPR_3D.pro                         # Qt/qmake 工程文件
│   ├── main.cpp                            # 程序入口
│   ├── mainwindow.*                        # 主窗口、视图组织、处理主链路
│   ├── realtimeacquisition.*               # 实时采集、设备配置、保存文件
│   ├── filereplayview.*                    # 文件回放、文件头显示、批量回放
│   ├── realtimeprocessingsettingview.*     # 在线处理设置面板
│   ├── adjustgainsettingdialog.*           # 手动/自动增益调节窗口
│   ├── image2dplot.*                       # B-scan、俯视图、侧视图显示
│   ├── timedomainplot.*                    # 单道时域曲线显示
│   ├── backgroundsubtraction.*             # 背景建模与背景消除
│   ├── v2hyperboladetector.*               # V2 谷值双曲检测算法
│   ├── algorithmtimingprofiler.*           # 算法耗时统计与 CSV 输出
│   ├── three_d_volume_dialog.*             # 3D 体数据窗口
│   ├── three_d_volume_view.*               # 3D 体数据显示封装
│   ├── OpenGLWidget.*                      # OpenGL 体数据渲染控件
│   ├── icons/                              # 工具栏图标资源
│   ├── timing_logs/                        # 算法耗时日志
│   ├── release/                            # Release 构建产物
│   └── config.ini                          # 默认设备与采集参数
└── GPR_SDK
    └── bin
        ├── include/                        # SDK 头文件
        └── lib/                            # SDK dll/lib/exp/pdb
```

## 核心模块说明

### 1. 实时采集模块

相关文件：

- `realtimeacquisition.h`
- `realtimeacquisition.cpp`
- `realtimeacquisition.ui`

主要职责：

- 读取 `config.ini` 中的雷达连接和采集参数。
- 一键配置设备，包括复位、触发源、触发模式、触发间隔、扫描频率、中频带宽等。
- 加载标定文件。
- 启动或停止实时采集。
- 按帧组织多通道数据。
- 可选将采集数据保存到文件。

典型操作入口：

- `realtimeacquisition::on_One_Click_Configuration_button_clicked()`
- `realtimeacquisition::on_Start_Capture_Data_button_toggled(bool checked)`

### 2. 文件回放模块

相关文件：

- `filereplayview.h`
- `filereplayview.cpp`
- `filereplayview.ui`

主要职责：

- 选择历史数据文件。
- 读取并显示文件头信息。
- 设置回放通道数。
- 支持普通数据文件和时域文件回放。
- 将回放数据按批次送入主处理链路，提高大文件回放效率。

典型操作入口：

- `FileReplayView::on_replay_button_toggled(bool checked)`
- `FileReplayView::on_replay(const GPR_FileHeader& file_header)`

### 3. 主显示与处理链路

相关文件：

- `mainwindow.h`
- `mainwindow.cpp`
- `mainwindow.ui`

主要职责：

- 初始化左侧控制面板、工具栏、状态栏和各类图像视图。
- 接收实时采集或文件回放产生的多通道复数数据。
- 完成频域到时域转换。
- 组织 B-scan、俯视图、侧视图和时域曲线显示。
- 管理背景消除、增益、趋势展平、目标检测等处理开关。
- 将处理后的数据缓存到临时块文件，支持回放重建和 3D 体数据加载。

典型操作入口：

- `MainWindow::on_display_data(...)`
- `MainWindow::on_display_data_batch(...)`
- `MainWindow::on_eliminate_background(bool is_enabled)`
- `MainWindow::on_realtime_play(...)`
- `MainWindow::on_replay(...)`

### 4. 在线处理设置模块

相关文件：

- `realtimeprocessingsettingview.h`
- `realtimeprocessingsettingview.cpp`
- `realtimeprocessingsettingview.ui`

主要功能：

- 背景消除开关。
- 伪彩色图切换。
- 增益调节入口。
- 时域转换算法选择：CZT 当前算法 / SDK 时域缩放原版。
- 探雷专栏：趋势地面拉直 v2 开关。
- 探雷专栏：V2 谷值双曲检测开关。

### 5. 背景消除与增益控制

相关文件：

- `backgroundsubtraction.h`
- `backgroundsubtraction.cpp`
- `adjustgainsettingdialog.h`
- `adjustgainsettingdialog.cpp`

背景处理思路：

- 使用前若干道数据建立稳定背景。
- 对后续输入道进行背景扣除。
- 支持均值背景选项，进一步抑制稳定条带。

增益控制思路：

- 支持线性初始增益和线性斜率。
- 支持 RMS 自动增益。
- 支持手动拖拽增益点并保存/加载 CSV 增益文件。

### 6. 趋势地面拉直 v2

相关文件：

- `mainwindow.h`
- `mainwindow.cpp`

典型入口：

- `MainWindow::apply_mine_ground_trend_flatten_to_trace(...)`
- `MainWindow::on_mine_ground_trend_flatten_changed(bool enabled)`

算法目标：

- 从浅层搜索窗中拾取地面候选。
- 对单道幅值进行平滑和质量评价。
- 使用有限历史约束地面趋势，抑制异常跳变和噪声误拾取。
- 对每道进行插值平移校正，使地面响应在 B-scan 中更加稳定。

该算法只依赖当前道和短历史状态，不需要等待完整 B-scan，适合实时逐道处理。

### 7. V2 谷值双曲检测与目标高亮

相关文件：

- `v2hyperboladetector.h`
- `v2hyperboladetector.cpp`
- `mainwindow.cpp`

典型入口：

- `detect_v2_hyperbola(...)`
- `apply_v2_hyperbola_gain(...)`
- `apply_v2_hyperbola_detector(...)`
- `MainWindow::apply_realtime_v2_hyperbola_backfill(bool flush_all)`

算法目标：

- 对 B-scan 数据进行深度归一化和局部响应分析。
- 使用窗口谷值种子缩小候选范围。
- 对候选区域进行局部双曲线形态验证。
- 检查左右臂连续性、响应平衡性和上下背景差异。
- 合并候选区域并进行局部高亮，使疑似目标区域更容易被观察。

默认参数结构：

```cpp
struct V2HyperbolaParams {
    double t_min_ns = 0.0;
    double t_main_ns = 1.5;
    double t_max_ns = 6.0;
    int rmax_tr = 0;
    QVector<int> radii_tr;
    int stride_tr = 10;
    double seed_threshold = 2.6;
    double confirm_threshold = 0.2;
    double continuity_threshold = 0.2;
};
```

当 `radii_tr` 为空且 `rmax_tr <= 0` 时，当前默认使用 `{200, 300, 400}` 作为候选半径集合。

### 8. 3D 体数据

相关文件：

- `three_d_volume_dialog.h`
- `three_d_volume_dialog.cpp`
- `three_d_volume_view.h`
- `three_d_volume_view.cpp`
- `OpenGLWidget.h`
- `OpenGLWidget.cpp`

主要职责：

- 从缓存块读取多通道、多道、多采样点数据。
- 归一化后传入 3D 体数据窗口。
- 使用 OpenGL 控件进行体数据可视化。

该模块已经具备基础显示能力，后续可以继续完善交互、色图、裁剪、透明度和截图导出等功能。

## 数据处理流程

整体数据流如下：

```text
频域雷达板 / 回放文件
        ↓
GPR_SDK 回调数据
        ↓
多通道复数数据组织
        ↓
CZT 或 SDK 时域转换
        ↓
背景消除 / 均值背景
        ↓
线性增益 / 自动增益 / 手动增益
        ↓
趋势地面拉直 v2
        ↓
V2 谷值双曲检测与目标高亮
        ↓
B-scan / 俯视图 / 侧视图 / 时域曲线
        ↓
缓存、回放、3D 体数据、性能日志
```

## 构建环境

建议环境：

- Windows 10/11 x64
- Visual Studio 2019 或 Visual Studio 2022
- Qt 6.x MSVC x64 Kit
- qmake
- jom 或 nmake

当前工程中可见的历史构建环境包括：

- Qt 6.7.0 MSVC2019 x64
- Qt 6.9.3 MSVC2022 x64

实际构建时不强制使用完全相同版本，但需要保证 Qt、MSVC、SDK 库均为 x64 且 ABI 匹配。

## 构建步骤

### 方式一：Qt Creator

1. 打开 Qt Creator。
2. 选择 `GPR_3D/GPR_3D.pro`。
3. 选择 Windows x64 的 Qt Kit，例如 MSVC 2019 64-bit 或 MSVC 2022 64-bit。
4. 配置为 Release。
5. 构建项目。
6. 确保运行目录能找到 `GPR_SDK.dll` 和加密狗相关 DLL。

### 方式二：命令行 qmake

在已加载 MSVC 环境变量的命令行中执行：

```bat
cd /d D:\GPR_0602\GPR_3D
qmake GPR_3D.pro
jom -f Makefile.Release
```

如果没有 `jom`，可以使用：

```bat
nmake /f Makefile.Release
```

### 依赖库位置

工程文件默认从以下位置查找 SDK：

```text
../GPR_SDK/bin/include
../GPR_SDK/bin/lib
```

Release 链接：

```pro
LIBS += -L$$PWD/../GPR_SDK/bin/lib -lGPR_SDK
```

Debug 链接：

```pro
LIBS += -L$$PWD/../GPR_SDK/bin/lib -lGPR_SDKd
```

## 运行与部署

### 直接运行

当前已有 Release 程序：

```text
D:\GPR_0602\GPR_3D\release\GPR_3D.exe
```

运行前需要确认：

- SDK DLL 可以被找到。
- Qt 运行库可以被找到。
- 加密狗或授权 DLL 可以被找到。
- `config.ini` 中的雷达 IP、端口、频率和保存路径符合当前实验环境。

### 常见运行依赖

一般需要将以下文件放到 exe 同目录，或保证它们在系统 PATH 中：

- `GPR_SDK.dll`
- `dog_windows_x64_3163815.dll`
- Qt 运行库 DLL
- MSVC 运行库 DLL

可以使用 Qt 自带的 `windeployqt` 完成 Qt 运行库部署：

```bat
windeployqt --release --compiler-runtime D:\GPR_0602\GPR_3D\release\GPR_3D.exe
```

## 配置文件

默认配置文件：

```text
GPR_3D/config.ini
```

当前示例：

```ini
[Database]
Host=192.168.0.11
Port=5001
trigger_source=0
trigger_model=1
trigger_interval=0
trigger_frames=0
start_frequency=750
stop_frequency=3000
scan_points=2
IFBW=600khz
start_time=0
end_time=20
bin_file_path=D:\\bin
data_file_path=D:\\data
```

字段说明：

- `Host`：雷达设备 IP。
- `Port`：雷达设备端口。
- `trigger_source`：触发源。
- `trigger_model`：触发模式。
- `trigger_interval`：触发间隔。
- `trigger_frames`：单次触发帧数。
- `start_frequency`：扫频起始频率。
- `stop_frequency`：扫频终止频率。
- `scan_points`：扫描点数配置项。该值可能对应 SDK 或界面预设索引，修改前建议与 SDK 参数映射确认。
- `IFBW`：中频带宽，例如 `600khz`。
- `start_time`：时域显示起始时间，单位 ns。
- `end_time`：时域显示终止时间，单位 ns。
- `bin_file_path`：标定文件默认路径。
- `data_file_path`：采集数据默认保存路径。

中期材料中使用的联调描述为 1-3000 MHz、512 点/道、约 300 道/s。实际演示和答辩前建议检查 `config.ini`、设备实际参数和 PPT 口径是否一致。

## 使用流程

### 实时采集

1. 确认雷达板、天线、网线、工控主机和加密狗连接正常。
2. 打开软件。
3. 在采集控制区点击“一键配置”。
4. 按提示加载标定文件。
5. 如需保存数据，勾选“保存文件”。
6. 点击“开始采集”。
7. 观察 B-scan、俯视图、侧视图和道数统计。
8. 根据需要打开背景消除、趋势地面拉直或 V2 谷值双曲检测。
9. 采集结束后点击“停止采集”。

### 文件回放

1. 切换到文件回放面板。
2. 选择历史数据文件。
3. 设置通道数。
4. 点击“开始回放”。
5. 软件会读取文件头信息并连续回放数据。
6. 可以调整处理开关，对同一份数据重复观察和对比。

### 增益调节

1. 采集或回放至少一定数量的数据后，打开“调节增益”。
2. 可选择自动增益或手动增益。
3. 手动模式下可拖动增益曲线控制点。
4. 可保存或加载 CSV 增益文件。
5. 点击“应用”或“确定”使增益生效。

### 目标高亮验证

建议处理顺序：

1. 打开背景消除。
2. 选择合适的均值背景和线性增益。
3. 打开趋势地面拉直 v2。
4. 打开 V2 谷值双曲检测。
5. 对比处理前后 B-scan 的目标区域可见性。
6. 使用回放功能对相同数据重复调参。

## 性能日志

算法耗时日志位于：

```text
GPR_3D/timing_logs
```

当前包含两类 CSV：

```text
trace_timing_*.csv
valley_window_timing_*.csv
```

### trace_timing

列含义：

- `trace`：道号。
- `channel`：通道号。
- `background_us`：背景处理耗时，单位 us。
- `ground_flatten_us`：趋势地面拉直耗时，单位 us。
- `total_us`：单道合计耗时，单位 us。

### valley_window_timing

列含义：

- `window_start_trace`：检测窗口起始道号。
- `window_end_trace`：检测窗口结束道号。
- `trace_count`：窗口内道数。
- `valley_detector_us`：窗口谷值双曲检测耗时，单位 us。
- `valley_detector_us_per_trace`：折算到单道的耗时，单位 us。
- `seeds`：窗口谷值种子数量。
- `verified`：通过局部验证的候选数量。
- `boxes`：最终输出的目标框数量。

中期材料中给出的阶段统计为：

- 背景消除平均约 1.48 us/道。
- 地面拉平平均约 26.30 us/道。
- 谷值算法平均约 84.40 us/道。

实际数值会随数据长度、通道数、窗口大小、目标数量和电脑性能变化。

## 开发注意事项

### 编码

项目中包含中文界面文本和中文注释。修改源码和 UI 文件时应保持 UTF-8 编码，避免中文乱码进一步扩散。

### 构建产物

`release/` 和 `build/` 目录包含大量生成文件。日常开发时优先修改源文件、头文件和 `.ui` 文件，不要手动编辑 `ui_*.h`、`moc_*.cpp`、`qrc_*.cpp` 等生成文件。

### SDK ABI

`GPR_SDK.lib`、`GPR_SDK.dll`、Qt Kit 和 MSVC 工具链需要保持同一架构。若出现链接错误或运行时找不到入口点，优先检查 Debug/Release、x86/x64 和 MSVC 版本是否混用。

### 实时处理状态

部分处理开关会触发历史缓存重建或延迟到背景初始化之后生效。调试实时链路时建议先单独验证：

1. 原始采集显示。
2. 背景消除。
3. 增益。
4. 趋势地面拉直。
5. V2 谷值双曲检测。

不要一开始同时打开所有增强选项，否则不利于定位问题。

## 常见问题

### 程序启动失败或闪退

可能原因：

- Qt DLL 缺失。
- MSVC 运行库缺失。
- `GPR_SDK.dll` 缺失。
- 加密狗 DLL 或授权环境缺失。

建议：

- 使用 `windeployqt` 部署 Qt 运行库。
- 将 SDK DLL 复制到 exe 同目录。
- 检查加密狗和驱动是否正常。

### 一键配置失败

可能原因：

- 雷达 IP 或端口不正确。
- 网线连接异常。
- 设备未上电。
- `config.ini` 中频率、IFBW 或触发参数不被设备支持。
- 标定文件未正确加载。

建议：

- 检查 `Host` 和 `Port`。
- 确认电脑网卡与雷达设备在同一网段。
- 使用设备 SDK 示例程序或工具先验证设备连通性。
- 将 IFBW 写成软件可识别格式，例如 `600khz`。

### 回放可以显示，但实时采集没有数据

可能原因：

- SDK 回调没有正常触发。
- 触发模式或触发源设置不匹配。
- 标定文件不匹配。
- 设备没有进入采集状态。

建议：

- 先关闭所有处理开关，只观察原始显示。
- 检查回调中的错误码提示。
- 检查采集道数是否增长。

### B-scan 目标不明显

建议按顺序调试：

1. 检查原始数据是否正常。
2. 打开背景消除。
3. 调整线性增益初值和斜率。
4. 尝试 RMS 或手动增益。
5. 打开趋势地面拉直。
6. 打开 V2 谷值双曲检测。
7. 使用同一份回放数据反复调参。

### V2 检测误检或漏检

可能原因：

- 地面基准未稳定。
- 背景条带过强。
- 增益过大导致噪声被放大。
- 目标双曲线响应不完整。
- 默认半径或阈值不适配当前扫描速度、道间距或目标尺度。

建议：

- 先调好背景和增益。
- 再打开趋势地面拉直。
- 最后调整 V2 检测参数。
- 对不同材质、埋深、土壤条件分别记录参数。

## 后续计划

根据中期答辩材料，后续重点包括：

- 极化数据融合：进一步利用 L-L 与 L-R 两路数据，提高原始数据质量和显示稳定性。
- 算法参数优化：降低误检与漏检，提高目标高亮稳定性。
- 多场景实验：扩展目标材质、埋深、土壤湿度、地表起伏和扫描姿态测试。
- 整机装配验证：完成线缆规划、结构设计、便携性和显示流畅性评估。
- 材料整理：沉淀系统设计、软件实现、实验数据、结果图和论文材料。

阶段目标：

- 2026/7 前后：整机结构设计、线缆规划、装配联调。
- 2026/12 前后：极化数据融合、参数优化、多目标实验、结果分析。
- 2027/4 前后：论文撰写、答辩材料完善、系统成果展示。

## 参考文件

- `GPR_3D.pro`：工程依赖、源文件列表、SDK 链接配置。
- `config.ini`：当前默认采集参数。
- `timing_logs/`：算法实时性验证数据。
- `v2hyperboladetector.*`：谷值双曲检测算法实现。
- `mainwindow.*`：主处理链路和界面组织。
- `realtimeacquisition.*`：实时采集和设备配置。
- `filereplayview.*`：数据回放和文件头显示。
