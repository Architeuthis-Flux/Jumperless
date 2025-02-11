#ifndef PERSSISTENTSTUFF_H
#define PERSSISTENTSTUFF_H

extern bool debugFP;
extern bool debugFPtime;

extern bool debugNM;
extern bool debugNMtime;

extern bool debugNTCC;
extern bool debugNTCC2;

extern bool debugLEDs;
extern bool debugMM;

extern int probeSwap;





void debugFlagSet(int flag);
void debugFlagInit(int forceDefaults = 0);
void saveLEDbrightness(int forceDefaults = 0);

void saveDuplicateSettings(int forceDefaults = 0);
void saveVoltages(float top , float bot , float dac0 ,  float dac1 );
void readVoltages(void);
void saveDebugFlags(void);
void saveDacCalibration(void);

void saveLogoBindings(void);
void readLogoBindings(void);
char lastCommandRead(void);
void lastCommandWrite(char lastCommand);

void runCommandAfterReset(char);
#endif