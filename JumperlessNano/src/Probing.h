
// SPDX-License-Identifier: MIT
#ifndef PROBING_H
#define PROBING_H


extern unsigned long probingTimer;
extern long probeFrequency;

enum measuredState
{
  floating = 0,
  high = 1,
  low = 2,
  probe = 3
};

int probeMode(int pin = 19);
int checkProbeButton();
int readFloatingOrState (int pin = 0, int row = 0);
void startProbe (long probeSpeed = 50000);
void stopProbe();
int scanRows(int pin = 0, bool clearLastFound = false);






#endif