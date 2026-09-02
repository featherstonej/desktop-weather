#include "storage.h"
#include <LittleFS.h>
#include <ArduinoJson.h>

static const char *CONFIG_FILE = "/config.json";

bool Storage::begin() {
    if (!LittleFS.begin()) {
        Serial.println("LittleFS mount failed, formatting...");
        LittleFS.format();
        return LittleFS.begin();
    }
    return true;
}

bool Storage::loadConfig(DeviceConfig &config) {
    if (!LittleFS.exists(CONFIG_FILE)) {
        Serial.println("Config file not found, saving defaults.");
        return saveConfig(config);
    }

    File file = LittleFS.open(CONFIG_FILE, "r");
    if (!file) {
        Serial.println("Failed to open config file for reading");
        return false;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.f_str());
        return false;
    }

    if (doc["city"].is<const char*>()) config.cityName = doc["city"].as<String>();
    if (doc["country"].is<const char*>()) config.countryCode = doc["country"].as<String>();
    if (doc["lat"].is<float>()) config.latitude = doc["lat"].as<float>();
    if (doc["lon"].is<float>()) config.longitude = doc["lon"].as<float>();
    if (doc["tz"].is<const char*>()) config.timezone = doc["tz"].as<String>();
    if (doc["tzName"].is<const char*>()) config.tzName = doc["tzName"].as<String>();
    if (doc["fahrenheit"].is<bool>()) config.useFahrenheit = doc["fahrenheit"].as<bool>();
    if (doc["use24h"].is<bool>()) config.use24Hour = doc["use24h"].as<bool>();
    if (doc["dateFormat"].is<const char*>()) config.dateFormat = doc["dateFormat"].as<String>();
    if (doc["brightness"].is<uint8_t>()) config.brightness = doc["brightness"].as<uint8_t>();
    if (doc["rotation"].is<uint8_t>()) config.rotation = doc["rotation"].as<uint8_t>();
    if (doc["autoCycle"].is<bool>()) config.autoCycleScreens = doc["autoCycle"].as<bool>();
    if (doc["cycleInterval"].is<uint16_t>()) config.cycleIntervalSec = doc["cycleInterval"].as<uint16_t>();
    if (doc["staticIcon"].is<uint8_t>()) config.staticIcon = doc["staticIcon"].as<uint8_t>();

    // Font & Typography Options
    if (doc["fontStyle"].is<uint8_t>()) config.clockFontStyle = doc["fontStyle"].as<uint8_t>();
    if (doc["clockSize"].is<uint8_t>()) config.clockSize = doc["clockSize"].as<uint8_t>();
    if (doc["labelSize"].is<uint8_t>()) config.labelSize = doc["labelSize"].as<uint8_t>();
    if (doc["feelsLikeSize"].is<uint8_t>()) config.feelsLikeSize = doc["feelsLikeSize"].as<uint8_t>();

    // Theme Colors
    if (doc["colCity"].is<const char*>()) config.colCity = doc["colCity"].as<String>();
    if (doc["colBadgeBg"].is<const char*>()) config.colBadgeBg = doc["colBadgeBg"].as<String>();
    if (doc["colBadgeTxt"].is<const char*>()) config.colBadgeTxt = doc["colBadgeTxt"].as<String>();
    if (doc["colFeelsLike"].is<const char*>()) config.colFeelsLike = doc["colFeelsLike"].as<String>();
    if (doc["colHour"].is<const char*>()) config.colHour = doc["colHour"].as<String>();
    if (doc["colMin"].is<const char*>()) config.colMin = doc["colMin"].as<String>();
    if (doc["colWeatherBadgeBg"].is<const char*>()) config.colWeatherBadgeBg = doc["colWeatherBadgeBg"].as<String>();
    if (doc["colWeatherBadgeTxt"].is<const char*>()) config.colWeatherBadgeTxt = doc["colWeatherBadgeTxt"].as<String>();
    if (doc["colDate"].is<const char*>()) config.colDate = doc["colDate"].as<String>();
    if (doc["colDay"].is<const char*>()) config.colDay = doc["colDay"].as<String>();
    if (doc["colTempBar"].is<const char*>()) config.colTempBar = doc["colTempBar"].as<String>();
    if (doc["colHumBar"].is<const char*>()) config.colHumBar = doc["colHumBar"].as<String>();
    if (doc["colBg"].is<const char*>()) config.colBg = doc["colBg"].as<String>();

    Serial.println("Config successfully loaded from LittleFS");
    return true;
}

bool Storage::saveConfig(const DeviceConfig &config) {
    File file = LittleFS.open(CONFIG_FILE, "w");
    if (!file) {
        Serial.println("Failed to open config file for writing");
        return false;
    }

    JsonDocument doc;
    doc["city"] = config.cityName;
    doc["country"] = config.countryCode;
    doc["lat"] = config.latitude;
    doc["lon"] = config.longitude;
    doc["tz"] = config.timezone;
    doc["tzName"] = config.tzName;
    doc["fahrenheit"] = config.useFahrenheit;
    doc["use24h"] = config.use24Hour;
    doc["dateFormat"] = config.dateFormat;
    doc["brightness"] = config.brightness;
    doc["rotation"] = config.rotation;
    doc["autoCycle"] = config.autoCycleScreens;
    doc["cycleInterval"] = config.cycleIntervalSec;
    doc["staticIcon"] = config.staticIcon;

    // Font & Typography Options
    doc["fontStyle"] = config.clockFontStyle;
    doc["clockSize"] = config.clockSize;
    doc["labelSize"] = config.labelSize;
    doc["feelsLikeSize"] = config.feelsLikeSize;

    // Theme Colors
    doc["colCity"] = config.colCity;
    doc["colBadgeBg"] = config.colBadgeBg;
    doc["colBadgeTxt"] = config.colBadgeTxt;
    doc["colFeelsLike"] = config.colFeelsLike;
    doc["colHour"] = config.colHour;
    doc["colMin"] = config.colMin;
    doc["colWeatherBadgeBg"] = config.colWeatherBadgeBg;
    doc["colWeatherBadgeTxt"] = config.colWeatherBadgeTxt;
    doc["colDate"] = config.colDate;
    doc["colDay"] = config.colDay;
    doc["colTempBar"] = config.colTempBar;
    doc["colHumBar"] = config.colHumBar;
    doc["colBg"] = config.colBg;

    if (serializeJson(doc, file) == 0) {
        Serial.println("Failed to write config file");
        file.close();
        return false;
    }

    file.close();
    Serial.println("Config successfully saved to LittleFS");
    return true;
}
