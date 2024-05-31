#ifndef ROTARYENCODER_H
#define ROTARYENCODER_H

#define NUM_SLOTS 8

extern volatile int rotaryEncoderMode;
extern int netSlot;
extern volatile int slotChanged;

extern volatile int slotPreview;
extern int rotState;
extern int encoderIsPressed;
extern int showingPreview;

void initRotaryEncoder(void);
void unInitRotaryEncoder(void);
void printRotaryEncoderHelp(void);
void rotaryEncoderStuff(void);











#endif