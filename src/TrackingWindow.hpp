#pragma once

#include "BaseWindow.hpp"
#include "TrackingLogic.hpp"

class TrackingWindow : public BaseWindow {
public:
    TrackingWindow(int w, int h, const char* title, Fl_Window* parent);

    TrackingLogic logic;

    void exit_window() override;
    void start_tracking();

protected:
    void draw() override;
    int handle(int event) override;
    bool needs_redraw() const override;

private:
    static void update_cb(void* w);
    static constexpr double UPDATE_INTERVAL = 0.016; // 60 FPS
};