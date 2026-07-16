#pragma once

#include <cstdlib>
#include <iostream>

#define MARKER_AMOUNT 5

enum State {
    START_SCREEN,
    RUNNING,
    PAUSED,
    FAILED,
    FINISH
};

struct Marker {
    int x;
    int y;
    long reaction_in_ms;
};

class MarkerLogic {
    private:
        Marker markers[MARKER_AMOUNT] {};
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
        const Marker* get_all_markers() const;
        int get_current_marker_index() const;
        long get_reaction() const;
        void set_reaction(long time);
        State get_state() const;
        void set_state(State state);

        void reset();
};