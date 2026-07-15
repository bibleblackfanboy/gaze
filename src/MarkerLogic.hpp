#pragma once

#include <cstdlib>
#include <iostream>

enum State {
    START_SCREEN,
    RUNNING,
    PAUSED,
    FAILED
};

struct Marker {
    int x;
    int y;
};

class MarkerLogic {
    private:
        Marker markers[20] {};
        int nextMarker = 0;
        int currentMarker = -1;
        State currentState = START_SCREEN;

    public:
        MarkerLogic() = default;

        void generateMarkers(int width, int height);
        bool hasCurrentMarker() const;
        bool maxMarkersReached() const;
        void showNextMarker();


        Marker const& getCurrentMarker() const;
        Marker const& getMarker(int index) const;
        int getCurrentMarkerIndex() const;
        int getNextMarkerIndex() const;
        State getState() const;
        void setState(State state);

        void reset();
};