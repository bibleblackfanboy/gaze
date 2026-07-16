#pragma once
#include <chrono>
#include <algorithm>

#define MARKER_TTL_IN_MS 2000

class MarkerTiming{
    private:
        std::chrono::steady_clock::time_point start_time;
        int ttl_ms;
        int reaction_time = 400;
    
    public:
        explicit MarkerTiming(int ttl = MARKER_TTL_IN_MS);
        
        void restart();

        long elapsed_time_ms() const;
        double marker_progress() const;

        int ttl() const;
        bool expired() const;

        int get_reaction_time() const;
};