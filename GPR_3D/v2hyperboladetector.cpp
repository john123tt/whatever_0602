#include "v2hyperboladetector.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>

namespace {

constexpr double kEps = 1e-12;

double finite_value(double value)
{
    return std::isfinite(value) ? value : 0.0;
}

double median(QVector<double> values)
{
    values.erase(std::remove_if(values.begin(), values.end(),
        [](double value) { return !std::isfinite(value); }), values.end());
    if (values.isEmpty()) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    std::sort(values.begin(), values.end());
    const int mid = values.size() / 2;
    if (values.size() % 2 == 1) {
        return values[mid];
    }
    return 0.5 * (values[mid - 1] + values[mid]);
}

double mean(const QVector<double>& values)
{
    if (values.isEmpty()) {
        return 0.0;
    }
    double sum = 0.0;
    int count = 0;
    for (double value : values) {
        if (std::isfinite(value)) {
            sum += value;
            ++count;
        }
    }
    return count > 0 ? sum / static_cast<double>(count) : 0.0;
}

double mean_matrix_region(
    const QVector<QVector<double>>& data,
    int r1,
    int r2,
    int c1,
    int c2)
{
    if (data.isEmpty() || data[0].isEmpty()) {
        return 0.0;
    }
    const int rows = data.size();
    const int cols = data[0].size();
    r1 = std::max(0, std::min(rows - 1, r1));
    r2 = std::max(0, std::min(rows - 1, r2));
    c1 = std::max(0, std::min(cols - 1, c1));
    c2 = std::max(0, std::min(cols - 1, c2));
    if (r1 > r2) {
        std::swap(r1, r2);
    }
    if (c1 > c2) {
        std::swap(c1, c2);
    }
    double sum = 0.0;
    int count = 0;
    for (int r = r1; r <= r2; ++r) {
        for (int c = c1; c <= c2; ++c) {
            const double value = data[r][c];
            if (std::isfinite(value)) {
                sum += value;
                ++count;
            }
        }
    }
    return count > 0 ? sum / static_cast<double>(count) : 0.0;
}

QVector<double> centered_moving_average_1d(const QVector<double>& values, int window)
{
    if (values.isEmpty() || window <= 1) {
        return values;
    }
    if (window % 2 == 0) {
        ++window;
    }
    const int size = values.size();
    const int half = window / 2;
    QVector<double> prefix(size + 1, 0.0);
    for (int i = 0; i < size; ++i) {
        prefix[i + 1] = prefix[i] + values[i];
    }
    QVector<double> out(size, 0.0);
    for (int i = 0; i < size; ++i) {
        const int a = std::max(0, i - half);
        const int b = std::min(size - 1, i + half);
        out[i] = (prefix[b + 1] - prefix[a]) / static_cast<double>(b - a + 1);
    }
    return out;
}

QVector<QVector<double>> horizontal_moving_average(const QVector<QVector<double>>& data, int window)
{
    QVector<QVector<double>> out;
    out.reserve(data.size());
    for (const auto& row : data) {
        out.append(centered_moving_average_1d(row, window));
    }
    return out;
}

double matrix_value(const QVector<QVector<double>>& data, int row, int col)
{
    if (row < 0 || row >= data.size() || col < 0 || data.isEmpty() || col >= data[row].size()) {
        return 0.0;
    }
    return data[row][col];
}

QVector<int> unique_sorted(QVector<int> values)
{
    std::sort(values.begin(), values.end());
    values.erase(std::unique(values.begin(), values.end()), values.end());
    return values;
}

QVector<int> radius_list(const V2HyperbolaParams& params, int nt)
{
    const int max_allowed = std::max(12, (nt - 1) / 2);
    QVector<int> values;
    if (!params.radii_tr.isEmpty()) {
        values = params.radii_tr;
    } else if (params.rmax_tr <= 0) {
        values = {200, 300, 400};
    } else {
        const int r_user = params.rmax_tr;
        values = {
            static_cast<int>(std::round(0.80 * r_user)),
            r_user,
            static_cast<int>(std::round(1.20 * r_user))
        };
    }
    QVector<int> filtered;
    for (int value : unique_sorted(values)) {
        if (value >= 12 && value <= max_allowed) {
            filtered.append(value);
        }
    }
    if (filtered.isEmpty()) {
        const int fallback_radius = !params.radii_tr.isEmpty()
            ? params.radii_tr.first()
            : params.rmax_tr;
        filtered.append(std::max(12, std::min(max_allowed, std::max(12, std::abs(fallback_radius)))));
    }
    return unique_sorted(filtered);
}

struct Seed {
    int row = 0;
    int col = 0;
    double value = 0.0;
};

struct CandidatePack {
    int row = 0;
    int col = 0;
    int radius = 0;
    int left_radius = 0;
    int right_radius = 0;
    int rise = 0;
    double hyp = 0.0;
    int mode = 0;
    bool edge = false;
    double hline = 0.0;
    double score = -std::numeric_limits<double>::infinity();
    bool black_pass = false;
    bool white_pass = false;
};

QVector<V2HyperbolaBox> prune_candidates(QVector<V2HyperbolaBox> boxes)
{
    std::sort(boxes.begin(), boxes.end(), [](const auto& a, const auto& b) {
        if (a.score == b.score) {
            return a.apex_ns < b.apex_ns;
        }
        return a.score > b.score;
    });
    QVector<V2HyperbolaBox> kept;
    for (const auto& box : boxes) {
        const double x1 = box.x;
        const double x2 = box.x + box.width - 1.0;
        bool duplicate = false;
        for (const auto& old : kept) {
            const double ox1 = old.x;
            const double ox2 = old.x + old.width - 1.0;
            const double inter = std::min(x2, ox2) - std::max(x1, ox1) + 1.0;
            const double union_width = std::max(x2, ox2) - std::min(x1, ox1) + 1.0;
            const double overlap = inter / std::max(1.0, union_width);
            if (overlap > 0.32) {
                if (box.score <= 0.94 * old.score) {
                    duplicate = true;
                    break;
                }
                if (std::abs(box.apex_ns - old.apex_ns) < 0.55 && box.score <= 1.05 * old.score) {
                    duplicate = true;
                    break;
                }
                if (box.apex_ns >= old.apex_ns + 0.80 && box.score <= 1.20 * old.score) {
                    duplicate = true;
                    break;
                }
            }
        }
        if (!duplicate) {
            kept.append(box);
        }
    }
    return kept;
}

QVector<V2HyperbolaBox> merge_boxes(QVector<V2HyperbolaBox> boxes, int merge_gap_tr, double merge_gap_ns)
{
    if (boxes.isEmpty()) {
        return boxes;
    }
    std::sort(boxes.begin(), boxes.end(), [](const auto& a, const auto& b) { return a.x < b.x; });
    QVector<bool> used(boxes.size(), false);
    QVector<V2HyperbolaBox> out;
    for (int i = 0; i < boxes.size(); ++i) {
        if (used[i]) {
            continue;
        }
        V2HyperbolaBox cur = boxes[i];
        used[i] = true;
        bool changed = true;
        while (changed) {
            changed = false;
            for (int j = 0; j < boxes.size(); ++j) {
                if (used[j]) {
                    continue;
                }
                const double x1a = cur.x;
                const double x2a = cur.x + cur.width - 1.0;
                const double y1a = cur.y_ns;
                const double y2a = cur.y_ns + cur.height_ns;
                const double x1b = boxes[j].x;
                const double x2b = boxes[j].x + boxes[j].width - 1.0;
                const double y1b = boxes[j].y_ns;
                const double y2b = boxes[j].y_ns + boxes[j].height_ns;
                const bool near_x = x1b <= x2a + merge_gap_tr && x2b >= x1a - merge_gap_tr;
                const bool near_y = (std::min(y2a, y2b) - std::max(y1a, y1b)) >= -merge_gap_ns;
                if (!near_x || !near_y) {
                    continue;
                }
                const double nx1 = std::min(x1a, x1b);
                const double nx2 = std::max(x2a, x2b);
                const double ny1 = std::min(y1a, y1b);
                const double ny2 = std::max(y2a, y2b);
                cur.x = nx1;
                cur.y_ns = ny1;
                cur.width = nx2 - nx1 + 1.0;
                cur.height_ns = ny2 - ny1;
                cur.score = std::max(cur.score, boxes[j].score);
                cur.hyperbola_score = std::max(cur.hyperbola_score, boxes[j].hyperbola_score);
                used[j] = true;
                changed = true;
            }
        }
        out.append(cur);
    }
    return out;
}

} // namespace

V2HyperbolaParams default_v2_hyperbola_params()
{
    return {};
}

V2HyperbolaResult detect_v2_hyperbola(
    const QVector<QVector<double>>& traces,
    double dt_ns,
    const V2HyperbolaParams& params)
{
    V2HyperbolaResult result;
    if (traces.isEmpty() || traces.size() < 25 || traces[0].size() < 10 || dt_ns <= 0.0) {
        return result;
    }

    const int nt = traces.size();
    const int ns = traces[0].size();
    QVector<QVector<double>> a(ns, QVector<double>(nt, 0.0));
    for (int c = 0; c < nt; ++c) {
        for (int r = 0; r < ns && r < traces[c].size(); ++r) {
            a[r][c] = finite_value(traces[c][r]);
        }
    }

    const double t_min = std::max(0.0, params.t_min_ns);
    const double t_main = std::max(t_min + dt_ns, params.t_main_ns);
    const double t_max = std::max(t_main + dt_ns, params.t_max_ns);
    const int stride = std::max(1, params.stride_tr);
    const double t_seed = std::max(0.05, params.seed_threshold);
    const double t_confirm = std::max(0.05, params.confirm_threshold);
    const double t_cont = std::max(0.05, params.continuity_threshold);
    const int idx_min = std::max(0, static_cast<int>(std::floor(t_min / dt_ns)));
    const int idx_max = std::min(ns - 1, static_cast<int>(std::ceil(t_max / dt_ns)));
    if (idx_min >= idx_max) {
        return result;
    }

    const QVector<int> radii = radius_list(params, nt);
    const int r_min = radii.isEmpty() ? 12 : radii.first();

    QVector<double> sigma(ns, 1.0);
    for (int r = 0; r < ns; ++r) {
        QVector<double> abs_row;
        abs_row.reserve(nt);
        for (int c = 0; c < nt; ++c) {
            abs_row.append(std::abs(a[r][c]));
        }
        sigma[r] = median(abs_row) / 0.6745;
    }
    sigma = centered_moving_average_1d(sigma, std::max(3, static_cast<int>(std::round(0.15 / dt_ns))));
    QVector<double> positive_sigma;
    for (double value : sigma) {
        if (std::isfinite(value) && value > 0.0) {
            positive_sigma.append(value);
        }
    }
    double median_sigma = median(positive_sigma);
    if (!std::isfinite(median_sigma) || median_sigma <= 0.0) {
        QVector<double> all_values;
        all_values.reserve(ns * nt);
        for (const auto& row : a) {
            for (double value : row) {
                all_values.append(value);
            }
        }
        const double avg = mean(all_values);
        double var = 0.0;
        for (double value : all_values) {
            var += (value - avg) * (value - avg);
        }
        median_sigma = all_values.isEmpty()
            ? 1.0
            : std::sqrt(var / std::max(1, static_cast<int>(all_values.size())));
    }
    if (!std::isfinite(median_sigma) || median_sigma <= 0.0) {
        median_sigma = 1.0;
    }
    for (double& value : sigma) {
        if (!std::isfinite(value) || value <= 0.0) {
            value = median_sigma;
        }
    }
    result.sigma = sigma;
    QVector<double> finite_sigma;
    finite_sigma.reserve(sigma.size());
    for (double value : sigma) {
        if (std::isfinite(value)) {
            finite_sigma.append(value);
        }
    }
    if (!finite_sigma.isEmpty()) {
        auto minmax = std::minmax_element(finite_sigma.begin(), finite_sigma.end());
        result.sigma_min = *minmax.first;
        result.sigma_max = *minmax.second;
        result.sigma_median = median(finite_sigma);
    }

    QVector<QVector<double>> nn(ns, QVector<double>(nt, 0.0));
    QVector<QVector<double>> pp(ns, QVector<double>(nt, 0.0));
    QVector<QVector<double>> ee(ns, QVector<double>(nt, 0.0));
    for (int r = 0; r < ns; ++r) {
        const double denom = sigma[r] + kEps;
        for (int c = 0; c < nt; ++c) {
            const double z = a[r][c] / denom;
            nn[r][c] = std::max(0.0, -z);
            pp[r][c] = std::max(0.0, z);
            ee[r][c] = std::abs(z);
        }
    }

    QVector<QVector<double>> s_black(ns, QVector<double>(nt, 0.0));
    QVector<QVector<double>> s_white(ns, QVector<double>(nt, 0.0));
    const QVector<int> scales{21, 61, 121};
    constexpr double gamma = 0.75;
    for (int scale : scales) {
        int sw = std::min(nt, std::max(5, scale));
        if (sw % 2 == 0) {
            ++sw;
        }
        int rw = std::min(nt, std::max(sw + 2, 3 * sw));
        if (rw % 2 == 0) {
            ++rw;
        }
        const auto cb = horizontal_moving_average(nn, sw);
        const auto wb = horizontal_moving_average(nn, rw);
        const auto cw = horizontal_moving_average(pp, sw);
        const auto ww = horizontal_moving_average(pp, rw);
        for (int r = 0; r < ns; ++r) {
            for (int c = 0; c < nt; ++c) {
                const double rb = std::max(0.0, (wb[r][c] * rw - cb[r][c] * sw) / std::max(1, rw - sw));
                const double rwht = std::max(0.0, (ww[r][c] * rw - cw[r][c] * sw) / std::max(1, rw - sw));
                s_black[r][c] = std::max(s_black[r][c], cb[r][c] - gamma * rb);
                s_white[r][c] = std::max(s_white[r][c], cw[r][c] - gamma * rwht);
            }
        }
    }

    QVector<double> depth_weight(ns, 0.0);
    const double near_ground_ns = std::max(0.45, t_min + 0.30);
    for (int r = 0; r < ns; ++r) {
        const double t_ns = r * dt_ns;
        if (t_ns >= t_min && t_ns <= t_main) {
            depth_weight[r] = 1.0;
        } else if (t_ns > t_main && t_ns <= t_max) {
            depth_weight[r] = std::exp(-(t_ns - t_main) / 1.2);
        }
        if (t_ns < near_ground_ns) {
            depth_weight[r] *= 0.25;
        }
    }

    QVector<Seed> seeds;
    for (int x = 0; x < nt; x += stride) {
        const int c2 = std::min(nt - 1, x + stride - 1);
        double best = -std::numeric_limits<double>::infinity();
        int best_r = idx_min;
        int best_c = x;
        for (int r = idx_min; r <= idx_max; ++r) {
            for (int c = x; c <= c2; ++c) {
                const double value = std::max(s_black[r][c], 0.92 * s_white[r][c]) * depth_weight[r];
                if (value > best) {
                    best = value;
                    best_r = r;
                    best_c = c;
                }
            }
        }
        if (best >= t_seed) {
            seeds.append({best_r, best_c, best});
        }
    }
    result.num_seeds = seeds.size();
    if (seeds.isEmpty()) {
        return result;
    }

    std::sort(seeds.begin(), seeds.end(), [](const auto& a, const auto& b) {
        return a.value > b.value;
    });
    const int zone_w = std::max(120, static_cast<int>(std::round(0.75 * r_min)));
    QVector<int> zone_count(std::max(1, static_cast<int>(std::ceil(nt / static_cast<double>(zone_w)))), 0);
    QVector<Seed> kept_seeds;
    const int max_seeds = std::min(static_cast<int>(seeds.size()), 180);
    for (const auto& seed : seeds) {
        const int zone = std::max(0, std::min(static_cast<int>(zone_count.size()) - 1, seed.col / zone_w));
        if (zone_count[zone] >= 8) {
            continue;
        }
        bool too_close = false;
        for (const auto& old : kept_seeds) {
            if (std::abs(seed.col - old.col) < std::max(18, r_min / 6) &&
                std::abs(seed.row - old.row) * dt_ns < 0.34) {
                too_close = true;
                break;
            }
        }
        if (too_close) {
            continue;
        }
        kept_seeds.append(seed);
        ++zone_count[zone];
        if (kept_seeds.size() >= max_seeds) {
            break;
        }
    }

    QVector<V2HyperbolaBox> candidates;
    for (const auto& seed : kept_seeds) {
        bool seed_verified = false;
        for (int radius : radii) {
            const int refine_span = std::min(std::max(20, static_cast<int>(std::round(0.10 * radius))), 45);
            const int refine_step = std::max(8, radius / 10);
            QVector<int> x_candidates;
            for (int dx = -refine_span; dx <= refine_span; dx += refine_step) {
                x_candidates.append(std::max(0, std::min(nt - 1, seed.col + dx)));
            }
            x_candidates.append(seed.col);
            x_candidates = unique_sorted(x_candidates);

            CandidatePack best_pack;
            for (int x0 : x_candidates) {
                const int left_avail = x0;
                const int right_avail = nt - 1 - x0;
                const int r_left = std::min(radius, left_avail);
                const int r_right = std::min(radius, right_avail);
                if (std::max(r_left, r_right) < std::max(12, static_cast<int>(std::round(0.35 * radius)))) {
                    continue;
                }
                const int edge_margin = std::max(12, static_cast<int>(std::round(0.12 * radius)));
                const bool near_image_edge = std::min(left_avail, right_avail) <= edge_margin;
                const bool edge_mode = near_image_edge && std::min(r_left, r_right) < 0.55 * radius;
                const int step_dx = std::max(2, radius / 52);
                QVector<int> dx_values;
                for (int dx = -r_left; dx <= r_right; dx += step_dx) {
                    dx_values.append(dx);
                }
                if (dx_values.size() < 12) {
                    continue;
                }
                for (int ro = -2; ro <= 2; ++ro) {
                    const int r0 = std::max(idx_min, std::min(idx_max, seed.row + ro));
                    const double t0_ns = r0 * dt_ns;
                    QVector<int> rise_set = unique_sorted({
                        std::max(2, static_cast<int>(std::round(0.055 * radius))),
                        std::max(2, static_cast<int>(std::round(0.095 * radius))),
                        std::max(2, static_cast<int>(std::round(0.145 * radius)))
                    });
                    for (int rise : rise_set) {
                        QVector<double> on_black;
                        QVector<double> on_white;
                        QVector<double> on_abs;
                        QVector<double> above_b;
                        QVector<double> below_b;
                        QVector<double> above_w;
                        QVector<double> below_w;
                        QVector<int> dx_valid;
                        on_black.reserve(dx_values.size());
                        on_white.reserve(dx_values.size());
                        on_abs.reserve(dx_values.size());
                        const int sep = std::max(2, static_cast<int>(std::round(0.06 * rise + 1.0)));
                        for (int dx : dx_values) {
                            const int rr = r0 + static_cast<int>(std::round(rise * std::pow(dx / static_cast<double>(std::max(radius, 1)), 2.0)));
                            const int cc = x0 + dx;
                            if (rr < 0 || rr >= ns || cc < 0 || cc >= nt) {
                                continue;
                            }
                            dx_valid.append(dx);
                            on_black.append(matrix_value(nn, rr, cc));
                            on_white.append(matrix_value(pp, rr, cc));
                            on_abs.append(matrix_value(ee, rr, cc));
                            above_b.append(matrix_value(nn, std::max(0, rr - sep), cc));
                            below_b.append(matrix_value(nn, std::min(ns - 1, rr + sep), cc));
                            above_w.append(matrix_value(pp, std::max(0, rr - sep), cc));
                            below_w.append(matrix_value(pp, std::min(ns - 1, rr + sep), cc));
                        }
                        if (on_black.size() < 12) {
                            continue;
                        }
                        QVector<double> left_b;
                        QVector<double> right_b;
                        QVector<double> left_w;
                        QVector<double> right_w;
                        for (int i = 0; i < dx_valid.size(); ++i) {
                            if (dx_valid[i] < -0.22 * radius) {
                                left_b.append(on_black[i]);
                                left_w.append(on_white[i]);
                            } else if (dx_valid[i] > 0.22 * radius) {
                                right_b.append(on_black[i]);
                                right_w.append(on_white[i]);
                            }
                        }
                        const int n_left = left_b.size();
                        const int n_right = right_b.size();
                        const bool two_side_visible = n_left >= 4 && n_right >= 4;
                        const double lb = mean(left_b);
                        const double rb = mean(right_b);
                        const double lw = mean(left_w);
                        const double rw = mean(right_w);
                        const double cont_lb = std::count_if(left_b.begin(), left_b.end(), [t_cont](double v) { return v > t_cont; }) / static_cast<double>(std::max(1, n_left));
                        const double cont_rb = std::count_if(right_b.begin(), right_b.end(), [t_cont](double v) { return v > t_cont; }) / static_cast<double>(std::max(1, n_right));
                        const double cont_lw = std::count_if(left_w.begin(), left_w.end(), [t_cont](double v) { return v > t_cont; }) / static_cast<double>(std::max(1, n_left));
                        const double cont_rw = std::count_if(right_w.begin(), right_w.end(), [t_cont](double v) { return v > t_cont; }) / static_cast<double>(std::max(1, n_right));
                        const double bal_b = std::min(lb, rb) / (std::max(lb, rb) + kEps);
                        const double bal_w = std::min(lw, rw) / (std::max(lw, rw) + kEps);
                        const double side_cont_b = std::min(cont_lb, cont_rb);
                        const double side_cont_w = std::min(cont_lw, cont_rw);
                        const double best_side_b = std::max(cont_lb, cont_rb);
                        const double best_side_w = std::max(cont_lw, cont_rw);
                        const double cont_b = std::count_if(on_black.begin(), on_black.end(), [t_cont](double v) { return v > t_cont; }) /
                                              static_cast<double>(std::max(1, static_cast<int>(on_black.size())));
                        const double cont_w = std::count_if(on_white.begin(), on_white.end(), [t_cont](double v) { return v > t_cont; }) /
                                              static_cast<double>(std::max(1, static_cast<int>(on_white.size())));
                        const int ap_half = std::max(4, static_cast<int>(std::round(0.075 * radius)));
                        const double apex_black = mean_matrix_region(nn, r0 - 3, r0 + 3, x0 - ap_half, x0 + ap_half);
                        const double apex_white = mean_matrix_region(pp, r0 - 3, r0 + 3, x0 - ap_half, x0 + ap_half);
                        const double hyp_b = mean(on_black) - 0.45 * mean(above_b) - 0.45 * mean(below_b);
                        const double hyp_w = mean(on_white) - 0.45 * mean(above_w) - 0.45 * mean(below_w);
                        if (!std::isfinite(hyp_b) || !std::isfinite(hyp_w)) {
                            continue;
                        }
                        const int x1p = std::max(0, x0 - std::max(r_left, 0));
                        const int x2p = std::min(nt - 1, x0 + std::max(r_right, 0));
                        const int y2p = std::min(ns - 1, r0 + std::max(3, rise));
                        int texture_count = 0;
                        int texture_total = 0;
                        for (int rr = r0; rr <= y2p; ++rr) {
                            for (int cc = x1p; cc <= x2p; ++cc) {
                                if (ee[rr][cc] > t_cont) {
                                    ++texture_count;
                                }
                                ++texture_total;
                            }
                        }
                        const double p_texture = texture_count / static_cast<double>(std::max(1, texture_total));
                        const double top_penalty = t0_ns < near_ground_ns ? 0.80 : 0.0;
                        const double center_b = two_side_visible ? (0.68 * bal_b + 0.48 * side_cont_b) : (0.25 * best_side_b);
                        const double center_w = two_side_visible ? (0.68 * bal_w + 0.48 * side_cont_w) : (0.25 * best_side_w);
                        const bool black_two_side_ok =
                            two_side_visible && bal_b >= 0.24 && cont_lb >= 0.080 && cont_rb >= 0.080;
                        const bool white_two_side_ok =
                            two_side_visible && bal_w >= 0.20 && cont_lw >= 0.070 && cont_rw >= 0.070;
                        const bool black_shape_ok = black_two_side_ok ||
                                                    (edge_mode && best_side_b >= 0.16 && apex_black >= 0.45);
                        const bool white_shape_ok = white_two_side_ok ||
                                                    (edge_mode && best_side_w >= 0.15 && apex_white >= 0.50);
                        const double shallow_bonus = 0.16 * std::exp(-t0_ns / 4.0);
                        const double score_black = depth_weight[r0] * (0.30 * s_black[r0][x0] + 0.52 * hyp_b +
                            0.80 * cont_b + 0.72 * center_b + 0.20 * apex_black + shallow_bonus) -
                            0.10 * p_texture - top_penalty;
                        const double score_white = depth_weight[r0] * (0.30 * s_white[r0][x0] + 0.52 * hyp_w +
                            0.80 * cont_w + 0.72 * center_w + 0.20 * apex_white + shallow_bonus) -
                            0.10 * p_texture - 0.65 * top_penalty;
                        const bool black_pass = black_shape_ok && score_black >= t_confirm;
                        const bool white_pass = white_shape_ok && score_white >= t_confirm;
                        double score = -std::numeric_limits<double>::infinity();
                        int mode = 0;
                        if (black_pass && white_pass) {
                            score = std::max(score_black, score_white) + 0.35 * std::min(score_black, score_white);
                            mode = 2;
                        } else if (black_pass) {
                            score = score_black;
                            mode = 1;
                        } else {
                            if (white_pass) {
                                ++result.num_white_only_rejected;
                            }
                            continue;
                        }
                        if (score > best_pack.score) {
                            best_pack = {r0, x0, radius, r_left, r_right, rise,
                                std::max(hyp_b, hyp_w), mode, edge_mode, mean(on_abs),
                                score, black_pass, white_pass};
                        }
                    }
                }
            }
            if (best_pack.score > -std::numeric_limits<double>::infinity()) {
                seed_verified = true;
                ++result.num_verified;
                if (best_pack.black_pass && best_pack.white_pass) {
                    ++result.num_black_white;
                } else {
                    ++result.num_black;
                }
                const int x_pad = std::max(8, static_cast<int>(std::round(0.08 * best_pack.radius)));
                const double x1 = std::max(0.0, static_cast<double>(best_pack.col) - best_pack.radius - x_pad);
                const double x2 = std::min(static_cast<double>(nt - 1), static_cast<double>(best_pack.col) + best_pack.radius + x_pad);
                const double t0_ns = best_pack.row * dt_ns;
                const double y1 = std::max(0.0, t0_ns - 0.75);
                double y2 = std::min(t_max, t0_ns + std::max(1.70, std::min(2.80, 2.10 + 0.0012 * best_pack.radius)));
                if (y2 <= y1 + dt_ns) {
                    y2 = std::min(t_max, y1 + std::max(dt_ns, 0.5));
                }
                candidates.append({
                    x1,
                    y1,
                    x2 - x1 + 1.0,
                    y2 - y1,
                    best_pack.score,
                    best_pack.hyp,
                    t0_ns,
                    static_cast<double>(best_pack.radius),
                    best_pack.mode,
                    best_pack.edge
                });
            }
        }
        if (!seed_verified) {
            ++result.num_rejected_seed;
        }
    }

    if (candidates.isEmpty()) {
        return result;
    }
    candidates = prune_candidates(candidates);
    if (candidates.isEmpty()) {
        return result;
    }
    candidates = merge_boxes(candidates, std::max(14, static_cast<int>(std::round(0.10 * radii.last()))),
                             std::max(2.0 * dt_ns, 0.40));
    result.boxes = candidates;
    result.score_max = 0.0;
    for (const auto& box : result.boxes) {
        result.score_max = std::max(result.score_max, box.score);
    }
    return result;
}

QVector<QVector<double>> apply_v2_hyperbola_gain(
    const QVector<QVector<double>>& traces,
    const QVector<V2HyperbolaBox>& boxes,
    double dt_ns,
    double base_gain,
    double slope_per_ns,
    int left_pad_tr,
    int right_pad_tr,
    int up_pad_pts,
    int down_pad_pts)
{
    QVector<QVector<double>> out = traces;
    if (out.isEmpty() || out[0].isEmpty() || boxes.isEmpty() || dt_ns <= 0.0) {
        return out;
    }
    const int nt = out.size();
    const int ns = out[0].size();
    if (boxes.size() == 1) {
        const auto& box = boxes[0];
        const int x1 = std::max(0, static_cast<int>(std::floor(box.x)) - std::max(0, left_pad_tr));
        const int x2 = std::min(nt - 1, static_cast<int>(std::ceil(box.x + box.width - 1.0)) + std::max(0, right_pad_tr));
        const int y1 = std::max(0, static_cast<int>(std::floor(box.y_ns / dt_ns)) - std::max(0, up_pad_pts));
        const int y2 = std::min(ns - 1, static_cast<int>(std::ceil((box.y_ns + box.height_ns) / dt_ns)) + std::max(0, down_pad_pts));
        if (x1 > x2 || y1 > y2) {
            return out;
        }
        for (int x = x1; x <= x2; ++x) {
            for (int y = y1; y <= y2 && y < out[x].size(); ++y) {
                const double row_gain = std::max(0.0, base_gain + slope_per_ns * y * dt_ns);
                out[x][y] *= row_gain;
            }
        }
        return out;
    }
    QVector<QVector<double>> gain(nt, QVector<double>(ns, 1.0));
    for (const auto& box : boxes) {
        const int x1 = std::max(0, static_cast<int>(std::floor(box.x)) - std::max(0, left_pad_tr));
        const int x2 = std::min(nt - 1, static_cast<int>(std::ceil(box.x + box.width - 1.0)) + std::max(0, right_pad_tr));
        const int y1 = std::max(0, static_cast<int>(std::floor(box.y_ns / dt_ns)) - std::max(0, up_pad_pts));
        const int y2 = std::min(ns - 1, static_cast<int>(std::ceil((box.y_ns + box.height_ns) / dt_ns)) + std::max(0, down_pad_pts));
        if (x1 > x2 || y1 > y2) {
            continue;
        }
        for (int x = x1; x <= x2; ++x) {
            for (int y = y1; y <= y2; ++y) {
                const double row_gain = std::max(0.0, base_gain + slope_per_ns * y * dt_ns);
                gain[x][y] = std::max(gain[x][y], row_gain);
            }
        }
    }
    for (int x = 0; x < nt; ++x) {
        for (int y = 0; y < ns && y < out[x].size(); ++y) {
            out[x][y] *= gain[x][y];
        }
    }
    return out;
}

QVector<QVector<double>> apply_v2_hyperbola_detector(
    const QVector<QVector<double>>& traces,
    double dt_ns,
    const V2HyperbolaParams& params,
    V2HyperbolaResult* result)
{
    V2HyperbolaResult local_result = detect_v2_hyperbola(traces, dt_ns, params);
    if (result) {
        *result = local_result;
    }
    return apply_v2_hyperbola_gain(traces, local_result.boxes, dt_ns, 5.0, 2.0, 0, 0, 0, 0);
}
