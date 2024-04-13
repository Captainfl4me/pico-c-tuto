#include <stdio.h>
#include <cmath>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/i2c.h"

#include "WS2812.hpp"
#include "ssd1306.hpp"
#include "siera_logo.h"

#define LED_PIN 16
#define LED_LENGTH 1
#define SLEEPTIME 10

struct RGB
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

RGB HSVtoRGB(float H, float S, float V)
{
    if (H > 360 || H < 0 || S > 100 || S < 0 || V > 100 || V < 0)
    {
        return RGB{0, 0, 0};
    }
    float s = S / 100;
    float v = V / 100;
    float C = s * v;
    float X = C * (1 - std::abs(std::fmod(H / 60.0, 2) - 1));
    float m = v - C;
    float r, g, b;
    if (H >= 0 && H < 60)
    {
        r = C, g = X, b = 0;
    }
    else if (H >= 60 && H < 120)
    {
        r = X, g = C, b = 0;
    }
    else if (H >= 120 && H < 180)
    {
        r = 0, g = C, b = X;
    }
    else if (H >= 180 && H < 240)
    {
        r = 0, g = X, b = C;
    }
    else if (H >= 240 && H < 300)
    {
        r = X, g = 0, b = C;
    }
    else
    {
        r = C, g = 0, b = X;
    }
    int R = (r + m) * 255;
    int G = (g + m) * 255;
    int B = (b + m) * 255;

    return RGB{(uint8_t)R, (uint8_t)G, (uint8_t)B};
}

void core1_entry()
{
    WS2812 built_in_led(LED_PIN, LED_LENGTH, pio0, 0, WS2812::FORMAT_RGB);

    while (true)
    {
        for (int i = 0; i < 360; i++)
        {
            RGB rgb = HSVtoRGB(i, 100, 40);
            built_in_led.fill(WS2812::RGB(rgb.r, rgb.g, rgb.b));
            built_in_led.show();
            sleep_ms(10);
        }
    }
}

int main()
{
    stdio_init_all();
    printf("0. Initialize RP2040");

    printf("1. Starting RBG circle on core1");
    multicore_launch_core1(core1_entry);

    i2c_init(i2c0, 400000);
    gpio_set_function(0, GPIO_FUNC_I2C);
    gpio_set_function(1, GPIO_FUNC_I2C);
    gpio_pull_up(0);
    gpio_pull_up(1);

    ssd1306_t disp;
    disp.external_vcc = false;
    ssd1306_init(&disp, 128, 64, 0x3C, i2c0);
    ssd1306_clear(&disp);
    printf("2. Screen setup");

    for (int y = 0; y < 31; ++y)
    {
        ssd1306_draw_line(&disp, 0, y, 127, y);
        ssd1306_show(&disp);
        sleep_ms(SLEEPTIME/2);
        ssd1306_clear(&disp);
    }

    for (int y = 0, i = 1; y >= 0; y += i)
    {
        ssd1306_draw_line(&disp, 0, 31 - y, 127, 31 + y);
        ssd1306_draw_line(&disp, 0, 31 + y, 127, 31 - y);
        ssd1306_show(&disp);
        sleep_ms(SLEEPTIME);
        ssd1306_clear(&disp);
        if (y == 32)
            i = -1;
    }

    ssd1306_bmp_show_image(&disp, siera_logo, siera_logocnt);
    ssd1306_draw_string(&disp, 20, 40, 1, "ROBOTS - DRONES");
    ssd1306_show(&disp);
    
    while (1)
        tight_loop_contents();
        
    return 0;
}
