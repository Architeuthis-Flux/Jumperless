// SPDX-License-Identifier: MIT

#include "ArduinoStuff.h"
#include "LEDs.h"
#include "MatrixStateRP2040.h"
#include "NetsToChipConnections.h"

Adafruit_USBD_CDC USBSer1;
Adafruit_USBD_CDC USBSer2;

int baudRateUSBSer1 = 115200; // for Arduino-Serial
int baudRateUSBSer2 = 115200; // for Routable Serial

volatile int backpowered = 1;

void initArduino(void) // if the UART is set up, the Arduino won't flash from
                       // it's own USB port
{

  // Serial1.setRX(17);

  // Serial1.setTX(16);

  // Serial1.begin(115200);

  // delay(1);
}

void initSecondSerial(void) {
#ifdef USE_TINYUSB

  USBSer1.setStringDescriptor("JL Arduino");  //Not working
  USBSer2.setStringDescriptor("JL Routable"); //Not working

  USBSer1.begin(baudRateUSBSer1, getSerial1Config());
  Serial1.begin(baudRateUSBSer1, getSerial1Config());


  USBSer2.begin(baudRateUSBSer2, getSerial2Config());
  Serial2.begin(baudRateUSBSer2, getSerial2Config());
#endif
}


bool ManualArduinoReset = false;
bool LastArduinoDTR = false;
bool LastRoutableDTR = false;
uint8_t numbitsUSBSer1 = 8;
uint8_t paritytypeUSBSer1 = 0;
uint8_t stopbitsUSBSer1 = 1;
uint8_t numbitsUSBSer2 = 8;
uint8_t paritytypeUSBSer2 = 0;
uint8_t stopbitsUSBSer2 = 1;

unsigned long FirstDTRTime;
bool FirstDTR = true;
bool ESPBoot = false;
unsigned long ESPBootTime;


int serConfigChangedUSBSer1 = 0;
int serConfigChangedUSBSer2 = 0;

uint16_t makeSerialConfig(uint8_t numbits, uint8_t paritytype,
                          uint8_t stopbits) {
  uint16_t config = 0;

  //   #define SERIAL_PARITY_EVEN   (0x1ul)
  // #define SERIAL_PARITY_ODD    (0x2ul)
  // #define SERIAL_PARITY_NONE   (0x3ul)
  // #define SERIAL_PARITY_MARK   (0x4ul)
  // #define SERIAL_PARITY_SPACE  (0x5ul)
  // #define SERIAL_PARITY_MASK   (0xFul)

  // #define SERIAL_STOP_BIT_1    (0x10ul)
  // #define SERIAL_STOP_BIT_1_5  (0x20ul)
  // #define SERIAL_STOP_BIT_2    (0x30ul)
  // #define SERIAL_STOP_BIT_MASK (0xF0ul)

  // #define SERIAL_DATA_5        (0x100ul)
  // #define SERIAL_DATA_6        (0x200ul)
  // #define SERIAL_DATA_7        (0x300ul)
  // #define SERIAL_DATA_8        (0x400ul)
  // #define SERIAL_DATA_MASK     (0xF00ul)
  // #define SERIAL_5N1           (SERIAL_STOP_BIT_1 | SERIAL_PARITY_NONE  |
  // SERIAL_DATA_5)

  unsigned long parity = 0x3ul;
  unsigned long stop = 0x10ul;
  unsigned long data = 0x400ul;

  switch (numbits) {
  case 5:
    data = 0x100ul;
    break;
  case 6:
    data = 0x200ul;
    break;
  case 7:
    data = 0x300ul;
    break;
  case 8:
    data = 0x400ul;
    break;
  default:
    data = 0x400ul;
    break;
  }

  switch (paritytype) {
  case 0:
    parity = 0x3ul;
    break;
  case 2:
    parity = 0x1ul;
    break;
  case 1:
    parity = 0x2ul;
    break;
  case 3:
    parity = 0x3ul;
    break;
  case 4:
    parity = 0x4ul;
    break;
  case 5:
    parity = 0x5ul;
    break;
  default:
    parity = 0x3ul;
    break;
  }

  switch (stopbits) {
  case 1:
    stop = 0x10ul;
    break;
  case 2:
    stop = 0x30ul;
    break;
  default:
    stop = 0x10ul;
    break;
  }

  config = data | parity | stop;

  return config;
}

uint16_t getSerial1Config(void) {


  uint8_t numbits = USBSer1.numbits();
  uint8_t paritytype = USBSer1.paritytype();
  uint8_t stopbits = USBSer1.stopbits();

  return makeSerialConfig(numbits, paritytype, stopbits);
}

uint16_t getSerial2Config(void) {

  uint8_t numbits = USBSer2.numbits();
  uint8_t paritytype = USBSer2.paritytype();
  uint8_t stopbits = USBSer2.stopbits();

  return makeSerialConfig(numbits, paritytype, stopbits);
}

void checkForConfigChangesUSBSer1(bool print) {

  if (USBSer1.numbits() != numbitsUSBSer1) {
    numbitsUSBSer1 = USBSer1.numbits();
    serConfigChangedUSBSer1 = 1;
  }

  if (USBSer1.paritytype() != paritytypeUSBSer1) {
    paritytypeUSBSer1 = USBSer1.paritytype();
    serConfigChangedUSBSer1 = 1;
  }

  if (USBSer1.stopbits() != stopbitsUSBSer1) {
    stopbitsUSBSer1 = USBSer1.stopbits();
    serConfigChangedUSBSer1 = 1;
  }


  if (USBSer1.baud() != baudRateUSBSer1) {
    baudRateUSBSer1 = USBSer1.baud();
    // USBSer1.begin(baudRate);
    serConfigChangedUSBSer1 = 1;
  }

  if (serConfigChangedUSBSer1 == 3) {
    USBSer1.begin(baudRateUSBSer1, makeSerialConfig(numbitsUSBSer1, paritytypeUSBSer1, stopbitsUSBSer1));
    Serial1.begin(baudRateUSBSer1, makeSerialConfig(numbitsUSBSer1, paritytypeUSBSer1, stopbitsUSBSer1));

    serConfigChangedUSBSer1 = 0;

    Serial.print("Serial1 config changed to ");
    Serial.print(baudRateUSBSer1);
    Serial.print(" ");

    Serial.print(numbitsUSBSer1);
    switch (paritytypeUSBSer1) {
    case 0:
      Serial.print("N");
      break;
    case 1:
      Serial.print("O");
      break;
    case 2:
      Serial.print("E");
      break;
    case 3:
      Serial.print("M");
      break;
    case 4:
      Serial.print("S");
      break;
    default:
      Serial.print("N");
      break;
    }

    Serial.println(stopbitsUSBSer1);

    delay(10);
  } else if (serConfigChangedUSBSer1 == 1) {
    serConfigChangedUSBSer1 = 2;
    delay(10);
  } else if (serConfigChangedUSBSer1 == 2) {
    serConfigChangedUSBSer1 = 3;
    delay(10);
  }
}

void checkForConfigChangesUSBSer2(bool print) {

  if (USBSer2.numbits() != numbitsUSBSer2) {
    numbitsUSBSer2 = USBSer2.numbits();
    serConfigChangedUSBSer2 = 1;
  }

  if (USBSer2.paritytype() != paritytypeUSBSer2) {
    paritytypeUSBSer2 = USBSer2.paritytype();
    serConfigChangedUSBSer2 = 1;
  }

  if (USBSer2.stopbits() != stopbitsUSBSer2) {
    stopbitsUSBSer2 = USBSer2.stopbits();
    serConfigChangedUSBSer2 = 1;
  }


  if (USBSer2.baud() != baudRateUSBSer2) {
    baudRateUSBSer2 = USBSer2.baud();
    // USBSer1.begin(baudRate);
    serConfigChangedUSBSer2 = 1;

  }

  if (serConfigChangedUSBSer2 == 3) {
    USBSer2.begin(baudRateUSBSer2, makeSerialConfig(numbitsUSBSer2, paritytypeUSBSer2, stopbitsUSBSer2));
    Serial2.begin(baudRateUSBSer2, makeSerialConfig(numbitsUSBSer2, paritytypeUSBSer2, stopbitsUSBSer2));

    serConfigChangedUSBSer2 = 0;

    Serial.print("Serial2 config changed to ");
    Serial.print(baudRateUSBSer2);
    Serial.print(" ");

    Serial.print(numbitsUSBSer2);
    switch (paritytypeUSBSer2) {
    case 0:
      Serial.print("N");
      break;
    case 1:
      Serial.print("O");
      break;
    case 2:
      Serial.print("E");
      break;
    case 3:
      Serial.print("M");
      break;
    case 4:
      Serial.print("S");
      break;
    default:
      Serial.print("N");
      break;
    }
    Serial.println(stopbitsUSBSer2);
    delay(10);
  } else if (serConfigChangedUSBSer2 == 1) {
    serConfigChangedUSBSer2 = 2;
    delay(10);
  } else if (serConfigChangedUSBSer2 == 2) {
    serConfigChangedUSBSer2 = 3;
    delay(10);
  }
}

void secondSerialHandler(void) {

  checkForConfigChangesUSBSer1();
  checkForConfigChangesUSBSer2();

  bool actArduinoDTR = USBSer1.dtr();
  bool actRouteableDTR = USBSer2.dtr();

  if(millis()-FirstDTRTime >= 1000) FirstDTR=true;
  if(millis()-ESPBootTime>=1000) ESPBoot=false;

  if(actRouteableDTR != LastRoutableDTR){
    LastRoutableDTR=actRouteableDTR;
    pinMode(GPIO_2_PIN, OUTPUT);
    digitalWrite(GPIO_2_PIN, actRouteableDTR);
  }

  if((actArduinoDTR != LastArduinoDTR)){
    LastArduinoDTR = actArduinoDTR;
    if(actArduinoDTR==0 && FirstDTR){
      FirstDTRTime=millis();
      FirstDTR=false;
    } else if(actArduinoDTR==0 &&!FirstDTR){
      ESPBoot=true;
      ESPBootTime=millis();
      ESPReset();
    }
    if(!ESPBoot){
      SetArduinoResetLine(LOW);
      delay(1);
      SetArduinoResetLine(HIGH);
    }
  }

  if(ManualArduinoReset){
    ManualArduinoReset = false;
    SetArduinoResetLine(LOW);
    delay(1);
    SetArduinoResetLine(HIGH);
  }


  if (USBSer1.available()) {
    char c = USBSer1.read();
    Serial1.write(c);
    // Serial1.print(c);
  }
  if (Serial1.available()) {
    char c = Serial1.read();
    USBSer1.write(c);
    //  Serial.print(c);
  }

  if (USBSer2.available()) {
    char c = USBSer2.read();
    Serial2.write(c);
  }

   if (Serial2.available()) {
    char c = Serial2.read();
    USBSer2.write(c);
    //  Serial.print(c);
  }
}

void SetArduinoResetLine(bool state){
  pinMode(18, OUTPUT_8MA);
  pinMode(19, OUTPUT_8MA);
  digitalWrite(18, state);
  digitalWrite(19, state);
  pinMode(18, INPUT);
  pinMode(19, INPUT);
}

void ESPReset(){
  Serial.println("ESP Boot Mode");
  pinMode(18, OUTPUT);
  pinMode(19, OUTPUT);
  digitalWrite(18, LOW);
  digitalWrite(19, LOW);
  delay(1);
  digitalWrite(19, HIGH);
  delay(2);
  digitalWrite(18, HIGH);
}
void setBaudRate(int baudRate) {}

void arduinoPrint(void) {}

void uploadArduino(void) {}
