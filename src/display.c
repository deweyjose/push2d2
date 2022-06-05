//
// Created by Dewey Jose on 5/7/22.
//

#include <stdio.h>
#include <lcd.h>
#include <display.h>
#include <config.h>

#define log(format, ...) printf("DISPLAY: " format "\n" , ##__VA_ARGS__)

int lcd = 0;

int display_initialize(const struct config *s) {
    log("initializing");

    int mode4 = s->display_config.bit_mode == 4;
    lcd = lcdInit(
            s->display_config.rows,
            s->display_config.columns,
            s->display_config.bit_mode,
            s->display_config.register_select_pin,
            s->display_config.enable_pin,
            mode4 ? s->display_config.d4_pin : s->display_config.d0_pin,
            mode4 ? s->display_config.d5_pin : s->display_config.d1_pin,
            mode4 ? s->display_config.d6_pin : s->display_config.d2_pin,
            mode4 ? s->display_config.d7_pin : s->display_config.d3_pin,
            mode4 ? 0 : s->display_config.d4_pin,
            mode4 ? 0 : s->display_config.d5_pin,
            mode4 ? 0 : s->display_config.d6_pin,
            mode4 ? 0 : s->display_config.d7_pin);

    if (lcd == -1) {
        log("failed to initialize display");
        return 0;
    }

    lcdPosition(lcd, 0, 0);
    lcdClear(lcd);
    return 1;
}

void display_clear() {
    lcdClear(lcd);
}

void display_text(char *text, int line) {
    //log("=> lcd: %s", text);
    lcdPosition(lcd, 0, line);
    lcdPuts(lcd, text);
}