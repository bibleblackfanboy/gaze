#include <cstdlib>
#include <chrono>

#include "logger.hpp"
#include "MarkerLogic.hpp"

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
    std::chrono::steady_clock::time_point marker_start_time = std::chrono::steady_clock::now();

    MarkerLogic logic;
    int ttl_in_ms = 5000;

    static void marker_timeout_cb(void* w){
        auto* win = static_cast<MarkerWindow*>(w);
        win->logic.setState(FAILED);
        win->redraw();
    }
    static void refresh_cb(void* w) {
        auto* win = static_cast<MarkerWindow*>(w);
        if (win->logic.hasCurrentMarker())
            win->redraw();
        Fl::repeat_timeout(0.03, refresh_cb, w);
    }

    long elapsed_ms() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - marker_start_time).count();
    }

    public:
    MarkerWindow(int w, int h, const char * title, Fl_Window* parent): Fl_Double_Window(w, h, title), parentWindow(parent) { 
        Fl::add_timeout(0.03, refresh_cb, this);
    }

    void exit_marker_window() {
        logic.setState(PAUSED);
        Fl::remove_timeout(marker_timeout_cb, this);
        fullscreen_off();
        if (parentWindow)
            parentWindow->show();
        hide();
    }

    void show_marker() {
        if(!logic.hasCurrentMarker())
            return;
        const auto& marker = logic.getCurrentMarker();
        char buffer[100];
        sprintf(buffer, "Marker showing at (%d|%d)", marker.x, marker.y);
        logger->log(buffer);
        sprintf(buffer, "Reaction time: %ld ms", elapsed_ms() - ttl_in_ms);
        logger->log(buffer);
        
        if (logic.maxMarkersReached()) {
            logger->log("Maximum number of markers reached.");
            exit_marker_window();
            return;
        }
        logic.setState(RUNNING);
        Fl::remove_timeout(marker_timeout_cb, this);
        marker_start_time = std::chrono::steady_clock::now();
        redraw();
        Fl::add_timeout(ttl_in_ms / 1000.0, marker_timeout_cb, this);
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
        switch(logic.getState()) {
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

        if(logic.hasCurrentMarker() && logic.getState() == RUNNING) {
            const auto& marker = logic.getCurrentMarker();
            int marker_radius = 10;
            int ring_start_radius = 15 * marker_radius;
            double progress = std::min(1.0, (double)elapsed_ms() / (ttl_in_ms - 500));
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
                    switch(logic.getState()) {
                        case START_SCREEN:
                            logic.showNextMarker();
                            show_marker();
                            break;
                        case PAUSED:
                            show_marker();
                            break;
                        case FAILED:
                            show_marker();
                            break;
                        case RUNNING:
                            logic.showNextMarker();
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
        if (logic.getState() != PAUSED) {
            return;
        }
        marker_start_time = std::chrono::steady_clock::now();
        Fl::add_timeout(ttl_in_ms / 1000.0, marker_timeout_cb, this);
        redraw();
    }

    void generate_new_markers() {
        logic.generateMarkers(this->w(), this->h());
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