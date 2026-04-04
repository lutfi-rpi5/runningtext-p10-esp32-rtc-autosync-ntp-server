#include "runningtext.h"
#include "fonts/SystemFont5x7.h"
#include "fonts/Arial_black_16.h"

#define DISPLAYS_ACROSS 4
#define DISPLAYS_DOWN 1

DMD* dmd;

hw_timer_t* timer = NULL;

uint16_t scrollSpeed = 30;

void IRAM_ATTR RunningText::triggerScan() {
  dmd->scanDisplayBySPI();
}

void RunningText::begin() {

  dmd = new DMD(DISPLAYS_ACROSS, DISPLAYS_DOWN);

  uint8_t cpuClock = ESP.getCpuFreqMHz();

  timer = timerBegin(0, cpuClock, true);
  timerAttachInterrupt(timer, &RunningText::triggerScan, true);
  timerAlarmWrite(timer, 300, true);
  timerAlarmEnable(timer);

  dmd->clearScreen(true);
}

void RunningText::setSpeed(uint16_t speed) {
  scrollSpeed = speed;
}

void RunningText::sceneText(String text, uint8_t repeat) {

  textScene = text;
  repeatTarget = repeat;
  repeatCount = 0;

  sceneInit = false;   // reset init

  currentScene = 1;
}

void RunningText::sceneRTC(String baris1, String baris2, uint32_t duration) {

  rtcLine1 = baris1;
  rtcLine2 = baris2;

  sceneDuration = duration;
  sceneTimer = millis();

  sceneInit = false;   // reset init

  currentScene = 2;

  dmd->clearScreen(true);
}

void RunningText::update() {

  if (currentScene == 1) drawText();
  if (currentScene == 2) drawRTC();
}

void RunningText::drawText() {

  static unsigned long timer = 0;
  static char buffer[512];

  if (!sceneInit) {

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

      if (repeatCount >= repeatTarget) {

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

    dmd->clearScreen(true);
    sceneInit = true;
  }

  if (millis() - sceneTimer > sceneDuration) {

    currentScene = 0;
    sceneInit = false;
    return;
  }

  dmd->selectFont(System5x7);

  int width1 = rtcLine1.length() * 6;
  int width2 = rtcLine2.length() * 6;

  int x1 = (128 - width1) / 2;
  int x2 = (128 - width2) / 2;

  char line1[64];
  char line2[64];

  rtcLine1.toCharArray(line1, rtcLine1.length() + 1);
  rtcLine2.toCharArray(line2, rtcLine2.length() + 1);

  dmd->drawString(x1, 0, line1, rtcLine1.length(), GRAPHICS_NORMAL);
  dmd->drawString(x2, 8, line2, rtcLine2.length(), GRAPHICS_NORMAL);
}

void RunningText::updateRTC(String baris1, String baris2) {

  rtcLine1 = baris1;
  rtcLine2 = baris2;
}

bool RunningText::isFinished() {
  return currentScene == 0;
}