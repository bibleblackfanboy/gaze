#include <cstdlib>
#include <vector>

#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>

struct Marker {
    int position_x;
    int position_y;
};

Fl_Text_Display* outputBox;
Fl_Text_Buffer* outputBuffer;
void log_message(const char* msg) {
    outputBuffer->append(msg);
    outputBuffer->append("\n");

    outputBox->insert_position(outputBuffer->length());
    outputBox->show_insert_position();
}

class MarkerWindow : public Fl_Double_Window {
    Fl_Window* parentWindow;

    Marker markers[20];
    int marker_count = 0;
    int current_marker = -1;

    public:
    MarkerWindow(int w, int h, const char * title, Fl_Window* parent): Fl_Double_Window(w, h, title), parentWindow(parent) { }

    void exit_marker_window() {
        log_message("Exiting Marker Window");
        fullscreen_off();
        if (parentWindow)
            parentWindow->show();
        hide();
    }

    void show_marker(int x, int y) {
        char buffer[100];
        sprintf(buffer, "Marker showing at (%d|%d)", x, y);
        log_message(buffer);
        
        if (marker_count < 20) {
            markers[marker_count] = {x, y};
            current_marker = marker_count;
            marker_count++;
        } else {
            log_message("Maximum number of markers reached.");
            exit_marker_window();
        }
        redraw();
    }

    void draw() override {
        Fl_Double_Window::draw();
        fl_color(FL_RED);
        if(current_marker >= 0 && current_marker < marker_count) {
            const auto& marker = markers[current_marker];
            fl_pie(marker.position_x - 10, marker.position_y - 10, 20, 20, 0, 360);
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
                    show_marker(rand() % w(), rand() % h());
                    return 1;
                }
                break;

            case FL_CLOSE:
                exit_marker_window();
                return 1;
        }
        return Fl_Double_Window::handle(event);
    }
};

Fl_Window* mainWin = nullptr;
MarkerWindow* markerWin = nullptr;

void marker_close_cb(Fl_Widget* w, void*) {
    auto* mw = static_cast<MarkerWindow*>(w);
    mw->exit_marker_window();
}

void start_cb(Fl_Widget*, void* data) {
    mainWin = static_cast<Fl_Window*>(data);
    if (!markerWin) {
        markerWin = new MarkerWindow(0, 0, " ", mainWin);
        markerWin->callback(marker_close_cb);
    }
    markerWin->show();
    log_message("Switch to Markerscreen");
    markerWin->fullscreen();
    markerWin->resize(0, 0, Fl::w(), Fl::h());
    markerWin->take_focus();

    mainWin->hide();
}

int main(int argc, char **argv) {

    Fl_Window win(400, 400, " ");
    int button_width = 100;
    int button_height = 40;
    Fl_Button start_button((win.w() - button_width) / 4, 80, button_width, button_height, "Start");
    Fl_Button show_array_button((win.w() - button_width) * 3 / 4, 80, button_width, button_height, "Show Array");

    outputBuffer = new Fl_Text_Buffer();
    int outputBoxWidth = 300;
    int outputBoxHeight = 200;
    outputBox = new Fl_Text_Display((win.w() - outputBoxWidth) / 2, 150, outputBoxWidth, outputBoxHeight);
    outputBox->buffer(outputBuffer);

    start_button.callback(start_cb, &win);

    win.end();
    win.show(argc, argv);
    return Fl::run();
}