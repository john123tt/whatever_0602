#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QVector>
#include <vector>

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit OpenGLWidget(QWidget *parent = nullptr);
    ~OpenGLWidget();

    // 设置3D数据 (QVector版本)
    // data: QVector<QVector<QVector<double>>> 格式
    // data[channel][trace][sample], 数据范围应在[0,1]
    void setData3D(const QVector<QVector<QVector<double>>>& data);

    // 切片控制
    void setSliceMode(bool enabled);
    void setSlicePositionX(float pos);  // 0.0 - 1.0
    void setSlicePositionY(float pos);  // 0.0 - 1.0
    void setSlicePositionZ(float pos);  // 0.0 - 1.0
    void setSliceStep(float step);      // 设置切片移动步长 (0.0 - 1.0)
    bool isSliceModeEnabled() const { return m_sliceMode; }
    float getSliceStep() const { return m_sliceStep; }

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void setupShaders();
    void setupVertexData();
    void updateVertexData();

    // Shader程序
    QOpenGLShaderProgram *m_program;

    // VAO和VBO
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vbo;

    // 变换矩阵
    QMatrix4x4 m_projection;
    QMatrix4x4 m_view;
    QMatrix4x4 m_model;

    // 鼠标交互
    QPoint m_lastPos;
    float m_rotationX;
    float m_rotationY;
    float m_zoom;

    // 3D数据维度
    int m_channels;  // 通道数 (Z轴)
    int m_traces;    // 道数 (X轴)
    int m_samples;   // 采样点数 (Y轴)

    // 数据存储
    std::vector<float> m_seismicData;
    std::vector<float> m_vertices;

    int m_vertexCount;

    // 切片模式
    bool m_sliceMode;
    float m_sliceX;  // 切片位置 0.0 - 1.0
    float m_sliceY;
    float m_sliceZ;
    float m_sliceStep;  // 切片移动步长
};

#endif // OPENGLWIDGET_H
