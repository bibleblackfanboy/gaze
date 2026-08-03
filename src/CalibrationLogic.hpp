#pragma once

#include <vector>

#include "State.hpp"

struct CalibrationPoint {
    int x;
    int y;
};

class CalibrationLogic {
public:
    static constexpr int COLUMNS = 4;
    static constexpr int ROWS = 3;
    static constexpr int POINT_COUNT = COLUMNS * ROWS;

    void generate_points(int width, int height);
    void reset();

    bool has_current_point() const;
    const CalibrationPoint& get_current_point() const;
    int get_current_point_index() const;

    const CalibrationPoint& get_point(int index) const;
    const std::vector<CalibrationPoint>& get_all_points() const;

    void show_next_point();
    bool max_points_reached() const;

    State get_state() const;
    void set_state(State state);

private:
    std::vector<CalibrationPoint> points;
    int current_marker = -1;
    State current_state = START_SCREEN;
};