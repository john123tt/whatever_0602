#include "filereplayview.h"
#include <array>
#include <codecvt>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDialog>
#include <QPushButton>
#include <QBoxLayout>
#include <QAbstractButton>
#include <QSizePolicy>
#include <QSignalBlocker>
#include "ui_filereplayview.h"
#include "GPR_SDK.h"
#include "setting.h"
#include "utility.h"
#include <cmath>
#include <QScrollArea>
#include <QVBoxLayout>
using namespace GPR;

FileReplayView::FileReplayView(QWidget *parent)
    : QWidget(parent),
    m_sequence_counter(0),
      ui(new Ui::FileReplayView) {
  ui->setupUi(this);
  ui->replay_button->setCheckable(true);
  ui->data_replay_group_box->setStyleSheet(R"(
    QGroupBox#data_replay_group_box {
      background: transparent;
      border: 1px solid #cbd2d9;
      border-radius: 4px;
      margin-top: 1ex;
    }
    QGroupBox#data_replay_group_box::title {
      subcontrol-origin: margin;
      left: 8px;
      padding: 0 3px;
    }
  )");

  auto headerButton = new QPushButton(QString::fromUtf8("查看文件头"), this);
  headerButton->setObjectName("show_file_header_button");
  const auto buttons = findChildren<QAbstractButton*>();
  for (auto* button : buttons) {
    if (!button) {
      continue;
    }
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    button->setMinimumHeight(static_cast<int>(button->sizeHint().height() * 1.33));
  }
  if (auto layout = qobject_cast<QBoxLayout*>(ui->data_replay_group_box->layout())) {
    layout->removeWidget(ui->file_header_scroll_area);
    const int insertIndex = ui->line ? layout->indexOf(ui->line) : -1;
    layout->insertWidget(insertIndex >= 0 ? insertIndex : layout->count(), headerButton);
  }
  if (ui->line) {
    ui->line->hide();
  }

  m_header_dialog = new QDialog(this);
  m_header_dialog->setWindowTitle(QString::fromUtf8("文件头"));
  m_header_dialog->setModal(true);
  m_header_dialog->resize(560, 620);
  auto dialogLayout = new QVBoxLayout(m_header_dialog);
  dialogLayout->setContentsMargins(8, 8, 8, 8);
  ui->file_header_scroll_area->setParent(m_header_dialog);
  dialogLayout->addWidget(ui->file_header_scroll_area);
  connect(headerButton, &QPushButton::clicked, this, [this]() {
    if (!m_header_dialog) {
      return;
    }
    if (auto topLevel = window()) {
      const QPoint center = topLevel->geometry().center()
          - QPoint(m_header_dialog->width() / 2, m_header_dialog->height() / 2);
      m_header_dialog->move(center);
    }
    m_header_dialog->show();
    m_header_dialog->raise();
    m_header_dialog->activateWindow();
  });

  ui->channel_number_spinBox->setRange(1,56);
  ui->channel_number_spinBox->setValue(10);
  connect(ui->channel_number_spinBox, QOverload<int>::of(&QSpinBox::valueChanged),
          this, [this](int value) {
            if (m_detected_channel_count > 0) {
              emit display_channel_changed(value - 1);
            }
          });
  ui->replay_file_path->setText(m_settings.value(replay_file_key,
    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toString());
  on_replay_file_path_textChanged(ui->replay_file_path->text());
  connect(this, &FileReplayView::error_occur,
    this, &FileReplayView::on_error, Qt::QueuedConnection);
  connect(this, &FileReplayView::replay,
    this, &FileReplayView::on_replay, Qt::QueuedConnection);
  QString headerStyle = R"(
    QGroupBox#file_header_group_box {
        border: 1px solid #cbd2d9;
        border-radius: 4px;
        margin-top: 1ex;
    }
    QGroupBox#file_header_group_box::title {
        subcontrol-origin: margin;
        left: 10px;
        padding: 0 3px 0 3px;
        color: #1f2933;
    }
    QScrollArea#file_header_scroll_area {
        border: none;
    }
    QGroupBox#file_header_group_box QLabel,
    QScrollArea#file_header_scroll_area QLabel {
        color: #1f2933;
    }
    QGroupBox#file_header_group_box QLineEdit,
    QScrollArea#file_header_scroll_area QLineEdit,
    QGroupBox#file_header_group_box QSpinBox,
    QScrollArea#file_header_scroll_area QSpinBox {
        border: 1px solid #cccccc;
        color: #1f2933;
    }
)";

  ui->file_header_group_box->setStyleSheet(headerStyle);
  ui->file_header_scroll_area->setStyleSheet(headerStyle);
}

FileReplayView::~FileReplayView() {
  delete ui;
}

void FileReplayView::realtime_play_start() {
  if(ui->replay_button->isChecked()) {
    ui->replay_button->setChecked(false);
  }
}

bool FileReplayView::is_replaying() const {
  return ui->replay_button->isChecked();
}

void FileReplayView::on_error(int error_code) {
  if(ui->replay_button->isChecked()) {
    flush_replay_batch();
    ui->replay_button->setChecked(false);
    if(error_code != GPR_ERROR_READ_END_OF_FILE) {
      QMessageBox::warning(this, tr("GPR"),
        QString::fromUtf8("读取数据失败，错误码是：%1").arg(error_code), QMessageBox::Ok);
    }
  }
}

void FileReplayView::on_replay_file_browse_clicked() {
  auto file_name = QFileDialog::getOpenFileName(this,
    QString::fromUtf8("打开文件"), m_settings.value(replay_file_key).toString(),
    QString::fromUtf8("Data files (*.txt *.dat);;All files(*.*)"));
  if(!file_name.isEmpty()) {
    ui->replay_file_path->setText(file_name);
    m_settings.setValue(replay_file_key, file_name);
  }
}

void FileReplayView::on_replay_button_toggled(bool checked) {
  if(!checked) {
    if(m_using_sdk_time_domain) {
      GPR_StopReadingTimeDomainFile();
    } else {
      GPR_StopReadingFile();
    }
    ui->replay_button->setText(QString::fromUtf8("开始回放"));
    m_sequence_counter=0;
    ui->replay_file_browse->setEnabled(true);
    static const auto empty_data = QVector<QVector<GPR_Complex>>();
    emit data_ready(empty_data, true);
    emit replay_state_changed(false);
  } else {
    m_detected_channel_count = 0;
    m_settings.setValue(channel_number,ui->channel_number_spinBox->value());
    m_using_sdk_time_domain = m_settings.value(time_domain_transform_key, 0).toInt() == 1;
    if(m_using_sdk_time_domain) {
      auto time_range = GPR_TimeRange{m_settings.value(start_time_key).toInt(),
        m_settings.value(end_time_key).toInt()};
      auto error = GPR_StartReadingTimeDomainFile(
        ui->replay_file_path->text().toStdString().c_str(), GPR_TIME_SCALE,
        &time_range, m_settings.value(window_parameter_key, 6.0).toDouble(),
        [] (int error, GPR_CallbackDataType type, void* buffer, int size, void* user) {
          auto view = reinterpret_cast<FileReplayView*>(user);
          if(error != 0) {
            view->error_occur(error);
            return;
          }
          if(type == GPR_CALLBACK_FILE_HEADER) {
            view->reset_replay_state();
            auto file_header = *reinterpret_cast<GPR_FileHeader*>(buffer);
            view->m_header = file_header;
            view->replay(file_header, view->ui->channel_number_spinBox->value());
            view->m_settings.setValue(points_key, static_cast<int>(file_header.frequency_range_points.points));
          } else if(type == GPR_CALLBACK_FILE_DATA_HEADER) {
            view->m_data_header = *reinterpret_cast<GPR_FileDataHeader*>(buffer);
            view->m_frame++;
          } else if(type == GPR_CALLBACK_TIME_DOMAIN_DATA) {
            view->handle_replay_frame(convert(reinterpret_cast<GPR_ComplexArray*>(buffer), size));
          }
        }, this);
      if(error != GPR_SUCCESS) {
        ui->replay_button->blockSignals(true);
        ui->replay_button->setChecked(false);
        ui->replay_button->blockSignals(false);
        QMessageBox::warning(this, tr("GPR"),
          QString::fromUtf8("回放失败，错误码是：%1").arg(error), QMessageBox::Ok);
      } else {
        ui->replay_button->setText(QString::fromUtf8("停止回放"));
        ui->replay_file_browse->setEnabled(false);
        emit replay_state_changed(true);
      }
      return;
    }
    auto error = GPR_StartReadingFile(
      ui->replay_file_path->text().toStdString().c_str(), GPR_CALIBRATED_DATA_TYPE,      [] (int error, GPR_CallbackDataType type,
          void* buffer, int size, void* user) {
        auto view = reinterpret_cast<FileReplayView*>(user);
        if(error != 0) {
          view->error_occur(error);
          return;
        }
        if(type == GPR_CALLBACK_FILE_HEADER) {
            view->reset_replay_state();
            auto file_header = *reinterpret_cast<GPR_FileHeader*>(buffer);
            view->m_header = file_header;
            view->replay(file_header,view->ui->channel_number_spinBox->value());
            view->m_settings.setValue(points_key,static_cast<int>(file_header.frequency_range_points.points));        } else if(type == GPR_CALLBACK_FILE_DATA_HEADER) {
          view->m_data_header = *reinterpret_cast<GPR_FileDataHeader*>(buffer);
            view->m_frame++;
          //   qDebug()<<view->m_data_header.tx_rx_channel.tx_channel;
          // qDebug()<<view->m_frame;
          //   if((view->m_frame-1)%5!=view->m_data_header.tx_rx_channel.tx_channel)
          //   {
          //       qDebug()<<"error";
          //   }
        } else if(type == GPR_CALLBACK_CALIBRATED_DATA) /*{
          auto display_rx_index = view->ui->display_rx->currentIndex();
          if(view->m_data_header.tx_rx_channel.tx_channel == view->ui->display_tx->currentIndex() &&
              GPR_IsRxChannelMaskEnabled(view->m_data_header.tx_rx_channel.rx_channels,
                static_cast<GPR_RxChannel>(display_rx_index))) {
            const auto start_time = view->m_header.time_range.start_time;
            const auto end_time = view->m_header.time_range.end_time > start_time
                ? view->m_header.time_range.end_time
                : 20;
            auto data = matlab_default_czt_transform(
                reinterpret_cast<GPR_ComplexArray*>(buffer),
                size,
                view->m_header.frequency_range_points,
                start_time,
                end_time);
            auto index = GPR_GetRxChannelIndex(view->m_data_header.tx_rx_channel.rx_channels,
              static_cast<GPR_RxChannel>(display_rx_index));
            if(index >= 0 && index < data.size()) {
              emit view->data_ready(data[index]);
            }
          }
        }*/
        // {
        //     // auto display_rx_index = view->ui->display_rx->currentIndex();
        //     // QVector<QVector<GPR_Complex>> data;
        //     // if(data.size()<view->multichannel_number) {
        //     auto data=convert(reinterpret_cast<GPR_ComplexArray*>(buffer), size);
        //     // auto index = GPR_GetRxChannelIndex(view->m_data_header.tx_rx_channel.rx_channels,
        //     //                                    static_cast<GPR_RxChannel>(display_rx_index));
        //     // if(index >= 0 && index < data.size()) {
        //     static GPR_TxChannel first_tx_channel=view->m_data_header.tx_rx_channel.tx_channel;
        //     // view->m_all_channel_data.append(data);
        //     qDebug()<<view->m_data_header.tx_rx_channel.tx_channel;
        //     if (view->m_all_channel_data.size()!=0 && view->m_data_header.tx_rx_channel.tx_channel == first_tx_channel) {
        //         emit view->data_ready(view->m_all_channel_data, false);
        //         if(view->m_all_channel_data.size()!=10)
        //         {
        //             int a=0;
        //         }
        //         view->m_all_channel_data.clear();
        //     }
        //     view->m_all_channel_data.append(data);
        //     // }
        //     // }
        // }

        {
            const auto start_time = view->m_header.time_range.start_time;
            const auto end_time = view->m_header.time_range.end_time > start_time
                ? view->m_header.time_range.end_time
                : 20;
            auto data = matlab_default_czt_transform(
                reinterpret_cast<GPR_ComplexArray*>(buffer),
                size,
                view->m_header.frequency_range_points,
                start_time,
                end_time);
            GPR_TxChannel tx = view->m_data_header.tx_rx_channel.tx_channel;

            // ????????????
            if (!view->m_frame_active) {
                view->m_frame_active = true;
                view->m_frame_timer.restart();
                view->m_frame_buffer.clear();
            }

            // ????????????????
            if (view->m_frame_timer.elapsed() > view->m_frame_timeout_ms) {
                view->m_frame_buffer.clear();
                view->m_frame_timer.restart();
                // ??????????? m_tx_order????????
            }

            // ====== 1) ????? TX ?? ======
            if (view->m_calibrating) {
                // ?????? TX?????
                if (!view->m_seen_tx_once.contains(tx)) {
                    view->m_seen_tx_once.insert(tx);
                    view->m_tx_order.push_back(tx);
                } else {
                    // ????????? TX??? TX ??????????????
                    view->m_calibrating = false;
                    view->m_tx_set.clear();
                    for (auto learned_tx : view->m_tx_order) {
                        view->m_tx_set.insert(learned_tx);
                    }

                    // ???????????
                    // qDebug() << "Calibrated TX count =" << view->m_tx_order.size();
                }
                // 学习阶段也照常把数据放进 buffer
            }

            // ???? TX??? TX ???
            view->m_frame_buffer[tx] = data;

            // ====== 2) 收齐判断：必须先完成学习 ======
            if (!view->m_calibrating && !view->m_tx_order.isEmpty()) {

                // 判断 frame_buffer 是否包含全部 TX
                bool full = true;
                for (auto t : view->m_tx_order) {
                    if (!view->m_frame_buffer.contains(t)) {
                        full = false;
                        break;
                    }
                }

                if (full) {

                    int total_channels = 0;
                    for (auto t : view->m_tx_order) {
                        total_channels += view->m_frame_buffer[t].size();
                    }

                    QVector<QVector<GPR_Complex>> output;
                    output.reserve(total_channels);

                    // ?????? TX ????????????
                    for (auto t : view->m_tx_order) {
                        const auto& tx_data = view->m_frame_buffer[t]; // QVector<QVector<GPR_Complex>>
                        for (const auto& rx_vec : tx_data) {
                            output.append(rx_vec);
                        }
                    }
                    int detected_channels = total_channels;

                    // UI ?????????
                    QMetaObject::invokeMethod(view, [view, detected_channels](){
                            view->update_detected_channel_count(detected_channels);
                        }, Qt::QueuedConnection);

                    view->m_replay_batch_buffer.append(output);
                    constexpr int replay_batch_size = 64;
                    if (view->m_replay_batch_buffer.size() >= replay_batch_size) {
                        emit view->data_batch_ready(view->m_replay_batch_buffer);
                        view->m_replay_batch_buffer.clear();
                    }

                    // ????????
                    view->m_frame_buffer.clear();
                    view->m_frame_active = false;
                }
            }
        }
      }, this);
    if(error != GPR_SUCCESS) {
      ui->replay_button->blockSignals(true);
      ui->replay_button->setChecked(false);
      ui->replay_button->blockSignals(false);
      QMessageBox::warning(this, tr("GPR"),
        QString::fromUtf8("回放失败，错误码是：%1").arg(error), QMessageBox::Ok);
    } else {
      ui->replay_button->setText(QString::fromUtf8("停止回放"));
      ui->replay_file_browse->setEnabled(false);
      emit replay_state_changed(true);
    }
  }
}

void FileReplayView::on_replay_file_path_textChanged(const QString& path) {
  m_settings.setValue(replay_file_key, path);
}

void FileReplayView::on_launch_channel_combo_box_currentIndexChanged(int) {
  //replay(to_int(m_header.frequency_range_points.points));
}

void FileReplayView::on_receive_channel_combo_box_currentIndexChanged(int) {
  //replay(to_int(m_header.frequency_range_points.points));
}

void FileReplayView::on_replay(const GPR_FileHeader& file_header) {
  ui->replay_button->setEnabled(true);
  ui->frame_number_line_edit->setText(QString("%1").arg(file_header.frames));
  ui->start_frequencty_spin_box->setValue(file_header.frequency_range_points.start_frequency);
  ui->stop_frequencty_spin_box->setValue(file_header.frequency_range_points.stop_frequency);
  ui->scan_points_combo_box->setCurrentIndex(file_header.frequency_range_points.points);
  ui->IFBW_combo_box->setCurrentIndex(file_header.ifbw);
  ui->antenna_start_frequency_spin_box->setValue(file_header.antenna_frequency_range.start_frequency);
  ui->antenna_stop_frequency_spin_box->setValue(file_header.antenna_frequency_range.stop_frequency);
  ui->start_time_spin_box->setValue(file_header.time_range.start_time);
  ui->end_time_spin_box->setValue(file_header.time_range.end_time);
  ui->trigger_source_combo_box->setCurrentIndex(static_cast<int>(file_header.trigger_source));
  ui->trigger_mode_combo_box->setCurrentIndex(static_cast<int>(file_header.trigger_mode));
  ui->trigger_interval->setText(QString("%1").arg(file_header.trigger_interval));
  ui->trigger_frames->setText(QString("%1").arg(file_header.trigger_frame_number));
  ui->start_length->setText(QString("%1").arg(file_header.profile.start_length));
  ui->stop_length->setText(QString("%1").arg(file_header.profile.stop_length));
  ui->dielectric->setText(QString("%1").arg(file_header.profile.dielectric));
  ui->window_parameter->setValue(file_header.window_parameter);
  ui->max_modulus_value_spin_box->setValue(file_header.profile.max_modulus_value);
  ui->gain_parameter1_spin_box->setValue(file_header.profile.gains[0]);
  ui->gain_parameter2_spin_box->setValue(file_header.profile.gains[1]);
  try {
    auto company_name = utf8_to_wstring(file_header.user_info.company_name);
    ui->company_name_line_edit->setText(QString::fromStdWString(company_name));
    auto location = utf8_to_wstring(file_header.user_info.location);
    ui->testing_location_line_edit->setText(QString::fromStdWString(location));
  } catch(...) {}
  auto date_time = QDateTime::fromSecsSinceEpoch(file_header.create_timestamp);
  ui->create_time_line_edit->setText(date_time.toString("yyyy-MM-dd HH:mm:ss"));
  date_time = QDateTime::fromSecsSinceEpoch(file_header.modify_timestamp);
  ui->modify_time_line_edit->setText(date_time.toString("yyyy-MM-dd HH:mm:ss"));
}

void FileReplayView::reset_replay_state()
{
    m_detected_channel_count = 0;
    m_calibrating = true;
    m_seen_tx_once.clear();
    m_tx_order.clear();
    m_tx_set.clear();

    m_frame_buffer.clear();
    m_replay_batch_buffer.clear();
    m_frame_active = false;

    // ??????????
    // m_expected_tx_count = 0;
    // m_total_channel_count = 0;

    // timer ?????????????? restart
}

void FileReplayView::update_detected_channel_count(int total_channels)
{
    if (total_channels <= 0) {
        return;
    }

    const bool count_changed = (m_detected_channel_count != total_channels);
    m_detected_channel_count = total_channels;
    const int old_value = ui->channel_number_spinBox->value();
    const int new_value = count_changed
        ? total_channels
        : std::max(1, std::min(old_value, total_channels));

    {
        const QSignalBlocker blocker(ui->channel_number_spinBox);
        ui->channel_number_spinBox->setRange(1, total_channels);
        ui->channel_number_spinBox->setValue(new_value);
    }
    m_settings.setValue(channel_number, total_channels);

    if (count_changed || new_value != old_value) {
        emit display_channel_changed(new_value - 1);
    }
}

void FileReplayView::flush_replay_batch()
{
    if (m_replay_batch_buffer.isEmpty()) {
        return;
    }
    emit data_batch_ready(m_replay_batch_buffer);
    m_replay_batch_buffer.clear();
}

void FileReplayView::handle_replay_frame(QVector<QVector<GPR_Complex>> data)
{
    GPR_TxChannel tx = m_data_header.tx_rx_channel.tx_channel;

    if (!m_frame_active) {
        m_frame_active = true;
        m_frame_timer.restart();
        m_frame_buffer.clear();
    }

    if (m_frame_timer.elapsed() > m_frame_timeout_ms) {
        m_frame_buffer.clear();
        m_frame_timer.restart();
    }

    if (m_calibrating) {
        if (!m_seen_tx_once.contains(tx)) {
            m_seen_tx_once.insert(tx);
            m_tx_order.push_back(tx);
        } else {
            m_calibrating = false;
            m_tx_set.clear();
            for (auto learned_tx : m_tx_order) {
                m_tx_set.insert(learned_tx);
            }
        }
    }

    m_frame_buffer[tx] = std::move(data);

    if (m_calibrating || m_tx_order.isEmpty()) {
        return;
    }

    bool full = true;
    for (auto t : m_tx_order) {
        if (!m_frame_buffer.contains(t)) {
            full = false;
            break;
        }
    }
    if (!full) {
        return;
    }

    int total_channels = 0;
    for (auto t : m_tx_order) {
        total_channels += m_frame_buffer[t].size();
    }

    QVector<QVector<GPR_Complex>> output;
    output.reserve(total_channels);
    for (auto t : m_tx_order) {
        const auto& tx_data = m_frame_buffer[t];
        for (const auto& rx_vec : tx_data) {
            output.append(rx_vec);
        }
    }

    QMetaObject::invokeMethod(this, [this, total_channels](){
            update_detected_channel_count(total_channels);
        }, Qt::QueuedConnection);

    m_replay_batch_buffer.append(output);
    constexpr int replay_batch_size = 64;
    if (m_replay_batch_buffer.size() >= replay_batch_size) {
        emit data_batch_ready(m_replay_batch_buffer);
        m_replay_batch_buffer.clear();
    }

    m_frame_buffer.clear();
    m_frame_active = false;
}
