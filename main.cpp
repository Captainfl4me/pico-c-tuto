#include <stdio.h>
#include <cmath>
#include "pico/stdlib.h"
#include "WS2812.hpp"

#define LED_PIN 16
#define LED_LENGTH 1 

struct RGB{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

RGB HSVtoRGB(float H, float S,float V){
    if(H>360 || H<0 || S>100 || S<0 || V>100 || V<0){
        return RGB{0,0,0};
    }
    float s = S/100;
    float v = V/100;
    float C = s*v;
    float X = C*(1-std::abs(std::fmod(H/60.0, 2)-1));
    float m = v-C;
    float r,g,b;
    if(H >= 0 && H < 60){
        r = C,g = X,b = 0;
    }
    else if(H >= 60 && H < 120){
        r = X,g = C,b = 0;
    }
    else if(H >= 120 && H < 180){
        r = 0,g = C,b = X;
    }
    else if(H >= 180 && H < 240){
        r = 0,g = X,b = C;
    }
    else if(H >= 240 && H < 300){
        r = X,g = 0,b = C;
    }
    else{
        r = C,g = 0,b = X;
    }
    int R = (r+m)*255;
    int G = (g+m)*255;
    int B = (b+m)*255;

    return RGB{(uint8_t)R,(uint8_t)G,(uint8_t)B};
}

int main() {
    stdio_init_all();

    // 0. Initialize LED strip
    printf("0. Initialize LED strip");
    WS2812 built_in_led(LED_PIN, LED_LENGTH, pio0, 0, WS2812::FORMAT_RGB);

    printf("1. Starting RBG circle");
    while (true) {    
        for (int i = 0; i < 360; i++) {
            RGB rgb = HSVtoRGB(i,100,40);
            built_in_led.fill(WS2812::RGB(rgb.r, rgb.g, rgb.b));
            built_in_led.show();
            sleep_ms(5);
        }
    }

    return 0;
}
