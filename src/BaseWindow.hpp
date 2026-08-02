#pragma once

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Window.H>

class BaseWindow : public Fl_Double_Window {
public:
    BaseWindow(int w, int h, const char* title, Fl_Window* parent);
    virtual ~BaseWindow() = default;

    virtual void exit_window();

protected:
    Fl_Window* parentWindow;

    void draw_centered_text(const char* s);
    virtual bool needs_redraw() const = 0;

private:
    static void refresh_cb(void* w);
};