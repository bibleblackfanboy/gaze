#pragma once 

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <fstream>

#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>
#include "MarkerLogic.hpp"

class Logger {
    private:
        Fl_Text_Display* display_;
        Fl_Text_Buffer* buffer_;

    public:
        Logger(
            Fl_Text_Display* display,
            Fl_Text_Buffer* buffer
        );

        void log(const char* message);
        void save(const Marker* marker_array);
        void reset();
};