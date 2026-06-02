#ifndef Three_D_Volume_view_H
#define Three_D_Volume_view_H

#include <QWidget>
#include <QVector>
#include "OpenGLWidget.h"


class Three_D_Volume_view : public QWidget
{
    Q_OBJECT

public:
    explicit Three_D_Volume_view(QWidget *parent = nullptr);
    ~Three_D_Volume_view();


    void setData(const QVector<QVector<QVector<double>>>& data);


    void clearData();


    OpenGLWidget* getGLWidget();

private:
    OpenGLWidget* m_glWidget;
};

#endif // Three_D_Volume_view_H
