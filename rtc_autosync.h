#ifndef RTC_AUTOSYNC_H
#define RTC_AUTOSYNC_H

#include <Arduino.h>
#include "virtuabotixRTC.h"

class RTCAutoSync {

  public:
    void begin();
    void update();

    String getTimeString();
    String getDateString();

  private:

    void syncNTP();

    virtuabotixRTC myRTC = virtuabotixRTC(25,26,27); // CLK, DATA, RESET

    bool wifiConnecting = false;
    unsigned long wifiTimer = 0;
    unsigned long syncTimer = 0;

};

#endif