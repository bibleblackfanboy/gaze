#include "MarkerTiming.hpp"


MarkerTiming::MarkerTiming(int ttl) : start_time(std::chrono::steady_clock::now()), ttl_ms(ttl) { }

// Restart marker time
void MarkerTiming::restart() {
    start_time = std::chrono::steady_clock::now();
}

// Get time passed since marker time start
long MarkerTiming::elapsed_time_ms() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).count();
};

// Get marker progress for clearing
double MarkerTiming::marker_progress() const {
    return std::min(1.0, static_cast<double>(elapsed_time_ms()) / static_cast<double>(ttl_ms - reaction_time));
};

// Get marker time to live
int MarkerTiming::ttl() const {
    return ttl_ms;
};

// Check if marker has expired
bool MarkerTiming::expired() const {
    return elapsed_time_ms() >= ttl_ms;
};