#include "three_d_volume_dialog.h"
#include "ui_three_d_volume_dialog.h"
#include <QLayout>

static QVector<QVector<QVector<double>>>
limitAndDownsample(
    const QVector<QVector<QVector<double>>>& src,
    int maxChannels,
    int maxTraces,
    int maxSamples)
{
    QVector<QVector<QVector<double>>> dst;

    if (src.isEmpty() ||
        src[0].isEmpty() ||
        src[0][0].isEmpty())
        return dst;

    int srcC = src.size();
    int srcT = src[0].size();
    int srcS = src[0][0].size();

    int stepC = qMax(1, srcC / maxChannels);
    int stepT = qMax(1, srcT / maxTraces);
    int stepS = qMax(1, srcS / maxSamples);

    int outC = srcC / stepC;
    int outT = srcT / stepT;
    int outS = srcS / stepS;

    dst.resize(outC);
    for (int c = 0; c < outC; ++c) {
        dst[c].resize(outT);
        for (int t = 0; t < outT; ++t) {
            dst[c][t].resize(outS);
        }
    }

    int oc = 0;
    for (int c = 0; c < srcC && oc < outC; c += stepC, ++oc) {
        int ot = 0;
        for (int t = 0; t < srcT && ot < outT; t += stepT, ++ot) {
            int os = 0;
            for (int s = 0; s < srcS && os < outS; s += stepS, ++os) {
                dst[oc][ot][os] = src[c][t][s];
            }
        }
    }

    qDebug() << "[3D Volume Downsample]"
             << "C:" << srcC << "→" << outC
             << "T:" << srcT << "→" << outT
             << "S:" << srcS << "→" << outS;

    return dst;
}


Three_D_Volume_Dialog::Three_D_Volume_Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Three_D_Volume_Dialog)
{
    ui->setupUi(this);
    setWindowTitle("3D图像");
    init_layout();
}

Three_D_Volume_Dialog::~Three_D_Volume_Dialog()
{
    delete ui;
}

void Three_D_Volume_Dialog::init_layout()
{
    // m_three_d_volume_view = new Three_D_Volume_view(this);
    // auto vLayout = new QVBoxLayout();
    // vLayout->addWidget(m_three_d_volume_view);


    // 创建3D视图
    m_three_d_volume_view = new Three_D_Volume_view(this);

    // 设置视图大小策略（关键！）
    m_three_d_volume_view->setSizePolicy(
        QSizePolicy::Expanding,  // 水平方向扩展
        QSizePolicy::Expanding   // 垂直方向扩展
        );

    // 创建主布局并设置给对话框
    auto vLayout = new QVBoxLayout(this);

    // 设置边距（提供左右间距）
    vLayout->setContentsMargins(20, 20, 20, 20);  // 左右20像素间距

    // 添加3D视图到布局
    vLayout->addWidget(m_three_d_volume_view);

    // 设置布局的对齐方式（使视图在布局中居中）
    vLayout->setAlignment(Qt::AlignCenter);

    // 如果希望视图充满整个空间（不留边距），可以使用：
    // vLayout->setContentsMargins(0, 0, 0, 0);

    // 设置布局
    setLayout(vLayout);

}

void Three_D_Volume_Dialog::create_and_set_volume_data(const QVector<QVector<QVector<double>>>& raw_data_all_channel)
{
    if (m_three_d_volume_view) {
        qDebug() << "开始生成模拟3D体数据...";
        constexpr int MAX_CHANNELS = 10;
        constexpr int MAX_TRACES   = 1000;
        constexpr int MAX_SAMPLES  = 1024;
        auto safeData = limitAndDownsample
            (
                raw_data_all_channel,
                MAX_CHANNELS,
                MAX_TRACES,
                MAX_SAMPLES
                );

        m_three_d_volume_view->setData(safeData);
    }
}
