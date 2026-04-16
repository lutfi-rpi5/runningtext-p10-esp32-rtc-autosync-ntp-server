#include "runningtext.h"
#include "fonts/SystemFont5x7.h"
#include "fonts/Arial_black_16.h"
#include <Ticker.h>

/*
===========================================================
RUNNING TEXT ENGINE
-----------------------------------------------------------
Fungsi :
- Running text P10
- Scene management
- RTC display
- Hardware timer scan DMD
===========================================================
*/

/*
GND – GND
OE – D22
A – D19
B – D21
CLK – D18
LAT – D2
DR – D23
VCC – VP
GND – EN
*/


#define DISPLAYS_ACROSS 3
#define DISPLAYS_DOWN 1

DMD* dmd;

// hw_timer_t* timer = NULL;
Ticker displayTicker;

uint16_t scrollSpeed = 30;

/*
===========================================================
TIMER INTERRUPT
-----------------------------------------------------------
Digunakan untuk scan display P10
Harus cepat dan ringan
===========================================================
*/
void RunningText::triggerScan() {
  dmd->scanDisplayBySPI();
}
// void IRAM_ATTR RunningText::triggerScan() {
//   dmd->scanDisplayBySPI();
// }
// void IRAM_ATTR RunningText::triggerScan() {
//   if (dmd) {
//     dmd->scanDisplayBySPI();
//   }
// }


/*
===========================================================
BEGIN
-----------------------------------------------------------
Inisialisasi DMD dan Timer Hardware
===========================================================
*/
void RunningText::begin() {

  Serial.println("[DISPLAY] Init Running Text");

  dmd = new DMD(DISPLAYS_ACROSS, DISPLAYS_DOWN);

  displayTicker.attach_ms(2, RunningText::triggerScan);

  dmd->clearScreen(true);

  Serial.println("[DISPLAY] Display Ready");
}

// void RunningText::begin() {

//   Serial.println("[DISPLAY] Init Running Text");

//   dmd = new DMD(DISPLAYS_ACROSS, DISPLAYS_DOWN);

//   delay(10);

//   uint8_t cpuClock = ESP.getCpuFreqMHz();

//   Serial.print("[DISPLAY] CPU Clock : ");
//   Serial.println(cpuClock);

//   // timer = timerBegin(0, cpuClock, true);
//   // timerAttachInterrupt(timer, &RunningText::triggerScan, true);
//   // timerAlarmWrite(timer, 300, true);
//   // timerAlarmEnable(timer);

//   timer = timerBegin(0, 80, true);
//   timerAttachInterrupt(timer, &RunningText::triggerScan, true);
//   timerAlarmWrite(timer, 2000, true);
//   timerAlarmEnable(timer);

//   dmd->clearScreen(true);

//   Serial.println("[DISPLAY] Display Ready");
// }

/*
===========================================================
SET SPEED
-----------------------------------------------------------
Mengatur kecepatan running text
===========================================================
*/
void RunningText::setSpeed(uint16_t speed) {

  scrollSpeed = speed;

  Serial.print("[DISPLAY] Scroll Speed : ");
  Serial.println(scrollSpeed);
}

/*
===========================================================
SCENE TEXT
-----------------------------------------------------------
Menampilkan running text
===========================================================
*/
void RunningText::sceneText(String text, uint8_t repeat) {

  Serial.println("[DISPLAY] Scene Running Text");

  textScene = text;
  repeatTarget = repeat;
  repeatCount = 0;

  sceneInit = false;   // reset init

  currentScene = 1;
}

void RunningText::sceneRTC(String baris1, String baris2, uint32_t duration) {

  Serial.println("[DISPLAY] Scene RTC");

  rtcLine1 = baris1;
  rtcLine2 = baris2;

  sceneDuration = duration;
  sceneTimer = millis();

  sceneInit = false;   // reset init

  currentScene = 2;

  dmd->clearScreen(true);
}

/*
===========================================================
UPDATE
-----------------------------------------------------------
Scene manager
===========================================================
*/
void RunningText::update() {

  if (currentScene == 1) drawText();
  if (currentScene == 2) drawRTC();
}

/*
===========================================================
DRAW TEXT
-----------------------------------------------------------
Menampilkan running text
===========================================================
*/
void RunningText::drawText() {

  static unsigned long timer = 0;
  // static char buffer[512];
  char buffer[256];

  if (!sceneInit) {

    Serial.println("[DISPLAY] Init Running Text Scene");

    dmd->clearScreen(true);
    dmd->selectFont(Arial_Black_16);

    textScene.toCharArray(buffer, textScene.length() + 1);

    dmd->drawMarquee(
      buffer,
      strlen(buffer),
      (32 * DISPLAYS_ACROSS) - 1,
      0
    );

    timer = millis();   // reset timer
    sceneInit = true;
  }

  if (millis() - timer > scrollSpeed) {

    bool ret = dmd->stepMarquee(-1, 0);

    if (ret) {

      repeatCount++;

      Serial.print("[DISPLAY] Repeat : ");
      Serial.println(repeatCount);

      if (repeatCount >= repeatTarget) {

        Serial.println("[DISPLAY] Running Text Scene Finished");

        currentScene = 0;
        sceneInit = false;
        return;
      }

      textScene.toCharArray(buffer, textScene.length() + 1);

      dmd->drawMarquee(
        buffer,
        strlen(buffer),
        (32 * DISPLAYS_ACROSS) - 1,
        0
      );
    }

    timer = millis();
  }
}

void RunningText::drawRTC() {

  if (!sceneInit) {

    Serial.println("[DISPLAY] Init RTC Scene");

    dmd->clearScreen(true);
    sceneInit = true;
  }

  if (millis() - sceneTimer > sceneDuration) {

    Serial.println("[DISPLAY] RTC Scene Finished");

    currentScene = 0;
    sceneInit = false;
    return;
  }

  dmd->selectFont(System5x7);

  int width1 = rtcLine1.length() * 6;
  int width2 = rtcLine2.length() * 6;

  // int x1 = (128 - width1) / 2;
  // int x2 = (128 - width2) / 2;

  int x1 = (32 * DISPLAYS_ACROSS - width1) / 2;
  int x2 = (32 * DISPLAYS_ACROSS - width2) / 2;

  char line1[64];
  char line2[64];

  rtcLine1.toCharArray(line1, rtcLine1.length() + 1);
  rtcLine2.toCharArray(line2, rtcLine2.length() + 1);

  dmd->drawString(x1, 0, line1, rtcLine1.length(), GRAPHICS_NORMAL);
  dmd->drawString(x2, 8, line2, rtcLine2.length(), GRAPHICS_NORMAL);
}

/*
===========================================================
UPDATE RTC
-----------------------------------------------------------
Update text RTC realtime
===========================================================
*/
void RunningText::updateRTC(String baris1, String baris2) {

  rtcLine1 = baris1;
  rtcLine2 = baris2;
}

/*
===========================================================
IS FINISHED
-----------------------------------------------------------
Cek apakah scene selesai
===========================================================
*/
bool RunningText::isFinished() {
  return currentScene == 0;
}