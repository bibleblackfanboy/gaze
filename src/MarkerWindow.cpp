#include <cstdio>

#include "logger.hpp"
#include "MarkerWindow.hpp"

#include <FL/Fl.H>
#include <FL/fl_draw.H>


extern Logger* logger;

MarkerWindow::MarkerWindow(int w, int h, const char* title, Fl_Window* parent)
    : BaseWindow(w, h, title, parent) {}

void MarkerWindow::marker_timeout_cb(void* w) {
    auto* win = static_cast<MarkerWindow*>(w);
    win->logic.set_state(FAILED);
    win->redraw();
}

bool MarkerWindow::needs_redraw() const {
    return logic.has_current_marker();
}

void MarkerWindow::exit_window() {
    logic.set_state(PAUSED);
    Fl::remove_timeout(marker_timeout_cb, this);
    fullscreen_off();
    if (parentWindow)
        parentWindow->show();
    hide();
    print_marker_log();
}

void MarkerWindow::show_marker() {
    if (!logic.has_current_marker()) {
        logger->log("Maximum number of markers reached.");
        exit_window();
        return;
    }

    logic.set_state(RUNNING);
    Fl::remove_timeout(marker_timeout_cb, this);
    timer.restart();
    redraw();
    Fl::add_timeout(timer.ttl() / 1000.0, marker_timeout_cb, this);
}

void MarkerWindow::print_marker_log() {
    logger->reset();
    char buffer[128];
    for (int i = 0; i < logic.get_current_marker_index() + 1; i++) {
        const Marker& marker = logic.get_marker(i);
        snprintf(buffer, sizeof(buffer), "%02d | X = %04d | Y = %04d | Time = %04ld",
                 i + 1, marker.x, marker.y, marker.reaction_in_ms);
        logger->log(buffer);
    }
}

void MarkerWindow::draw() {
    Fl_Double_Window::draw();
    switch (logic.get_state()) {
        case START_SCREEN:
            draw_centered_text("Press ENTER to start. Use SPACE to claim markers.");
            break;
        case PAUSED:
            draw_centered_text("Press ENTER to resume.");
            break;
        case FAILED:
            draw_centered_text("Missed timing, try again!");
            break;
        case FINISH:
            draw_centered_text("Finished! Press ENTER to exit.");
        case RUNNING:
            break;
    }

    if (logic.has_current_marker() && logic.get_state() == RUNNING) {
        const auto& marker = logic.get_current_marker();
        int marker_radius = 5;
        int ring_start_radius = 50 * marker_radius;
        double progress = timer.marker_progress();
        int claim_radius = static_cast<int>(marker_radius + ((ring_start_radius) * (1.0 - progress)));

        // Marker
        fl_line_style(0);
        if (progress == 1) {
            fl_color(FL_GREEN);
        } else {
            fl_color(FL_RED);
        }
        fl_pie(marker.x - marker_radius, marker.y - marker_radius, 2 * marker_radius, 2 * marker_radius, 0, 360);

        // Ring
        fl_line_style(FL_SOLID, 1);
        fl_color(FL_BLACK);
        fl_circle(marker.x, marker.y, claim_radius);
    }
}

int MarkerWindow::handle(int event) {
    switch (event) {
        case FL_KEYDOWN:
            // Escape
            if (Fl::event_key() == FL_Escape) {
                do_callback();
                return 1;
            }
            // Space
            if (Fl::event_key() == ' ') {
                switch (logic.get_state()) {
                    case RUNNING:
                        logic.set_reaction(timer.elapsed_time_ms() - timer.ttl() + timer.get_reaction_time());
                        if (timer.marker_progress() != 1) {
                            logic.set_state(FAILED);
                            Fl::remove_timeout(marker_timeout_cb, this);
                            break;
                        }
                        if (logic.max_markers_reached()) {
                            logic.set_state(FINISH);
                            Fl::remove_timeout(marker_timeout_cb, this);
                            break;
                        }
                        logic.show_next_marker();
                        show_marker();
                        break;

                    default:
                        break;
                }
                return 1;
            }
            // Enter
            if (Fl::event_key() == FL_Enter) {
                switch (logic.get_state()) {
                    case START_SCREEN:
                        logic.show_next_marker();
                        show_marker();
                        break;

                    case PAUSED:
                        if(logic.get_current_marker_index() >= 0) {
                            show_marker();
                        }else {
                            logic.show_next_marker();
                            show_marker();
                        }
                        break;

                    case FAILED:
                        show_marker();
                        timer.restart();
                        break;

                    case FINISH:
                        exit_window();

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

void MarkerWindow::resume() {
    if (logic.get_state() != PAUSED) {
        return;
    }
    timer.restart();
    Fl::add_timeout(timer.ttl() / 1000.0, marker_timeout_cb, this);
    redraw();
}