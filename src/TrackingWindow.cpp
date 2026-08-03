#include "TrackingWindow.hpp"

#include <FL/Fl.H>
#include <FL/fl_draw.H>

TrackingWindow::TrackingWindow(int w, int h, const char* title, Fl_Window* parent)
    : BaseWindow(w, h, title, parent) {}

bool TrackingWindow::needs_redraw() const {
   return false;
}

void TrackingWindow::exit_window() {
    logic.set_state(PAUSED);
    Fl::remove_timeout(update_cb, this);
    BaseWindow::exit_window();
}

void TrackingWindow::start_tracking() {
    logic.start();
    logic.set_state(RUNNING);
    Fl::remove_timeout(update_cb, this);
    Fl::add_timeout(UPDATE_INTERVAL, update_cb, this);
    redraw();
}

void TrackingWindow::update_cb(void* w) {
    auto* win = static_cast<TrackingWindow*>(w);
    win->logic.update(UPDATE_INTERVAL);
    win->redraw();
    if (win->logic.is_finished()) {
        win->logic.set_state(FINISH);
        win->redraw();
        return;
    }
    Fl::repeat_timeout(UPDATE_INTERVAL, update_cb, w);
}

void TrackingWindow::draw() {
    Fl_Double_Window::draw();
    switch(logic.get_state()) {
        case START_SCREEN:
            draw_centered_text("Tracking: Press Enter to start");
            break;
        case RUNNING:
            break;
        case PAUSED:
            draw_centered_text("Restart Tracking: Press Enter to start over.");
            break;
        case FAILED:
            break;
        case FINISH:
            draw_centered_text("Tracking finished. Press Enter to exit.");
            break;
    }

    if (logic.get_state() == RUNNING) {
        auto pos = logic.get_current_position();
        int marker_radius = 5;
        fl_color(FL_BLUE);
        fl_pie(pos.x - marker_radius, pos.y - marker_radius, marker_radius * 2, marker_radius * 2, 0, 360);
    }
}

int TrackingWindow::handle(int event) {
    switch (event) {
        case FL_KEYDOWN:
            // Escape
            if (Fl::event_key() == FL_Escape) {
                exit_window();
                return 1;
            }
            // Enter
            if (Fl::event_key() == FL_Enter) {
                switch(logic.get_state()) {
                    case START_SCREEN:
                        start_tracking();
                        break;
                    case PAUSED:
                        logic.reset();
                        start_tracking();
                        break;
                    case FINISH:
                        exit_window();
                        break;
                    default:
                        break;
                }
                return 1;
            }
            break;
        case FL_CLOSE:
            exit_window();
            return 1;
    }
    return Fl_Double_Window::handle(event);
}
