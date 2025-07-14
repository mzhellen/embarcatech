#ifndef TASKCNT_H
#define TASKCNT_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "bib/ssd1306.h"
#include "hardware/i2c.h"
#include "hardware/timer.h"

void setup_led_rgb();
void setup_buttons();
void initial_txt(u_int8_t *ssd, struct render_area frame_area);
void task_txt(u_int8_t *ssd, struct render_area frame_area, uint8_t task_inc);
void final_txt(u_int8_t *ssd, struct render_area frame_area);

struct time_data {
    struct repeating_timer timer;
    uint8_t hrs;
    uint8_t min;
    uint8_t sec;
};

#endif