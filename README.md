# Running Text P10 ESP32 + RTC + Auto Sync NTP

## 📌 Overview

Project ini adalah **Running Text LED P10 berbasis ESP32** yang dilengkapi dengan:

* Running Text P10
* RTC (Real Time Clock)
* Auto Sync NTP Server
* Fallback ke RTC saat offline
* Sinkronisasi otomatis saat internet tersedia
* Non-blocking NTP sync

Sistem dirancang agar **running text tetap berjalan walaupun tanpa internet**, namun **otomatis sinkron waktu saat koneksi tersedia**.

---

## ✨ Features

* Running Text LED P10 Single Color
* RTC Offline Mode
* Auto Sync ke NTP Server
* Non-blocking Internet Sync
* Fallback otomatis RTC saat offline
* Modular code (lebih mudah dikembangkan)
* ESP32 Compatible

---

## 🧠 System Architecture

```
Internet (NTP)
      │
      ▼
ESP32 ─── RTC
  │
  ▼
LED P10 Running Text
```

Flow Sistem:

1. ESP32 Boot
2. Coba koneksi WiFi
3. Jika berhasil → Sync NTP
4. Simpan waktu ke RTC
5. Jika gagal → Gunakan RTC
6. Running text tetap berjalan

---

## 🛠 Hardware Requirements

* ESP32
* LED Panel P10 Single Color
* RTC Module (DS1302 / DS3231)
* Power Supply 5V

---

## 🔌 Pin Configuration

Sesuaikan dengan kebutuhan hardware

Contoh konfigurasi:

### P10 Panel -> ESP32

```
GND -> GND
OE  -> GPIO 22
A   -> GPIO 19
B   -> GPIO 21
CLK -> GPIO 18
LAT -> GPIO 2
DR  -> GPIO 23
```

### RTC -> ESP32

```
CLK -> GPIO 25
DAT -> GPIO 26
RST -> GPIO 27
```

---

## 📥 Installation

### 1. Clone Repository

```
git clone https://github.com/lutfi-rpi5/runningtext-p10-esp32-rtc-autosync-ntp-server.git
```

Atau download ZIP dari GitHub

---

### 2. Install ESP32 Board

Buka Arduino IDE:

```
File → Preferences
```

Tambahkan URL berikut:

```
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

Kemudian:

```
Tools → Board → Board Manager
```

Install:

```
ESP32 by Espressif Systems Versi 1.0.5
```

---

## 📚 Required Libraries

### Perlu Install

Library berikut perlu di-install:
* DMD32 (search on Library Manager Arduino IDE)


### Tidak Perlu Install (Built-in ESP32)

Library berikut sudah tersedia:

* WiFi.h
* time.h
* SPI.h
* Wire.h


### Library Sudah Termasuk Dalam Project

Tidak perlu install manual:

* runningtext.h
* runningtext.cpp
* rtc_autosync.h
* rtc_autosync.cpp
* virtuabotixRTC.h
* virtuabotixRTC.cpp

---

## 📁 Project Structure

```
runningtext-p10-esp32-rtc-autosync-ntp-server
│
├── runningtext-p10-esp32-rtc-autosync-ntp-server.ino
│
├── runningtext.cpp
├── runningtext.h
│
├── rtc_autosync.cpp
├── rtc_autosync.h
│
├── virtuabotixRTC.cpp
├── virtuabotixRTC.h
│
└── README.md
```

---

## 📖 File Explanation

### Main File

```
runningtext-p10-esp32-rtc-autosync-ntp-server.ino
```

Fungsi:

* Setup WiFi
* Setup Running Text
* Setup RTC
* Main Loop

---

### Running Text Module

```
runningtext.cpp
runningtext.h
```

Fungsi:

* Render Running Text
* Control Panel P10
* Animasi Text

---

### RTC Auto Sync Module

```
rtc_autosync.cpp
rtc_autosync.h
```

Fungsi:

* Sync waktu dari NTP
* Fallback RTC
* Auto Sync Interval
* Non Blocking Sync

---

### RTC Library

```
virtuabotixRTC.cpp
virtuabotixRTC.h
```

Fungsi:

* Read RTC
* Write RTC
* Backup waktu offline

---

## 🔄 Auto Sync Mechanism

Logic sistem:

```
Jika WiFi tersedia
    Sync ke NTP
    Simpan ke RTC

Jika WiFi tidak tersedia
    Gunakan RTC

Jika WiFi kembali
    Sync otomatis
```

---

## ⚙️ Configuration

Edit konfigurasi WiFi:

```
//rtc_autosync.cpp
const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_PASSWORD";
```

Edit konfigurasi Zona Waktu:

```
//rtc_autosync.cpp
const long gmtOffset_sec = 25200; // Example: WIB GMT+7 * 3600 = 25200
const int daylightOffset_sec = 0;
```

---

## ⏱ Sync Interval

Default:

* Sync setiap 10 menit
* Non blocking

Bisa diubah pada:

```
rtc_autosync.cpp
```

---

## 🚀 Upload

Pilih Board:

```
ESP32 Dev Module
```

Upload Program.

---

## 🧪 Serial Monitor

Baudrate:

```
115200
```

Contoh Output:

```
Connecting WiFi...
WiFi Connected
Sync NTP...
RTC Updated
Running Text Started
```

---

## 📌 Notes

* Running text tetap berjalan tanpa internet
* RTC sebagai backup waktu
* NTP sebagai sumber waktu utama
* Non-blocking internet sync

---

## 📜 License

Open Source
Free to use and modify

---

## 👨‍💻 Author

Lutfi
Embedded System Engineer
