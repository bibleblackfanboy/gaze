#include <cstdlib>
#include <chrono>

#include "logger.hpp"
#include "MarkerLogic.hpp"
#include "MarkerTiming.hpp"
#include "MarkerWindow.hpp"

#include <FL/Fl.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_draw.H>

Fl_Text_Display* outputBuffer = nullptr;
Logger* logger = nullptr;

Fl_Window* mainWin = nullptr;
MarkerWindow* markerWin = nullptr;

void marker_close_cb(Fl_Widget* w, void*) {
    auto* mw = static_cast<MarkerWindow*>(w);
    mw->exit_window();
}

void screen_setup(MarkerWindow* markerWin) {
    markerWin->show();
    markerWin->fullscreen();
    markerWin->resize(0, 0, Fl::w(), Fl::h());
    markerWin->take_focus();
}

void start_button_cb(Fl_Widget*, void* w) {
    mainWin = static_cast<Fl_Window*>(w);
    if(!markerWin) {
        markerWin = new MarkerWindow(0, 0, " ", mainWin);
        markerWin->callback(marker_close_cb);
        screen_setup(markerWin);
        markerWin->logic.generate_markers(markerWin->w(), markerWin->h());
        mainWin->hide();
    } else {
        if(markerWin->logic.max_markers_reached()) {
            return;
        }
        screen_setup(markerWin);
        markerWin->resume();
        mainWin->hide();
    }
}

void reset_button_cb(Fl_Widget* w, void*) {
    mainWin = static_cast<Fl_Window*>(w);
    logger->reset();
    if(markerWin) {
        markerWin->logic.reset();
    }
}

void save_button_cb(Fl_Widget* w, void*){
    mainWin = static_cast<Fl_Window*>(w);
    if(markerWin && markerWin->logic.max_markers_reached()) {
        logger->save(markerWin->logic.get_all_markers(), MARKER_AMOUNT);
        logger->log("Positions have been saved");
    } else {
        logger->log("Cannot save yet. Test must be finished");
    }
}

int main(int argc, char **argv) {

    Fl_Window win(100, 50, 800, 600, " ");

    Fl_Menu_Bar menu(0, 0, 800, 25);
    menu.add("Reset", FL_CTRL + "r", reset_button_cb);
    menu.add("Calibration");

    int button_width = 100;
    int button_height = 40;
    Fl_Button start_button((win.w() - button_width) / 4, 40, button_width, button_height, "Start");
    Fl_Button reset_button((win.w() - button_width) * 3 / 4, 40, button_width, button_height, "Save");

    Fl_Text_Buffer* outputBuffer = new Fl_Text_Buffer;
    int outputBoxWidth = 400;
    int outputBoxHeight = 450;
    Fl_Text_Display* outputBox = new Fl_Text_Display((win.w() - outputBoxWidth) / 2, 100, outputBoxWidth, outputBoxHeight);
    outputBox->buffer(outputBuffer);
    logger = new Logger(outputBox, outputBuffer);

    start_button.callback(start_button_cb, &win);
    reset_button.callback(save_button_cb, &win);

    win.end();
    win.show(argc, argv);
    return Fl::run();
}