#pragma once

#include "BaseWindow.hpp"
#include "CalibrationLogic.hpp"

class CalibrationWindow : public BaseWindow {
public:
    CalibrationWindow(int w, int h, const char* title, Fl_Window* parent);

    CalibrationLogic logic;

    void exit_window() override;
    void show_point();

protected:
    void draw() override;
    int handle(int event) override;
    bool needs_redraw() const override;

private:
    void draw_grid();
};