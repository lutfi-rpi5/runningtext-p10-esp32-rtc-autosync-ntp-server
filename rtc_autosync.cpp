#include "rtc_autosync.h"
#include <WiFi.h>
#include "time.h"

const char* ssid = "Raxva IoT";
const char* password = "12345678";

const char* ntpServer = "pool.ntp.org";

const long gmtOffset_sec = 25200; // WIB 7*3600
const int daylightOffset_sec = 0;

// void RTCAutoSync::begin() {

//   WiFi.begin(ssid, password);

//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//   }

//   configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

//   struct tm timeinfo;

//   if(getLocalTime(&timeinfo)) {

//     myRTC.setDS1302Time(
//       timeinfo.tm_sec,
//       timeinfo.tm_min,
//       timeinfo.tm_hour,
//       timeinfo.tm_wday,
//       timeinfo.tm_mday,
//       timeinfo.tm_mon + 1,
//       timeinfo.tm_year + 1900
//     );
//   }

//   WiFi.disconnect(true);
//   WiFi.mode(WIFI_OFF);
// }

void RTCAutoSync::begin() {

  myRTC.updateTime();

  syncTimer = millis();
}

// void RTCAutoSync::update() {
//   myRTC.updateTime();
// }

void RTCAutoSync::update() {

  myRTC.updateTime();

  // sync tiap 10 menit
  if (millis() - syncTimer > 600000) {

    WiFi.begin(ssid, password);
    wifiConnecting = true;
    wifiTimer = millis();

    syncTimer = millis();
  }

  // cek koneksi tanpa blocking
  if (wifiConnecting) {

    if (WiFi.status() == WL_CONNECTED) {

      syncNTP();

      WiFi.disconnect(true);
      WiFi.mode(WIFI_OFF);

      wifiConnecting = false;
    }

    // timeout 10 detik
    if (millis() - wifiTimer > 10000) {

      WiFi.disconnect(true);
      WiFi.mode(WIFI_OFF);

      wifiConnecting = false;
    }
  }
}

void RTCAutoSync::syncNTP() {

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  struct tm timeinfo;

  if(getLocalTime(&timeinfo)) {

    myRTC.setDS1302Time(
      timeinfo.tm_sec,
      timeinfo.tm_min,
      timeinfo.tm_hour,
      timeinfo.tm_wday,
      timeinfo.tm_mday,
      timeinfo.tm_mon + 1,
      timeinfo.tm_year + 1900
    );
  }
}

String RTCAutoSync::getTimeString() {

  char buffer[20];

  sprintf(buffer,"%02d:%02d:%02d",
    myRTC.hours,
    myRTC.minutes,
    myRTC.seconds
  );

  return String(buffer);
}

// String RTCAutoSync::getDateString() {

//   char buffer[30];

//   sprintf(buffer,"%02d/%02d/%04d",
//     myRTC.dayofmonth,
//     myRTC.month,
//     myRTC.year
//   );

//   return String(buffer);
// }

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

  char buffer[32];

  sprintf(
    buffer,
    "%s %02d-%02d-%04d",
    hari[myRTC.dayofweek],
    myRTC.dayofmonth,
    myRTC.month,
    myRTC.year
  );

  return String(buffer);
}