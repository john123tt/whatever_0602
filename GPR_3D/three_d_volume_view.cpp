#include "Three_D_Volume_view.h"
#include <QVBoxLayout>

Three_D_Volume_view::Three_D_Volume_view(QWidget *parent)
    : QWidget(parent)
    , m_glWidget(nullptr)
{
    // 创建布局
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    // 创建OpenGL渲染窗口
    m_glWidget = new OpenGLWidget(this);
    layout->addWidget(m_glWidget);

    setLayout(layout);

    // 设置默认大小
    resize(800, 600);
}

Three_D_Volume_view::~Three_D_Volume_view()
{
    // QWidget会自动删除子控件
}

void Three_D_Volume_view::setData(const QVector<QVector<QVector<double>>>& data)
{
    if (m_glWidget) {
        m_glWidget->setData3D(data);
    }
}

void Three_D_Volume_view::clearData()
{
    if (m_glWidget) {
        QVector<QVector<QVector<double>>> emptyData;
        emptyData.resize(1);
        emptyData[0].resize(1);
        emptyData[0][0].resize(1);
        emptyData[0][0][0] = 0.5;
        m_glWidget->setData3D(emptyData);
    }
}

OpenGLWidget* Three_D_Volume_view::getGLWidget()
{
    return m_glWidget;
}
