#ifndef V2HYPERBOLADETECTOR_H
#define V2HYPERBOLADETECTOR_H

#include <QVector>

struct V2HyperbolaParams {
    double t_min_ns = 0.0;
    double t_main_ns = 1.5;
    double t_max_ns = 6.0;
    int rmax_tr = 0;
    QVector<int> radii_tr;
    int stride_tr = 10;
    double seed_threshold = 2.6;
    double confirm_threshold = 0.2;
    double continuity_threshold = 0.2;
};

struct V2HyperbolaBox {
    double x = 0.0;
    double y_ns = 0.0;
    double width = 0.0;
    double height_ns = 0.0;
    double score = 0.0;
    double hyperbola_score = 0.0;
    double apex_ns = 0.0;
    double radius = 0.0;
    int mode = 0;
    bool edge = false;
};

struct V2HyperbolaResult {
    QVector<V2HyperbolaBox> boxes;
    QVector<double> sigma;
    int num_seeds = 0;
    int num_verified = 0;
    int num_rejected_seed = 0;
    int num_black = 0;
    int num_black_white = 0;
    int num_white_only_rejected = 0;
    double score_max = 0.0;
    double sigma_min = 0.0;
    double sigma_median = 0.0;
    double sigma_max = 0.0;
};

V2HyperbolaParams default_v2_hyperbola_params();

V2HyperbolaResult detect_v2_hyperbola(
    const QVector<QVector<double>>& traces,
    double dt_ns,
    const V2HyperbolaParams& params = default_v2_hyperbola_params());

QVector<QVector<double>> apply_v2_hyperbola_gain(
    const QVector<QVector<double>>& traces,
    const QVector<V2HyperbolaBox>& boxes,
    double dt_ns,
    double base_gain = 5.0,
    double slope_per_ns = 2.0,
    int left_pad_tr = 0,
    int right_pad_tr = 0,
    int up_pad_pts = 0,
    int down_pad_pts = 0);

QVector<QVector<double>> apply_v2_hyperbola_detector(
    const QVector<QVector<double>>& traces,
    double dt_ns,
    const V2HyperbolaParams& params = default_v2_hyperbola_params(),
    V2HyperbolaResult* result = nullptr);

#endif // V2HYPERBOLADETECTOR_H
