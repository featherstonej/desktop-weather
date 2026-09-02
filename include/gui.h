#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <TFT_eSPI.h>
#include "config.h"
#include "weather.h"

enum ScreenState {
    SCREEN_CLOCK_WEATHER = 0,
    SCREEN_FORECAST = 1,
    SCREEN_SYSTEM_INFO = 2,
    SCREEN_COUNT = 3
};

class GUI {
public:
    static void init(TFT_eSPI *display);
    static void drawScreen(ScreenState screen, const DeviceConfig &config, const WeatherData &weather);
    static void drawDesktopIcon(TFT_eSPI *t, int x, int y, uint8_t iconType, int weatherCode);
    static void drawSpaceman(TFT_eSPI *t, int x, int y);
    static void drawCat(TFT_eSPI *t, int x, int y);
    static void drawPlanet(TFT_eSPI *t, int x, int y);
    static void drawRocket(TFT_eSPI *t, int x, int y);
    static void drawCoffee(TFT_eSPI *t, int x, int y);
    static void drawPacman(TFT_eSPI *t, int x, int y);
    static void drawWeatherArt(TFT_eSPI *t, int x, int y, int weatherCode);
    static void drawGamepad(TFT_eSPI *t, int x, int y);
    
    static void drawThermometerIcon(TFT_eSPI *t, int x, int y, uint16_t color);
    static void drawHumidityIcon(TFT_eSPI *t, int x, int y, uint16_t color);
    static void drawPillGauge(TFT_eSPI *t, int x, int y, int w, int h, float percent, uint16_t barColor, uint16_t bgColor);
    static void drawWeatherIcon(TFT_eSPI *t, int x, int y, int code, bool mini = false);

private:
    static TFT_eSPI *tft;
    static void drawMainScreen(const DeviceConfig &config, const WeatherData &weather);
    static void drawForecastScreen(const DeviceConfig &config, const WeatherData &weather);
    static void drawSystemScreen(const DeviceConfig &config, const WeatherData &weather);
};
