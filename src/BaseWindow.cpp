#include "BaseWindow.hpp"

#include <FL/Fl.H>
#include <FL/fl_draw.H>

BaseWindow::BaseWindow(int w, int h, const char* title, Fl_Window* parent)
    : Fl_Double_Window(w, h, title), parentWindow(parent) {
    Fl::add_timeout(0.016, refresh_cb, this);
}

void BaseWindow::refresh_cb(void* w) {
    auto* win = static_cast<BaseWindow*>(w);
    if (win->needs_redraw())
        win->redraw();
    Fl::repeat_timeout(0.016, refresh_cb, w);
}

void BaseWindow::exit_window() {
    fullscreen_off();
    if (parentWindow)
        parentWindow->show();
    hide();
}

void BaseWindow::draw_centered_text(const char* s) {
    fl_font(FL_HELVETICA_BOLD, 32);
    fl_color(FL_BLACK);
    int tw = 0, th = 0;
    fl_measure(s, tw, th);
    fl_draw(s, (w() - tw) / 2, (h() + th) / 2);
}