#ifndef UTILITY_H
#define UTILITY_H
#include <span>
#include <QComboBox>
#include <QIcon>
#include <QListView>
#include <QPushButton>
#include <QString>
#include <QToolBar>
#include <QToolButton>
#include <QWidgetAction>
#include "GPR_SDK.h"

namespace GPR {
  int to_int(GPR_ScanPoints scan_points);

  QWidgetAction* make_action(const QString& name, QWidget& component, QIcon* icon, QWidget* parent);

  QToolButton* make_tool_button(const QIcon& icon, const QString& name, QToolBar& tool_bar, QWidget* parent);

  double custom_mapping(double x);

  std::wstring utf8_to_wstring(const std::string& str);

  std::string wstring_to_utf8(const std::wstring& wstr);

  QVector<QVector<GPR_Complex>> convert(const GPR_ComplexArray* data, int count);

  QVector<QVector<GPR_Complex>> matlab_default_czt_transform(
      const GPR_ComplexArray* arrays,
      int array_count,
      const GPR_FrequencyRangeAndPoints& frequency_range_points,
      int start_time_ns,
      int end_time_ns);

  template<typename Enum>
  struct EnumTraits;

  template<>
  struct EnumTraits<GPR_TxChannel> {
    static constexpr GPR_TxChannel NONE = GPR_TX_NONE;
  };

  template<>
  struct EnumTraits<GPR_RxChannel> {
    static constexpr GPR_RxChannel NONE = GPR_RX_NONE;
  };

//   template<typename T>
//   void update_calibrated_channel(std::span<T> channels, QComboBox& combo_box) {
//     auto list_view = qobject_cast<QListView*>(combo_box.view());
//     auto valid_index = -1;
//     for(auto i = 0; i < channels.size(); ++i) {
//       if(channels[i] == EnumTraits<T>::NONE) {
//         list_view->setRowHidden(i, true);
//       } else {
//         if(valid_index < 0) {
//           valid_index = i;
//         }
//         list_view->setRowHidden(i, false);
//       }
//     }
//     if(channels[combo_box.currentIndex()] == EnumTraits<T>::NONE &&
//         valid_index >= 0) {
//       combo_box.setCurrentIndex(valid_index);
//     }
//   }
}
#endif // UTILITY_H
