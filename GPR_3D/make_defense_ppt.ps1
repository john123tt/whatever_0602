$ErrorActionPreference = 'Stop'

$templatePath = 'D:\GPR_0601\赵思明-Z20160004-PPT1.pptx'
$outputPath = 'D:\GPR_0601\探地雷达采集系统与目标增强算法答辩PPT_初稿.pptx'

if (!(Test-Path -LiteralPath $templatePath)) {
    throw "Template not found: $templatePath"
}

if (Test-Path -LiteralPath $outputPath) {
    Remove-Item -LiteralPath $outputPath -Force
}

$ppLayoutBlank = 12
$msoFalse = 0
$msoTrue = -1
$ppSaveAsOpenXMLPresentation = 24

$ppt = New-Object -ComObject PowerPoint.Application
$ppt.Visible = $msoTrue
$presentation = $ppt.Presentations.Open($templatePath, $msoFalse, $msoFalse, $msoTrue)

while ($presentation.Slides.Count -gt 0) {
    $presentation.Slides.Item(1).Delete()
}

$W = $presentation.PageSetup.SlideWidth
$H = $presentation.PageSetup.SlideHeight

$blue = 0x5C2B12
$lightBlue = 0xE8F2FA
$dark = 0x333333
$muted = 0x666666
$orange = 0x3F8CFF
$gray = 0xF4F6F8
$line = 0xD7DEE8

function Add-TextBox {
    param($slide, [double]$x, [double]$y, [double]$w, [double]$h, [string]$text,
          [double]$size = 20, [int]$color = 0x333333, [string]$font = '微软雅黑',
          [bool]$bold = $false, [int]$align = 1)
    $shape = $slide.Shapes.AddTextbox(1, $x, $y, $w, $h)
    $shape.TextFrame.TextRange.Text = $text
    $shape.TextFrame.TextRange.Font.Name = $font
    $shape.TextFrame.TextRange.Font.Size = $size
    $shape.TextFrame.TextRange.Font.Color.RGB = $color
    $shape.TextFrame.TextRange.Font.Bold = $(if ($bold) { -1 } else { 0 })
    $shape.TextFrame.TextRange.ParagraphFormat.Alignment = $align
    $shape.TextFrame.MarginLeft = 0
    $shape.TextFrame.MarginRight = 0
    $shape.TextFrame.MarginTop = 0
    $shape.TextFrame.MarginBottom = 0
    return $shape
}

function Add-Line {
    param($slide, [double]$x1, [double]$y1, [double]$x2, [double]$y2, [int]$color = 0x5C2B12, [double]$weight = 1.5)
    $shape = $slide.Shapes.AddLine($x1, $y1, $x2, $y2)
    $shape.Line.ForeColor.RGB = $color
    $shape.Line.Weight = $weight
    return $shape
}

function Add-Rect {
    param($slide, [double]$x, [double]$y, [double]$w, [double]$h, [int]$fill = 0xFFFFFF,
          [int]$stroke = 0xD7DEE8, [double]$weight = 1.0)
    $shape = $slide.Shapes.AddShape(1, $x, $y, $w, $h)
    $shape.Fill.ForeColor.RGB = $fill
    $shape.Line.ForeColor.RGB = $stroke
    $shape.Line.Weight = $weight
    return $shape
}

function Add-Placeholder {
    param($slide, [double]$x, [double]$y, [double]$w, [double]$h, [string]$text)
    $shape = Add-Rect $slide $x $y $w $h 0xFAFBFD 0xAAB7C4 1.2
    $shape.Line.DashStyle = 4
    Add-TextBox $slide ($x + 12) ($y + $h/2 - 20) ($w - 24) 44 $text 15 0x777777 '微软雅黑' $false 2 | Out-Null
    return $shape
}

function Add-Header {
    param($slide, [string]$title, [string]$section, [int]$num)
    Add-TextBox $slide 38 20 610 32 $title 21 $blue '微软雅黑' $true 1 | Out-Null
    Add-TextBox $slide 814 24 70 24 ("{0:00}" -f $num) 16 $muted 'Arial' $false 3 | Out-Null
    Add-Line $slide 38 58 890 58 $blue 1.8 | Out-Null
    if ($section) {
        Add-TextBox $slide 38 62 360 22 $section 10 $muted '微软雅黑' $false 1 | Out-Null
    }
}

function Add-Bullets {
    param($slide, [double]$x, [double]$y, [double]$w, [double]$h, [string[]]$items, [double]$size = 18)
    $shape = Add-TextBox $slide $x $y $w $h (($items | ForEach-Object { "• $_" }) -join "`r") $size $dark '微软雅黑' $false 1
    $shape.TextFrame.TextRange.ParagraphFormat.SpaceAfter = 8
    return $shape
}

function Add-TitleSlide {
    param([string]$title, [string]$subtitle)
    $slide = $presentation.Slides.Add($presentation.Slides.Count + 1, $ppLayoutBlank)
    Add-Rect $slide 0 0 $W $H 0xFFFFFF 0xFFFFFF 0 | Out-Null
    Add-Rect $slide 0 0 170 $H $blue $blue 0 | Out-Null
    Add-TextBox $slide 210 120 610 92 $title 32 $blue '微软雅黑' $true 1 | Out-Null
    Add-Line $slide 210 228 820 228 $blue 2.5 | Out-Null
    Add-TextBox $slide 210 250 610 38 $subtitle 18 $muted '微软雅黑' $false 1 | Out-Null
    Add-TextBox $slide 210 345 590 80 "汇报人：__________`r指导老师：__________`r专业：电子信息" 18 $dark '微软雅黑' $false 1 | Out-Null
    Add-Placeholder $slide 610 335 230 126 "图片占位：放置平面螺线天线实物图或系统整体照片"
    Add-TextBox $slide 210 480 610 24 "探地雷达硬件系统、采集软件与目标增强算法研究" 13 $muted '微软雅黑' $false 1 | Out-Null
}

function Add-SectionSlide {
    param([string]$title, [string]$num, [string]$subtitle)
    $slide = $presentation.Slides.Add($presentation.Slides.Count + 1, $ppLayoutBlank)
    Add-Rect $slide 0 0 $W $H 0xFFFFFF 0xFFFFFF 0 | Out-Null
    Add-TextBox $slide 90 185 120 60 $num 40 $blue 'Arial' $true 2 | Out-Null
    Add-Line $slide 245 210 780 210 $blue 2.2 | Out-Null
    Add-TextBox $slide 250 170 570 50 $title 30 $blue '微软雅黑' $true 1 | Out-Null
    Add-TextBox $slide 252 226 550 28 $subtitle 16 $muted '微软雅黑' $false 1 | Out-Null
}

function Add-ContentSlide {
    param([string]$title, [string]$section, [string[]]$bullets, [string]$placeholder, [int]$num)
    $slide = $presentation.Slides.Add($presentation.Slides.Count + 1, $ppLayoutBlank)
    Add-Rect $slide 0 0 $W $H 0xFFFFFF 0xFFFFFF 0 | Out-Null
    Add-Header $slide $title $section $num
    Add-Bullets $slide 58 105 430 330 $bullets 18 | Out-Null
    Add-Placeholder $slide 520 115 330 250 $placeholder | Out-Null
    return $slide
}

function Add-ProcessSlide {
    param([string]$title, [string]$section, [string[]]$steps, [string[]]$notes, [int]$num)
    $slide = $presentation.Slides.Add($presentation.Slides.Count + 1, $ppLayoutBlank)
    Add-Rect $slide 0 0 $W $H 0xFFFFFF 0xFFFFFF 0 | Out-Null
    Add-Header $slide $title $section $num
    $x = 55
    for ($i = 0; $i -lt $steps.Count; $i++) {
        Add-Rect $slide $x 135 142 72 $lightBlue $blue 1.3 | Out-Null
        Add-TextBox $slide ($x + 8) 154 126 36 $steps[$i] 16 $blue '微软雅黑' $true 2 | Out-Null
        if ($i -lt $steps.Count - 1) {
            Add-Line $slide ($x + 145) 171 ($x + 178) 171 $blue 2 | Out-Null
            Add-TextBox $slide ($x + 160) 158 24 24 ">" 18 $blue 'Arial' $true 2 | Out-Null
        }
        $x += 176
    }
    Add-Bullets $slide 70 260 760 170 $notes 17 | Out-Null
}

function Add-TableSlide {
    param([string]$title, [string]$section, [string[][]]$rows, [int]$num)
    $slide = $presentation.Slides.Add($presentation.Slides.Count + 1, $ppLayoutBlank)
    Add-Rect $slide 0 0 $W $H 0xFFFFFF 0xFFFFFF 0 | Out-Null
    Add-Header $slide $title $section $num
    $left = 65; $top = 115; $cw = @(180, 225, 225, 195); $rh = 52
    for ($r = 0; $r -lt $rows.Count; $r++) {
        $x = $left
        for ($c = 0; $c -lt $rows[$r].Count; $c++) {
            $fill = $(if ($r -eq 0) { $blue } elseif ($r % 2 -eq 0) { 0xF8FAFC } else { 0xFFFFFF })
            $fg = $(if ($r -eq 0) { 0xFFFFFF } else { $dark })
            Add-Rect $slide $x ($top + $r*$rh) $cw[$c] $rh $fill $line 0.8 | Out-Null
            Add-TextBox $slide ($x + 8) ($top + $r*$rh + 10) ($cw[$c]-16) 34 $rows[$r][$c] 14 $fg '微软雅黑' $(if ($r -eq 0) { $true } else { $false }) 2 | Out-Null
            $x += $cw[$c]
        }
    }
}

$slideNo = 1
Add-TitleSlide '探地雷达采集系统与目标增强算法研究' '基于平面螺线天线、工控机与实时采集软件的系统设计'

$slide = $presentation.Slides.Add($presentation.Slides.Count + 1, $ppLayoutBlank)
Add-Rect $slide 0 0 $W $H 0xFFFFFF 0xFFFFFF 0 | Out-Null
Add-TextBox $slide 80 50 240 50 '目 录' 30 $blue '微软雅黑' $true 1 | Out-Null
Add-TextBox $slide 80 95 220 25 'Content' 14 $muted 'Arial' $false 1 | Out-Null
$contents = @(
    @('01','研究背景及总体方案'),
    @('02','硬件系统设计'),
    @('03','采集软件设计与实现'),
    @('04','采集与探雷算法研究'),
    @('05','实验验证与总结展望')
)
$y = 155
foreach ($c in $contents) {
    Add-TextBox $slide 120 $y 70 34 $c[0] 22 $blue 'Arial' $true 1 | Out-Null
    Add-Line $slide 190 ($y+17) 260 ($y+17) $blue 1.2 | Out-Null
    Add-TextBox $slide 285 $y 430 34 $c[1] 22 $dark '微软雅黑' $false 1 | Out-Null
    $y += 62
}

Add-SectionSlide '研究背景及总体方案' '01' '从探地雷达探测需求出发，构建硬件、软件和算法一体化系统'

Add-ContentSlide '研究背景及意义' '01 研究背景及总体方案' @(
    '浅埋目标探测需要非接触、快速、可重复的电磁探测手段',
    '探地雷达数据具有强杂波、强直达波、目标回波弱等特点',
    '传统离线处理流程难以及时反馈采集质量和目标位置',
    '本课题面向小型化探测场景，搭建采集系统并实现实时成像与目标增强'
) '图片占位：放置探地雷达应用场景图、浅埋目标示意图或实验场地照片' 3 | Out-Null

Add-ContentSlide '课题研究内容' '01 研究背景及总体方案' @(
    '硬件部分：平面螺线天线与小屏工控机组成便携式采集平台',
    '软件部分：基于 Qt/C++ 的多通道实时采集、回放、成像和显示软件',
    '算法部分：CZT 时域转换、背景抑制、趋势地面拉直与 V2 谷值双曲目标增强',
    '工程目标：形成可采集、可显示、可处理、可复现实验的数据闭环'
) '图片占位：放置“硬件-软件-算法”三模块系统结构图' 4 | Out-Null

Add-ProcessSlide '总体技术路线' '01 研究背景及总体方案' @('天线接收','工控机采集','时域转换','实时成像','目标增强') @(
    '频域复数数据经 CZT/SDK 转换形成时域 A-scan',
    '多通道数据合成为 B-scan、俯视图和侧视图',
    '探雷专栏算法在实时流中执行，增强疑似目标区域',
    '回放模块复用同一处理链路，便于算法调试和对比'
) 5 | Out-Null

Add-SectionSlide '硬件系统设计' '02' '平面螺线天线与小屏工控机组成的小型化采集系统'

Add-ContentSlide '平面螺线天线设计思路' '02 硬件系统设计' @(
    '采用平面螺线结构，目标是获得宽频带、低剖面和便于安装的天线形式',
    '天线作为当前已具备的硬件主体，后续与工控机、射频前端和采集软件联调',
    '答辩中应突出：结构尺寸、工作频带、馈电方式、测试曲线和实物加工状态',
    '与传统天线相比，平面结构更适合小型化、便携式探测平台'
) '图片占位：放置平面螺线天线实物照片；旁边可补 S11/方向图测试曲线' 7 | Out-Null

Add-ContentSlide '小屏工控机采集平台规划' '02 硬件系统设计' @(
    '工控机负责设备配置、采集控制、数据缓存和实时显示',
    '小屏交互要求软件界面按钮清晰、信息密度适中、采集状态可见',
    '当前阶段硬件只有天线，工控机与外设集成作为后续系统完善内容',
    '软件已预留实时采集、文件保存、参数配置和多图显示能力'
) '图片占位：放置小屏工控机照片或目标系统外观草图；没有实物可放结构框图' 8 | Out-Null

Add-ProcessSlide '硬件与软件接口关系' '02 硬件系统设计' @('GPR SDK','通道配置','回调数据','帧合并','显示处理') @(
    '软件通过 GPR SDK 配置触发源、频率范围、采样点数和中频带宽',
    '采集回调返回头信息与复数数据，软件按 Tx/Rx 通道合并为一帧',
    '原始数据可实时保存为文件，回放时沿用同一显示和处理流程'
) 9 | Out-Null

Add-SectionSlide '采集软件设计与实现' '03' '围绕实时采集、回放、显示和算法验证构建的软件平台'

Add-ContentSlide '软件总体架构' '03 采集软件设计与实现' @(
    '开发环境：Qt/C++，使用 QCustomPlot 完成二维雷达图像显示',
    '功能模块：实时采集、文件回放、处理设置、手动增益、时域波形、三维体数据',
    '数据组织：多通道复数数据转换为时域序列，形成 B-scan 与通道俯视图',
    '缓存机制：分块缓存与多分辨率加载，兼顾实时滚动和大数据回放'
) '图片占位：放置软件主界面截图，最好包含 B-scan、右侧设置栏和采集按钮' 11 | Out-Null

Add-ProcessSlide '实时采集流程' '03 采集软件设计与实现' @('设备配置','开始采集','回调接收','通道合帧','图像刷新') @(
    '支持校准文件加载、频段/点数/触发模式设置和保存文件控制',
    '采集线程通过 SDK 回调获取数据，UI 线程以队列方式更新显示',
    '多通道帧合并后生成主视图、俯视图和可选侧视图'
) 12 | Out-Null

Add-ContentSlide '回放与可视化功能' '03 采集软件设计与实现' @(
    '回放模块读取历史雷达数据，支持按文件头信息恢复通道和采样参数',
    '主图支持实时滚动、缩放、颜色映射切换和局部数据替换',
    '时域波形视图用于检查单道信号质量和变换结果',
    '三维体数据模块用于多通道或多测线数据的扩展显示'
) '图片占位：放置文件回放界面截图；可再放一张时域波形窗口截图' 13 | Out-Null

Add-ContentSlide '软件工程实现特点' '03 采集软件设计与实现' @(
    '实时与回放共用处理链路，便于用历史数据复现实验现象',
    '显示采用分块缓存，避免长测线一次性加载造成内存压力',
    '算法开关集中在“探雷专栏”，包括趋势地面拉直 v2 和 V2 谷值双曲检测',
    '支持当前 CZT 算法与 SDK 原版时域缩放结果对比'
) '图片占位：放置“探雷专栏”设置区域截图，突出 CZT、趋势地面拉直、V2 检测开关' 14 | Out-Null

Add-SectionSlide '采集与探雷算法研究' '04' '从频域到时域、从预处理到目标增强的算法链路'

Add-ContentSlide 'CZT 时域转换算法' '04 采集与探雷算法研究' @(
    '输入为频域复数采样数据，输出为指定时间窗内的时域响应',
    '采用 Kaiser 窗抑制旁瓣，并通过 chirp 卷积实现 CZT 计算',
    '相比固定 SDK 缩放，CZT 便于控制起止时间窗和采样映射',
    '该算法是后续 B-scan 成像、背景处理和目标检测的基础'
) '图片占位：放置频域数据到时域 A-scan 的流程图；可补一条转换前后波形对比' 16 | Out-Null

Add-ContentSlide '背景抑制与显示增益' '04 采集与探雷算法研究' @(
    '背景消除用于削弱水平方向稳定干扰和系统固定响应',
    '均值背景与手动/自动增益用于增强深部弱反射',
    '显示色阶根据采集初期信号统计自适应设置，减少强异常值影响',
    '实时显示节流和缓存替换降低目标增强过程中的卡顿'
) '图片占位：放置原始 B-scan、背景去除后 B-scan、增益后 B-scan 三联图' 17 | Out-Null

Add-ContentSlide '趋势地面拉直 v2' '04 采集与探雷算法研究' @(
    '针对地面/直达波起伏造成的目标形态变形，估计并跟踪首波位置',
    '采用初始化、保持、恢复等状态机制，降低瞬时噪声导致的误校正',
    '输出用于 V2 双曲检测前的预处理，使浅层目标在统一时间基准下比较',
    '适合实时流处理，也可用于回放数据批处理验证'
) '图片占位：放置地面拉直前后 B-scan 对比，标出地面趋势线' 18 | Out-Null

Add-ContentSlide 'V2 谷值双曲检测与目标增强' '04 采集与探雷算法研究' @(
    '在 ROI 时间窗内估计逐深度噪声尺度，构建黑/白极性响应图',
    '按候选种子、半径集合和左右翼连续性验证疑似双曲目标',
    '默认关注 0-6 ns 范围，主层约 2 ns，实时半径集合约 300/400 道',
    '检测到目标后对框内区域施加随深度变化的增益，实现目标显著化'
) '图片占位：放置 V2 检测结果截图：原图、检测框/增强区域、增强后图像' 19 | Out-Null

Add-ProcessSlide 'V2 算法流程' '04 采集与探雷算法研究' @('稳健归一化','种子筛选','曲线验证','目标合并','局部增益') @(
    '稳健噪声估计：按深度行计算中位数/MAD 量级，降低异常值影响',
    '形态验证：检查左右翼连续性、对称性、黑白极性组合和顶点响应',
    '实时回填：采用滑动窗口检测，过滤窗口边界截断框，避免小块补块现象'
) 20 | Out-Null

$rows = @(
    @('算法/模块','主要思想','优点','局限或用途'),
    @('V2 谷值双曲检测','基于稳健归一化、左右翼验证和局部增益','适合实时增强，和软件显示链路结合紧密','依赖 ROI、半径和阈值设置'),
    @('Hough 双曲检测','在曲率参数空间投票并验证双曲线','几何解释直观，可作为离线对比方法','参数空间搜索量较大，实时性需优化'),
    @('地面到时估计','根据平均包络阈值自动拾取首波','可为地面校正提供基准','强杂波场景可能误触发'),
    @('背景高通去除','沿道向移动平均估计并扣除背景','实现简单，能削弱水平杂波','可能削弱连续层状反射')
)
Add-TableSlide '与 GPR_CODE_V10.0 探雷算法对比' '04 采集与探雷算法研究' $rows 21

Add-ContentSlide '算法验证思路' '04 采集与探雷算法研究' @(
    '用同一数据分别运行：原始显示、背景去除、趋势拉直、V2 增强、Hough 检测',
    '对比指标建议：疑似目标可见性、误增强区域、处理耗时、参数敏感性',
    '答辩前建议补充典型样例：有目标、无目标、强杂波、边界目标各一组',
    '当前 PPT 先保留图片和表格位置，后续替换为实际实验截图和数值'
) '图片占位：放置算法对比实验四宫格；旁边可放耗时/检测数量统计表' 22 | Out-Null

Add-SectionSlide '实验验证与总结展望' '05' '展示阶段性成果，并明确后续工程化与实验验证方向'

Add-ContentSlide '阶段性成果' '05 实验验证与总结展望' @(
    '已完成平面螺线天线硬件主体，为后续系统集成提供基础',
    '已实现 GPR 采集软件：实时采集、保存、回放、成像和多视图显示',
    '已实现 CZT 时域转换、背景处理、趋势地面拉直和 V2 双曲目标增强',
    '已建立与 MATLAB 探雷算法的对比入口，便于后续实验验证'
) '图片占位：放置当前成果拼图：天线实物、软件截图、算法增强结果' 24 | Out-Null

Add-ContentSlide '存在问题与改进方向' '05 实验验证与总结展望' @(
    '硬件系统尚未完全闭环：工控机、外壳、供电和射频连接仍需集成',
    '算法参数仍需通过多场景实测数据标定，提高泛化稳定性',
    '目标检测目前更偏增强与提示，后续可加入定量评价和自动报警策略',
    '可继续优化实时处理线程、缓存写入和小屏交互体验'
) '图片占位：放置后续系统集成计划图或实验平台改进示意图' 25 | Out-Null

Add-ContentSlide '后续工作计划' '05 实验验证与总结展望' @(
    '完成平面螺线天线参数测试：S11、方向图、实际探测深度和分辨率',
    '完成工控机集成，形成可携带采集样机',
    '构建标准实验数据集：不同埋深、不同目标、不同土壤/介质条件',
    '完善算法对比：V2、Hough、背景高通、直达波校正等模块统一评估'
) '图片占位：放置甘特图或实验计划表；可按月份列出硬件/软件/算法节点' 26 | Out-Null

$slide = $presentation.Slides.Add($presentation.Slides.Count + 1, $ppLayoutBlank)
Add-Rect $slide 0 0 $W $H 0xFFFFFF 0xFFFFFF 0 | Out-Null
Add-Rect $slide 0 0 $W 95 $blue $blue 0 | Out-Null
Add-TextBox $slide 0 165 $W 60 '恳请老师批评指正！' 34 $blue '微软雅黑' $true 2 | Out-Null
Add-TextBox $slide 0 240 $W 38 'THANK YOU FOR WATCHING' 20 $muted 'Arial' $false 2 | Out-Null
Add-TextBox $slide 0 330 $W 70 "汇报人：__________      指导老师：__________`r专业：电子信息" 18 $dark '微软雅黑' $false 2 | Out-Null

$presentation.SaveAs($outputPath, $ppSaveAsOpenXMLPresentation)
$presentation.Close()
$ppt.Quit()

[System.Runtime.InteropServices.Marshal]::ReleaseComObject($presentation) | Out-Null
[System.Runtime.InteropServices.Marshal]::ReleaseComObject($ppt) | Out-Null

Write-Output $outputPath
