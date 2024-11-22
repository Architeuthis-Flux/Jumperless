// SPDX-License-Identifier: MIT

/*
Kevin Santo Cappuccio
Architeuthis Flux

KevinC@ppucc.io

5/28/2024

*/

#include <Arduino.h>

#define USE_TINYUSB 1

#define LED LED_BUILTIN

#ifdef USE_TINYUSB
#include <Adafruit_TinyUSB.h>
#endif

#ifdef CFG_TUSB_CONFIG_FILE
#include CFG_TUSB_CONFIG_FILE
#else
#include "tusb_config.h"
#endif

#include "ArduinoStuff.h"
#include "JumperlessDefinesRP2040.h"
#include "MatrixStateRP2040.h"
#include "NetManager.h"

#include "LEDs.h"
#include "NetsToChipConnections.h"
// #include "CH446Q.h"
#include "MachineCommands.h"
#include "Peripherals.h"
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
// #include <EEPROM.h>

#include <EEPROM.h>

#include "CH446Q.h"
#include "FileParsing.h"
#include "RotaryEncoder.h"

#include "LittleFS.h"

#include "Probing.h"

// #include "AdcUsb.h"

// #include "logic_analyzer.h"
// #include "pico/multicore.h"
// #include "hardware/watchdog.h"

// using namespace logic_analyzer;

// int pinStart=26;
// int numberOfPins=2;
// LogicAnalyzer logicAnalyzer;
// Capture capture(MAX_FREQ, MAX_FREQ_THRESHOLD);

Adafruit_USBD_CDC USBSer1;

int supplySwitchPosition = 0;

void machineMode(void);
void lastNetConfirm(int forceLastNet = 0);

unsigned long lastNetConfirmTimer = 0;
// int machineMode = 0;

volatile int sendAllPathsCore2 =
    0; // this signals the core 2 to send all the paths to the CH446Q

int loadFileOnStart = 1;
int rotEncInit = 0;
// https://wokwi.com/projects/367384677537829889

int core2setupFinished = 0;

int baudRate = 115200;

int excelCounter = 0;

void setup() {
  pinMode(0, OUTPUT);
  pinMode(2, INPUT);
  pinMode(3, INPUT);

  // debugFlagInit();
  USBDevice.setProductDescriptor("Jumperless");
  USBDevice.setManufacturerDescriptor("Architeuthis Flux");
  USBDevice.setSerialDescriptor("0");
  USBDevice.setID(0x1D50, 0xACAB);
  USBDevice.addStringDescriptor("Jumperless");
  USBDevice.addStringDescriptor("Architeuthis Flux");

  USBSer1.setStringDescriptor("Jumperless USB Serial");

  USBSer1.begin(115200);

#ifdef EEPROMSTUFF
  EEPROM.begin(256);
  debugFlagInit();

#endif
  // delay(1);
  // initADC();
  delay(1);
  initDAC(); // also sets revisionNumber
  delay(1);
  initINA219();
  delay(1);
  Serial.begin(115200);

  initArduino();
  delay(4);

  LittleFS.begin();

  setDac0_5Vvoltage(0.0);
  setDac1_8Vvoltage(1.9);

  // if (rotaryEncoderMode == 1)
  // {
  createSlots(-1, rotaryEncoderMode);
  //}
  //
  clearAllNTCC();

  // if (rotaryEncoderMode == 1)
  // {
  // rotEncInit = 1;
  initRotaryEncoder();
  //}
  initADC();

  // delay(20);
  // setupAdcUsbStuff(); // I took this out because it was causing a crash on
  delay(10);
  setBaudRate(baudRate);
}

void setup1() {
  delay(4);

  initCH446Q();

  delay(4);
  initLEDs();
  delay(4);
  startupColors();
  delay(4);
  lightUpRail();
  core2setupFinished = 1;
  delay(4);

  showLEDsCore2 = 1;
}

char connectFromArduino = '\0';

char input;

int serSource = 0;
int readInNodesArduino = 0;

int restoredNodeFile = 0;

const char firmwareVersion[] = "1.3.19"; //// remember to update this

int firstLoop = 1;
volatile int probeActive = 1;

int showExtraMenu = 0;
int printingMeasurements = 0;

void loop() {

  unsigned long timer = 0;

menu:

  // showMeasurements();
  //   unsigned long connecttimer = 0;
  // //   while (tud_connected() == 0)
  // //   {
  // // connecttimer = millis();

  // //   }
  // Serial.print("Updated!\n\r");

  Serial.print("\n\n\r\t\tMenu\n\r");
  // Serial.print("Slot ");
  // Serial.print(netSlot);
  Serial.print("\n\r");
  Serial.print("\tm = show this menu\n\r");
  Serial.print("\tn = show netlist\n\r");
  Serial.print("\ts = show node files by slot\n\r");
  Serial.print("\to = load node files by slot\n\r");
  Serial.print("\tf = load node file to current slot\n\r");
  Serial.print("\tr = rotary encoder mode -");
  rotaryEncoderMode == 1 ? Serial.print(" ON (z/x to cycle)\n\r")
                         : Serial.print(" off\n\r");
  Serial.print("\t\b\bz/x = cycle slots - current slot ");
  Serial.print(netSlot);
  Serial.print("\n\r");
  Serial.print("\te = show extra menu options\n\r");

  if (showExtraMenu == 1) {
    Serial.print("\tb = show bridge array\n\r");
    Serial.print("\tp = probe connections\n\r");
    Serial.print("\tw = waveGen\n\r");
    Serial.print("\tv = toggle show current/voltage\n\r");
    Serial.print("\tu = set baud rate for USB-Serial\n\r");
    Serial.print("\tl = LED brightness / test\n\r");
    Serial.print("\td = toggle debug flags\n\r");
  }
  // Serial.print("\tc = clear nodes with probe\n\r");
  Serial.print("\n\n\r");

  if (firstLoop == 1 && rotaryEncoderMode == 1) {
    Serial.print("Use the rotary encoder to change slots\n\r");
    Serial.print("Press the button to select\n\r");
    Serial.print("\n\n\r");
    firstLoop = 0;
    probeActive = 0;

    goto loadfile;
  } else if (firstLoop == 1) {
    // delay(20);
    // Serial.println("Press the button to load the current slot\n\r");
    // delay(20);
    firstLoop = 0;
    probeActive = 0;

    goto loadfile;
  }
  // if (firstLoop == 1 && rotaryEncoderMode == 0 && loadFileOnStart == 1)
  // {
  //   delay(50);
  //   firstLoop = 0;
  //   probeActive = 0;

  //   goto loadfile;
  // }

  // Serial.print("Slot: ");
  // Serial.println(netSlot);

dontshowmenu:

  connectFromArduino = '\0';

  while (Serial.available() == 0 && connectFromArduino == '\0' &&
         slotChanged == 0) {

    if (showReadings >= 1) {
      showMeasurements();
      // Serial.print("\n\n\r");
      // showLEDsCore2 = 1;
    }
    if (BOOTSEL) {
      lastNetConfirm(1);
    }

    if ((millis() % 200) < 5) {
      if (checkProbeButton() == 1) {
        int longShort = longShortPress(1000);
        if (longShort == 1) {
          input = 'c';
          probingTimer = millis();
          goto skipinput;
        } else if (longShort == 0) {
          input = 'p';
          probingTimer = millis();
          goto skipinput;
        }
      }

      // pinMode(19, INPUT);
    }
  }

  if (slotChanged == 1) {

    // showLEDsCore2 = 1;

    goto loadfile;
  }

  if (connectFromArduino != '\0') {
  } else {
    input = Serial.read();
    // Serial.print("\n\r");
    if (input == '}' || input == ' ' || input == '\n' || input == '\r') {
      goto dontshowmenu;
    }
    // Serial.write(input);
  }

// Serial.print(input);
skipinput:
  switch (input) {
  case '?': {
    Serial.print("Jumperless firmware version: ");
    Serial.println(firmwareVersion);
    break;
  }
  case '$': {
    // return current slot number
    Serial.println(netSlot);
    break;
  }
  case '_': // hold arduino in reset
  {
    pinMode(16, OUTPUT);
    pinMode(17, INPUT);
    clearAllConnectionsOnChip(CHIP_I, 0);

    sendXYraw(CHIP_I, 11, 0, 1);
    sendXYraw(CHIP_I, 15, 0, 1);

    sendXYraw(CHIP_I, 11, 1, 1); // double up connections
    sendXYraw(CHIP_I, 15, 1, 1);

    goto dontshowmenu;
  }
  case '-': {
    clearAllConnectionsOnChip(CHIP_I, 1);

    sendXYraw(CHIP_I, 11, 0, 0);
    sendXYraw(CHIP_I, 15, 0, 0);

    sendXYraw(CHIP_I, 11, 1, 0); // double up connections
    sendXYraw(CHIP_I, 15, 1, 0);

    sendAllPathsCore2 = 1;

    break;
  }
  case 'e': {
    if (showExtraMenu == 0) {
      showExtraMenu = 1;
    } else {
      showExtraMenu = 0;
    }
    break;
  }

  case 's': {
    int fileNo = -1;

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

        Serial.print("\n\rf ");
        printNodeFile(i);
        Serial.print("\n\n\n\r");
      }
    } else {

      Serial.print("\n\rnodeFileSlot");
      Serial.print(fileNo - '0');
      Serial.print(".txt\n\r");

      Serial.print("\n\rf ");

      printNodeFile(fileNo - '0');
      Serial.print("\n\r");
    }

    break;
  }
  case 'v':

    if (showReadings >= 3 || (inaConnected == 0 && showReadings >= 1)) {
      showReadings = 0;
      break;
    } else {
      showReadings++;

      chooseShownReadings();
      // Serial.println(showReadings);

      // Serial.write("\033"); //these VT100/ANSI commands work on some
      // terminals and not others so I took it out Serial.write("\x1B\x5B");
      // Serial.write("1F");//scroll up one line
      // Serial.write("\x1B\x5B");
      // Serial.write("\033");
      // Serial.write("2K");//clear line
      // Serial.write("\033");
      // Serial.write("\x1B\x5B");
      // Serial.write("1F");//scroll up one line
      // Serial.write("\x1B\x5B");
      // Serial.write("\033");
      // Serial.write("2K");//clear line

      goto dontshowmenu;
      // break;
    }
  case 'p': {
    probeActive = 1;

    delayMicroseconds(1500);
    probeMode(19, 1);
    delayMicroseconds(1500);
    probeActive = 0;
    break;
  }
  case 'c': {
    // removeBridgeFromNodeFile(19, 1);
    probeActive = 1;
    delayMicroseconds(1500);
    probeMode(19, 0);
    delayMicroseconds(1500);
    probeActive = 0;
    break;
  }
  case 'n':
    couldntFindPath(1);
    Serial.print("\n\n\rnetlist\n\n\r");
    listSpecialNets();
    listNets();

    break;
  case 'b':
    couldntFindPath(1);
    Serial.print("\n\n\rBridge Array\n\r");
    printBridgeArray();
    Serial.print("\n\n\n\rPaths\n\r");
    printPathsCompact();
    Serial.print("\n\n\rChip Status\n\r");
    printChipStatus();
    Serial.print("\n\n\r");
    Serial.print("Revision ");
    Serial.print(revisionNumber);
    Serial.print("\n\n\r");
    break;

  case 'm':

    break;

  case '!':
    printNodeFile();
    break;

  case '*': {
    excelCounter++;
    Serial.print("42, 56.9, ");
    Serial.println(excelCounter);
    break;
  }

  case 'w':

    if (waveGen() == 1) {
      break;
    }
  case 'o': {
    probeActive = 1;
    inputNodeFileList(rotaryEncoderMode);
    showSavedColors(netSlot);
    // input = ' ';
    showLEDsCore2 = 1;
    sendAllPathsCore2 = 1;
    probeActive = 0;
    chooseShownReadings();
    goto dontshowmenu;
    break;
  }
  case '&': {
    int numberOfAddressBits = 0;
    int addressRows[20] = {-1};

    // while (Serial.available() > 0) {
    //   Serial.read();
    // }

    Serial.print("\n\rEnter rows for address lines (lsb first)\n\r(enter x "
                 "when finished)\n\r");
    for (int i = 0; i < 20; i++) {

      Serial.print("\n\rAddress ");
      Serial.print(i);
      Serial.print(">  ");

      while (Serial.available() == 0) {
      }

      int row = Serial.read();
      if (row == 'x') {
        numberOfAddressBits = i;
        Serial.print("x\n\r");
        break;
      }
      if (isDigit(row) == false) {
        continue;
      }

      addressRows[i] = row - '0';
      char nextChar = Serial.peek();
      if (Serial.available() > 0 && isDigit(nextChar) == true) {
        row = Serial.read();

        addressRows[i] *= 10;
        addressRows[i] += row - '0';
      }

      // Serial.print(addressRows[i]);
    }

    for (int i = 0; i < numberOfAddressBits; i++) {
      Serial.print(addressRows[i]);
      Serial.print(" ");
    }

    int numberOfDataBits = 0;
    int dataRows[20] = {-1};

    // while (Serial.available() > 0) {
    //   Serial.read();
    // }

    Serial.print("\n\rEnter rows for data lines (lsb first)\n\r(enter x when "
                 "finished)\n\r");
    for (int i = 0; i < 20; i++) {

      Serial.print("\n\rData ");
      Serial.print(i);
      Serial.print(">  ");

      while (Serial.available() <= 0) {
      }

      int row = Serial.read();
      if (row == 'x') {
        numberOfDataBits = i;
        Serial.print("x\n\r");
        break;
      }
      if (isDigit(row) == false) {
        continue;
      }

      dataRows[i] = row - '0';
      char nextChar = Serial.peek();
      if (Serial.available() > 0 && isDigit(nextChar) == true) {
        row = Serial.read();

        dataRows[i] *= 10;
        dataRows[i] += row - '0';
      }

      Serial.print(dataRows[i]);
    }

    int clockRow = 0;

    // while (Serial.available() > 0) {
    //   Serial.read();
    // }

    Serial.print("enter clock row\n\rClock >");

    while (Serial.available() == 0) {
    }

    int row = Serial.read();
    if (isDigit(row) == false) {

    } else {

      clockRow = row - '0';
      if (Serial.available() > 0 && Serial.peek() != '\n') {
        row = Serial.read();

        clockRow *= 10;
        clockRow += row - '0';
      }
    }
    Serial.print("\n\n\n\n\r");

    Serial.print("\n\rNumber of address bits: ");
    Serial.print(numberOfAddressBits);
    // Serial.print("\n\r");
    Serial.print("\n\rNumber of data bits: ");
    Serial.print(numberOfDataBits);
    Serial.print("\n\r");
    Serial.print("\n\rClock row: ");

    Serial.print(clockRow);
    Serial.print("\n\r");
    for (int i = 0; i < numberOfAddressBits; i++) {
      Serial.print("a");
      Serial.print(i);
      Serial.print(": ");
      Serial.print(addressRows[i]);
      Serial.print("  ");
    }

    Serial.print("\n\r");
    for (int i = 0; i < numberOfDataBits; i++) {
      Serial.print("d");
      Serial.print(i);
      Serial.print(": ");
      Serial.print(dataRows[i]);
      Serial.print("  ");
    }

    // createSlots(netSlot, 0); //clears the netlist
    int addressCount = 0;

    bool addressBits[20] = {0};

    bool dataBits[20] = {0};

    for (addressCount = 0; addressCount < pow(2, numberOfAddressBits);
         addressCount++) {
      addressBits[addressRows[addressCount]] = 1;
      // Serial.println(addressRows[addressCount], BIN);
      // Serial.println(addressCount, BIN);

      // createSlots(netSlot, 0); // clears the netlist
      // delay(100);

      for (int i = 0; i < numberOfAddressBits; i++) {
        // Serial.print(addressCount & (1 << i) ? "1" : "0");
        addressBits[i] = addressCount & (1 << i);

        Serial.print(addressBits[i] == true ? "1" : "0");
        removeBridgeFromNodeFile(addressRows[i], -1, netSlot);
        delay(1);
        addBridgeToNodeFile(addressRows[i],
                            addressBits[i] == true ? SUPPLY_5V : GND, netSlot);
      }
      Serial.println();
      // TODO: add data bits or do something with them
      // TODO: wiggle the clock line

      delay(10);
      // printNodeFile();

      probeActive = 1;
      clearAllNTCC();
      openNodeFile(netSlot);
      getNodesToConnect();
      bridgesToPaths();
      clearLEDs();
      digitalWrite(RESETPIN, HIGH);
      delayMicroseconds(100);
      digitalWrite(RESETPIN, LOW);
      showSavedColors(netSlot);
      chooseShownReadings();
      sendAllPathsCore2 = 1;
      showLEDsCore2 = 1;
      probeActive = 0;

      delay(200);
    }

    break;
  }

  case 'x': {

    if (netSlot == NUM_SLOTS - 1) {
      netSlot = 0;
    } else {
      netSlot++;
    }

    Serial.print("\r                                         \r");
    Serial.print("Slot ");
    Serial.print(netSlot);
    slotPreview = netSlot;
    chooseShownReadings();
    goto loadfile;
  }
  case 'z': {

    if (netSlot == 0) {
      netSlot = NUM_SLOTS - 1;
    } else {
      netSlot--;
    }
    Serial.print("\r                                         \r");
    Serial.print("Slot ");
    Serial.print(netSlot);
    slotPreview = netSlot;
    chooseShownReadings();
    goto loadfile;
  }
  case 'y': {
  loadfile:
    probeActive = 1;
    clearAllNTCC();
    openNodeFile(netSlot);
    getNodesToConnect();
    bridgesToPaths();
    clearLEDs();
    // leds.clear();
    // assignNetColors();
    digitalWrite(RESETPIN, HIGH);
    delayMicroseconds(3900);
    // Serial.print("bridgesToPaths\n\r");
    digitalWrite(RESETPIN, LOW);
    // showNets();
    // saveRawColors(netSlot);
    probeActive = 0;
    showSavedColors(netSlot);
    chooseShownReadings();
    sendAllPathsCore2 = 1;
    slotChanged = 0;
    input = ' ';
    // break;
    // if (rotaryEncoderMode == 1)
    // {
    //   goto dontshowmenu;
    // }

    break;
  }
  case 'f':

    probeActive = 1;
    readInNodesArduino = 1;
    clearAllNTCC();

    // sendAllPathsCore2 = 1;
    timer = millis();

    //clearNodeFile(netSlot);
    digitalWrite(RESETPIN, HIGH);
    if (connectFromArduino != '\0') {
      serSource = 1;
    } else {
      serSource = 0;
    }
    savePreformattedNodeFile(serSource, netSlot, rotaryEncoderMode);

    // Serial.print("savePFNF\n\r");
    // debugFP = 1;
    openNodeFile(netSlot);
    getNodesToConnect();
    // Serial.print("openNF\n\r");

    bridgesToPaths();
    clearLEDs();
    assignNetColors();
    delay(10);
    // Serial.print("bridgesToPaths\n\r");
    digitalWrite(RESETPIN, LOW);
    // showNets();
    // saveRawColors(netSlot);
    chooseShownReadings();
    sendAllPathsCore2 = 1;
    showLEDsCore2 = 1;

    if (debugNMtime) {
      Serial.print("\n\n\r");
      Serial.print("took ");
      Serial.print(millis() - timer);
      Serial.print("ms");
    }
    input = ' ';

    probeActive = 0;
    if (connectFromArduino != '\0') {
      connectFromArduino = '\0';
      // Serial.print("connectFromArduino\n\r");
      //  delay(2000);
      input = ' ';
      readInNodesArduino = 0;

      goto dontshowmenu;
    }

    connectFromArduino = '\0';
    readInNodesArduino = 0;
    break;

  case '\n':
    goto menu;
    break;

  case 't':
#ifdef FSSTUFF
    clearNodeFile();
#endif

#ifdef EEPROMSTUFF
    lastCommandWrite(input);

    runCommandAfterReset('t');
#endif

#ifdef FSSTUFF
    openNodeFile();
    getNodesToConnect();
#endif
    Serial.println("\n\n\rnetlist\n\n\r");

    bridgesToPaths();

    listSpecialNets();
    listNets();
    printBridgeArray();
    Serial.print("\n\n\r");
    Serial.print(numberOfNets);

    Serial.print("\n\n\r");
    Serial.print(numberOfPaths);

    assignNetColors();
#ifdef PIOSTUFF
    sendAllPaths();
#endif

    break;

  case 'l':
    if (LEDbrightnessMenu() == '!') {
      clearLEDs();
      delayMicroseconds(9200);
      sendAllPathsCore2 = 1;
    }
    break;

  case 'r':

    if (rotaryEncoderMode == 1) {
      // unInitRotaryEncoder();

      rotaryEncoderMode = 0;
      // createSlots(-1, rotaryEncoderMode);
      //  showSavedColors(netSlot);
      // assignNetColors();

      // showNets();
      lightUpRail();

      showLEDsCore2 = 1;
      debugFlagSet(10); // encoderModeOff
      goto menu;
    } else {
      rotaryEncoderMode = 1;
      if (rotEncInit == 0) // only do this once
      {
        createSlots(-1, rotaryEncoderMode);
        // initRotaryEncoder();
        rotEncInit = 1;
        // Serial.print("\n\n\r (you should unplug an)");
      }
      printRotaryEncoderHelp();
      delay(100);
      // initRotaryEncoder();
      // refreshSavedColors();
      showSavedColors(netSlot);
      showLEDsCore2 = 1;
      debugFlagSet(11); // encoderModeOn

      //   delay(700);
      //   Serial.flush();
      //   Serial.end();

      //   delay(700);
      //       watchdog_enable(1, 1);
      // while(1);
      //*((volatile uint32_t*)(PPB_BASE + 0x0ED0C)) = 0x5FA0004;
    }
    goto dontshowmenu;

    break;

  case 'u':
    Serial.print("\n\r");
    Serial.print("enter baud rate\n\r");
    while (Serial.available() == 0)
      ;
    baudRate = Serial.parseInt();
    Serial.print("\n\r");
    Serial.print("setting baud rate to ");
    Serial.print(baudRate);
    Serial.print("\n\r");

    setBaudRate(baudRate);
    break;

  case 'd': {
    debugFlagInit();

  debugFlags:

    Serial.print("\n\r0.   all off");
    Serial.print("\n\r9.   all on");
    Serial.print("\n\ra-z. exit\n\r");

    Serial.print("\n\r1. file parsing           =    ");
    Serial.print(debugFP);
    Serial.print("\n\r2. net manager            =    ");
    Serial.print(debugNM);
    Serial.print("\n\r3. chip connections       =    ");
    Serial.print(debugNTCC);
    Serial.print("\n\r4. chip conns alt paths   =    ");
    Serial.print(debugNTCC2);
    Serial.print("\n\r5. LEDs                   =    ");
    Serial.print(debugLEDs);
    Serial.print("\n\n\r6. swap probe pin         =    ");
    if (probeSwap == 0) {
      Serial.print("19");
    } else {
      Serial.print("18");
    }
    Serial.print("\n\n\r7. load slots on start    =    ");
    Serial.print(loadFileOnStart ? "on" : "off");

    Serial.print("\n\n\n\r");

    while (Serial.available() == 0)
      ;

    int toggleDebug = Serial.read();
    Serial.write(toggleDebug);
    toggleDebug -= '0';

    if (toggleDebug >= 0 && toggleDebug <= 9) {

      debugFlagSet(toggleDebug);

      delay(10);

      goto debugFlags;
    } else {
      break;
    }
  }

  case ':':

    if (Serial.read() == ':') {
      // Serial.print("\n\r");
      // Serial.print("entering machine mode\n\r");
      machineMode();
      showLEDsCore2 = 1;
      goto dontshowmenu;
      break;
    } else {
      break;
    }

  default:
    while (Serial.available() > 0) {
      int f = Serial.read();
      delayMicroseconds(30);
    }

    break;
  }

  goto menu;
}

// #include <string> // Include the necessary header file

void lastNetConfirm(int forceLastNet) {
  // while (tud_connected() == 0 && millis() < 500)
  //   ;

  // if (millis() - lastNetConfirmTimer < 3000 && tud_connected() == 1)
  // {
  //   // Serial.println(lastNetConfirmTimer);

  //   // lastNetConfirmTimer = millis();
  //   return;
  // }

  if (forceLastNet == 1) {

    int bootselPressed = 0;
    openNodeFile();
    getNodesToConnect();
    // Serial.print("openNF\n\r");
    digitalWrite(RESETPIN, HIGH);
    bridgesToPaths();
    clearLEDs();
    assignNetColors();

    sendAllPathsCore2 = 1;
    Serial.print("\n\r   short press BOOTSEL to restore last netlist\n\r");
    Serial.print("   long press to cancel\n\r");
    delay(250);
    if (BOOTSEL) {
      bootselPressed = 1;
    }

    while (forceLastNet == 1) {
      if (BOOTSEL)
        bootselPressed = 1;

      // clearLEDs();
      // leds.show();
      leds.clear();
      lightUpRail(-1, -1, 1, 28, supplySwitchPosition);
      leds.show();
      // showLEDsCore2 = 1;

      if (BOOTSEL)
        bootselPressed = 1;

      delay(250);

      // showLEDsCore2 = 2;
      sendAllPathsCore2 = 1;
      // Serial.print("p\n\r");
      if (BOOTSEL)
        bootselPressed = 1;
      // delay(250);

      if (bootselPressed == 1) {
        unsigned long longPressTimer = millis();
        int fade = 8;
        while (BOOTSEL) {

          sendAllPathsCore2 = 1;
          showLEDsCore2 = 2;
          delay(250);
          clearLEDs();
          // leds.clear();
          showLEDsCore2 = 2;

          if (fade <= 0) {
            clearAllNTCC();
            clearLEDs();
            startupColors();
            // clearNodeFile();
            sendAllPathsCore2 = 1;
            lastNetConfirmTimer = millis();
            restoredNodeFile = 0;
            // delay(1000);
            Serial.print("\n\r   cancelled\n\r");
            return;
          }

          delay(fade * 10);
          fade--;
        }

        digitalWrite(RESETPIN, LOW);
        restoredNodeFile = 1;
        sendAllPathsCore2 = 1;
        Serial.print("\n\r   restoring last netlist\n\r");
        printNodeFile();
        return;
      }
      delay(250);
    }
  }
}
unsigned long lastTimeNetlistLoaded = 0;
unsigned long lastTimeCommandRecieved = 0;

void machineMode(void) // read in commands in machine readable format
{
  int sequenceNumber = -1;

  lastTimeCommandRecieved = millis();

  if (millis() - lastTimeCommandRecieved > 100) {
    machineModeRespond(sequenceNumber, true);
    return;
  }
  enum machineModeInstruction receivedInstruction =
      parseMachineInstructions(&sequenceNumber);

  // Serial.print("receivedInstruction: ");
  // Serial.print(receivedInstruction);
  // Serial.print("\n\r");

  switch (receivedInstruction) {
  case netlist:
    lastTimeNetlistLoaded = millis();
    clearAllNTCC();

    // writeNodeFileFromInputBuffer();

    digitalWrite(RESETPIN, HIGH);

    machineNetlistToNetstruct();
    populateBridgesFromNodes();
    bridgesToPaths();

    clearLEDs();
    assignNetColors();
    // showNets();
    digitalWrite(RESETPIN, LOW);
    sendAllPathsCore2 = 1;
    break;

  case getnetlist:
    if (millis() - lastTimeNetlistLoaded > 300) {

      listNetsMachine();
    } else {
      machineModeRespond(0, true);
      // Serial.print ("too soon bro\n\r");
      return;
    }
    break;

  case bridgelist:
    clearAllNTCC();

    writeNodeFileFromInputBuffer();

    openNodeFile();
    getNodesToConnect();
    // Serial.print("openNF\n\r");
    digitalWrite(RESETPIN, HIGH);
    bridgesToPaths();
    clearLEDs();
    assignNetColors();
    // Serial.print("bridgesToPaths\n\r");
    digitalWrite(RESETPIN, LOW);
    // showNets();

    sendAllPathsCore2 = 1;
    break;

  case getbridgelist:
    listBridgesMachine();
    break;

  case lightnode:
    lightUpNodesFromInputBuffer();
    break;

  case lightnet:
    lightUpNetsFromInputBuffer();
    //   lightUpNet();
    // assignNetColors();
    // showLEDsCore2 = 1;
    break;

    // case getmeasurement:
    //   showMeasurements();
    //   break;

  case setsupplyswitch:

    supplySwitchPosition = setSupplySwitch();
    // printSupplySwitch(supplySwitchPosition);
    machineModeRespond(sequenceNumber, true);

    showLEDsCore2 = 1;
    break;

  case getsupplyswitch:
    // if (millis() - lastTimeNetlistLoaded > 100)
    //{

    printSupplySwitch(supplySwitchPosition);
    // machineModeRespond(sequenceNumber, true);

    // }else {
    // Serial.print ("\n\rtoo soon bro\n\r");
    // machineModeRespond(0, true);
    // return;
    // }
    break;

  case getchipstatus:
    printChipStatusMachine();
    break;

    // case gpio:
    //   break;
  case getunconnectedpaths:
    getUnconnectedPaths();
    break;

  case unknown:
    machineModeRespond(sequenceNumber, false);
    return;
  }

  machineModeRespond(sequenceNumber, true);
}

void loadFile(int slot) {
  clearAllNTCC();
  openNodeFile(netSlot);
  getNodesToConnect();
  bridgesToPaths();
  clearLEDs();
  assignNetColors();
  digitalWrite(RESETPIN, HIGH);
  delayMicroseconds(2000);
  // Serial.print("bridgesToPaths\n\r");
  digitalWrite(RESETPIN, LOW);
  // showNets();
  saveRawColors(netSlot);
  sendAllPathsCore2 = 1;
  /// input = ' ';
}

unsigned long logoFlashTimer = 0;

int arduinoReset = 0;
unsigned long lastTimeReset = 0;
volatile uint8_t pauseCore2 = 0;

void loop1() // core 2 handles the LEDs and the CH446Q8
{

  // while (1) rainbowBounce(50); //I uncomment this to test the LEDs on a fresh
  // board

  while (pauseCore2 == 1) {
  }

  if (showLEDsCore2 >= 1) {
    int rails = showLEDsCore2;

    if (rails == 1 || rails == 2) {
      lightUpRail(-1, -1, 1, 28, supplySwitchPosition);
    }

    if (rails != 2) {
      showNets();
    }

    if (rails > 3) {
      Serial.print("\n\r");
      Serial.print(rails);
    }
    delayMicroseconds(2200);

    leds.show();
    delayMicroseconds(5200);
    showLEDsCore2 = 0;
  }

  if (sendAllPathsCore2 == 1) {
    digitalWrite(RESETPIN, HIGH);
    delayMicroseconds(3900);
    digitalWrite(RESETPIN, LOW);
    delayMicroseconds(6200);
    sendAllPaths();
    delayMicroseconds(2200);
    showNets();
    delayMicroseconds(7200);
    sendAllPathsCore2 = 0;
  }
  // } else if (USBSer1.available() > 0)
  // {
  //   logicAnalyzer.processCommand();
  // }

  if (arduinoReset == 0 &&
      USBSer1.peek() == 0x30) // 0x30 is the first thing AVRDUDE sends
  {
    // Serial.print("resetting arduino\n\r");

    // resetArduino();
    arduinoReset = 1;
    lastTimeReset = millis();
  }

  // if (digitalRead(RP_GPIO_0) == 1)
  // {
  //   setDac0_5Vvoltage(5.0);
  // } else
  // {
  //   setDac0_5Vvoltage(0.0);
  // }

  while (arduinoReset == 1) {

    if (USBSer1.available()) {

      char ch = USBSer1.read();
      // Serial.write(ch);
      Serial1.write(ch);
    }

    if (Serial1.available()) {
      char ch = Serial1.read();
      // Serial.write(ch);
      USBSer1.write(ch);
    }

    if (millis() - lastTimeReset >
        4000) // if the arduino hasn't been reset in a second, reset the flag
    {
      arduinoReset = 0;
    }
  }

  if (readInNodesArduino == 0) {

    while (USBSer1.available()) {

      char ch = USBSer1.read();
      Serial1.write(ch);
      // Serial.print(ch);
    }

    if (Serial1.available()) {
      char ch = Serial1.read();
      USBSer1.write(ch);
      // Serial.print(ch);

      if (ch == 'f' && connectFromArduino == '\0') {
        input = 'f';

        connectFromArduino = 'f';
        Serial.print("!!!!");
        while (connectFromArduino == 'f') {
          // delay(10);
        }
      } else {
        // connectFromArduino = '\0';
      }
    }
  }

  if (rotaryEncoderMode == 1) {
    rotaryEncoderStuff();
  } else {
    showingPreview = 0;

    if (logoFlash == 2) {
      logoFlashTimer = millis();
      logoFlash = 1;
    }

    if (logoFlash == 1 && logoFlashTimer != 0 &&
        millis() - logoFlashTimer > (20)) {
      logoFlash = 0;
      logoFlashTimer = 0;
      lightUpRail();
      //  if (rotaryEncoderMode == 1)
      //  {
      //    showSavedColors(netSlot);
      //  }
      leds.setPixelColor(110, rawOtherColors[1]);
      // showLEDsCore2 = 1;
      leds.show();
    }
  }
  if (showReadings == 0 && probeActive == 0) {
    showLEDmeasurements();
  }

  // logicAnalyzer.processCommand();
}
