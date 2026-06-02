#ifndef THREE_D_VOLUME_DIALOG_H
#define THREE_D_VOLUME_DIALOG_H

#include <QDialog>
#include "three_d_volume_view.h"

namespace Ui {
class Three_D_Volume_Dialog;
}

class Three_D_Volume_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Three_D_Volume_Dialog(QWidget *parent = nullptr);
    Three_D_Volume_view* m_three_d_volume_view;
    void init_layout();
    void create_and_set_volume_data(const QVector<QVector<QVector<double>>>& raw_data_all_channel);
    ~Three_D_Volume_Dialog();

private:
    Ui::Three_D_Volume_Dialog *ui;
};

#endif // THREE_D_VOLUME_DIALOG_H
