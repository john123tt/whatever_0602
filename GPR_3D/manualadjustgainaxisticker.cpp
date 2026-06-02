#include "manualadjustgainaxisticker.h"
#include "utility.h"

using namespace GPR;

ManualAdjustGainAxisTicker::ManualAdjustGainAxisTicker()
  : QCPAxisTicker() {}

QString ManualAdjustGainAxisTicker::getTickLabel(double tick, const QLocale& locale,
    QChar formatChar, int precision) {
  if(tick >= -2 && tick < 1) {
    return locale.toString(custom_mapping(tick), formatChar.toLatin1(), precision);

  } else if(tick >= 1) {
    return locale.toString(tick, formatChar.toLatin1(), precision);
  }
  return "";
}
