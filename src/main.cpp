#include <cstdlib>

#include <FL/Fl.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Window.H>


void quit_cb(Fl_Widget*, void*)
{
    std::exit(0);
}

int main(int argc, char **argv) {

    Fl_Window win(800, 600, "mosVision");
    
    Fl_Menu_Bar menubar(0, 0, 800, 25);

    menubar.add("&Datei/&Beenden", FL_CTRL + 'q', quit_cb);

    win.end();
    win.show(argc, argv);
    return Fl::run();
}