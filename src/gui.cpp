#include "gui.h"
#include <time.h>

TFT_eSPI *GUI::tft = nullptr;

void GUI::init(TFT_eSPI *display) {
    tft = display;
}

void GUI::drawWeatherIcon(TFT_eSPI *t, int x, int y, int code, bool mini) {
    if (!t) return;
    if (code == 0 || code == 1) {
        t->fillCircle(x + 10, y + 10, 6, TFT_YELLOW);
        t->drawCircle(x + 10, y + 10, 8, 0xFD20);
    } else if (code == 2 || code == 3) {
        t->fillCircle(x + 7, y + 11, 4, 0x9CD3);
        t->fillCircle(x + 13, y + 10, 5, TFT_WHITE);
        t->fillRoundRect(x + 4, y + 10, 14, 6, 2, TFT_WHITE);
    } else if ((code >= 51 && code <= 67) || (code >= 80 && code <= 82)) {
        t->fillCircle(x + 7, y + 8, 4, 0x7BEF);
        t->fillRoundRect(x + 4, y + 8, 14, 5, 2, 0x7BEF);
        t->drawLine(x + 6, y + 14, x + 4, y + 19, TFT_CYAN);
        t->drawLine(x + 12, y + 14, x + 10, y + 19, TFT_CYAN);
    } else {
        t->drawFastHLine(x + 3, y + 8, 14, TFT_LIGHTGREY);
        t->drawFastHLine(x + 2, y + 11, 16, TFT_WHITE);
        t->drawFastHLine(x + 4, y + 14, 12, TFT_LIGHTGREY);
    }
}

void GUI::drawPillGauge(TFT_eSPI *t, int x, int y, int w, int h, float percent, uint16_t barColor, uint16_t bgColor) {
    if (!t) return;
    percent = constrain(percent, 0.0f, 1.0f);
    int r = h / 2;
    t->fillRoundRect(x, y, w, h, r, bgColor);
    t->drawRoundRect(x, y, w, h, r, 0x5AEB);
    int fillW = (int)(w * percent);
    if (fillW >= h) {
        t->fillRoundRect(x, y, fillW, h, r, barColor);
    } else if (fillW > 0) {
        t->fillCircle(x + r, y + r, r, barColor);
    }
}

void GUI::drawThermometerIcon(TFT_eSPI *t, int x, int y, uint16_t color) {
    if (!t) return;
    t->drawRoundRect(x + 3, y, 6, 14, 3, color);
    t->fillCircle(x + 5, y + 15, 5, color);
    t->drawCircle(x + 5, y + 15, 6, TFT_WHITE);
    t->fillCircle(x + 5, y + 15, 3, color);
    t->fillRect(x + 4, y + 6, 4, 8, color);
}

void GUI::drawHumidityIcon(TFT_eSPI *t, int x, int y, uint16_t color) {
    if (!t) return;
    t->fillTriangle(x + 6, y, x + 1, y + 10, x + 11, y + 10, color);
    t->fillCircle(x + 6, y + 11, 5, color);
    t->fillCircle(x + 4, y + 10, 2, TFT_WHITE);
}

// --- 8 Selectable High-Quality Desktop Icons ---

// 0. The Classic Spaceman / Floating Astronaut
void GUI::drawSpaceman(TFT_eSPI *t, int x, int y) {
    if (!t) return;
    // Stars
    t->drawPixel(x + 6, y + 8, TFT_WHITE);
    t->drawPixel(x + 48, y + 4, TFT_YELLOW);
    t->drawPixel(x + 72, y + 22, TFT_CYAN);
    t->drawPixel(x + 10, y + 58, TFT_WHITE);
    t->drawPixel(x + 66, y + 64, TFT_YELLOW);

    // Speed lines
    t->drawLine(x + 52, y + 6, x + 40, y + 22, 0x4208);
    t->drawLine(x + 62, y + 16, x + 48, y + 34, 0x4208);

    int ax = x + 24;
    int ay = y + 12;

    // Backpack
    t->fillRoundRect(ax - 10, ay + 6, 10, 20, 3, 0x9CD3);
    t->drawRoundRect(ax - 10, ay + 6, 10, 20, 3, TFT_WHITE);

    // Suit Torso
    t->fillRoundRect(ax - 3, ay + 10, 16, 18, 4, TFT_WHITE);
    t->fillRect(ax + 1, ay + 14, 8, 8, 0x3A86);
    t->drawPixel(ax + 3, ay + 16, TFT_RED);
    t->drawPixel(ax + 6, ay + 16, TFT_GREEN);

    // Helmet & Gold Visor
    t->fillCircle(ax + 6, ay + 4, 10, TFT_WHITE);
    t->fillRoundRect(ax + 3, ay + 1, 10, 7, 3, 0xFD20);
    t->drawPixel(ax + 5, ay + 2, TFT_WHITE);

    // Arms
    t->fillRoundRect(ax + 9, ay + 8, 12, 6, 3, TFT_WHITE);
    t->fillCircle(ax + 21, ay + 10, 3, 0x9CD3);
    t->fillRoundRect(ax - 6, ay + 14, 6, 10, 3, TFT_WHITE);
    t->fillCircle(ax - 4, ay + 24, 3, 0x9CD3);

    // Legs
    t->fillRoundRect(ax - 2, ay + 26, 7, 16, 3, TFT_WHITE);
    t->fillRoundRect(ax - 4, ay + 38, 9, 6, 2, 0x630C);
    t->fillRoundRect(ax + 7, ay + 25, 7, 14, 3, TFT_WHITE);
    t->fillRoundRect(ax + 6, ay + 36, 9, 6, 2, 0x630C);
}

// 1. Desk Cat
void GUI::drawCat(TFT_eSPI *t, int x, int y) {
    if (!t) return;
    int cx = x + 24;
    int cy = y + 16;

    // Ears
    t->fillTriangle(cx + 4, cy, cx, cy + 10, cx + 10, cy + 10, 0xFD20);
    t->fillTriangle(cx + 26, cy, cx + 20, cy + 10, cx + 30, cy + 10, 0xFD20);
    t->fillTriangle(cx + 5, cy + 3, cx + 3, cy + 9, cx + 9, cy + 9, 0xFBEF);
    t->fillTriangle(cx + 25, cy + 3, cx + 21, cy + 9, cx + 27, cy + 9, 0xFBEF);

    // Head
    t->fillRoundRect(cx, cy + 8, 30, 22, 7, 0xFD20);

    // Eyes
    t->fillCircle(cx + 8, cy + 16, 3, TFT_BLACK);
    t->fillCircle(cx + 21, cy + 16, 3, TFT_BLACK);
    t->drawPixel(cx + 7, cy + 15, TFT_WHITE);
    t->drawPixel(cx + 20, cy + 15, TFT_WHITE);

    // Whiskers & Nose
    t->fillTriangle(cx + 14, cy + 20, cx + 16, cy + 20, cx + 15, cy + 22, 0xFBEF);
    t->drawLine(cx + 2, cy + 19, cx - 4, cy + 18, TFT_WHITE);
    t->drawLine(cx + 2, cy + 22, cx - 4, cy + 24, TFT_WHITE);
    t->drawLine(cx + 28, cy + 19, cx + 34, cy + 18, TFT_WHITE);
    t->drawLine(cx + 28, cy + 22, cx + 34, cy + 24, TFT_WHITE);

    // Body
    t->fillRoundRect(cx + 2, cy + 26, 26, 22, 8, 0xFD20);
    t->fillRoundRect(cx + 8, cy + 28, 14, 16, 5, TFT_WHITE); // White chest
    t->fillCircle(cx + 8, cy + 46, 4, TFT_WHITE);
    t->fillCircle(cx + 22, cy + 46, 4, TFT_WHITE);

    // Tail
    t->drawLine(cx + 26, cy + 40, cx + 38, cy + 32, 0xFD20);
    t->drawLine(cx + 27, cy + 40, cx + 39, cy + 32, 0xFD20);
    t->fillCircle(cx + 39, cy + 31, 3, 0xFD20);
}

// 2. Saturn Planet
void GUI::drawPlanet(TFT_eSPI *t, int x, int y) {
    if (!t) return;
    int px = x + 42;
    int py = y + 36;

    // Stars
    t->drawPixel(x + 10, y + 14, TFT_WHITE);
    t->drawPixel(x + 70, y + 10, TFT_YELLOW);
    t->drawPixel(x + 65, y + 60, TFT_CYAN);
    t->drawPixel(x + 16, y + 55, TFT_WHITE);

    // Back of Ring
    t->drawEllipse(px, py, 34, 10, 0x7BEF);
    t->drawEllipse(px, py, 32, 9, 0xFD20);

    // Planet Body
    t->fillCircle(px, py, 18, 0xFD20); // Gold
    t->fillCircle(px - 4, py - 4, 16, 0xFEE0); // Highlight
    t->drawCircle(px, py, 18, 0xFD20);

    // Surface Bands
    t->drawFastHLine(px - 14, py - 4, 28, 0xDC00);
    t->drawFastHLine(px - 16, py + 2, 32, 0xDC00);
    t->drawFastHLine(px - 12, py + 8, 24, 0xDC00);

    // Front of Ring
    for (int deg = 0; deg <= 180; deg += 4) {
        float rad = deg * 0.0174533;
        int rx1 = px + cos(rad) * 34;
        int ry1 = py + sin(rad) * 10;
        t->drawPixel(rx1, ry1, 0xFD20);
        t->drawPixel(rx1, ry1 - 1, 0x7BEF);
    }

    // Tiny Orbiting Moon
    t->fillCircle(px + 28, py - 14, 3, TFT_WHITE);
}

// 3. Sci-Fi Rocket Ship
void GUI::drawRocket(TFT_eSPI *t, int x, int y) {
    if (!t) return;
    int rx = x + 38;
    int ry = y + 32;

    // Exhaust Flames
    t->fillTriangle(rx - 8, ry + 16, rx + 8, ry + 16, rx, ry + 32, TFT_RED);
    t->fillTriangle(rx - 4, ry + 16, rx + 4, ry + 16, rx, ry + 26, TFT_YELLOW);
    t->fillTriangle(rx - 2, ry + 16, rx + 2, ry + 16, rx, ry + 22, TFT_WHITE);

    // Fins
    t->fillTriangle(rx - 8, ry + 4, rx - 18, ry + 16, rx - 8, ry + 16, 0xDC00); // Red Fin
    t->fillTriangle(rx + 8, ry + 4, rx + 18, ry + 16, rx + 8, ry + 16, 0xDC00);

    // Fuselage
    t->fillRoundRect(rx - 8, ry - 14, 16, 30, 6, TFT_WHITE);
    t->fillTriangle(rx - 8, ry - 12, rx + 8, ry - 12, rx, ry - 26, 0xDC00); // Red Nose cone

    // Porthole Window
    t->fillCircle(rx, ry - 4, 5, 0x7BEF);
    t->fillCircle(rx, ry - 4, 3, 0x07FF); // Cyan Glass
    t->drawPixel(rx - 1, ry - 5, TFT_WHITE); // Glint
}

// 4. Steaming Coffee Mug
void GUI::drawCoffee(TFT_eSPI *t, int x, int y) {
    if (!t) return;
    int mx = x + 34;
    int my = y + 26;

    // Steam
    t->drawLine(mx + 4, my - 6, mx + 2, my - 12, 0xBDF7);
    t->drawLine(mx + 2, my - 12, mx + 6, my - 18, 0xBDF7);

    t->drawLine(mx + 14, my - 8, mx + 16, my - 14, 0xBDF7);
    t->drawLine(mx + 16, my - 14, mx + 12, my - 20, 0xBDF7);

    t->drawLine(mx + 24, my - 6, mx + 22, my - 12, 0xBDF7);
    t->drawLine(mx + 22, my - 12, mx + 26, my - 18, 0xBDF7);

    // Mug Body
    t->fillRoundRect(mx, my, 28, 28, 4, 0x3A86); // Blue ceramic
    t->drawRoundRect(mx, my, 28, 28, 4, TFT_WHITE);

    // Mug Handle
    t->drawRoundRect(mx + 26, my + 4, 10, 18, 4, TFT_WHITE);
    t->drawRoundRect(mx + 26, my + 6, 8, 14, 3, 0x3A86);

    // Coffee surface
    t->fillEllipse(mx + 14, my + 2, 12, 3, 0x8A22); // Brown coffee
    t->fillEllipse(mx + 14, my + 2, 8, 2, 0xD69A);  // Crema/foam
}

// 5. Retro Pacman & Ghost
void GUI::drawPacman(TFT_eSPI *t, int x, int y) {
    if (!t) return;
    int px = x + 20;
    int py = y + 36;

    // Pacman
    t->fillCircle(px, py, 14, TFT_YELLOW);
    t->fillTriangle(px, py, px + 15, py - 9, px + 15, py + 9, TFT_BLACK); // Open mouth

    // Pellets
    t->fillCircle(px + 22, py, 2, 0xFEE0);
    t->fillCircle(px + 32, py, 2, 0xFEE0);
    t->fillCircle(px + 42, py, 2, 0xFEE0);

    // Red Ghost
    int gx = x + 58;
    int gy = y + 24;
    t->fillCircle(gx + 9, gy + 9, 9, TFT_RED);
    t->fillRect(gx, gy + 9, 18, 12, TFT_RED);

    t->fillTriangle(gx, gy + 21, gx + 4, gy + 21, gx + 2, gy + 24, TFT_RED);
    t->fillTriangle(gx + 6, gy + 21, gx + 10, gy + 21, gx + 8, gy + 24, TFT_RED);
    t->fillTriangle(gx + 12, gy + 21, gx + 16, gy + 21, gx + 14, gy + 24, TFT_RED);

    // Ghost Eyes
    t->fillCircle(gx + 5, gy + 8, 3, TFT_WHITE);
    t->fillCircle(gx + 12, gy + 8, 3, TFT_WHITE);
    t->fillCircle(gx + 4, gy + 8, 1, TFT_BLUE);
    t->fillCircle(gx + 11, gy + 8, 1, TFT_BLUE);
}

// 6. Live Weather Art
void GUI::drawWeatherArt(TFT_eSPI *t, int x, int y, int weatherCode) {
    if (!t) return;
    if (weatherCode == 0 || weatherCode == 1) {
        // Bright Radiant Sun
        int sx = x + 44;
        int sy = y + 36;
        t->fillCircle(sx, sy, 18, 0xFD20);
        t->fillCircle(sx, sy, 15, TFT_YELLOW);
        t->fillCircle(sx - 3, sy - 3, 11, 0xFEE0);
        // Sun rays
        for (int a = 0; a < 360; a += 45) {
            float rad = a * 0.0174533;
            t->drawLine(sx + cos(rad)*21, sy + sin(rad)*21, sx + cos(rad)*27, sy + sin(rad)*27, TFT_YELLOW);
        }
    } else if (weatherCode >= 95) {
        // Storm Cloud + Lightning
        t->fillCircle(x + 28, y + 26, 12, 0x4208);
        t->fillCircle(x + 46, y + 20, 16, 0x630C);
        t->fillCircle(x + 60, y + 26, 12, 0x4208);
        t->fillRoundRect(x + 18, y + 24, 52, 16, 6, 0x630C);
        t->fillTriangle(x + 44, y + 34, x + 34, y + 46, x + 42, y + 46, TFT_YELLOW);
        t->fillTriangle(x + 42, y + 44, x + 32, y + 58, x + 40, y + 44, TFT_YELLOW);
    } else {
        // Soft Cloud + Rain
        t->fillCircle(x + 28, y + 26, 12, 0x7BEF);
        t->fillCircle(x + 46, y + 20, 16, TFT_WHITE);
        t->fillCircle(x + 60, y + 26, 12, 0x7BEF);
        t->fillRoundRect(x + 18, y + 24, 52, 16, 6, TFT_WHITE);
        t->drawLine(x + 28, y + 46, x + 24, y + 56, TFT_CYAN);
        t->drawLine(x + 44, y + 44, x + 40, y + 54, TFT_CYAN);
        t->drawLine(x + 60, y + 46, x + 56, y + 56, TFT_CYAN);
    }
}

// 7. Retro Game Controller
void GUI::drawGamepad(TFT_eSPI *t, int x, int y) {
    if (!t) return;
    int gx = x + 20;
    int gy = y + 24;

    // Controller Body
    t->fillRoundRect(gx, gy, 50, 28, 8, 0x4A49); // Dark grey body
    t->drawRoundRect(gx, gy, 50, 28, 8, TFT_WHITE);

    // D-Pad
    t->fillRect(gx + 8, gy + 8, 12, 4, TFT_BLACK);
    t->fillRect(gx + 12, gy + 4, 4, 12, TFT_BLACK);

    // Action Buttons
    t->fillCircle(gx + 34, gy + 14, 3, TFT_RED);
    t->fillCircle(gx + 42, gy + 10, 3, TFT_YELLOW);

    // Select / Start
    t->drawFastHLine(gx + 22, gy + 18, 3, TFT_LIGHTGREY);
    t->drawFastHLine(gx + 26, gy + 18, 3, TFT_LIGHTGREY);
}

void GUI::drawDesktopIcon(TFT_eSPI *t, int x, int y, uint8_t iconType, int weatherCode) {
    if (!t) return;
    t->fillRect(x, y, 88, 76, TFT_BLACK);

    switch (iconType) {
        case 0: drawSpaceman(t, x, y); break;
        case 1: drawCat(t, x, y); break;
        case 2: drawPlanet(t, x, y); break;
        case 3: drawRocket(t, x, y); break;
        case 4: drawCoffee(t, x, y); break;
        case 5: drawPacman(t, x, y); break;
        case 6: drawWeatherArt(t, x, y, weatherCode); break;
        case 7: drawGamepad(t, x, y); break;
        default: drawSpaceman(t, x, y); break;
    }
}

void GUI::drawScreen(ScreenState screen, const DeviceConfig &config, const WeatherData &weather) {
    if (!tft) return;

    switch (screen) {
        case SCREEN_CLOCK_WEATHER:
            drawMainScreen(config, weather);
            break;
        case SCREEN_FORECAST:
            drawForecastScreen(config, weather);
            break;
        case SCREEN_SYSTEM_INFO:
            drawSystemScreen(config, weather);
            break;
        default:
            drawMainScreen(config, weather);
            break;
    }
}

void GUI::drawMainScreen(const DeviceConfig &config, const WeatherData &weather) {
    uint16_t cBg       = hexToRGB565(config.colBg);
    uint16_t cCity     = hexToRGB565(config.colCity);
    uint16_t cBadgeBg  = hexToRGB565(config.colBadgeBg);
    uint16_t cBadgeTxt = hexToRGB565(config.colBadgeTxt);
    uint16_t cHour     = hexToRGB565(config.colHour);
    uint16_t cMin      = hexToRGB565(config.colMin);
    uint16_t cDate     = hexToRGB565(config.colDate);
    uint16_t cDay      = hexToRGB565(config.colDay);
    uint16_t cTempBar  = hexToRGB565(config.colTempBar);
    uint16_t cHumBar   = hexToRGB565(config.colHumBar);

    // 1. Clean Top Banner: City Name & Country Badge ONLY
    uint8_t cityFont = (config.labelSize == 1) ? 2 : 4;
    tft->setTextDatum(TL_DATUM);
    tft->setTextColor(cCity, cBg);
    tft->drawString(config.cityName, 14, (config.labelSize == 1 ? 12 : 8), cityFont);

    int cityWidth = tft->textWidth(config.cityName, cityFont);
    int badgeX = 14 + cityWidth + 8;
    if (badgeX > 180) badgeX = 180;
    
    tft->fillRoundRect(badgeX, (config.labelSize == 1 ? 11 : 9), 32, 18, 4, cBadgeBg);
    tft->setTextDatum(MC_DATUM);
    tft->setTextColor(cBadgeTxt, cBadgeBg);
    tft->drawString(config.countryCode, badgeX + 16, (config.labelSize == 1 ? 20 : 18), 2);

    // Top Banner Divider Line
    tft->drawFastHLine(14, 28, 212, 0x2104);

    // 2. Feels Like Temperature (Above Clock)
    String unit = config.useFahrenheit ? "F" : "C";
    char feelsBuf[32];
    if (weather.valid) {
        sprintf(feelsBuf, "Feels like %.0f`%s", weather.apparentTemp, unit.c_str());
    } else {
        sprintf(feelsBuf, "Connecting...");
    }

    tft->setTextDatum(TL_DATUM);
    tft->setTextColor(TFT_LIGHTGREY, cBg);

    if (config.feelsLikeSize == 1) {
        // ✨ In-Between (~19px FreeSans 12pt)
        tft->setFreeFont(&FreeSans12pt7b);
        tft->drawString(feelsBuf, 14, 34);
        tft->setTextFont(1);
    } else if (config.feelsLikeSize == 2) {
        // Large Bold (26px Font 4)
        tft->drawString(feelsBuf, 14, 32, 4);
    } else if (config.feelsLikeSize == 3) {
        // Compact (~12px FreeSans 9pt)
        tft->setFreeFont(&FreeSans9pt7b);
        tft->drawString(feelsBuf, 14, 36);
        tft->setTextFont(1);
    } else {
        // Standard (16px Font 2)
        tft->drawString(feelsBuf, 14, 34, 2);
    }

    // 2. Main Clock Section (With Comfortable Spacing)
    time_t now = time(nullptr);
    struct tm *timeinfo = localtime(&now);

    char hourStr[8] = "--:";
    char minStr[8]  = "--";
    char dateStr[36] = "01/01/2026";
    char dayStr[8]   = "THU";

    if (timeinfo && timeinfo->tm_year > (2020 - 1900)) {
        int h = timeinfo->tm_hour;
        if (!config.use24Hour) {
            h = h % 12;
            if (h == 0) h = 12;
            sprintf(hourStr, "%02d:", h);
        } else {
            sprintf(hourStr, "%02d:", h);
        }

        sprintf(minStr, "%02d", timeinfo->tm_min);

        if (config.dateFormat == "MM/DD/YYYY") {
            sprintf(dateStr, "%02d/%02d/%04d", timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_year + 1900);
        } else {
            sprintf(dateStr, "%02d/%02d/%04d", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900);
        }

        const char *days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
        sprintf(dayStr, "%s", days[timeinfo->tm_wday]);
    }

    uint8_t selectedFont = 7;
    uint8_t fontScale = 1;
    int clockY = 62; // Added breathing room below Feels Like text

    if (config.clockFontStyle == 1) {
        selectedFont = 6;
        clockY = 62;
    } else if (config.clockFontStyle == 2) {
        selectedFont = 4;
        fontScale = (config.clockSize == 2) ? 1 : 2;
        clockY = 60;
    } else if (config.clockFontStyle == 3) {
        selectedFont = 4;
        fontScale = 1;
        clockY = 66;
    }

    tft->setTextDatum(TL_DATUM);
    tft->setTextSize(fontScale);

    tft->setTextColor(cHour, cBg);
    tft->drawString(hourStr, 14, clockY, selectedFont);

    int hourW = tft->textWidth(hourStr, selectedFont) * fontScale;
    tft->setTextColor(cMin, cBg);
    tft->drawString(minStr, 14 + hourW, clockY, selectedFont);

    tft->setTextSize(1);

    // 3. Shifted Down Weather Orb & Condition Text Badge
    int orbX = 194;
    int orbY = 68;

    if (weather.weatherCode == 0 || weather.weatherCode == 1) {
        tft->fillCircle(orbX, orbY, 15, 0xFD20);
        tft->fillCircle(orbX, orbY, 12, TFT_YELLOW);
    } else if (weather.weatherCode == 2 || weather.weatherCode == 3) {
        tft->fillCircle(orbX + 6, orbY - 6, 10, 0xFD20);
        tft->fillCircle(orbX - 6, orbY + 2, 9, 0xBDF7);
        tft->fillRoundRect(orbX - 14, orbY, 24, 10, 4, TFT_WHITE);
    } else if (weather.weatherCode >= 51 && weather.weatherCode <= 67) {
        tft->fillCircle(orbX - 6, orbY - 4, 10, 0x7BEF);
        tft->fillRoundRect(orbX - 16, orbY - 4, 26, 10, 4, 0x7BEF);
        tft->drawLine(orbX - 10, orbY + 8, orbX - 14, orbY + 14, TFT_CYAN);
        tft->drawLine(orbX - 2, orbY + 8, orbX - 6, orbY + 14, TFT_CYAN);
        tft->drawLine(orbX + 6, orbY + 8, orbX + 2, orbY + 14, TFT_CYAN);
    } else {
        tft->fillCircle(orbX - 4, orbY - 2, 11, 0x9CD3);
        tft->fillRoundRect(orbX - 16, orbY - 4, 28, 11, 4, TFT_WHITE);
    }

    // Condition Text Badge (Shifted Down below the Orb)
    String cond = weather.valid ? weather.conditionText : "Clear";
    if (cond.length() > 7) cond = cond.substring(0, 7);
    tft->fillRoundRect(164, 92, 62, 20, 4, TFT_WHITE);
    tft->setTextDatum(MC_DATUM);
    tft->setTextColor(0x000F, TFT_WHITE);
    tft->drawString(cond, 195, 102, 2);

    // 4. Date & Day Section (Matching Feels Like font size)
    tft->setTextDatum(TL_DATUM);

    if (config.feelsLikeSize == 1) {
        // In-Between (~19px FreeSans 12pt)
        tft->setFreeFont(&FreeSans12pt7b);
        tft->setTextColor(cDate, cBg);
        tft->drawString(dateStr, 14, 122);
        tft->setTextColor(cDay, cBg);
        tft->drawString(dayStr, 150, 122);
        tft->setTextFont(1);
    } else if (config.feelsLikeSize == 2) {
        // Large Bold (Font 4)
        tft->setTextColor(cDate, cBg);
        tft->drawString(dateStr, 14, 120, 4);
        tft->setTextColor(cDay, cBg);
        tft->drawString(dayStr, 150, 120, 4);
    } else if (config.feelsLikeSize == 3) {
        // Compact (FreeSans 9pt)
        tft->setFreeFont(&FreeSans9pt7b);
        tft->setTextColor(cDate, cBg);
        tft->drawString(dateStr, 14, 124);
        tft->setTextColor(cDay, cBg);
        tft->drawString(dayStr, 150, 124);
        tft->setTextFont(1);
    } else {
        // Standard (16px Font 2)
        tft->setTextColor(cDate, cBg);
        tft->drawString(dateStr, 14, 124, 2);
        tft->setTextColor(cDay, cBg);
        tft->drawString(dayStr, 150, 124, 2);
    }

    tft->drawFastHLine(14, 148, 212, 0x2104);

    // 5. Bottom Section: Gauges & Selected Static Icon
    drawThermometerIcon(tft, 14, 158, cTempBar);

    float tempVal = weather.valid ? weather.currentTemp : 20.0f;
    float tempPercent = config.useFahrenheit ? (tempVal - 20.0f) / 80.0f : (tempVal / 40.0f);
    drawPillGauge(tft, 32, 162, 48, 8, tempPercent, cTempBar, 0x18C3);

    char tBuf[16];
    sprintf(tBuf, "%.0f`%s", tempVal, unit.c_str());
    tft->setTextDatum(ML_DATUM);
    tft->setTextColor(TFT_WHITE, cBg);
    tft->drawString(tBuf, 86, 166, 2);

    drawHumidityIcon(tft, 14, 190, cHumBar);

    float humVal = weather.valid ? weather.humidity : 50.0f;
    float humPercent = humVal / 100.0f;
    drawPillGauge(tft, 32, 194, 48, 8, humPercent, cHumBar, 0x18C3);

    char hBuf[16];
    sprintf(hBuf, "%.0f%%", humVal);
    tft->setTextDatum(ML_DATUM);
    tft->setTextColor(TFT_WHITE, cBg);
    tft->drawString(hBuf, 86, 198, 2);

    // Selected Desktop Icon (Spaceman, Desk Cat, Saturn, etc.)
    drawDesktopIcon(tft, 146, 154, config.staticIcon, weather.weatherCode);
}

void GUI::drawForecastScreen(const DeviceConfig &config, const WeatherData &weather) {
    uint16_t cBg   = hexToRGB565(config.colBg);
    uint16_t cCity = hexToRGB565(config.colCity);

    tft->fillScreen(cBg);

    tft->fillRect(0, 0, 240, 28, 0x0841);
    tft->setTextDatum(MC_DATUM);
    tft->setTextColor(cCity, 0x0841);
    tft->drawString("5-DAY FORECAST (" + config.cityName + ")", 120, 14, 2);

    if (!weather.valid) {
        tft->setTextColor(TFT_YELLOW, cBg);
        tft->drawString("No forecast data yet", 120, 120, 2);
        return;
    }

    String unit = config.useFahrenheit ? "F" : "C";

    for (int i = 0; i < 5; i++) {
        int y = 34 + (i * 40);
        uint16_t cardBg = (i % 2 == 0) ? 0x1082 : 0x18E3;
        tft->fillRoundRect(6, y, 228, 36, 6, cardBg);

        tft->setTextDatum(ML_DATUM);
        tft->setTextColor((i == 0) ? hexToRGB565(config.colMin) : TFT_WHITE, cardBg);
        tft->drawString(weather.forecast[i].dayName, 14, y + 18, 2);

        drawWeatherIcon(tft, 75, y + 6, weather.forecast[i].weatherCode, true);

        String cond = WeatherService::getWeatherConditionText(weather.forecast[i].weatherCode);
        if (cond.length() > 9) cond = cond.substring(0, 8) + ".";
        tft->setTextColor(TFT_LIGHTGREY, cardBg);
        tft->drawString(cond, 105, y + 18, 1);

        char tempStr[24];
        sprintf(tempStr, "%.0f` / %.0f`%s", weather.forecast[i].tempMin, weather.forecast[i].tempMax, unit.c_str());
        tft->setTextDatum(MR_DATUM);
        tft->setTextColor(hexToRGB565(config.colTempBar), cardBg);
        tft->drawString(tempStr, 224, y + 18, 2);
    }
}

void GUI::drawSystemScreen(const DeviceConfig &config, const WeatherData &weather) {
    uint16_t cBg = hexToRGB565(config.colBg);
    tft->fillScreen(cBg);

    tft->fillRect(0, 0, 240, 28, 0x0841);
    tft->setTextDatum(MC_DATUM);
    tft->setTextColor(TFT_CYAN, 0x0841);
    tft->drawString("SYSTEM & NETWORK", 120, 14, 2);

    tft->fillRoundRect(8, 34, 224, 66, 6, 0x18C3);
    tft->setTextDatum(TL_DATUM);
    tft->setTextColor(TFT_ORANGE, 0x18C3);
    tft->drawString("WI-FI CONNECTION", 16, 40, 1);
    tft->setTextColor(TFT_WHITE, 0x18C3);
    tft->drawString("SSID: " + WiFi.SSID(), 16, 54, 2);
    tft->drawString("IP: " + WiFi.localIP().toString(), 16, 72, 2);

    tft->fillRoundRect(8, 106, 224, 62, 6, 0x2104);
    tft->setTextColor(TFT_CYAN, 0x2104);
    tft->drawString("WEB PORTAL & OTA UPDATE", 16, 112, 1);
    tft->setTextColor(TFT_YELLOW, 0x2104);
    tft->drawString("http://" + WiFi.localIP().toString() + "/", 16, 126, 2);
    tft->setTextColor(TFT_LIGHTGREY, 0x2104);
    tft->drawString("Settings & Color Customization", 16, 146, 1);

    tft->fillRoundRect(8, 174, 224, 58, 6, 0x10A2);
    tft->setTextColor(TFT_LIGHTGREY, 0x10A2);
    tft->drawString("DIAGNOSTICS", 16, 180, 1);
    unsigned long sec = millis() / 1000;
    char upBuf[48];
    sprintf(upBuf, "Uptime: %luh %lum | RAM: %dKB", sec / 3600, (sec % 3600) / 60, ESP.getFreeHeap() / 1024);
    tft->setTextColor(TFT_WHITE, 0x10A2);
    tft->drawString(upBuf, 16, 194, 1);
    tft->setTextColor(0x07FF, 0x10A2);
    tft->drawString("Weather Sync: " + weather.lastSyncTime, 16, 210, 1);
}
