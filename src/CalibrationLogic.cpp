#include "CalibrationLogic.hpp"

void CalibrationLogic::generate_points(int width, int height) {
    points.clear();
    points.reserve(POINT_COUNT);
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLUMNS; ++col) {
            CalibrationPoint point;
            point.x = (col + 0.5) * width / COLUMNS;
            point.y = (row + 0.5) * height / ROWS;
            points.push_back(point);
        }
    }
    current_marker = -1;
    current_state = START_SCREEN;
}

void CalibrationLogic::reset() {
    current_marker = -1;
    current_state = START_SCREEN;
}

bool CalibrationLogic::has_current_point() const {
    return current_marker >= 0 && current_marker < POINT_COUNT;
}

const CalibrationPoint& CalibrationLogic::get_current_point() const {
    return points[current_marker];
}

int CalibrationLogic::get_current_point_index() const {
    return current_marker;
}

const CalibrationPoint& CalibrationLogic::get_point(int index) const {
    return points[index];
}

const std::vector<CalibrationPoint>& CalibrationLogic::get_all_points() const {
    return points;
}

void CalibrationLogic::show_next_point() {
    if (current_marker + 1 < POINT_COUNT) {
        ++current_marker;
    }
}

bool CalibrationLogic::max_points_reached() const {
    return current_marker + 1>= POINT_COUNT;
}

State CalibrationLogic::get_state() const {
    return current_state;
}

void CalibrationLogic::set_state(State state) {
    current_state = state;
}