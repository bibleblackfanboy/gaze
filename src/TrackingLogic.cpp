#include "TrackingLogic.hpp"

#include <algorithm>
#include <cmath>
#include <random>

void TrackingLogic::generate_path(int width, int height) {
    path.clear();

    current_waypoint_index = 0;
    path_progress = 0.0;
    finished = false;
    state = START_SCREEN;

    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist_x(0, std::max(0, width - 1));
    std::uniform_int_distribution<int> dist_y(0, std::max(0, height - 1));

    auto random_point = [&]() -> TrackingPoint {
        return {dist_x(rng), dist_y(rng)};
    };

    TrackingPoint start_point = random_point();
    TrackingPoint end_point = random_point();

    path.reserve(WAYPOINT_COUNT + 2);
    path.push_back(start_point);
    for (int i = 0; i < WAYPOINT_COUNT; ++i) {
        path.push_back(random_point());
    }
    path.push_back(end_point);
}

void TrackingLogic::reset() {
    current_waypoint_index = 0;
    path_progress = 0.0;
    finished = false;
    state = START_SCREEN;
}

void TrackingLogic::start() {
    if (state == START_SCREEN) {
        state = RUNNING;
        current_waypoint_index = 0;
        path_progress = 0.0;
        finished = false;
    }
}

double TrackingLogic::path_length(int waypoint_index) const {
    const auto& a = path[waypoint_index];
    const auto& b = path[waypoint_index + 1];
    double dx = b.x - a.x;
    double dy = b.y - a.y;
    return std::sqrt(dx * dx + dy * dy);
}

double TrackingLogic::ease(double t) const {
    double linear = t;
    double smooth = t * t * (3 - 2 * t);
    return linear + smoothness * (smooth - linear);
}

void TrackingLogic::update(double delta_seconds) {
    if (finished || path.size() < 2 || state != RUNNING) {
        return;
    }

    int last_waypoint_index = static_cast<int>(path.size()) - 2;

    double distance = path_length(current_waypoint_index);
    double duration = distance > 0.0 ? distance / speed : 0.0;
    path_progress += (duration > 0.0 ? delta_seconds / duration : 1.0);

    while(path_progress >= 1.0) {
        if(current_waypoint_index < last_waypoint_index) {
            current_waypoint_index = last_waypoint_index;
            path_progress = 1.0;
            finished = true;
            break;
        }
        path_progress -= 1.0;
        ++current_waypoint_index;

        distance = path_length(current_waypoint_index);
        duration = distance > 0.0 ? distance / speed : 0.0;
        if (duration <= 0.0) {
            path_progress = 1.0;
        }
    }
}

TrackingPoint TrackingLogic::get_current_position() const {
    if (path.empty()) {
        return {0, 0};
    }
    if (path.size() < 2) {
        return path[0];
    }

    const auto& a = path[current_waypoint_index];
    const auto& b = path[current_waypoint_index + 1];

    double eased_progress = ease(path_progress);

    int x = a.x + static_cast<int>((b.x - a.x) * eased_progress);
    int y = a.y + static_cast<int>((b.y - a.y) * eased_progress);

    return {x, y};
}

bool TrackingLogic::is_finished() const {
    return finished;
}

void TrackingLogic::setspeed(double pixels_per_second) {
    speed = pixels_per_second > 0.0 ? pixels_per_second : 1.0;
}

void TrackingLogic::set_smoothness(double smoothness_value) {
    smoothness = std::clamp(smoothness_value, 0.0, 1.0);
}

double TrackingLogic::get_speed() const {
    return speed;
}

double TrackingLogic::get_smoothness() const {
    return smoothness;
}

State TrackingLogic::get_state() const {
    return state;
}

void TrackingLogic::set_state(State new_state) {
    state = new_state;
}