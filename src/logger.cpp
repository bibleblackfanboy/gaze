#include "logger.hpp"

Logger::Logger(Fl_Text_Display* display, Fl_Text_Buffer* buffer) : display_(display), buffer_(buffer) { }

void Logger::log(const char* message) {
    if (!display_ || !buffer_)
        return;

    buffer_->append(message);
    buffer_->append("\n");

    display_->insert_position(buffer_->length());
    display_->show_insert_position();
}

void Logger::reset() {
    if (!display_ || !buffer_)
        return;
    buffer_->text("");
}

