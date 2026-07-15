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
        int next_marker = 0;
        int current_marker = -1;
        State current_state = START_SCREEN;

    public:
        MarkerLogic() = default;

        void generate_markers(int width, int height);
        bool has_current_marker() const;
        bool max_markers_reached() const;
        void show_next_marker();

        Marker const& get_current_marker() const;
        Marker const& get_marker(int index) const;
        int get_current_marker_index() const;
        int get_next_marker_index() const;
        State get_state() const;
        void set_state(State state);

        void reset();
};