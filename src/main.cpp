#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WiFiManager.h>
#include <TFT_eSPI.h>
#include <LittleFS.h>
#include <time.h>

#include "config.h"
#include "storage.h"
#include "weather.h"
#include "gui.h"

// Hardware & System Instances
TFT_eSPI tft = TFT_eSPI();
ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;
WiFiManager wifiManager;

// State
DeviceConfig config;
WeatherData weather;
ScreenState currentScreen = SCREEN_CLOCK_WEATHER;

unsigned long lastWeatherFetch = 0;
const unsigned long WEATHER_FETCH_INTERVAL = 15 * 60 * 1000; // 15 mins

unsigned long lastClockDraw = 0;
unsigned long lastScreenCycle = 0;

bool buttonPressed = false;
unsigned long buttonPressStart = 0;

// Function Declarations
void setupWebServer();
void updateNTP();
void checkButton();
void handleRoot();
void handleSaveSettings();
void handleRestart();

void setup() {
    Serial.begin(115200);
    Serial.println("\n\n--- SmallTV-Ultra Weather & Clock Studio ---");

    // Initialize Storage
    Storage::begin();
    Storage::loadConfig(config);

    // Initialize Pins
    pinMode(PIN_BL, OUTPUT);
    pinMode(PIN_BTN, INPUT_PULLUP);
    pinMode(PIN_LED, OUTPUT);
    digitalWrite(PIN_LED, HIGH);

    // Backlight initially low
    analogWrite(PIN_BL, 50);

    // Initialize Display & GUI
    tft.init();
    tft.setRotation(config.rotation);
    tft.fillScreen(hexToRGB565(config.colBg));
    GUI::init(&tft);

    // Ramp up backlight
    for (int b = 50; b <= config.brightness; b += 10) {
        analogWrite(PIN_BL, b);
        delay(10);
    }

    // Splash Screen
    tft.fillRect(0, 0, 240, 240, 0x0841);
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_CYAN, 0x0841);
    tft.drawString("SmallTV Ultra", 120, 80, 4);
    tft.setTextColor(TFT_WHITE, 0x0841);
    tft.drawString("Connecting to Wi-Fi...", 120, 130, 2);

    // Wi-Fi Setup
    wifiManager.setConfigPortalTimeout(180);
    wifiManager.setAPCallback([](WiFiManager *myWiFiManager) {
        tft.fillScreen(TFT_BLACK);
        tft.setTextDatum(TC_DATUM);
        tft.setTextColor(TFT_YELLOW, TFT_BLACK);
        tft.drawString("Wi-Fi Setup Mode", 120, 40, 4);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.drawString("Connect to Wi-Fi AP:", 120, 90, 2);
        tft.setTextColor(TFT_CYAN, TFT_BLACK);
        tft.drawString(AP_SSID, 120, 120, 4);
        tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
        tft.drawString("http://192.168.4.1", 120, 165, 2);
    });

    bool connected = wifiManager.autoConnect(AP_SSID);
    if (!connected) {
        Serial.println("Wi-Fi timeout, restarting...");
        ESP.restart();
    }

    Serial.println("Wi-Fi Connected! IP: " + WiFi.localIP().toString());

    // Sync NTP Time
    updateNTP();

    // Start Web Server & OTA Updater
    setupWebServer();

    // Initial Weather Fetch
    tft.drawString("Fetching Weather Data...", 120, 180, 2);
    WeatherService::fetchWeather(config, weather);
    lastWeatherFetch = millis();

    // Render Initial Screen
    tft.fillScreen(hexToRGB565(config.colBg));
    GUI::drawScreen(currentScreen, config, weather);
}

void loop() {
    server.handleClient();
    checkButton();

    // Periodic Weather Fetch
    if (millis() - lastWeatherFetch > WEATHER_FETCH_INTERVAL) {
        lastWeatherFetch = millis();
        WeatherService::fetchWeather(config, weather);
        GUI::drawScreen(currentScreen, config, weather);
    }

    // Update screen display clock every 500ms
    if (millis() - lastClockDraw >= 500) {
        lastClockDraw = millis();
        if (currentScreen == SCREEN_CLOCK_WEATHER) {
            GUI::drawScreen(currentScreen, config, weather);
        }
    }

    // Auto screen cycling (if enabled)
    if (config.autoCycleScreens && (millis() - lastScreenCycle > (unsigned long)config.cycleIntervalSec * 1000)) {
        lastScreenCycle = millis();
        currentScreen = (ScreenState)((currentScreen + 1) % SCREEN_COUNT);
        tft.fillScreen(hexToRGB565(config.colBg));
        GUI::drawScreen(currentScreen, config, weather);
    }
}

void updateNTP() {
    Serial.println("Configuring NTP with timezone: " + config.timezone);
    configTime(config.timezone.c_str(), "pool.ntp.org", "time.nist.gov", "time.google.com");
}

void checkButton() {
    int btnState = digitalRead(PIN_BTN);
    if (btnState == LOW) {
        if (!buttonPressed) {
            buttonPressed = true;
            buttonPressStart = millis();
        }
    } else {
        if (buttonPressed) {
            unsigned long duration = millis() - buttonPressStart;
            buttonPressed = false;

            if (duration > 50 && duration < 800) {
                // Short Tap: Switch Screen
                currentScreen = (ScreenState)((currentScreen + 1) % SCREEN_COUNT);
                tft.fillScreen(hexToRGB565(config.colBg));
                GUI::drawScreen(currentScreen, config, weather);
                lastScreenCycle = millis();
                Serial.printf("Switched to screen: %d\n", currentScreen);
            } else if (duration >= 800) {
                // Long Press (>800ms): Cycle brightness
                if (config.brightness == 200) config.brightness = 255;
                else if (config.brightness == 255) config.brightness = 80;
                else if (config.brightness == 80) config.brightness = 150;
                else config.brightness = 200;

                analogWrite(PIN_BL, config.brightness);
                Storage::saveConfig(config);
                Serial.printf("Brightness changed to: %d\n", config.brightness);
            }
        }
    }
}

// --- Web Server Handlers ---

void setupWebServer() {
    server.on("/", HTTP_GET, handleRoot);
    server.on("/save-settings", HTTP_POST, handleSaveSettings);
    server.on("/restart", HTTP_POST, handleRestart);

    // OTA Update endpoint
    httpUpdater.setup(&server, "/update");

    server.begin();
    Serial.println("HTTP Server started on port 80");
}

void handleRoot() {
    String html = "<!DOCTYPE html><html><head><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'>";
    html += "<title>SmallTV Ultra Studio</title>";
    html += "<style>";
    html += "body{font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',Roboto,sans-serif;background:#121212;color:#eee;margin:0;padding:16px;}";
    html += ".container{max-width:580px;margin:auto;background:#1e1e1e;border-radius:14px;padding:24px;box-shadow:0 8px 30px rgba(0,0,0,0.6);}";
    html += "h1{color:#00d2ff;margin:0 0 16px 0;font-size:24px;display:flex;align-items:center;gap:8px;}";
    html += ".card{background:#262626;border-radius:10px;padding:18px;margin-bottom:18px;}";
    html += "h3{margin-top:0;color:#00d2ff;font-size:16px;border-bottom:1px solid #333;padding-bottom:8px;}";
    html += "label{display:block;margin:10px 0 4px 0;font-size:13px;color:#aaa;font-weight:600;}";
    html += "input[type='text'],input[type='number'],select{width:100%;box-sizing:border-box;padding:10px;background:#141414;border:1px solid #444;border-radius:6px;color:#fff;font-size:14px;}";
    html += ".color-row{display:flex;align-items:center;justify-content:space-between;padding:6px 0;border-bottom:1px solid #333;}";
    html += ".color-row input[type='color']{border:none;width:40px;height:32px;border-radius:4px;cursor:pointer;background:none;}";
    html += ".color-label{font-size:14px;color:#ddd;}";
    html += ".checkbox-row{display:flex;align-items:center;margin:14px 0;}";
    html += "input[type='checkbox']{transform:scale(1.3);margin-right:8px;}";
    html += ".btn{display:inline-block;padding:12px 20px;border:none;border-radius:8px;font-size:14px;cursor:pointer;text-decoration:none;font-weight:600;}";
    html += ".btn-primary{background:#00d2ff;color:#121212;width:100%;margin-top:14px;}";
    html += ".btn-preset{background:#333;color:#fff;font-size:12px;padding:8px 12px;border-radius:6px;margin:4px 2px;display:inline-block;border:1px solid #555;cursor:pointer;}";
    html += ".btn-danger{background:#ff4757;color:#fff;width:100%;}";
    html += ".grid2{display:grid;grid-template-columns:1fr 1fr;gap:12px;}";
    html += ".nav-links{display:flex;justify-content:space-between;margin-top:16px;}";
    html += ".nav-links a{color:#00d2ff;text-decoration:none;font-size:13px;}";
    html += "</style></head><body>";

    html += "<div class='container'>";
    html += "<h1>📺 SmallTV-Ultra Studio</h1>";

    html += "<form method='POST' action='/save-settings'>";

    // Desktop Icon Card
    html += "<div class='card'>";
    html += "<h3>🖼️ Desktop Graphic Icon</h3>";
    html += "<label>Select Corner Icon:</label>";
    html += "<select name='staticIcon'>";
    html += "<option value='0'" + String(config.staticIcon == 0 ? " selected" : "") + ">🧑‍🚀 Floating Astronaut (Spaceman)</option>";
    html += "<option value='1'" + String(config.staticIcon == 1 ? " selected" : "") + ">🐱 Orange Desk Cat (Pixel Pet)</option>";
    html += "<option value='2'" + String(config.staticIcon == 2 ? " selected" : "") + ">🪐 Cosmic Saturn (Ringed Planet)</option>";
    html += "<option value='3'" + String(config.staticIcon == 3 ? " selected" : "") + ">🚀 Sci-Fi Rocket Ship</option>";
    html += "<option value='4'" + String(config.staticIcon == 4 ? " selected" : "") + ">☕ Steaming Coffee Mug</option>";
    html += "<option value='5'" + String(config.staticIcon == 5 ? " selected" : "") + ">👾 Retro Arcade Pacman & Ghost</option>";
    html += "<option value='6'" + String(config.staticIcon == 6 ? " selected" : "") + ">🌦️ Dynamic Live Weather Art</option>";
    html += "<option value='7'" + String(config.staticIcon == 7 ? " selected" : "") + ">🎮 Retro Game Controller</option>";
    html += "</select>";
    html += "</div>";

    // Theme & Colors Card
    html += "<div class='card'>";
    html += "<h3>🎨 UI Theme & Colors</h3>";
    html += "<p style='font-size:12px;color:#aaa;margin-top:0;'>Click a 1-click theme preset or customize individual elements:</p>";
    html += "<div>";
    html += "<button type='button' class='btn-preset' onclick='applyPreset(\"#FFB800\",\"#39FF14\",\"#000000\",\"#FFFFFF\",\"#FFDE00\",\"#FFFFFF\",\"#FFFFFF\",\"#00E676\",\"#3A86FF\",\"#FFD166\",\"#000000\")'>🌟 Cyberpunk Gold</button>";
    html += "<button type='button' class='btn-preset' onclick='applyPreset(\"#FF007F\",\"#8338EC\",\"#FFFFFF\",\"#00F0FF\",\"#FF007F\",\"#00F0FF\",\"#FFFFFF\",\"#FF007F\",\"#FF007F\",\"#00F0FF\",\"#000000\")'>🌆 Synthwave Neon</button>";
    html += "<button type='button' class='btn-preset' onclick='applyPreset(\"#00FF66\",\"#00FF66\",\"#000000\",\"#00FF66\",\"#00FF66\",\"#00FF66\",\"#00FF66\",\"#00FF66\",\"#00FF66\",\"#00FF66\",\"#000000\")'>🟢 Matrix Green</button>";
    html += "<button type='button' class='btn-preset' onclick='applyPreset(\"#00D2FF\",\"#3A86FF\",\"#FFFFFF\",\"#FFFFFF\",\"#00D2FF\",\"#00D2FF\",\"#FFFFFF\",\"#00D2FF\",\"#00D2FF\",\"#FFFFFF\",\"#0A0E1A\")'>❄️ Ice Glacier</button>";
    html += "<button type='button' class='btn-preset' onclick='applyPreset(\"#FF7B00\",\"#FFB703\",\"#000000\",\"#FFFFFF\",\"#FFB703\",\"#FFFFFF\",\"#FFFFFF\",\"#FF7B00\",\"#FF7B00\",\"#FFB703\",\"#140800\")'>🌅 Sunset Amber</button>";
    html += "</div>";
    html += "<br>";

    // Color Pickers
    html += "<div class='color-row'><span class='color-label'>City Name:</span><input type='color' id='colCity' name='colCity' value='" + config.colCity + "'></div>";
    html += "<div class='color-row'><span class='color-label'>Country Badge Background:</span><input type='color' id='colBadgeBg' name='colBadgeBg' value='" + config.colBadgeBg + "'></div>";
    html += "<div class='color-row'><span class='color-label'>Country Badge Text:</span><input type='color' id='colBadgeTxt' name='colBadgeTxt' value='" + config.colBadgeTxt + "'></div>";
    html += "<div class='color-row'><span class='color-label'>Hours Digits:</span><input type='color' id='colHour' name='colHour' value='" + config.colHour + "'></div>";
    html += "<div class='color-row'><span class='color-label'>Minutes Digits:</span><input type='color' id='colMin' name='colMin' value='" + config.colMin + "'></div>";
    html += "<div class='color-row'><span class='color-label'>Seconds Digits:</span><input type='color' id='colSec' name='colSec' value='" + config.colSec + "'></div>";
    html += "<div class='color-row'><span class='color-label'>Date Text:</span><input type='color' id='colDate' name='colDate' value='" + config.colDate + "'></div>";
    html += "<div class='color-row'><span class='color-label'>Day of Week:</span><input type='color' id='colDay' name='colDay' value='" + config.colDay + "'></div>";
    html += "<div class='color-row'><span class='color-label'>Temperature Gauge & Icon:</span><input type='color' id='colTempBar' name='colTempBar' value='" + config.colTempBar + "'></div>";
    html += "<div class='color-row'><span class='color-label'>Humidity Gauge & Icon:</span><input type='color' id='colHumBar' name='colHumBar' value='" + config.colHumBar + "'></div>";
    html += "<div class='color-row'><span class='color-label'>Screen Background:</span><input type='color' id='colBg' name='colBg' value='" + config.colBg + "'></div>";
    html += "</div>";

    // Typography & Font Style Card
    html += "<div class='card'>";
    html += "<h3>🔤 Typography & Font Options</h3>";
    html += "<label>Clock Font Style:</label>";
    html += "<select name='fontStyle'>";
    html += "<option value='0'" + String(config.clockFontStyle == 0 ? " selected" : "") + ">📟 7-Segment Digital (Retro Alarm)</option>";
    html += "<option value='1'" + String(config.clockFontStyle == 1 ? " selected" : "") + ">📱 Modern Bold Rounded (Clean Digits)</option>";
    html += "<option value='2'" + String(config.clockFontStyle == 2 ? " selected" : "") + ">🤖 Cyberpunk Tech / Monospace</option>";
    html += "<option value='3'" + String(config.clockFontStyle == 3 ? " selected" : "") + ">✨ Minimalist Clean Sans</option>";
    html += "</select>";

    html += "<div class='grid2'>";
    html += "<div><label>Clock Size Scale:</label>";
    html += "<select name='clockSize'>";
    html += "<option value='0'" + String(config.clockSize == 0 ? " selected" : "") + ">Standard Fit</option>";
    html += "<option value='1'" + String(config.clockSize == 1 ? " selected" : "") + ">Large / Bold</option>";
    html += "<option value='2'" + String(config.clockSize == 2 ? " selected" : "") + ">Compact</option>";
    html += "</select></div>";

    html += "<div><label>City Label Size:</label>";
    html += "<select name='labelSize'>";
    html += "<option value='0'" + String(config.labelSize == 0 ? " selected" : "") + ">Medium (24px)</option>";
    html += "<option value='1'" + String(config.labelSize == 1 ? " selected" : "") + ">Small (16px)</option>";
    html += "</select></div>";
    html += "</div>";

    html += "<div class='grid2'>";
    html += "<div><label>Feels Like Size:</label>";
    html += "<select name='feelsLikeSize'>";
    html += "<option value='1'" + String(config.feelsLikeSize == 1 ? " selected" : "") + ">✨ In-Between (19px)</option>";
    html += "<option value='0'" + String(config.feelsLikeSize == 0 ? " selected" : "") + ">Standard (16px)</option>";
    html += "<option value='2'" + String(config.feelsLikeSize == 2 ? " selected" : "") + ">Large Bold (26px)</option>";
    html += "<option value='3'" + String(config.feelsLikeSize == 3 ? " selected" : "") + ">Compact (12px)</option>";
    html += "</select></div>";
    html += "</div>";
    html += "</div>";

    // Location & Weather Card
    html += "<div class='card'>";
    html += "<h3>🌤️ Location & Weather</h3>";
    html += "<div class='grid2'>";
    html += "<div><label>City Name:</label><input type='text' id='city' name='city' value='" + config.cityName + "' required></div>";
    html += "<div><label>Country/State Badge:</label><input type='text' id='country' name='country' maxlength='3' value='" + config.countryCode + "' required></div>";
    html += "</div>";

    html += "<div class='grid2'>";
    html += "<div><label>Latitude:</label><input type='text' id='lat' name='lat' value='" + String(config.latitude, 4) + "' required></div>";
    html += "<div><label>Longitude:</label><input type='text' id='lon' name='lon' value='" + String(config.longitude, 4) + "' required></div>";
    html += "</div>";

    html += "<button type='button' class='btn-preset' style='margin-top:8px;' onclick='lookupGPS()'>📍 Auto-Lookup GPS & Country Code</button>";

    html += "<label>Temperature Unit:</label>";
    html += "<select name='fahrenheit'>";
    html += "<option value='1'" + String(config.useFahrenheit ? " selected" : "") + ">Fahrenheit (°F, mph)</option>";
    html += "<option value='0'" + String(!config.useFahrenheit ? " selected" : "") + ">Celsius (°C, km/h)</option>";
    html += "</select>";
    html += "</div>";

    // Clock & Date Settings
    html += "<div class='card'>";
    html += "<h3>⏰ Clock & Timezone</h3>";
    html += "<label>Timezone (POSIX):</label>";
    html += "<select name='tz'>";
    html += "<option value='PST8PDT,M3.2.0,M11.1.0'" + String(config.timezone == "PST8PDT,M3.2.0,M11.1.0" ? " selected" : "") + ">US Pacific (PST/PDT)</option>";
    html += "<option value='MST7MDT,M3.2.0,M11.1.0'" + String(config.timezone == "MST7MDT,M3.2.0,M11.1.0" ? " selected" : "") + ">US Mountain (MST/MDT)</option>";
    html += "<option value='CST6CDT,M3.2.0,M11.1.0'" + String(config.timezone == "CST6CDT,M3.2.0,M11.1.0" ? " selected" : "") + ">US Central (CST/CDT)</option>";
    html += "<option value='EST5EDT,M3.2.0,M11.1.0'" + String(config.timezone == "EST5EDT,M3.2.0,M11.1.0" ? " selected" : "") + ">US Eastern (EST/EDT)</option>";
    html += "<option value='GMT0BST,M3.5.0/1,M10.5.0'" + String(config.timezone.startsWith("GMT0BST") ? " selected" : "") + ">London (GMT/BST)</option>";
    html += "<option value='CET-1CEST,M3.5.0,M10.5.0/3'" + String(config.timezone.startsWith("CET-1CEST") ? " selected" : "") + ">Central Europe (CET/CEST)</option>";
    html += "<option value='JST-9'" + String(config.timezone == "JST-9" ? " selected" : "") + ">Seoul / Tokyo (KST/JST)</option>";
    html += "<option value='AEST-10AEDT,M10.1.0,M4.1.0/3'" + String(config.timezone.startsWith("AEST-10") ? " selected" : "") + ">Sydney (AEST/AEDT)</option>";
    html += "</select>";

    html += "<label>Date Format:</label>";
    html += "<select name='dateFormat'>";
    html += "<option value='DD/MM/YYYY'" + String(config.dateFormat == "DD/MM/YYYY" ? " selected" : "") + ">DD/MM/YYYY (e.g. 20/10/2026)</option>";
    html += "<option value='MM/DD/YYYY'" + String(config.dateFormat == "MM/DD/YYYY" ? " selected" : "") + ">MM/DD/YYYY (e.g. 10/20/2026)</option>";
    html += "</select>";

    html += "<div class='checkbox-row'>";
    html += "<input type='checkbox' id='use24h' name='use24h' value='1'" + String(config.use24Hour ? " checked" : "") + ">";
    html += "<label for='use24h' style='margin:0;'>Use 24-Hour Clock format</label>";
    html += "</div>";
    html += "</div>";

    // Display & Hardware
    html += "<div class='card'>";
    html += "<h3>🖥️ Display Settings</h3>";
    html += "<label>Screen Brightness (10 - 255):</label>";
    html += "<input type='number' min='10' max='255' name='brightness' value='" + String(config.brightness) + "'>";

    html += "<label>Screen Rotation:</label>";
    html += "<select name='rotation'>";
    html += "<option value='0'" + String(config.rotation == 0 ? " selected" : "") + ">Standard (0°)</option>";
    html += "<option value='2'" + String(config.rotation == 2 ? " selected" : "") + ">Flipped (180°)</option>";
    html += "</select>";

    html += "<div class='checkbox-row'>";
    html += "<input type='checkbox' id='autoCycle' name='autoCycle' value='1'" + String(config.autoCycleScreens ? " checked" : "") + ">";
    html += "<label for='autoCycle' style='margin:0;'>Auto-cycle screens</label>";
    html += "</div>";

    html += "<label>Cycle interval (seconds):</label>";
    html += "<input type='number' min='5' max='300' name='cycleInterval' value='" + String(config.cycleIntervalSec) + "'>";
    html += "</div>";

    html += "<button type='submit' class='btn btn-primary'>💾 Save & Apply Settings</button>";
    html += "</form>";

    // OTA Link & Reboot
    html += "<div class='nav-links'>";
    html += "<a href='/update'>🚀 Firmware Upgrade (OTA)</a>";
    html += "</div>";

    html += "<div style='margin-top:20px;'>";
    html += "<form method='POST' action='/restart' onsubmit='return confirm(\"Restart device?\");'>";
    html += "<button class='btn btn-danger' type='submit'>🔄 Restart SmallTV</button>";
    html += "</form>";
    html += "</div>";

    // JavaScript for Theme Presets & Geocoding
    html += "<script>";
    html += "function applyPreset(city, bBg, bTxt, hr, min, sec, date, day, tBar, hBar, bg){";
    html += "  document.getElementById('colCity').value = city;";
    html += "  document.getElementById('colBadgeBg').value = bBg;";
    html += "  document.getElementById('colBadgeTxt').value = bTxt;";
    html += "  document.getElementById('colHour').value = hr;";
    html += "  document.getElementById('colMin').value = min;";
    html += "  document.getElementById('colSec').value = sec;";
    html += "  document.getElementById('colDate').value = date;";
    html += "  document.getElementById('colDay').value = day;";
    html += "  document.getElementById('colTempBar').value = tBar;";
    html += "  document.getElementById('colHumBar').value = hBar;";
    html += "  document.getElementById('colBg').value = bg;";
    html += "}";
    html += "function lookupGPS(){";
    html += "  const city = document.getElementById('city').value;";
    html += "  if(!city){alert('Please enter a city name first');return;}";
    html += "  fetch('https://geocoding-api.open-meteo.com/v1/search?name=' + encodeURIComponent(city) + '&count=1')";
    html += "  .then(r => r.json())";
    html += "  .then(data => {";
    html += "    if(data.results && data.results.length > 0){";
    html += "      const res = data.results[0];";
    html += "      document.getElementById('lat').value = res.latitude.toFixed(4);";
    html += "      document.getElementById('lon').value = res.longitude.toFixed(4);";
    html += "      document.getElementById('city').value = res.name;";
    html += "      document.getElementById('country').value = (res.country_code || 'US').toUpperCase();";
    html += "      alert('Found: ' + res.name + ' (' + (res.country_code || '').toUpperCase() + ')');";
    html += "    } else { alert('City not found. Please enter coordinates manually.'); }";
    html += "  }).catch(e => alert('Geocoding error: ' + e));";
    html += "}";
    html += "</script>";

    html += "</div></body></html>";

    server.send(200, "text/html", html);
}

void handleSaveSettings() {
    if (server.hasArg("staticIcon")) config.staticIcon = server.arg("staticIcon").toInt();
    if (server.hasArg("city")) config.cityName = server.arg("city");
    if (server.hasArg("country")) config.countryCode = server.arg("country");
    if (server.hasArg("lat")) config.latitude = server.arg("lat").toFloat();
    if (server.hasArg("lon")) config.longitude = server.arg("lon").toFloat();
    if (server.hasArg("tz")) config.timezone = server.arg("tz");
    if (server.hasArg("fahrenheit")) config.useFahrenheit = (server.arg("fahrenheit") == "1");
    config.use24Hour = server.hasArg("use24h");
    if (server.hasArg("dateFormat")) config.dateFormat = server.arg("dateFormat");

    // Typography
    if (server.hasArg("fontStyle")) config.clockFontStyle = server.arg("fontStyle").toInt();
    if (server.hasArg("clockSize")) config.clockSize = server.arg("clockSize").toInt();
    if (server.hasArg("labelSize")) config.labelSize = server.arg("labelSize").toInt();
    if (server.hasArg("feelsLikeSize")) config.feelsLikeSize = server.arg("feelsLikeSize").toInt();

    // Colors
    if (server.hasArg("colCity")) config.colCity = server.arg("colCity");
    if (server.hasArg("colBadgeBg")) config.colBadgeBg = server.arg("colBadgeBg");
    if (server.hasArg("colBadgeTxt")) config.colBadgeTxt = server.arg("colBadgeTxt");
    if (server.hasArg("colHour")) config.colHour = server.arg("colHour");
    if (server.hasArg("colMin")) config.colMin = server.arg("colMin");
    if (server.hasArg("colSec")) config.colSec = server.arg("colSec");
    if (server.hasArg("colDate")) config.colDate = server.arg("colDate");
    if (server.hasArg("colDay")) config.colDay = server.arg("colDay");
    if (server.hasArg("colTempBar")) config.colTempBar = server.arg("colTempBar");
    if (server.hasArg("colHumBar")) config.colHumBar = server.arg("colHumBar");
    if (server.hasArg("colBg")) config.colBg = server.arg("colBg");

    if (server.hasArg("brightness")) {
        config.brightness = constrain(server.arg("brightness").toInt(), 10, 255);
        analogWrite(PIN_BL, config.brightness);
    }
    if (server.hasArg("rotation")) {
        config.rotation = server.arg("rotation").toInt();
        tft.setRotation(config.rotation);
    }
    config.autoCycleScreens = server.hasArg("autoCycle");
    if (server.hasArg("cycleInterval")) config.cycleIntervalSec = constrain(server.arg("cycleInterval").toInt(), 5, 300);

    // Save persistent settings to flash
    Storage::saveConfig(config);

    // Reconfigure NTP and re-fetch weather
    updateNTP();
    WeatherService::fetchWeather(config, weather);
    lastWeatherFetch = millis();

    // Redraw screen
    tft.fillScreen(hexToRGB565(config.colBg));
    GUI::drawScreen(currentScreen, config, weather);

    // Redirect back to home
    server.sendHeader("Location", "/");
    server.send(303);
}

void handleRestart() {
    server.send(200, "text/html", "<html><body style='background:#121212;color:#fff;text-align:center;padding:50px;'><h2>Restarting SmallTV...</h2><p>Please wait 10 seconds.</p><script>setTimeout(function(){window.location='/';},10000);</script></body></html>");
    delay(1000);
    ESP.restart();
}
