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

void Logger::save(const Marker* marker_array, int marker_count) {
    std::time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    std::stringstream ss;
    ss << "log_markers_" << std::put_time(std::localtime(&t), "%Y-%m-%d_%H-%M-%S");

    std::string filename = "../logs/" + ss.str() + ".csv";
    std::ofstream datei(filename);
    if (!datei.is_open()) {
        return;
    }

    for(int i = 0; i < marker_count; i++) {
        datei << marker_array[i].x << ";"
              << marker_array[i].y << "\n";
    }
}

void Logger::reset() {
    if (!display_ || !buffer_)
        return;
    buffer_->text("");
}

