/**
 * J1939 Diagnostic Tool - Advanced Button Driver Implementation
 * 
 * Versão: 2.1.0
 */

#include "button_driver.h"
#include "../../core/config.h"

// --- RTOS Queue Definition ---
QueueHandle_t button_event_queue;

// --- State Machine Configuration ---
#define DEBOUNCE_TIME_MS 50
#define LONG_PRESS_TIME_MS 700
#define DOUBLE_CLICK_GAP_MS 250

// --- Private State Machine Struct and Instances ---
typedef enum { IDLE, DEBOUNCING, PRESSED, WAITING_FOR_UP, WAITING_FOR_SECOND_PRESS } ButtonFsmState;

struct ButtonState {
    uint8_t pin;
    button_event_t press_event;
    button_event_t long_press_event;
    button_event_t double_click_event;

    ButtonFsmState state = IDLE;
    unsigned long last_change_time = 0;

    // Constructor to allow safe initialization of members at runtime
    ButtonState(uint8_t p, button_event_t e1, button_event_t e2, button_event_t e3)
        : pin(p), press_event(e1), long_press_event(e2), double_click_event(e3) {}
};

// Use pointers so we can initialize from runtime `config` values in init()
static ButtonState* select_button = nullptr;
static ButtonState* next_button = nullptr;

// --- Private State Machine Logic ---

void update_button_fsm(ButtonState& btn) {
    unsigned long now = millis();
    bool pin_is_low = (digitalRead(btn.pin) == LOW);

    switch (btn.state) {
        case IDLE:
            if (pin_is_low) {
                btn.state = DEBOUNCING;
                btn.last_change_time = now;
            }
            break;

        case DEBOUNCING:
            if (now - btn.last_change_time > DEBOUNCE_TIME_MS) {
                if (pin_is_low) {
                    btn.state = PRESSED;
                    btn.last_change_time = now;
                } else {
                    btn.state = IDLE;
                }
            }
            break;

        case PRESSED:
            if (pin_is_low) {
                if (now - btn.last_change_time > LONG_PRESS_TIME_MS) {
                    xQueueSend(button_event_queue, &btn.long_press_event, 0);
                    btn.state = WAITING_FOR_UP;
                }
            } else { // Pin is high (released)
                if (now - btn.last_change_time < DOUBLE_CLICK_GAP_MS) {
                    btn.state = WAITING_FOR_SECOND_PRESS;
                    btn.last_change_time = now;
                } else {
                    xQueueSend(button_event_queue, &btn.press_event, 0);
                    btn.state = IDLE;
                }
            }
            break;

        case WAITING_FOR_UP: // After a long press
            if (!pin_is_low) {
                btn.state = IDLE;
            }
            break;

        case WAITING_FOR_SECOND_PRESS:
            if (now - btn.last_change_time > DOUBLE_CLICK_GAP_MS) {
                // Double click window timed out, it was a single press
                xQueueSend(button_event_queue, &btn.press_event, 0);
                btn.state = IDLE;
            } else if (pin_is_low) {
                // Second press detected!
                xQueueSend(button_event_queue, &btn.double_click_event, 0);
                btn.state = WAITING_FOR_UP;
            }
            break;
    }
}

// --- RTOS Task ---

void button_driver_task_fn(void* pv) {
    if (select_button) pinMode(select_button->pin, INPUT_PULLUP);
    if (next_button) pinMode(next_button->pin, INPUT_PULLUP);

    for (;;) {
        if (select_button) update_button_fsm(*select_button);
        if (next_button) update_button_fsm(*next_button);
        vTaskDelay(pdMS_TO_TICKS(10)); // Poll every 10ms
    }
}

// --- Public API ---

void button_driver_init() {
    // allocate button state objects using runtime config values
    select_button = new ButtonState(config.buttons.select_pin, BTN_PRESS_SELECT, BTN_LONG_PRESS_SELECT, BTN_DOUBLE_CLICK_SELECT);
    next_button = new ButtonState(config.buttons.next_pin, BTN_PRESS_NEXT, BTN_LONG_PRESS_NEXT, BTN_DOUBLE_CLICK_NEXT);

    button_event_queue = xQueueCreateStatic(10, sizeof(button_event_t), (uint8_t*)ps_malloc(10 * sizeof(button_event_t)), new StaticQueue_t);
    xTaskCreate(button_driver_task_fn, "Button Driver Task", 4096, NULL, 10, NULL);
}
