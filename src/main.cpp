#include <cstdlib>
#include <chrono>

#include "logger.hpp"
#include "MarkerLogic.hpp"
#include "MarkerTiming.hpp"

#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_draw.H>

Fl_Text_Display* outputBuffer = nullptr;
Logger* logger = nullptr;

class MarkerWindow : public Fl_Double_Window {
    Fl_Window* parentWindow;

    MarkerLogic logic;
    MarkerTiming timer;

    static void marker_timeout_cb(void* w){
        auto* win = static_cast<MarkerWindow*>(w);
        win->logic.set_state(FAILED);
        win->redraw();
    }
    static void refresh_cb(void* w) {
        auto* win = static_cast<MarkerWindow*>(w);
        if (win->logic.has_current_marker())
            win->redraw();
        Fl::repeat_timeout(0.03, refresh_cb, w);
    }

    public:
    MarkerWindow(int w, int h, const char * title, Fl_Window* parent): Fl_Double_Window(w, h, title), parentWindow(parent) { 
        Fl::add_timeout(0.03, refresh_cb, this);
    }

    void exit_marker_window() {
        logic.set_state(PAUSED);
        Fl::remove_timeout(marker_timeout_cb, this);
        fullscreen_off();
        if (parentWindow)
            parentWindow->show();
        hide();
    }

    void show_marker() {
        if(!logic.has_current_marker())
            return;
        const auto& marker = logic.get_current_marker();
        char buffer[100];
        sprintf(buffer, "Marker showing at (%d|%d)", marker.x, marker.y);
        logger->log(buffer);
        sprintf(buffer, "Reaction time: %ld ms", timer.elapsed_time_ms() - timer.ttl());
        logger->log(buffer);
        
        if (logic.max_markers_reached()) {
            logger->log("Maximum number of markers reached.");
            exit_marker_window();
            return;
        }
        logic.set_state(RUNNING);
        Fl::remove_timeout(marker_timeout_cb, this);
        timer.restart();
        redraw();
        Fl::add_timeout(timer.ttl() / 1000.0, marker_timeout_cb, this);
    }
    void draw_centered_text(const char* s) {
        fl_font(FL_HELVETICA_BOLD, 32);
        fl_color(FL_BLACK);
        const char* text = s;
        int tw = 0, th = 0;
        fl_measure(text, tw, th);
        fl_draw(text, (w() - tw) / 2, (h() + th) / 2);
    }
    void draw() override {
        Fl_Double_Window::draw();
        switch(logic.get_state()) {
            case START_SCREEN:
                draw_centered_text("Press SPACE to start");
                break;
            case PAUSED:
                draw_centered_text("Press SPACE to resume");
                break;
            case FAILED:
                draw_centered_text("Missed timing, try again!");
                break;
            case RUNNING:
                break;
        }

        if(logic.has_current_marker() && logic.get_state() == RUNNING) {
            const auto& marker = logic.get_current_marker();
            int marker_radius = 10;
            int ring_start_radius = 15 * marker_radius;
            double progress = timer.marker_progress();
            int radius = static_cast<int>(marker_radius + ((ring_start_radius) * (1.0 - progress)));

            // Marker
            fl_line_style(0);
            fl_color(FL_RED);
            fl_pie(marker.x - marker_radius, marker.y - marker_radius, 2 * marker_radius, 2 * marker_radius, 0, 360);

            // Ring
            fl_line_style(FL_SOLID, 2);
            fl_color(FL_BLACK);
            fl_circle(marker.x, marker.y, radius);
        }

        
    }

    int handle(int event) override {
        switch (event) {
            case FL_KEYDOWN:
                if (Fl::event_key() == FL_Escape) {
                    do_callback();
                    return 1;
                }
                if (Fl::event_key() == ' ') {
                    switch(logic.get_state()) {
                        case START_SCREEN:
                            logic.show_next_marker();
                            show_marker();
                            break;
                        case PAUSED:
                            show_marker();
                            break;
                        case FAILED:
                            show_marker();
                            break;
                        case RUNNING:
                            logic.show_next_marker();
                            show_marker();
                            break;
                    }
                    return 1;
                }
                break;

            case FL_CLOSE:
                exit_marker_window();
                return 1;
        }
        return Fl_Double_Window::handle(event);
    }

    void resume(){
        if (logic.get_state() != PAUSED) {
            return;
        }
        timer.restart();
        Fl::add_timeout(timer.ttl() / 1000.0, marker_timeout_cb, this);
        redraw();
    }

    void generate_new_markers() {
        logic.generate_markers(this->w(), this->h());
    }
};

Fl_Window* mainWin = nullptr;
MarkerWindow* markerWin = nullptr;

void marker_close_cb(Fl_Widget* w, void*) {
    auto* mw = static_cast<MarkerWindow*>(w);
    mw->exit_marker_window();
}

void start_button_cb(Fl_Widget* w, void*) {
    mainWin = static_cast<Fl_Window*>(w);
    if (!markerWin) {
        markerWin = new MarkerWindow(0, 0, " ", mainWin);
        markerWin->callback(marker_close_cb);
        markerWin->show();
        markerWin->fullscreen();
        markerWin->resize(0, 0, Fl::w(), Fl::h());
        markerWin->take_focus();
        markerWin->generate_new_markers();
    } else {
        markerWin->show();
        markerWin->fullscreen();
        markerWin->resize(0, 0, Fl::w(), Fl::h());
        markerWin->take_focus();
        markerWin->resume();
    }
    mainWin->hide();
}

int main(int argc, char **argv) {

    Fl_Window win(400, 400, " ");
    int button_width = 100;
    int button_height = 40;
    Fl_Button start_button((win.w() - button_width) / 4, 80, button_width, button_height, "Start");
    Fl_Button show_array_button((win.w() - button_width) * 3 / 4, 80, button_width, button_height, "Show Array");

    Fl_Text_Buffer* outputBuffer = new Fl_Text_Buffer;
    int outputBoxWidth = 300;
    int outputBoxHeight = 200;
    Fl_Text_Display* outputBox = new Fl_Text_Display((win.w() - outputBoxWidth) / 2, 150, outputBoxWidth, outputBoxHeight);
    outputBox->buffer(outputBuffer);
    logger = new Logger(outputBox, outputBuffer);

    start_button.callback(start_button_cb, &win);

    win.end();
    win.show(argc, argv);
    return Fl::run();
}