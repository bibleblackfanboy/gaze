#pragma once

#include "BaseWindow.hpp"
#include "MarkerLogic.hpp"
#include "MarkerTiming.hpp"

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Window.H>

class MarkerWindow : public BaseWindow {
public:
    MarkerWindow(int w, int h, const char* title, Fl_Window* parent);

    MarkerLogic logic;
    MarkerTiming timer;

    void exit_window() override;
    void show_marker();
    void resume();

    void print_marker_log();

protected:
    void draw() override;
    int handle(int event) override;
    bool needs_redraw() const override;

private:
    static void marker_timeout_cb(void* w);

};