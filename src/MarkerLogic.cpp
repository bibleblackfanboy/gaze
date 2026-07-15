#include "MarkerLogic.hpp"

// Generate 20 markers for array within width and height
void MarkerLogic::generateMarkers(int width, int height) {
    for (int i = 0; i < 20; i++) {
        markers[i] = {
            rand() % width,
            rand() % height
        };
    }
}

// Check if currentmarker is active
bool MarkerLogic::hasCurrentMarker() const {
    return currentMarker >= 0 &&
           currentMarker < nextMarker;
}

// Check if maximum index for array has been reached [20]
bool MarkerLogic::maxMarkersReached() const {
    return nextMarker >= 20;
}

// Currentmarker index is set to nextmarker index and nextmarker++
void MarkerLogic::showNextMarker() {
    currentMarker = nextMarker;
    nextMarker++;
}

// Get current marker from array. Marker contains x and y position.
Marker const& MarkerLogic::getCurrentMarker() const {
    return markers[currentMarker];
}

// Get marker from array at index
Marker const& MarkerLogic::getMarker(int index) const {
    return markers[index];
}

// Get currentmarker index
int MarkerLogic::getCurrentMarkerIndex() const {
    return currentMarker;
}

// Get nextmarker index
int MarkerLogic::getNextMarkerIndex() const {
    return nextMarker;
}

// Get program state. Can be START_SCREEN, RUNNING, PAUSED or FAILED
State MarkerLogic::getState() const {
    return currentState;
}

// Set program state. Can be START_SCREEN, RUNNING, PAUSED or FAILED
void MarkerLogic::setState(State state) {
    currentState = state;
}

// Reset program. Affects marker
void MarkerLogic::reset() {
    nextMarker = 0;
    currentMarker = -1;
    currentState = START_SCREEN;
}