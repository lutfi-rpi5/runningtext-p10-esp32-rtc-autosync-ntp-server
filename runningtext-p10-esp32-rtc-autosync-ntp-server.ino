/*
PROGRAM NAME :
"Running Text P10 ESP32 with RTC DS1302 (Auto Sync WIB)"

DATE          : 03-4-2026
AUTHOR        : Muhammad Lutfi Nur Anendi
BOARD         : ESP32 WROOM-32
BOARD VERSION : ESP32 Dev Module v1.0.6
*/

#include "runningtext.h"
#include "rtc_autosync.h"
#include "buzzer.h"

RunningText display;
RTCAutoSync main_rtc;

// String space_text = "              ";

String text_utama =
// "Selamat Datang di Bengkel Jurusan Teknik Elektro";
"Selamat Datang di Bengkel Jurusan Teknik Elektro  |  Prodi Teknik Elektronika              Politeknik Negeri Sriwijaya              Utamakan Keselamatan dan Kesehatan Kerja";

void setup() {

  Serial.begin(115200); 

  delay(500);

  // digitalWrite(33, HIGH);

  buzzer.begin(33);
  main_rtc.begin();
  display.begin();

  // DISPLAY SPEED
  display.setSpeed(20);

  Serial.println("[MAIN] runningtext_p10_esp32_rtc_autosync_ntp_server");
  Serial.println("[MAIN] System is Starting...");
  Serial.println("[MAIN] _____________________________________________");
  buzzer.beepLong(1000);
}

void loop() {

  // main_rtc.update();

  static uint8_t scene = 0;
  static unsigned long rtc_timer = 0;

  static String waktu;
  static String tanggal;

  // Update RTC tiap 1 detik
  if (millis() - rtc_timer > 1000) {

    main_rtc.update();

    waktu   = main_rtc.getTimeString();
    tanggal = main_rtc.getDateString();

    rtc_timer = millis();
  }

  // Scene 1 : Running Text
  if (scene == 0) {

    display.sceneText(text_utama, 1);
    scene = 1;
  }

  // Pindah ke Scene 2
  else if (scene == 1 && display.isFinished()) {

    display.sceneRTC(
      tanggal,
      waktu,
      (3 * 60) * 1000 //DURASI TAMPIL SCENE RTC
    );

    scene = 2;
  }

  // Scene 2 : RTC Live Update
  else if (scene == 2) {

    display.updateRTC(
      tanggal,
      waktu
    );

    if (display.isFinished()) {
      scene = 0;
    }
  }

  display.update();
  buzzer.update();
}