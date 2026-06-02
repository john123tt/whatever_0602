/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionnihao;
    QWidget *centralwidget;
    QGroupBox *eliminate_background_group_box;
    QGridLayout *gridLayout;
    QGridLayout *eliminate_background_grid_layout;
    QLabel *eliminate_background_label;
    QLabel *background_frames_label;
    QComboBox *auto_adjust_gain_combo_box;
    QComboBox *eliminate_background_combo_box;
    QLabel *map_k_label;
    QSlider *map_k_slider;
    QLabel *map_k_value_label;
    QSlider *max_depth_slider;
    QHBoxLayout *horizontalLayout_3;
    QSpinBox *max_depth_value_spin_box;
    QLabel *max_depth_label;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(805, 600);
        actionnihao = new QAction(MainWindow);
        actionnihao->setObjectName("actionnihao");
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        eliminate_background_group_box = new QGroupBox(centralwidget);
        eliminate_background_group_box->setObjectName("eliminate_background_group_box");
        eliminate_background_group_box->setGeometry(QRect(550, 410, 204, 158));
        eliminate_background_group_box->setFlat(true);
        gridLayout = new QGridLayout(eliminate_background_group_box);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(-1, 30, -1, -1);
        eliminate_background_grid_layout = new QGridLayout();
        eliminate_background_grid_layout->setObjectName("eliminate_background_grid_layout");
        eliminate_background_grid_layout->setHorizontalSpacing(3);
        eliminate_background_grid_layout->setVerticalSpacing(6);
        eliminate_background_label = new QLabel(eliminate_background_group_box);
        eliminate_background_label->setObjectName("eliminate_background_label");

        eliminate_background_grid_layout->addWidget(eliminate_background_label, 0, 0, 1, 1);

        background_frames_label = new QLabel(eliminate_background_group_box);
        background_frames_label->setObjectName("background_frames_label");

        eliminate_background_grid_layout->addWidget(background_frames_label, 1, 0, 1, 1);

        auto_adjust_gain_combo_box = new QComboBox(eliminate_background_group_box);
        auto_adjust_gain_combo_box->addItem(QString());
        auto_adjust_gain_combo_box->addItem(QString());
        auto_adjust_gain_combo_box->setObjectName("auto_adjust_gain_combo_box");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(auto_adjust_gain_combo_box->sizePolicy().hasHeightForWidth());
        auto_adjust_gain_combo_box->setSizePolicy(sizePolicy);
        auto_adjust_gain_combo_box->setStyleSheet(QString::fromUtf8("QComboBox {\n"
"    background-color: white;\n"
"    color: black;\n"
"    border: 1px solid #cccccc;\n"
"    border-radius: 4px;\n"
"    padding: 3px 10px 3px 5px;\n"
"}\n"
"QComboBox QAbstractItemView {\n"
"    background-color: white;\n"
"    color: black;\n"
"    border: 1px solid #cccccc;\n"
"    selection-background-color: #e0e0e0;\n"
"    selection-color: black;\n"
"}\n"
""));

        eliminate_background_grid_layout->addWidget(auto_adjust_gain_combo_box, 1, 1, 1, 1);

        eliminate_background_combo_box = new QComboBox(eliminate_background_group_box);
        eliminate_background_combo_box->addItem(QString());
        eliminate_background_combo_box->addItem(QString());
        eliminate_background_combo_box->setObjectName("eliminate_background_combo_box");
        sizePolicy.setHeightForWidth(eliminate_background_combo_box->sizePolicy().hasHeightForWidth());
        eliminate_background_combo_box->setSizePolicy(sizePolicy);
        eliminate_background_combo_box->setStyleSheet(QString::fromUtf8("QComboBox {\n"
"    background-color: white;\n"
"    color: black;\n"
"    border: 1px solid #cccccc;\n"
"    border-radius: 4px;\n"
"    padding: 3px 10px 3px 5px;\n"
"}\n"
"QComboBox QAbstractItemView {\n"
"    background-color: white;\n"
"    color: black;\n"
"    border: 1px solid #cccccc;\n"
"    selection-background-color: #e0e0e0;\n"
"    selection-color: black;\n"
"}\n"
""));

        eliminate_background_grid_layout->addWidget(eliminate_background_combo_box, 0, 1, 1, 1);

        map_k_label = new QLabel(eliminate_background_group_box);
        map_k_label->setObjectName("map_k_label");

        eliminate_background_grid_layout->addWidget(map_k_label, 3, 0, 1, 1);

        map_k_slider = new QSlider(eliminate_background_group_box);
        map_k_slider->setObjectName("map_k_slider");
        map_k_slider->setOrientation(Qt::Horizontal);

        eliminate_background_grid_layout->addWidget(map_k_slider, 3, 1, 1, 1);

        map_k_value_label = new QLabel(eliminate_background_group_box);
        map_k_value_label->setObjectName("map_k_value_label");
        map_k_value_label->setAlignment(Qt::AlignCenter);

        eliminate_background_grid_layout->addWidget(map_k_value_label, 3, 2, 1, 1);

        max_depth_slider = new QSlider(eliminate_background_group_box);
        max_depth_slider->setObjectName("max_depth_slider");
        max_depth_slider->setOrientation(Qt::Horizontal);

        eliminate_background_grid_layout->addWidget(max_depth_slider, 2, 1, 1, 1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        max_depth_value_spin_box = new QSpinBox(eliminate_background_group_box);
        max_depth_value_spin_box->setObjectName("max_depth_value_spin_box");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(max_depth_value_spin_box->sizePolicy().hasHeightForWidth());
        max_depth_value_spin_box->setSizePolicy(sizePolicy1);
        max_depth_value_spin_box->setWrapping(false);
        max_depth_value_spin_box->setFrame(true);
        max_depth_value_spin_box->setAlignment(Qt::AlignCenter);
        max_depth_value_spin_box->setButtonSymbols(QAbstractSpinBox::NoButtons);

        horizontalLayout_3->addWidget(max_depth_value_spin_box);


        eliminate_background_grid_layout->addLayout(horizontalLayout_3, 2, 2, 1, 1);

        max_depth_label = new QLabel(eliminate_background_group_box);
        max_depth_label->setObjectName("max_depth_label");

        eliminate_background_grid_layout->addWidget(max_depth_label, 2, 0, 1, 1);


        gridLayout->addLayout(eliminate_background_grid_layout, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        auto_adjust_gain_combo_box->setCurrentIndex(1);
        eliminate_background_combo_box->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        actionnihao->setText(QCoreApplication::translate("MainWindow", "nihao", nullptr));
        eliminate_background_group_box->setTitle(QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256\345\244\204\347\220\206", nullptr));
        eliminate_background_label->setText(QCoreApplication::translate("MainWindow", "\350\203\214\346\231\257\346\266\210\351\231\244\357\274\232", nullptr));
        background_frames_label->setText(QCoreApplication::translate("MainWindow", "\350\207\252\345\212\250\345\242\236\347\233\212\357\274\232", nullptr));
        auto_adjust_gain_combo_box->setItemText(0, QCoreApplication::translate("MainWindow", "\345\274\200\345\220\257", nullptr));
        auto_adjust_gain_combo_box->setItemText(1, QCoreApplication::translate("MainWindow", "\345\205\263\351\227\255", nullptr));

        auto_adjust_gain_combo_box->setCurrentText(QCoreApplication::translate("MainWindow", "\345\205\263\351\227\255", nullptr));
        eliminate_background_combo_box->setItemText(0, QCoreApplication::translate("MainWindow", "\345\274\200\345\220\257", nullptr));
        eliminate_background_combo_box->setItemText(1, QCoreApplication::translate("MainWindow", "\345\205\263\351\227\255", nullptr));

        eliminate_background_combo_box->setCurrentText(QCoreApplication::translate("MainWindow", "\345\205\263\351\227\255", nullptr));
        map_k_label->setText(QCoreApplication::translate("MainWindow", "\345\210\206\350\276\250\347\216\207\357\274\232", nullptr));
        map_k_value_label->setText(QCoreApplication::translate("MainWindow", "0.20", nullptr));
        max_depth_value_spin_box->setSuffix(QString());
        max_depth_label->setText(QCoreApplication::translate("MainWindow", "\345\205\250\345\261\200\345\242\236\347\233\212\357\274\232", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
