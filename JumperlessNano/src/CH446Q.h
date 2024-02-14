// SPDX-License-Identifier: MIT
#ifndef CH446Q_H
#define CH446Q_H


extern int netNumberC2;
extern int onOffC2;
extern int nodeC2;
extern int brightnessC2;
extern int hueShiftC2;
extern int lightUpNetCore2;

enum measuredState
{
  floating = 0,
  high = 1,
  low = 2,
  probe = 3
};


void initCH446Q(void);
void sendXYraw(int chip, int x, int y, int setorclear);
int readFloatingOrState (int pin = 0, int row = 0);
void startProbe (int probeSpeed = 50000);
int scanRows(int pin = 0, bool clearLastFound = false);
void sendAllPaths(void); // should we sort them by chip? for now, no
void resetArduino (void);
void sendPath(int path, int setOrClear = 1);

void createXYarray(void);

#endif
