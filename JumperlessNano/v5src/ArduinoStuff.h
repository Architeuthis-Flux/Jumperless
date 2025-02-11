// SPDX-License-Identifier: MIT
#ifndef ARDUINOSTUFF_H
#define ARDUINOSTUFF_H

#include <Arduino.h>



extern bool ManualArduinoReset;
extern uint8_t numbitsUSBSer1, numbitsUSBSer2;
extern uint8_t paritytypeUSBSer1, paritytypeUSBSer2;
extern uint8_t stopbitsUSBSer1, stopbitsUSBSer2;
extern int baudRateUSBSer1, baudRateUSBSer2;
extern volatile int backpowered;
#ifdef USE_TINYUSB
extern Adafruit_USBD_CDC USBSer1;
extern Adafruit_USBD_CDC USBSer2;
#endif


void initArduino(void);

void initSecondSerial(void);
uint16_t getSerial1Config(void);
uint16_t getSerial2Config(void);

void checkForConfigChangesUSBSer1(bool print = true);
void checkForConfigChangesUSBSer2(bool print = true);
void secondSerialHandler(void);
uint16_t makeSerialConfig(uint8_t numbits = 8, uint8_t paritytype = 0,
                          uint8_t stopbits = 1);
void arduinoPrint(void);
void uploadArduino(void);
void setBaudRate(int baudRate);
void ESPReset(void);
void SetArduinoResetLine(bool state);







#endif
