# 📺 SmallTV-Ultra Desktop Weather & Clock Station

Custom open-source firmware for the **GeekMagic SmallTV-Ultra** (ESP8266 / ESP-12F with 1.54″ ST7789 IPS LCD). Features a clean customizable UI, real-time Open-Meteo weather synchronization, NTP atomic timekeeping, persistent web configuration, 1-click theme presets, and selectable pixel-art desktop icons.

---

## ✨ Features

* **Real-Time Weather Integration:** Real-time temperature, apparent temperature ("Feels like"), humidity gauge, condition icon, and 5-day daily forecast via [Open-Meteo](https://open-meteo.com/) (no API key required).
* **Location & Geocoding:** Auto-lookup GPS coordinates and country codes directly from the web settings portal.
* **Atomic Clock Sync:** Global NTP synchronization with automatic POSIX daylight saving time (DST) support.
* **Customizable Typography:**
  * 4 Clock Font Styles: 📟 7-Segment Digital, 📱 Modern Bold Rounded, 🤖 Cyberpunk Tech/Mono, ✨ Clean Sans.
  * Scalable font size scales & in-between (~19px) typography for Feels-like and Date/Day.
* **8 Selectable Desktop Icons:**
  * 🧑‍🚀 Classic Floating Spaceman
  * 🐱 Orange Desk Cat (Pixel Pet)
  * 🪐 Cosmic Saturn (Ringed Planet with Moon)
  * 🚀 Sci-Fi Rocket Ship
  * ☕ Steaming Coffee Mug
  * 👾 Retro Arcade Pacman & Ghost
  * 🌦️ Dynamic Live Weather Art
  * 🎮 Retro Game Controller
* **1-Click Theme Engine:** Instant theme presets (Cyberpunk Gold, Synthwave Neon, Matrix Green, Ice Glacier, Sunset Amber) + individual 24-bit RGB color pickers.
* **Wireless Web Updater (OTA):** Full browser-based firmware flashing at `http://<device-ip>/update`.
* **Hardware Controls:** Top touch button supports short-press screen switching (Clock -> 5-Day Forecast -> System Info) and long-press brightness cycling.

---

## 🖥️ Screen Layout

```
┌─────────────────────────────────────────────────────────────┐
│  Seoul   [ KR ]                                             │  <-- Top banner: Location ONLY
│ ─────────────────────────────────────────────────────────── │
│  Feels like 12°C                      ( ~19px In-Between )  │  <-- Sub-header
│                                                             │
│                                            ┌─────────────┐  │
│   17 : 39                                  │   ( ☀️ )     │  │  <-- Clock (No Seconds)
│   (Large Clock)                            │  [ Clear ]  │  │  <-- Weather Orb & Badge
│                                            └─────────────┘  │
│                                                             │
│   20/10/2026                 FRIDAY   ( ~19px In-Between )  │  <-- Date & Day
│ ─────────────────────────────────────────────────────────── │
│   🌡️  [====   ] 14°C               ┌─────────────────────┐  │
│   💧  [===    ] 45%                │  🧑‍🚀  Selected       │  │
│                                    │      Desktop Icon   │  │
│                                    └─────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

---

## 🛠️ Hardware Specifications & Pinout

| Function | ESP8266 GPIO | Description |
| :--- | :---: | :--- |
| **Display Controller** | SPI | 1.54″ ST7789 IPS LCD (240×240) |
| **MOSI** | `GPIO 13` | SPI Data |
| **SCLK** | `GPIO 14` | SPI Clock |
| **DC** | `GPIO 0` | Data / Command |
| **RST** | `GPIO 2` | Display Reset |
| **Backlight PWM** | `GPIO 5` | Hardware PWM Dimming (0–255) |
| **Touch Button** | `GPIO 4` | Top Capacitive Touch Key (Active LOW) |
| **Status LED** | `GPIO 2` | Onboard Blue LED (Active LOW) |

---

## 🚀 Building & Flashing

### Prerequisites
* [PlatformIO Core](https://platformio.org/) (`pio`) or PlatformIO IDE extension for VSCode.

### Build Firmware
```bash
# Clone the repository
git clone https://github.com/<your-username>/desktop-weather.git
cd desktop-weather

# Build the firmware binary
pio run
```

The compiled binary will be located at `.pio/build/smalltv_ultra/firmware.bin`.

### Flashing via Web OTA (Over Wi-Fi)
1. Power on the SmallTV.
2. Connect to the device's IP in your browser: `http://<your-device-ip>/update`
3. Upload `.pio/build/smalltv_ultra/firmware.bin` and click **Update**.

---

## ⚙️ Initial Wi-Fi Setup
When booting for the first time (or after moving to a new network):
1. The device creates a Wi-Fi Access Point named **`SmallTV-Setup`**.
2. Connect to `SmallTV-Setup` from your phone or PC.
3. Open `http://192.168.4.1` to select your home Wi-Fi network and enter your password.
4. The device will connect to your network and display its local IP address.

---

## 📄 License
This project is open-source and available under the [MIT License](LICENSE).
