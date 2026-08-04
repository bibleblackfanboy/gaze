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
        win->logic.set_state(SHOW_FINISH);
        win->redraw();
        return;
    }
    Fl::repeat_timeout(UPDATE_INTERVAL, update_cb, w);
}

void TrackingWindow::draw() {
    Fl_Double_Window::draw();
    switch(logic.get_state()) {
        case START_SCREEN:
            draw_centered_text("Tracking: Follow the blue dot");
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

    auto pos = logic.get_current_position();
    // log current position?

    if(logic.get_state() == SHOW_START) {
        // lines
        fl_color(FL_DARK3);
        fl_line_style(FL_SOLID, 1);
        fl_line(0, pos.y, w(), pos.y);  // horizontal
        fl_line(pos.x, 0, pos.x, h());  // vertical
        // dot
        fl_color(FL_BLUE);
        fl_pie(pos.x - 5, pos.y - 5, 10, 10, 0, 360);

    }

    if(logic.get_state() == RUNNING) {
        // dot
        int marker_radius = 5;
        fl_color(FL_BLUE);
        fl_pie(pos.x - marker_radius, pos.y - marker_radius, marker_radius * 2, marker_radius * 2, 0, 360);

    }

    if(logic.get_state() == SHOW_FINISH) {
        // lines
        fl_color(FL_DARK3);
        fl_line_style(FL_SOLID, 1);
        fl_line(0, pos.y, w(), pos.y);  // horizontal
        fl_line(pos.x, 0, pos.x, h());  // vertical
        // dot
        fl_color(FL_BLUE);
        fl_pie(pos.x - 5, pos.y - 5, 10, 10, 0, 360);
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
            // Space
            if (Fl::event_key() == ' ') {
                switch (logic.get_state()) {
                    case SHOW_START:
                        start_tracking();
                        break;
                    case SHOW_FINISH:
                        logic.set_state(FINISH);
                        redraw();
                        break;
                    default:
                        break;
                }
            }
            // Enter
            if (Fl::event_key() == FL_Enter) {
                switch(logic.get_state()) {
                    case START_SCREEN:
                        logic.reset();
                        logic.set_state(SHOW_START);
                        redraw();
                        break;
                    case RUNNING:
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
