#include "realtimeprocessingsettingview.h"
#include "ui_realtimeprocessingsettingview.h"
#include "setting.h"
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QAbstractButton>
#include <QSizePolicy>
using namespace GPR;

RealTimeProcessingSettingView::RealTimeProcessingSettingView(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::RealTimeProcessingSettingView) {
  ui->setupUi(this);
  ui->eliminate_background->setVisible(false);
  ui->eliminate_background->setCurrentIndex(1);
  ui->label_trigger_source_4->setVisible(false);
  ui->adjust_gain_button->setVisible(false);
  setStyleSheet(R"(
    QGroupBox {
      background: transparent;
      border: 1px solid #cbd2d9;
      border-radius: 4px;
      margin-top: 1ex;
    }
    QGroupBox::title {
      subcontrol-origin: margin;
      left: 8px;
      padding: 0 3px;
    }
  )");
  const auto buttons = findChildren<QAbstractButton*>();
  for (auto* button : buttons) {
    if (!button) {
      continue;
    }
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    button->setMinimumHeight(static_cast<int>(button->sizeHint().height() * 1.33));
  }
  m_settings.setValue(mine_ground_trend_flatten_enabled_key, false);
  m_settings.setValue(v2_hyperbola_detector_enabled_key, false);

  auto transform_group = new QGroupBox(QString::fromUtf8("时域转换"), this);
  auto transform_layout = new QGridLayout(transform_group);
  transform_layout->setContentsMargins(8, 8, 8, 8);
  transform_layout->setHorizontalSpacing(6);
  transform_layout->setVerticalSpacing(6);

  auto transform_combo = new QComboBox(transform_group);
  transform_combo->setObjectName("time_domain_transform_combo");
  transform_combo->addItem(QString::fromUtf8("CZT（当前算法）"), 0);
  transform_combo->addItem(QString::fromUtf8("SDK 时域缩放（原版）"), 1);
  transform_combo->setCurrentIndex(m_settings.value(time_domain_transform_key, 0).toInt() == 1 ? 1 : 0);

  transform_layout->addWidget(new QLabel(QString::fromUtf8("转换算法"), transform_group), 0, 0);
  transform_layout->addWidget(transform_combo, 0, 1);
  ui->gridLayout->addWidget(transform_group, 2, 0, 1, 3);

  connect(transform_combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, &RealTimeProcessingSettingView::on_time_domain_transform_currentIndexChanged);

  auto mine_group = new QGroupBox(QString::fromUtf8("探雷专栏"), this);
  auto mine_layout = new QGridLayout(mine_group);
  mine_layout->setContentsMargins(8, 8, 8, 8);
  mine_layout->setHorizontalSpacing(6);
  mine_layout->setVerticalSpacing(6);

  auto trend_flatten_combo = new QComboBox(mine_group);
  trend_flatten_combo->setObjectName("mine_ground_trend_flatten_combo");
  trend_flatten_combo->addItem(QString::fromUtf8("开启"), true);
  trend_flatten_combo->addItem(QString::fromUtf8("关闭"), false);
  trend_flatten_combo->setCurrentIndex(1);

  mine_layout->addWidget(new QLabel(QString::fromUtf8("趋势地面拉直 v2"), mine_group), 0, 0);
  mine_layout->addWidget(trend_flatten_combo, 0, 1);
  auto v2_detector_combo = new QComboBox(mine_group);
  v2_detector_combo->setObjectName("v2_hyperbola_detector_combo");
  v2_detector_combo->addItem(QString::fromUtf8("开启"), true);
  v2_detector_combo->addItem(QString::fromUtf8("关闭"), false);
  v2_detector_combo->setCurrentIndex(1);

  mine_layout->addWidget(new QLabel(QString::fromUtf8("V2谷值双曲检测"), mine_group), 1, 0);
  mine_layout->addWidget(v2_detector_combo, 1, 1);
  mine_layout->addWidget(new QLabel(QString::fromUtf8("ROI 0-5ns, 主层2ns, R=200/300/400"), mine_group), 2, 0, 1, 2);
  ui->gridLayout->addWidget(mine_group, 3, 0, 1, 3);

  connect(trend_flatten_combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, &RealTimeProcessingSettingView::on_mine_ground_trend_flatten_currentIndexChanged);
  connect(v2_detector_combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, &RealTimeProcessingSettingView::on_v2_hyperbola_detector_currentIndexChanged);
}

RealTimeProcessingSettingView::~RealTimeProcessingSettingView() {
  delete ui;
}

void RealTimeProcessingSettingView::set_eliminate_background(bool is_enabled) {
  ui->eliminate_background->blockSignals(true);
  ui->eliminate_background->setCurrentIndex(is_enabled ? 0 : 1);
  ui->eliminate_background->blockSignals(false);
}

bool RealTimeProcessingSettingView::is_background_elimination_enabled() const {
  return ui->eliminate_background->currentIndex() == 0;
}

bool RealTimeProcessingSettingView::is_sdk_time_transform_enabled() const {
  const auto transform_combo = findChild<QComboBox*>("time_domain_transform_combo");
  return transform_combo && transform_combo->currentData().toInt() == 1;
}

bool RealTimeProcessingSettingView::is_mine_ground_trend_flatten_enabled() const {
  const auto combo = findChild<QComboBox*>("mine_ground_trend_flatten_combo");
  return combo && combo->currentData().toBool();
}

bool RealTimeProcessingSettingView::is_v2_hyperbola_detector_enabled() const {
  const auto combo = findChild<QComboBox*>("v2_hyperbola_detector_combo");
  return combo && combo->currentData().toBool();
}

void RealTimeProcessingSettingView::enable_eliminate_background_button(bool is_enabled) {
  ui->eliminate_background->setEnabled(is_enabled);
}

void RealTimeProcessingSettingView::enable_adjust_gain_button(bool is_enabled) {
  ui->adjust_gain_button->setEnabled(is_enabled);
}

void RealTimeProcessingSettingView::enable_time_transform_selector(bool is_enabled) {
  const auto transform_combo = findChild<QComboBox*>("time_domain_transform_combo");
  if(transform_combo) {
    transform_combo->setEnabled(is_enabled);
  }
}

void RealTimeProcessingSettingView::on_adjust_gain_button_clicked() {
  emit adjust_gain();
}

void RealTimeProcessingSettingView::on_eliminate_background_currentIndexChanged(int index) {
  emit eliminate_background(index == 0);
}

void RealTimeProcessingSettingView::on_comboBox_currentIndexChanged(int index)
{
    QVector<QPair<double, QColor>> color_stops;
    if(index==0){
        color_stops={{0.0, Qt::black}, {1.0, Qt::white}};
    }
    else if(index==1)
    {
        color_stops={
            {1.0, QColor(0, 0, 0)},   // 黑色
            {0.75, QColor(0, 0, 255)},    // 蓝色
            {0.5, QColor(0, 255, 0)},     // 绿色
            {0.25, QColor(255, 0, 0)},    // 红色
            {0.0, QColor(255, 255, 0)}        // 黄色
        };
    }
    else if(index==2){
        color_stops={{1.0, QColor(0, 0, 255)}, // 红色
        {0.5, QColor(255, 255, 255)},// 白色
        {0.0, QColor(255, 0, 0)} };// 蓝色
    }
    else if(index==3)
    {
        color_stops = {
            {1.0, QColor(0, 0, 0)},       // 黑色
            {0.75, QColor(0, 0, 128)},    // 深蓝色
            {0.5, QColor(0, 255, 255)}, // 浅蓝色
            {0.25, QColor(255, 0, 0)},    // 红色
            {0.0, QColor(139, 0, 0)}      // 褐红色
        };
    }
    emit change_color_stops(color_stops);
}

void RealTimeProcessingSettingView::on_time_domain_transform_currentIndexChanged(int index)
{
    m_settings.setValue(time_domain_transform_key, index == 1 ? 1 : 0);
}

void RealTimeProcessingSettingView::on_mine_ground_trend_flatten_currentIndexChanged(int)
{
    const bool enabled = is_mine_ground_trend_flatten_enabled();
    m_settings.setValue(mine_ground_trend_flatten_enabled_key, enabled);
    emit mine_ground_trend_flatten_changed(enabled);
}

void RealTimeProcessingSettingView::on_v2_hyperbola_detector_currentIndexChanged(int)
{
    const bool enabled = is_v2_hyperbola_detector_enabled();
    m_settings.setValue(v2_hyperbola_detector_enabled_key, enabled);
    emit v2_hyperbola_detector_changed(enabled);
}
