#include "rtc_autosync.h"
#include <WiFi.h>
#include "time.h"
#include "buzzer.h"

/*
===========================================================
RTCAutoSync
-----------------------------------------------------------
Fungsi :
- Menggabungkan RTC DS1302 + NTP Internet
- RTC tetap berjalan jika tidak ada internet
- Sinkronisasi otomatis jika internet tersedia
- Non-blocking WiFi connection
===========================================================
*/

const char* ssid = "sync-elektro";
const char* password = "12345678";

const char* ntpServer = "pool.ntp.org";

const long gmtOffset_sec = 25200; // WIB 7*3600
const int daylightOffset_sec = 0;

unsigned long bootDelay = 0;
const unsigned long bootWait = 0; // 10 detik

const unsigned long ntp_sync_interval = (60 * 60) * 1000; // setiap 60 menit
const unsigned long ntp_sync_starting_at = (0.1 * 60) * 1000; // 10 detik pertama sejak esp32 booting
/*
===========================================================
BEGIN
-----------------------------------------------------------
Inisialisasi RTC
Tidak melakukan koneksi WiFi (Non Blocking Design)
===========================================================
*/
// void RTCAutoSync::begin() {

//   Serial.println(); 
//   Serial.println("[RTC] Init RTC..."); 

//   myRTC.updateTime(); 

//   syncTimer = millis(); 

//   Serial.println("[RTC] RTC Ready"); 
// }
void RTCAutoSync::begin() {
  
  Serial.println();
  Serial.println("[RTC] Init RTC...");

  myRTC.updateTime();

  bootDelay = millis();

  // sync pertama setelah bootWait
  syncTimer = millis() - ntp_sync_interval + bootWait;

  Serial.println("[RTC] RTC Ready");
}

/*
===========================================================
UPDATE
-----------------------------------------------------------
Dipanggil terus menerus di loop()

Fungsi :
- Update RTC lokal
- Cek interval sync NTP
- Background WiFi connection
- Timeout handling
===========================================================
*/
void RTCAutoSync::update() {

  myRTC.updateTime();

  // tunggu boot stabil dulu
  if (millis() - bootDelay < bootWait) {
    return;
  }

  // sync setiap nilai waktu dari variabel ntp_sync_interval
  if (millis() - syncTimer > ntp_sync_interval) {

    Serial.println("[RTC] Request NTP Sync...");

    WiFi.begin(ssid, password);

    Serial.println("[RTC] Connecting WiFi...");

    wifiConnecting = true;
    wifiTimer = millis();

    syncTimer = millis();
  }

  // cek koneksi tanpa blocking
  if (wifiConnecting) {

    if (WiFi.status() == WL_CONNECTED) {

      Serial.println("[RTC] WiFi Connected");
      Serial.println("[RTC] Sync NTP...");
      buzzer.beep(2);

      syncNTP();

      Serial.println("[RTC] Sync Complete");
      buzzer.beep(3);

      WiFi.disconnect(true);
      WiFi.mode(WIFI_OFF);

      Serial.println("[RTC] WiFi Off");

      wifiConnecting = false;
    }

    // timeout 10 detik
    if (millis() - wifiTimer > 60000) {

      Serial.println("[RTC] WiFi Timeout");

      WiFi.disconnect(true);
      WiFi.mode(WIFI_OFF);

      wifiConnecting = false;
    }
  }
}

/*
===========================================================
SYNC NTP
-----------------------------------------------------------
Sinkronisasi waktu dari NTP server
===========================================================
*/
void RTCAutoSync::syncNTP() {

  Serial.println("[RTC] Config NTP");

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  struct tm timeinfo;

  if(getLocalTime(&timeinfo)) {

    Serial.println("[RTC] NTP Time Received");

    myRTC.setDS1302Time(
      timeinfo.tm_sec,
      timeinfo.tm_min,
      timeinfo.tm_hour,
      timeinfo.tm_wday,
      timeinfo.tm_mday,
      timeinfo.tm_mon + 1,
      timeinfo.tm_year + 1900
    );

    Serial.println("[RTC] RTC Updated");
  }
  else {
    Serial.println("[RTC] NTP Failed");
  }
}

/*
===========================================================
GET TIME STRING
-----------------------------------------------------------
Format :
HH:MM:SS
===========================================================
*/
String RTCAutoSync::getTimeString() {

  char buffer[20];

  sprintf(buffer,"%02d:%02d:%02d",
    myRTC.hours,
    myRTC.minutes,
    myRTC.seconds
  );

  return String(buffer);
}

/*
===========================================================
GET DATE STRING
-----------------------------------------------------------
Format :
JUM'AT 03-04-2026
===========================================================
*/
String RTCAutoSync::getDateString() {

  const char* hari[] = {
    "MINGGU",
    "SENIN",
    "SELASA",
    "RABU",
    "KAMIS",
    "JUM'AT",
    "SABTU"
  };
  
  const char* hariSingkat[] = {
    "MIG",
    "SENIN",
    "SEL",
    "RABU",
    "KAMIS",
    "JUMAT",
    "SABTU"
  };

  char buffer[32];

  sprintf(
    buffer,
    "%s %02d-%02d-%04d",
    hariSingkat[myRTC.dayofweek],
    myRTC.dayofmonth,
    myRTC.month,
    myRTC.year
  );

  return String(buffer);
}