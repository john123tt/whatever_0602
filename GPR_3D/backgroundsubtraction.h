#ifndef BACKGROUND_SUBTRACTION_H
#define BACKGROUND_SUBTRACTION_H
#include <optional>
#include <QVector>

class BackgroundSubtraction {
  public:
    BackgroundSubtraction();

    void enable(bool is_enabled);
    bool is_enabled() const;

    void set_background_frames(int frames);
    void set_max_depth(int max_depth);
    void set_time_parameters(int start_time, int end_time);

    void reset();
    void update_background(const QVector<double>& input);
    QVector<double> process(const QVector<double>& input);

  private:
    bool m_is_enabled;
    int m_frames;
    int m_max_depth;
    int m_start_time;
    int m_end_time;
    std::optional<int> m_size;
    QVector<QVector<double>> m_buffer;
    QVector<double> m_background;
};

#endif // BACKGROUND_SUBTRACTION_H
