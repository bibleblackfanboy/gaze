#include "CalibrationWindow.hpp"

#include <FL/Fl.H>
#include <FL/fl_draw.H>

CalibrationWindow::CalibrationWindow(int w, int h, const char* title, Fl_Window* parent)
    : BaseWindow(w, h, title, parent) {}

bool CalibrationWindow::needs_redraw() const {
    return logic.get_state() == RUNNING && logic.has_current_point();
}

void CalibrationWindow::exit_window() {
    logic.set_state(PAUSED);
    BaseWindow::exit_window();
}

void CalibrationWindow::show_point() {
    if (!logic.has_current_point()) {
        logic.set_state(FINISH);
        exit_window();
        return;
    }
    logic.set_state(RUNNING);
    redraw();
}

void CalibrationWindow::draw() {
    Fl_Double_Window::draw();
    switch(logic.get_state()) {
        case START_SCREEN:
            draw_centered_text("Calibration: Press Enter to start");
            break;
        case RUNNING:
            break;
        case PAUSED:
            draw_centered_text("Calibration was interrupted. Press Enter to reset.");
            break;
        case FAILED:
            break;
        case FINISH:
            draw_centered_text("Calibration finished. Press Enter to exit.");
            break;
    }

    if (logic.get_state() == RUNNING) {
        draw_grid();
    }

    if(logic.get_state() == RUNNING && logic.has_current_point()) {
        const auto& point = logic.get_current_point();
        int marker_radius = 5;
        fl_color(FL_RED);
        fl_pie(point.x - marker_radius, point.y - marker_radius, marker_radius * 2, marker_radius * 2, 0, 360);
    }
}

void CalibrationWindow::draw_grid() {
    const auto& points = logic.get_all_points();
    if(points.empty()) {
        return;
    }

    fl_color(FL_DARK3);
    fl_line_style(FL_SOLID, 1);
    // horizontal
    for(int row = 0; row < CalibrationLogic::ROWS; ++row) {
        int y = points[row * CalibrationLogic::COLUMNS].y;
        fl_line(0, y, w(), y);
    }
    // vertical
    for(int col = 0; col < CalibrationLogic::COLUMNS; ++col) {
        int x = points[col].x;
        fl_line(x, 0, x, h());
    }
}

int CalibrationWindow::handle(int event) {
    switch (event) {
        case FL_KEYDOWN:
            // Escape
            if (Fl::event_key() == FL_Escape) {
                do_callback();
                return 1;
            }
            // Space
            if (Fl::event_key() == ' ') {
                if(logic.get_state() == RUNNING) {
                    if(logic.max_points_reached()) {
                        logic.set_state(FINISH);
                        redraw();
                    } else {
                        logic.show_next_point();
                        show_point();
                    }
                }
                return 1;
            }
            // Enter
            if (Fl::event_key() == FL_Enter) {
                switch(logic.get_state()) {
                    case START_SCREEN:
                        logic.show_next_point();
                        show_point();
                        break;
                    case PAUSED:
                        logic.reset();
                        logic.show_next_point();
                        show_point();
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