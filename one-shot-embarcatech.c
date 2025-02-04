#include <stdio.h>
#include "pico/stdlib.h"

#define LED_GREEN_PIN 11
#define LED_BLUE_PIN 12
#define LED_RED_PIN 13
#define BUTTON_A_PIN 5

static bool alarm_triggered = false;

void turn_leds(bool red, bool blue, bool green);
void gpio_irq_handler(uint gpio, uint32_t events);

int64_t turn_off_red_led_alarm_callback(alarm_id_t id, void *user_data) {
    turn_leds(false, true, true);
    return 0;
}

int64_t turn_off_blue_led_alarm_callback(alarm_id_t id, void *user_data) {
    turn_leds(false, false, true);
    return 0;
}

int64_t turn_off_green_led_alarm_callback(alarm_id_t id, void *user_data) {
    turn_leds(false, false, false);
    alarm_triggered = false;
    return 0;
}

void set_alarm_in_ms(uint32_t ms, alarm_callback_t alarm_callback) {
    alarm_id_t alarm = add_alarm_in_ms(ms, alarm_callback, NULL, false);
    if (!alarm) {
        printf("Erro ao criar alarme!\n");
    }
}

int main() {
    stdio_init_all();

    gpio_init(LED_RED_PIN);
    gpio_init(LED_BLUE_PIN);
    gpio_init(LED_GREEN_PIN);
    gpio_set_dir(LED_RED_PIN, GPIO_OUT);
    gpio_set_dir(LED_BLUE_PIN, GPIO_OUT);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);

    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    while (true) {
        sleep_ms(1000);
    }
}

void turn_leds(bool red, bool blue, bool green) {
    gpio_put(LED_RED_PIN, red);
    gpio_put(LED_BLUE_PIN, blue);
    gpio_put(LED_GREEN_PIN, green);
}

void gpio_irq_handler(uint gpio, uint32_t events) {
    if (alarm_triggered) {
        return;
    }

    volatile static uint32_t last_time = 0;
    volatile uint32_t current_time = to_ms_since_boot(get_absolute_time());

    if (current_time - last_time < 400) {
        return;
    }

    last_time = current_time;

    turn_leds(true, true, true);
    set_alarm_in_ms(3000, turn_off_red_led_alarm_callback);
    set_alarm_in_ms(6000, turn_off_blue_led_alarm_callback);
    set_alarm_in_ms(9000, turn_off_green_led_alarm_callback);

    alarm_triggered = true;
}
