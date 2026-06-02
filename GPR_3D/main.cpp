#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("GPR_3D");
    QCoreApplication::setApplicationName("GPR_3D");
    MainWindow w;
    w.setWindowTitle("GPR_3D");
    w.setWindowState(Qt::WindowMaximized);
    w.show();
    return a.exec();
}
