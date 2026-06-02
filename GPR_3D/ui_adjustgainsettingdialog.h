/********************************************************************************
** Form generated from reading UI file 'adjustgainsettingdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADJUSTGAINSETTINGDIALOG_H
#define UI_ADJUSTGAINSETTINGDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_AdjustGainSettingDialog
{
public:
    QHBoxLayout *horizontalLayout;
    QCustomPlot *gain_plot;
    QFrame *frame;
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QPushButton *save_gain_button;
    QPushButton *load_gain_button;
    QPushButton *clear_gain_button;
    QPushButton *reset_gain_button;
    QPushButton *apply_button;
    QPushButton *ok_button;
    QPushButton *cancel_button;
    QLabel *label_gain_type;
    QDoubleSpinBox *gain_parameter;
    QComboBox *gain_type;
    QLabel *label_gain_point;
    QSpinBox *gain_point;
    QLabel *label_gain_parameter;

    void setupUi(QDialog *AdjustGainSettingDialog)
    {
        if (AdjustGainSettingDialog->objectName().isEmpty())
            AdjustGainSettingDialog->setObjectName("AdjustGainSettingDialog");
        AdjustGainSettingDialog->resize(580, 460);
        AdjustGainSettingDialog->setModal(true);
        horizontalLayout = new QHBoxLayout(AdjustGainSettingDialog);
        horizontalLayout->setObjectName("horizontalLayout");
        gain_plot = new QCustomPlot(AdjustGainSettingDialog);
        gain_plot->setObjectName("gain_plot");

        horizontalLayout->addWidget(gain_plot);

        frame = new QFrame(AdjustGainSettingDialog);
        frame->setObjectName("frame");
        frame->setMaximumSize(QSize(170, 400));
        gridLayout = new QGridLayout(frame);
        gridLayout->setObjectName("gridLayout");
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        save_gain_button = new QPushButton(frame);
        save_gain_button->setObjectName("save_gain_button");

        verticalLayout->addWidget(save_gain_button, 0, Qt::AlignHCenter);

        load_gain_button = new QPushButton(frame);
        load_gain_button->setObjectName("load_gain_button");

        verticalLayout->addWidget(load_gain_button, 0, Qt::AlignHCenter);

        clear_gain_button = new QPushButton(frame);
        clear_gain_button->setObjectName("clear_gain_button");

        verticalLayout->addWidget(clear_gain_button, 0, Qt::AlignHCenter);

        reset_gain_button = new QPushButton(frame);
        reset_gain_button->setObjectName("reset_gain_button");

        verticalLayout->addWidget(reset_gain_button, 0, Qt::AlignHCenter);

        apply_button = new QPushButton(frame);
        apply_button->setObjectName("apply_button");

        verticalLayout->addWidget(apply_button, 0, Qt::AlignHCenter);

        ok_button = new QPushButton(frame);
        ok_button->setObjectName("ok_button");

        verticalLayout->addWidget(ok_button, 0, Qt::AlignHCenter);

        cancel_button = new QPushButton(frame);
        cancel_button->setObjectName("cancel_button");

        verticalLayout->addWidget(cancel_button, 0, Qt::AlignHCenter);


        gridLayout->addLayout(verticalLayout, 3, 0, 1, 2);

        label_gain_type = new QLabel(frame);
        label_gain_type->setObjectName("label_gain_type");

        gridLayout->addWidget(label_gain_type, 0, 0, 1, 1);

        gain_parameter = new QDoubleSpinBox(frame);
        gain_parameter->setObjectName("gain_parameter");
        gain_parameter->setMaximum(100000.000000000000000);

        gridLayout->addWidget(gain_parameter, 2, 1, 1, 1);

        gain_type = new QComboBox(frame);
        gain_type->addItem(QString());
        gain_type->addItem(QString());
        gain_type->setObjectName("gain_type");

        gridLayout->addWidget(gain_type, 0, 1, 1, 1);

        label_gain_point = new QLabel(frame);
        label_gain_point->setObjectName("label_gain_point");

        gridLayout->addWidget(label_gain_point, 1, 0, 1, 1);

        gain_point = new QSpinBox(frame);
        gain_point->setObjectName("gain_point");
        gain_point->setMinimum(3);
        gain_point->setMaximum(10000);

        gridLayout->addWidget(gain_point, 1, 1, 1, 1);

        label_gain_parameter = new QLabel(frame);
        label_gain_parameter->setObjectName("label_gain_parameter");

        gridLayout->addWidget(label_gain_parameter, 2, 0, 1, 1);


        horizontalLayout->addWidget(frame);


        retranslateUi(AdjustGainSettingDialog);

        QMetaObject::connectSlotsByName(AdjustGainSettingDialog);
    } // setupUi

    void retranslateUi(QDialog *AdjustGainSettingDialog)
    {
        AdjustGainSettingDialog->setWindowTitle(QCoreApplication::translate("AdjustGainSettingDialog", "\350\260\203\350\212\202\345\242\236\347\233\212", nullptr));
        save_gain_button->setText(QCoreApplication::translate("AdjustGainSettingDialog", "\344\277\235\345\255\230\345\242\236\347\233\212", nullptr));
        load_gain_button->setText(QCoreApplication::translate("AdjustGainSettingDialog", "\345\212\240\350\275\275\345\242\236\347\233\212", nullptr));
        clear_gain_button->setText(QCoreApplication::translate("AdjustGainSettingDialog", "\346\270\205\351\231\244\345\242\236\347\233\212", nullptr));
        reset_gain_button->setText(QCoreApplication::translate("AdjustGainSettingDialog", "\351\207\215\347\275\256\345\242\236\347\233\212", nullptr));
        apply_button->setText(QCoreApplication::translate("AdjustGainSettingDialog", "\345\272\224\347\224\250", nullptr));
        ok_button->setText(QCoreApplication::translate("AdjustGainSettingDialog", "\347\241\256\345\256\232", nullptr));
        cancel_button->setText(QCoreApplication::translate("AdjustGainSettingDialog", "\345\217\226\346\266\210", nullptr));
        label_gain_type->setText(QCoreApplication::translate("AdjustGainSettingDialog", "\345\242\236\347\233\212\347\261\273\345\236\213", nullptr));
        gain_type->setItemText(0, QCoreApplication::translate("AdjustGainSettingDialog", "\346\211\213\345\212\250\345\242\236\347\233\212", nullptr));
        gain_type->setItemText(1, QCoreApplication::translate("AdjustGainSettingDialog", "\350\207\252\345\212\250\345\242\236\347\233\212", nullptr));

        label_gain_point->setText(QCoreApplication::translate("AdjustGainSettingDialog", "\345\242\236\347\233\212\347\202\271\346\225\260", nullptr));
        label_gain_parameter->setText(QCoreApplication::translate("AdjustGainSettingDialog", "\345\242\236\347\233\212\347\263\273\346\225\260", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AdjustGainSettingDialog: public Ui_AdjustGainSettingDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADJUSTGAINSETTINGDIALOG_H
