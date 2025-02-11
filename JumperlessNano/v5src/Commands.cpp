#include "Commands.h"
#include "CH446Q.h"
#include "FileParsing.h"
#include "Graphics.h"
#include "JumperlessDefinesRP2040.h"
#include "LEDs.h"
#include "MatrixStateRP2040.h"
#include "Menus.h"
#include "NetManager.h"
#include "NetsToChipConnections.h"
#include "Peripherals.h"
#include "PersistentStuff.h"
#include "Probing.h"
#include "RotaryEncoder.h"
volatile int sendAllPathsCore2 =
    0; // this signals the core 2 to send all the paths to the CH446Q
volatile int showLEDsCore2 = 0; // this signals the core 2 to show the LEDs
volatile int showProbeLEDs =
    0; // this signals the core 2 to show the probe LEDs

volatile int core1request = 0; // this signals core 1 wants to do something

unsigned long waitCore2() {

  // delayMicroseconds(60);
  unsigned long timeout = micros();
  core1request = 1;
  while (core2busy || sendAllPathsCore2 != 0) {
    // Serial.println("waiting for core2 to finish");
    if (micros() - timeout > 1000000) {
      core2busy = false;
      sendAllPathsCore2 = 0;
      break;
    }
  }
  // Serial.println(micros() - timeout);

  core1request = 0;
  return micros() - timeout;
}

int lastSlot = netSlot;

void refreshConnections(int ledShowOption, int fillUnused) {

  waitCore2();
  core1busy = true;
  clearAllNTCC();

  // return;
  openNodeFile(netSlot, 0);

  getNodesToConnect();

  bridgesToPaths();
  chooseShownReadings();
  //assignNetColors();

  // if (lastSlot != netSlot) {
  //   createLocalNodeFile(netSlot);
  //   lastSlot = netSlot;
  // }
core1busy = false;

  if (ledShowOption != 0) {

    showLEDsCore2 = ledShowOption;
    waitCore2();
  }
  sendAllPathsCore2 = 1;

  
  
  // sendPaths();
}

void refreshLocalConnections(int ledShowOption, int fillUnused) {

   waitCore2();
   

  clearAllNTCC();

  openNodeFile(netSlot, 1);

  getNodesToConnect();

  bridgesToPaths();
  chooseShownReadings();
  //assignNetColors();

  //  if (lastSlot != netSlot) {
  //   createLocalNodeFile(netSlot);
  //   lastSlot = netSlot;
  // }

  core1busy = false;
  if (ledShowOption != 0) {

    showLEDsCore2 = ledShowOption;
    waitCore2();
  }
  sendAllPathsCore2 = 1;

  // sendPaths();
  
  //waitCore2();
}

void refreshBlind(
    int disconnectFirst,
    int fillUnused) { // this doesn't actually touch the flash so we don't
  // need to wait for core 2
  waitCore2();
  //core1busy = true;
  // fillUnused = 0;
  clearAllNTCC();
  openNodeFile(netSlot, 1);
  core1busy = true;
  getNodesToConnect();
  bridgesToPaths();
  assignNetColors();

  // printPathsCompact();
  core1busy = false;
  //   if (lastSlot != netSlot) {
  //   createLocalNodeFile(netSlot);
  //   lastSlot = netSlot;
  // }
  if (disconnectFirst == 1) {
    sendAllPathsCore2 = 1;
  } else if (disconnectFirst == 0) {
    sendAllPathsCore2 = 1;
  } else {
    sendAllPathsCore2 = 1; // disconnectFirst;
  }

  chooseShownReadings();
  // sendPaths();
  //core1busy = false;
  waitCore2();
}

struct rowLEDs getRowLEDdata(int row) {

  struct rowLEDs rowLEDs = {0, 0, 0, 0, 0};
  // uint8_t *pixelPointer = leds.getPixels();
  if (row < 1) {
    return rowLEDs;
  } else if (row >= 70 && row < 125) {
    // row = row - 1;
    for (int i = 0; i < 35; i++) { // stored in GRB order
      if (bbPixelToNodesMapV5[i][0] == row) {
        rowLEDs.color[0] = leds.getPixelColor(bbPixelToNodesMapV5[i][1]);
        return rowLEDs;
      }
    }

    // Serial.print(row);
    // Serial.print(" ");
    // Serial.println(rowLEDs.color[0]);

    return rowLEDs;
  }
  row = row - 1;
  for (int i = 0; i < 5; i++) { // stored in GRB order
  rowLEDs.color[i] = 0x000000;
    rowLEDs.color[i] = leds.getPixelColor(row * 5 + i);
    // rowLEDs.color[i] = packRgb(pixelPointer[15 * row + (3 * i) + 1],
    //                            pixelPointer[15 * row + (3 * i)],
    //                            pixelPointer[15 * row + (3 * i) + 2]);
    // Serial.print(row * 5 + i);
    // Serial.print(" ");
    // Serial.println(leds.getPixelColor(row * 5 + i));
  }

  return rowLEDs;
}

void setRowLEDdata(int row, struct rowLEDs rowLEDcolors) {

  // uint8_t *pixelPointer = leds.getPixels();
  if (row < 1 || row > 125) {
    return;
  } else if (row >= 70 && row < 125) {
    // row = row - 1;
    rgbColor colorrgb = unpackRgb(rowLEDcolors.color[0]);
    for (int i = 0; i < 35; i++) { // stored in GRB order
      if (bbPixelToNodesMapV5[i][0] == row) {
        leds.setPixelColor(bbPixelToNodesMapV5[i][1], colorrgb.r, colorrgb.g,
                           colorrgb.b);
        return;
      }
    }
    return;
  }
  row = row - 1;
  for (int i = 0; i < 5; i++) { // stored in GRB order

    leds.setPixelColor(row * 5 + i, rowLEDcolors.color[i]);
    // rgbColor colorrgb = unpackRgb(rowLEDcolors.color[i]);
    // pixelPointer[15 * row + (3 * i) + 1] = colorrgb.r;
    // pixelPointer[15 * row + (3 * i)] = colorrgb.g;
    // pixelPointer[15 * row + (3 * i) + 2] = colorrgb.b;
  }
  return;
}

void connectNodes(int node1, int node2) {

  if (node1 == node2 || node1 < 1 || node2 < 1) {
    return;
  }
  if ((node1 > 60 && node1 < 70) || (node2 > 60 && node2 < 70)) {
    return;
  }

  addBridgeToNodeFile(node1, node2, netSlot, 0);

  refreshConnections();
  waitCore2();
  // createLocalNodeFile(netSlot);
}

void disconnectNodes(int node1, int node2) {
  removeBridgeFromNodeFile(node1, node2, netSlot, 0);
  refreshConnections();
  waitCore2();
}

float measureVoltage(int adcNumber, int node, bool checkForFloating) {
  int adcDefine = 0;

  switch (adcNumber) {
  case 1:
    adcDefine = ADC1;
    break;
  case 2:
    adcDefine = ADC2;
    break;
  case 3:
    adcDefine = ADC3;
    break;
  case 4:
    adcDefine = ADC4;
    break;
  case 5:
    // adcDefine = ADC5;
    break;
  case 6:
    // adcDefine = ADC6;
    break;
  case 7:
    adcDefine = ADC7;
    break;
  default:
    return 0.0;
  }

  // removeBridgeFromNodeFile(adcDefine, -1, netSlot, 0);

  // delay(2);
  waitCore2();
  addBridgeToNodeFile(node, adcDefine, netSlot, 1);
  //refreshLocalConnections(-1 , 0);
  refreshBlind(-1);
  //         printPathsCompact();
  // printChipStatus();

  // Serial.println(readAdc(adcNumber, 32) * (5.0 / 4095));
  // core1busy = true;
  float voltage = 0.0;
  if (adcDefine == ADC1) {
    voltage = (float)((readAdc(adcNumber, 4) - 50) * (5.0 / 4096));
  } else {
    voltage = (float)((readAdc(adcNumber, 4)) * (16.0 / 4096));
    voltage -= 8.0;
  }

  int floating = 0;
  if (checkForFloating == true) {
    if (voltage < 0.8 && voltage > -0.8) {

      if (checkFloating(node) == true) {
        floating = 1;
      }
    }
    waitCore2();
  }
  removeBridgeFromNodeFile(node, adcDefine, netSlot, 1);
  refreshBlind();

  if (floating == 1) {

    return 0xFFFFFFFF;
  }

  return voltage;
}

bool checkFloating(int node) {
  // delay(2);
  // Serial.print("node = ");
  // Serial.println(node);
  int gpioNumber = RP_GPIO_1;
  int gpioPin = GPIO_1_PIN;

  switch (node) {

  case 1 ... 60: {
    for (int i = 0; i < 4; i++) {
      if (ch[11].xStatus[i + 4] == -1) {
        gpioNumber = RP_GPIO_1 + i;
        break;
      }
    }
    break;
  }

  case 70 ... 120: {
    for (int i = 0; i < 12; i++) {
      if (ch[8].xMap[i] == node) {
        gpioNumber = RP_UART_RX;
        break;
      }
      if (ch[9].xMap[i] == node) {
        gpioNumber = RP_UART_TX;
        break;
      }
    }

    break;
  }
  default:
    return true;
  }
  // Serial.print("gpioNumber = ");
  // Serial.println(gpioNumber);

  switch (gpioNumber) {
  case RP_GPIO_1:
    gpioPin = GPIO_1_PIN;
    break;
  case RP_GPIO_2:

    gpioPin = GPIO_2_PIN;
    break;
  case RP_GPIO_3:

    gpioPin = GPIO_3_PIN;
    break;
  case RP_GPIO_4:
    gpioPin = GPIO_4_PIN;
    break;
  case RP_UART_RX:
    gpioPin = GPIO_RX_PIN;
    break;
  case RP_UART_TX:
    gpioPin = GPIO_TX_PIN;
    break;
  }
  // Serial.print("gpioPin = ");
  // Serial.println(gpioPin);

  // removeBridgeFromNodeFile(gpioNumber, -1, netSlot, 1);
  addBridgeToNodeFile(node, gpioNumber, netSlot, 1);
  refreshBlind();

  pinMode(gpioPin, INPUT_PULLUP);
  delayMicroseconds(30);
  int reading = digitalRead(gpioPin);

  waitCore2();
  if (reading == HIGH) {

    removeBridgeFromNodeFile(node, gpioNumber, netSlot, 1);

    return true;
  } else {
    removeBridgeFromNodeFile(node, gpioNumber, netSlot, 1);
    return false;
  }
}

float measureCurrent(int node1, int node2) { return 0; }

void setRailVoltage(int topBottom, float voltage) {
  switch (topBottom) {
  case 0:
    setTopRail(voltage, 1);
    break;
  case 1:
    setBotRail(voltage, 1);
    break;
  default:
    break;
  }

  return;
}

void connectGPIO(int gpioNumber, int node) {

  switch (gpioNumber) {
  case 1:
    gpioNumber = RP_GPIO_1;
    break;
  case 2:
    gpioNumber = RP_GPIO_2;
    break;
  case 3:
    gpioNumber = RP_GPIO_3;
    break;
  case 4:
    gpioNumber = RP_GPIO_4;
    break;
  case 5:
    gpioNumber = RP_GPIO_5;
    break;
  case 6:
    gpioNumber = RP_GPIO_6;
    break;
  case 7:
    gpioNumber = RP_GPIO_7;
    break;
  case 8:
    gpioNumber = RP_GPIO_8;
    break;
  }
  addBridgeToNodeFile(gpioNumber, node, netSlot, 0);
  refreshConnections();
}

void printSlots(int fileNo) {
  if (fileNo == -1)

    if (Serial.available() > 0) {
      fileNo = Serial.read();
      // break;
    }

  Serial.print("\n\n\r");
  if (fileNo == -1) {
    Serial.print("\tSlot Files");
  } else {
    Serial.print("\tSlot File ");
    Serial.print(fileNo - '0');
  }
  Serial.print("\n\n\r");
  Serial.print(
      "\n\ryou can paste this text reload this circuit (enter 'o' first)");
  Serial.print("\n\r(or even just a single slot)\n\n\n\r");
  if (fileNo == -1) {
    for (int i = 0; i < NUM_SLOTS; i++) {
      Serial.print("\n\rSlot ");
      Serial.print(i);
      if (i == netSlot) {
        Serial.print("        <--- current slot");
      }

      Serial.print("\n\rnodeFileSlot");
      Serial.print(i);
      Serial.print(".txt\n\r");
      if (getSlotLength(i, 0) > 0) {
        Serial.print("\n\rf ");
        printNodeFile(i);
        Serial.print("\n\n\r");
      }
    }
  } else {

    Serial.print("\n\rnodeFileSlot");
    Serial.print(fileNo - '0');
    Serial.print(".txt\n\r");

    Serial.print("\n\rf ");

    printNodeFile(fileNo - '0');
    Serial.print("\n\r");
  }
}