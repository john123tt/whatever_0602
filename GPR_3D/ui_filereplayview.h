/********************************************************************************
** Form generated from reading UI file 'filereplayview.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FILEREPLAYVIEW_H
#define UI_FILEREPLAYVIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FileReplayView
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *data_replay_group_box;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QLineEdit *replay_file_path;
    QPushButton *replay_file_browse;
    QHBoxLayout *horizontalLayout_2;
    QLabel *channel_number_label;
    QSpinBox *channel_number_spinBox;
    QPushButton *replay_button;
    QFrame *line;
    QScrollArea *file_header_scroll_area;
    QWidget *file_header_scroll_area_widget;
    QVBoxLayout *file_header_scroll_layout;
    QGroupBox *file_header_group_box;
    QGridLayout *gridLayout;
    QDoubleSpinBox *gain_parameter2_spin_box;
    QLineEdit *modify_time_line_edit;
    QLabel *scan_points_label;
    QLabel *modify_time_label;
    QLabel *trigger_frames_label;
    QLabel *frame_number_label;
    QDoubleSpinBox *max_modulus_value_spin_box;
    QLabel *trigger_mode_label;
    QLabel *IFBW_label;
    QSpinBox *antenna_stop_frequency_spin_box;
    QSpinBox *start_frequencty_spin_box;
    QLabel *trigger_interval_label;
    QLineEdit *stop_length;
    QLabel *company_name_label;
    QLineEdit *testing_location_line_edit;
    QLabel *max_modulus_value_label;
    QLineEdit *frame_number_line_edit;
    QLabel *antenna_start_frequency_label;
    QLabel *stop_frequency_label;
    QComboBox *trigger_source_combo_box;
    QLabel *window_parameter_label;
    QLabel *stop_length_label;
    QLabel *trigger_source_label;
    QLabel *start_length_label;
    QComboBox *scan_points_combo_box;
    QSpinBox *end_time_spin_box;
    QLabel *start_time_label;
    QSpinBox *stop_frequencty_spin_box;
    QLabel *create_time_label;
    QDoubleSpinBox *window_parameter;
    QLineEdit *company_name_line_edit;
    QLineEdit *trigger_interval;
    QLabel *dielectric_label;
    QLineEdit *trigger_frames;
    QLabel *start_frequencty_label;
    QLineEdit *create_time_line_edit;
    QLineEdit *dielectric;
    QLabel *antenna_stop_frequency_label;
    QComboBox *IFBW_combo_box;
    QSpinBox *start_time_spin_box;
    QLineEdit *start_length;
    QDoubleSpinBox *gain_parameter1_spin_box;
    QLabel *end_time_label;
    QLabel *testing_location_label;
    QLabel *gain_parameter2_label;
    QComboBox *trigger_mode_combo_box;
    QLabel *gain_parameter1_label;
    QSpinBox *antenna_start_frequency_spin_box;

    void setupUi(QWidget *FileReplayView)
    {
        if (FileReplayView->objectName().isEmpty())
            FileReplayView->setObjectName("FileReplayView");
        FileReplayView->resize(321, 979);
        FileReplayView->setStyleSheet(QString::fromUtf8(""));
        verticalLayout = new QVBoxLayout(FileReplayView);
        verticalLayout->setSpacing(10);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(2, 10, 2, 2);
        data_replay_group_box = new QGroupBox(FileReplayView);
        data_replay_group_box->setObjectName("data_replay_group_box");
        data_replay_group_box->setFlat(true);
        verticalLayout_2 = new QVBoxLayout(data_replay_group_box);
        verticalLayout_2->setObjectName("verticalLayout_2");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        replay_file_path = new QLineEdit(data_replay_group_box);
        replay_file_path->setObjectName("replay_file_path");

        horizontalLayout->addWidget(replay_file_path);

        replay_file_browse = new QPushButton(data_replay_group_box);
        replay_file_browse->setObjectName("replay_file_browse");
        replay_file_browse->setMaximumSize(QSize(30, 16777215));

        horizontalLayout->addWidget(replay_file_browse);


        verticalLayout_2->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        channel_number_label = new QLabel(data_replay_group_box);
        channel_number_label->setObjectName("channel_number_label");

        horizontalLayout_2->addWidget(channel_number_label);

        channel_number_spinBox = new QSpinBox(data_replay_group_box);
        channel_number_spinBox->setObjectName("channel_number_spinBox");

        horizontalLayout_2->addWidget(channel_number_spinBox);


        verticalLayout_2->addLayout(horizontalLayout_2);

        replay_button = new QPushButton(data_replay_group_box);
        replay_button->setObjectName("replay_button");

        verticalLayout_2->addWidget(replay_button);

        line = new QFrame(data_replay_group_box);
        line->setObjectName("line");
        line->setFrameShape(QFrame::Shape::HLine);
        line->setFrameShadow(QFrame::Shadow::Sunken);

        verticalLayout_2->addWidget(line);

        file_header_scroll_area = new QScrollArea(data_replay_group_box);
        file_header_scroll_area->setObjectName("file_header_scroll_area");
        file_header_scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        file_header_scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        file_header_scroll_area->setWidgetResizable(true);
        file_header_scroll_area_widget = new QWidget();
        file_header_scroll_area_widget->setObjectName("file_header_scroll_area_widget");
        file_header_scroll_area_widget->setGeometry(QRect(0, 0, 291, 822));
        file_header_scroll_layout = new QVBoxLayout(file_header_scroll_area_widget);
        file_header_scroll_layout->setObjectName("file_header_scroll_layout");
        file_header_scroll_layout->setContentsMargins(0, 0, 0, 0);
        file_header_group_box = new QGroupBox(file_header_scroll_area_widget);
        file_header_group_box->setObjectName("file_header_group_box");
        gridLayout = new QGridLayout(file_header_group_box);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(-1, 30, -1, -1);
        gain_parameter2_spin_box = new QDoubleSpinBox(file_header_group_box);
        gain_parameter2_spin_box->setObjectName("gain_parameter2_spin_box");
        gain_parameter2_spin_box->setEnabled(false);
        gain_parameter2_spin_box->setReadOnly(true);
        gain_parameter2_spin_box->setDecimals(4);
        gain_parameter2_spin_box->setMinimum(0.000000000000000);
        gain_parameter2_spin_box->setMaximum(100000000000.000000000000000);
        gain_parameter2_spin_box->setSingleStep(0.010000000000000);

        gridLayout->addWidget(gain_parameter2_spin_box, 21, 2, 1, 1);

        modify_time_line_edit = new QLineEdit(file_header_group_box);
        modify_time_line_edit->setObjectName("modify_time_line_edit");
        modify_time_line_edit->setEnabled(false);

        gridLayout->addWidget(modify_time_line_edit, 25, 2, 1, 1);

        scan_points_label = new QLabel(file_header_group_box);
        scan_points_label->setObjectName("scan_points_label");

        gridLayout->addWidget(scan_points_label, 4, 0, 1, 1);

        modify_time_label = new QLabel(file_header_group_box);
        modify_time_label->setObjectName("modify_time_label");

        gridLayout->addWidget(modify_time_label, 25, 0, 1, 1);

        trigger_frames_label = new QLabel(file_header_group_box);
        trigger_frames_label->setObjectName("trigger_frames_label");

        gridLayout->addWidget(trigger_frames_label, 14, 0, 1, 1);

        frame_number_label = new QLabel(file_header_group_box);
        frame_number_label->setObjectName("frame_number_label");

        gridLayout->addWidget(frame_number_label, 0, 0, 1, 1);

        max_modulus_value_spin_box = new QDoubleSpinBox(file_header_group_box);
        max_modulus_value_spin_box->setObjectName("max_modulus_value_spin_box");
        max_modulus_value_spin_box->setEnabled(false);
        max_modulus_value_spin_box->setReadOnly(true);
        max_modulus_value_spin_box->setDecimals(4);
        max_modulus_value_spin_box->setMinimum(0.000000000000000);
        max_modulus_value_spin_box->setMaximum(10000000000.000000000000000);
        max_modulus_value_spin_box->setSingleStep(0.010000000000000);

        gridLayout->addWidget(max_modulus_value_spin_box, 19, 2, 1, 1);

        trigger_mode_label = new QLabel(file_header_group_box);
        trigger_mode_label->setObjectName("trigger_mode_label");

        gridLayout->addWidget(trigger_mode_label, 12, 0, 1, 1);

        IFBW_label = new QLabel(file_header_group_box);
        IFBW_label->setObjectName("IFBW_label");

        gridLayout->addWidget(IFBW_label, 5, 0, 1, 1);

        antenna_stop_frequency_spin_box = new QSpinBox(file_header_group_box);
        antenna_stop_frequency_spin_box->setObjectName("antenna_stop_frequency_spin_box");
        antenna_stop_frequency_spin_box->setEnabled(false);
        antenna_stop_frequency_spin_box->setMinimum(0);
        antenna_stop_frequency_spin_box->setMaximum(100000000);

        gridLayout->addWidget(antenna_stop_frequency_spin_box, 8, 2, 1, 1);

        start_frequencty_spin_box = new QSpinBox(file_header_group_box);
        start_frequencty_spin_box->setObjectName("start_frequencty_spin_box");
        start_frequencty_spin_box->setEnabled(false);
        start_frequencty_spin_box->setReadOnly(true);
        start_frequencty_spin_box->setMinimum(0);
        start_frequencty_spin_box->setMaximum(100000000);

        gridLayout->addWidget(start_frequencty_spin_box, 2, 2, 1, 1);

        trigger_interval_label = new QLabel(file_header_group_box);
        trigger_interval_label->setObjectName("trigger_interval_label");

        gridLayout->addWidget(trigger_interval_label, 13, 0, 1, 1);

        stop_length = new QLineEdit(file_header_group_box);
        stop_length->setObjectName("stop_length");
        stop_length->setEnabled(false);

        gridLayout->addWidget(stop_length, 16, 2, 1, 1);

        company_name_label = new QLabel(file_header_group_box);
        company_name_label->setObjectName("company_name_label");

        gridLayout->addWidget(company_name_label, 22, 0, 1, 1);

        testing_location_line_edit = new QLineEdit(file_header_group_box);
        testing_location_line_edit->setObjectName("testing_location_line_edit");
        testing_location_line_edit->setEnabled(false);
        testing_location_line_edit->setReadOnly(true);

        gridLayout->addWidget(testing_location_line_edit, 23, 2, 1, 1);

        max_modulus_value_label = new QLabel(file_header_group_box);
        max_modulus_value_label->setObjectName("max_modulus_value_label");

        gridLayout->addWidget(max_modulus_value_label, 19, 0, 1, 1);

        frame_number_line_edit = new QLineEdit(file_header_group_box);
        frame_number_line_edit->setObjectName("frame_number_line_edit");
        frame_number_line_edit->setEnabled(false);
        frame_number_line_edit->setReadOnly(true);

        gridLayout->addWidget(frame_number_line_edit, 0, 2, 1, 1);

        antenna_start_frequency_label = new QLabel(file_header_group_box);
        antenna_start_frequency_label->setObjectName("antenna_start_frequency_label");

        gridLayout->addWidget(antenna_start_frequency_label, 6, 0, 2, 2);

        stop_frequency_label = new QLabel(file_header_group_box);
        stop_frequency_label->setObjectName("stop_frequency_label");

        gridLayout->addWidget(stop_frequency_label, 3, 0, 1, 1);

        trigger_source_combo_box = new QComboBox(file_header_group_box);
        trigger_source_combo_box->addItem(QString());
        trigger_source_combo_box->addItem(QString());
        trigger_source_combo_box->setObjectName("trigger_source_combo_box");
        trigger_source_combo_box->setEnabled(false);

        gridLayout->addWidget(trigger_source_combo_box, 11, 2, 1, 1);

        window_parameter_label = new QLabel(file_header_group_box);
        window_parameter_label->setObjectName("window_parameter_label");

        gridLayout->addWidget(window_parameter_label, 18, 0, 1, 1);

        stop_length_label = new QLabel(file_header_group_box);
        stop_length_label->setObjectName("stop_length_label");

        gridLayout->addWidget(stop_length_label, 16, 0, 1, 1);

        trigger_source_label = new QLabel(file_header_group_box);
        trigger_source_label->setObjectName("trigger_source_label");

        gridLayout->addWidget(trigger_source_label, 11, 0, 1, 1);

        start_length_label = new QLabel(file_header_group_box);
        start_length_label->setObjectName("start_length_label");

        gridLayout->addWidget(start_length_label, 15, 0, 1, 1);

        scan_points_combo_box = new QComboBox(file_header_group_box);
        scan_points_combo_box->addItem(QString());
        scan_points_combo_box->addItem(QString());
        scan_points_combo_box->addItem(QString());
        scan_points_combo_box->addItem(QString());
        scan_points_combo_box->setObjectName("scan_points_combo_box");
        scan_points_combo_box->setEnabled(false);
        scan_points_combo_box->setEditable(true);

        gridLayout->addWidget(scan_points_combo_box, 4, 2, 1, 1);

        end_time_spin_box = new QSpinBox(file_header_group_box);
        end_time_spin_box->setObjectName("end_time_spin_box");
        end_time_spin_box->setEnabled(false);
        end_time_spin_box->setReadOnly(true);
        end_time_spin_box->setMinimum(-1000000000);
        end_time_spin_box->setMaximum(1000000000);

        gridLayout->addWidget(end_time_spin_box, 10, 2, 1, 1);

        start_time_label = new QLabel(file_header_group_box);
        start_time_label->setObjectName("start_time_label");

        gridLayout->addWidget(start_time_label, 9, 0, 1, 1);

        stop_frequencty_spin_box = new QSpinBox(file_header_group_box);
        stop_frequencty_spin_box->setObjectName("stop_frequencty_spin_box");
        stop_frequencty_spin_box->setEnabled(false);
        stop_frequencty_spin_box->setReadOnly(true);
        stop_frequencty_spin_box->setMinimum(0);
        stop_frequencty_spin_box->setMaximum(100000000);

        gridLayout->addWidget(stop_frequencty_spin_box, 3, 2, 1, 1);

        create_time_label = new QLabel(file_header_group_box);
        create_time_label->setObjectName("create_time_label");

        gridLayout->addWidget(create_time_label, 24, 0, 1, 1);

        window_parameter = new QDoubleSpinBox(file_header_group_box);
        window_parameter->setObjectName("window_parameter");
        window_parameter->setEnabled(false);
        window_parameter->setMaximum(1000000.000000000000000);

        gridLayout->addWidget(window_parameter, 18, 2, 1, 1);

        company_name_line_edit = new QLineEdit(file_header_group_box);
        company_name_line_edit->setObjectName("company_name_line_edit");
        company_name_line_edit->setEnabled(false);
        company_name_line_edit->setReadOnly(true);

        gridLayout->addWidget(company_name_line_edit, 22, 2, 1, 1);

        trigger_interval = new QLineEdit(file_header_group_box);
        trigger_interval->setObjectName("trigger_interval");
        trigger_interval->setEnabled(false);

        gridLayout->addWidget(trigger_interval, 13, 2, 1, 1);

        dielectric_label = new QLabel(file_header_group_box);
        dielectric_label->setObjectName("dielectric_label");

        gridLayout->addWidget(dielectric_label, 17, 0, 1, 1);

        trigger_frames = new QLineEdit(file_header_group_box);
        trigger_frames->setObjectName("trigger_frames");
        trigger_frames->setEnabled(false);

        gridLayout->addWidget(trigger_frames, 14, 2, 1, 1);

        start_frequencty_label = new QLabel(file_header_group_box);
        start_frequencty_label->setObjectName("start_frequencty_label");

        gridLayout->addWidget(start_frequencty_label, 2, 0, 1, 1);

        create_time_line_edit = new QLineEdit(file_header_group_box);
        create_time_line_edit->setObjectName("create_time_line_edit");
        create_time_line_edit->setEnabled(false);
        create_time_line_edit->setReadOnly(true);

        gridLayout->addWidget(create_time_line_edit, 24, 2, 1, 1);

        dielectric = new QLineEdit(file_header_group_box);
        dielectric->setObjectName("dielectric");
        dielectric->setEnabled(false);

        gridLayout->addWidget(dielectric, 17, 2, 1, 1);

        antenna_stop_frequency_label = new QLabel(file_header_group_box);
        antenna_stop_frequency_label->setObjectName("antenna_stop_frequency_label");

        gridLayout->addWidget(antenna_stop_frequency_label, 8, 0, 1, 1);

        IFBW_combo_box = new QComboBox(file_header_group_box);
        IFBW_combo_box->addItem(QString());
        IFBW_combo_box->addItem(QString());
        IFBW_combo_box->addItem(QString());
        IFBW_combo_box->addItem(QString());
        IFBW_combo_box->addItem(QString());
        IFBW_combo_box->addItem(QString());
        IFBW_combo_box->addItem(QString());
        IFBW_combo_box->addItem(QString());
        IFBW_combo_box->addItem(QString());
        IFBW_combo_box->addItem(QString());
        IFBW_combo_box->addItem(QString());
        IFBW_combo_box->addItem(QString());
        IFBW_combo_box->addItem(QString());
        IFBW_combo_box->addItem(QString());
        IFBW_combo_box->addItem(QString());
        IFBW_combo_box->addItem(QString());
        IFBW_combo_box->addItem(QString());
        IFBW_combo_box->addItem(QString());
        IFBW_combo_box->addItem(QString());
        IFBW_combo_box->addItem(QString());
        IFBW_combo_box->addItem(QString());
        IFBW_combo_box->addItem(QString());
        IFBW_combo_box->setObjectName("IFBW_combo_box");
        IFBW_combo_box->setEnabled(false);
        IFBW_combo_box->setEditable(true);

        gridLayout->addWidget(IFBW_combo_box, 5, 2, 1, 1);

        start_time_spin_box = new QSpinBox(file_header_group_box);
        start_time_spin_box->setObjectName("start_time_spin_box");
        start_time_spin_box->setEnabled(false);
        start_time_spin_box->setReadOnly(true);
        start_time_spin_box->setMinimum(-1000000000);
        start_time_spin_box->setMaximum(1000000000);

        gridLayout->addWidget(start_time_spin_box, 9, 2, 1, 1);

        start_length = new QLineEdit(file_header_group_box);
        start_length->setObjectName("start_length");
        start_length->setEnabled(false);

        gridLayout->addWidget(start_length, 15, 2, 1, 1);

        gain_parameter1_spin_box = new QDoubleSpinBox(file_header_group_box);
        gain_parameter1_spin_box->setObjectName("gain_parameter1_spin_box");
        gain_parameter1_spin_box->setEnabled(false);
        gain_parameter1_spin_box->setReadOnly(true);
        gain_parameter1_spin_box->setDecimals(4);
        gain_parameter1_spin_box->setMinimum(0.000000000000000);
        gain_parameter1_spin_box->setMaximum(100000000000.000000000000000);
        gain_parameter1_spin_box->setSingleStep(0.010000000000000);

        gridLayout->addWidget(gain_parameter1_spin_box, 20, 2, 1, 1);

        end_time_label = new QLabel(file_header_group_box);
        end_time_label->setObjectName("end_time_label");

        gridLayout->addWidget(end_time_label, 10, 0, 1, 1);

        testing_location_label = new QLabel(file_header_group_box);
        testing_location_label->setObjectName("testing_location_label");

        gridLayout->addWidget(testing_location_label, 23, 0, 1, 1);

        gain_parameter2_label = new QLabel(file_header_group_box);
        gain_parameter2_label->setObjectName("gain_parameter2_label");

        gridLayout->addWidget(gain_parameter2_label, 21, 0, 1, 1);

        trigger_mode_combo_box = new QComboBox(file_header_group_box);
        trigger_mode_combo_box->addItem(QString());
        trigger_mode_combo_box->addItem(QString());
        trigger_mode_combo_box->setObjectName("trigger_mode_combo_box");
        trigger_mode_combo_box->setEnabled(false);

        gridLayout->addWidget(trigger_mode_combo_box, 12, 2, 1, 1);

        gain_parameter1_label = new QLabel(file_header_group_box);
        gain_parameter1_label->setObjectName("gain_parameter1_label");

        gridLayout->addWidget(gain_parameter1_label, 20, 0, 1, 1);

        antenna_start_frequency_spin_box = new QSpinBox(file_header_group_box);
        antenna_start_frequency_spin_box->setObjectName("antenna_start_frequency_spin_box");
        antenna_start_frequency_spin_box->setEnabled(false);
        antenna_start_frequency_spin_box->setMinimum(0);
        antenna_start_frequency_spin_box->setMaximum(100000000);

        gridLayout->addWidget(antenna_start_frequency_spin_box, 7, 2, 1, 1);


        file_header_scroll_layout->addWidget(file_header_group_box);

        file_header_scroll_area->setWidget(file_header_scroll_area_widget);

        verticalLayout_2->addWidget(file_header_scroll_area);


        verticalLayout->addWidget(data_replay_group_box);


        retranslateUi(FileReplayView);

        QMetaObject::connectSlotsByName(FileReplayView);
    } // setupUi

    void retranslateUi(QWidget *FileReplayView)
    {
        FileReplayView->setWindowTitle(QCoreApplication::translate("FileReplayView", "Form", nullptr));
        data_replay_group_box->setTitle(QCoreApplication::translate("FileReplayView", "\345\233\236\346\224\276", nullptr));
        replay_file_browse->setText(QCoreApplication::translate("FileReplayView", "...", nullptr));
        channel_number_label->setText(QCoreApplication::translate("FileReplayView", "\351\200\232\351\201\223\346\225\260", nullptr));
        replay_button->setText(QCoreApplication::translate("FileReplayView", "\345\274\200\345\247\213\345\233\236\346\224\276", nullptr));
        file_header_group_box->setTitle(QCoreApplication::translate("FileReplayView", "\346\226\207\344\273\266\345\244\264", nullptr));
        scan_points_label->setText(QCoreApplication::translate("FileReplayView", "\346\211\253\346\217\217\347\202\271\346\225\260:", nullptr));
        modify_time_label->setText(QCoreApplication::translate("FileReplayView", "\344\277\256\346\224\271\346\227\266\351\227\264\357\274\232", nullptr));
        trigger_frames_label->setText(QCoreApplication::translate("FileReplayView", "\345\215\225\346\254\241\350\247\246\345\217\221\345\270\247\346\225\260\357\274\232", nullptr));
        frame_number_label->setText(QCoreApplication::translate("FileReplayView", "\351\201\223\346\225\260\357\274\232", nullptr));
        trigger_mode_label->setText(QCoreApplication::translate("FileReplayView", "\350\247\246\345\217\221\346\250\241\345\274\217\357\274\232", nullptr));
        IFBW_label->setText(QCoreApplication::translate("FileReplayView", "\344\270\255\351\242\221\345\270\246\345\256\275", nullptr));
        antenna_stop_frequency_spin_box->setSuffix(QCoreApplication::translate("FileReplayView", "MHz", nullptr));
        start_frequencty_spin_box->setSuffix(QCoreApplication::translate("FileReplayView", "MHz", nullptr));
        trigger_interval_label->setText(QCoreApplication::translate("FileReplayView", "\350\247\246\345\217\221\351\227\264\351\232\224\357\274\232", nullptr));
        company_name_label->setText(QCoreApplication::translate("FileReplayView", "\345\205\254\345\217\270\345\220\215\347\247\260\357\274\232", nullptr));
        max_modulus_value_label->setText(QCoreApplication::translate("FileReplayView", "\346\234\200\345\244\247\345\200\274\357\274\232", nullptr));
        antenna_start_frequency_label->setText(QCoreApplication::translate("FileReplayView", "\345\244\251\347\272\277\350\265\267\345\247\213\351\242\221\347\216\207\357\274\232", nullptr));
        stop_frequency_label->setText(QCoreApplication::translate("FileReplayView", "\347\273\210\346\255\242\351\242\221\347\216\207\357\274\232", nullptr));
        trigger_source_combo_box->setItemText(0, QCoreApplication::translate("FileReplayView", "\345\206\205\351\203\250\350\247\246\345\217\221", nullptr));
        trigger_source_combo_box->setItemText(1, QCoreApplication::translate("FileReplayView", "\345\244\226\351\203\250\350\247\246\345\217\221", nullptr));

        window_parameter_label->setText(QCoreApplication::translate("FileReplayView", "\347\252\227\345\217\243\345\217\202\346\225\260\357\274\232", nullptr));
        stop_length_label->setText(QCoreApplication::translate("FileReplayView", "\347\273\210\346\255\242\351\207\214\347\250\213\347\202\271\357\274\232", nullptr));
        trigger_source_label->setText(QCoreApplication::translate("FileReplayView", "\350\247\246\345\217\221\346\272\220\357\274\232", nullptr));
        start_length_label->setText(QCoreApplication::translate("FileReplayView", "\350\265\267\345\247\213\351\207\214\347\250\213\347\202\271\357\274\232", nullptr));
        scan_points_combo_box->setItemText(0, QCoreApplication::translate("FileReplayView", "128", nullptr));
        scan_points_combo_box->setItemText(1, QCoreApplication::translate("FileReplayView", "256", nullptr));
        scan_points_combo_box->setItemText(2, QCoreApplication::translate("FileReplayView", "512", nullptr));
        scan_points_combo_box->setItemText(3, QCoreApplication::translate("FileReplayView", "1024", nullptr));

        end_time_spin_box->setSuffix(QCoreApplication::translate("FileReplayView", "ns", nullptr));
        start_time_label->setText(QCoreApplication::translate("FileReplayView", "\350\265\267\345\247\213\346\227\266\351\227\264\357\274\232", nullptr));
        stop_frequencty_spin_box->setSuffix(QCoreApplication::translate("FileReplayView", "MHz", nullptr));
        create_time_label->setText(QCoreApplication::translate("FileReplayView", "\345\210\233\345\273\272\346\227\266\351\227\264\357\274\232", nullptr));
        dielectric_label->setText(QCoreApplication::translate("FileReplayView", "\344\273\213\347\224\265\345\270\270\346\225\260\357\274\232", nullptr));
        start_frequencty_label->setText(QCoreApplication::translate("FileReplayView", "\350\265\267\345\247\213\351\242\221\347\216\207\357\274\232", nullptr));
        antenna_stop_frequency_label->setText(QCoreApplication::translate("FileReplayView", "\345\244\251\347\272\277\347\273\210\346\255\242\351\242\221\347\216\207\357\274\232", nullptr));
        IFBW_combo_box->setItemText(0, QCoreApplication::translate("FileReplayView", "1Hz", nullptr));
        IFBW_combo_box->setItemText(1, QCoreApplication::translate("FileReplayView", "2Hz", nullptr));
        IFBW_combo_box->setItemText(2, QCoreApplication::translate("FileReplayView", "5Hz", nullptr));
        IFBW_combo_box->setItemText(3, QCoreApplication::translate("FileReplayView", "10Hz", nullptr));
        IFBW_combo_box->setItemText(4, QCoreApplication::translate("FileReplayView", "20Hz", nullptr));
        IFBW_combo_box->setItemText(5, QCoreApplication::translate("FileReplayView", "50Hz", nullptr));
        IFBW_combo_box->setItemText(6, QCoreApplication::translate("FileReplayView", "100Hz", nullptr));
        IFBW_combo_box->setItemText(7, QCoreApplication::translate("FileReplayView", "200Hz", nullptr));
        IFBW_combo_box->setItemText(8, QCoreApplication::translate("FileReplayView", "500Hz", nullptr));
        IFBW_combo_box->setItemText(9, QCoreApplication::translate("FileReplayView", "1KHz", nullptr));
        IFBW_combo_box->setItemText(10, QCoreApplication::translate("FileReplayView", "2KHz", nullptr));
        IFBW_combo_box->setItemText(11, QCoreApplication::translate("FileReplayView", "5KHz", nullptr));
        IFBW_combo_box->setItemText(12, QCoreApplication::translate("FileReplayView", "10KHz", nullptr));
        IFBW_combo_box->setItemText(13, QCoreApplication::translate("FileReplayView", "15KHz", nullptr));
        IFBW_combo_box->setItemText(14, QCoreApplication::translate("FileReplayView", "20KHz", nullptr));
        IFBW_combo_box->setItemText(15, QCoreApplication::translate("FileReplayView", "30KHz", nullptr));
        IFBW_combo_box->setItemText(16, QCoreApplication::translate("FileReplayView", "50KHz", nullptr));
        IFBW_combo_box->setItemText(17, QCoreApplication::translate("FileReplayView", "100KHz", nullptr));
        IFBW_combo_box->setItemText(18, QCoreApplication::translate("FileReplayView", "200KHz", nullptr));
        IFBW_combo_box->setItemText(19, QCoreApplication::translate("FileReplayView", "600KHz", nullptr));
        IFBW_combo_box->setItemText(20, QCoreApplication::translate("FileReplayView", "1MHz", nullptr));
        IFBW_combo_box->setItemText(21, QCoreApplication::translate("FileReplayView", "2MHz", nullptr));

        start_time_spin_box->setSuffix(QCoreApplication::translate("FileReplayView", "ns", nullptr));
        end_time_label->setText(QCoreApplication::translate("FileReplayView", "\347\273\210\346\255\242\346\227\266\351\227\264\357\274\232", nullptr));
        testing_location_label->setText(QCoreApplication::translate("FileReplayView", "\346\265\213\350\257\225\345\234\260\347\202\271\357\274\232", nullptr));
        gain_parameter2_label->setText(QCoreApplication::translate("FileReplayView", "\345\242\236\347\233\212\345\217\202\346\225\2602\357\274\232", nullptr));
        trigger_mode_combo_box->setItemText(0, QCoreApplication::translate("FileReplayView", "\345\215\225\346\254\241\350\247\246\345\217\221", nullptr));
        trigger_mode_combo_box->setItemText(1, QCoreApplication::translate("FileReplayView", "\350\277\236\347\273\255\350\247\246\345\217\221", nullptr));

        gain_parameter1_label->setText(QCoreApplication::translate("FileReplayView", "\345\242\236\347\233\212\345\217\202\346\225\2601\357\274\232", nullptr));
        antenna_start_frequency_spin_box->setSuffix(QCoreApplication::translate("FileReplayView", "MHz", nullptr));
    } // retranslateUi

};

namespace Ui {
    class FileReplayView: public Ui_FileReplayView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FILEREPLAYVIEW_H
