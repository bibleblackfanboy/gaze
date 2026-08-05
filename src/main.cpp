#include <cstdlib>
#include <chrono>

#include "logger.hpp"

#include "MarkerWindow.hpp"
#include "CalibrationWindow.hpp"
#include "TrackingWindow.hpp"

#include <FL/Fl.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Double_Window.H>

Fl_Text_Display* outputBuffer = nullptr;
Logger* logger = nullptr;

Fl_Window* mainWin = nullptr;
MarkerWindow* markerWin = nullptr;
CalibrationWindow* calibrationWin = nullptr;
TrackingWindow* trackingWin = nullptr;

void marker_close_cb(Fl_Widget* w, void*) {
    auto* mw = static_cast<MarkerWindow*>(w);
    mw->exit_window();
}

void calibration_close_cb(Fl_Widget* w, void*) {
    auto* cw = static_cast<CalibrationWindow*>(w);
    cw->exit_window();
}

void tracking_close_cb(Fl_Widget* w, void*) {
    auto* tw = static_cast<TrackingWindow*>(w);
    tw->exit_window();
}

void screen_setup(Fl_Double_Window* win) {
    win->show();
    int X, Y, W, H;
    Fl::screen_xywh(X, Y, W, H);
    win->resize(X, Y, W, H);
    win->take_focus();
}

void test_button_cb(Fl_Widget*, void* w) {
    mainWin = static_cast<Fl_Window*>(w);
    if(!markerWin) {
        markerWin = new MarkerWindow(0, 0, " ", mainWin);
        markerWin->callback(marker_close_cb);
        screen_setup(markerWin);
        markerWin->logic.generate_markers(markerWin->w(), markerWin->h());
        mainWin->hide();
    } else {
        screen_setup(markerWin);
        if(markerWin->logic.max_markers_reached()) {
            markerWin->logic.generate_markers(markerWin->w(), markerWin->h());
            markerWin->logic.set_state(START_SCREEN);
        } else {
        markerWin->resume();
        }
        mainWin->hide();
    }
}

void alternative_button_cb(Fl_Widget*, void* w) {
    mainWin = static_cast<Fl_Window*>(w);
    if(!trackingWin) {
        trackingWin = new TrackingWindow(0, 0, " ", mainWin);
        trackingWin->callback(tracking_close_cb);
        trackingWin->logic.setspeed(80.0);
        trackingWin->logic.set_smoothness(0.6);
        screen_setup(trackingWin);
        trackingWin->color(fl_rgb_color(238, 238, 238));
        trackingWin->logic.generate_path(trackingWin->w(), trackingWin->h());
        mainWin->hide();
    } else {
        trackingWin->logic.generate_path(trackingWin->w(), trackingWin->h());
        screen_setup(trackingWin);
        mainWin->hide();
    }
}

void calibration_button_cb(Fl_Widget*, void* w) {
    mainWin = static_cast<Fl_Window*>(w);
    if(!calibrationWin) {
        calibrationWin = new CalibrationWindow(0, 0, " ", mainWin);
        calibrationWin->callback(calibration_close_cb);
        screen_setup(calibrationWin);
        calibrationWin->logic.generate_points(calibrationWin->w(), calibrationWin->h());
        mainWin->hide();
    } else {
        if(calibrationWin->logic.max_points_reached()) {
            return;
        }
        screen_setup(calibrationWin);
        mainWin->hide();
    }
}

void reset_button_cb(Fl_Widget* w, void*) {
    mainWin = static_cast<Fl_Window*>(w);
    logger->reset();
    if(markerWin) {
        markerWin->logic.reset();
    }
    if (calibrationWin) {
        calibrationWin->logic.reset();
    }
    if (trackingWin) {
        trackingWin->logic.reset();
    }
}

void save_button_cb(Fl_Widget* w, void*) {
    mainWin = static_cast<Fl_Window*>(w);
    if(markerWin && markerWin->logic.max_markers_reached()) {
        logger->save(markerWin->logic.get_all_markers(), MARKER_AMOUNT);
        logger->log("Positions have been saved");
    } else {
        logger->log("Cannot save yet. Test must be finished");
    }
}

void fullscreen_cb(Fl_Widget* w, void*) {
    Fl_Window* mainWin = w->window();
    if(mainWin->fullscreen_active()) {
        mainWin->fullscreen_off();
    } else {
        mainWin->fullscreen();
    }
}

int main(int argc, char **argv) {
    Fl_Window win(100, 50, 800, 600, " ");

    Fl_Menu_Bar menu(0, 0, 800, 25);
    menu.add("Reset", FL_CTRL + "r", reset_button_cb);
    menu.add("Save", FL_CTRL + "s", save_button_cb);
    menu.add("Fullscreen", 0, fullscreen_cb);

    int button_width = 100;
    int button_height = 40;
    Fl_Button static_test((win.w() - button_width) / 4, 40, button_width, button_height, "Static Test");
    Fl_Button dynamic_test((win.w() - button_width) * 2 / 4, 40, button_width, button_height, "Dynamic Test");
    Fl_Button calibration((win.w() - button_width) * 3 / 4, 40, button_width, button_height, "Calibration");
    static_test.callback(test_button_cb, &win);
    dynamic_test.callback(alternative_button_cb, &win);
    calibration.callback(calibration_button_cb, &win);

    Fl_Text_Buffer* outputBuffer = new Fl_Text_Buffer;
    int outputBoxWidth = 600;
    int outputBoxHeight = 450;
    Fl_Text_Display* outputBox = new Fl_Text_Display((win.w() - outputBoxWidth) / 2, 100, outputBoxWidth, outputBoxHeight);
    outputBox->buffer(outputBuffer);
    logger = new Logger(outputBox, outputBuffer);



    win.end();
    win.show(argc, argv);
    return Fl::run();
}