#include "weather.h"
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

String WeatherService::getWeatherConditionText(int code) {
    switch (code) {
        case 0: return "Clear";
        case 1: return "Mainly Clear";
        case 2: return "Partly Cloudy";
        case 3: return "Overcast";
        case 45:
        case 48: return "Foggy";
        case 51:
        case 53:
        case 55: return "Drizzle";
        case 56:
        case 57: return "Frz Drizzle";
        case 61:
        case 63:
        case 65: return "Rain";
        case 66:
        case 67: return "Frz Rain";
        case 71:
        case 73:
        case 75: return "Snow";
        case 77: return "Snow Grains";
        case 80:
        case 81:
        case 82: return "Showers";
        case 85:
        case 86: return "Snow Showers";
        case 95:
        case 96:
        case 99: return "Thunder";
        default: return "Partly Cloudy";
    }
}

static String getDayOfWeek(const char* dateStr) {
    if (!dateStr || strlen(dateStr) < 10) return "---";
    int y, m, d;
    if (sscanf(dateStr, "%d-%d-%d", &y, &m, &d) == 3) {
        static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
        y -= m < 3;
        int day = (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
        const char *days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
        return String(days[day]);
    }
    return "---";
}

bool WeatherService::fetchWeather(const DeviceConfig &config, WeatherData &weather) {
    WiFiClient client;
    HTTPClient http;

    String tempUnit = config.useFahrenheit ? "fahrenheit" : "celsius";
    String windUnit = config.useFahrenheit ? "mph" : "kmh";

    String url = "http://api.open-meteo.com/v1/forecast?";
    url += "latitude=" + String(config.latitude, 4);
    url += "&longitude=" + String(config.longitude, 4);
    url += "&current=temperature_2m,relative_humidity_2m,apparent_temperature,weather_code,wind_speed_10m";
    url += "&daily=weather_code,temperature_2m_max,temperature_2m_min";
    url += "&temperature_unit=" + tempUnit;
    url += "&wind_speed_unit=" + windUnit;
    url += "&timezone=auto&forecast_days=5";

    Serial.print("Fetching weather from: ");
    Serial.println(url);

    http.begin(client, url);
    http.setTimeout(10000);
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    http.setUserAgent("Mozilla/5.0 (ESP8266; SmallTV-Ultra)");

    int httpCode = http.GET();

    if (httpCode != HTTP_CODE_OK) {
        Serial.printf("HTTP GET failed, error: %s (%d)\n", http.errorToString(httpCode).c_str(), httpCode);
        http.end();
        return false;
    }

    // Use getString() to automatically decode HTTP chunked transfer encoding!
    String payload = http.getString();
    http.end();

    if (payload.length() == 0) {
        Serial.println("Empty payload received from Open-Meteo");
        return false;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.f_str());
        Serial.println("Payload preview: " + payload.substring(0, 100));
        return false;
    }

    // Parse current conditions
    JsonObject current = doc["current"];
    if (current.isNull()) {
        Serial.println("Missing 'current' in weather JSON");
        return false;
    }

    weather.currentTemp = current["temperature_2m"].as<float>();
    weather.apparentTemp = current["apparent_temperature"].as<float>();
    weather.humidity = current["relative_humidity_2m"].as<int>();
    weather.windSpeed = current["wind_speed_10m"].as<float>();
    weather.weatherCode = current["weather_code"].as<int>();
    weather.conditionText = getWeatherConditionText(weather.weatherCode);

    // Parse daily forecast
    JsonObject daily = doc["daily"];
    if (!daily.isNull()) {
        JsonArray timeArr = daily["time"];
        JsonArray codeArr = daily["weather_code"];
        JsonArray maxArr = daily["temperature_2m_max"];
        JsonArray minArr = daily["temperature_2m_min"];

        int count = min((int)timeArr.size(), 5);
        for (int i = 0; i < count; i++) {
            const char* dStr = timeArr[i].as<const char*>();
            weather.forecast[i].dayName = (i == 0) ? "Today" : getDayOfWeek(dStr);
            weather.forecast[i].weatherCode = codeArr[i].as<int>();
            weather.forecast[i].tempMin = minArr[i].as<float>();
            weather.forecast[i].tempMax = maxArr[i].as<float>();
        }

        if (count > 0) {
            weather.tempMinToday = weather.forecast[0].tempMin;
            weather.tempMaxToday = weather.forecast[0].tempMax;
        }
    }

    // Record last sync time
    time_t now = time(nullptr);
    struct tm *timeinfo = localtime(&now);
    if (timeinfo && timeinfo->tm_year > (2020 - 1900)) {
        char buf[16];
        if (config.use24Hour) {
            sprintf(buf, "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);
        } else {
            int h = timeinfo->tm_hour % 12;
            if (h == 0) h = 12;
            sprintf(buf, "%d:%02d %s", h, timeinfo->tm_min, timeinfo->tm_hour >= 12 ? "PM" : "AM");
        }
        weather.lastSyncTime = String(buf);
    } else {
        weather.lastSyncTime = "Synced";
    }

    weather.valid = true;
    Serial.printf("Weather parsed successfully! Temp: %.1f, Code: %d, Cond: %s\n",
                  weather.currentTemp, weather.weatherCode, weather.conditionText.c_str());
    return true;
}
