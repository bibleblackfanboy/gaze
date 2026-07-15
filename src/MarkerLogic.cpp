#include "MarkerLogic.hpp"

// Generate 20 markers for array within width and height
void MarkerLogic::generate_markers(int width, int height) {
    for (int i = 0; i < 20; i++) {
        markers[i] = {
            rand() % width,
            rand() % height
        };
    }
}

// Check if currentmarker is active
bool MarkerLogic::has_current_marker() const {
    return current_marker >= 0 &&
           current_marker < next_marker;
}

// Check if maximum index for array has been reached [20]
bool MarkerLogic::max_markers_reached() const {
    return next_marker >= 20;
}

// Currentmarker index is set to nextmarker index and nextmarker++
void MarkerLogic::show_next_marker() {
    current_marker = next_marker;
    next_marker++;
}

// Get current marker from array. Marker contains x and y position.
Marker const& MarkerLogic::get_current_marker() const {
    return markers[current_marker];
}

// Get marker from array at index
Marker const& MarkerLogic::get_marker(int index) const {
    return markers[index];
}

// Get currentmarker index
int MarkerLogic::get_current_marker_index() const {
    return current_marker;
}

// Get nextmarker index
int MarkerLogic::get_next_marker_index() const {
    return next_marker;
}

// Get program state. Can be START_SCREEN, RUNNING, PAUSED or FAILED
State MarkerLogic::get_state() const {
    return current_state;
}

// Set program state. Can be START_SCREEN, RUNNING, PAUSED or FAILED
void MarkerLogic::set_state(State state) {
    current_state = state;
}

// Reset program. Affects indexes and state. For new markers use generate_markers()
void MarkerLogic::reset() {
    next_marker = 0;
    current_marker = -1;
    current_state = START_SCREEN;
}