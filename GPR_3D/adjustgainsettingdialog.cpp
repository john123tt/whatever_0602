#include "adjustgainsettingdialog.h"
#include <iostream>
#include <fstream>
#include <ranges>
#include <sstream>
#include "ui_adjustgainsettingdialog.h"
#include "manualadjustgainaxisticker.h"
#include "utility.h"

using namespace GPR;

namespace {
  const auto GAIN_CIRCLE_SIZE = 9;
  const auto GAIN_CIRCLE_HALF_SIZE = 9 / 2;

  void export_data_as_csv(const QVector<double>& data, std::ostream& out) {
    for(auto i = 0; i < data.size(); ++i) {
      if(i != 0) {
        out << std::endl;
      }
      out << data[i];
    }
  }

  QVector<double> import_data_as_csv(std::istream& in) {
    auto data = QVector<double>();
    auto line = std::string();
    while(std::getline(in, line)) {
      auto sstream = std::stringstream(line);
      auto value = 1.0;
      sstream >> value;
      data.append(value);
    }
    return data;
  }

  QVector<double> rsm_gain(const QVector<double>& data) {
    auto gain_rms = QVector<double>(data.size());
    auto half_window_length = 32;
    auto sum = 0.0;
    auto update_rms = [&] (double rms, int index) {
      if(rms <= 0.0) {
        gain_rms[index] = 0;
      } else {
        gain_rms[index] = data[index] / std::sqrt(rms);
      }
    };
    for(int i = 0; i < half_window_length; ++i) {
      sum += data[i] * data[i];
    }
    auto nwin = 2 * half_window_length + 1;
    auto rms = sum / nwin;
    for(int i = 0; i < half_window_length; ++i) {
      sum += data[i + half_window_length] * data[i + half_window_length];
      ++nwin;
      rms = sum / nwin;
      update_rms(rms, i);
    }
    for(int i = half_window_length; i < data.size() - half_window_length; ++i) {
      sum += data[i + half_window_length] * data[i + half_window_length];
      sum -= data[i - half_window_length] * data[i - half_window_length];
      rms = sum / nwin;
      update_rms(rms, i);
    }
    for(int i = data.size() - half_window_length; i < data.size(); ++i) {
      sum -= data[i - half_window_length] * data[i - half_window_length];
      nwin--;
      rms = sum / nwin;
      update_rms(rms, i);
    }
    auto gains = QVector<double>(data.size());
    for(auto i = 0; i < data.size(); ++i) {
      gains[i] = gain_rms[i] * 0.4 / data[i];
    }
    return gains;
  }
}

AdjustGainSettingDialog::AdjustGainSettingDialog(QVector<double> data,
    QVector<double> gains, double gain_parameter, 
    double start_time, double end_time, QWidget* parent)
    : QDialog(parent),
      ui(new Ui::AdjustGainSettingDialog),
      m_data(std::move(data)),
      m_original_gains(std::move(gains)),
      m_gain_parameter(gain_parameter),
      m_start_time(start_time),
      m_end_time(end_time),
      m_gains(m_original_gains),
      m_base_gains(m_original_gains),
      m_point_index(-1),
      m_has_submitted(false) {
  ui->setupUi(this);
  m_manual_gains.resize(m_gains.size(), 1.0);
  ui->gain_plot->xAxis->setVisible(false);
  ui->gain_plot->xAxis2->setVisible(true);
  ui->gain_plot->xAxis2->setLabel(tr("强度"));
  ui->gain_plot->xAxis2->setTickLength(0, 3);
  ui->gain_plot->xAxis2->setSubTickLength(0, 1);
  ui->gain_plot->xAxis2->grid()->setVisible(true);
  auto ticker = QSharedPointer<ManualAdjustGainAxisTicker>(new ManualAdjustGainAxisTicker());
  ui->gain_plot->xAxis2->setTicker(ticker);
  ui->gain_plot->xAxis2->setRange(-2, 4);
  ui->gain_plot->yAxis->setLabel("时间标尺(ns)");
  ui->gain_plot->yAxis->setRange(m_start_time, m_end_time);
  ui->gain_plot->yAxis->setRangeReversed(true);
  ui->gain_plot->yAxis->setTickLength(0, 3);
  ui->gain_plot->yAxis->setSubTickLength(0, 1);
  ui->gain_plot->legend->setVisible(false);
  ui->gain_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
  ui->gain_plot->axisRect()->setRangeDragAxes(nullptr, nullptr);
  ui->gain_plot->axisRect()->setRangeZoomAxes(ui->gain_plot->xAxis2, ui->gain_plot->yAxis);
  auto data_graph = ui->gain_plot->addGraph(ui->gain_plot->yAxis, ui->gain_plot->xAxis2);
  data_graph->setPen(QPen(Qt::red, 1));
  auto gain_graph = ui->gain_plot->addGraph(ui->gain_plot->yAxis, ui->gain_plot->xAxis2);
  gain_graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,
    QPen(Qt::black, 1), QBrush(Qt::white), GAIN_CIRCLE_SIZE));
  ui->gain_point->setValue(m_settings.value(gain_point_key, 8).toInt());
  connect(ui->gain_plot, &QCustomPlot::mousePress, this, &AdjustGainSettingDialog::on_mouse_press);
  connect(ui->gain_plot, &QCustomPlot::mouseRelease, this, &AdjustGainSettingDialog::on_mouse_release);
  connect(ui->gain_plot, &QCustomPlot::mouseMove, this, &AdjustGainSettingDialog::on_mouse_move);
  plot_data();
}

AdjustGainSettingDialog::~AdjustGainSettingDialog() {
  delete ui;
}

void AdjustGainSettingDialog::on_gain_type_currentIndexChanged(int index) {
  if(index == static_cast<int>(GAIN_TYPE::AUTO_GAIN)) {
    auto data = QVector<double>(m_data.size());
    std::transform(m_data.begin(), m_data.end(), data.begin(),
      [this] (auto value) {
        return value * m_gain_parameter;
      });
    m_gains = rsm_gain(data);
    m_base_gains = m_gains;
    plot_data();
    reset_gain_points();
    ui->gain_plot->replot();
  }
  m_has_submitted = false;
}

void AdjustGainSettingDialog::on_gain_point_valueChanged(int value) {
  m_settings.setValue(gain_point_key, value);
  reset_gains();
  std::fill(m_manual_gains.begin(), m_manual_gains.end(), 1.0);
  auto gain_time_values = QVector<double>(ui->gain_point->value());
  auto gain_strength_values = QVector<double>(ui->gain_point->value(), 1.0);
  auto time_range = m_end_time - m_start_time;
  for(int i = 0; i < ui->gain_point->value(); ++i) {
    gain_time_values[i] = m_start_time + i * time_range / (ui->gain_point->value() - 1);
    gain_strength_values[i] = 1.0;
  }
  ui->gain_plot->graph(1)->setData(gain_time_values, gain_strength_values, true);
  ui->gain_plot->replot();
  m_has_submitted = false;
}

void AdjustGainSettingDialog::on_save_gain_button_clicked() {
  auto file_name = QFileDialog::getSaveFileName(
    this, tr("保存增益文件"),
    m_settings.value(gain_file_key, QApplication::applicationDirPath()).toString(),
    tr("Files (*.csv)"));
  if(!file_name.isEmpty()) {
    auto out = std::ofstream(file_name.toStdString());
    export_data_as_csv(m_gains, out);
    m_settings.setValue(gain_file_key, QFileInfo(file_name).absolutePath());
  }
}

void AdjustGainSettingDialog::on_load_gain_button_clicked() {
  auto file_name = QFileDialog::getOpenFileName(
    this, tr("打开增益文件"),
    m_settings.value(gain_file_key, QApplication::applicationDirPath()).toString(),
    tr("Files (*.csv)"));
  if(!file_name.isEmpty()) {
    m_settings.setValue(gain_file_key, QFileInfo(file_name).absolutePath());
    auto in = std::ifstream(file_name.toStdString());
    m_gains = import_data_as_csv(in);
    m_base_gains = m_gains;
    reset_gain_points();
    plot_data();
    m_has_submitted = false;
  }
}


void AdjustGainSettingDialog::on_reset_gain_button_clicked() {
  reset_gains();
  reset_gain_points();
  m_has_submitted = false;
}

void AdjustGainSettingDialog::on_clear_gain_button_clicked() {
  ui->gain_type->blockSignals(true);
  ui->gain_type->setCurrentIndex(static_cast<int>(GAIN_TYPE::MANUAL_GAIN));
  ui->gain_type->blockSignals(false);
  clear_gains();
  reset_gain_points();
  m_has_submitted = false;
}

void AdjustGainSettingDialog::on_apply_button_clicked() {
  emit submit_gains(m_gains);
  m_has_submitted = true;
}

void AdjustGainSettingDialog::on_ok_button_clicked() {
  if(!m_has_submitted) {
    on_apply_button_clicked();
  }
  close();
}

void AdjustGainSettingDialog::on_cancel_button_clicked() {
  close();
}

int AdjustGainSettingDialog::hit_gain_point(QMouseEvent* event) const {
  auto data = ui->gain_plot->graph(1)->data();
  for(auto iter = data->begin(); iter != data->end(); ++iter) {
    if(abs(event->pos().x() - ui->gain_plot->xAxis2->coordToPixel(iter->value)) <= GAIN_CIRCLE_HALF_SIZE &&
        abs(event->pos().y() - ui->gain_plot->yAxis->coordToPixel(iter->key)) <= GAIN_CIRCLE_HALF_SIZE) {
      auto index = std::distance(data->begin(), iter);
      if(index == 0 || index == data->size() - 1) {
        break;
      }
      return index;
    }
  }
  return -1;
}

void AdjustGainSettingDialog::plot_data() {
  auto time_range = m_end_time - m_start_time;
  auto time_step = time_range / (m_data.size() - 1);
  auto time_values = QVector<double>(m_data.size());
  auto strength_values = QVector<double>(m_data.size());
  for(auto i = 0; i < m_data.size(); ++i) {
    time_values[i] = m_start_time + i * time_step;
    strength_values[i] = m_data[i] * m_gains[i] * m_gain_parameter + 1.0;
  }
  ui->gain_plot->graph(0)->setData(time_values, strength_values, true);
  ui->gain_plot->replot();
}

void AdjustGainSettingDialog::reset_gain_points() {
  std::fill(m_manual_gains.begin(), m_manual_gains.end(), 1.0);
  auto data = ui->gain_plot->graph(1)->data();
  for(auto iter = data->begin(); iter != data->end(); ++iter) {
    iter->value = 1.0;
  }
  ui->gain_plot->replot();
}

void AdjustGainSettingDialog::reset_gains() {
  if(ui->gain_type->currentIndex() == static_cast<int>(GAIN_TYPE::AUTO_GAIN)) {
    m_gains = m_base_gains;
  } else {
    m_gains = m_original_gains;
    m_base_gains = m_gains;
  }
  plot_data();
}

void AdjustGainSettingDialog::clear_gains() {
  std::fill(m_original_gains.begin(), m_original_gains.end(), 1.0);
  m_gains = m_original_gains;
  m_base_gains = m_gains;
  plot_data();
}

void AdjustGainSettingDialog::update_manual_gains(int gain_point_index) {
  if(gain_point_index <= 0 || gain_point_index >= ui->gain_point->value() - 1) {
    return;
  }
  auto data = ui->gain_plot->graph(1)->data();
  auto resolution =
    static_cast<int>((m_manual_gains.size() - 2) / (ui->gain_point->value() - 1.0));
  for(auto i : std::views::iota(gain_point_index, gain_point_index + 2)) {
    auto value = custom_mapping(data->at(i)->value);
    auto value_step = (value - custom_mapping(data->at(i - 1)->value)) / resolution;
    auto end_point = std::min(i * resolution, static_cast<int>(m_manual_gains.size()) - 1);
    for(auto j = (i - 1) * resolution + 1; j <= end_point; ++j) {
      m_manual_gains[j] = value_step + m_manual_gains[j - 1];
    }
  }
}

void AdjustGainSettingDialog::on_mouse_press(QMouseEvent* event) {
  m_point_index = hit_gain_point(event);
}

void AdjustGainSettingDialog::on_mouse_release(QMouseEvent*) {
  m_point_index = -1;
}

void AdjustGainSettingDialog::on_mouse_move(QMouseEvent* event) {
  if(m_point_index == -1) {
    if(auto hit_index = hit_gain_point(event); hit_index >= 0) {
      setCursor(Qt::SizeHorCursor);
    } else {
      setCursor(Qt::ArrowCursor);
    }
    return;
  }
  auto iter = ui->gain_plot->graph(1)->data()->begin();
  std::advance(iter, m_point_index);
  auto x_value = ui->gain_plot->xAxis2->pixelToCoord(event->pos().x());
  if(!ui->gain_plot->xAxis2->range().contains(x_value)) {
    return;
  }
  iter->value = x_value;
  ui->gain_plot->replot();
  update_manual_gains(m_point_index);
  std::transform(m_base_gains.begin(), m_base_gains.end(),
    m_manual_gains.begin(), m_gains.begin(),
    [](auto value1, auto value2) {
      return value1 * value2;
    });
  plot_data();
  m_has_submitted = false;
}
