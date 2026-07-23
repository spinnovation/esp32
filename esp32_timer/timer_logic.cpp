#include "timer_logic.h"
#include "config.h"

TimerLogic g_timer;

TimerLogic::TimerLogic() 
    : state(TIMER_STOPPED), total_seconds(DEFAULT_TIMER_SECONDS), remaining_seconds(DEFAULT_TIMER_SECONDS), last_tick_ms(0) {}

void TimerLogic::begin(uint32_t default_seconds) {
    total_seconds = default_seconds;
    remaining_seconds = default_seconds;
    state = TIMER_STOPPED;
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);
}

void TimerLogic::start() {
    if (remaining_seconds > 0) {
        state = TIMER_RUNNING;
        last_tick_ms = millis();
    }
}

void TimerLogic::pause() {
    if (state == TIMER_RUNNING) {
        state = TIMER_PAUSED;
    }
}

void TimerLogic::toggle() {
    if (state == TIMER_RUNNING) {
        pause();
    } else {
        start();
    }
}

void TimerLogic::reset() {
    state = TIMER_STOPPED;
    remaining_seconds = total_seconds;
    digitalWrite(BUZZER_PIN, LOW);
}

void TimerLogic::set_duration(uint32_t seconds) {
    total_seconds = seconds;
    remaining_seconds = seconds;
    state = TIMER_STOPPED;
    digitalWrite(BUZZER_PIN, LOW);
}

void TimerLogic::update() {
    if (state != TIMER_RUNNING) return;

    unsigned long now = millis();
    if (now - last_tick_ms >= 1000) {
        last_tick_ms += 1000;
        if (remaining_seconds > 0) {
            remaining_seconds--;
        }

        if (remaining_seconds == 0) {
            state = TIMER_FINISHED;
            trigger_alarm();
        }
    }
}

float TimerLogic::get_progress() const {
    if (total_seconds == 0) return 0.0f;
    return (float)remaining_seconds / (float)total_seconds;
}

void TimerLogic::get_formatted_time(char* buffer, size_t size) const {
    uint32_t mins = remaining_seconds / 60;
    uint32_t secs = remaining_seconds % 60;
    snprintf(buffer, size, "%02u:%02u", mins, secs);
}

void TimerLogic::trigger_alarm() {
    // Generate beep alarm tone on completion
    for (int i = 0; i < 3; i++) {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(150);
        digitalWrite(BUZZER_PIN, LOW);
        delay(100);
    }
}
