#pragma once

#include <vector>

#include "state.hpp"

struct TrackingPoint {
    int x;
    int y;
};

class TrackingLogic {
public:
    static constexpr int WAYPOINT_COUNT = 5;

    void generate_path(int width, int height);
    void reset();
    void start();
    void update(double delta_seconds);
    TrackingPoint get_current_position() const;
    bool is_finished() const;

    void setspeed(double pixels_per_second);
    void set_smoothness(double smoothness);
    double get_speed() const;
    double get_smoothness() const;

    State get_state() const;
    void set_state(State state);

private:
    std::vector<TrackingPoint> path;
    int current_waypoint_index = 0;
    double path_progress = 0.0;
    bool finished = false;

    double speed = 100.0; // pixel per second
    double smoothness = 0.5; // [0,1] 0 = linear, 1 = smooth
    State state = START_SCREEN;

    double ease(double t) const;
    double path_length(int waypoint_index) const;
};