

#include "Probing.h"
#include "CH446Q.h"
#include "FileParsing.h"
#include "JumperlessDefinesRP2040.h"
#include "LEDs.h"
#include "MatrixStateRP2040.h"
#include "NetManager.h"
#include "NetsToChipConnections.h"
#include "Peripherals.h"
// #include "AdcUsb.h"
#include "Commands.h"
#include "Graphics.h"
#include "PersistentStuff.h"
#include "RotaryEncoder.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"

#include <EEPROM.h>
// #include <FastLED.h>
#include "ArduinoStuff.h"
#include <algorithm>
int debugProbing = 0;

volatile unsigned long blockProbing = 0;
volatile unsigned long blockProbingTimer = 0;



int switchPosition = 1;
int showProbeCurrent = 0;

volatile bool bufferPowerConnected = 0;
int probeToRowMap2[102] = {
    0,  1,  2,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15,  16,  17,
    18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,  33,  34,
    35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,  50,  51,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66,  67,  68,
    69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83,  84,  85,
    86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101,
};
int lastReadRaw = 0;
int probeSwap = 0;
int probeHalfPeriodus = 20;

unsigned long probingTimer = 0;
long probeFrequency = 25000;

unsigned long probeTimeout = 0;

int lastFoundIndex = 0;

//int lastFoundHistory[50] = {-1};

int connectedRowsIndex = 0;
int connectedRows[32] = {-1};

int connectedRows2Index[4] = {0, 0, 0, 0};
int connectedRows2[4][32];

int nodesToConnect[2] = {-1, -1};

int node1or2 = 0;

int probePin = 10;
int buttonPin = 9;

unsigned long probeButtonTimer = 0;

int voltageSelection = SUPPLY_3V3;
int voltageChosen = 0;
volatile int connectOrClearProbe = 0;

volatile int inPadMenu = 0;
int rainbowList[13][3] = {
    {40, 50, 80}, {88, 33, 70}, {30, 15, 45}, {8, 27, 45},  {45, 18, 19},
    {35, 42, 5},  {02, 45, 35}, {18, 25, 45}, {40, 12, 45}, {10, 32, 45},
    {18, 5, 43},  {45, 28, 13}, {8, 12, 8}};
int rainbowIndex = 0;

int nextIsSupply = 0;
int nextIsGnd = 0;
int justCleared = 1;

int checkingPads = 0;
int probeHighlight = -1;

int justAttached = 0;
uint32_t deleteFade[13] = {0x371f16, 0x28160b, 0x191307, 0x141005, 0x0f0901,
                           0x090300, 0x050200, 0x030100, 0x030000, 0x020000,
                           0x010000, 0x000000, 0x000000};
int fadeIndex = 0;
volatile int removeFade = 0;
int probeMode(int pin, int setOrClear) {


extern int minProbeReadingMap;
extern int maxProbeReadingMap;



  if (checkingPads == 1) {
    Serial.println("checkingPads\n\r");
    return -1;
  }

  int deleteMisses[20] = {
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  };
  int deleteMissesIndex = 0;

  // clearLEDsExceptRails();
  // startProbe();

  // createLocalNodeFile(netSlot);
  routableBufferPower(1);

  if (checkSwitchPosition() == 1) {
    routableBufferPower(1);
    // Serial.println("Select");
  } else {
    // showProbeLEDs = 3;
    // probeActive = 0;
    // Serial.println("Measure");

    // routableBufferPower(0);
    // measureMode();
    // showProbeLEDs = 6;
    // return -1;
  }
  // calibrateDac0();
restartProbing:
  connectOrClearProbe = setOrClear;
  probeActive = 1;
  brightenNet(-1);
  if (bufferPowerConnected == false) {
    routableBufferPower(1);
  }
  probeHighlight = -1;
  saveLocalNodeFile();

  int numberOfLocalChanges = 0;

  connectOrClearProbe = setOrClear;
  int lastRow[10];

  // Serial.print(numberOfNets);

  if (numberOfNets == 0) {
    // clearNodeFile(netSlot);
  }

  int probedNodes[40][2];
  int probedNodesIndex = 0;

  int row[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  row[1] = -2;
  row[0] = -2;
  probeTimeout = millis();

  // probingTimer = millis();

  // Serial.print("Press any key to exit and commit paths (or touch probe to
  // gpio 18)   ");
  // Serial.print("\n\r\t  Probe Active\n\r");
  // Serial.print("   long press  = connect (blue) / clear (red)\n\r");
  // Serial.println("   short press = commit");

  if (setOrClear == 1) {
    // sprintf(oledBuffer, "connect  ");
    // drawchar();
    Serial.println("\r\t connect nodes (blue)\n\r");
    rawOtherColors[1] = 0x4500e8;
    rainbowIndex = 0;
  } else {
    // sprintf(oledBuffer, "clear");
    // drawchar();
    Serial.println("\r\t clear nodes (red)\n\r");
    rawOtherColors[1] = 0x6644A8;
    rainbowIndex = 12;
  }

  if (setOrClear == 0) {
    probeButtonTimer = millis();

    // probingTimer = millis() -400;
  }

  if (setOrClear == 1) {
    showProbeLEDs = 1;
  } else {
    showProbeLEDs = 2;
  }

  // Serial.print("setOrClear = ");
  // Serial.println(setOrClear);

  // Serial.print("showProbeLEDs = ");
  // Serial.println(showProbeLEDs);
  showLEDsCore2 = 1;
  unsigned long doubleSelectTimeout = millis();
  int doubleSelectCountdown = 0;

  int lastProbedRows[4] = {0, 0, 0, 0};
  unsigned long fadeTimer = millis();
  int fadeClear = -1;

  // Serial.print("\n\r");
  // Serial.println(setOrClear);
  while (Serial.available() == 0 && (millis() - probeTimeout) < 6200 &&
         encoderButtonState == IDLE) {
    delayMicroseconds(500);

    connectedRowsIndex = 0;

    row[0] = readProbe();
    // Serial.println(row[0]);

    // probeButtonToggle = checkProbeButton();

    if (setOrClear == 1) {
      deleteMissesIndex = 0;
      if (millis() - fadeTimer > 500) {
        fadeTimer = millis();
        if (numberOfLocalChanges > 0) {
          saveLocalNodeFile(netSlot);
          // Serial.print("\n\r");
          // Serial.print("saving local node file\n\r");

          // refreshConnections();
          numberOfLocalChanges = 0;
        }
      }
      // showLEDsCore2 = -1;

    } else {
      if (millis() - fadeTimer > 10) {
        fadeTimer = millis();

        if (fadeIndex < 12) {
          fadeIndex++;
          if (removeFade > 0) {
            removeFade--;
            showProbeLEDs = 2;
          }
        } else {
          deleteMissesIndex = 0;
          for (int i = 0; i < 20; i++) {
            deleteMisses[i] = -1;
          }
        }

        int fadeFloor = fadeIndex;
        if (fadeFloor < 0) {
          fadeFloor = 0;
        }
        // Serial.println(deleteMissesIndex);
        // Serial.println(fadeIndex);

        //   if (fadeIndex == -2) {
        //  removeFade = 10;
        //   }

        // Serial.println(removeFade);

        for (int i = deleteMissesIndex - 1; i >= 0; i--) {
          int fadeOffset = map(i, 0, deleteMissesIndex, 0, 12) + fadeFloor;
          if (fadeOffset > 12) {
            fadeOffset = 12;
            // showLEDsCore2 = -1;
          }
          // clearLEDsExceptMiddle(deleteMisses[i], -1);
          b.printRawRow(0b00000100, deleteMisses[i] - 1, deleteFade[fadeOffset],
                        0xfffffe);

          //   Serial.print(i);
          //   Serial.print("   ");
          //   Serial.print(deleteMisses[i]);
          //   Serial.print("    ");
          //  Serial.println(map(i, 0,deleteMissesIndex, 0, 19));
        }
        if (deleteMissesIndex == 0 && fadeClear == 0) {
          fadeClear = 1;
          showLEDsCore2 = -1;
          if (numberOfLocalChanges > 0) {
            saveLocalNodeFile(netSlot);
            // Serial.print("\n\r");
            // Serial.print("saving local node file\n\r");

            // refreshConnections();
            numberOfLocalChanges = 0;
          }
        }
      }
    }

    if ((row[0] == -18 || row[0] == -16) &&
        (millis() - probingTimer > 500)) { //&&

      if (row[0] == -18) {

        if (setOrClear == 0) {
          nodesToConnect[0] = -1;
          nodesToConnect[1] = -1;
          node1or2 = 0;
        } else {

          setOrClear = 0;
          probingTimer = millis();
          probeButtonTimer = millis();

          sfProbeMenu = 0;
          connectedRowsIndex = 0;
          connectedRows[0] = -1;
          connectedRows[1] = -1;
          nodesToConnect[0] = -1;
          nodesToConnect[1] = -1;
          //           lastProbedRows[0] = -1;
          // lastProbedRows[1] = -1;
          showLEDsCore2 = -1;
          node1or2  = 0;
          // showProbeLEDs = 1;
          goto restartProbing;
        }
        // break;
      } else if (row[0] == -16) {

        if (setOrClear == 1) {
          // showProbeLEDs = 2;
          //  delay(100);
        } else {
          setOrClear = 1;
          // showProbeLEDs = 2;

          probingTimer = millis();
          probeButtonTimer = millis();
          // showNets();
          // showLEDsCore2 = 1;
          sfProbeMenu = 0;
          connectedRowsIndex = 0;
          connectedRows[0] = -1;
          connectedRows[1] = -1;
          nodesToConnect[0] = -1;
          nodesToConnect[1] = -1;
          node1or2 = 0;
          // lastProbedRows[0] = -1;
          // lastProbedRows[1] = -1;

          for (int i = deleteMissesIndex - 1; i >= 0; i--) {

            b.printRawRow(0b00000100, deleteMisses[i] - 1, 0, 0xfffffe);
            //   Serial.print(i);
            //   Serial.print("   ");
            //   Serial.print(deleteMisses[i]);
            //   Serial.print("    ");
            //  Serial.println(map(i, 0,deleteMissesIndex, 0, 19));
          }
          showLEDsCore2 = -1;

          goto restartProbing;
        }
        // break;
      }

      // Serial.print("\n\rCommitting paths!\n\r");
      row[1] = -2;
      probingTimer = millis();

      connectedRowsIndex = 0;

      node1or2 = 0;
      nodesToConnect[0] = -1;
      nodesToConnect[1] = -1;
      probeHighlight = -1;
      break;
    } else {
      // probingTimer = millis();
    }

    if (row[0] != -1 && row[0] != row[1]) { // && row[0] != lastProbedRows[0] &&
      // row[0] != lastProbedRows[1]) {

      lastProbedRows[1] = lastProbedRows[0];
      lastProbedRows[0] = row[0];
      if (connectedRowsIndex == 1) {
        nodesToConnect[node1or2] = connectedRows[0];
        printNodeOrName(nodesToConnect[0]);

        Serial.print("\r\t");
        probeHighlight = nodesToConnect[node1or2];
        if (setOrClear == 1) {
          brightenNet(probeHighlight, 5);
        }

        // Serial.print("probing Highlight: ");
        // Serial.println(probeHighlight);
        // showProbeLEDs = 1;

        if (nodesToConnect[node1or2] > 0 &&
            nodesToConnect[node1or2] <= NANO_RESET_1 && setOrClear == 1) {

          // b.clear();
          b.printRawRow(0b0010001, nodesToConnect[node1or2] - 1, 0x000121e,
                        0xfffffe);
          showLEDsCore2 = 2;
          delay(40);
          b.printRawRow(0b00001010, nodesToConnect[node1or2] - 1, 0x0f0498,
                        0xfffffe);
          showLEDsCore2 = 2;
          delay(40);

          b.printRawRow(0b00000100, nodesToConnect[node1or2] - 1, 0x4000e8,
                        0xfffffe);
          showLEDsCore2 = 2;
          delay(80);
          // showLEDsCore2 = 1;
        }

        node1or2++;
        probingTimer = millis();
        showLEDsCore2 = 1;
        doubleSelectTimeout = millis();
        doubleSelectCountdown = 200;
        // delay(500);

        // delay(3);
      }

      if (node1or2 >= 2 || (setOrClear == 0 && node1or2 >= 1)) {

        probeHighlight = -1;

        if (setOrClear == 1 && (nodesToConnect[0] != nodesToConnect[1]) &&
            nodesToConnect[0] > 0 && nodesToConnect[1] > 0) {
          b.printRawRow(0b00011111, nodesToConnect[0] - 1, 0x0, 0x00000000);
          b.printRawRow(0b00011111, nodesToConnect[1] - 1, 0x0, 0x00000000);
          Serial.print("\r           \r");

          printNodeOrName(nodesToConnect[0]);
          Serial.print(" - ");
          printNodeOrName(nodesToConnect[1]);

          Serial.print("   \t ");
          Serial.print("connected\n\r");

          char node1Name[12];

          strcpy(node1Name, definesToChar(nodesToConnect[0]));

          char node2Name[12];

          strcpy(node2Name, definesToChar(nodesToConnect[1]));

          addBridgeToNodeFile(nodesToConnect[0], nodesToConnect[1], netSlot, 1);
          numberOfLocalChanges++;
          brightenNet(-1);
          // Serial.println(numberOfLocalChanges);
          refreshLocalConnections(-1);
          fadeTimer = millis();
          // if (numberOfLocalChanges > 5) {
          //   saveLocalNodeFile(netSlot);
          //   // refreshConnections();
          //   numberOfLocalChanges = 0;

          // } // else {
          //  saveLocalNodeFile(netSlot);

          //}

          row[1] = -1;

          // doubleSelectTimeout = millis();
          for (int i = 0; i < 12; i++) {
            deleteMisses[i] = -1;
          }

          doubleSelectTimeout = millis();
          doubleSelectCountdown = 200;

//             Serial.print("\n\n\rPaths: \n\r");
//   printPathsCompact();
//   Serial.print("\n\n\n\rChip Status: \n\r");
// printChipStatus();
// Serial.println("\n\n\r");

          // delay(400);
        } else if (setOrClear == 0) {

          Serial.print("\r                      \r");
          printNodeOrName(nodesToConnect[0]);

          for (int i = 12; i > 0; i--) {
            deleteMisses[i] = deleteMisses[i - 1];
            // Serial.print(i);
            // Serial.print("   ");
            // Serial.println(deleteMisses[i]);
          }
          // Serial.print("\n\r");
          deleteMisses[0] = nodesToConnect[0];

          // deleteMisses[deleteMissesIndex] = nodesToConnect[0];
          if (deleteMissesIndex < 12) {
            deleteMissesIndex++;
          }
          fadeIndex = -3;

          //  Serial.println("\n\r");
          //  Serial.print("deleteMissesIndex: ");
          //   Serial.print(deleteMissesIndex);
          //   Serial.print("\n\r");
          for (int i = deleteMissesIndex - 1; i >= 0; i--) {

            b.printRawRow(0b00000100, deleteMisses[i] - 1,
                          deleteFade[map(i, 0, deleteMissesIndex, 0, 12)],
                          0xfffffe);
            //   Serial.print(i);
            //   Serial.print("   ");
            //   Serial.print(deleteMisses[i]);
            //   Serial.print("    ");
            //  Serial.println(map(i, 0,deleteMissesIndex, 0, 19));
          }

          //  Serial.println();
          int rowsRemoved =
              removeBridgeFromNodeFile(nodesToConnect[0], -1, netSlot, 1);
          // numberOfLocalChanges += rowsRemoved;
          if (rowsRemoved > 0) {

            removeFade = 10;
            rainbowIndex = 12;

            // goto restartProbing;
            numberOfLocalChanges += rowsRemoved;
            // Serial.println(numberOfLocalChanges);
            // clearLEDsExceptMiddle(1,60);
            refreshLocalConnections();

            showLEDsCore2 = -6;
            Serial.print("\t cleared");
            Serial.println();
            // else {

            // showLEDsCore2 = -1;
            // }
            //  refreshLocalConnections(1);
            //   deleteMissesIndex = 0;
            //   for (int i = 0; i < 20; i++) {
            //     deleteMisses[i] = -1;
            //   }
            //   delay(20);
            //  showLEDsCore2 = -1;
            fadeClear = 0;
            fadeTimer = 0;
          }
        }
        node1or2 = 0;
        nodesToConnect[0] = -1;
        nodesToConnect[1] = -1;
        // row[1] = -2;
        doubleSelectTimeout = millis();
      }

      row[1] = row[0];
    }
    // Serial.print("\n\r");
    // Serial.print(" ");
    // Serial.println(row[0]);



    if (millis() - doubleSelectTimeout > 700) {
      // Serial.println("doubleSelectCountdown");
      row[1] = -2;
      lastReadRaw = 0;
      lastProbedRows[0] = 0;
      lastProbedRows[1] = 0;
      doubleSelectTimeout = millis();
      doubleSelectCountdown = 700;
    }

    // Serial.println(doubleSelectCountdown);

    if (doubleSelectCountdown <= 0) {

      doubleSelectCountdown = 0;
    } else {
      doubleSelectCountdown =
          doubleSelectCountdown - (millis() - doubleSelectTimeout);

      doubleSelectTimeout = millis();
    }

    probeTimeout = millis();
  }
  // Serial.println("fuck you");
  //  digitalWrite(RESETPIN, LOW);

  probeActive = false;
  probeHighlight = -1;
  showProbeLEDs = 4;
  brightenNet(-1);

  // showLEDsCore2 = -1;
  refreshLocalConnections(0);
  // delay(10);
  saveLocalNodeFile();
  // delay(10);
  refreshConnections(0);
  row[1] = -2;
  showLEDsCore2 = -1;
  // sprintf(oledBuffer, "        ");
  // drawchar();

  // rotaryEncoderMode = wasRotaryMode;
  // routableBufferPower(0);
  // delay(10);

  return 1;
}

volatile int measureModeActive = 0;

float measureMode(int updateSpeed) {
  measureModeActive = 1;
  while (checkProbeButton() != 0) {
    delay(1);
  }
  //   removeBridgeFromNodeFile(ROUTABLE_BUFFER_IN, -1, netSlot, 1);
  // removeBridgeFromNodeFile(ROUTABLE_BUFFER_OUT, -1, netSlot, 1);
  //   addBridgeToNodeFile(ROUTABLE_BUFFER_OUT, ADC3, netSlot, 1);

  // refreshLocalConnections();
  float measurement = 0.0;
  while (checkProbeButton() == 0) {
    measurement = (readAdc(7, 16) * (16.0 / 4090)) - 8.0;
    if (measurement > -0.05 && measurement < 0.05) {
      measurement = 0.0;
      delay(1);
    }
    uint32_t measColor = scaleBrightness(
        logoColors8vSelect[map((long)(measurement * 10), -80, 80, 0, 59)], -50);
    // Serial.println(map((long)(measurement*10), -80, 80, 0, 59));
    char measChar[10] = "         ";
    // b.print("        ", (uint32_t)0x000000,(uint32_t)0xffffff);
    b.clear(0);
    sprintf(measChar, "% .1f V", measurement);

    b.print(measChar, (uint32_t)measColor, (uint32_t)0xfffffe);

    Serial.print("                        \r");
    Serial.print(measChar);

    delay(updateSpeed);
  }
  // removeBridgeFromNodeFile(ROUTABLE_BUFFER_OUT, -1, netSlot, 1);

  // addBridgeToNodeFile(ROUTABLE_BUFFER_IN, RP_GPIO_23, netSlot, 1);

  // refreshLocalConnections(-1);
  // delay(20);
  measureModeActive = 0;
  showProbeLEDs = 3;
  return measurement;
}

unsigned long blinkTimer = 0;
volatile int sfProbeMenu = 0;

uint32_t sfOptionColors[12] = {
    0x09000a, 0x0d0500, 0x000809, 0x040f00, 0x000f03, 0x00030d,
    0x080a00, 0x030013, 0x000a03, 0x00030a, 0x040010, 0x070006,
};

int selectSFprobeMenu(int function) {

  if (checkingPads == 1) {
    // inPadMenu = 0;
    return function;
  }
  inPadMenu = 1;
  switch (function) {

  case 132: {
    inPadMenu = 1;
    function = chooseADC();
    blockProbing = 800;
    blockProbingTimer = millis();
    // delay(10);
    inPadMenu = 0;

    break;
  }
  case 131: {
    inPadMenu = 1;
    function = chooseDAC();
    blockProbing = 800;
    blockProbingTimer = millis();
    // delay(10);
    inPadMenu = 0;

    break;
  }
  case 130: {

    function = chooseGPIO();
    blockProbing = 800;
    blockProbingTimer = millis();
    // delay(10);

    break;
  }
  case 128 ... 129:
  case 133 ... 134: {
    // b.clear();
    //     function = -1;
    // break;
    // b.clear();
    clearLEDsExceptRails();
    switch (function) {
    case 128: {
      inPadMenu = 1;
      b.print("UART", sfOptionColors[3], 0xFFFFFF, 0, 0, 0);
      b.print("Tx", sfOptionColors[7], 0xFFFFFF, 0, 1, 0);
      b.printRawRow(0b00000001, 23, 0x400014, 0xffffff);
      b.printRawRow(0b00000011, 24, 0x400014, 0xffffff);
      b.printRawRow(0b00011111, 25, 0x400014, 0xffffff);
      b.printRawRow(0b00011011, 26, 0x400014, 0xffffff);
      b.printRawRow(0b00000001, 27, 0x400014, 0xffffff);

      b.printRawRow(0b00011100, 53, 0x400014, 0xffffff);
      b.printRawRow(0b00011000, 54, 0x400014, 0xffffff);
      b.printRawRow(0b00010000, 55, 0x400014, 0xffffff);
      function = RP_UART_TX;
      break;
    }
    case 129: {
      inPadMenu = 1;
      b.print("UART", sfOptionColors[3], 0xFFFFFF, 0, 0, -1);
      b.print("Rx", sfOptionColors[5], 0xFFFFFF, 0, 1, -1);

      b.printRawRow(0b00000000, 25, 0x280032, 0xffffff);
      b.printRawRow(0b00000001, 26, 0x280032, 0xffffff);
      b.printRawRow(0b00000011, 27, 0x280032, 0xffffff);

      b.printRawRow(0b00001110, 53, 0x280032, 0xffffff);
      b.printRawRow(0b00011110, 54, 0x280032, 0xffffff);
      b.printRawRow(0b00010000, 55, 0x280032, 0xffffff);
      b.printRawRow(0b00011111, 56, 0x280032, 0xffffff);
      b.printRawRow(0b00011111, 57, 0x280032, 0xffffff);
      b.printRawRow(0b00000011, 58, 0x280032, 0xffffff);

      b.printRawRow(0b00000001, 52, 0x050500, 0xfffffe);
      b.printRawRow(0b00000001, 53, 0x050500, 0xfffffe);
      b.printRawRow(0b00000001, 54, 0x050500, 0xfffffe);
      b.printRawRow(0b00000001, 55, 0x050500, 0xfffffe);
      b.printRawRow(0b00000001, 59, 0x050500, 0xfffffe);
      function = RP_UART_RX;
      break;
    }
    case 133: {
      inPadMenu = 1;
      b.print("Buildng", sfOptionColors[6], 0xFFFFFF, 0, 1, -1);
      b.print("Top", sfOptionColors[7], 0xFFFFFF, 0, 0, 1);

      b.printRawRow(0b00011000, 24, 0x200010, 0xffffff);
      b.printRawRow(0b00011000, 25, 0x200010, 0xffffff);
      b.printRawRow(0b00011000, 26, 0x200010, 0xffffff);
      b.printRawRow(0b00011000, 27, 0x200010, 0xffffff);

      b.printRawRow(0b00000011, 24, 0x010201, 0xfffffe);
      b.printRawRow(0b00000011, 25, 0x010201, 0xfffffe);
      b.printRawRow(0b00000011, 26, 0x010201, 0xfffffe);
      b.printRawRow(0b00000011, 27, 0x010201, 0xfffffe);

      break;
    }
    case 134: {
      inPadMenu = 1;
      b.print("Buildng", sfOptionColors[6], 0xFFFFFF, 0, 1, -1);
      b.print("Bottom", sfOptionColors[5], 0xFFFFFF, 0, 0, -1);

      b.printRawRow(0b00000011, 25, 0x200010, 0xffffff);
      b.printRawRow(0b00000011, 26, 0x200010, 0xffffff);
      b.printRawRow(0b00000011, 27, 0x200010, 0xffffff);
      b.printRawRow(0b00000011, 28, 0x200010, 0xffffff);

      b.printRawRow(0b00011000, 25, 0x010201, 0xfffffe);
      b.printRawRow(0b00011000, 26, 0x010201, 0xfffffe);
      b.printRawRow(0b00011000, 27, 0x010201, 0xfffffe);
      b.printRawRow(0b00011000, 28, 0x010201, 0xfffffe);
      break;
    }
    }
    // showLEDsCore2 = 2;
    delayWithButton(900);

    // b.clear();
    clearLEDsExceptRails();

    // lastReadRaw = 0;
    // b.print("Attach", sfOptionColors[0], 0xFFFFFF, 0, 0, -1);
    // b.print("to Pad", sfOptionColors[2], 0xFFFFFF, 0, 1, -1);
    // showLEDsCore2 = 2;

    // delayWithButton(800);

    // delay(800);

    // function = attachPadsToSettings(function);
    //  node1or2 = 0;
    //  nodesToConnect[0] = function;
    //  nodesToConnect[1] = -1;
    //  connectedRowsIndex = 1;

    // Serial.print("function!!!!!: ");
    // printNodeOrName(function, 1);
    showLEDsCore2 = 1;
    lightUpRail();
    delay(200);
    inPadMenu = 0;
    sfProbeMenu = 0;
    // return function;

    delay(100);

    break;
  }

  case 0: {
    Serial.print("0function: ");
    printNodeOrName(function, 1);
    Serial.print(function);
    Serial.println();
    function = -1;
    break;
  }
  case 104:
  case 126: {
    function = 100;
    break;
  }
  default: {
    // inPadMenu = 0;
  }
  }
  connectedRows[0] = function;
  connectedRowsIndex = 1;
  lightUpRail();
  // delay(500);
  // showLEDsCore2 = 1;
  // delayWithButton(900);
  sfProbeMenu = 0;
  inPadMenu = 0;

  return function;
}

int logoTopSetting[2] = {-1, -1}; //{function, settingOption}
int logoBottomSetting[2] = {-1, -1};
int buildingTopSetting[2] = {-1, -1};
int buildingBottomSetting[2] = {-1, -1};

int attachPadsToSettings(int pad) {
  int function = -1;
  int functionSetting = -1; // 0 = DAC, 1 = ADC, 2 = GPIO
  int settingOption =
      -1; // 0 = toggle, 1 = up/down, 2 = pwm, 3 = set voltage, 4 = input
  int dacChosen = -1;
  int adcChosen = -1;
  int gpioChosen = -1;
  connectedRowsIndex = 0;
  connectedRows[0] = -1;
  node1or2 = 0;
  unsigned long skipTimer = millis();
  inPadMenu = 1;
  b.clear();
  clearLEDsExceptRails();
  // showLEDsCore2 = 2;
  //   lastReadRaw = 0;
  b.print("DAC", sfOptionColors[0], 0xFFFFFF, 0, 0, -1);
  b.print("ADC", sfOptionColors[1], 0xFFFFFF, 4, 0, 0);
  b.print("GPIO", sfOptionColors[2], 0xFFFFFF, 8, 1, 1);

  int selected = -1;

  while (selected == -1 && longShortPress(500) != 1 &&
         longShortPress(500) != 2) {
    int reading = justReadProbe();
    if (reading != -1) {
      switch (reading) {
      case 1 ... 13: {
        selected = 0;
        functionSetting = 0;
        dacChosen = chooseDAC(1);
        Serial.print("dacChosen: ");
        Serial.println(dacChosen);
        // b.clear();
        settingOption = dacChosen - DAC0;
        clearLEDsExceptRails();
        // showLEDsCore2 = 1;

        break;
      }
      case 18 ... 30: {
        selected = 1;
        functionSetting = 1;
        adcChosen = chooseADC();
        Serial.print("adcChosen: ");
        Serial.println(adcChosen);
        settingOption = adcChosen - ADC0;

        // b.clear();
        clearLEDsExceptRails();
        delayWithButton(400);
        // showLEDsCore2 = 1;

        break;
      }
      case 37 ... 53: {
        selected = 2;
        functionSetting = 2;
        // b.clear();
        clearLEDsExceptRails();
        // showLEDsCore2 = 2;

        gpioChosen = chooseGPIO(1);
        // b.clear();
        clearLEDsExceptRails();
        // showLEDsCore2 = 2;
        if (gpioChosen >= 122 && gpioChosen <= 125) {
          gpioChosen = gpioChosen - 122 + 5;
        } else if (gpioChosen >= 135 && gpioChosen <= 138) {
          gpioChosen = gpioChosen - 134;
        }
        if (gpioState[gpioChosen] != 0) {
          clearLEDsExceptRails();
          // showLEDsCore2 = 2;
          Serial.print("Set GP");
          Serial.print(gpioChosen);
          Serial.println(" to Output");
          char gpString[4];
          itoa(gpioChosen, gpString, 10);

          b.print("GPIO", sfOptionColors[(gpioChosen + 1) % 7], 0xFFFFFF, 0, 0,
                  0);
          b.print(gpString, sfOptionColors[gpioChosen - 1], 0xFFFFFF, 4, 0, 3);
          // b.print(" ", sfOptionColors[0], 0xFFFFFF, 0, 1, -2);
          b.print("Output", sfOptionColors[(gpioChosen + 3) % 7], 0xFFFFFF, 1,
                  1, 1);
          b.printRawRow(0b00000100, 31, 0x200010, 0xffffff);
          b.printRawRow(0b00000100, 32, 0x200010, 0xffffff);
          b.printRawRow(0b00010101, 33, 0x200010, 0xffffff);
          b.printRawRow(0b00001110, 34, 0x200010, 0xffffff);
          b.printRawRow(0b00000100, 35, 0x200010, 0xffffff);
          // showLEDsCore2 = 2;
          delayWithButton(400);

        } else {
        }
        gpioState[gpioChosen] = 0;
        settingOption = gpioChosen;
        setGPIO();
        clearLEDsExceptRails();

        // showLEDsCore2 = 2;
        b.print("Tap to", sfOptionColors[(gpioChosen + 1) % 7], 0xFFFFFF, 0, 0,
                1);
        b.print("toggle", sfOptionColors[(gpioChosen + 2) % 7], 0xFFFFFF, 0, 1,
                1);
        delayWithButton(500);
        clearLEDsExceptRails();
        // showLEDsCore2 = 1;
        // inPadMenu = 0;

        break;
      }
      }
    }
  }
  // inPadMenu = 0;
  Serial.print("pad: ");
  Serial.println(pad);
  Serial.print("functionSetting: ");
  Serial.println(functionSetting);
  Serial.print("settingOption: ");
  Serial.println(settingOption);
  switch (functionSetting) {
  case 2: {
    switch (gpioChosen) {
    case 1: {
      function = 135;
      break;
    }
    case 2: {
      function = 136;
      break;
    }
    case 3: {
      function = 137;
      break;
    }
    case 4: {
      function = 138;
      break;
    }
    case 5: {
      function = 122;
      break;
    }
    case 6: {
      function = 123;
      break;
    }
    case 7: {
      function = 124;
      break;
    }
    case 8: {
      function = 125;
      break;
    }
    }
    break;
  }
  case 1: {
    function = adcChosen;
    break;
  }
  case 0: {
    function = dacChosen;
    break;
  }
  }

  switch (pad) {
  case 128: {
    logoTopSetting[0] = functionSetting;
    logoTopSetting[1] = settingOption;

    break;
  }
  case 129: {
    logoBottomSetting[0] = functionSetting;
    logoBottomSetting[1] = settingOption;

    break;
  }
  case 133: {
    buildingTopSetting[0] = functionSetting;
    buildingTopSetting[1] = settingOption;

    break;
  }
  case 134: {
    buildingBottomSetting[0] = functionSetting;
    buildingBottomSetting[1] = settingOption;

    break;
  }
  }
  saveLogoBindings();
  delay(3);
  inPadMenu = 0;
  showLEDsCore2 = 1;
  return function;
}

int delayWithButton(int delayTime) {
  unsigned long skipTimer = millis();
  while (millis() - skipTimer < delayTime) {
    int reading = checkProbeButton();
    if (reading == 1) {
      Serial.print("skipped 1 ");
      Serial.println(millis() - skipTimer);

      return 1;
    } else if (reading == 2) {
      Serial.print("skipped 2 ");
      Serial.println(millis() - skipTimer);
      return 2;
    }
    delayMicroseconds(100);
  }
  Serial.print("skipped 0 ");
  Serial.println(millis() - skipTimer);
  return 0;
}

int chooseDAC(int justPickOne) {
  int function = -1;
  // b.clear();
  clearLEDsExceptRails();
  showLEDsCore2 = 2;

  // lastReadRaw = 0;
  b.print("DAC", scaleDownBrightness(rawOtherColors[9], 4, 22), 0xFFFFFF, 1, 0,
          3);

  b.print("0", sfOptionColors[0], 0xFFFFFF, 0, 1, 3);
  // b.print("5v", sfOptionColors[0], 0xFFFFFF, 0, 0, -2);
  //  b.printRawRow(0b00011000, 31, sfOptionColors[7], 0xffffff);
  //  b.printRawRow(0b00000100, 32, sfOptionColors[7], 0xffffff);
  //  b.printRawRow(0b00000100, 33, sfOptionColors[7], 0xffffff);
  //  b.printRawRow(0b00010101, 34, sfOptionColors[7], 0xffffff);
  //  b.printRawRow(0b00001110, 35, sfOptionColors[7], 0xffffff);
  //  b.printRawRow(0b00000100, 36, sfOptionColors[7], 0xffffff);
  //  b.printRawRow(0b00011100, 32,sfOptionColors[0], 0xffffff);
  //   b.printRawRow(0b00011100, 33,sfOptionColors[0], 0xffffff);

  b.print("1", sfOptionColors[2], 0xFFFFFF, 5, 1, 0);
  // b.print("8v", sfOptionColors[2], 0xFFFFFF, 5, 0, 1);
  // b.printRawRow(0b00011000, 58, sfOptionColors[4], 0xffffff);
  // b.printRawRow(0b00000100, 57, sfOptionColors[4], 0xffffff);
  // b.printRawRow(0b00000100, 56, sfOptionColors[4], 0xffffff);
  // b.printRawRow(0b00010101, 55, sfOptionColors[4], 0xffffff);
  // b.printRawRow(0b00001110, 54, sfOptionColors[4], 0xffffff);
  // b.printRawRow(0b00000100, 53, sfOptionColors[4], 0xffffff);

  sfProbeMenu = 2;

  int selected = -1;
  function = 0;
  while (selected == -1 && longShortPress(500) == -1) {
    int reading = justReadProbe();
    if (reading != -1) {
      switch (reading) {
      case 31 ... 43: {
        selected = 106;
        function = 106;
        if (justPickOne == 1) {
          return function;
        }
        setDac0voltage(voltageSelect(5));
        // showNets();
        showLEDsCore2 = -1;
        delay(100);

        break;
      }
      case 48 ... 60: {
        selected = 107;
        function = 107;
        if (justPickOne == 1) {
          return function;
          // break;
        }
        setDac1voltage(voltageSelect(8));
        // showNets();
        showLEDsCore2 = -1;
        delay(100);
        break;
      }
      }
    }
  }

  return function;
}
int chooseADC(void) {
  int function = -1;
  // b.clear();

  // probeActive = false;
  clearLEDsExceptRails();

  // lastReadRaw = 0;
  // inPadMenu = 0;
  // showLEDsCore2 = 2;

  // waitCore2();

  // inPadMenu = 1;

  // sfProbeMenu = 1;
  // delay(100);
  // clearLEDsExceptRails();

  // core1busy = 1;
  b.print(" ADC", scaleDownBrightness(rawOtherColors[8], 4, 22), 0xFFFFFF, 0, 0,
          3);

  //  delay(1000);
  //  function = 111;
  b.print("0", sfOptionColors[0], 0xFFFFFF, 0, 1, -1);
  b.print("1", sfOptionColors[1], 0xFFFFFF, 1, 1, 0);
  b.print("2", sfOptionColors[2], 0xFFFFFF, 2, 1, 1);
  b.print("3", sfOptionColors[3], 0xFFFFFF, 3, 1, 2);
  b.print("4", sfOptionColors[4], 0xFFFFFF, 4, 1, 3);
  b.print("P", sfOptionColors[5], 0xFFFFFF, 5, 1, 4);

  showLEDsCore2 = 2;
  // Serial.print("inPadMenu: ");
  // Serial.println(inPadMenu);
  // Serial.print("sfProbeMenu: ");
  // Serial.println(sfProbeMenu);
  // Serial.print("probeActive: ");
  // Serial.println(probeActive);
  // while (true);
  int selected = -1;
  while (selected == -1 && longShortPress(500) != 1) {
    int reading = justReadProbe();
    // Serial.print("reading: ");
    // Serial.println(reading);
    if (reading != -1) {
      //       Serial.print("reading: ");
      // Serial.println(reading);
      switch (reading) {
      case 31 ... 35: {
        selected = ADC0;
        function = ADC0;

        break;
      }
      case 36 ... 40: {
        selected = ADC1;
        function = ADC1;
        // while (justReadProbe() == reading) {
        //   // Serial.print("reading: ");
        //   // Serial.println(justReadProbe());
        //   delay(10);
        // }
        break;
      }
      case 41 ... 45: {
        selected = ADC2;
        function = ADC2;
        // while (justReadProbe() == reading) {
        //   // Serial.print("reading: ");
        //   // Serial.println(justReadProbe());
        //   delay(10);
        // }
        break;
      }
      case 46 ... 50: {
        selected = ADC3;
        function = ADC3;
        // while (justReadProbe() == reading) {
        //   // Serial.print("reading: ");
        //   // Serial.println(justReadProbe());
        //   delay(10);
        // }
        break;
      }
      case 51 ... 55: {
        selected = ADC4;
        function = ADC4;
        // while (justReadProbe() == reading) {
        //   // Serial.print("reading: ");
        //   // Serial.println(justReadProbe());
        //   delay(10);
        // }
        break;
      }
      case 56 ... 60: {
        selected = ADC7;
        function = ADC7;
        // while (justReadProbe() == reading) {
        //   // Serial.print("reading: ");
        //   // Serial.println(justReadProbe());
        //   delay(10);
        // }
        break;
      }
      }
      // while (justReadProbe() == reading) {
      //   Serial.print("reading: ");
      //   Serial.println(justReadProbe());
      //   delay(100);
      // }
    }
  }

  clearLEDsExceptRails();
  // showNets();
  // showLEDsCore2 = 1;
  return function;
}

int chooseGPIOinputOutput(int gpioChosen) {
  int settingOption = -1;

  b.print("Input", sfOptionColors[gpioChosen + 1 % 7], 0xFFFFFF, 1, 0, 3);
  b.print(gpioChosen, sfOptionColors[gpioChosen - 1], 0xFFFFFF, 0, 0, -2);
  b.print("Output", sfOptionColors[gpioChosen % 7], 0xFFFFFF, 0, 1, 3);

  showLEDsCore2 = 2;

  // Serial.print("gpioChosen: ");
  // Serial.println(gpioChosen);

  // delay(100);

  while (settingOption == -1 && longShortPress(500) != 1) {
    int reading = justReadProbe();
    if (reading != -1) {
      switch (reading) {
      case 9 ... 29: {
        gpioState[gpioChosen - 1] = 2;
        settingOption = 4;
        break;
      }
      case 35 ... 59: {
        gpioState[gpioChosen - 1] = 1;
        settingOption = 0;
        break;
      }
      }
    }
  }

  // clearLEDsExceptRails();
  // showNets();
  // showLEDsCore2 = 1;
  return settingOption;
}

int chooseGPIO(int skipInputOutput) {
  int function = -1;

  b.clear();
  clearLEDsExceptRails();
  showLEDsCore2 = 2;
  sfProbeMenu = 3;
  // lastReadRaw = 0;
  // b.print("3v", 0x0f0002, 0xFFFFFF, 0, 0, -2);
  b.print("1", sfOptionColors[0], 0xFFFFFF, 0, 0, -1);
  b.print("2", sfOptionColors[1], 0xFFFFFF, 2, 0, -1);
  b.print("3", sfOptionColors[2], 0xFFFFFF, 4, 0, 0);
  b.print("4", sfOptionColors[3], 0xFFFFFF, 6, 0, 0);
  // b.print("5v", 0x0f0200, 0xFFFFFF, 0, 1, -2);
  b.print("5", sfOptionColors[4], 0xFFFFFF, 0, 1, -1);
  b.print("6", sfOptionColors[5], 0xFFFFFF, 2, 1, -1);
  b.print("7", sfOptionColors[6], 0xFFFFFF, 4, 1, 0);
  b.print("8", sfOptionColors[7], 0xFFFFFF, 6, 1, 0);
  int selected = -1;
  // delayWithButton(300);
  //  return 0;
  while (selected == -1 && checkProbeButton() == 0) {
    int reading = justReadProbe();
    if (reading != -1) {
      switch (reading) {
      case 1 ... 6: {
        selected = 135;
        function = 135;
        break;
      }
      case 7 ... 15: {
        selected = 136;
        function = 136;
        break;
      }
      case 16 ... 24: {
        selected = 137;
        function = 137;
        break;
      }
      case 25 ... 30: {
        selected = 138;
        function = 138;
        break;
      }
      case 31 ... 36: {
        selected = 122;
        function = 122;
        break;
      }
      case 37 ... 45: {
        selected = 123;
        function = 123;
        break;
      }
      case 46 ... 54: {
        selected = 124;
        function = 124;
        break;
      }
      case 55 ... 60: {
        selected = 125;
        function = 125;
        break;
      }
      }
    }
  }

  if (function == -1) {
    return function;
  }
  if (selected == -1) {
    return function;
  }
  if (skipInputOutput == 0) {

    int gpioChosen = -1;

    switch (function) {
    case 135 ... 138: {
      gpioChosen = function - 134;
      break;
    }
    case 122 ... 125: {
      gpioChosen = function - 117;
      break;
    }
    }
    Serial.print("gpioChosen: ");
    Serial.println(gpioChosen);
    clearLEDsExceptRails();
    chooseGPIOinputOutput(gpioChosen);
  }
  // clearLEDsExceptRails();
  //  showNets();
  showLEDsCore2 = -1;
  return function;
}

float voltageSelect(int fiveOrEight) {
  float voltageProbe = 0.0;
  uint32_t color = 0x000000;

  // fiveOrEight = 8; // they're both 8v now
  if (fiveOrEight == 5 && false) {

    b.clear();
    clearLEDsExceptRails();

    uint8_t step = 0b0000000;
    for (int i = 31; i <= 60; i++) {
      if ((i - 1) % 6 == 0) {
        step = step << 1;
        step = step | 0b00000001;
      }

      b.printRawRow(step, i - 1, logoColors8vSelect[(i - 31) * 2], 0xffffff);
    }
    // b.print("Set", scaleDownBrightness(rawOtherColors[9], 4, 22),
    //         0xFFFFFF, 1, 0, 3);
    b.print("Set", scaleDownBrightness(rawOtherColors[9], 4, 22), 0xFFFFFF, 1,
            0, 3);
    b.print("0v", sfOptionColors[7], 0xFFFFFF, 0, 0, -2);
    b.print("5v", sfOptionColors[7], 0xFFFFFF, 5, 0, 1);
    int vSelected = -1;
    int encoderReadingPos = 45;
    rotaryDivider = 4;
    while (vSelected == -1) {
      int reading = justReadProbe();
      // rotaryEncoderStuff();
      int encodeEdit = 0;
      if (encoderDirectionState == UP || reading == -19) {
        encoderDirectionState = NONE;
        voltageProbe = voltageProbe + 0.1;
        encodeEdit = 1;
        // Serial.println(reading);

      } else if (encoderDirectionState == DOWN || reading == -17) {
        encoderDirectionState = NONE;
        voltageProbe = voltageProbe - 0.1;

        encodeEdit = 1;
        // Serial.println(voltageProbe);

      } else if (encoderButtonState == PRESSED &&
                     lastButtonEncoderState == IDLE ||
                 reading == -10) {
        encodeEdit = 1;
        encoderButtonState = IDLE;
        vSelected = 10;
      }
      if (voltageProbe < 0.0) {
        voltageProbe = 0.0;
      } else if (voltageProbe > 5.0) {
        voltageProbe = 5.0;
      }
      // Serial.println(reading);
      if (reading > 0 && reading >= 31 && reading <= 60 || encodeEdit == 1) {
        //
        b.clear(1);

        char voltageString[7] = " 0.0 V";

        if (voltageProbe < 0.0) {
          voltageProbe = 0.0;
        } else if (voltageProbe > 5.0) {
          voltageProbe = 5.0;
        }

        if (encodeEdit == 0) {
          voltageProbe = (reading - 31) * (5.0 / 29);

        } else {
          reading = 31 + (voltageProbe + 8.0) * (29.0 / 16.0);
        }
        // Serial.println(voltageProbe);
        color = logoColors8vSelect[(reading - 31) * 2];

        snprintf(voltageString, 7, "%0.1f v", voltageProbe);
        b.print(voltageString, color, 0xFFFFFF, 0, 1, 3);
        showLEDsCore2 = -2;
        delay(10);
      }
      if (checkProbeButton() > 0 || vSelected == 10) {
        // Serial.println("button\n\r");

        rawSpecialNetColors[4] = color;
        rgbColor rg = unpackRgb(color);
        specialNetColors[4].r = rg.r;
        specialNetColors[4].g = rg.g;
        specialNetColors[4].b = rg.b;
        b.clear();
        // clearLEDsExceptRails();
        // showLEDsCore2 = 1;
        if (vSelected != 10) {
          vSelected = 1;
        } else {
          vSelected = 10;
          Serial.println("encoder button\n\r");
          delay(500);
        }
        // if (checkProbeButton() == 2) {
        //   vSelected = 10;
        // }
        vSelected = 1;
        return voltageProbe;
        showLEDsCore2 = -1;
        break;
      }
    }

  } else if (fiveOrEight == 8 || true) { // they're both 8v now
    b.clear();
    clearLEDsExceptRails();

    uint8_t step = 0b00011111;
    for (int i = 31; i <= 60; i++) {
      if ((i - 1) % 3 == 0 && i < 46 && i > 32) {
        step = step >> 1;
        step = step & 0b01111111;

      } else if ((i) % 3 == 1 && i > 46) {
        step = step << 1;
        step = step | 0b00000001;
      }

      b.printRawRow(step, i - 1, logoColors8vSelect[(i - 31) * 2], 0xffffff);
    }
    // b.print("Set", scaleDownBrightness(rawOtherColors[9], 4, 22),
    //         0xFFFFFF, 1, 0, 3);
    b.print("-8v", sfOptionColors[0], 0xFFFFFF, 0, 0, -2);
    b.print("+8v", sfOptionColors[1], 0xFFFFFF, 4, 0, 1);
    int vSelected = -1;
    int encoderReadingPos = 45;
    rotaryDivider = 4;

    float lastVoltageProbe = -10.0;

    while (vSelected == -1) {

      int reading = justReadProbe();
      rotaryEncoderStuff();

      int encodeEdit = 0;
      if (encoderDirectionState == UP || reading == -19) {
        encoderDirectionState = NONE;
        voltageProbe = voltageProbe + 0.1;
        encodeEdit = 1;
        // Serial.println(reading);

      } else if (encoderDirectionState == DOWN || reading == -17) {
        encoderDirectionState = NONE;
        voltageProbe = voltageProbe - 0.1;
        encodeEdit = 1;
        // Serial.println(voltageProbe);

      } else if (encoderButtonState == PRESSED &&
                     lastButtonEncoderState == IDLE ||
                 reading == -10) {
        encodeEdit = 1;
        encoderButtonState = IDLE;
        vSelected = 10;
      }
      // Serial.println(reading);
      if (reading > 0 && reading >= 31 && reading <= 60 || encodeEdit == 1) {
        //
        b.clear(1);

        char voltageString[7] = " 0.0 V";

        if (voltageProbe < -8.0) {
          voltageProbe = -8.0;
        } else if (voltageProbe > 8.0) {
          voltageProbe = 8.0;
        }

        if (encodeEdit == 0) {
          voltageProbe = (reading - 31) * (16.0 / 29);
          voltageProbe = voltageProbe - 8.0;
          if (voltageProbe < 0.4 && voltageProbe > -0.4) {
            voltageProbe = 0.0;
          }
        } else {
          reading = 31 + (voltageProbe + 8.0) * (29.0 / 16.0);
        }
        //

        color = logoColors8vSelect[(reading - 31) * 2];

        snprintf(voltageString, 7, "%0.1f v", voltageProbe);
        b.print(voltageString, color, 0xFFFFFF, 0, 1, 3);
        showLEDsCore2 = 2;
        Serial.print("\r                                           \r");
        Serial.print("DAC ");
        Serial.print(fiveOrEight ? "1:  " : "0:  ");
        Serial.print(voltageProbe, 1);
        Serial.print(" V");
        // delay(10);
      }

      if (checkProbeButton() > 0 || vSelected == 10) {
        Serial.println(" ");

        rawSpecialNetColors[4] = color;
        rgbColor rg = unpackRgb(color);
        specialNetColors[4].r = rg.r;
        specialNetColors[4].g = rg.g;
        specialNetColors[4].b = rg.b;
        b.clear();
        // clearLEDsExceptRails();
        // showLEDsCore2 = 1;
        if (vSelected != 10) {
          vSelected = 1;
        } else {
          vSelected = 10;
          // Serial.println("encoder button\n\r");
          // delay(500);
        }
        vSelected = 1;
        showLEDsCore2 = -1;
        return voltageProbe;
        break;
      }
    }
  }
  // Serial.println(" ");
  return 0.0;
}

int checkSwitchPosition() { // 0 = measure, 1 = select

  unsigned long timer = micros();
  checkingButton = 0;
  // showProbeLEDs = 8;
  // //delay(200);
  //   //
  // //waitCore2();
  // while(showProbeLEDs == 8) {
  //  // delay(10);
  // }
  if (checkProbeCurrent() > 0.08) {
    // showProbeLEDs = 0;
    //  Serial.print("probe current: ");
    //  Serial.println(micros() - timer);
    //  if (probeActive == 0) {
    //    showProbeLEDs = 4;
    //  } else {
    //    if (connectOrClearProbe == 1) {
    //      showProbeLEDs = 2;
    //    } else {
    //      showProbeLEDs = 3;
    //    }
    //  }
    timer = micros();
    switchPosition = 1;
    // Serial1.begin(baudRateUSBSer1, getSerial1Config());
    // Serial2.begin(baudRateUSBSer2, getSerial2Config());
    return 1;
  } else {
    // showProbeLEDs = 0;
    if (bufferPowerConnected == true) {
      // routableBufferPower(0);
      //  addBridgeToNodeFile(ROUTABLE_BUFFER_IN, RP_GPIO_23, netSlot, 1);
    }
    // refreshBlind();
    // refreshLocalConnections();
    // showProbeLEDs = 3;
    switchPosition = 0;
    // Serial1.begin(baudRateUSBSer1, getSerial1Config());
    // Serial2.begin(baudRateUSBSer2, getSerial2Config());
    return 0;
  }
}
// float calibrated3v3 = 3.3;

// void calibrateDac0(float target) {
//   target = 3.33;
//   int calibrationFound = 0;

//   removeBridgeFromNodeFile(DAC0, -1, netSlot, 1);
//   removeBridgeFromNodeFile(ROUTABLE_BUFFER_IN, -1, netSlot, 1);
//   addBridgeToNodeFile(ROUTABLE_BUFFER_IN, DAC0, netSlot, 1);

//   refreshBlind(-1);

//   while (calibrationFound == 0) {
//     setDac0voltage(calibrated3v3, 0);
//     delay(10);
//     float MeasuredVoltage = INA1.getBusVoltage();
//     Serial.print("setVoltage: ");
//     Serial.print(calibrated3v3);
//     Serial.print("\t\tMeasuredVoltage: ");
//     Serial.println(MeasuredVoltage);

//     if (MeasuredVoltage > target + 0.01) {
//       calibrated3v3 = calibrated3v3 - 0.01;
//     } else if (MeasuredVoltage < target - 0.01) {
//       calibrated3v3 = calibrated3v3 + 0.01;
//     } else {
//       calibrationFound = 1;
//     }
//   }
// }

float checkProbeCurrent(void) {

  int bs = 0;

  float lastDac = dacOutput[0];
  // unsigned long timer[4];
  // timer[0] = micros();
  // setDac0voltage(3.3, 0);
  // if ( removeBridgeFromNodeFile(ROUTABLE_BUFFER_IN, DAC0, netSlot, 1, 1) ==
  // 0) {

  // addBridgeToNodeFile(ROUTABLE_BUFFER_IN, DAC0, netSlot, 1, 0);
  refreshBlind(1, 0);
  //  }
  // addBridgeToNodeFile(ROUTABLE_BUFFER_IN, DAC0, netSlot, 1, 0);
  // timer[1] = micros();
  // printNodeFile();
  // setDac0voltage(3.33, 1, 1);
  // chooseShownReadings();
  // timer[2] = micros();

  // timer[3] = micros();
  //    printPathsCompact();
  //  printChipStatus();
  //  pinMode(23, INPUT);
  digitalWrite(10, LOW);
  delayMicroseconds(10);
  // refreshLocalConnections();
  // showProbeLEDs = 8;
  // while(showProbeLEDs == 8) {
  //   //delay(10);
  // }

  float current = INA1.getCurrent_mA();

 if (showProbeCurrent == 1) {
  Serial.print("current: ");
  Serial.print(current);
  Serial.println(" mA\n\r");
  }

  // for (int i = 1; i < 4; i++) {
  //   Serial.print("timer[");
  //   Serial.print(i);
  //   Serial.print("]: ");

  //     Serial.println(timer[i] - timer[i - 1]);
  //     //Serial.print("\t");
  //   }

  digitalWrite(10, HIGH);

  return current;
}

void routableBufferPower(int offOn, int flash) {
  if (offOn == 1) {
    // Serial.println("power on\n\r");
    //  delay(10);

    setDac0voltage(3.33, 1, 1);

    // removeBridgeFromNodeFile(DAC0, -1, netSlot, 1);
    //   pinMode(27, OUTPUT);
    //    digitalWrite(27, HIGH);

    if (flash == 1) {
      addBridgeToNodeFile(ROUTABLE_BUFFER_IN, DAC0, netSlot, 0, 0);
      // addBridgeToNodeFile(ROUTABLE_BUFFER_IN, RP_GPIO_8, netSlot, 1, 0);

      refreshConnections();
    } else {

      addBridgeToNodeFile(ROUTABLE_BUFFER_IN, DAC0, netSlot, 1, 0);
      // addBridgeToNodeFile(ROUTABLE_BUFFER_IN, RP_GPIO_8, netSlot, 1, 0);
      refreshBlind(1, 0);
    }
    // addBridgeToNodeFile(ROUTABLE_BUFFER_IN, DAC0, netSlot, 1, 0);

    // addBridgeToNodeFile(ROUTABLE_BUFFER_IN, DAC0, netSlot, 1, 1);

    bufferPowerConnected = true;

  } else {
    // removeBridgeFromNodeFile(ROUTABLE_BUFFER_IN, RP_GPIO_8, netSlot, 1);
    // removeBridgeFromNodeFile(ROUTABLE_BUFFER_IN, RP_GPIO_8, netSlot, 0);
    removeBridgeFromNodeFile(ROUTABLE_BUFFER_IN, DAC0, netSlot, 1);
    removeBridgeFromNodeFile(ROUTABLE_BUFFER_IN, DAC0, netSlot, 0);
    setDac0voltage(0.0, 1);
    bufferPowerConnected = false;
    refreshBlind();
  }
}

int selectFromLastFound(void) {

  rawOtherColors[1] = 0x0010ff;

  blinkTimer = 0;
  int selected = 0;
  int selectionConfirmed = 0;
  int selected2 = connectedRows[selected];
  Serial.print("\n\r");
  Serial.print("      multiple nodes found\n\n\r");
  Serial.print("  short press = cycle through nodes\n\r");
  Serial.print("  long press  = select\n\r");

  Serial.print("\n\r ");
  for (int i = 0; i < connectedRowsIndex; i++) {

    printNodeOrName(connectedRows[i]);
    if (i < connectedRowsIndex - 1) {
      Serial.print(", ");
    }
  }
  Serial.print("\n\n\r");
  delay(10);

  uint32_t previousColor[connectedRowsIndex];

  for (int i = 0; i < connectedRowsIndex; i++) {
    previousColor[i] = leds.getPixelColor(nodesToPixelMap[connectedRows[i]]);
  }
  int lastSelected = -1;

  while (selectionConfirmed == 0) {
    probeTimeout = millis();
    // if (millis() - blinkTimer > 100)
    // {
    if (lastSelected != selected && selectionConfirmed == 0) {
      for (int i = 0; i < connectedRowsIndex; i++) {
        if (i == selected) {
          leds.setPixelColor(nodesToPixelMap[connectedRows[i]],
                             rainbowList[1][0], rainbowList[1][1],
                             rainbowList[1][2]);
        } else {
          // uint32_t previousColor =
          // leds.getPixelColor(nodesToPixelMap[connectedRows[i]]);
          if (previousColor[i] != 0) {
            int r = (previousColor[i] >> 16) & 0xFF;
            int g = (previousColor[i] >> 8) & 0xFF;
            int b = (previousColor[i] >> 0) & 0xFF;
            leds.setPixelColor(nodesToPixelMap[connectedRows[i]], (r / 4) + 5,
                               (g / 4) + 5, (b / 4) + 5);
          } else {

            leds.setPixelColor(nodesToPixelMap[connectedRows[i]],
                               rainbowList[1][0] / 8, rainbowList[1][1] / 8,
                               rainbowList[1][2] / 8);
          }
        }
      }
      lastSelected = selected;

      Serial.print(" \r");
      // Serial.print("");
      printNodeOrName(connectedRows[selected]);
      Serial.print("  ");
    }
    // leds.show();
    showLEDsCore2 = 2;
    blinkTimer = millis();
    //  }
    delay(30);
    int longShort = longShortPress();
    delay(5);
    if (longShort == 1) {
      selectionConfirmed = 1;
      // for (int i = 0; i < connectedRowsIndex; i++)
      // {
      //     if (i == selected)
      //     // if (0)
      //     {
      //         leds.setPixelColor(nodesToPixelMap[connectedRows[i]],
      //         rainbowList[rainbowIndex][0], rainbowList[rainbowIndex][1],
      //         rainbowList[rainbowIndex][2]);
      //     }
      //     else
      //     {
      //         leds.setPixelColor(nodesToPixelMap[connectedRows[i]], 0, 0, 0);
      //     }
      // }
      // showLEDsCore2 = 1;
      // selected = lastFound[node1or2][selected];
      //  clearLastFound();

      // delay(500);
      selected2 = connectedRows[selected];
      // return selected2;
      break;
    } else if (longShort == 0) {

      selected++;
      blinkTimer = 0;

      if (selected >= connectedRowsIndex) {

        selected = 0;
      }
      // delay(100);
    }
    delay(15);
    //  }
    //}

    // showLEDsCore2 = 1;
  }
  selected2 = connectedRows[selected];

  for (int i = 0; i < connectedRowsIndex; i++) {
    if (i == selected) {
      leds.setPixelColor(nodesToPixelMap[connectedRows[selected]],
                         rainbowList[0][0], rainbowList[0][1],
                         rainbowList[0][2]);
    } else if (previousColor[i] != 0) {

      int r = (previousColor[i] >> 16) & 0xFF;
      int g = (previousColor[i] >> 8) & 0xFF;
      int b = (previousColor[i] >> 0) & 0xFF;
      leds.setPixelColor(nodesToPixelMap[connectedRows[i]], r, g, b);
    } else {

      leds.setPixelColor(nodesToPixelMap[connectedRows[i]], 0, 0, 0);
    }
  }

  // leds.setPixelColor(nodesToPixelMap[selected2], rainbowList[0][0],
  // rainbowList[0][1], rainbowList[0][2]); leds.show(); showLEDsCore2 = 1;
  probeButtonTimer = millis();
  // connectedRowsIndex = 0;
  //justSelectedConnectedNodes = 1;
  return selected2;
}

int longShortPress(int pressLength) {
  int longShort = 0;
  unsigned long clickTimer = 0;

  clickTimer = millis();
  int buttonState = checkProbeButton();
  if (buttonState > 0) {

    while (millis() - clickTimer < pressLength) {
      if (checkProbeButton() == 0) {
        // Serial.print("buttonState: ");
        // Serial.println(buttonState);
        return buttonState;
      }
      delay(5);
    }
  } else {
    return -1;
  }
  // Serial.print("buttonState: ");
  // Serial.println(buttonState);
  return buttonState;
}
int countLED = 0;
volatile int checkingButton = 0;

int checkProbeButton(void) {
  int buttonState = 0;
  int buttonState2 = 0;
  int buttonState3 = 0;
  checkingButton = 1;

  // what we're doing here is tricking the Neopixel library into letting us use
  // its pin to read the 2 buttons on the probe without it stopping

  while (showingProbeLEDs == 1) {
    // delay(10);
  }
  waitCore2();
  // setDac0voltage(3.33, 1, 1);

  core1busy = true;

  // gpio_set_function(2, GPIO_FUNC_SIO);
  // gpio_disable_pulls(2);
  // pinMode(2, INPUT);
  pinMode(BUTTON_PIN, INPUT);
  // probeLEDs.setPin(3);
  gpio_set_function(2, GPIO_FUNC_SIO);
  gpio_disable_pulls(2);
  // gpio_set_inover(2, true);
  //  gpio_set_outover(2, false);
  // pads_bank0_hw->io[8] = PADS_BANK0_GPIO2_GPIO2;
  // hw_set_bits(&pads_bank0_hw->io[2], PADS_BANK0_GPIO2_ISO_BITS);

  gpio_set_dir(2, false);
  // hw_clear_bits(&pads_bank0_hw->io[2], PADS_BANK0_GPIO2_OD_BITS);
  // gpio_set_inover(2, true);
  // hw_clear_bits(&pads_bank0_hw->io[2], PADS_BANK0_GPIO2_ISO_BITS);
  // hw_set_bits(PADS_BANK0_BASE,  PADS_BANK0_GPIO2_ISO_BITS);
  //  pads_bank0_hw->io
  //  // probeLEDs.begin();
  //  pinMode(2, OUTPUT_8MA);
  //  digitalWrite(2, LOW);
  //  delayMicroseconds(1);
  // pinMode(2, INPUT);
  delayMicroseconds(20);
  //  pinMode(9, OUTPUT_4MA);
  // pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(10, OUTPUT_8MA);

  digitalWrite(10, HIGH);

  gpio_set_pulls(BUTTON_PIN, false, true);

  gpio_set_input_enabled(BUTTON_PIN, true); // rp2350 errata hack: setting it to input
                                   // right before reading and back fixes it
  buttonState = gpio_get(BUTTON_PIN);
  gpio_set_input_enabled(BUTTON_PIN, false);

  delayMicroseconds(10);
  delayMicroseconds(40);
  // pinMode(9, OUTPUT_4MA);

  // delayMicroseconds(5);
  //  pinMode(2, OUTPUT_8MA);
  //  digitalWrite(2, LOW);
  //  delayMicroseconds(1);
  //  pinMode(2, INPUT);
  //  digitalWrite(2, HIGH);
  gpio_set_pulls(BUTTON_PIN, true, false);
  // gpio_set_input_enabled(BUTTON_PIN, false);
  gpio_set_input_enabled(BUTTON_PIN, true);
  delayMicroseconds(20); // pinMode(BUTTON_PIN, INPUT_PULLDOWN);
  // Serial.print("pin BUTTON_PIN is pulled ");
  // Serial.print(gpio_is_pulled_up(BUTTON_PIN) ? "up\n\r" : "");
  // Serial.print(gpio_is_pulled_down(BUTTON_PIN) ? "down\n\r" : "");

  buttonState2 = gpio_get(BUTTON_PIN);
  gpio_set_input_enabled(BUTTON_PIN, false);
  delayMicroseconds(1);

  delayMicroseconds(20);
  // pinMode(2, OUTPUT_8MA);
  // digitalWrite(2, LOW);
  // delayMicroseconds(1);
  // pinMode(2, INPUT);
  // pinMode(BUTTON_PIN, OUTPUT_4MA);
  gpio_set_pulls(BUTTON_PIN, false, false);
  // pinMode(2, OUTPUT_8MA);
  // digitalWrite(2, LOW);
  // delayMicroseconds(1);
  // pinMode(2, INPUT);
  // pinMode(BUTTON_PIN, INPUT_PULLUP);
  // gpio_set_input_enabled(BUTTON_PIN, true);
  delayMicroseconds(20);

  pinMode(BUTTON_PIN, OUTPUT_8MA);
  digitalWrite(BUTTON_PIN, LOW);
  delayMicroseconds(1);
  pinMode(BUTTON_PIN, INPUT);
  gpio_set_pulls(BUTTON_PIN, false, true);
  // Serial.print("pin BUTTON_PIN is pulled ");
  // Serial.print(gpio_is_pulled_up(BUTTON_PIN) ? "up\n\r" : "");
  // Serial.print(gpio_is_pulled_down(BUTTON_PIN) ? "down\n\r" : "");
  // gpio_set_input_enabled(BUTTON_PIN, false);
  gpio_set_input_enabled(BUTTON_PIN, true);
  buttonState3 = gpio_get(BUTTON_PIN);
  gpio_set_input_enabled(BUTTON_PIN, false);

  gpio_set_pulls(BUTTON_PIN, false, false);
  //   while(Serial.available() == 0)
  // {
  //   Serial.read();
  // }

  gpio_set_function(2, GPIO_FUNC_PIO0);

  checkingButton = 0;
  core1busy = false;
  // delayMicroseconds(500);
  // probeLEDs.show();

  // Serial.print(buttonState);
  // Serial.print(" ");
  // Serial.print(buttonState2);
  // Serial.print(" ");
  // Serial.print(buttonState3);
  // Serial.println(" ");
  if (buttonState == 1 && buttonState2 == 1 &&
      buttonState3 == 1) { // disconnect Button
    // Serial.print("buttonState ");
    // Serial.print(buttonState);
    // Serial.print("buttonState2 ");
    // Serial.println(buttonState2);
    // Serial.println(" ");
    // Serial.println("disconnect button");
    if (PROBE_REV >= 4) {
      return 2;
    } else {
      return 1;
    }
    // return 1;
  } else if (buttonState == 0 && buttonState2 == 0 &&
             buttonState3 == 0) { // connect Button
    // Serial.print("buttonState ");
    // Serial.println(buttonState);
    // Serial.print("buttonState2 ");
    // Serial.println(buttonState2);
    // Serial.println(" ");
    // Serial.println("connect button");
    if (PROBE_REV >= 4) {
      return 1;
    } else {
      return 2;
    }
    // return 2;
  }
  //   Serial.print("buttonState ");
  // Serial.println(buttonState);
  // Serial.print("buttonState2 ");
  // Serial.println(buttonState2);
  // Serial.println(" ");

  return 0;
}

int readFloatingOrState(int pin, int rowBeingScanned) {
  // return 0;
  enum measuredState state = unknownState;
  // enum measuredState state2 = floating;

  int readingPullup = 0;
  int readingPullup2 = 0;
  int readingPullup3 = 0;

  int readingPulldown = 0;
  int readingPulldown2 = 0;
  int readingPulldown3 = 0;

  // pinMode(pin, INPUT_PULLUP);

  if (rowBeingScanned != -1) {

    analogWrite(PROBE_PIN, 128);

    while (1) // this is the silliest way to align to the falling edge of the
              // probe PWM signal
    {
      if (gpio_get(PROBE_PIN) != 0) {
        if (gpio_get(PROBE_PIN) == 0) {
          break;
        }
      }
    }
  }

  delayMicroseconds((probeHalfPeriodus * 5) + (probeHalfPeriodus / 2));

  readingPullup = digitalRead(pin);
  delayMicroseconds(probeHalfPeriodus * 3);
  readingPullup2 = digitalRead(pin);
  delayMicroseconds(probeHalfPeriodus * 1);
  readingPullup3 = digitalRead(pin);

  // pinMode(pin, INPUT_PULLDOWN);

  if (rowBeingScanned != -1) {
    while (1) // this is the silliest way to align to the falling edge of the
              // probe PWM signal
    {
      if (gpio_get(PROBE_PIN) != 0) {
        if (gpio_get(PROBE_PIN) == 0) {
          break;
        }
      }
    }
  }

  delayMicroseconds((probeHalfPeriodus * 5) + (probeHalfPeriodus / 2));

  readingPulldown = digitalRead(pin);
  delayMicroseconds(probeHalfPeriodus * 3);
  readingPulldown2 = digitalRead(pin);
  delayMicroseconds(probeHalfPeriodus * 1);
  readingPulldown3 = digitalRead(pin);

  // if (readingPullup == 0 && readingPullup2 == 1 && readingPullup3 == 0 &&
  // readingPulldown == 1 && readingPulldown2 == 0 && readingPulldown3 == 1)
  // {
  //     state = probe;
  // }

  if ((readingPullup != readingPullup2 || readingPullup2 != readingPullup3) &&
      (readingPulldown != readingPulldown2 ||
       readingPulldown2 != readingPulldown3) &&
      rowBeingScanned != -1) {
    state = probe;

    // if (readingPulldown != readingPulldown2 || readingPulldown2 !=
    // readingPulldown3)
    // {
    //     state = probe;

    // } else
    // {
    //     Serial.print("!");
    // }
  } else {

    if (readingPullup2 == 1 && readingPulldown2 == 0) {

      state = floating;
    } else if (readingPullup2 == 1 && readingPulldown2 == 1) {
      //              Serial.print(readingPullup);
      // // Serial.print(readingPullup2);
      // // Serial.print(readingPullup3);
      // // //Serial.print(" ");
      //  Serial.print(readingPulldown);
      // // Serial.print(readingPulldown2);
      // // Serial.print(readingPulldown3);
      //  Serial.print("\n\r");

      state = high;
    } else if (readingPullup2 == 0 && readingPulldown2 == 0) {
      //  Serial.print(readingPullup);
      // // Serial.print(readingPullup2);
      // // Serial.print(readingPullup3);
      // // //Serial.print(" ");
      //  Serial.print(readingPulldown);
      // // Serial.print(readingPulldown2);
      // // Serial.print(readingPulldown3);
      //  Serial.print("\n\r");
      state = low;
    } else if (readingPullup == 0 && readingPulldown == 1) {
      // Serial.print("shorted");
    }
  }

  // Serial.print("\n");
  // showLEDsCore2 = 1;
  // leds.show();
  // delayMicroseconds(100);

  return state;
}

void startProbe(long probeSpeed) {

  // pinMode(PROBE_PIN, OUTPUT_4MA);
  // // pinMode(BUTTON_PIN, INPUT_PULLDOWN);
  // // pinMode(ADC0_PIN, INPUT);
  // digitalWrite(PROBE_PIN, HIGH);
}

void stopProbe() {
  // pinMode(PROBE_PIN, INPUT);
  // pinMode(BUTTON_PIN, INPUT);
}

int checkLastFound(int found) {
  int found2 = 0;
  return found2;
}

void clearLastFound() {}

int probeADCmap[102];

int nothingTouchedReading = 35;
int mapFrom = 35;
// int calibrateProbe() {
//   /* clang-format off */

//   int probeRowMap[102] = {

//       0,	      1,	      2,	      3,	      4,
//       5,	      6,	      7,	      8, 9,	      10,
//       11,	      12,	      13,	      14,	      15,
//       16,	      17, 18,	      19,	      20,	      21,
//       22,	      23,	      24,	      25,	      26, 27,
//       28,	      29,	      30,	      TOP_RAIL, TOP_RAIL_GND,
//       BOTTOM_RAIL,	      BOTTOM_RAIL_GND, 31,	      32, 33, 34,
//       35,	      36,	      37,	      38,	      39, 40,
//       41,	      42,	      43,	      44,	      45,
//       46,	      47,	      48, 49,	      50,	      51,
//       52,	      53,	      54,	      55,	      56,
//       57, 58,	      59,	      60,	      NANO_D1, NANO_D0,
//       NANO_RESET_1,	      NANO_GND_1,	      NANO_D2,	      NANO_D3,
//       NANO_D4,	      NANO_D5,	      NANO_D6,	      NANO_D7, NANO_D8,
//       NANO_D9,	      NANO_D10,	      NANO_D11,	      NANO_D12,
//       NANO_D13,	      NANO_3V3,	      NANO_AREF,	      NANO_A0,
//       NANO_A1,	      NANO_A2,	      NANO_A3,	      NANO_A4, NANO_A5,
//       NANO_A6,	      NANO_A7,	      NANO_5V,	      NANO_RESET_0,
//       NANO_GND_0,	      NANO_VIN, LOGO_PAD_BOTTOM, LOGO_PAD_TOP,
//       GPIO_PAD, DAC_PAD,	      ADC_PAD,	      BUILDING_PAD_TOP,
//       BUILDING_PAD_BOTTOM,
//   };
//   /* clang-format on */

int nothingTouchedSamples[16] = {0};

int getNothingTouched(int samples) {

  startProbe();
  int rejects = 0;
  int loops = 0;

  for (int i = 0; i < 16; i++) {

    nothingTouchedSamples[i] = 0;
  }
  do {

    // samples = 2;

    int sampleAverage = 0;
    rejects = 0;
    nothingTouchedReading = 0;
    for (int i = 0; i < samples; i++) {
      // int reading = readProbeRaw(1);
      int readNoth = readAdc(5, 32);
      nothingTouchedSamples[i] = readNoth;
      //   delayMicroseconds(50);
      //   Serial.print("nothingTouchedSample ");
      //   Serial.print(i);
      //   Serial.print(": ");
      // Serial.println(readNoth);
    }
    loops++;

    for (int i = 0; i < samples; i++) {

      if (nothingTouchedSamples[i] < 100) {
        sampleAverage += nothingTouchedSamples[i];
      } else {
        rejects++;
      }
    }
    if (samples - rejects <= 1) {
      Serial.println("All nothing touched samples rejected, check sense pad "
                     "connections\n\r");
      nothingTouchedReading = 36;
      return 0;
      break;
    }
    sampleAverage = sampleAverage / (samples - rejects);
    rejects = 0;

    for (int i = 0; i < samples; i++) {
      if (abs(nothingTouchedSamples[i] - sampleAverage) < 15) {
        nothingTouchedReading += nothingTouchedSamples[i];
        //       Serial.print("nothingTouchedSample ");
        //   Serial.print(i);
        //   Serial.print(": ");
        // Serial.println(nothingTouchedSamples[i]);
      } else {
        rejects++;
      }
    }

    nothingTouchedReading = nothingTouchedReading / (samples - rejects);
    mapFrom = nothingTouchedReading + 15;

    if (loops > 10) {
      break;
    }

  } while ((nothingTouchedReading > 80 || rejects > samples / 2) && loops < 9);
  // Serial.print("nothingTouchedReading: ");
  // Serial.println(nothingTouchedReading);
  return nothingTouchedReading;
}
unsigned long doubleTimeout = 0;

unsigned long padTimeout = 0;
int padTimeoutLength = 250;

int state = 0;
int lastPadTouched = 0;
unsigned long padNoTouch = 0;

void checkPads(void) {
  // startProbe();
  checkingPads = 1;
  int probeReading = readProbeRaw();
  if (probeReading == -1) {

    checkingPads = 0;
    padNoTouch++;

    if (millis() - padTimeout > padTimeoutLength) {
      padTimeout = millis();
      lastReadRaw = 0;
    }
    return;
  }
  // Serial.print("padNoTouch: ");
  // Serial.println(padNoTouch);
  padNoTouch = 0;

  /* clang-format off */
  int probeRowMap[103] = {

      -1,        1,         2,        3,        4,        5,        6,        7,       8,
       9,       10,        11,       12,       13,       14,       15,       16,
      17,       18,        19,       20,       21,       22,       23,       24,
      25,       26,        27,       28,       29,       30,       TOP_RAIL,       TOP_RAIL_GND,
      BOTTOM_RAIL,       BOTTOM_RAIL_GND,      31,       32,       33,       34,       35,       36,
      37,       38,       39,       40,        41,       42,       43,       44,       45,
      46,       47,       48,       49,        50,       51,       52,       53,       54,
      55,       56,       57,       58,        59,       60,       NANO_D1,       NANO_D0,       NANO_RESET_1,
      GND,       NANO_D2,       NANO_D3,       NANO_D4,       NANO_D5,       NANO_D6,       NANO_D7,       NANO_D8,
      NANO_D9,	      NANO_D10,	      NANO_D11,	      NANO_D12,	      NANO_D13,	      NANO_3V3,	      NANO_AREF,	      NANO_A0,
      NANO_A1,	      NANO_A2,	      NANO_A3,	      NANO_A4,	      NANO_A5,	      NANO_A6,	      NANO_A7,	      NANO_5V,
      NANO_RESET_0,	      GND,	      NANO_VIN,	      LOGO_PAD_BOTTOM,	      LOGO_PAD_TOP,	      GPIO_PAD,	      DAC_PAD,
      ADC_PAD,	      BUILDING_PAD_TOP,	      BUILDING_PAD_BOTTOM,
  };

  /* clang-format on */
  probeReading = probeRowMap[map(probeReading, 40, 4050, 101, 0)];

  // stopProbe();
  if (probeReading < LOGO_PAD_TOP || probeReading > BUILDING_PAD_BOTTOM) {
    padTimeout = millis();
    lastReadRaw = 0;
    checkingPads = 0;
    return;
  }

  padTimeout = millis();
  // Serial.print("probeReading: ");
  // Serial.println(probeReading);
  int foundGpio = 0;
  int foundAdc = 0;
  int foundDac = 0;
  // inPadMenu = 1;
  switch (probeReading) {

  case LOGO_PAD_TOP: {
    switch (logoTopSetting[0]) {
    case -1:
      break;
    case 0: {
      foundDac = 1;
      if (logoTopSetting[1] >= 0 && logoTopSetting[1] <= 1) {

        // probeActive = 1;
        sfProbeMenu = 1;
        clearLEDsExceptRails();
        // checkingPads = 0;
        if (logoTopSetting[1] == 0) {

          dacOutput[0] = voltageSelect(5);

        } else {
          dacOutput[1] = voltageSelect(8);
        }

        setRailsAndDACs();
      }

      break;
    }
    case 1: {
      foundAdc = 1;
      if (logoTopSetting[1] >= 0 && logoTopSetting[1] <= 3) {
        adcRange[logoTopSetting[1]][1] += 0.1;
      }
      break;
    }
    case 2: {
      foundGpio = 1;
      if (logoTopSetting[1] >= 0 && logoTopSetting[1] <= 8) {
        if (gpioState[logoTopSetting[1]] == 0) {
          gpioState[logoTopSetting[1]] = 1;
        } else {
          gpioState[logoTopSetting[1]] = 0;
        }
      }
      setGPIO();
      break;
    }
    }
    break;
  }

  case LOGO_PAD_BOTTOM: {
    switch (logoBottomSetting[0]) {
    case -1:
      break;
    case 0: {
      foundDac = 1;
      if (logoBottomSetting[1] >= 0 && logoBottomSetting[1] <= 1) {
        // probeActive = 1;
        // sfProbeMenu = 1;
        clearLEDsExceptRails();
        // checkingPads = 0;
        if (logoBottomSetting[1] == 0) {
          dacOutput[2] = voltageSelect(5);
        } else {
          dacOutput[3] = voltageSelect(8);
        }
        setRailsAndDACs();
      }
      break;
    }
    case 1: {
      foundAdc = 1;
      if (logoBottomSetting[1] >= 0 && logoBottomSetting[1] <= 3) {
        adcRange[logoBottomSetting[1]][1] += 0.1;
      }
      break;
    }
    case 2: {
      foundGpio = 1;
      if (logoBottomSetting[1] >= 0 && logoBottomSetting[1] <= 8) {
        if (gpioState[logoBottomSetting[1]] == 0) {
          gpioState[logoBottomSetting[1]] = 1;
        } else {
          gpioState[logoBottomSetting[1]] = 0;
        }
      }
      setGPIO();
      break;
    }
    }
    break;
  }
  case BUILDING_PAD_TOP: {
    switch (buildingTopSetting[0]) {
    case -1:
      break;
    case 0: {
      foundDac = 1;
      if (buildingTopSetting[1] >= 0 && buildingTopSetting[1] <= 1) {
        // probeActive = 1;
        // sfProbeMenu = 1;
        clearLEDsExceptRails();
        // checkingPads = 0;
        if (buildingTopSetting[1] == 0) {
          dacOutput[4] = voltageSelect(5);
        } else {
          dacOutput[5] = voltageSelect(8);
        }
        setRailsAndDACs();
      }
      break;
    }
    case 1: {
      foundAdc = 1;
      if (buildingTopSetting[1] >= 0 && buildingTopSetting[1] <= 3) {
        adcRange[buildingTopSetting[1]][1] += 0.1;
      }
      break;
    }
    case 2: {
      foundGpio = 1;
      if (buildingTopSetting[1] >= 0 && buildingTopSetting[1] <= 8) {
        if (gpioState[buildingTopSetting[1]] == 0) {
          gpioState[buildingTopSetting[1]] = 1;
        } else {
          gpioState[buildingTopSetting[1]] = 0;
        }
      }
      setGPIO();
      break;
    }
    }
    break;
  }
  case BUILDING_PAD_BOTTOM: {
    switch (buildingBottomSetting[0]) {
    case -1:
      break;
    case 0: {
      foundDac = 1;
      if (buildingBottomSetting[1] >= 0 && buildingBottomSetting[1] <= 1) {
        // probeActive = 1;
        // sfProbeMenu = 1;
        clearLEDsExceptRails();
        // checkingPads = 0;
        if (buildingBottomSetting[1] == 0) {
          dacOutput[6] = voltageSelect(5);
        } else {
          dacOutput[7] = voltageSelect(8);
        }
        setRailsAndDACs();
      }
      break;
    }
    case 1: {
      foundAdc = 1;
      if (buildingBottomSetting[1] >= 0 && buildingBottomSetting[1] <= 3) {
        adcRange[buildingBottomSetting[1]][1] += 0.1;
      }
      break;
    }
    case 2: {
      foundGpio = 1;
      if (buildingBottomSetting[1] >= 0 && buildingBottomSetting[1] <= 8) {
        if (gpioState[buildingBottomSetting[1]] == 0) {
          gpioState[buildingBottomSetting[1]] = 1;
        } else {
          gpioState[buildingBottomSetting[1]] = 0;
        }
      }
      setGPIO();
      break;
    }
    }
    break;
  }
  }
  inPadMenu = 0;
  //  delay(1000);
}

int readProbeRaw(int readNothingTouched) {
  // nothingTouchedReading = 165;
  // lastReadRaw = 0;

  int measurements[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  // digitalWrite(PROBE_PIN, HIGH);
  if (connectOrClearProbe == 1) {

    for (int i = 0; i < 4; i++) {
      measurements[i] = readAdc(5, 16);
      delayMicroseconds(30);
    }
  } else if (checkingPads == 1) {
    for (int i = 0; i < 4; i++) {
      measurements[i] = readAdc(5, 128);
      delayMicroseconds(30);
    }

  } else {
    for (int i = 0; i < 4; i++) {
      measurements[i] = readAdc(5, 8);
      delayMicroseconds(30);
    }
  }

  int maxVariance = 0;
  int average = 0;
  int variance = 0;
  for (int i = 0; i < 3; i++) {
    variance = abs(measurements[i] - measurements[i + 1]);
    if (variance > maxVariance) {
      maxVariance = variance;
    }
    average = average + measurements[i];
  }
  average = average / 3;
  // Serial.print("average ");
  // Serial.println(average);
  int rowProbed = -1;
  // if (average < 90 && abs(average - nothingTouchedReading) > 10) {
  //   Serial.print("average ");
  //   Serial.println(average);
  // }

  if (maxVariance < 4 && (abs(average - nothingTouchedReading) > 30) &&
      (abs(average - lastReadRaw) > 5)) {
    lastReadRaw = average;
    //      Serial.println("  ");

    //  Serial.println(average);

    return average;

  } else {
    // Serial.print("average ");
    // Serial.println(average);
    return -1;
  }
}

int convertPadsToRows(int pad) {
  int row = pad;
  if (pad == LOGO_PAD_BOTTOM) {
    row = 108;
  } else if (pad == LOGO_PAD_TOP) {
    row = 109;
  } else if (pad == GPIO_PAD) {
    row = 116;
  } else if (pad == DAC_PAD) {
    row = 106;
  } else if (pad == ADC_PAD) {
    row = 111;
  } else if (pad == BUILDING_PAD_TOP) {
    row = 116;
  } else if (pad == BUILDING_PAD_BOTTOM) {
    row = 117;
  }
  return row;
}
unsigned long lastProbeTime = millis();

int justReadProbe() {
  int probeRowMap[108] = {

      -1,
      1,
      2,
      3,
      4,
      5,
      6,
      7,
      8,
      9,
      10,
      11,
      12,
      13,
      14,
      15,
      16,
      17,
      18,
      19,
      20,
      21,
      22,
      23,
      24,
      25,
      26,
      27,
      28,
      29,
      30,
      TOP_RAIL,
      GND,
      BOTTOM_RAIL,
      GND,
      31,
      32,
      33,
      34,
      35,
      36,
      37,
      38,
      39,
      40,
      41,
      42,
      43,
      44,
      45,
      46,
      47,
      48,
      49,
      50,
      51,
      52,
      53,
      54,
      55,
      56,
      57,
      58,
      59,
      60,
      NANO_D1,
      NANO_D0,
      NANO_RESET_1,
      GND,
      NANO_D2,
      NANO_D3,
      NANO_D4,
      NANO_D5,
      NANO_D6,
      NANO_D7,
      NANO_D8,
      NANO_D9,
      NANO_D10,
      NANO_D11,
      NANO_D12,
      NANO_D13,
      NANO_3V3,
      NANO_AREF,
      NANO_A0,
      NANO_A1,
      NANO_A2,
      NANO_A3,
      NANO_A4,
      NANO_A5,
      NANO_A6,
      NANO_A7,
      NANO_5V,
      NANO_RESET_0,
      GND,
      -1,
      LOGO_PAD_BOTTOM,
      LOGO_PAD_TOP,
      GPIO_PAD,
      DAC_PAD,
      ADC_PAD,
      BUILDING_PAD_TOP,
      BUILDING_PAD_BOTTOM,
      -1,
      -1,
      -1,
      -1};
  if (blockProbing > 0) {
    return -1;
  }
  int probeRead = readProbeRaw();

  if (probeRead <= 0) {
    return -1;
  }
  //   Serial.print("probeRead: ");
  // Serial.println(probeRead);

  //int rowProbed = map(probeRead, mapFrom, 4045, 101, 0);
  int rowProbed = map(probeRead, mapFrom, 4045, 101, 0);
  // Serial.print("rowProbed: ");
  // Serial.println(rowProbed);

  if (rowProbed <= 0 || rowProbed > sizeof(probeRowMap)) {
    if (debugProbing == 1) {
      Serial.print("out of bounds of probeRowMap[");
      Serial.println(rowProbed);
    }
    return -1;
  }

  return probeRowMap[rowProbed];
}

int readProbe() {
  int found = -1;
  // connectedRows[0] = -1;
  unsigned long buttonCheck = 0;
  // if (checkProbeButton() == 1) {
  //   return -18;
  // }
  if (blockProbing > 0) {
    return -1;
  }
  /* clang-format off */
  
  int probeRowMap[108] = {

      -1,        1,         2,        3,        4,        5,        6,        7,       8,
       9,       10,        11,       12,       13,       14,       15,       16,
      17,       18,        19,       20,       21,       22,       23,       24,
      25,       26,        27,       28,       29,       30,       TOP_RAIL,       TOP_RAIL_GND,
      BOTTOM_RAIL,       BOTTOM_RAIL_GND,      31,       32,       33,       34,       35,       36,
      37,       38,       39,       40,        41,       42,       43,       44,       45,
      46,       47,       48,       49,        50,       51,       52,       53,       54,
      55,       56,       57,       58,        59,       60,       NANO_D1,       NANO_D0,       NANO_RESET_1,
      GND,       NANO_D2,       NANO_D3,       NANO_D4,       NANO_D5,       NANO_D6,       NANO_D7,       NANO_D8,
      NANO_D9,	      NANO_D10,	      NANO_D11,	      NANO_D12,	      NANO_D13,	      NANO_3V3,	      NANO_AREF,	      NANO_A0,
      NANO_A1,	      NANO_A2,	      NANO_A3,	      NANO_A4,	      NANO_A5,	      NANO_A6,	      NANO_A7,	      NANO_5V,
      NANO_RESET_0,	      GND,	     -1,	      LOGO_PAD_BOTTOM,	      LOGO_PAD_TOP,	      GPIO_PAD,	      DAC_PAD,
      ADC_PAD,	    -1,-1, -1, -1 , -1, -1
  };// BUILDING_PAD_TOP,	      BUILDING_PAD_BOTTOM,  i took this out

  /* clang-format on */

  // startProbe();
  // int measurements[8] = {0, 0, 0, 0, 0, 0, 0, 0};

  int probeRead = readProbeRaw();
  // delay(100);
  // Serial.println(probeRead);
  // Serial.println(debugLEDs);
  while (probeRead <= 0) {
    /// delay(50);
    // return -1;
    // Serial.println(debugLEDs);

    probeRead = readProbeRaw();
    rotaryEncoderStuff();

    if (encoderDirectionState != NONE) {
      if (encoderDirectionState == UP) {
        return -19;
      } else if (encoderDirectionState == DOWN) {
        return -17;
      }
    } else if (encoderButtonState == PRESSED &&
               lastButtonEncoderState == IDLE) {
      return -10;
    }

    // buttonCheck = millis();
    if (millis() - buttonCheck > 40) {
      buttonCheck = millis();
      int buttonState = checkProbeButton();
      if (buttonState == 1) {
        return -18;
      } else if (buttonState == 2) {
        return -16;
      }
    }

    // delayMicroseconds(200);

    if (millis() - doubleTimeout > 1000) {
      doubleTimeout = millis();
      lastReadRaw = 0;
    }

    if (millis() - lastProbeTime > 50) {
      lastProbeTime = millis();
      // Serial.println("probe timeout");
      return -1;
    }
  }
  doubleTimeout = millis();
  if (debugProbing == 1) {
    // Serial.print("probeRead: ");
    // Serial.println(probeRead);
  }
  if (probeRead == -1) {
    return -1;
  }
  
  int rowProbed = map(probeRead, mapFrom, 4060, 101, 0);
  // Serial.print("probeRead: ");
  // Serial.println(probeRead);

  if (rowProbed <= 0 || rowProbed >= sizeof(probeRowMap)) {
    if (debugProbing == 1) {
      Serial.print("out of bounds of probeRowMap[");
      Serial.println(rowProbed);
    }
    return -1;
  }
  if (debugProbing == 1) {
    Serial.print("probeRowMap[");
    Serial.print(rowProbed);
    Serial.print("]: ");
    Serial.println(probeRowMap[rowProbed]);
  }

  rowProbed = selectSFprobeMenu(probeRowMap[rowProbed]);
  if (debugProbing == 1) {
    Serial.print("rowProbed: ");
    Serial.println(rowProbed);
  }
  connectedRows[0] = rowProbed;
  connectedRowsIndex = 1;

  // Serial.print("maxVariance: ");
  // Serial.println(maxVariance);
  return rowProbed;
  // return probeRowMap[rowProbed];
}

int scanRows(int pin) {
  // return readProbe();
  return 0;
  int found = -1;
  connectedRows[0] = -1;

  if (checkProbeButton() == 1) {
    return -18;
  }

  // pin = ADC1_PIN;

  // digitalWrite(RESETPIN, HIGH);
  // delayMicroseconds(20);
  // digitalWrite(RESETPIN, LOW);
  // delayMicroseconds(20);

  pinMode(PROBE_PIN, INPUT);
  delayMicroseconds(400);
  int probeRead = readFloatingOrState(PROBE_PIN, -1);

  if (probeRead == high) {
    found = voltageSelection;
    connectedRows[connectedRowsIndex] = found;
    connectedRowsIndex++;
    found = -1;
    // return connectedRows[connectedRowsIndex];
    // Serial.print("high");
    // return found;
  }

  else if (probeRead == low) {
    found = GND;
    connectedRows[connectedRowsIndex] = found;
    connectedRowsIndex++;
    // return found;
    found = -1;
    // return connectedRows[connectedRowsIndex];
    // Serial.print(connectedRows[connectedRowsIndex]);

    // return connectedRows[connectedRowsIndex];
  }

  startProbe();
  int chipToConnect = 0;
  int rowBeingScanned = 0;

  int xMapRead = 15;

  if (pin == ADC0_PIN) {
    xMapRead = 2;
  } else if (pin == ADC1_PIN) {
    xMapRead = 3;
  } else if (pin == ADC2_PIN) {
    xMapRead = 4;
  } else if (pin == ADC3_PIN) {
    xMapRead = 5;
  }

  for (int chipScan = CHIP_A; chipScan < 8;
       chipScan++) // scan the breadboard (except the corners)
  {

    sendXYraw(CHIP_L, xMapRead, chipScan, 1);

    for (int yToScan = 1; yToScan < 8; yToScan++) {

      sendXYraw(chipScan, 0, 0, 1);
      sendXYraw(chipScan, 0, yToScan, 1);

      rowBeingScanned = ch[chipScan].yMap[yToScan];
      if (readFloatingOrState(pin, rowBeingScanned) == probe) {
        found = rowBeingScanned;

        if (found != -1) {
          connectedRows[connectedRowsIndex] = found;
          connectedRowsIndex++;
          found = -1;
          // delayMicroseconds(100);
          // stopProbe();
          // break;
        }
      }

      sendXYraw(chipScan, 0, 0, 0);
      sendXYraw(chipScan, 0, yToScan, 0);
    }
    sendXYraw(CHIP_L, 2, chipScan, 0);
  }

  int corners[4] = {1, 30, 31, 60};
  sendXYraw(CHIP_L, xMapRead, 0, 1);
  for (int cornerScan = 0; cornerScan < 4; cornerScan++) {

    sendXYraw(CHIP_L, cornerScan + 8, 0, 1);

    rowBeingScanned = corners[cornerScan];
    if (readFloatingOrState(pin, rowBeingScanned) == probe) {
      found = rowBeingScanned;
      // if (nextIsSupply)
      // {
      //     leds.setPixelColor(nodesToPixelMap[rowBeingScanned], 65, 10, 10);
      // }
      // else if (nextIsGnd)
      // {
      //     leds.setPixelColor(nodesToPixelMap[rowBeingScanned], 10, 65, 10);
      // }
      // else
      // {
      //     leds.setPixelColor(nodesToPixelMap[rowBeingScanned],
      //     rainbowList[rainbowIndex][0], rainbowList[rainbowIndex][1],
      //     rainbowList[rainbowIndex][2]);
      // }
      // showLEDsCore2 = 1;
      if (found != -1) {
        connectedRows[connectedRowsIndex] = found;
        connectedRowsIndex++;
        found = -1;
        // stopProbe();
        // break;
      }
    }

    sendXYraw(CHIP_L, cornerScan + 8, 0, 0);
  }
  sendXYraw(CHIP_L, xMapRead, 0, 0);

  for (int chipScan2 = CHIP_I; chipScan2 <= CHIP_J;
       chipScan2++) // scan the breadboard (except the corners)
  {

    int pinHeader = ADC0_PIN + (chipScan2 - CHIP_I);

    for (int xToScan = 0; xToScan < 12; xToScan++) {

      sendXYraw(chipScan2, xToScan, 0, 1);
      sendXYraw(chipScan2, 13, 0, 1);

      // analogRead(ADC0_PIN);

      rowBeingScanned = ch[chipScan2].xMap[xToScan];
      //   Serial.print("rowBeingScanned: ");
      //     Serial.println(rowBeingScanned);
      //     Serial.print("chipScan2: ");
      //     Serial.println(chipScan2);
      //     Serial.print("xToScan: ");
      //     Serial.println(xToScan);

      if (readFloatingOrState(pinHeader, rowBeingScanned) == probe) {

        found = rowBeingScanned;

        // if (nextIsSupply)
        // {
        //     //leds.setPixelColor(nodesToPixelMap[rowBeingScanned], 65, 10,
        //     10);
        // }
        // else if (nextIsGnd)
        // {
        //    // leds.setPixelColor(nodesToPixelMap[rowBeingScanned], 10, 65,
        //    10);
        // }
        // else
        // {
        //     //leds.setPixelColor(nodesToPixelMap[rowBeingScanned],
        //     rainbowList[rainbowIndex][0], rainbowList[rainbowIndex][1],
        //     rainbowList[rainbowIndex][2]);
        // }
        // //showLEDsCore2 = 1;
        // // leds.show();

        if (found != -1) {
          connectedRows[connectedRowsIndex] = found;
          connectedRowsIndex++;
          found = -1;
          // stopProbe();
          // break;
        }
      }
      sendXYraw(chipScan2, xToScan, 0, 0);
      sendXYraw(chipScan2, 13, 0, 0);
    }
  }

  // stopProbe();
  // probeTimeout = millis();

  digitalWrite(RESETPIN, HIGH);
  delayMicroseconds(20);
  digitalWrite(RESETPIN, LOW);
  return connectedRows[0];
  // return found;

  // return 0;
}

int readRails(int pin) {
  int state = -1;

  // Serial.print("adc0 \t");
  // Serial.println(adcReadings[0]);
  // Serial.print("adc1 \t");
  // Serial.println(adcReadings[1]);
  // Serial.print("adc2 \t");
  // Serial.println(adcReadings[2]);
  // Serial.print("adc3 \t");
  // Serial.println(adcReadings[3]);

  return state;
}
int hsvProbe = 0;
int hsvProbe2 = 0;
unsigned long probeRainbowTimer = 0;

int lastProbeLEDs = -1;
volatile int showingProbeLEDs = 0;

void probeLEDhandler(void) {

  core2busy = true;
  // pinMode(2, OUTPUT);
  // pinMode(9, INPUT);
  showingProbeLEDs = 1;

  //         Serial.print("showProbeLEDs = ");
  // Serial.println(showProbeLEDs);
  switch (showProbeLEDs) {
  case 1:
    if (connectOrClearProbe == 1 && node1or2 == 1) {
      probeLEDs.setPixelColor(0, 0x0f0fc6); // connect bright
    } else {
      probeLEDs.setPixelColor(0, 0x000032); // connect
    }
    // probeLEDs[0].setColorCode(0x000011);
    //  Serial.println(showProbeLEDs);
    //   probeLEDs.show();
    // showProbeLEDs = 0;
    break;
  case 2: {

    // if (connectOrClearProbe == 0) {
    //  Serial.print("removeFade = ");
    //  Serial.println(removeFade);
    switch (removeFade) {
    case 0:
      probeLEDs.setPixelColor(0, 0x280000); // remove
      break;
    case 1:
      probeLEDs.setPixelColor(0, 0x330101); // remove
      break;
    case 2:
      probeLEDs.setPixelColor(0, 0x3c0202); // remove
      break;
    case 3:
      probeLEDs.setPixelColor(0, 0x450303); // remove
      break;
    case 4:
      probeLEDs.setPixelColor(0, 0x4e0404); // remove
      break;
    case 5:
      probeLEDs.setPixelColor(0, 0x570505); // remove
      break;
    case 6:
      probeLEDs.setPixelColor(0, 0x600707); // remove
      break;
    case 7:
      probeLEDs.setPixelColor(0, 0x690909); // remove
      break;
    case 8:
      probeLEDs.setPixelColor(0, 0x820a0a); // remove
      break;
    case 9:
      probeLEDs.setPixelColor(0, 0xab1010); // remove
      break;
    case 10:
      probeLEDs.setPixelColor(0, 0xff1a1a); // remove
      break;
    default:
      probeLEDs.setPixelColor(0, 0x280000); // remove
      break;
    }
    // } else {
    //   probeLEDs.setPixelColor(0, 0x360000); // remove
    // }
    // probeLEDs.setPixelColor(0, 0x360000); // remove
    // probeLEDs[0].setColorCode(0x110000);
    //  probeLEDs.show();
    //  Serial.println(showProbeLEDs);
    showProbeLEDs = 0;
    break;
  }
  case 3:
    probeLEDs.setPixelColor(0, 0x003600); // measure
    // probeLEDs[0].setColorCode(0x001100);
    //  probeLEDs.show();
    //  Serial.println(showProbeLEDs);
    break;
  case 4:

    probeLEDs.setPixelColor(0, 0x170c17); // select idle
    // probeLEDs[0].setColorCode(0x110011);
    //  probeLEDs.show();
    //  Serial.println(showProbeLEDs);
    break;
  case 5:
    probeLEDs.setPixelColor(0, 0x111111); // all
    // probeLEDs[0].setColorCode(0x111111);
    //  Serial.println(showProbeLEDs);
    break;
  case 6:
    probeLEDs.setPixelColor(0, 0x0c190c); // measure dim
    break;
  case 7: {
    // hsvProbe++;
    if (hsvProbe > 255) {
      hsvProbe = 0;
      hsvProbe2 -= 8;
      if (hsvProbe2 < 15) {
        hsvProbe2 = 255;
      }
    }
    hsvColor probeColor;
    probeColor.h = hsvProbe;
    probeColor.s = hsvProbe2;
    probeColor.v = 25;

    uint32_t colorp = packRgb(HsvToRgb(probeColor));
    probeLEDs.setPixelColor(0, colorp); // select idle dim
    break;
  }
  case 8:
    probeLEDs.setPixelColor(0, 0xffffff); // max
    showProbeLEDs = 9;
    while (showProbeLEDs == 9) {
      probeLEDs.show();
      delayMicroseconds(100);
      // Serial.println("max");
    }
    showProbeLEDs = 0;
    Serial.println("max");
    break;
  default:
    break;
  }
  lastProbeLEDs = showProbeLEDs;

  // while(checkingButton == 1) {
  //   //Serial.println("checkingButton");
  // }

  // FastLED.show();
  // delay(1);
  /// pinMode(2, INPUT);
  // pinMode(9, OUTPUT);
  //       Serial.print("showProbeLEDs = ");
  // Serial.println(showProbeLEDs);

  probeLEDs.show();
  showingProbeLEDs = 0;
}


void highlightNets(int probeReading) {
   // Serial.print("justReadProbe = ");
      // Serial.println(probeReading);
      // delay(100);

      int netHighlighted = brightenNet(probeReading);
      // Serial.print("netHighlighted = ");
      // Serial.println(netHighlighted);
      if (netHighlighted != -1) {

        Serial.print("\r                                               \r");
        switch (netHighlighted) {
        case 0:
          break;
        case 1:
          Serial.print("Net Highlighted = GND");
          break;
        case 2:
          Serial.print("Net Highlighted = Top Rail  ");

          Serial.print(railVoltage[0]);
          break;
        case 3:
          Serial.print("Net Highlighted = Bottom Rail  ");

          Serial.print(railVoltage[1]);
          break;
        case 4:
          Serial.print("Net Highlighted = DAC 0  ");
          Serial.print(dacOutput[0]);
          break;
        case 5:
          Serial.print("Net Highlighted = DAC 1  ");
          Serial.print(dacOutput[1]);
          break;
        default:
          Serial.print("Net Highlighted = ");
          Serial.print(netHighlighted);
        }
        showLEDsCore2 = 1;
      } else {

        // brightenNet(-1);
      }

}