#include "RotaryEncoder.h"
#include "CH446Q.h"
#include "FileParsing.h"
#include "JumperlessDefinesRP2040.h"
#include "LEDs.h"
#include "MatrixStateRP2040.h"
#include "NetManager.h"
#include "NetsToChipConnections.h"
#include "Peripherals.h"
#include "Probing.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include "quadrature.pio.h"

#include "Graphics.h"
#include "Menus.h"
#include "Commands.h"

volatile int slotChanged = 0;
PIO pioEnc = pio1;

const uint smEnc = 0;
uint offsetEnc = 0;
const uint PIN_AB = 12;

int netSlot = 0;

int newPositionEncoder = 0;
int lastPositionEncoder = 0;
int encoderRaw = 0;
int lastPosition = 0;
int position = 0;
volatile bool resetPosition = false;

int lastButtonState = 0;

volatile int rotaryEncoderMode = 0;

volatile int slotPreview = 0;

// volatile enum { IDLE, UP, DOWN } encoderDirectionState;
// volatile enum { IDLE, PRESSED, HELD, RELEASED } encoderButtonState;

void initRotaryEncoder(void) {
  pinMode(BUTTON_ENC, INPUT);
    pinMode(QUADRATURE_A_PIN, INPUT_PULLUP);
    pinMode(QUADRATURE_B_PIN, INPUT_PULLUP);
  // stdio_init_all();
  pio_add_program(pioEnc, &quadrature_encoder_program);
  quadrature_encoder_program_init(pioEnc, smEnc, PIN_AB, 0);

  //   offsetEnc = pio_add_program(pioEnc, &quadrature_program);
  //   smEnc = pio_claim_unused_sm(pioEnc, true);
  //   quadrature_program_init(pioEnc, smEnc, offsetEnc, QUADRATURE_A_PIN,
  //                           QUADRATURE_B_PIN);
  // delay(1000);
  // Serial.print(smEnc);
  // Serial.print("\n\rRotary Encoder Mode: ");
  // Serial.print(rotaryEncoderMode);
  // Serial.print("\n\r");
}

void unInitRotaryEncoder(void) {
  // pio_sm_unclaim(pioEnc, smEnc);
}

const char rotaryEncoderHelp[] =
    "\n\r" // Github copilot, please don't help me with this
    "\t\t  Rotary Encoder Help\n\r"
    "\t\t  -------------------\n\r"
    "\n\r"
    "            A  COM  B                                 \n\r"
    "    _________________________________________________   \n\r"
    "  /        D12 D11 D10                                \\ \n\r"
    " / .        [,][,][,][][][][][][][][][][][][][]        \\  \n\r"
    "|  ' `0    |  /```\\  |                                  | \n\r"
    "|  '   `   | (  O  ) |                                  | \n\r"
    "|  '     ` |  \\___/  |                                  | \n\r"
    "|  '     '  ['][ ][']{}{}{}{}{}{}{}{}{}[][][][]         | \n\r"
    "|  '     '  D13  AREF                                   | \n\r"
    "|            button                                     | \n\r"
    "\n\r"
    "Stick a 5 pin rotary encoder into the board as shown above. \n\n\r"
    "When rotary encoder mode is on, these pins will be connected\n\r"
    "to the Jumperless [A-UART_Rx(16), B-UART_Tx(17), SW-GPIO_0] \n\n\r"

    "The LEDs under A0-A7 {shown in curly braces} show which of the \n\r"
    "8 slots are active/connected(pink) and previewing(blue/green)\n\n\r"
    "Press the encoder button to made the previewed slot active\n\r"
    "(going into probing mode will make the previewed slot active)\n\n\r"

    "You can cycle through slots by entering z(next) or x(previous)\n\r"
    "or by turning the rotary encoder and then pressing (obviously)\n\n\r"

    "You can show the contents of all the slot files by entering s\n\r"
    "(copy/paste the output into a text file on your computer) \n\n\r"
    "Load files by entering o and paste the text into this terminal \n\n\r"

    "Wokwi sketches will be loaded into whichever slot is active\n\n\r"

    "This is a WIP, so let me know if something's broken or you want\n\r"
    "something added. \n\n\r ";

void printRotaryEncoderHelp(void) {
  Serial.print(rotaryEncoderHelp);
  return;
}

unsigned long buttonHoldTimer = 0;
unsigned long buttonHoldLength = 500;

unsigned long doubleClickTimer = 0;
unsigned long doubleClickLength = 250;

unsigned long buttonDebounceTimer = 0;
unsigned long buttonDebounceTimer2 = 0;
unsigned long debounceTime = 2000;

long positionOffset = 0;

int showingPreview = 0;
int rotState = 0;
int encoderWasPressed = 1;
int encoderIsPressed = 0;
int buttonState = 1;//digitalRead(BUTTON_ENC);

int encoderAstate = 0;
int encoderBstate = 0;
int justPressed = 1;
int lastEncoderBstate = 0;

int probeWasActive = 0;
int encoderReleased = 0;
int printSlotChanges = 0;

int debugEncoder = 0;

int encoderStepsToChangePosition = 2;

int lastRotaryDivider = 8;
int rotaryDivider = 8;
volatile int numberOfSteps = 0;
volatile encoderDirectionStates encoderDirectionState = NONE;
volatile encoderButtonStates encoderButtonState = IDLE;

volatile encoderButtonStates lastButtonEncoderState = IDLE;

volatile encoderDirectionStates lastDirectionState = NONE;

void rotaryEncoderStuff(void) {

  lastButtonEncoderState = encoderButtonState;

  buttonState = digitalRead(BUTTON_ENC);
  // Serial.print("buttonState: ");
  // Serial.println(buttonState);

  if (buttonState == 0) {
     encoderIsPressed = 1;
    //     Serial.print("pressed: ");
   //Serial.println(encoderIsPressed);
  } else {
    encoderIsPressed = 0;
    // Serial.print("pressed: ");
    // Serial.println(encoderIsPressed);
  }

  if (encoderIsPressed == 0 && encoderWasPressed == 0) {
    encoderButtonState = IDLE;
    // lastButtonEncoderState = IDLE;
  }

  if (encoderIsPressed == 0 && encoderWasPressed == 1) {
    encoderButtonState = RELEASED;
    // lastButtonEncoderState = PRESSED;
    encoderReleased = 1;
    doubleClickTimer = millis();
    buttonDebounceTimer2 = micros();

    showLEDsCore2 = 1;
    encoderWasPressed = encoderIsPressed;
  }

  if (encoderIsPressed == 1 && encoderWasPressed == 1) {
    if (millis() - buttonHoldTimer > buttonHoldLength) {
      // lastButtonEncoderState = PRESSED;
      encoderButtonState = HELD;
    }
  }

  if (encoderIsPressed == 1 && encoderWasPressed == 0) {
    buttonHoldTimer = millis();
    // lastButtonEncoderState = IDLE;
    pio_sm_restart(pioEnc, smEnc);
    // encoderRaw = quadrature_encoder_get_count(pioEnc, smEnc);
    // lastPositionEncoder = encoderRaw;

    if (millis() - doubleClickTimer < doubleClickLength) {
      encoderButtonState = DOUBLECLICKED;

    } else {
      encoderButtonState = PRESSED;
    }
    doubleClickTimer = millis();
    // buttonHoldTimer = millis();

    encoderWasPressed = encoderIsPressed;
  }

  lastButtonState = buttonState;
  encoderWasPressed = encoderIsPressed;

  if (lastRotaryDivider != rotaryDivider) {
    pio_sm_restart(pioEnc, smEnc);
    lastRotaryDivider = rotaryDivider;
    encoderRaw = quadrature_encoder_get_count(pioEnc, smEnc);
   //encoderRaw -= positionOffset;
    encoderRaw = encoderRaw / rotaryDivider;
    lastPositionEncoder = encoderRaw;

    // quadrature_program_init(pioEnc, smEnc, offsetEnc, QUADRATURE_A_PIN,
    // QUADRATURE_B_PIN);
  }
  // if (resetPosition == true) {
  //  // quadrature_encoder_program_init(pioEnc, smEnc, PIN_AB, 0);
  //   //pio_sm_restart(pioEnc, smEnc);
  //   //pio_sm_clear_fifos(pioEnc, smEnc);
  //   //pio_sm_drain_tx_fifo(pioEnc, smEnc);
    
  //   positionOffset = quadrature_encoder_get_count(pioEnc, smEnc);
  //   positionOffset = positionOffset / rotaryDivider;
  //   Serial.print("\n\n\rencoderRaw: ");
  //   Serial.println(encoderRaw);
  //   Serial.print("positionOffset: ");
  //   Serial.println(positionOffset);
    

  //   // encoderRaw -= positionOffset;
  //   // encoderRaw = encoderRaw / rotaryDivider;
  //   //lastPositionEncoder = positionOffset/rotaryDivider;
  //   resetPosition = false;
  // }

  encoderRaw = quadrature_encoder_get_count(pioEnc, smEnc);

  encoderRaw = encoderRaw / rotaryDivider;
//encoderRaw -= positionOffset;
  numberOfSteps = abs(lastPositionEncoder - encoderRaw);

  if ((lastPositionEncoder - encoderRaw > 1 || lastPositionEncoder - encoderRaw < -1) || (lastPositionEncoder != encoderRaw && rotaryDivider < 4)) {

    if (lastPositionEncoder > encoderRaw && encoderDirectionState != DOWN) {
      position++;
      encoderDirectionState = UP;
      //numberOfSteps = abs(lastPositionEncoder - encoderRaw);
      numberOfSteps = abs(lastPositionEncoder - encoderRaw);

      lastPositionEncoder = encoderRaw;

    } else if (lastPositionEncoder < encoderRaw &&
              encoderDirectionState != UP) {
      position--;
      encoderDirectionState = DOWN;
numberOfSteps = lastPositionEncoder - encoderRaw;
      lastPositionEncoder = encoderRaw;

    } else {
      encoderDirectionState = NONE;
    }

    //}

  } else {
    encoderDirectionState = NONE;
  }

  // slotManager();

  // buttonState = digitalRead(BUTTON_ENC);
  //  if (millis() - buttonHoldTimer > buttonHoldLength && buttonState == 0 )
  //  {
  //      //Serial.println("held\n\r");
  //      //refreshSavedColors();

  //     slotChanged = 1;
  //     netSlot = slotPreview;
  // }

  if (debugEncoder == 1) {
    if (encoderButtonState != lastButtonEncoderState) {
      switch (encoderButtonState) {
      case IDLE:
        // Serial.print("IDLE");
        break;
      case PRESSED:
        Serial.print(lastButtonEncoderState);
        Serial.println(" PRESSED");
        // delay(150);
        break;
      case HELD:

        Serial.println("HELD");
        // delay(150);
        break;
      case RELEASED:

        Serial.println("RELEASED");
        // delay(150);
        break;

      case DOUBLECLICKED:
        Serial.println("DOUBLECLICKED");
        /// delay(150);
        break;

      default:
        break;
      }
    }
    if (encoderDirectionState != NONE || encoderButtonState != IDLE) {
      switch (encoderDirectionState) {
      case NONE:

        // Serial.print("NONE");
        break;
      case UP:
        Serial.print(position);
        Serial.println("  UP");

        // delay(150);
        break;
      case DOWN:
        Serial.print(position);
        Serial.println("  DOWN");
        // delay(150);
        break;
      default:
        break;
      }
    }
  }
}

unsigned long previewLength = 30500;
unsigned long previewTimer = 0;
int lastSlotPreview = 0;

void slotManager(void) {

  //   if (lastButtonEncoderState != encoderButtonState) {
  //    // if (1){
  //     // buttonDebounceTimer2 = millis();
  //     Serial.print("WasPressed: ");
  //     Serial.print(encoderWasPressed);
  //     Serial.print("\t\t");
  //     Serial.print("IsPressed: ");
  //     Serial.print(encoderIsPressed);
  //     Serial.println("\t");

  //     Serial.print("lastButtonState: ");
  //     Serial.print(lastButtonEncoderState);
  //     Serial.print("\t");
  //     Serial.print("ButtonState: ");
  //     Serial.print(encoderButtonState);
  //     Serial.print("\t");

  //     Serial.println("\n\r");
  //     delay(150);
  //   }

  // showingPreview = 0;
  // rawOtherColors[1] = 0x550008,
  // printSlotChanges = 1;
  if (lastDirectionState == NONE && encoderDirectionState != NONE) {
    // lastSlotPreview = slotPreview;
    switch (encoderDirectionState) {
    case UP:
      slotPreview++;
      if (slotPreview >= NUM_SLOTS) {
        slotPreview = 0;
      }
      break;
    case DOWN:
      slotPreview--;
      if (slotPreview < 0) {
        slotPreview = NUM_SLOTS - 1;
      }
      break;
    default:
      break;
    }
  }

  if (encoderButtonState == PRESSED && lastButtonEncoderState == IDLE) {

    // Serial.println("netSlot: ");
    // Serial.print(netSlot);
    // Serial.print("\t");
    // Serial.print("slotPreview: ");
    // Serial.print(slotPreview);
    // Serial.println("\n\r");
    // delay(150);
    if (netSlot != slotPreview) {
      Serial.print("netSlot: ");
      Serial.print(netSlot);
      Serial.print("\t");
      Serial.print("slotPreview: ");
      Serial.print(slotPreview);
      Serial.print("\n\r");

      netSlot = slotPreview;
      showingPreview = 0;
      showSavedColors(netSlot);
      // lightUpRail();
      //  leds.show();
      // showLEDsCore2 = 1;
      slotChanged = 1;
    }

    // buttonHoldTimer = millis();
    if (printSlotChanges == 1) {
      Serial.print("\r                                          \r");
      Serial.print("\rCurrent Slot: ");
      Serial.print(netSlot);
      Serial.print("\t");
    }
  }
  // return;
  if (millis() - previewTimer > previewLength && showingPreview == 1 &&
      netSlot != slotPreview) {
    slotPreview = netSlot;

    showSavedColors(netSlot);

    showingPreview = 0;
    rawOtherColors[1] = 0x550008;

    if (printSlotChanges == 1) {
      Serial.print("\r                                          \r");
      Serial.print("\rCurrent Slot: ");
      Serial.print(netSlot);
      Serial.print("\t");
    }
    previewTimer = millis();
    showLEDsCore2 = 1;
  }

  if (lastSlotPreview != slotPreview) {

    b.print("Slot ", (uint32_t)0x050007);
    b.print(slotPreview, 5);

    showLEDsCore2 = 1;
    lastSlotPreview = slotPreview;

    // lastPosition = position;

    showingPreview = 1;
    //  rawOtherColors[1] = 0x960095;
    rawOtherColors[1] = 0x1800FF;
    showSavedColors(slotPreview);

    previewTimer = millis();

    if (printSlotChanges == 1) {
      Serial.print("\r                                          \r");
      Serial.print("\rCurrent Slot: ");
      Serial.print(netSlot);
      Serial.print("\t");

      if (slotPreview != netSlot) {
        Serial.print("\tSelected Slot: ");
        Serial.print(slotPreview);
      }
    }
  }
}
