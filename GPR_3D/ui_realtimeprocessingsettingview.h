/********************************************************************************
** Form generated from reading UI file 'realtimeprocessingsettingview.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REALTIMEPROCESSINGSETTINGVIEW_H
#define UI_REALTIMEPROCESSINGSETTINGVIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RealTimeProcessingSettingView
{
public:
    QGridLayout *gridLayout;
    QLabel *label_trigger_source_4;
    QComboBox *eliminate_background;
    QLabel *label;
    QComboBox *comboBox;
    QPushButton *adjust_gain_button;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *RealTimeProcessingSettingView)
    {
        if (RealTimeProcessingSettingView->objectName().isEmpty())
            RealTimeProcessingSettingView->setObjectName("RealTimeProcessingSettingView");
        RealTimeProcessingSettingView->resize(279, 521);
        gridLayout = new QGridLayout(RealTimeProcessingSettingView);
        gridLayout->setObjectName("gridLayout");
        label_trigger_source_4 = new QLabel(RealTimeProcessingSettingView);
        label_trigger_source_4->setObjectName("label_trigger_source_4");

        gridLayout->addWidget(label_trigger_source_4, 0, 0, 1, 1);

        eliminate_background = new QComboBox(RealTimeProcessingSettingView);
        eliminate_background->addItem(QString());
        eliminate_background->addItem(QString());
        eliminate_background->setObjectName("eliminate_background");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(eliminate_background->sizePolicy().hasHeightForWidth());
        eliminate_background->setSizePolicy(sizePolicy);

        gridLayout->addWidget(eliminate_background, 0, 1, 1, 2);

        label = new QLabel(RealTimeProcessingSettingView);
        label->setObjectName("label");

        gridLayout->addWidget(label, 1, 0, 1, 1);

        comboBox = new QComboBox(RealTimeProcessingSettingView);
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->setObjectName("comboBox");
        comboBox->setStyleSheet(QString::fromUtf8("QComboBox {\n"
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

        gridLayout->addWidget(comboBox, 1, 1, 1, 2);

        adjust_gain_button = new QPushButton(RealTimeProcessingSettingView);
        adjust_gain_button->setObjectName("adjust_gain_button");

        gridLayout->addWidget(adjust_gain_button, 2, 1, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        gridLayout->addItem(verticalSpacer, 3, 1, 1, 1);


        retranslateUi(RealTimeProcessingSettingView);

        eliminate_background->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(RealTimeProcessingSettingView);
    } // setupUi

    void retranslateUi(QWidget *RealTimeProcessingSettingView)
    {
        RealTimeProcessingSettingView->setWindowTitle(QCoreApplication::translate("RealTimeProcessingSettingView", "Form", nullptr));
        label_trigger_source_4->setText(QCoreApplication::translate("RealTimeProcessingSettingView", "\350\203\214\346\231\257\346\266\210\351\231\244\357\274\232", nullptr));
        eliminate_background->setItemText(0, QCoreApplication::translate("RealTimeProcessingSettingView", "\345\274\200\345\220\257", nullptr));
        eliminate_background->setItemText(1, QCoreApplication::translate("RealTimeProcessingSettingView", "\345\205\263\351\227\255", nullptr));

        label->setText(QCoreApplication::translate("RealTimeProcessingSettingView", "\344\274\252\345\275\251\350\211\262\345\233\276", nullptr));
        comboBox->setItemText(0, QCoreApplication::translate("RealTimeProcessingSettingView", "\347\201\260\345\272\246\345\233\276", nullptr));
        comboBox->setItemText(1, QCoreApplication::translate("RealTimeProcessingSettingView", "\351\273\221\347\273\277\347\272\242", nullptr));
        comboBox->setItemText(2, QCoreApplication::translate("RealTimeProcessingSettingView", "\350\223\235\347\231\275\347\272\242", nullptr));
        comboBox->setItemText(3, QCoreApplication::translate("RealTimeProcessingSettingView", "\351\273\221\350\223\235\347\272\242", nullptr));

        adjust_gain_button->setText(QCoreApplication::translate("RealTimeProcessingSettingView", "\350\260\203\350\212\202\345\242\236\347\233\212", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RealTimeProcessingSettingView: public Ui_RealTimeProcessingSettingView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REALTIMEPROCESSINGSETTINGVIEW_H
