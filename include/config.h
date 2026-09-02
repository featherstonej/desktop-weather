#pragma once

#include <Arduino.h>

// Hardware Pin Definitions for GeekMagic SmallTV-Ultra
#define PIN_BL          5   // Backlight PWM (0-255)
#define PIN_BTN         4   // Top Touch / Key button (Active LOW)
#define PIN_LED         2   // Blue LED on ESP-12F (Active LOW)

// Display Dimensions
#define SCREEN_WIDTH    240
#define SCREEN_HEIGHT   240

// Firmware Details
#define FW_NAME         "SmallTV Weather"
#define FW_VERSION      "3.1.0"
#define AP_SSID         "SmallTV-Setup"

// Color conversion helper functions
inline uint16_t hexToRGB565(String hex) {
    if (hex.startsWith("#")) hex = hex.substring(1);
    long number = strtol(hex.c_str(), NULL, 16);
    long r = (number >> 16) & 0xFF;
    long g = (number >> 8) & 0xFF;
    long b = number & 0xFF;
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

inline String rgb565ToHex(uint16_t color) {
    uint8_t r = (color >> 11) << 3;
    uint8_t g = ((color >> 5) & 0x3F) << 2;
    uint8_t b = (color & 0x1F) << 3;
    char hex[8];
    sprintf(hex, "#%02X%02X%02X", r, g, b);
    return String(hex);
}

// Device Settings Structure
struct DeviceConfig {
    String cityName = "Seoul";
    String countryCode = "KR";
    float latitude = 37.5665;
    float longitude = 126.9780;
    String timezone = "JST-9";
    String tzName = "KST";
    bool useFahrenheit = false;
    bool use24Hour = true;
    String dateFormat = "DD/MM/YYYY";
    uint8_t brightness = 200;
    uint8_t rotation = 0;
    bool autoCycleScreens = false;
    uint16_t cycleIntervalSec = 10;

    // Static Icon Selection:
    // 0=Spaceman, 1=Desk Cat, 2=Saturn Planet, 3=Rocket Ship, 4=Coffee Mug, 5=Pacman, 6=Live Weather Art, 7=Game Controller
    uint8_t staticIcon = 0;

    // Typography & Font Options
    uint8_t clockFontStyle = 0; // 0=7-Segment Digital, 1=Modern Bold, 2=Futuristic Mono, 3=Clean Sans
    uint8_t clockSize = 0;      // 0=Normal, 1=Large, 2=Compact
    uint8_t labelSize = 0;      // 0=Normal, 1=Small, 2=Large
    uint8_t feelsLikeSize = 1;  // 0=16px (Font 2), 1=19px (FreeSans 12pt), 2=26px (Font 4), 3=12px (FreeSans 9pt)

    // Customizable UI Theme Colors
    String colCity     = "#FFB800";
    String colBadgeBg  = "#39FF14";
    String colBadgeTxt = "#000000";
    String colHour     = "#FFFFFF";
    String colMin      = "#FFDE00";
    String colSec      = "#FFFFFF";
    String colDate     = "#FFFFFF";
    String colDay      = "#00E676";
    String colTempBar  = "#3A86FF";
    String colHumBar   = "#FFD166";
    String colBg       = "#000000";
};
