#include <cstdlib>
#include <chrono>

#include "logger.hpp"
#include "MarkerLogic.hpp"
#include "MarkerTiming.hpp"

#include <FL/Fl.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_draw.H>

Fl_Text_Display* outputBuffer = nullptr;
Logger* logger = nullptr;

void print_array();

class MarkerWindow : public Fl_Double_Window {
    Fl_Window* parentWindow;

    static void marker_timeout_cb(void* w){
        auto* win = static_cast<MarkerWindow*>(w);
        win->logic.set_state(FAILED);
        win->redraw();
    }
    static void refresh_cb(void* w) {
        auto* win = static_cast<MarkerWindow*>(w);
        if(win->logic.has_current_marker())
            win->redraw();
        Fl::repeat_timeout(0.016, refresh_cb, w);
    }

    public:
    MarkerLogic logic;
    MarkerTiming timer;

    MarkerWindow(int w, int h, const char * title, Fl_Window* parent): Fl_Double_Window(w, h, title), parentWindow(parent) { 
        Fl::add_timeout(0.016, refresh_cb, this);
    }

    void exit_marker_window() {
        logic.set_state(PAUSED);
        Fl::remove_timeout(marker_timeout_cb, this);
        fullscreen_off();
        if(parentWindow)
            parentWindow->show();
        hide();
        print_array();
        if(logic.max_markers_reached()) {
            logger->save(logic.get_all_markers());
            logger->log("Positions have been saved");
        }
    }

    void show_marker() {
        if(!logic.has_current_marker()) {
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

        if(logic.has_current_marker() && logic.get_state() == RUNNING) {
            const auto& marker = logic.get_current_marker();
            int marker_radius = 5;
            int ring_start_radius = 50 * marker_radius;
            double progress = timer.marker_progress();
            int radius = static_cast<int>(marker_radius + ((ring_start_radius) * (1.0 - progress)));

            // Marker
            fl_line_style(0);
            if(progress == 1){
                fl_color(FL_GREEN);
            } else {
                fl_color(FL_RED);
            }
            fl_pie(marker.x - marker_radius, marker.y - marker_radius, 2 * marker_radius, 2 * marker_radius, 0, 360);

            // Ring
            fl_line_style(FL_SOLID, 1);
            fl_color(FL_BLACK);
            fl_circle(marker.x, marker.y, radius);
        }

    }

    int handle(int event) override {
        switch (event) {
            case FL_KEYDOWN:
                // Escape
                if(Fl::event_key() == FL_Escape) {
                    do_callback();
                    return 1;
                }
                // Space
                if(Fl::event_key() == ' ') {
                    switch(logic.get_state()) {
                        case RUNNING:
                            logic.set_reaction(timer.elapsed_time_ms() - timer.ttl() + timer.get_reaction_time());
                            if(timer.marker_progress() != 1){
                                logic.set_state(FAILED);
                                Fl::remove_timeout(marker_timeout_cb, this);
                                break;
                            }
                            if(logic.max_markers_reached()) {
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
                if(Fl::event_key() == FL_Enter) {
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
                            timer.restart();
                            break;

                        case FINISH:
                            exit_marker_window();

                        default:
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
        if(logic.get_state() != PAUSED) {
            return;
        }
        timer.restart();
        Fl::add_timeout(timer.ttl() / 1000.0, marker_timeout_cb, this);
        redraw();
    }
};

Fl_Window* mainWin = nullptr;
MarkerWindow* markerWin = nullptr;

void print_array() {
    logger->reset();
    if(markerWin){
        char buffer[128];
        for (int i = 0; i < markerWin->logic.get_current_marker_index()+1; i++) {
            const Marker& marker = markerWin->logic.get_marker(i);
            snprintf(buffer, sizeof(buffer), "%02d | X = %04d | Y = %04d | Time = %04ld",
                i + 1, marker.x, marker.y, marker.reaction_in_ms);
            logger->log(buffer);
        }
    }
}

void marker_close_cb(Fl_Widget* w, void*) {
    auto* mw = static_cast<MarkerWindow*>(w);
    mw->exit_marker_window();
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

int main(int argc, char **argv) {

    Fl_Window win(600, 600, " ");
    int button_width = 100;
    int button_height = 40;
    Fl_Button start_button((win.w() - button_width) / 4, 30, button_width, button_height, "Start");
    Fl_Button reset_button((win.w() - button_width) * 3 / 4, 30, button_width, button_height, "Reset");

    Fl_Text_Buffer* outputBuffer = new Fl_Text_Buffer;
    int outputBoxWidth = 400;
    int outputBoxHeight = 450;
    Fl_Text_Display* outputBox = new Fl_Text_Display((win.w() - outputBoxWidth) / 2, 100, outputBoxWidth, outputBoxHeight);
    outputBox->buffer(outputBuffer);
    logger = new Logger(outputBox, outputBuffer);

    start_button.callback(start_button_cb, &win);
    reset_button.callback(reset_button_cb, &win);

    win.end();
    win.show(argc, argv);
    return Fl::run();
}