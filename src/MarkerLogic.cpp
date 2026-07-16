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
           current_marker < 20;
}

// Check if maximum index for array has been reached [20]
bool MarkerLogic::max_markers_reached() const {
    return current_marker >= 19;
}

// Currentmarker index is increased by one
void MarkerLogic::show_next_marker() {
    if(current_marker < 19) {
        ++current_marker;
    } else {
        current_marker = -1;
    }

}

// Get current marker from array. Marker contains x and y position.
Marker const& MarkerLogic::get_current_marker() const {
    return markers[current_marker];
}

// Get marker from array at index
Marker const& MarkerLogic::get_marker(int index) const {
    return markers[index];
}

// Get marker from array at index
const Marker* MarkerLogic::get_all_markers() const {
    return markers;
}

// Get currentmarker index
int MarkerLogic::get_current_marker_index() const {
    return current_marker;
}

// Get reaction time as long in miliseconds
long MarkerLogic::get_reaction() const {
    return markers[current_marker].reaction_in_ms;
};

// Set reaction time in miliseconds.
void MarkerLogic::set_reaction(long time) {
    markers[current_marker].reaction_in_ms = time;
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
    current_marker = -1;
    current_state = START_SCREEN;
}