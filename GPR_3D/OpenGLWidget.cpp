#include "OpenGLWidget.h"
#include <QDebug>
#include <QKeyEvent>
#include <cmath>

OpenGLWidget::OpenGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_program(nullptr)
    , m_vbo(QOpenGLBuffer::VertexBuffer)
    , m_rotationX(180.0f)
    , m_rotationY(0.0f)
    , m_zoom(1.5f)
    , m_channels(20)
    , m_traces(50)
    , m_samples(200)
    , m_vertexCount(0)
    , m_sliceMode(false)
    , m_sliceX(0.5f)
    , m_sliceY(0.5f)
    , m_sliceZ(0.5f)
    , m_sliceStep(0.02f)
{
    // 设置OpenGL格式
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    // 启用多重采样抗锯齿(MSAA)
    format.setSamples(4);  // 4x MSAA
    setFormat(format);

    // 设置焦点策略以接收键盘事件
    setFocusPolicy(Qt::StrongFocus);
}

OpenGLWidget::~OpenGLWidget()
{
    makeCurrent();
    m_vbo.destroy();
    m_vao.destroy();
    delete m_program;
    doneCurrent();
}

void OpenGLWidget::setData3D(const QVector<QVector<QVector<double>>>& data)
{
    if (data.isEmpty()) {
        qWarning() << "输入数据为空!";
        return;
    }

    // 获取维度
    m_channels = data.size();
    if (m_channels == 0) return;

    m_traces = data[0].size();
    if (m_traces == 0) return;

    m_samples = data[0][0].size();
    if (m_samples == 0) return;

    // 转换数据到内部格式
    m_seismicData.clear();
    m_seismicData.resize(m_channels * m_traces * m_samples);

    for (int ch = 0; ch < m_channels; ++ch) {
        for (int tr = 0; tr < m_traces; ++tr) {
            for (int s = 0; s < m_samples; ++s) {
                int idx = ch * m_traces * m_samples + tr * m_samples + s;
                double value = qMax(0.0, qMin(1.0, data[ch][tr][s]));
                m_seismicData[idx] = static_cast<float>(value);
            }
        }
    }

    // 更新顶点数据并重绘
    updateVertexData();
    update();
}

void OpenGLWidget::setSliceMode(bool enabled)
{
    m_sliceMode = enabled;
    updateVertexData();
    update();
    qDebug() << "切片模式:" << (enabled ? "开启" : "关闭");
}

void OpenGLWidget::setSlicePositionX(float pos)
{
    m_sliceX = qBound(0.0f, pos, 1.0f);
    if (m_sliceMode) {
        updateVertexData();
        update();
    }
}

void OpenGLWidget::setSlicePositionY(float pos)
{
    m_sliceY = qBound(0.0f, pos, 1.0f);
    if (m_sliceMode) {
        updateVertexData();
        update();
    }
}

void OpenGLWidget::setSlicePositionZ(float pos)
{
    m_sliceZ = qBound(0.0f, pos, 1.0f);
    if (m_sliceMode) {
        updateVertexData();
        update();
    }
}

void OpenGLWidget::setSliceStep(float step)
{
    m_sliceStep = qBound(0.001f, step, 0.5f);  // 限制在0.1%-50%之间
    qDebug() << "切片移动步长设置为:" << m_sliceStep;
}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    // 禁用背面剔除以支持切片双面渲染
    glDisable(GL_CULL_FACE);
    // 启用多重采样抗锯齿
    glEnable(GL_MULTISAMPLE);

    setupShaders();

    // 创建VAO
    m_vao.create();
    m_vao.bind();

    // 创建VBO
    m_vbo.create();
    m_vbo.bind();

    // 初始化空数据，等待用户加载
    m_seismicData.clear();
    m_seismicData.resize(m_channels * m_traces * m_samples, 0.5f);
    setupVertexData();

    m_vao.release();
}

void OpenGLWidget::setupShaders()
{
    m_program = new QOpenGLShaderProgram(this);

    // 顶点着色器
    const char *vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aColor;

        out vec3 vertexColor;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        void main()
        {
            gl_Position = projection * view * model * vec4(aPos, 1.0);
            vertexColor = aColor;
        }
    )";

    // 片段着色器
    const char *fragmentShaderSource = R"(
        #version 330 core
        in vec3 vertexColor;
        out vec4 FragColor;

        void main()
        {
            FragColor = vec4(vertexColor, 1.0);
        }
    )";

    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();
    m_program->bind();
}

void OpenGLWidget::setupVertexData()
{
    updateVertexData();
}

void OpenGLWidget::updateVertexData()
{
    if (!isValid())
        return;

    // 🔥 强制切换到当前 OpenGL 上下文
    makeCurrent();

    // 🔥 VAO 必须绑定，否则 attribute 会写到“未知对象”
    m_vao.bind();

    m_vertices.clear();

    // 归一化参数
    float xScale = 8.0f / m_traces;
    float yScale = 2.0f / m_samples;
    float zScale = 1.0f / m_channels;

    // 辅助函数: 添加一个四边形(两个三角形)
    auto addQuad = [&](float x1, float y1, float z1, float c1,
                       float x2, float y2, float z2, float c2,
                       float x3, float y3, float z3, float c3,
                       float x4, float y4, float z4, float c4) {
        // 三角形1: v1, v2, v3
        m_vertices.insert(m_vertices.end(), {x1, y1, z1, c1, c1, c1});
        m_vertices.insert(m_vertices.end(), {x2, y2, z2, c2, c2, c2});
        m_vertices.insert(m_vertices.end(), {x3, y3, z3, c3, c3, c3});
        // 三角形2: v1, v3, v4
        m_vertices.insert(m_vertices.end(), {x1, y1, z1, c1, c1, c1});
        m_vertices.insert(m_vertices.end(), {x3, y3, z3, c3, c3, c3});
        m_vertices.insert(m_vertices.end(), {x4, y4, z4, c4, c4, c4});
    };

    if (m_sliceMode) {
        // 切片模式：渲染三个相互垂直的切片平面

        // 计算切片位置
        int sliceTraceIdx = static_cast<int>(m_sliceX * (m_traces - 1));
        int sliceSampleIdx = static_cast<int>(m_sliceY * (m_samples - 1));
        int sliceChannelIdx = static_cast<int>(m_sliceZ * (m_channels - 1));

        // YZ平面切片 (固定X, 垂直于trace方向)
        float x = sliceTraceIdx * xScale - 4.0f;
        for (int ch = 0; ch < m_channels - 1; ++ch) {
            for (int s = 0; s < m_samples - 1; ++s) {
                int idx1 = ch * m_traces * m_samples + sliceTraceIdx * m_samples + s;
                int idx2 = (ch + 1) * m_traces * m_samples + sliceTraceIdx * m_samples + s;
                int idx3 = (ch + 1) * m_traces * m_samples + sliceTraceIdx * m_samples + (s + 1);
                int idx4 = ch * m_traces * m_samples + sliceTraceIdx * m_samples + (s + 1);

                float z1 = ch * zScale - 0.5f;
                float y1 = s * yScale - 1.0f;
                float c1 = qBound(0.0f, m_seismicData[idx1], 1.0f);

                float z2 = (ch + 1) * zScale - 0.5f;
                float y2 = s * yScale - 1.0f;
                float c2 = qBound(0.0f, m_seismicData[idx2], 1.0f);

                float z3 = (ch + 1) * zScale - 0.5f;
                float y3 = (s + 1) * yScale - 1.0f;
                float c3 = qBound(0.0f, m_seismicData[idx3], 1.0f);

                float z4 = ch * zScale - 0.5f;
                float y4 = (s + 1) * yScale - 1.0f;
                float c4 = qBound(0.0f, m_seismicData[idx4], 1.0f);

                // 双面渲染
                addQuad(x, y1, z1, c1, x, y2, z2, c2, x, y3, z3, c3, x, y4, z4, c4);
                addQuad(x, y1, z1, c1, x, y4, z4, c4, x, y3, z3, c3, x, y2, z2, c2);
            }
        }

        // 2. XZ平面切片 (固定Y, 垂直于sample方向)
        float y = sliceSampleIdx * yScale - 1.0f;
        for (int ch = 0; ch < m_channels - 1; ++ch) {
            for (int tr = 0; tr < m_traces - 1; ++tr) {
                int idx1 = ch * m_traces * m_samples + tr * m_samples + sliceSampleIdx;
                int idx2 = ch * m_traces * m_samples + (tr + 1) * m_samples + sliceSampleIdx;
                int idx3 = (ch + 1) * m_traces * m_samples + (tr + 1) * m_samples + sliceSampleIdx;
                int idx4 = (ch + 1) * m_traces * m_samples + tr * m_samples + sliceSampleIdx;

                float x1 = tr * xScale - 4.0f;
                float z1 = ch * zScale - 0.5f;
                float c1 = qBound(0.0f, m_seismicData[idx1], 1.0f);

                float x2 = (tr + 1) * xScale - 4.0f;
                float z2 = ch * zScale - 0.5f;
                float c2 = qBound(0.0f, m_seismicData[idx2], 1.0f);

                float x3 = (tr + 1) * xScale - 4.0f;
                float z3 = (ch + 1) * zScale - 0.5f;
                float c3 = qBound(0.0f, m_seismicData[idx3], 1.0f);

                float x4 = tr * xScale - 4.0f;
                float z4 = (ch + 1) * zScale - 0.5f;
                float c4 = qBound(0.0f, m_seismicData[idx4], 1.0f);

                // 双面渲染
                // 上面：逆时针
                addQuad(x1, y, z1, c1, x4, y, z4, c4, x3, y, z3, c3, x2, y, z2, c2);
                // 下面：顺时针
                addQuad(x1, y, z1, c1, x2, y, z2, c2, x3, y, z3, c3, x4, y, z4, c4);
            }
        }

        // XY平面切片 (固定Z, 垂直于channel方向)
        float z = sliceChannelIdx * zScale - 0.5f;
        for (int tr = 0; tr < m_traces - 1; ++tr) {
            for (int s = 0; s < m_samples - 1; ++s) {
                int idx1 = sliceChannelIdx * m_traces * m_samples + tr * m_samples + s;
                int idx2 = sliceChannelIdx * m_traces * m_samples + (tr + 1) * m_samples + s;
                int idx3 = sliceChannelIdx * m_traces * m_samples + (tr + 1) * m_samples + (s + 1);
                int idx4 = sliceChannelIdx * m_traces * m_samples + tr * m_samples + (s + 1);

                float x1 = tr * xScale - 4.0f;
                float y1 = s * yScale - 1.0f;
                float c1 = qBound(0.0f, m_seismicData[idx1], 1.0f);

                float x2 = (tr + 1) * xScale - 4.0f;
                float y2 = s * yScale - 1.0f;
                float c2 = qBound(0.0f, m_seismicData[idx2], 1.0f);

                float x3 = (tr + 1) * xScale - 4.0f;
                float y3 = (s + 1) * yScale - 1.0f;
                float c3 = qBound(0.0f, m_seismicData[idx3], 1.0f);

                float x4 = tr * xScale - 4.0f;
                float y4 = (s + 1) * yScale - 1.0f;
                float c4 = qBound(0.0f, m_seismicData[idx4], 1.0f);

                // 双面渲染
                addQuad(x1, y1, z, c1, x4, y4, z, c4, x3, y3, z, c3, x2, y2, z, c2);
                addQuad(x1, y1, z, c1, x2, y2, z, c2, x3, y3, z, c3, x4, y4, z, c4);
            }
        }

    } else {
        // 正常模式：渲染立方体的六个面

        // 1. 前后两个面 (Z方向)
        for (int ch_idx = 0; ch_idx < 2; ++ch_idx) {
            int ch = (ch_idx == 0) ? 0 : (m_channels - 1);
            float z = ch * zScale - 0.5f;

            for (int tr = 0; tr < m_traces - 1; ++tr) {
                for (int s = 0; s < m_samples - 1; ++s) {
                    // 四个角的顶点
                    int idx1 = ch * m_traces * m_samples + tr * m_samples + s;
                    int idx2 = ch * m_traces * m_samples + (tr + 1) * m_samples + s;
                    int idx3 = ch * m_traces * m_samples + (tr + 1) * m_samples + (s + 1);
                    int idx4 = ch * m_traces * m_samples + tr * m_samples + (s + 1);

                    float x1 = tr * xScale - 4.0f;
                    float y1 = s * yScale - 1.0f;
                    float c1 = qBound(0.0f, m_seismicData[idx1], 1.0f);

                    float x2 = (tr + 1) * xScale - 4.0f;
                    float y2 = s * yScale - 1.0f;
                    float c2 = qBound(0.0f, m_seismicData[idx2], 1.0f);

                    float x3 = (tr + 1) * xScale - 4.0f;
                    float y3 = (s + 1) * yScale - 1.0f;
                    float c3 = qBound(0.0f, m_seismicData[idx3], 1.0f);

                    float x4 = tr * xScale - 4.0f;
                    float y4 = (s + 1) * yScale - 1.0f;
                    float c4 = qBound(0.0f, m_seismicData[idx4], 1.0f);

                    if (ch_idx == 0) {
                        // 前面：逆时针顺序
                        addQuad(x1, y1, z, c1, x4, y4, z, c4, x3, y3, z, c3, x2, y2, z, c2);
                    } else {
                        // 后面：顺时针顺序
                        addQuad(x1, y1, z, c1, x2, y2, z, c2, x3, y3, z, c3, x4, y4, z, c4);
                    }
                }
            }
        }

        // 2. 左右两个面 (X方向)
        for (int tr_idx = 0; tr_idx < 2; ++tr_idx) {
            int tr = (tr_idx == 0) ? 0 : (m_traces - 1);
            float x = tr * xScale - 4.0f;

            for (int ch = 0; ch < m_channels - 1; ++ch) {
                for (int s = 0; s < m_samples - 1; ++s) {
                    int idx1 = ch * m_traces * m_samples + tr * m_samples + s;
                    int idx2 = (ch + 1) * m_traces * m_samples + tr * m_samples + s;
                    int idx3 = (ch + 1) * m_traces * m_samples + tr * m_samples + (s + 1);
                    int idx4 = ch * m_traces * m_samples + tr * m_samples + (s + 1);

                    float z1 = ch * zScale - 0.5f;
                    float y1 = s * yScale - 1.0f;
                    float c1 = qBound(0.0f, m_seismicData[idx1], 1.0f);

                    float z2 = (ch + 1) * zScale - 0.5f;
                    float y2 = s * yScale - 1.0f;
                    float c2 = qBound(0.0f, m_seismicData[idx2], 1.0f);

                    float z3 = (ch + 1) * zScale - 0.5f;
                    float y3 = (s + 1) * yScale - 1.0f;
                    float c3 = qBound(0.0f, m_seismicData[idx3], 1.0f);

                    float z4 = ch * zScale - 0.5f;
                    float y4 = (s + 1) * yScale - 1.0f;
                    float c4 = qBound(0.0f, m_seismicData[idx4], 1.0f);

                    if (tr_idx == 0) {
                        // 左面：逆时针
                        addQuad(x, y1, z1, c1, x, y2, z2, c2, x, y3, z3, c3, x, y4, z4, c4);
                    } else {
                        // 右面：顺时针
                        addQuad(x, y1, z1, c1, x, y4, z4, c4, x, y3, z3, c3, x, y2, z2, c2);
                    }
                }
            }
        }

        // 3. 上下两个面 (Y方向)
        for (int s_idx = 0; s_idx < 2; ++s_idx) {
            int s = (s_idx == 0) ? 0 : (m_samples - 1);
            float y = s * yScale - 1.0f;

            for (int ch = 0; ch < m_channels - 1; ++ch) {
                for (int tr = 0; tr < m_traces - 1; ++tr) {
                    int idx1 = ch * m_traces * m_samples + tr * m_samples + s;
                    int idx2 = ch * m_traces * m_samples + (tr + 1) * m_samples + s;
                    int idx3 = (ch + 1) * m_traces * m_samples + (tr + 1) * m_samples + s;
                    int idx4 = (ch + 1) * m_traces * m_samples + tr * m_samples + s;

                    float x1 = tr * xScale - 4.0f;
                    float z1 = ch * zScale - 0.5f;
                    float c1 = qBound(0.0f, m_seismicData[idx1], 1.0f);

                    float x2 = (tr + 1) * xScale - 4.0f;
                    float z2 = ch * zScale - 0.5f;
                    float c2 = qBound(0.0f, m_seismicData[idx2], 1.0f);

                    float x3 = (tr + 1) * xScale - 4.0f;
                    float z3 = (ch + 1) * zScale - 0.5f;
                    float c3 = qBound(0.0f, m_seismicData[idx3], 1.0f);

                    float x4 = tr * xScale - 4.0f;
                    float z4 = (ch + 1) * zScale - 0.5f;
                    float c4 = qBound(0.0f, m_seismicData[idx4], 1.0f);

                    if (s_idx == 0) {
                        // 底面：逆时针
                        addQuad(x1, y, z1, c1, x2, y, z2, c2, x3, y, z3, c3, x4, y, z4, c4);
                    } else {
                        // 顶面：顺时针
                        addQuad(x1, y, z1, c1, x4, y, z4, c4, x3, y, z3, c3, x2, y, z2, c2);
                    }
                }
            }
        }
    }

    m_vertexCount = m_vertices.size() / 6;

    qDebug() << "顶点数量:" << m_vertexCount << "内存使用:" << (m_vertices.size() * sizeof(float) / 1024.0f) << "KB";

    // 更新VBO数据
    // m_vbo.bind();
    // m_vbo.allocate(m_vertices.data(), m_vertices.size() * sizeof(float));

    // // 设置顶点属性指针
    // m_program->enableAttributeArray(0);
    // m_program->setAttributeBuffer(0, GL_FLOAT, 0, 3, 6 * sizeof(float));

    // m_program->enableAttributeArray(1);
    // m_program->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(float), 3, 6 * sizeof(float));

    // m_vbo.release();
    m_vbo.bind();
    m_vbo.allocate(
        m_vertices.data(),
        static_cast<int>(m_vertices.size() * sizeof(float))
        );

    // 顶点坐标
    m_program->enableAttributeArray(0);
    m_program->setAttributeBuffer(
        0, GL_FLOAT, 0, 3, 6 * sizeof(float)
        );

    // 颜色
    m_program->enableAttributeArray(1);
    m_program->setAttributeBuffer(
        1, GL_FLOAT, 3 * sizeof(float), 3, 6 * sizeof(float)
        );

    m_vbo.release();
    m_vao.release();

    // 🔥 释放上下文（防止析构阶段继续误用）
    doneCurrent();
}

void OpenGLWidget::resizeGL(int w, int h)
{
    m_projection.setToIdentity();
    m_projection.perspective(45.0f, (float)w / (float)h, 0.1f, 100.0f);
}

void OpenGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_program->bind();

    // 设置视图矩阵
    m_view.setToIdentity();
    m_view.translate(0.0f, 0.0f, -5.0f * m_zoom);

    // 设置模型矩阵
    m_model.setToIdentity();
    m_model.rotate(m_rotationX, 1.0f, 0.0f, 0.0f);
    m_model.rotate(m_rotationY, 0.0f, 1.0f, 0.0f);

    // 传递矩阵到着色器
    m_program->setUniformValue("projection", m_projection);
    m_program->setUniformValue("view", m_view);
    m_program->setUniformValue("model", m_model);

    m_vao.bind();

    // 绘制所有三角形(实心面)
    glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);

    m_vao.release();
    m_program->release();
}

void OpenGLWidget::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - m_lastPos.x();
    int dy = event->y() - m_lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        m_rotationX += dy * 0.5f;
        m_rotationY += dx * 0.5f;
        update();
    }

    m_lastPos = event->pos();
}

void OpenGLWidget::wheelEvent(QWheelEvent *event)
{
    float delta = event->angleDelta().y() / 120.0f;
    m_zoom -= delta * 0.1f;
    m_zoom = qMax(0.5f, qMin(m_zoom, 5.0f));
    update();
}

void OpenGLWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
        case Qt::Key_S:
            // 切换切片模式
            setSliceMode(!m_sliceMode);
            break;

        // X方向切片控制 (A/D键)
        case Qt::Key_A:
            setSlicePositionX(m_sliceX - m_sliceStep);
            qDebug() << "X切片位置:" << m_sliceX;
            break;
        case Qt::Key_D:
            setSlicePositionX(m_sliceX + m_sliceStep);
            qDebug() << "X切片位置:" << m_sliceX;
            break;

        // Y方向切片控制 (W/X键)
        case Qt::Key_W:
            setSlicePositionY(m_sliceY - m_sliceStep);
            qDebug() << "Y切片位置:" << m_sliceY;
            break;
        case Qt::Key_X:
            setSlicePositionY(m_sliceY + m_sliceStep);
            qDebug() << "Y切片位置:" << m_sliceY;
            break;

        // Z方向切片控制 (Q/E键)
        case Qt::Key_Q:
            setSlicePositionZ(m_sliceZ - m_sliceStep);
            qDebug() << "Z切片位置:" << m_sliceZ;
            break;
        case Qt::Key_E:
            setSlicePositionZ(m_sliceZ + m_sliceStep);
            qDebug() << "Z切片位置:" << m_sliceZ;
            break;

        // 调整移动步长 (+/-键)
        case Qt::Key_Plus:
        case Qt::Key_Equal:  // 不按Shift的+键
            setSliceStep(m_sliceStep * 1.5f);
            break;
        case Qt::Key_Minus:
        case Qt::Key_Underscore:
            setSliceStep(m_sliceStep / 1.5f);
            break;

        default:
            QOpenGLWidget::keyPressEvent(event);
            break;
    }
}
