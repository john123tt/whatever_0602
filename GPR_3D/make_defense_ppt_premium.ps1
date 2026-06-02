$ErrorActionPreference = 'Stop'

$templatePath = 'D:\GPR_0601\赵思明-Z20160004-PPT1.pptx'
$outputPath = 'D:\GPR_0601\探地雷达采集系统与目标增强算法答辩PPT_美化版.pptx'

if (!(Test-Path -LiteralPath $templatePath)) {
    throw "Template not found: $templatePath"
}

if (Test-Path -LiteralPath $outputPath) {
    Remove-Item -LiteralPath $outputPath -Force
}

$ppLayoutBlank = 12
$msoTrue = -1
$msoFalse = 0
$ppSaveAsOpenXMLPresentation = 24

function RGB($r, $g, $b) {
    return [int]($r + ($g * 256) + ($b * 65536))
}

$C = @{
    Ink       = RGB 18 27 43
    Navy      = RGB 15 43 74
    Blue      = RGB 24 90 168
    Cyan      = RGB 34 162 205
    Teal      = RGB 31 140 122
    Amber     = RGB 237 160 54
    Red       = RGB 195 75 61
    Paper     = RGB 247 249 252
    White     = RGB 255 255 255
    Line      = RGB 218 226 236
    Muted     = RGB 98 111 130
    SoftBlue  = RGB 229 241 251
    SoftCyan  = RGB 229 247 250
    SoftAmber = RGB 255 244 225
    DarkPanel = RGB 22 36 56
}

$fontCN = 'Microsoft YaHei'
$fontEN = 'Aptos'

$ppt = New-Object -ComObject PowerPoint.Application
$ppt.Visible = $msoTrue
$presentation = $ppt.Presentations.Open($templatePath, $msoFalse, $msoFalse, $msoTrue)

while ($presentation.Slides.Count -gt 0) {
    $presentation.Slides.Item(1).Delete()
}

$W = $presentation.PageSetup.SlideWidth
$H = $presentation.PageSetup.SlideHeight

function Add-Box {
    param($slide, [double]$x, [double]$y, [double]$w, [double]$h, [int]$fill, [int]$line, [double]$weight = 0.8, [int]$shapeType = 1)
    $shape = $slide.Shapes.AddShape($shapeType, $x, $y, $w, $h)
    $shape.Fill.ForeColor.RGB = $fill
    $shape.Line.ForeColor.RGB = $line
    $shape.Line.Weight = $weight
    return $shape
}

function Add-Line {
    param($slide, [double]$x1, [double]$y1, [double]$x2, [double]$y2, [int]$color, [double]$weight = 1.5)
    $shape = $slide.Shapes.AddLine($x1, $y1, $x2, $y2)
    $shape.Line.ForeColor.RGB = $color
    $shape.Line.Weight = $weight
    return $shape
}

function Add-Text {
    param(
        $slide, [double]$x, [double]$y, [double]$w, [double]$h, [string]$text,
        [double]$size = 18, [int]$color = 0, [bool]$bold = $false,
        [int]$align = 1, [string]$font = $fontCN
    )
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

function Add-Bullets {
    param($slide, [double]$x, [double]$y, [double]$w, [double]$h, [string[]]$items, [double]$size = 15, [int]$color = $C.Ink)
    $text = ($items | ForEach-Object { "• $_" }) -join "`r"
    $shape = Add-Text $slide $x $y $w $h $text $size $color $false 1 $fontCN
    $shape.TextFrame.TextRange.ParagraphFormat.SpaceAfter = 6
    return $shape
}

function Add-Page {
    param([int]$bg = $C.Paper)
    $slide = $presentation.Slides.Add($presentation.Slides.Count + 1, $ppLayoutBlank)
    Add-Box $slide 0 0 $W $H $bg $bg 0 | Out-Null
    return $slide
}

function Add-Header {
    param($slide, [string]$section, [string]$title, [string]$num, [int]$accent = $C.Blue)
    Add-Box $slide 0 0 18 $H $accent $accent 0 | Out-Null
    Add-Text $slide 44 22 160 18 $section 9 $C.Muted $false 1 $fontEN | Out-Null
    Add-Text $slide 44 42 660 34 $title 22 $C.Ink $true 1 $fontCN | Out-Null
    Add-Line $slide 44 82 888 82 $C.Line 1 | Out-Null
    Add-Text $slide 820 34 54 30 $num 16 $C.Muted $false 3 $fontEN | Out-Null
}

function Add-Card {
    param($slide, [double]$x, [double]$y, [double]$w, [double]$h, [string]$title, [string]$body, [int]$accent = $C.Blue)
    Add-Box $slide $x $y $w $h $C.White $C.Line 0.8 5 | Out-Null
    Add-Box $slide $x $y 5 $h $accent $accent 0 | Out-Null
    Add-Text $slide ($x + 18) ($y + 16) ($w - 32) 24 $title 15 $accent $true 1 $fontCN | Out-Null
    Add-Text $slide ($x + 18) ($y + 48) ($w - 32) ($h - 62) $body 12.5 $C.Muted $false 1 $fontCN | Out-Null
}

function Add-ImageSlot {
    param($slide, [double]$x, [double]$y, [double]$w, [double]$h, [string]$title, [string]$hint, [int]$accent = $C.Cyan)
    Add-Box $slide $x $y $w $h $C.White $C.Line 1.0 5 | Out-Null
    Add-Box $slide ($x + 12) ($y + 12) ($w - 24) ($h - 24) (RGB 242 246 251) (RGB 176 190 207) 1.2 1 | Out-Null
    $inner = $slide.Shapes.Item($slide.Shapes.Count)
    $inner.Line.DashStyle = 4
    Add-Box $slide ($x + 32) ($y + 32) 44 44 $accent $accent 0 9 | Out-Null
    Add-Text $slide ($x + 92) ($y + 33) ($w - 124) 24 $title 15 $C.Ink $true 1 $fontCN | Out-Null
    Add-Text $slide ($x + 92) ($y + 62) ($w - 124) 62 $hint 12.5 $C.Muted $false 1 $fontCN | Out-Null
    Add-Text $slide ($x + 32) ($y + $h - 45) ($w - 64) 20 "图片占位 / 后续替换为实测图或截图" 10 (RGB 137 150 168) $false 2 $fontCN | Out-Null
}

function Add-SectionSlide {
    param([string]$num, [string]$title, [string]$subtitle, [int]$accent)
    $slide = Add-Page $C.Ink
    Add-Box $slide 0 0 $W $H $C.Ink $C.Ink 0 | Out-Null
    Add-Box $slide 0 0 250 $H $accent $accent 0 | Out-Null
    Add-Text $slide 74 145 120 70 $num 48 $C.White $true 2 $fontEN | Out-Null
    Add-Line $slide 305 194 820 194 $accent 3 | Out-Null
    Add-Text $slide 306 145 540 46 $title 30 $C.White $true 1 $fontCN | Out-Null
    Add-Text $slide 308 216 520 52 $subtitle 15 (RGB 198 211 226) $false 1 $fontCN | Out-Null
    return $slide
}

function Add-Flow {
    param($slide, [string[]]$steps, [double]$x, [double]$y, [double]$w, [int]$accent)
    $stepW = 126
    $gap = 34
    for ($i = 0; $i -lt $steps.Count; $i++) {
        $sx = $x + ($i * ($stepW + $gap))
        Add-Box $slide $sx $y $stepW 58 $C.White $accent 1.2 5 | Out-Null
        Add-Text $slide ($sx + 8) ($y + 15) ($stepW - 16) 24 $steps[$i] 13 $accent $true 2 $fontCN | Out-Null
        if ($i -lt ($steps.Count - 1)) {
            Add-Line $slide ($sx + $stepW + 4) ($y + 29) ($sx + $stepW + $gap - 6) ($y + 29) $accent 2 | Out-Null
            Add-Text $slide ($sx + $stepW + 10) ($y + 18) 18 22 ">" 13 $accent $true 2 $fontEN | Out-Null
        }
    }
}

function Add-Metric {
    param($slide, [double]$x, [double]$y, [double]$w, [string]$value, [string]$label, [int]$accent)
    Add-Box $slide $x $y $w 76 $C.White $C.Line 0.8 5 | Out-Null
    Add-Text $slide ($x + 12) ($y + 11) ($w - 24) 30 $value 22 $accent $true 2 $fontEN | Out-Null
    Add-Text $slide ($x + 12) ($y + 46) ($w - 24) 18 $label 10.5 $C.Muted $false 2 $fontCN | Out-Null
}

function Add-Table {
    param($slide, [double]$x, [double]$y, [int[]]$widths, [string[][]]$rows, [int]$accent)
    $rh = 43
    for ($r = 0; $r -lt $rows.Count; $r++) {
        $cx = $x
        for ($cidx = 0; $cidx -lt $rows[$r].Count; $cidx++) {
            $fill = $(if ($r -eq 0) { $accent } elseif ($r % 2 -eq 1) { $C.White } else { RGB 243 247 251 })
            $fg = $(if ($r -eq 0) { $C.White } else { $C.Ink })
            Add-Box $slide $cx ($y + $r * $rh) $widths[$cidx] $rh $fill $C.Line 0.7 1 | Out-Null
            Add-Text $slide ($cx + 8) ($y + $r * $rh + 9) ($widths[$cidx] - 16) 26 $rows[$r][$cidx] 11.5 $fg $(if ($r -eq 0) { $true } else { $false }) 2 $fontCN | Out-Null
            $cx += $widths[$cidx]
        }
    }
}

$slide = Add-Page $C.Ink
Add-Box $slide 0 0 $W $H $C.Ink $C.Ink 0 | Out-Null
Add-Box $slide 0 0 260 $H $C.Navy $C.Navy 0 | Out-Null
Add-Box $slide 260 0 36 $H $C.Cyan $C.Cyan 0 | Out-Null
Add-Text $slide 72 70 150 24 "GRADUATION DEFENSE" 11 (RGB 183 205 229) $false 1 $fontEN | Out-Null
Add-Text $slide 72 122 142 96 "GPR`rSYSTEM" 34 $C.White $true 1 $fontEN | Out-Null
Add-Text $slide 340 98 510 98 "探地雷达采集系统与`r目标增强算法研究" 32 $C.White $true 1 $fontCN | Out-Null
Add-Line $slide 342 226 790 226 $C.Cyan 3 | Out-Null
Add-Text $slide 344 246 500 34 "平面螺线天线 · 小屏工控机 · 实时采集软件 · 探雷算法" 16 (RGB 199 215 233) $false 1 $fontCN | Out-Null
Add-Card $slide 344 334 220 110 "汇报信息" "汇报人：__________`r指导教师：__________`r专业：电子信息" $C.Cyan
Add-ImageSlot $slide 596 324 254 130 "系统主视觉" "放置平面螺线天线实物图；如果后续工控机到位，替换为整机系统照片。" $C.Amber

$slide = Add-Page $C.Paper
Add-Text $slide 76 54 260 38 "目录" 30 $C.Ink $true 1 $fontCN | Out-Null
Add-Text $slide 78 96 200 18 "CONTENTS" 10 $C.Muted $false 1 $fontEN | Out-Null
$items = @(
    @('01','研究背景与总体方案','从探地雷达探测需求出发，明确硬件、软件、算法三条主线'),
    @('02','硬件系统设计','平面螺线天线与小屏工控机组成的小型化采集平台'),
    @('03','采集软件设计','Qt/C++ 实时采集、回放、显示与算法验证平台'),
    @('04','采集与探雷算法','CZT、背景抑制、地面拉直与 V2 谷值双曲目标增强'),
    @('05','实验验证与总结展望','展示阶段成果、对比分析、后续改进方向')
)
$y = 145
foreach ($it in $items) {
    Add-Box $slide 82 $y 72 46 $C.Ink $C.Ink 0 5 | Out-Null
    Add-Text $slide 82 ($y + 9) 72 25 $it[0] 18 $C.White $true 2 $fontEN | Out-Null
    Add-Text $slide 182 ($y - 2) 500 24 $it[1] 18 $C.Ink $true 1 $fontCN | Out-Null
    Add-Text $slide 182 ($y + 27) 620 20 $it[2] 11.5 $C.Muted $false 1 $fontCN | Out-Null
    Add-Line $slide 154 ($y + 23) 176 ($y + 23) $C.Cyan 2 | Out-Null
    $y += 70
}

Add-SectionSlide '01' '研究背景与总体方案' '围绕浅埋目标探测，构建硬件接收、实时采集、算法增强的一体化技术路线。' $C.Blue | Out-Null

$slide = Add-Page $C.Paper
Add-Header $slide '01 BACKGROUND' '研究背景与问题定义' '03' $C.Blue
Add-Card $slide 58 120 250 118 '应用需求' '浅埋目标探测需要非接触、快速、可重复的数据获取方式，现场还要求结果能够实时反馈。' $C.Blue
Add-Card $slide 338 120 250 118 '数据难点' '探地雷达 B-scan 中存在强直达波、背景杂波、目标回波弱和形态不稳定等问题。' $C.Teal
Add-Card $slide 618 120 250 118 '工程目标' '形成“采集-显示-处理-回放-复现”的闭环平台，为后续实验与算法迭代提供基础。' $C.Amber
Add-ImageSlot $slide 58 285 360 150 '应用场景图' '放置浅埋目标探测、实验沙箱、室外测线或探地雷达应用示意图。' $C.Blue
Add-ImageSlot $slide 448 285 420 150 '典型 B-scan 图' '放置一张原始雷达剖面，标出强背景、地面层和疑似目标区域。' $C.Teal

$slide = Add-Page $C.Paper
Add-Header $slide '01 OVERVIEW' '课题研究内容与技术闭环' '04' $C.Blue
Add-Flow $slide @('平面螺线天线','工控机采集','CZT 时域转换','实时成像','目标增强') 62 130 760 $C.Blue
Add-Bullets $slide 80 240 360 145 @(
    '硬件：以平面螺线天线为前端，规划与小屏工控机形成便携式系统',
    '软件：基于 Qt/C++ 构建实时采集、文件回放、参数设置和多视图显示',
    '算法：围绕 CZT、背景抑制、趋势地面拉直和 V2 谷值检测形成处理链'
) 14.5 $C.Ink | Out-Null
Add-ImageSlot $slide 500 236 330 160 '系统结构图' '建议绘制“天线-采集设备/SDK-软件-算法-结果显示”的模块框图。' $C.Cyan

Add-SectionSlide '02' '硬件系统设计' '当前实物以平面螺线天线为主，后续与小屏工控机、射频前端和采集软件组成整机。' $C.Teal | Out-Null

$slide = Add-Page $C.Paper
Add-Header $slide '02 HARDWARE' '平面螺线天线设计思路' '06' $C.Teal
Add-ImageSlot $slide 56 116 380 256 '天线实物 / 版图' '放置平面螺线天线照片、PCB 版图或仿真模型。建议加尺寸标注。' $C.Teal
Add-Metric $slide 480 125 108 '宽带' '覆盖 GPR 频段' $C.Teal
Add-Metric $slide 616 125 108 '低剖面' '便携集成' $C.Blue
Add-Metric $slide 752 125 108 '易加工' '适合实验迭代' $C.Amber
Add-Bullets $slide 488 228 350 138 @(
    '采用平面螺线结构，目标是获得宽频带、低剖面、便于安装的天线形式',
    '答辩中应突出结构尺寸、馈电方式、测试曲线和实物加工状态',
    '与传统体积较大的天线相比，更适合小型化、便携式探测平台'
) 14.2 $C.Ink | Out-Null

$slide = Add-Page $C.Paper
Add-Header $slide '02 HARDWARE' '小屏工控机采集平台规划' '07' $C.Teal
Add-Card $slide 58 122 245 150 '工控机职责' '负责设备配置、采集控制、数据缓存、文件保存和实时显示，是系统现场交互中心。' $C.Teal
Add-Card $slide 330 122 245 150 '小屏交互要求' '界面按钮信息清晰，状态反馈明显，设置项集中，适合现场快速操作。' $C.Blue
Add-Card $slide 602 122 245 150 '当前阶段说明' '目前硬件只有天线，工控机与外设集成作为后续系统完善内容。' $C.Amber
Add-ImageSlot $slide 96 314 320 130 '工控机或结构草图' '没有实物时可放目标系统外观草图、模块堆叠图或设备选型图。' $C.Teal
Add-ImageSlot $slide 462 314 320 130 '硬件接口关系' '放置 SDK/采集设备、天线、PC、软件之间的数据与控制连接图。' $C.Blue

Add-SectionSlide '03' '采集软件设计' '软件承担实时采集、回放验证、多视图成像和算法入口，是系统工程化落地的核心。' $C.Cyan | Out-Null

$slide = Add-Page $C.Paper
Add-Header $slide '03 SOFTWARE' '软件总体架构' '09' $C.Cyan
Add-ImageSlot $slide 58 118 392 248 '软件主界面截图' '放置采集软件主界面，最好包含 B-scan、右侧参数栏、开始采集按钮和状态显示。' $C.Cyan
Add-Card $slide 486 118 342 64 '开发框架' 'Qt/C++，使用 QCustomPlot 等控件完成雷达图像与波形显示。' $C.Cyan
Add-Card $slide 486 200 342 64 '功能模块' '实时采集、文件回放、处理设置、手动增益、时域波形、三维体数据。' $C.Blue
Add-Card $slide 486 282 342 64 '数据组织' '多通道复数数据转为时域序列，合成为 B-scan、俯视图和侧视图。' $C.Teal
Add-Card $slide 486 364 342 64 '工程特点' '实时与回放共用处理链，便于用历史数据复现实验现象和验证算法。' $C.Amber

$slide = Add-Page $C.Paper
Add-Header $slide '03 SOFTWARE' '实时采集流程' '10' $C.Cyan
Add-Flow $slide @('设备配置','开始采集','SDK 回调','通道合帧','图像刷新') 62 125 760 $C.Cyan
Add-Bullets $slide 76 230 385 150 @(
    '支持校准文件加载、频段、点数、触发模式与保存路径配置',
    '采集线程通过 SDK 回调获取数据，UI 线程以队列方式更新显示',
    '多通道帧合并后生成主视图、俯视图和可选侧视图'
) 14.5 $C.Ink | Out-Null
Add-ImageSlot $slide 500 230 318 148 '实时采集截图' '放置正在采集时的界面截图，突出实时滚动 B-scan 与状态栏。' $C.Cyan

$slide = Add-Page $C.Paper
Add-Header $slide '03 SOFTWARE' '回放与可视化功能' '11' $C.Cyan
Add-Card $slide 58 116 250 132 '文件回放' '读取历史雷达数据，按文件头恢复通道数、采样点、频率范围等参数。' $C.Cyan
Add-Card $slide 338 116 250 132 '图像显示' '支持滚动、缩放、颜色映射切换、局部数据替换和多视图联动。' $C.Blue
Add-Card $slide 618 116 250 132 '算法验证' '回放数据复用同一处理链，方便调节算法参数并对比处理前后结果。' $C.Teal
Add-ImageSlot $slide 58 292 375 145 '回放界面截图' '放置文件回放窗口，体现历史数据加载与 B-scan 浏览。' $C.Cyan
Add-ImageSlot $slide 480 292 348 145 '时域波形窗口' '放置单道 A-scan 波形窗口，用于展示信号质量和时域转换结果。' $C.Blue

Add-SectionSlide '04' '采集与探雷算法' '算法链路从频域数据转换开始，逐步完成背景抑制、地面校正和疑似目标增强。' $C.Amber | Out-Null

$slide = Add-Page $C.Paper
Add-Header $slide '04 ALGORITHM' '算法处理链路总览' '13' $C.Amber
Add-Flow $slide @('复数频域数据','CZT/SDK 转换','背景抑制','地面拉直','V2 目标增强') 50 122 780 $C.Amber
Add-Bullets $slide 68 222 370 168 @(
    'CZT 转换将频域复数数据映射为时域 A-scan，形成后续 B-scan 基础',
    '背景抑制降低强直达波和横向稳定杂波对目标回波的遮蔽',
    '趋势地面拉直减少地表起伏对浅层 ROI 的影响',
    'V2 谷值双曲检测面向浅层目标，输出局部增益和疑似区域增强'
) 14.3 $C.Ink | Out-Null
Add-ImageSlot $slide 494 226 330 150 '处理链路效果图' '放置同一数据的原始图、背景抑制后、V2 增强后三联图。' $C.Amber

$slide = Add-Page $C.Paper
Add-Header $slide '04 ALGORITHM' 'CZT 时域转换算法' '14' $C.Amber
Add-Card $slide 58 120 245 145 '输入' 'GPR SDK 返回的频域复数采样数据，包含多通道、多频点信息。' $C.Blue
Add-Card $slide 334 120 245 145 '方法' '采用 Kaiser 窗与 Chirp 卷积实现 CZT，获得指定时间窗内的时域响应。' $C.Amber
Add-Card $slide 610 120 245 145 '输出' '每个通道对应一条 A-scan，进一步按测线方向堆叠生成 B-scan 图像。' $C.Teal
Add-ImageSlot $slide 84 310 330 128 '频域/时域对比图' '放置同一道数据的频域幅相曲线与 CZT 后时域波形。' $C.Blue
Add-ImageSlot $slide 470 310 330 128 'B-scan 生成示意' '放置 A-scan 堆叠生成 B-scan 的示意图或实际截图。' $C.Amber

$slide = Add-Page $C.Paper
Add-Header $slide '04 ALGORITHM' '背景抑制与趋势地面拉直' '15' $C.Amber
Add-Card $slide 58 118 368 108 '背景抑制' '沿测线方向估计稳定背景分量，减弱直达波和横向连续杂波，保留局部异常回波。' $C.Blue
Add-Card $slide 58 250 368 108 '地面拉直' '估计地面到达时间趋势，将主层对齐到统一位置，使浅层 ROI 的检测条件更稳定。' $C.Teal
Add-ImageSlot $slide 482 112 345 130 '背景抑制前后' '左侧原始 B-scan，右侧背景抑制后 B-scan，保持同一色标。' $C.Blue
Add-ImageSlot $slide 482 292 345 130 '地面拉直前后' '放置地面层校正前后对比，并标出主层或 ROI 范围。' $C.Teal

$slide = Add-Page $C.Paper
Add-Header $slide '04 ALGORITHM' 'V2 谷值双曲目标检测与增强' '16' $C.Amber
Add-Bullets $slide 68 120 380 170 @(
    '面向 0-6 ns 浅层 ROI，主层约 2 ns，支持 R=200/300/400 等参数配置',
    '利用鲁棒标准差归一化，寻找黑/白响应种子点',
    '结合双曲线形态、连续性和左右翼对称性进行确认',
    '对确认区域实施局部增益，增强疑似目标而避免整幅图过度放大'
) 14.2 $C.Ink | Out-Null
Add-ImageSlot $slide 500 112 330 132 '检测示意图' '放置带有双曲线拟合或疑似目标框的 B-scan 截图。' $C.Amber
Add-ImageSlot $slide 500 284 330 132 '增强前后对比' '放置 V2 增益前后同一区域对比，标出增强框。' $C.Red

$slide = Add-Page $C.Paper
Add-Header $slide '04 COMPARISON' '探雷专栏算法对比与借鉴' '17' $C.Amber
$rows = @(
    @('算法/模块','核心思想','适用价值','本课题借鉴'),
    @('Hough 双曲检测','参数空间投票与形态约束','适合明显双曲目标识别','用于对比 V2 检测稳定性'),
    @('地面估计','包络阈值估计地面到达','校正浅层时间基准','对应趋势地面拉直思路'),
    @('高通背景抑制','测线方向移动平均背景扣除','削弱横向稳定杂波','作为预处理链路参考'),
    @('直达波校正','频域相位/直达波处理','改善早期强响应影响','为后续信号校正提供方向')
)
Add-Table $slide 58 118 @(145,245,205,225) $rows $C.Amber
Add-Text $slide 68 365 760 28 '对比重点：不是简单堆叠算法，而是选择能服务实时采集显示和浅层目标增强的处理环节。' 13 $C.Muted $false 1 $fontCN | Out-Null

$slide = Add-Page $C.Paper
Add-Header $slide '04 RESULT' '算法效果展示页模板' '18' $C.Amber
Add-ImageSlot $slide 52 112 250 250 '原始数据' '放置未处理 B-scan。' $C.Blue
Add-ImageSlot $slide 326 112 250 250 '预处理后' '放置背景抑制/地面拉直后的图。' $C.Teal
Add-ImageSlot $slide 600 112 250 250 'V2 增强后' '放置检测框和局部增益后的图。' $C.Amber
Add-Text $slide 72 392 758 36 '建议答辩时三张图使用同一测线、同一色标、同一时间窗，便于直接说明算法贡献。' 13 $C.Muted $false 2 $fontCN | Out-Null

Add-SectionSlide '05' '实验验证与总结展望' '汇总当前阶段已完成工作，说明不足，并给出后续硬件集成与实测验证计划。' $C.Red | Out-Null

$slide = Add-Page $C.Paper
Add-Header $slide '05 VALIDATION' '实验验证方案' '20' $C.Red
Add-Flow $slide @('天线测试','系统联调','标准目标采集','算法处理','指标评价') 58 122 780 $C.Red
Add-Bullets $slide 72 232 385 160 @(
    '天线测试：S11、工作频段、方向性或近场响应',
    '系统联调：SDK 参数、采集稳定性、数据保存与回放一致性',
    '目标采集：典型浅埋目标、不同深度、不同位置的可重复实验',
    '算法评价：目标增强可见性、误检情况、实时处理卡顿情况'
) 14.2 $C.Ink | Out-Null
Add-ImageSlot $slide 506 230 318 150 '实验布置图' '放置沙箱/测线/目标埋设位置示意，最好标注深度和间距。' $C.Red

$slide = Add-Page $C.Paper
Add-Header $slide '05 SUMMARY' '阶段成果总结' '21' $C.Red
Add-Card $slide 64 125 230 168 '硬件成果' '完成平面螺线天线设计与实物基础，为后续与小屏工控机集成提供前端基础。' $C.Teal
Add-Card $slide 334 125 230 168 '软件成果' '完成实时采集、回放、显示、参数设置和处理算法入口，形成可用的软件平台。' $C.Cyan
Add-Card $slide 604 125 230 168 '算法成果' '实现 CZT 转换、背景处理、趋势地面拉直和 V2 谷值双曲增强，支持实时链路验证。' $C.Amber
Add-Text $slide 82 345 740 42 '核心价值：把“硬件采集能力、软件实时显示能力、算法增强能力”整合为一个可实验、可迭代、可展示的研究平台。' 15 $C.Ink $true 2 $fontCN | Out-Null

$slide = Add-Page $C.Paper
Add-Header $slide '05 OUTLOOK' '不足与后续工作' '22' $C.Red
Add-Card $slide 66 120 352 105 '硬件集成' '补全小屏工控机、射频前端、供电与结构固定，完成整机便携化设计。' $C.Teal
Add-Card $slide 486 120 352 105 '实测数据' '增加标准目标和多场景测线，建立可复现实验数据集。' $C.Blue
Add-Card $slide 66 265 352 105 '算法鲁棒性' '继续优化浅层 ROI、增益框连续性、误检抑制和实时显示流畅度。' $C.Amber
Add-Card $slide 486 265 352 105 '软件工程化' '完善参数模板、结果导出、日志记录和答辩/论文所需图表生成能力。' $C.Red

$slide = Add-Page $C.Ink
Add-Box $slide 0 0 $W $H $C.Ink $C.Ink 0 | Out-Null
Add-Box $slide 0 0 $W 96 $C.Navy $C.Navy 0 | Out-Null
Add-Text $slide 96 158 730 76 '谢谢各位老师批评指正' 38 $C.White $true 2 $fontCN | Out-Null
Add-Line $slide 215 248 705 248 $C.Cyan 3 | Out-Null
Add-Text $slide 152 285 620 38 '探地雷达采集系统与目标增强算法研究' 18 (RGB 198 211 226) $false 2 $fontCN | Out-Null
Add-ImageSlot $slide 308 355 300 92 '结束页图片' '可放天线实物图、软件界面小截图或学校/实验室标识。' $C.Cyan

$presentation.SaveAs($outputPath, $ppSaveAsOpenXMLPresentation)
$presentation.Close()
$ppt.Quit()

[System.Runtime.InteropServices.Marshal]::ReleaseComObject($presentation) | Out-Null
[System.Runtime.InteropServices.Marshal]::ReleaseComObject($ppt) | Out-Null

$outputPath
