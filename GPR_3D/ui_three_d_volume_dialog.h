/********************************************************************************
** Form generated from reading UI file 'three_d_volume_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_THREE_D_VOLUME_DIALOG_H
#define UI_THREE_D_VOLUME_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>

QT_BEGIN_NAMESPACE

class Ui_Three_D_Volume_Dialog
{
public:

    void setupUi(QDialog *Three_D_Volume_Dialog)
    {
        if (Three_D_Volume_Dialog->objectName().isEmpty())
            Three_D_Volume_Dialog->setObjectName("Three_D_Volume_Dialog");
        Three_D_Volume_Dialog->resize(640, 480);

        retranslateUi(Three_D_Volume_Dialog);

        QMetaObject::connectSlotsByName(Three_D_Volume_Dialog);
    } // setupUi

    void retranslateUi(QDialog *Three_D_Volume_Dialog)
    {
        Three_D_Volume_Dialog->setWindowTitle(QCoreApplication::translate("Three_D_Volume_Dialog", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Three_D_Volume_Dialog: public Ui_Three_D_Volume_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_THREE_D_VOLUME_DIALOG_H
