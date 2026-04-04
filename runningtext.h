#ifndef RUNNINGTEXT_H
#define RUNNINGTEXT_H

#include <Arduino.h>
#include <DMD32.h>

class RunningText {

  public:
    void begin();
    void update();

    void setSpeed(uint16_t speed);

    void sceneText(String text, uint8_t repeat);
    void sceneRTC(String baris1, String baris2, uint32_t duration);
    void updateRTC(String baris1, String baris2);

    bool isFinished();   // <-- tambah ini

  private:

    static void IRAM_ATTR triggerScan();

    void drawText();
    void drawRTC();

    uint8_t currentScene = 0;
    uint8_t repeatCount = 0;
    uint8_t repeatTarget = 0;
    bool sceneInit = false;

    uint32_t sceneTimer = 0;
    uint32_t sceneDuration = 0;

    String textScene;
    String rtcLine1;
    String rtcLine2;

};

#endif