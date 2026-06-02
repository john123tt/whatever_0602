#ifndef MANUALADJUSTGAINAXISTICKER_H
#define MANUALADJUSTGAINAXISTICKER_H
#include "qcustomplot.h"

class ManualAdjustGainAxisTicker : public QCPAxisTicker {
public:

  ManualAdjustGainAxisTicker();

  QString getTickLabel(double tick, const QLocale& locale, QChar formatChar,
    int precision) override;
};

#endif
