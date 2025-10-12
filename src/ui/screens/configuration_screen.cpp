#include "configuration_screen.h"
#include "../../core/config.h"
#include "../../driver/display/st7789_driver.h"

void ConfigurationScreen::on_enter() {
    st7789_fill_screen(COLOR_BLACK);
    st7789_draw_text("CONFIGURACOES", 120, 20, COLOR_WHITE, COLOR_BLACK);
    st7789_draw_text("--------------------", 120, 40, COLOR_WHITE, COLOR_BLACK);

    char buf[100];

    sprintf(buf, "WiFi SSID: %s", config.wifi.ssid.c_str());
    st7789_draw_text(buf, 5, 60, COLOR_WHITE, COLOR_BLACK);

    sprintf(buf, "MQTT Broker: %s", config.mqtt.broker.c_str());
    st7789_draw_text(buf, 5, 80, COLOR_WHITE, COLOR_BLACK);
}

void ConfigurationScreen::on_exit() {
}

void ConfigurationScreen::handle_input() {
    // TODO: Implement back button
}

void ConfigurationScreen::draw() {
    title_bar.draw();
}
