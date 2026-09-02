#pragma once

#include <Arduino.h>
#include "config.h"

struct DailyForecast {
    String dayName;
    int weatherCode;
    float tempMin;
    float tempMax;
};

struct WeatherData {
    bool valid = false;
    float currentTemp = 0.0;
    float apparentTemp = 0.0;
    int humidity = 0;
    float windSpeed = 0.0;
    int weatherCode = 0;
    float tempMinToday = 0.0;
    float tempMaxToday = 0.0;
    String conditionText = "Unknown";
    DailyForecast forecast[5];
    String lastSyncTime = "--:--";
};

class WeatherService {
public:
    static bool fetchWeather(const DeviceConfig &config, WeatherData &weather);
    static String getWeatherConditionText(int code);
};
