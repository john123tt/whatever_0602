#include "utility.h"
#include <cmath>
#include <complex>
#include <memory>
#include <mutex>
#include <vector>
#include <QHBoxLayout>
#include <QLabel>
#include <Windows.h>
#include "global.h"

namespace {
constexpr double kPi = 3.141592653589793238462643383279502884;
constexpr double kMatlabDefaultKaiserBeta = 2.0;

double bessel_i0(double x)
{
  double sum = 1.0;
  double term = 1.0;
  const double half_x = x * 0.5;
  for(int k = 1; k <= 50; ++k) {
    term *= (half_x * half_x) / (k * k);
    sum += term;
    if(term < sum * 1e-14) {
      break;
    }
  }
  return sum;
}

QVector<double> kaiser_window(int count, double beta)
{
  QVector<double> window(count);
  if(count <= 0) {
    return window;
  }
  if(count == 1) {
    window[0] = 1.0;
    return window;
  }

  const double denominator = bessel_i0(beta);
  const double scale = 2.0 / (count - 1);
  for(int i = 0; i < count; ++i) {
    const double r = scale * i - 1.0;
    const double inside = 1.0 - r * r;
    window[i] = bessel_i0(beta * std::sqrt(inside > 0.0 ? inside : 0.0)) / denominator;
  }
  return window;
}

int next_power_of_two(int value)
{
  int result = 1;
  while(result < value) {
    result <<= 1;
  }
  return result;
}

void fft_inplace(std::vector<std::complex<double>>& data, bool inverse)
{
  const int n = static_cast<int>(data.size());
  for(int i = 1, j = 0; i < n; ++i) {
    int bit = n >> 1;
    for(; j & bit; bit >>= 1) {
      j ^= bit;
    }
    j ^= bit;
    if(i < j) {
      std::swap(data[i], data[j]);
    }
  }

  for(int len = 2; len <= n; len <<= 1) {
    const double angle = (inverse ? -2.0 : 2.0) * kPi / len;
    const std::complex<double> w_len(std::cos(angle), std::sin(angle));
    for(int i = 0; i < n; i += len) {
      std::complex<double> w(1.0, 0.0);
      const int half = len >> 1;
      for(int j = 0; j < half; ++j) {
        const auto u = data[i + j];
        const auto v = data[i + j + half] * w;
        data[i + j] = u + v;
        data[i + j + half] = u - v;
        w *= w_len;
      }
    }
  }

  if(inverse) {
    const double scale = 1.0 / n;
    for(auto& value : data) {
      value *= scale;
    }
  }
}

std::vector<std::complex<double>> convolve_fft(
    const std::vector<std::complex<double>>& a,
    const std::vector<std::complex<double>>& b)
{
  const int needed = static_cast<int>(a.size() + b.size()) - 1;
  const int n = next_power_of_two(needed);
  std::vector<std::complex<double>> fa(n, {0.0, 0.0});
  std::vector<std::complex<double>> fb(n, {0.0, 0.0});
  std::copy(a.begin(), a.end(), fa.begin());
  std::copy(b.begin(), b.end(), fb.begin());
  fft_inplace(fa, false);
  fft_inplace(fb, false);
  for(int i = 0; i < n; ++i) {
    fa[i] *= fb[i];
  }
  fft_inplace(fa, true);
  fa.resize(needed);
  return fa;
}

struct CztPlan {
  int count = 0;
  double bandwidth_hz = 0.0;
  int start_time_ns = 0;
  int end_time_ns = 0;
  int fft_size = 0;
  QVector<double> window;
  std::vector<std::complex<double>> chirp;
  std::vector<std::complex<double>> a_factor;
  std::vector<std::complex<double>> b_fft;
};

bool matches_czt_plan(
    const CztPlan& plan,
    int count,
    double bandwidth_hz,
    int start_time_ns,
    int end_time_ns)
{
  return plan.count == count &&
      plan.bandwidth_hz == bandwidth_hz &&
      plan.start_time_ns == start_time_ns &&
      plan.end_time_ns == end_time_ns;
}

std::shared_ptr<const CztPlan> get_czt_plan(
    int count,
    double bandwidth_hz,
    int start_time_ns,
    int end_time_ns)
{
  static std::mutex mutex;
  static std::shared_ptr<CztPlan> cached_plan;

  std::lock_guard<std::mutex> lock(mutex);
  if(cached_plan &&
      matches_czt_plan(*cached_plan, count, bandwidth_hz, start_time_ns, end_time_ns)) {
    return cached_plan;
  }

  auto plan = std::make_shared<CztPlan>();
  plan->count = count;
  plan->bandwidth_hz = bandwidth_hz;
  plan->start_time_ns = start_time_ns;
  plan->end_time_ns = end_time_ns;

  const double t0 = start_time_ns * 1e-9;
  const double t1 = end_time_ns * 1e-9;
  const double ts_val = 1.0 / (bandwidth_hz / count);
  const auto W = std::exp(std::complex<double>(0.0, 2.0 * kPi * (t1 - t0) / count / ts_val));
  const auto A = std::exp(std::complex<double>(0.0, 2.0 * kPi * (-t0) / ts_val));
  const auto A_inverse = 1.0 / A;
  const std::complex<double> log_w = std::log(W);

  plan->window = kaiser_window(count, kMatlabDefaultKaiserBeta);
  plan->chirp.resize(count);
  plan->a_factor.resize(count);
  std::complex<double> a_factor_value(1.0, 0.0);
  for(int n = 0; n < count; ++n) {
    plan->chirp[n] = std::exp(log_w * (0.5 * n * n));
    plan->a_factor[n] = a_factor_value;
    a_factor_value *= A_inverse;
  }

  const int needed = 3 * count - 2;
  plan->fft_size = next_power_of_two(needed);
  plan->b_fft.assign(plan->fft_size, {0.0, 0.0});
  for(int m = -(count - 1); m <= count - 1; ++m) {
    plan->b_fft[m + count - 1] = std::exp(log_w * (-0.5 * m * m));
  }
  fft_inplace(plan->b_fft, false);

  cached_plan = plan;
  return cached_plan;
}
}

int GPR::to_int(GPR_ScanPoints scan_points) {
  switch(scan_points) {
  case GPR_POINT_128:
    return 128;
  case GPR_POINT_256:
    return 256;
  case GPR_POINT_512:
    return 512;
  case GPR_POINT_1024:
    return 1024;
  default:
    return 1024;
  }
}

QWidgetAction* GPR::make_action(const QString& name, QWidget& component, QIcon* icon,
    QWidget* parent) {
  auto widget = new QWidget();
  widget->setObjectName("ActionWidget");
  auto layout = new QHBoxLayout(widget);
  layout->setContentsMargins({8, 2, 8, 2});
  layout->setSpacing(8);
  if(icon) {
    auto icon_label = new QLabel();
    icon_label->setPixmap(icon->pixmap(16, 16));
    icon_label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    layout->addWidget(icon_label);
  }
  if(!name.isEmpty()) {
    layout->addWidget(new QLabel(name), 0, Qt::AlignLeft | Qt::AlignVCenter);
  }
  layout->addWidget(&component, 0, Qt::AlignLeft | Qt::AlignVCenter);
  auto action = new QWidgetAction(parent);
  action->setDefaultWidget(widget);
  widget->setStyleSheet(R"(ActionWidget:hover {background: yellow})");
  return action;
}

QToolButton* GPR::make_tool_button(const QIcon& icon, const QString& name,
    QToolBar& tool_bar, QWidget* parent) {
  auto action = new QAction(icon, name, parent);
  tool_bar.addAction(action);
  auto button = static_cast<QToolButton*>(tool_bar.widgetForAction(action));
  button->addAction(action);
  button->setPopupMode(QToolButton::InstantPopup);
  return button;
}

double GPR::custom_mapping(double x) {
  if(x >= 0 && x <= 1) {
    return 0.5 + (x - 0) * 0.5;
  } else if(x >= -1 && x < 0) {
    return 0.3 + (x + 1) * 0.2;
  } else if(x >= -2 && x < -1) {
    return 0.2 + (x + 2) * 0.1;
  } else {
    return x;
  }
}

std::wstring GPR::utf8_to_wstring(const std::string& str) {
  if(str.empty()) {
    return std::wstring();
  }
  int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0],
    static_cast<int>(str.size()), nullptr, 0);
  std::wstring wstrTo(size_needed, 0);
  MultiByteToWideChar(CP_UTF8, 0, &str[0], static_cast<int>(str.size()), &wstrTo[0],
    size_needed);
  return wstrTo;
}

std::string GPR::wstring_to_utf8(const std::wstring& wstr) {
  if(wstr.empty()) {
    return std::string();
  }
  int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0],
    static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);
  std::string strTo(size_needed, 0);
  WideCharToMultiByte(CP_UTF8, 0, &wstr[0], static_cast<int>(wstr.size()), &strTo[0],
    size_needed, nullptr, nullptr);
  return strTo;
}

QVector<QVector<GPR_Complex>> GPR::convert(const GPR_ComplexArray* data, int count) {
  auto result = QVector<QVector<GPR_Complex>>();
  if(!data) {
    return result;
  }
  result.reserve(count);
  for(int i = 0; i < count; ++i) {
    if(data[i].data && data[i].count > 0) {
      auto complex_vector = QVector<GPR_Complex>();
      complex_vector.assign(data[i].data, data[i].data + data[i].count);
      result.push_back(std::move(complex_vector));
    }
  }
  return result;
}

QVector<QVector<GPR_Complex>> GPR::matlab_default_czt_transform(
    const GPR_ComplexArray* arrays,
    int array_count,
    const GPR_FrequencyRangeAndPoints& frequency_range_points,
    int start_time_ns,
    int end_time_ns)
{
  QVector<QVector<GPR_Complex>> result;
  if(!arrays || array_count <= 0) {
    return result;
  }

  const double bandwidth_hz =
      static_cast<double>(frequency_range_points.stop_frequency - frequency_range_points.start_frequency) * 1e6;
  if(!(bandwidth_hz > 0.0) || end_time_ns <= start_time_ns) {
    return result;
  }

  result.reserve(array_count);
  for(int array_index = 0; array_index < array_count; ++array_index) {
    const auto& input = arrays[array_index];
    const int count = input.count;
    if(!input.data || count <= 0) {
      continue;
    }

    const auto plan = get_czt_plan(count, bandwidth_hz, start_time_ns, end_time_ns);
    std::vector<std::complex<double>> fa(plan->fft_size, {0.0, 0.0});
    for(int n = 0; n < count; ++n) {
      const auto sample =
          std::complex<double>(input.data[n].real, input.data[n].imag) * plan->window[n];
      fa[n] = sample * plan->a_factor[n] * plan->chirp[n];
    }

    fft_inplace(fa, false);
    for(int i = 0; i < plan->fft_size; ++i) {
      fa[i] *= plan->b_fft[i];
    }
    fft_inplace(fa, true);

    QVector<GPR_Complex> output(count);
    for(int k = 0; k < count; ++k) {
      const auto value = plan->chirp[k] * fa[k + count - 1];
      output[k] = GPR_Complex{value.real(), value.imag()};
    }

    result.push_back(std::move(output));
  }
  return result;
}
