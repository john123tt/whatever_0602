#include "realtimeacquisition.h"
#include "ui_realtimeacquisition.h"
#include <QSettings>   // ??????????
#include <QApplication> // ??????
#include <QDebug>      // ????
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QRegularExpression>
#include <QAbstractButton>
#include <QSizePolicy>
#include <QGuiApplication>
#include <QProcess>
#include <QScreen>
#include "GPR_SDK.h"
#include "setting.h"
#include "utility.h"
using namespace GPR;

namespace {
bool parse_ifbw_value(const QVariant& value, GPR_IFBW& ifbw)
{
    const QString raw = value.toString().trimmed();
    bool ok = false;
    const int enum_value = raw.toInt(&ok);
    if (ok && enum_value >= GPR_IFBW_1HZ && enum_value <= GPR_IFBW_2MHZ) {
        ifbw = static_cast<GPR_IFBW>(enum_value);
        return true;
    }

    QString normalized = raw.toLower();
    normalized.remove(' ');
    normalized.replace(QString::fromUtf8("㎑"), "khz");
    normalized.replace(QString::fromUtf8("kHz"), "khz");
    normalized.replace("khz", "k");
    normalized.replace("hz", "");

    double numeric = 0.0;
    if (normalized.endsWith("mhz")) {
        normalized.chop(3);
        numeric = normalized.toDouble(&ok) * 1000000.0;
    } else if (normalized.endsWith("m")) {
        normalized.chop(1);
        numeric = normalized.toDouble(&ok) * 1000000.0;
    } else if (normalized.endsWith("k")) {
        normalized.chop(1);
        numeric = normalized.toDouble(&ok) * 1000.0;
    } else {
        numeric = normalized.toDouble(&ok);
    }
    if (!ok) {
        return false;
    }

    const int hz = static_cast<int>(std::round(numeric));
    static const QMap<int, GPR_IFBW> ifbw_map = {
        {1, GPR_IFBW_1HZ},
        {2, GPR_IFBW_2HZ},
        {5, GPR_IFBW_5HZ},
        {10, GPR_IFBW_10HZ},
        {20, GPR_IFBW_20HZ},
        {50, GPR_IFBW_50HZ},
        {100, GPR_IFBW_100HZ},
        {200, GPR_IFBW_200HZ},
        {500, GPR_IFBW_500HZ},
        {1000, GPR_IFBW_1KHZ},
        {3000, GPR_IFBW_3KHZ},
        {5000, GPR_IFBW_5KHZ},
        {10000, GPR_IFBW_10KHZ},
        {15000, GPR_IFBW_15KHZ},
        {20000, GPR_IFBW_20KHZ},
        {30000, GPR_IFBW_30KHZ},
        {50000, GPR_IFBW_50KHZ},
        {100000, GPR_IFBW_100KHZ},
        {200000, GPR_IFBW_200KHZ},
        {600000, GPR_IFBW_600KHZ},
        {1000000, GPR_IFBW_1MHZ},
        {2000000, GPR_IFBW_2MHZ}
    };
    if (!ifbw_map.contains(hz)) {
        return false;
    }
    ifbw = ifbw_map.value(hz);
    return true;
}

QString ifbw_to_string(GPR_IFBW ifbw)
{
    static const QStringList names = {
        "1Hz", "2Hz", "5Hz", "10Hz", "20Hz", "50Hz", "100Hz", "200Hz", "500Hz",
        "1KHz", "3KHz", "5KHz", "10KHz", "15KHz", "20KHz", "30KHz", "50KHz",
        "100KHz", "200KHz", "600KHz", "1MHz", "2MHz"
    };
    const int index = static_cast<int>(ifbw);
    return (index >= 0 && index < names.size()) ? names[index] : QString("Unknown(%1)").arg(index);
}

QString find_config_path()
{
    const QStringList candidates = {
        QDir(QDir::currentPath()).filePath("GPR_3D/config.ini"),
        QString(PROJECT_ROOT) + "/config.ini",
        QDir(QDir::currentPath()).filePath("config.ini"),
        QDir(QCoreApplication::applicationDirPath()).filePath("config.ini"),
        QDir(QCoreApplication::applicationDirPath()).filePath("../config.ini")
    };
    for (const auto& path : candidates) {
        const QString clean_path = QDir::cleanPath(path);
        if (QFileInfo::exists(clean_path)) {
            return clean_path;
        }
    }
    return QDir(QCoreApplication::applicationDirPath()).filePath("config.ini");
}
}

realtimeacquisition::realtimeacquisition(QWidget *parent)
    : QWidget(parent),
    m_track_number(0),
    ui(new Ui::realtimeacquisition)
{
    ui->setupUi(this);
    ui->Start_Capture_Data_button->setCheckable(true);
    const auto buttons = findChildren<QAbstractButton*>();
    for (auto* button : buttons) {
        if (!button) {
            continue;
        }
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        button->setMinimumHeight(static_cast<int>(button->sizeHint().height() * 1.33));
    }
    connect(this, &realtimeacquisition::error_occur, this, &realtimeacquisition::on_error,Qt::QueuedConnection);
    connect(this,&realtimeacquisition::raw_frame_arrived,this,&realtimeacquisition::on_raw_frame_arrived,Qt::QueuedConnection);


}

realtimeacquisition::~realtimeacquisition()
{
    delete ui;
}

void realtimeacquisition::on_One_Click_Shutdown_button_clicked()
{
    QMessageBox confirm(this);
    confirm.setWindowTitle(QString::fromUtf8("确认关机"));
    confirm.setText(QString::fromUtf8("确认要关闭电脑吗？"));
    confirm.setIcon(QMessageBox::Warning);
    auto* shutdownButton = confirm.addButton(QString::fromUtf8("确认关机"), QMessageBox::AcceptRole);
    auto* cancelButton = confirm.addButton(QString::fromUtf8("取消"), QMessageBox::RejectRole);
    confirm.setDefaultButton(qobject_cast<QPushButton*>(cancelButton));
    confirm.adjustSize();
    if (auto* screen = QGuiApplication::primaryScreen()) {
        const QRect screenGeometry = screen->availableGeometry();
        confirm.move(screenGeometry.center() - confirm.rect().center());
    }

    confirm.exec();
    if (confirm.clickedButton() == shutdownButton) {
        QProcess::startDetached("shutdown", QStringList{"/s", "/t", "0"});
    }
}

void realtimeacquisition::on_One_Click_Configuration_button_clicked()
{
    // 1. ??? QSettings????????????
    // ???????????? config.ini ??
    // QString configPath = QApplication::applicationDirPath() + "/config.ini";
    GPR_Calibration cal;
    cal.type = GPR_HOST_PORT_CALIBRATION;   // ? GPR_ANTENNA_PORT_CALIBRATION???????
    cal.mode = GPR_DUAL_ANTENNA_MODE;        // ????????
    GPR_ClearCalibration(cal);
    //GPR_ClearCalibration(GPR_DUAL_ANTENNA_MODE);//??
    QString configPath = find_config_path();
    QSettings settings(configPath, QSettings::IniFormat);

    // 2. ?????????????/????????
    // 读取字符串（数据库主机）
    QString host = settings.value("Database/host","192.168.0.11").toString();
    // ???????????
    int port = settings.value("Database/port",5001).toInt();
    GPR_SetDeviceAddress(host.toUtf8().constData(),port);
    // ???????????
    int trigger_source = settings.value("Database/trigger_source",0).toInt();


    int trigger_mode = settings.value("Database/trigger_mode",1).toInt();
    int trigger_interval = settings.value("Database/trigger_interval",0).toInt();
    int trigger_frames = settings.value("Database/trigger_frames",0).toInt();
    int start_frequency = settings.value("Database/start_frequency",1).toInt();
    int stop_frequency = settings.value("Database/stop_frequency",3000).toInt();
    int scan_points = settings.value("Database/scan_points",3).toInt();
    GPR_IFBW ifbw = GPR_IFBW_600KHZ;
    const QVariant ifbw_config = settings.value("Database/IFBW", "600KHz");
    if (!parse_ifbw_value(ifbw_config, ifbw)) {
        QMessageBox::warning(this, tr("GPR"),
                             QString::fromUtf8("config.ini 中 IFBW=%1 无法识别，已使用 600KHz")
                                 .arg(ifbw_config.toString()),
                             QMessageBox::Ok);
        ifbw = GPR_IFBW_600KHZ;
    }
    int window_parameter = settings.value("Database/window_parameter",6.0).toDouble();
    int start_time=settings.value("Database/start_time",0).toInt();
    int end_time=settings.value("Database/end_time",60).toInt();
    m_settings.setValue(points_key, scan_points);
    m_settings.setValue(window_parameter_key, window_parameter);
    m_settings.setValue(start_time_key, start_time);
    m_settings.setValue(end_time_key, end_time);
    QString defaultBinPath = QDir(QCoreApplication::applicationDirPath()).filePath("bin");
    QString defaultDataPath = QDir(QCoreApplication::applicationDirPath()).filePath("data");
    QString bin_file_path=settings.value("Database/bin_file_path",defaultBinPath).toString();
    QString data_file_path=settings.value("Database/data_file_path",defaultDataPath).toString();
    QDir().mkpath(bin_file_path);
    QDir().mkpath(data_file_path);
    m_settings.setValue(data_directory_key, data_file_path);
    GPR_EnableAutoAdjustTriggerInterval(false);
    // qDebug() << "数据库主机：" << host;
    // qDebug() << "数据库端口：" << port;
    // qDebug() << "起始频率" << start_frequency;
    if(auto error = GPR_Reset(); error != GPR_SUCCESS) {
        QMessageBox::warning(this, tr("GPR"), QString::fromUtf8("复位失败，错误码是：%1").arg(error), QMessageBox::Ok);
    }else{
        auto source = static_cast<GPR_TriggerSource>(trigger_source);
        if(auto error = GPR_SetTriggerSource(source);
            error != GPR_SUCCESS) {
            QMessageBox::warning(this, tr("GPR"), QString::fromUtf8("设置触发源失败，错误码是：%1").arg(error), QMessageBox::Ok);
        } else {
            if(auto error = GPR_SetTriggerMode(static_cast<GPR_TriggerMode>(trigger_mode));
                error != GPR_SUCCESS) {
                QMessageBox::warning(this, tr("GPR"), QString::fromUtf8("设置触发模式失败，错误码是：%1").arg(error), QMessageBox::Ok);
            } else {
                if(auto error = GPR_SetInternalTriggerInterval(trigger_interval);
                    error != GPR_SUCCESS) {
                    QMessageBox::warning(this, tr("GPR"), QString::fromUtf8("设置触发间隔失败，错误码是：%1").arg(error), QMessageBox::Ok);
                } else {
                    auto frequency_and_points=GPR_FrequencyRangeAndPoints{start_frequency,stop_frequency,static_cast<GPR_ScanPoints>(scan_points)};
                    if(auto error = GPR_SetFrequencyRangeAndPoints(&frequency_and_points);
                        error != GPR_SUCCESS) {
                        QMessageBox::warning(this, tr("GPR"), QString::fromUtf8("设置扫描频率失败，错误码是：%1").arg(error), QMessageBox::Ok);
                    } else {
                        if(auto error = GPR_SetIFBW(ifbw);
                            error != GPR_SUCCESS) {
                            QMessageBox::warning(this, tr("GPR"), QString::fromUtf8("设置中频带宽失败，错误码是：%1").arg(error), QMessageBox::Ok);
                        }else{
                            auto file_names = QFileDialog::getOpenFileNames(
                                this, QString::fromUtf8("打开文件"),
                                bin_file_path,
                                QString::fromUtf8("Files (*.bin)"));
                            auto loaded_files_count = 0;
                            for(auto& file_name : file_names) {
                                if(auto error = GPR_LoadCalibration(file_name.toStdString().c_str());
                                    error != GPR_SUCCESS) {
                                    QMessageBox::warning(this, tr("GPR"),
                                                         QString::fromUtf8("加载%1失败，错误码是：%2").arg(QFileInfo(file_name).fileName()).arg(error),
                                                         QMessageBox::Ok);

                                } else {
                                    ++loaded_files_count;
                                }
                            }
                            if(loaded_files_count != 0 && loaded_files_count == file_names.size()) {
                                QMessageBox::information(this, tr("GPR"), QString::fromUtf8("所有文件加载成功"), QMessageBox::Ok);
                                multichannel_number=loaded_files_count;
                            }
                            if(auto error = GPR_SetIFBW(ifbw);
                                error != GPR_SUCCESS) {
                                QMessageBox::warning(this, tr("GPR"),
                                                     QString::fromUtf8("标定文件加载后重新设置中频带宽失败，错误码是：%1").arg(error),
                                                     QMessageBox::Ok);
                            } else {
                                GPR_IFBW actual_ifbw = GPR_IFBW_600KHZ;
                                if (GPR_GetIFBW(&actual_ifbw) == GPR_SUCCESS && actual_ifbw != ifbw) {
                                    QMessageBox::warning(this, tr("GPR"),
                                                         QString::fromUtf8("中频带宽设置未生效：期望 %1，设备当前 %2")
                                                             .arg(ifbw_to_string(ifbw), ifbw_to_string(actual_ifbw)),
                                                         QMessageBox::Ok);
                                }
                            }
                            update_calibrated_channels();
                        }
                    }
                }
            }
        }
    }
}


void realtimeacquisition::on_Start_Capture_Data_button_toggled(bool checked)
{
    if(!checked) {
        if(ui->save_file_check_box->isChecked()) {
            GPR_StopSavingFile();
            ui->save_file_check_box->setChecked(false);
        }

        if(m_using_sdk_time_domain) {
            GPR_StopCapturingTimeDomainData();
        } else {
            GPR_StopCapturingData();
        }
        ui->Start_Capture_Data_button->setText(QString::fromUtf8("采集数据"));
        // enable_view(true);
        static const auto empty_data = QVector<QVector<GPR_Complex>>();
        m_all_channel_data.clear();
        emit data_ready(empty_data, true);
        emit capture_state_changed(false);
    } else {
        m_track_number=0;
        auto tx_rx_channels = std::vector<GPR_TxRxChannel>();
        // for(int i = 0; i < 1; ++i) {
        //     // auto tx_item = m_channel_model->item(i);
        //     // if(tx_item && tx_item->checkState() == Qt::Checked) {
        //         auto tx_rx_channel = GPR_TxRxChannel();
        //         tx_rx_channel.tx_channel = static_cast<GPR_TxChannel>(i);
        //         tx_rx_channel.rx_channels = 0;
        //         auto rx_channels = std::vector<GPR_RxChannel>();
        //         for(int j = 0; j < 1; ++j) {
        //             // auto rx_item = tx_item->child(j);
        //             // if(rx_item && rx_item->checkState() == Qt::Checked) {
        //                 tx_rx_channel.rx_channels =
        //                     GPR_EnableRxChannelMask(tx_rx_channel.rx_channels, static_cast<GPR_RxChannel>(j));
        //             // }
        //         // }
        //         tx_rx_channels.push_back(tx_rx_channel);
        //     }
        // }
        tx_rx_channels=m_tx_rx_channels;
        if(tx_rx_channels.empty()) {
            return;
        }
        int all_channel_count=0;
        for(auto tx_rx_channels:m_tx_rx_channels)
        {
            int channel_count = std::popcount(tx_rx_channels.rx_channels);
            all_channel_count+=channel_count;
        }
        m_settings.setValue(channel_number,all_channel_count);
        auto time_range = GPR_TimeRange{m_settings.value(start_time_key).toInt(),
                                        m_settings.value(end_time_key).toInt()};
        m_using_sdk_time_domain = m_settings.value(time_domain_transform_key, 0).toInt() == 1;
        if(m_using_sdk_time_domain) {
            auto error = GPR_StartCapturingTimeDomainData(GPR_TIME_SCALE, tx_rx_channels.data(),
                static_cast<int>(tx_rx_channels.size()), &time_range,
                m_settings.value(window_parameter_key, 6.0).toDouble(),
                [] (int error, GPR_CallbackDataType type, void* buffer, int size, void* user) {
                    auto view = reinterpret_cast<realtimeacquisition*>(user);
                    if(error != 0) {
                        view->error_occur(error);
                        return;
                    }
                    if(type == GPR_CALLBACK_DATA_HEADER) {
                        const auto& header = * reinterpret_cast<GPR_DataHeader*>(buffer);
                        view->m_data_header = header;
                        view->m_header_queue.enqueue(header);
                    } else if(type == GPR_CALLBACK_TIME_DOMAIN_DATA) {
                        if(view->m_header_queue.isEmpty()) {
                            qWarning() << "TIME_DOMAIN_DATA without header, drop";
                            return;
                        }
                        auto header = view->m_header_queue.dequeue();
                        auto data = convert(reinterpret_cast<GPR_ComplexArray*>(buffer), size);
                        emit view->raw_frame_arrived(header, std::move(data));
                    }
                }, this);
            if(error == GPR_SUCCESS) {
                ui->Start_Capture_Data_button->setText(QString::fromUtf8("停止采集"));
                enable_view(false);
                emit realtime_play(m_tx_rx_channels);
                emit capture_state_changed(true);
            } else {
                enable_view(true);
                ui->Start_Capture_Data_button->blockSignals(true);
                ui->Start_Capture_Data_button->setChecked(false);
                ui->Start_Capture_Data_button->blockSignals(false);
                QMessageBox::warning(this, tr("GPR"),
                                     QString::fromUtf8("采集数据失败，错误码是：%1").arg(error), QMessageBox::Ok);
            }
            return;
        }
        if(auto error = GPR_GetFrequencyRangeAndPoints(&m_frequency_range_points);
            error != GPR_SUCCESS) {
            enable_view(true);
            ui->Start_Capture_Data_button->blockSignals(true);
            ui->Start_Capture_Data_button->setChecked(false);
            ui->Start_Capture_Data_button->blockSignals(false);
            QMessageBox::warning(this, tr("GPR"),
                                 QString::fromUtf8("获取扫描参数失败，错误码是：%1").arg(error), QMessageBox::Ok);
            return;
        }        // qDebug()<<tx_rx_channels[0].rx_channels;
        auto error = GPR_StartCapturingData(GPR_CALIBRATED_DATA_TYPE, tx_rx_channels.data(),
            static_cast<int>(tx_rx_channels.size()),
            [] (int error, GPR_CallbackDataType type, void* buffer, int size, void* user) {
                auto view = reinterpret_cast<realtimeacquisition*>(user);
                if(error != 0) {
                    view->error_occur(error);
                    return;
                }
                if(type == GPR_CALLBACK_DATA_HEADER) {
                    view->m_data_header = *reinterpret_cast<GPR_DataHeader*>(buffer);
                    // qDebug()<<"???"<<view->m_data_header.tx_rx_channel.rx_channels;
                    // qDebug()<<"m_track_number"<<view->m_track_number+1;
                    // qDebug()<<"frame_id"<<view->m_data_header.frame_id;
                    // if((view->m_data_header.frame_id-1)%5!=view->m_data_header.tx_rx_channel.tx_channel)
                    // if((view->m_data_header.frame_id-1)!=view->m_track_number)
                    // {
                    //     qDebug()<<"error";
                    // }
                    // qDebug() << "header tx:" << view->m_data_header.tx_rx_channel.tx_channel
                    //          << "track:" << view->m_track_number;


                    const auto& header = * reinterpret_cast<GPR_DataHeader*>(buffer);
                    view->m_header_queue.enqueue(header);
                    // qDebug() << "enqueue header frame"
                    //          << header.frame_id
                    //          <<
                    //     "tx"
                    //          << header.tx_rx_channel.tx_channel;

                }else if (type == GPR_CALLBACK_CALIBRATED_DATA) {
                    if (view->m_header_queue.isEmpty()) {
                        qWarning() << "CALIBRATED_DATA without header, drop";
                        return;
                    }
                    auto header = view->m_header_queue.dequeue();
                    auto data = matlab_default_czt_transform(
                        reinterpret_cast<GPR_ComplexArray*>(buffer),
                        size,
                        view->m_frequency_range_points,
                        view->m_settings.value(start_time_key).toInt(),
                        view->m_settings.value(end_time_key).toInt());                    emit view->raw_frame_arrived(header, std::move(data));
                }
            }, this);
        if(error == GPR_SUCCESS) {
            ui->Start_Capture_Data_button->setText(QString::fromUtf8("停止采集"));
            enable_view(false);
            emit realtime_play(m_tx_rx_channels);
            emit capture_state_changed(true);
        } else {
            enable_view(true);
            ui->Start_Capture_Data_button->blockSignals(true);
            ui->Start_Capture_Data_button->setChecked(false);
            ui->Start_Capture_Data_button->blockSignals(false);
            QMessageBox::warning(this, tr("GPR"),
                                 QString::fromUtf8("采集数据失败，错误码是：%1").arg(error), QMessageBox::Ok);
        }
    }
}

void realtimeacquisition::on_error(int error_code) {
    if(ui->Start_Capture_Data_button->isChecked()) {
        ui->Start_Capture_Data_button->setChecked(false);
    }
    if(error_code == GPR_ERROR_READ_DATA_END) {
        QMessageBox::information(this, tr("GPR"), QString::fromUtf8("读取数据结束"), QMessageBox::Ok);
    } else {
        QMessageBox::warning(this, tr("GPR"), QString::fromUtf8("读取数据失败，错误码是：%1").arg(error_code), QMessageBox::Ok);
    }
}

void realtimeacquisition::update_calibrated_channels() {
    // for(int i = 0; i < m_channel_model->rowCount(); ++i) {
    //     if(auto tx_item = m_channel_model->item(i)) {
    //         tx_item->setEnabled(false);
    //         for(auto j = 0; j < tx_item->rowCount(); ++j) {
    //             if(auto rx_item = tx_item->child(j)) {
    //                 rx_item->setEnabled(false);
    //             }
    //         }
    //     }
    // }
    //auto mode = GPR_DUAL_ANTENNA_MODE;
    GPR_Calibration model;
    model.type = GPR_HOST_PORT_CALIBRATION;   // ? GPR_ANTENNA_PORT_CALIBRATION???????
    model.mode = GPR_DUAL_ANTENNA_MODE;

    m_tx_rx_channels.clear();
    auto tx_channels = GPR_TxChannels();
    GPR_GetCalibratedTxChannels(model, &tx_channels);
    for(auto i = 0; i < tx_channels.count; ++i) {
        // qDebug()<<tx_channels.channels[i];
        auto tx_rx_channel = GPR_TxRxChannel();
        tx_rx_channel.tx_channel = static_cast<GPR_TxChannel>(tx_channels.channels[i]);
        tx_rx_channel.rx_channels = 0;
            auto rx_channels = GPR_RxChannels();
            GPR_GetCalibratedRxChannels(model, tx_channels.channels[i], &rx_channels);
            for(auto j = 0; j < rx_channels.count; ++j) {
                // auto rx_channels = std::vector<GPR_RxChannel>();

                    // auto rx_item = tx_item->child(j);
                    // if(rx_item && rx_item->checkState() == Qt::Checked) {
                    tx_rx_channel.rx_channels =
                        GPR_EnableRxChannelMask(tx_rx_channel.rx_channels, static_cast<GPR_RxChannel>(rx_channels.channels[j]));


            }
        m_tx_rx_channels.push_back(tx_rx_channel);
    }
}

void realtimeacquisition::enable_view(bool is_enabled) {

}



void realtimeacquisition::on_save_file_check_box_clicked(bool checked)
{
    if(checked) {
        auto file_name = m_settings.value(data_directory_key).toString() + "/" +
                         QDateTime::currentDateTime().toString("yyyyMMddhhmmss") +"_"+QString::number(multichannel_number)+"number.dat";
        auto user_info = GPR_UserInfo();
        // auto company_name = m_settings.value(company_name_key).toString().toStdWString();
        // auto utf8_str = wstring_to_utf8(company_name);
        // std::copy(utf8_str.begin(), utf8_str.end(), std::begin(user_info.company_name));
        // auto location = "441";
        // utf8_str = wstring_to_utf8(location);
        // std::copy(utf8_str.begin(), utf8_str.end(), std::begin(user_info.location));
        auto afr =
            GPR_AntennaFrequencyRange{0,
                                      3000};
        auto parameters = GPR_RadarSignalProfile();
        parameters.start_length = 0;
        parameters.stop_length = 0;
        parameters.dielectric = 1.0;
        parameters.max_modulus_value = 511.4;
        parameters.gains[0] = 6.0;
        parameters.gains[1] = 0.01;
        if(auto error = GPR_StartSavingFile(
                file_name.toStdString().c_str(), &user_info, &afr, &parameters);
            error != GPR_SUCCESS) {
            ui->save_file_check_box->setChecked(false);
            QMessageBox::warning(this, tr("GPR"),
                                 QString::fromUtf8("保存文件失败，错误码是：%1").arg(error), QMessageBox::Ok);
        }
    } else {
        GPR_StopSavingFile();
    }
}

void realtimeacquisition::on_raw_frame_arrived(
    const GPR_DataHeader& header,
    QVector<QVector<GPR_Complex>> data)
{
    const int tx = header.tx_rx_channel.tx_channel;

    // 1️⃣ 判断 tx 是否在配置里
    auto is_valid_tx = [&]() {
        for (const auto& ch : m_tx_rx_channels) {
            if (ch.tx_channel == tx)
                return true;
        }
        return false;
    };

    if (!is_valid_tx())
        return;

    // 2. ?? tx -> ?????
    if (m_curr_frame.contains(tx)) {
        m_curr_frame.clear();
    }

    // 3. ???? tx ??
    m_curr_frame.insert(tx, std::move(data));

    // 4. ????
    if (m_curr_frame.size() != m_tx_rx_channels.size())
        return;

    // 5. ?????? m_tx_rx_channels ??
    int merged_size = 0;
    for (const auto& ch : m_tx_rx_channels) {
        auto it = m_curr_frame.find(ch.tx_channel);
        if (it != m_curr_frame.end()) {
            merged_size += it.value().size();
        }
    }

    QVector<QVector<GPR_Complex>> merged;
    merged.reserve(merged_size);
    for (const auto& ch : m_tx_rx_channels) {
        auto frame = m_curr_frame.take(ch.tx_channel);
        for (auto& trace : frame) {
            merged.push_back(std::move(trace));
        }
    }

    emit data_ready(merged, false);

    m_curr_frame.clear();
}







