/**
 * J1939 Diagnostic Tool - Title Bar Widget
 * 
 * Versão: 3.8.0
 */

#ifndef TITLE_BAR_H
#define TITLE_BAR_H

class TitleBar {
public:
    void draw();

private:
    void draw_can_status();
    void draw_gps_status();
    void draw_wifi_status();
};

#endif // TITLE_BAR_H
