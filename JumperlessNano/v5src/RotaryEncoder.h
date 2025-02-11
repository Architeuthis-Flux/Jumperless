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

extern int rotaryDivider;
extern int encoderRaw;
extern volatile int numberOfSteps;
extern volatile bool resetPosition;

enum encoderDirectionStates { NONE,UP,DOWN };

enum encoderButtonStates { IDLE, PRESSED, HELD, RELEASED, DOUBLECLICKED};

extern volatile encoderDirectionStates encoderDirectionState;
extern volatile encoderButtonStates encoderButtonState;
extern volatile encoderButtonStates lastButtonEncoderState;
extern volatile encoderDirectionStates lastDirectionState;

void initRotaryEncoder(void);
void unInitRotaryEncoder(void);
void printRotaryEncoderHelp(void);
void rotaryEncoderStuff(void);


void slotManager(void); 











#endif