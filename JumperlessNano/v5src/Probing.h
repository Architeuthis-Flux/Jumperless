
// SPDX-License-Identifier: MIT
#ifndef PROBING_H
#define PROBING_H

extern volatile int sfProbeMenu;
extern unsigned long probingTimer;
extern long probeFrequency;
extern int probePin;
extern int buttonPin;

extern volatile unsigned long blockProbing;
extern volatile unsigned long blockProbingTimer;

extern volatile int connectOrClearProbe;
extern int node1or2;
extern int probeHighlight;
extern int logoTopSetting[2];
extern int logoBottomSetting[2];
extern int buildingTopSetting[2];
extern int buildingBottomSetting[2];
extern int showProbeCurrent;

extern volatile int probeActive;
extern volatile int inPadMenu;
extern volatile int checkingButton;
extern volatile int measureModeActive;

extern int minProbeReadingMap;
extern int maxProbeReadingMap;

extern volatile int removeFade;

extern volatile bool bufferPowerConnected;

extern int debugProbing;

enum measuredState
{
  floating = 0,
  high = 1,
  low = 2,
  probe = 3,
  unknownState = 4 

};
extern volatile int showingProbeLEDs;
extern int switchPosition;
float measureMode(int updateSpeed = 150);
void checkPads(void);
int delayWithButton(int delayTime = 1000);

int chooseGPIO(int skipInputOutput = 0);
int chooseGPIOinputOutput(int gpioChosen);
int chooseADC(void);
int chooseDAC(int justPickOne = 0);
int attachPadsToSettings(int pad);

float voltageSelect(int fiveOrEight = 8);
int longShortPress(int pressLength = 500); 
int doubleSingleClick(void);
int selectFromLastFound(void);
int checkLastFound(int);
void clearLastFound(void);
int probeMode(int pin = 19, int setOrClear = 1);
int checkProbeButton(void);
int readFloatingOrState (int pin = 0, int row = 0);

int checkSwitchPosition(void);
float checkProbeCurrent(void);

void routableBufferPower (int offOn, int flash = 0);

void startProbe (long probeSpeed = 25000);
void stopProbe();

int selectSFprobeMenu(int function = 0);

int getNothingTouched(int samples = 8);
int scanRows(int pin = 0);

int readRails(int pin = 0);
int justReadProbe(void);
int readProbe(void);

int readProbeRaw(int readNothingTouched = 0); 
int calibrateProbe(void);
void calibrateDac0(float target = 3.3);

extern int lastProbeLEDs;


void probeLEDhandler(void); 
void highlightNets(int probeReading);

#endif