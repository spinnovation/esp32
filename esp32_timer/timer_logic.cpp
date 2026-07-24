#include "timer_logic.h"

TimerLogic g_timer;

TimerLogic::TimerLogic() 
    : state(TIMER_STOPPED), total_seconds(3000), remaining_seconds(3000), last_tick_ms(0) {
    snprintf(custom_message, sizeof(custom_message), "안녕하세요 승필님");
}

void TimerLogic::begin(uint32_t default_seconds) {
    total_seconds = default_seconds;
    remaining_seconds = default_seconds;
    state = TIMER_STOPPED;
    snprintf(custom_message, sizeof(custom_message), "안녕하세요 승필님");
}

void TimerLogic::start() {
    if (state != TIMER_RUNNING) {
        state = TIMER_RUNNING;
        last_tick_ms = millis();
        snprintf(custom_message, sizeof(custom_message), "🔥 50분 집중 시간이 시작되었습니다");
    }
}

void TimerLogic::pause() {
    if (state == TIMER_RUNNING) {
        state = TIMER_PAUSED;
        snprintf(custom_message, sizeof(custom_message), "⏸️ 타이머가 일시 정지되었습니다");
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
    snprintf(custom_message, sizeof(custom_message), "안녕하세요 승필님");
}

void TimerLogic::set_duration(uint32_t seconds) {
    total_seconds = seconds;
    remaining_seconds = seconds;
}

void TimerLogic::set_message(const char* msg) {
    if (msg && strlen(msg) > 0) {
        snprintf(custom_message, sizeof(custom_message), "%s", msg);
    }
}

float TimerLogic::get_progress() const {
    if (total_seconds == 0) return 0.0f;
    return (float)(total_seconds - remaining_seconds) / (float)total_seconds;
}

void TimerLogic::get_formatted_time(char* buffer, size_t size) const {
    uint32_t mins = remaining_seconds / 60;
    uint32_t secs = remaining_seconds % 60;
    snprintf(buffer, size, "%02u:%02u", mins, secs);
}

void TimerLogic::update() {
    if (state == TIMER_RUNNING) {
        unsigned long current_ms = millis();
        if (current_ms - last_tick_ms >= 1000) {
            unsigned long elapsed_seconds = (current_ms - last_tick_ms) / 1000;
            last_tick_ms += elapsed_seconds * 1000;

            if (remaining_seconds > elapsed_seconds) {
                remaining_seconds -= elapsed_seconds;
            } else {
                remaining_seconds = 0;
                state = TIMER_FINISHED;
                snprintf(custom_message, sizeof(custom_message), "🎉 수고하셨습니다! 50분 완료되었습니다");
                
                // Auto Reset after finished
                delay(3000);
                reset();
            }
        }
    }
}
