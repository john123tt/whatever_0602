DEFINES += QCUSTOMPLOT_USE_OPENGL
QT       += core gui widgets opengl openglwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    qcustomplot.cpp \
    mainwindow.cpp \
    image2dplot.cpp \
    rangeslider.cpp \
    three_d_volume_dialog.cpp \
    three_d_volume_view.cpp \
    timedomainplot.cpp \
    utility.cpp \
    filecachedcolormap.cpp \
    realtimeacquisition.cpp \
    realtimeprocessingsettingview.cpp \
    adjustgainsettingdialog.cpp \
    manualadjustgainaxisticker.cpp \
    OpenGLWidget.cpp \
    backgroundsubtraction.cpp \
    filereplayview.cpp \
    v2hyperboladetector.cpp

HEADERS += \
    qcustomplot.h \
    mainwindow.h \
    image2dplot.h \
    rangeslider.h \
    three_d_volume_dialog.h \
    three_d_volume_view.h \
    timedomainplot.h \
    global.h \
    utility.h \
    setting.h \
    filecachedcolormap.h \
    realtimeacquisition.h \
    realtimeprocessingsettingview.h \
    adjustgainsettingdialog.h \
    manualadjustgainaxisticker.h \
    OpenGLWidget.h \
    backgroundsubtraction.h \
    filereplayview.h \
    v2hyperboladetector.h

FORMS += \
    mainwindow.ui \
    realtimeacquisition.ui \
    realtimeprocessingsettingview.ui \
    adjustgainsettingdialog.ui \
    filereplayview.ui \
    three_d_volume_dialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RC_ICONS += logo.ico

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../GPR_SDK/bin/lib -lGPR_SDK
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../GPR_SDK/bin/lib -lGPR_SDKd
else:unix:!macx: LIBS += -L$$BUILD_BIN_DIR/../GPR_SDK/bin/lib -lGPR_SDK

INCLUDEPATH += $$PWD/../GPR_SDK/bin/include
DEPENDPATH += $$PWD/../GPR_SDK/bin/lib
DEFINES += PROJECT_ROOT=\\\"$$PWD\\\"
LIBS += opengl32.lib

RESOURCES += \
    icons.qrc
