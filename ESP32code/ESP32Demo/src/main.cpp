#include <Arduino.h>
#include <Wire.h>
#include "../lib/Adafruit-GFX-Library-master/Adafruit_GFX.h"
#include "../lib/Adafruit_SSD1306/Adafruit_SSD1306.h"
#include "../Freenove_WS2812_Lib_for_ESP32/src/Freenove_WS2812_Lib_for_ESP32.h"
#include "../lib/FastLED/FastLED.h"

#define Do 262 //音调频率
#define Re 294
#define Mi 330
#define Fa 349
#define Sol 392
#define La 440
#define Si 494
#define Do_h 523
#define Re_h 587
#define Mi_h 659
#define Fa_h 698
#define Sol_h 784
#define La_h 880
#define Si_h 988

int tune[] = {Sol, Sol, La, Sol, Do_h, Si,
              Sol, Sol, La, Sol, Re_h, Do_h,
              Sol, Sol, Sol_h, Mi_h, Do_h, Si, La,
              Fa_h, Fa_h, Mi_h, Do_h, Re_h, Do_h}; //生日歌曲谱
float durt[] =
    {
        //生日歌谱
        0.5,
        0.5,
        1,
        1,
        1,
        1 + 1,
        0.5,
        0.5,
        1,
        1,
        1,
        1 + 1,
        0.5,
        0.5,
        1,
        1,
        1,
        1,
        1,
        0.5,
        0.5,
        1,
        1,
        1,
        1 + 1,
};

int length;
int channel = 0;     //蜂鸣器频道
int resolution = 14; //蜂鸣器分辨率
int sr = 0;
int srrun = 0;

#define LEDS_COUNT 8   //彩灯数目
#define LEDS_COUNT1 16 //彩灯数目
#define LEDS_PIN 14    //ESP32控制ws2812的引脚
#define LEDS_PIN1 12   //ESP32控制ws2812的引脚
#define CHANNEL 0      //控制通道，最多8路
#define CHANNEL1 1     //控制通道，最多8路
#define USE_MULTCORE 1 //是否使用多线程

#define b1 16 //第一个徽章检测
#define b2 17 //第二个徽章检测
#define b3 18 //第三个徽章检测
#define b4 19 //第四个徽章检测
#define b5 23 //第五个徽章检测
#define b6 26 //第六个徽章检测
#define b7 27 //第七个徽章检测
#define b8 32 //第八个徽章检测

/*#define LED_PIN     14         
#define NUM_LEDS    16   
CRGB leds[NUM_LEDS];
CRGB led[NUM_LEDS];
CRGB led_bucket[NUM_LEDS];
int s=0;*/

Freenove_ESP32_WS2812 strip = Freenove_ESP32_WS2812(LEDS_COUNT, LEDS_PIN, CHANNEL, TYPE_GRB);   //申请彩灯控制对象
Freenove_ESP32_WS2812 strip1 = Freenove_ESP32_WS2812(LEDS_COUNT1, LEDS_PIN1, CHANNEL1, TYPE_GRB);

#define OLED_RESET 21   //OLED定义
Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);

static const unsigned char PROGMEM pic1[] = { //R标志
    0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xf8, 0x00, 0x01, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xfc, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xfe, 0x00, 0x00, 0x3f, 0xff, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0x00, 0x00, 0x1f, 0xff, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0x80, 0x00, 0x1f, 0xff, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xc0, 0x00, 0x1f, 0xff, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xe0, 0x00, 0x1f, 0xff, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xf0, 0x00, 0x3f, 0xff, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xf0, 0x00, 0x3f, 0xfe, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xf8, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xfe, 0x00, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xfe, 0x00, 0x7f, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xfe, 0x00, 0x3f, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xfc, 0x00, 0x1f, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xfc, 0x00, 0x0f, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xfc, 0x00, 0x07, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xfc, 0x00, 0x03, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xf8, 0x00, 0x03, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xf8, 0x00, 0x01, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xf8, 0x00, 0x00, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xf8, 0x00, 0x00, 0x7f, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xf0, 0x00, 0x00, 0x3f, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xf0, 0x07, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xf0, 0x01, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xf0, 0x00, 0x7f, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xe0, 0x00, 0x1f, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xe0, 0x00, 0x0f, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xe0, 0x00, 0x01, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xc0, 0x00, 0x00, 0x7f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x1f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x80, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xc0, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf0, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00};

static const unsigned char PROGMEM str_1[] =
    {
        0x08, 0x08, 0xFD, 0xFC, 0x09, 0x00, 0x09, 0xF8, 0x09, 0x00, 0x79, 0xF8, 0x41, 0x00, 0x47, 0xFE,
        0x41, 0x40, 0x79, 0x44, 0x09, 0x28, 0x09, 0x30, 0x09, 0x10, 0x09, 0x4E, 0x51, 0x84, 0x21, 0x00};

//中文名字1
static const unsigned char PROGMEM str_2[] =
    {
        0x00, 0x04, 0x04, 0xFE, 0x7E, 0x84, 0x44, 0x84, 0x44, 0x84, 0x44, 0xFC, 0x7C, 0x84, 0x44, 0x84,
        0x44, 0x84, 0x44, 0xFC, 0x7C, 0x84, 0x44, 0x84, 0x01, 0x04, 0x01, 0x04, 0x02, 0x14, 0x04, 0x08};

//中文名字2
static const unsigned char PROGMEM str_3[] =
    {
        0x01, 0x10, 0x01, 0x10, 0xF7, 0xFC, 0x21, 0x10, 0x21, 0x10, 0x21, 0xF0, 0xF9, 0x10, 0x21, 0x10,
        0x21, 0xF0, 0x21, 0x10, 0x21, 0x14, 0x27, 0xFE, 0x38, 0x00, 0xE1, 0x10, 0x42, 0x0C, 0x04, 0x04};

//中文名字3
static const unsigned char PROGMEM str_4[] =
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x31, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc1, 0xe0,
        0x00, 0x79, 0xe0, 0x00, 0x00, 0x00, 0xe0, 0x70, 0x00, 0x31, 0xe1, 0xf0,
        0x7f, 0xf9, 0xc0, 0x60, 0x00, 0x00, 0xff, 0xf8, 0x00, 0x79, 0xc1, 0xe0,
        0x38, 0x79, 0xc0, 0xf0, 0x1c, 0x1c, 0xe0, 0x70, 0x7f, 0xf9, 0xc1, 0xe0,
        0x00, 0x71, 0xc0, 0xf8, 0x0f, 0xfe, 0xe0, 0x70, 0x3f, 0x81, 0xc1, 0xf8,
        0x00, 0x71, 0xc1, 0xf0, 0x0e, 0x1c, 0xe0, 0x70, 0x07, 0x9f, 0xff, 0xfc,
        0x00, 0x71, 0xc3, 0xe0, 0x0e, 0x1c, 0xe0, 0x70, 0x07, 0x8f, 0xc1, 0xe0,
        0x00, 0x71, 0xc7, 0x80, 0x0e, 0x1c, 0xe0, 0x70, 0x07, 0x81, 0xc1, 0xe0,
        0x1c, 0x71, 0xcf, 0x00, 0x0e, 0x1c, 0xe0, 0x70, 0x07, 0x81, 0xc1, 0xe0,
        0x1f, 0xf1, 0xde, 0x00, 0x0e, 0x1c, 0xff, 0xf0, 0x07, 0x81, 0xc1, 0xe0,
        0x1c, 0x71, 0xf8, 0x00, 0x0f, 0xfc, 0xe0, 0x70, 0x07, 0x81, 0xff, 0xe0,
        0x1c, 0x71, 0xf0, 0x18, 0x0e, 0x1c, 0xe0, 0x70, 0x07, 0x81, 0xc1, 0xe0,
        0x1c, 0x01, 0xc0, 0x3c, 0x0e, 0x1c, 0xe0, 0x70, 0x07, 0xf1, 0xc1, 0xe0,
        0x1c, 0x1f, 0xff, 0xfe, 0x0e, 0x1c, 0xe0, 0x70, 0x7f, 0xf9, 0xc1, 0xe0,
        0x3c, 0xff, 0xd8, 0x00, 0x0e, 0x1c, 0xe0, 0x70, 0x3f, 0x81, 0xff, 0xe0,
        0x3f, 0xf9, 0xdc, 0x00, 0x0e, 0x1c, 0xe0, 0x70, 0x07, 0x81, 0xc1, 0xe0,
        0x38, 0xf1, 0xcc, 0x00, 0x0e, 0x1c, 0xff, 0xf0, 0x07, 0x81, 0xc1, 0xe0,
        0x00, 0xe1, 0xce, 0x00, 0x0f, 0xfc, 0xe0, 0x70, 0x07, 0x81, 0xc1, 0xe8,
        0x00, 0xe1, 0xce, 0x00, 0x0e, 0x1c, 0xe0, 0x70, 0x07, 0x81, 0xc1, 0xfc,
        0x00, 0xe1, 0xc7, 0x00, 0x0e, 0x1d, 0xe0, 0x70, 0x07, 0xbf, 0xff, 0xfe,
        0x00, 0xe1, 0xc7, 0x80, 0x0e, 0x11, 0xe0, 0x70, 0x07, 0x9c, 0x00, 0x00,
        0x00, 0xe1, 0xc3, 0xc0, 0x1c, 0x01, 0xc0, 0x70, 0x07, 0xfc, 0xe3, 0x00,
        0x00, 0xe1, 0xc3, 0xe0, 0x00, 0x03, 0xc0, 0x70, 0x07, 0xf1, 0xf3, 0xc0,
        0x00, 0xe1, 0xcf, 0xf8, 0x00, 0x07, 0x80, 0x70, 0x7f, 0x83, 0xf1, 0xf0,
        0x01, 0xe1, 0xfc, 0xfe, 0x00, 0x0f, 0x00, 0x70, 0x7e, 0x07, 0xc0, 0xf8,
        0x3f, 0xe1, 0xf8, 0x7e, 0x00, 0x1e, 0x0f, 0xf0, 0x78, 0x0f, 0x80, 0x7c,
        0x1f, 0xc1, 0xf0, 0x38, 0x00, 0x3c, 0x0f, 0xf0, 0x30, 0x1e, 0x00, 0x3c,
        0x0f, 0xc1, 0xe0, 0x18, 0x00, 0x78, 0x01, 0xf0, 0x00, 0x3c, 0x00, 0x1c,
        0x07, 0x00, 0xc0, 0x00, 0x00, 0xe0, 0x00, 0xe0, 0x00, 0x78, 0x00, 0x1c,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void HZJC(void *jchz)
{
  while (1)
  {
  }
}

void RGB1(void *rgb1) //灯条控制线程
{
  while (1)
  {
    for (int j = 0; j < 255; j += 2)
    {
      for (int i = 0; i < LEDS_COUNT; i++)
      {
        strip.setLedColorData(i, strip.Wheel((i * 256 / LEDS_COUNT + j) & 255)); //设置彩灯颜色数据
      }
      strip.show(); //显示颜色
      delay(10);
    }
  }
}
void SRGQ(void *srgq) //生日歌线程
{
  while (1)
  {
    if (digitalRead(b1))
    {
      srrun++;
    }
    if (srrun > 0)
    {
      if (srrun == 1)
      {
        delay(2000);
      }
      if (sr < 2)
      {
        for (int x = 0; x < length; x++)
        {
          ledcSetup(channel, tune[x], resolution);
          ledcAttachPin(25, channel);
          ledcWrite(channel, tune[x]);
          //ledcWriteTone(tonepin,tune[x]);
          delay(500 * durt[x]);
        }
        sr++;
      }
      else
      {
        ledcWrite(channel, 0);
      }
    }
  }
}
void RGB2(void *rgb2)
{
  while (1)
  {
    for (int j = 0; j < 255; j += 2)
    {
      for (int i = 0; i < LEDS_COUNT1; i++)
      {
        //strip1.setLedColorData(i, strip1.Wheel((i * 256 / LEDS_COUNT1 + j) & 255));   //设置彩灯颜色数据 RGB跑马灯

        strip1.setLedColorData(i, 255, 255, 255);                                       //设置彩灯颜色数据 常亮白色
      }
      strip1.show();                                                                    //显示颜色
      delay(10);
    }
  }
}

void setup()
{
  pinMode(b1, INPUT_PULLDOWN); //设置按键管脚上拉输入模式
  Serial.begin(115200);       
  strip.begin();            //初始化彩灯控制引脚
  strip.setBrightness(20);  //灯带1的亮度
  strip1.begin();           //初始化彩灯控制引脚
  strip1.setBrightness(25); //灯带2的亮度

  length = sizeof(tune) / sizeof(tune[0]);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE);                //开像素点发光
  display.clearDisplay();                     //清屏
  display.drawBitmap(0, 0, pic1, 128, 64, 1); //画出字符对应点阵数据
  display.display();                          //开显示
  delay(5000);
  display.clearDisplay();
  display.setTextSize(2);      //设置字体大小
  display.setTextColor(WHITE); //设置字体颜色白色
  display.setCursor(0, 0);     //设置字体的起始位置
  display.println(" RM000012");
  display.setTextSize(3);      //设置字体大小
  display.setTextColor(WHITE);
  //display.println("  ZMQ");
  //display.drawBitmap(26, 32, str_1, 16, 16, 1); //在坐标X:26  Y:16的位置显示中文字符凌
  //display.drawBitmap(42, 32, str_2, 16, 16, 1); //在坐标X:42  Y:16的位置显示中文字符顺
  //display.drawBitmap(58, 32, str_3, 16, 16, 1);
  //display.drawBitmap()
  display.drawBitmap(0, 32, str_4, 96, 32, 1);
  display.display();
#if !USE_MULTCORE
  xTaskCreatePinnedToCore(RGB1, "RGB1", 4096, NULL, 1, NULL);
  xTaskCreatePinnedToCore(RGB2, "RGB2", 1024, NULL, 2, NULL);
  xTaskCreatePinnedToCore(SRGQ, "SRGQ", 4096, NULL, 3, NULL);
#else
  xTaskCreatePinnedToCore(RGB1, "RGB1", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(RGB2, "RGB2", 1024, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(SRGQ, "SRGQ", 4096, NULL, 1, NULL, 1);
  //xTaskCreatePinnedToCore(HZJC, "HZJC", 1024, NULL, 3, NULL, 0);
#endif
}

void loop()
{
}