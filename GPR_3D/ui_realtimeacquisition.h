/********************************************************************************
** Form generated from reading UI file 'realtimeacquisition.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REALTIMEACQUISITION_H
#define UI_REALTIMEACQUISITION_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_realtimeacquisition
{
public:
    QVBoxLayout *verticalLayout;
    QPushButton *One_Click_Shutdown_button;
    QPushButton *One_Click_Configuration_button;
    QCheckBox *save_file_check_box;
    QPushButton *Start_Capture_Data_button;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *realtimeacquisition)
    {
        if (realtimeacquisition->objectName().isEmpty())
            realtimeacquisition->setObjectName("realtimeacquisition");
        realtimeacquisition->resize(367, 686);
        verticalLayout = new QVBoxLayout(realtimeacquisition);
        verticalLayout->setObjectName("verticalLayout");
        One_Click_Shutdown_button = new QPushButton(realtimeacquisition);
        One_Click_Shutdown_button->setObjectName("One_Click_Shutdown_button");

        verticalLayout->addWidget(One_Click_Shutdown_button);

        One_Click_Configuration_button = new QPushButton(realtimeacquisition);
        One_Click_Configuration_button->setObjectName("One_Click_Configuration_button");

        verticalLayout->addWidget(One_Click_Configuration_button);

        save_file_check_box = new QCheckBox(realtimeacquisition);
        save_file_check_box->setObjectName("save_file_check_box");

        verticalLayout->addWidget(save_file_check_box);

        Start_Capture_Data_button = new QPushButton(realtimeacquisition);
        Start_Capture_Data_button->setObjectName("Start_Capture_Data_button");

        verticalLayout->addWidget(Start_Capture_Data_button);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        retranslateUi(realtimeacquisition);

        QMetaObject::connectSlotsByName(realtimeacquisition);
    } // setupUi

    void retranslateUi(QWidget *realtimeacquisition)
    {
        realtimeacquisition->setWindowTitle(QCoreApplication::translate("realtimeacquisition", "Form", nullptr));
        One_Click_Shutdown_button->setText(QCoreApplication::translate("realtimeacquisition", "\344\270\200\351\224\256\345\205\263\346\234\272", nullptr));
        One_Click_Configuration_button->setText(QCoreApplication::translate("realtimeacquisition", "\344\270\200\351\224\256\351\205\215\347\275\256", nullptr));
        save_file_check_box->setText(QCoreApplication::translate("realtimeacquisition", "\344\277\235\345\255\230\346\226\207\344\273\266", nullptr));
        Start_Capture_Data_button->setText(QCoreApplication::translate("realtimeacquisition", "\345\274\200\345\247\213\351\207\207\351\233\206", nullptr));
    } // retranslateUi

};

namespace Ui {
    class realtimeacquisition: public Ui_realtimeacquisition {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REALTIMEACQUISITION_H
