#ifndef TIMER_LOGIC_H
#define TIMER_LOGIC_H

#include <Arduino.h>

enum TimerState {
    TIMER_STOPPED,
    TIMER_RUNNING,
    TIMER_PAUSED,
    TIMER_FINISHED
};

class TimerLogic {
public:
    TimerLogic();
    void begin(uint32_t default_seconds = 3000); // 50 minutes = 3000s
    void update();
    
    void start();
    void pause();
    void toggle();
    void reset();
    void set_duration(uint32_t seconds);
    void set_message(const char* msg);

    TimerState get_state() const { return state; }
    uint32_t get_remaining_seconds() const { return remaining_seconds; }
    uint32_t get_total_seconds() const { return total_seconds; }
    float get_progress() const;
    void get_formatted_time(char* buffer, size_t size) const;
    const char* get_message() const { return custom_message; }

private:
    TimerState state;
    uint32_t total_seconds;
    uint32_t remaining_seconds;
    unsigned long last_tick_ms;
    unsigned long finish_time_ms;
    char custom_message[128];
};

extern TimerLogic g_timer;

#endif // TIMER_LOGIC_H
