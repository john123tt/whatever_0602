#include "backgroundsubtraction.h"
#include <algorithm>
#include <ranges>

BackgroundSubtraction::BackgroundSubtraction()
  : m_is_enabled(false),
    m_frames(0),
    m_max_depth(0),
    m_start_time(0),
    m_end_time(0) {}

void BackgroundSubtraction::enable(bool is_enabled) {
  m_is_enabled = is_enabled;
}

bool BackgroundSubtraction::is_enabled() const {
  return m_is_enabled;
}

void BackgroundSubtraction::set_background_frames(int frames) {
  m_frames = frames;
}

void BackgroundSubtraction::set_max_depth(int max_depth) {
  if(m_max_depth != max_depth) {
    m_max_depth = max_depth;
    m_size = std::nullopt;
  }
}

void BackgroundSubtraction::set_time_parameters(int start_time, int end_time) {
  m_start_time = start_time;
  m_end_time = end_time;
}

void BackgroundSubtraction::reset() {
  m_background.clear();
  m_size = std::nullopt;
}

void BackgroundSubtraction::update_background(const QVector<double>& input) {
  if(!m_is_enabled || !m_background.isEmpty()) {
    return;
  }
  m_buffer.append(input);
  if(m_buffer.size() == m_frames) {
    m_background = QVector<double>(input.size(), 0.0);
    for(const auto& row : m_buffer) {
      std::ranges::transform(row, m_background, m_background.begin(), std::plus<>());
    }
    m_buffer.clear();
    std::transform(m_background.begin(), m_background.end(), m_background.begin(),
      [=] (double val) { return val / m_frames; });
  }
}

QVector<double> BackgroundSubtraction::process(const QVector<double>& input) {
  if(!m_is_enabled || m_background.isEmpty()) {
    return input;
  }
  if(!m_size) {
    m_size = input.size() *
      static_cast<double>(m_max_depth - m_start_time) / (m_end_time - m_start_time);
  }
  auto result = QVector<double>(input.size(), 0.0);
  std::ranges::transform(input | std::views::take(*m_size),
    m_background | std::views::take(*m_size), result.begin(), std::minus<>());
  std::copy(input.begin() + *m_size, input.end(), result.begin() + *m_size);
  return result;
}
