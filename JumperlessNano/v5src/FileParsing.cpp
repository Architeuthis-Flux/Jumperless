// SPDX-License-Identifier: MIT
#include "FileParsing.h"
#include "ArduinoJson.h"
#include "JumperlessDefinesRP2040.h"
#include "LEDs.h"
// #include "LittleFS.h"
#include "Commands.h"
#include "MachineCommands.h"
#include "MatrixStateRP2040.h"
#include "NetManager.h"
#include "Probing.h"
#include "RotaryEncoder.h"
#include "SafeString.h"
#include "menuTree.h"
#include <Arduino.h>
#include <EEPROM.h>
#include <FatFS.h>

volatile bool netsUpdated = true;

bool debugFP = EEPROM.read(DEBUG_FILEPARSINGADDRESS);
bool debugFPtime = EEPROM.read(TIME_FILEPARSINGADDRESS);

createSafeString(nodeFileString, 1800);



int numConnsJson = 0;
createSafeString(specialFunctionsString, 2800);

char inputBuffer[INPUTBUFFERLENGTH] = {0};

ArduinoJson::StaticJsonDocument<8000> wokwiJson;

String connectionsW[MAX_BRIDGES][5];

File nodeFile;

File wokwiFile;

File nodeFileBuffer;

File menuTreeFile;

unsigned long timeToFP = 0;

enum openType {
  w = 0,
  wplus = 1,
  r = 2,
  rplus = 3,
  a = 4,
  aplus = 5,

};

const char rotaryConnectionString[] =
    "{AREF-GND,D11-GND,D10-UART_TX,D12-UART_RX,D13-GPIO_0, ";

void closeAllFiles() {
  if (nodeFile) {
    nodeFile.close();
  }
  if (wokwiFile) {
    wokwiFile.close();
  }
  if (nodeFileBuffer) {
    nodeFileBuffer.close();
  }
}

int openFileThreadSafe(int openTypeEnum, int slot, int flashOrLocal) {
  core1request = 1;
  while (core2busy == true) {
  }
  core1request = 0;
  core1busy = true;

  if (nodeFile) {
    // Serial.println("nodeFile is open");
    nodeFile.close();
  }

  switch (openTypeEnum) {
  case 0:
    nodeFile = FatFS.open("nodeFileSlot" + String(slot) + ".txt", "w");
    break;
  case 1:
    nodeFile = FatFS.open("nodeFileSlot" + String(slot) + ".txt", "w+");
    break;
  case 2:
    nodeFile = FatFS.open("nodeFileSlot" + String(slot) + ".txt", "r");
    break;
  case 3:

    nodeFile = FatFS.open("nodeFileSlot" + String(slot) + ".txt", "r+");
    break;
  case 4:
    nodeFile = FatFS.open("nodeFileSlot" + String(slot) + ".txt", "a");
    break;
  case 5:

    nodeFile = FatFS.open("nodeFileSlot" + String(slot) + ".txt", "a+");
    break;
  default:
    break;
  }

  if (!nodeFile) {
    // if (debugFP)
    Serial.println("\n\n\rFailed to open nodeFile\n\n\r");
    // openFileThreadSafe(w, slot);
    core1busy = false;
    return 0;
  } else {
    if (debugFP)
      Serial.println(
          "\n\ropened nodeFile.txt\n\n\rloading bridges from file\n\r");
  }

  return 1;
}

void writeMenuTree(void) {
  while (core2busy == true) {
    // Serial.println("waiting for core2 to finish");
  }
  core1busy = true;
  FatFS.begin();
  //   delay(100);
  //   FatFS.remove("/MenuTree.txt");
  //  delay(100);
  menuTreeFile = FatFS.open("/MenuTree.txt", "w");
  if (!menuTreeFile) {

    Serial.println("Failed to open menuTree.txt");

  } else {
    // if (debugFP)
    // {
    //     Serial.println("\n\ropened menuTree.txt\n\r");
    // }
    // else
    // {
    //     // Serial.println("\n\r");
    // }
  }
  int menuIndex = 0;
  while (menuTree[menuIndex] != '\0') {
    menuTreeFile.print(menuTree[menuIndex]);
    // Serial.print(menuTree[menuIndex]);
    menuIndex++;
  }
  // menuTreeFile.write(menuTree);
  // menuTreeFile.print(menuTreeString);
  menuTreeFile.close();
  core1busy = false;
}

void createLocalNodeFile(int slot) {
  // Serial.println("creating local node file");
  // Serial.print ("slot = ");
  // Serial.println(slot);
  // Serial.print ("nodeFileString = ");
  // nodeFileString.printTo(Serial);

  // while (core2busy == true) {
  //   // Serial.println("waiting for core2 to finish");
  // }
  // core1busy = true;
  // delay(1);
  // nodeFile = FatFS.open("nodeFileSlot" + String(slot) + ".txt", "a+");
  //     if (!nodeFile) {
  //     if (debugFP)
  //       Serial.println("Failed to open nodeFile");
  //       core1busy = false;
  //     return;
  //   } else {
  //     if (debugFP)
  //       Serial.println("\n\ropened nodeFileSlot" + String(slot) +
  //                      +".txt\n\n\rloading bridges from file\n\r");
  //   }
  openFileThreadSafe(aplus, slot);

  nodeFileString.clear();
  nodeFileString.read(nodeFile);
  nodeFile.close();
  core1busy = false;
  // Serial.println(nodeFileString);
  nodeFileString.replace(" ", "");
  nodeFileString.replace(" ", "");
  nodeFileString.replace("{", "");
  nodeFileString.replace("}", "");
  nodeFileString.prefix("{ ");
  nodeFileString.concat(" } ");

  Serial.println(nodeFileString);
}

void clearNodeFileString() { nodeFileString.clear(); }

void saveLocalNodeFile(int slot) {
  // Serial.println("saving local node file");
  // Serial.print("nodeFileString = ");
  // Serial.println(nodeFileString);

  long count = 0;
  // removeBridgeFromNodeFile(RP_UART_RX, ISENSE_PLUS, netSlot, 1);
  // removeBridgeFromNodeFile(ROUTABLE_BUFFER_IN, ISENSE_MINUS, netSlot, 1);
  //   while (core2busy == true) {
  //     // delay(1);
  //     //  Serial.print("core2");
  //     //  Serial.println(count);
  //     //  count++;
  //   }

  //  core1busy = true;
  //  nodeFileString.printTo(Serial);
  //  //Serial.println("nodeFileSlot" + String(slot) + ".txt");
  //  nodeFile.close();
  //  delay(100);
  //   nodeFile = FatFS.open("nodeFileSlot" + String(slot) + ".txt", "w");
  openFileThreadSafe(w, netSlot);
  nodeFileString.replace(" ", "");
  // nodeFileString.replace(" ", "");
  nodeFileString.replace("{", "");
  nodeFileString.replace("}", "");
  nodeFileString.prefix("{ ");
  nodeFileString.concat(" } ");
  // Serial.println("nodeFileString");
  // delay(3);

  nodeFileString.printTo(nodeFile);
  //
  // for (int i = 0; i < nodeFileString.length(); i++) {
  //   delayMicroseconds(10);
  //   nodeFile.write(nodeFileString.charAt(i));
  //   //Serial.print(nodeFileString.charAt(i));
  // }
  // nodeFileString.printTo(nodeFile);
  // delay(10);
  //  nodeFileString.printTo(nodeFile);

  // nodeFile.close();
  //   nodeFile = FatFS.open("nodeFileSlot" + String(slot) + ".txt", "r");
  //   nodeFile.seek(0);
  // Serial.print ("nodeFile = ");
  //     while (nodeFile.available())
  //     {
  //         Serial.write(nodeFile.read());
  //     }
  // delay(10);
  nodeFile.close();
  core1busy = false;
  // Serial.println("\n\n\rsaved local node file");
}

void createSlots(int slot, int addRotaryConnections) {

  // FatFS.open("nodeFileSlot0.txt", "r");
  if (slot == -1) {
    for (int i = 0; i < NUM_SLOTS; i++) {
      int index = 0;
      // while (core2busy == true) {
      //   // Serial.println("waiting for core2 to finish");
      // }
      // core1busy = true;s
      // nodeFile = FatFS.open("nodeFileSlot" + String(i) + ".txt", "w");
      openFileThreadSafe(w, i);
      nodeFile.print("{} ");
      if (i >= 0) {
        // nodeFile.print("{ 83-103, 81-100, 82-110, 117-110, 85-111, 114-111,
        // 80-112, 116-112, "); nodeFile.print("{ D5-GND, A3-GND, A5-GPIO_0,
        // D4-UART_TX, D6-UART_RX, "); nodeFile.print("{ AREF-D8, D8-ADC0,
        // ADC0-GPIO_0, D11-GND, D10-ADC2, ADC2-UART_TX, D12-ADC1, ADC1-UART_RX,
        // D13-GND,  ");
        if (addRotaryConnections > 0) {
          // nodeFile.print("{AREF-GND,D11-GND,D10-UART_TX,D12-UART_RX,D13-GPIO_0,");
          // nodeFile.print(i * 4 + 1);
          // nodeFile.print("-");
          // nodeFile.print(i * 4 + 2);
          // nodeFile.print(",}");
        } else {
          // nodeFile.print("{ \n\r } \n\r");
        }
      }
      nodeFile.close();
      core1busy = false;
    }
  } else {
    // while (core2busy == true) {
    //   // Serial.println("waiting for core2 to finish");
    // }
    // core1busy = true;
    // nodeFile = FatFS.open("nodeFileSlot" + String(slot) + ".txt", "w");
    openFileThreadSafe(w, slot);

    // nodeFile.print("{ D12-D7, D7-ADC0, ADC0-UART_RX, D11-GND, D10-ADC2,
    // ADC2-UART_TX, AREF-ADC1, ADC1-GPIO_0, D13-GND,  "); if
    // (addRotaryConnections > 0)
    // {
    //     //
    //     nodeFile.print("{AREF-GND,D11-GND,D10-UART_TX,D12-UART_RX,D13-GPIO_0,}
    //     ");
    //     // nodeFile.print(slot * 4 + 1);
    //     // nodeFile.print("-");
    //     // nodeFile.print(slot * 4 + 2);
    //     // nodeFile.print(", } ");
    // }

    nodeFile.print("{} ");

    nodeFile.close();
    core1busy = false;
  }
}

void createConfigFile(int overwrite) {

  if (overwrite == 0) {
    if (FatFS.exists("config.txt")) {
      return;
    }
  }
  while (core2busy == true) {
    // Serial.println("waiting for core2 to finish");
  }
  core1busy = true;

  File configFile = FatFS.open("config.txt", "w");
  configFile.print("#Jumperless Config file\n\r");
  configFile.println("version: 5");
  configFile.print("revision: ");
  configFile.println(EEPROM.read(REVISIONADDRESS));

  configFile.close();
}


int checkIfBridgeExists(int node1, int node2, int slot, int flashOrLocal) {

  if (flashOrLocal == 0) {

    openFileThreadSafe(rplus, slot);

    if (!nodeFile) {
      if (debugFP) {
        Serial.println("Failed to open nodeFile (removeBridgeFromNodeFile)");
      }

      return -1;
    } else {
      if (debugFP)
        Serial.println(
            "\n\ropened nodeFile.txt\n\n\rloading bridges from file\n\r");
    }

    if (nodeFile.size() < 2) {
      // Serial.println("empty file");
      nodeFile.close();
      core1busy = false;
      return -1;
    }
    nodeFile.seek(0);
    nodeFile.setTimeout(8);

    //nodeFile.readStringUntil
  }

  











return 0;

}

void inputNodeFileList(int addRotaryConnections) {
  // addRotaryConnections = 1;
  // Serial.println("Paste the nodeFile list here\n\n\r");

  unsigned long humanTime = millis();

  int shown = 0;
  while (Serial.available() == 0) {
    if (millis() - humanTime == 400 && shown == 0) {
      Serial.println("Paste the nodeFile list here\n\n\r");
      shown = 1;
    }
  }
  nodeFileString.clear();
  // if (addRotaryConnections > 0)
  // {
  //     for (int i = 0; i < 59 ; i++)
  //     {
  //     nodeFileString.write(rotaryConnectionString[i]);
  //     Serial.print(rotaryConnectionString[i]);
  //     }
  // }
  int startInsertion = 0;
  // while (Serial.available() > 0) {
  //   uint8_t c = Serial.read();
  //   // if (c == '{' && addRotaryConnections > 0)
  //   // {
  //   //     // startInsertion = 1;
  //   //     for (int i = 0; i < 53; i++)
  //   //     {
  //   //         nodeFileString.write(rotaryConnectionString[i]);
  //   //         // Serial.print(rotaryConnectionString[i]);
  //   //     }
  //   //     continue;
  //   // }
  //   nodeFileString.write(c);
  //   delayMicroseconds(90);
  // }
  nodeFileString.read(Serial);

  // Serial.println("\n\r\n\rnodeFileString");

  int lastTokenIndex = 0;
  int tokenFound = 0;
  uint8_t lastReads[8] = {' ', ' ', ' ', ' '};
  uint8_t slotText[8] = {'S', 'l', 'o', 't', ' '};
  uint8_t searchFor[3] = {'f', ' ', '{'};
  while (core2busy == true) {
    //   // Serial.println("waiting for core2 to finish");
  }
  core1busy = true;
  nodeFileBuffer = FatFS.open("nodeFileBuffer.txt", "w+");
  // openFileThreadSafe(wplus, slot);
  nodeFileString.trim();
  if (nodeFileString.endsWith("}") == -1) {
    nodeFileString.concat(" } \n\r");
  }
  int openBraceIdx = nodeFileString.indexOf("{");

  if (openBraceIdx == -1) {
    Serial.println(
        "No opening curly braces found,\n\rhere is the correct format:");
    Serial.println("\n\n\rSlot [slot number] \n\n\rf "
                   "{ \n\r[node]-[node],\n\r[node]-[node],\n\r}\n\n\r");
    core1busy = false;
    return;
  }

  // Serial.println(nodeFileString);
  nodeFileString.printTo(nodeFileBuffer);

  int index = 0;

  int inFileMeat = 0;

  int numberOfSlotsFound = 0;
  int firstSlotNumber = 0;
  int firstSlotFound = 0;

  // nodeFileBuffer.seek(nodeFileBuffer.size());
  // nodeFileBuffer.print("fuck             \n\r");
  nodeFileBuffer.seek(0);
  // Serial.println(" \n\n\n\r");
  //  while (nodeFileBuffer.available())
  //  {
  //  Serial.write(nodeFileBuffer.read());
  //  }

  for (int i = 0; i < NUM_SLOTS;
       i++) // this just searches for how many slots are in the pasted list
  {
    tokenFound = 0;
    nodeFileBuffer.seek(index);
    inFileMeat = 0;

    while (nodeFileBuffer.available()) {
      uint8_t c = nodeFileBuffer.read();
      lastReads[0] = lastReads[1];
      lastReads[1] = lastReads[2];
      lastReads[2] = lastReads[3];
      lastReads[3] = lastReads[4];
      lastReads[4] = lastReads[5];
      lastReads[5] = c;

      if (lastReads[0] == slotText[0] && lastReads[1] == slotText[1] &&
          lastReads[2] == slotText[2] && lastReads[3] == slotText[3] &&
          lastReads[4] == slotText[4] && firstSlotFound == 0) {

        firstSlotFound = 1;
        firstSlotNumber = lastReads[5] - '0';

        // break;
      }

      if (lastReads[3] == searchFor[0] && lastReads[4] == searchFor[1] &&
          lastReads[5] == searchFor[2]) {
        inFileMeat = 1;
        numberOfSlotsFound++;
      }
      if (lastReads[2] == '}') {
        inFileMeat = 0;

        index++;
        break;
      }

      if (inFileMeat == 1) {

        // Serial.println(numberOfSlotsFound);
      }
      index++;
    }
  }

  index = 0;
  int lastSlotNumber = firstSlotNumber + numberOfSlotsFound - 1;

  for (int i = firstSlotNumber; i <= lastSlotNumber;
       i++) // this takes the pasted list fron the serial monitor and saves it
            // to the nodeFileSlot files
  {

    if (i >= firstSlotNumber && i <= lastSlotNumber) {
      // Serial.println(i);
      while (core2busy == true) {
        // Serial.println("waiting for core2 to finish");
      }
      core1busy = true;

      nodeFile = FatFS.open("nodeFileSlot" + String(i) + ".txt", "w");
    }

    // nodeFileStringSlot.clear();
    nodeFileBuffer.seek(index);

    inFileMeat = 0;

    while (nodeFileBuffer.available()) {
      uint8_t c = nodeFileBuffer.read();
      lastReads[0] = lastReads[1];
      lastReads[1] = lastReads[2];
      lastReads[2] = c;

      // nodeFile.write(c);

      if (lastReads[0] == searchFor[0] && lastReads[1] == searchFor[1] &&
          lastReads[2] == searchFor[2]) {
        inFileMeat = 1;
      }
      if (lastReads[1] == '}') {
        inFileMeat = 0;
        break;
      }

      if (inFileMeat == 1 && i >= firstSlotNumber && i <= lastSlotNumber) {
        nodeFile.write(c);
        // Serial.print(index);
      }
      index++;
    }
    if (i >= firstSlotNumber && i <= lastSlotNumber) {
      nodeFile.seek(0);
      nodeFile.close();
    }

    //refreshSavedColors(i);
    core1busy = false;
  }
  // uint8_t trash = Serial.read();
  // Serial.write(trash);
  // delay(10);

  // while(Serial.available() > 0)
  // {
  //     trash = Serial.read();
  //     //Serial.write(trash);
  //     delay(1);
  // }

  // for (int i = 0; i < NUM_SLOTS; i++)
  // {
  //     // printNodeFile(i);
  // }
}

void saveCurrentSlotToSlot(int slotFrom, int slotTo, int flashOrLocalfrom,
                           int flashOrLocalTo) {
  // while (core2busy == true) {
  //   // Serial.println("waiting for core2 to finish");
  // }
  // core1busy = true;
  openFileThreadSafe(r, slotFrom);
  // nodeFile = FatFS.open("nodeFileSlot" + String(slotFrom) + ".txt", "r");
  nodeFileString.clear();
  nodeFileString.read(nodeFile);
  nodeFile.close();

  // nodeFile = FatFS.open("nodeFileSlot" + String(slotTo) + ".txt", "w");
  openFileThreadSafe(w, slotTo);
  nodeFileString.printTo(nodeFile);
  nodeFile.close();
  core1busy = false;
}

void savePreformattedNodeFile(int source, int slot, int keepEncoder) {
  // while (core2busy == true) {
  //   // Serial.println("waiting for core2 to finish");
  // }
  // core1busy = true;
  // nodeFile = FatFS.open("nodeFileSlot" + String(slot) + ".txt", "w");
  openFileThreadSafe(w, slot);
  // Serial.println("Slot " + String(slot));

  // Serial.println(nodeFile);

  if (keepEncoder == 1) {
    // nodeFile.print("{ D12-D7, D7-ADC0, ADC0-UART_RX, D11-GND, D10-ADC2,
    // ADC2-UART_TX, AREF-ADC1, ADC1-GPIO_0, D13-GND,  ");
    // nodeFile.print("{AREF-GND,D11-GND,D10-UART_TX,D12-UART_RX,D13-GPIO_0, ");
    // Serial.println(" keeping encoder");
  }

  if (debugFP) {
    Serial.print("source = ");
    Serial.println(source);
  }

  if (source == 0) {
    int charCount = 0;
    while (Serial.available() == 0 || Serial.peek() == 'f') {
    }
    nodeFile.print("{");
    while (Serial.available() > 0) {
      // nodeFile.write(Serial.read());
      uint8_t c = Serial.read();
       
      if (c != 'f' && c != '}' && c != '{' && c != ' ' && c != '\n' &&
          c != '\r' && c != '\t') {
           if (charCount == 0 && c == '-') {
             continue;
             
           }
        nodeFile.write(c);
        charCount++;
        Serial.write(c);
      }

      delayMicroseconds(10);
    }
  }
  if (source == 1) {
    nodeFile.print("f 117-D1, 116-D0,");
    while (Serial1.available() == 0) {
    }
    delayMicroseconds(90);
    // Serial.println("waiting for Arduino to send file");
    while (Serial1.available() > 0) {
      nodeFile.write(Serial1.read());
      delayMicroseconds(10);
      // Serial.println(Serial1.available());
    }

    while (Serial1.available() > 0) {
      Serial1.read();
      delay(1);
    }
  }

  // nodeFile.write("\n\r");

  // nodeFile.seek(0);
  //  nodeFileString.read(nodeFile);
  // Serial.println(nodeFileString);

  nodeFile.print(" , } ");
  // Serial.println(" keeping encoder");

  nodeFile.close();
  core1busy = false;
  // Serial.println("Slot " + String(slot) + " saved");
  // printNodeFile(slot);
}

int getSlotLength(int slot, int flashOrLocal) {
  int slotLength = 0;
  if (flashOrLocal == 0) {
    while (core2busy == true) {
      // Serial.println("waiting for core2 to finish");
    }
    core1busy = true;
    nodeFile = FatFS.open("nodeFileSlot" + String(slot) + ".txt", "r");
    while (nodeFile.available()) {
      nodeFile.read();
      slotLength++;
    }
    nodeFile.close();
    core1busy = false;
  } else {
    slotLength = nodeFileString.length();
  }

  return slotLength;
}

void printNodeFile(int slot, int printOrString, int flashOrLocal) {

  if (flashOrLocal == 0) {
    while (core2busy == true) {
      // Serial.println("waiting for core2 to finish");
    }
    core1busy = true;

    nodeFile = FatFS.open("nodeFileSlot" + String(slot) + ".txt", "r");
    if (!nodeFile) {
      // if (debugFP)
      // Serial.println("Failed to open nodeFile");
      core1busy = false;
      return;
    } else {
      // if (debugFP)
      // Serial.println("\n\ropened nodeFile.txt\n\n\rloading bridges from
      // file\n\r");
    }
    specialFunctionsString.clear();

    specialFunctionsString.read(nodeFile);
    nodeFile.close();
    core1busy = false;
  } else {
    specialFunctionsString.clear();
    nodeFileString.printTo(specialFunctionsString);
    //specialFunctionsString.read(nodeFileString);
  }

  //     int newLines = 0;
  // Serial.println(specialFunctionsString.indexOf(","));
  // Serial.println(specialFunctionsString.charAt(specialFunctionsString.indexOf(",")+1));
  // Serial.println(specialFunctionsString.indexOf(","));
  if (debugFP == 0) {

    // specialFunctionsString.replace("116-80, 117-82, 114-83, 85-100, 81-100,",
    // "rotEnc_0,");

    specialFunctionsString.replace("100", "GND");
    specialFunctionsString.replace("101", "TOP_RAIL");
    specialFunctionsString.replace("102", "BOTTOM_RAIL");
    specialFunctionsString.replace("105", "5V");
    specialFunctionsString.replace("103", "3V3");
    specialFunctionsString.replace("106", "DAC0");
    specialFunctionsString.replace("107", "DAC1");
    specialFunctionsString.replace("108", "I_P");
    specialFunctionsString.replace("109", "I_N");
    specialFunctionsString.replace("110", "ADC0");
    specialFunctionsString.replace("111", "ADC1");
    specialFunctionsString.replace("112", "ADC2");
    specialFunctionsString.replace("113", "ADC3");
    specialFunctionsString.replace("114", "GPIO_0");
    specialFunctionsString.replace("116", "UART_TX");
    specialFunctionsString.replace("117", "UART_RX");
    specialFunctionsString.replace("118", "GPIO_18");
    specialFunctionsString.replace("119", "GPIO_19");
    specialFunctionsString.replace("120", "8V_P");
    specialFunctionsString.replace("121", "8V_N");
    specialFunctionsString.replace("70", "D0");
    specialFunctionsString.replace("71", "D1");
    specialFunctionsString.replace("72", "D2");
    specialFunctionsString.replace("73", "D3");
    specialFunctionsString.replace("74", "D4");
    specialFunctionsString.replace("75", "D5");
    specialFunctionsString.replace("76", "D6");
    specialFunctionsString.replace("77", "D7");
    specialFunctionsString.replace("78", "D8");
    specialFunctionsString.replace("79", "D9");
    specialFunctionsString.replace("80", "D10");
    specialFunctionsString.replace("81", "D11");
    specialFunctionsString.replace("82", "D12");
    specialFunctionsString.replace("83", "D13");
    specialFunctionsString.replace("84", "RESET");
    specialFunctionsString.replace("85", "AREF");
    specialFunctionsString.replace("86", "A0");
    specialFunctionsString.replace("87", "A1");
    specialFunctionsString.replace("88", "A2");
    specialFunctionsString.replace("89", "A3");
    specialFunctionsString.replace("90", "A4");
    specialFunctionsString.replace("91", "A5");
    specialFunctionsString.replace("92", "A6");
    specialFunctionsString.replace("93", "A7");
    specialFunctionsString.replace("94", "RESET_0");
    specialFunctionsString.replace("95", "RESET_1");
    specialFunctionsString.replace("96", "GND_1");
    specialFunctionsString.replace("97", "GND_0");
    specialFunctionsString.replace("98", "3V3");
    specialFunctionsString.replace("99", "5V");
    specialFunctionsString.replace("128", "LOGO_PAD_TOP");
    specialFunctionsString.replace("129", "LOGO_PAD_BOTTOM");
    specialFunctionsString.replace("130", "GPIO_PAD");
    specialFunctionsString.replace("131", "DAC_PAD");
    specialFunctionsString.replace("132", "ADC_PAD");
    specialFunctionsString.replace("133", "BUILDING_PAD_TOP");
    specialFunctionsString.replace("134", "BUILDING_PAD_BOTTOM");
    specialFunctionsString.replace("126", "BOTTOM_RAIL_GND");
    specialFunctionsString.replace("104", "TOP_RAIL_GND");
    specialFunctionsString.replace("122", "GPIO_5");
    specialFunctionsString.replace("123", "GPIO_6");
    specialFunctionsString.replace("124", "GPIO_7");
    specialFunctionsString.replace("125", "GPIO_8");
    specialFunctionsString.replace("135", "GPIO_1");
    specialFunctionsString.replace("136", "GPIO_2");
    specialFunctionsString.replace("137", "GPIO_3");
    specialFunctionsString.replace("138", "GPIO_4");
    specialFunctionsString.replace("139", "BUFFER_IN");
    specialFunctionsString.replace("140", "BUFFER_OUT");


  }

      if (specialFunctionsString.charAt(specialFunctionsString.indexOf(",") + 1) != '\n') {
        specialFunctionsString.replace(" ", "");
      specialFunctionsString.replace(",", ",\n\r");
      //specialFunctionsString.replace("{ ", "{\n\r");
      specialFunctionsString.replace("{", "{\n\r");
    }
    
  // int specialFunctionsStringLength = specialFunctionsString.indexOf("}");
  // if (specialFunctionsStringLength != -1)
  // {
  // //specialFunctionsString.remove(specialFunctionsStringLength + 1, -1);
  // }

  // if (specialFunctionsString.indexOf(",\n\r") == -1)
  // {
  //     specialFunctionsString.replace(",", ",\n\r");
  //     specialFunctionsString.replace("{", "{\n\r");
  // }

  // nodeFile.close();

  int specialFunctionsStringLength = specialFunctionsString.indexOf("}");
  if (specialFunctionsStringLength != -1) {
    if (specialFunctionsString.charAt(specialFunctionsStringLength + 1) != '\n') {
      specialFunctionsString.replace("}", "}\n\r");
    }
    specialFunctionsString.remove(specialFunctionsStringLength + 2, -1);
  }

  // specialFunctionsString.readUntilToken(specialFunctionsString, "{");
  // specialFunctionsString.removeLast(9);
  // Serial.print("*");
  if (printOrString == 0) {
    Serial.println(specialFunctionsString);
    //     Serial.println('*');
    // specialFunctionsString.clear();
  }
}

void parseWokwiFileToNodeFile(void) {

  // delay(3000);
  FatFS.begin();
  timeToFP = millis();

  wokwiFile = FatFS.open("wokwi.txt", "w+");
  if (!wokwiFile) {
    if (debugFP)
      Serial.println("Failed to open wokwi.txt");
    return;
  } else {
    if (debugFP) {
      Serial.println("\n\ropened wokwi.txt\n\r");
    } else {
      // Serial.println("\n\r");
    }
  }

  Serial.println("paste Wokwi diagram.json here\n\r");
  while (Serial.available() == 0) {
  }

  int numCharsRead = 0;

  char firstChar = Serial.read();

  if (firstChar != '{') // in case you just paste a wokwi file in from the menu,
                        // the opening brace will have already been read
  {
    inputBuffer[numCharsRead] = '{';
    numCharsRead++;
  } else {
    inputBuffer[numCharsRead] = firstChar;
    numCharsRead++;
  }
  /*
      Serial.println(firstChar);
    Serial.println(firstChar);
      Serial.println(firstChar);
     Serial.println(firstChar);
     Serial.println(firstChar);
     Serial.print(firstChar);
  */
  delay(1);
  while (Serial.available() > 0) {
    char c = Serial.read();
    inputBuffer[numCharsRead] = c;

    numCharsRead++;

    delayMicroseconds(1000);
  }

  createSafeStringFromCharArray(wokwiFileString, inputBuffer);
  delay(3);
  wokwiFile.write(inputBuffer, numCharsRead);

  delay(3);

  wokwiFile.seek(0);

  if (debugFP)
    Serial.println("\n\n\rwokwiFile\n\n\r");

  /* for (int i = 0; i < numCharsRead; i++)
   {
       Serial.print((char)wokwiFile.read());
   }*/
  if (debugFP) {
    Serial.print(wokwiFileString);

    Serial.println("\n\n\rnumCharsRead = ");

    Serial.print(numCharsRead);

    Serial.println("\n\n\r");
  }
  wokwiFile.close();

  deserializeJson(wokwiJson, inputBuffer);

  if (debugFP) {

    Serial.println("\n\n\rwokwiJson\n\n\r");

    Serial.println("\n\n\rconnectionsW\n\n\r");
  }

  numConnsJson = wokwiJson["connections"].size();

  copyArray(wokwiJson["connections"], connectionsW);

  // deserializeJson(connectionsW, Serial);
  if (debugFP) {
    Serial.println(wokwiJson["connections"].size());

    for (int i = 0; i < MAX_BRIDGES; i++) {
      // Serial.println(wokwiJson["connections"].size());
      if (connectionsW[i][0] == "") {
        break;
      }
      Serial.print(connectionsW[i][0]);
      Serial.print(",   \t ");

      Serial.print(connectionsW[i][1]);
      Serial.print(",   \t ");

      Serial.print(connectionsW[i][2]);
      Serial.print(",   \t ");

      Serial.print(connectionsW[i][3]);
      Serial.print(",   \t ");

      Serial.print(connectionsW[i][4]);
      Serial.print(",   \t ");

      Serial.println();
    }

    Serial.println("\n\n\rRedefining\n\n\r");
  }

  changeWokwiDefinesToJumperless();

  writeToNodeFile();
  // while(1);
  openNodeFile();
}

void changeWokwiDefinesToJumperless(void) {

  String connString1 = "                               ";
  String connString2 = "                               ";
  String connStringColor = "                               ";
  String bb = "bb1:";

  int nodeNumber;

  for (int i = 0; i < numConnsJson; i++) {
    if (debugFP) {
      Serial.println(' ');
    }
    for (int j = 0; j < 2; j++) {
      nodeNumber = -1;
      connString1 = connectionsW[i][j];
      if (debugFP) {
        Serial.print(connString1);
        Serial.print("   \t\t  ");
      }
      if (connString1.startsWith("bb1:") || connString1.startsWith("bb2:")) {
        // Serial.print("bb1 or bb2  ");

        int periodIndex = connString1.indexOf(".");
        connString1 = connString1.substring(4, periodIndex);

        if (connString1.endsWith("b")) {
          nodeNumber = 30;
          // Serial.println("bottom");
          connString1.substring(0, connString1.length() - 1);
          nodeNumber += connString1.toInt();
        } else if (connString1.endsWith("t")) {
          nodeNumber = 0;
          // Serial.println("top");
          connString1.substring(0, connString1.length() - 1);
          nodeNumber += connString1.toInt();
        } else if (connString1.endsWith("n")) {
          nodeNumber = GND;
        } else if (connString1.endsWith("p")) {
          nodeNumber = SUPPLY_5V;
        }
      } else if (connString1.startsWith("nano:")) {
        // Serial.print("nano\t");
        int periodIndex = connString1.indexOf(".");
        connString1 = connString1.substring(5, periodIndex);

        nodeNumber = NANO_D0;

        if (isDigit(connString1[connString1.length() - 1])) {

          nodeNumber += connString1.toInt();
        } else if (connString1.equals("5V")) {
          nodeNumber = SUPPLY_5V;
        } else if (connString1.equalsIgnoreCase("AREF")) {

          nodeNumber = NANO_AREF;
        } else if (connString1.equalsIgnoreCase("GND")) {
          nodeNumber = GND;
        } else if (connString1.equalsIgnoreCase("RESET")) {

          nodeNumber = NANO_RESET;
        } else if (connString1.equalsIgnoreCase("3.3V")) {
          nodeNumber = SUPPLY_3V3;
        } else if (connString1.startsWith("A")) {
          nodeNumber = NANO_A0;
          nodeNumber += connString1.toInt();
        }
      } else if (connString1.startsWith("vcc1:")) {
        // Serial.print("vcc1\t");
        nodeNumber = SUPPLY_5V;
      } else if (connString1.startsWith("vcc2:")) {
        // Serial.print("vcc2\t");
        nodeNumber = SUPPLY_3V3;
      } else if (connString1.startsWith("gnd1:")) {
        // Serial.print("gnd1\t");
        nodeNumber = GND;
      } else if (connString1.startsWith("gnd2:")) {
        // Serial.print("gnd2\t");
        nodeNumber = GND;
      } else if (connString1.startsWith("gnd3:")) {
        nodeNumber = GND;
      } else if (connString1.startsWith("pot1:")) {
        nodeNumber = DAC0;
      } else {

        connectionsW[i][j] = -1;
      }

      // nodeNumber += connString1.toInt();

      connectionsW[i][j] = nodeNumber;
      if (debugFP) {
        Serial.print(connectionsW[i][j]);

        Serial.print("   \t ");
      }
    }
  }
}
void clearNodeFile(int slot, int flashOrLocal) {
  if (flashOrLocal == 0) {
    openFileThreadSafe(w, slot);
    // nodeFile = FatFS.open("nodeFileSlot" + String(slot) + ".txt", "w");
    nodeFile.print(" { } ");
    nodeFile.close();
  } else {
    nodeFileString.clear();
  }
}
String slicedLines[130];
int slicedLinesIndex = 0;

int removeBridgeFromNodeFile(int node1, int node2, int slot, int flashOrLocal, int onlyCheck) {
  unsigned long timerStart = millis();
  unsigned long timerEnd[5] = {0, 0, 0, 0, 0};

  // Serial.print("Slot = ");
  // Serial.println(slot);
  if (flashOrLocal == 0) {

    openFileThreadSafe(rplus, slot);

    if (!nodeFile) {
      if (debugFP) {
        Serial.println("Failed to open nodeFile (removeBridgeFromNodeFile)");
      }

      return -1;
    } else {
      if (debugFP)
        Serial.println(
            "\n\ropened nodeFile.txt\n\n\rloading bridges from file\n\r");
    }

    if (nodeFile.size() < 2) {
      // Serial.println("empty file");
      nodeFile.close();
      core1busy = false;
      return -1;
    }
    nodeFile.seek(0);
    nodeFile.setTimeout(8);
  }
if (onlyCheck == 1) {
  Serial.print("Checking for bridge between ");
  Serial.print(node1);
  Serial.print(" and ");
  Serial.println(node2);
}
  timerEnd[0] = millis() - timerStart;

  for (int i = 0; i < 120; i++) {
    slicedLines[i] = " ";
  }
  slicedLinesIndex = 0;
  int numberOfLines = 0;
  // nodeFileString.clear();
  String lineBufString = "";
  // nodeFileString.printTo(Serial);
  // Serial.println(" ");
  // Serial.print("nodeFileString = ");
  // Serial.println(nodeFileString);
  // core1busy = true;
  createSafeString(lineBufSafe, 40);
  int lineIdx = 0;
  int charIdx = 0;
  for (int i = 0; i < 120; i++) {
    slicedLines[lineIdx] = ' ';

    if (flashOrLocal == 0) {
      lineBufString = nodeFile.readStringUntil(',');

    } else {
      charIdx = nodeFileString.stoken(lineBufSafe, charIdx, ",");
      lineBufString = lineBufSafe.c_str();
      if (charIdx == -1) {
        numberOfLines = lineIdx;
        // Serial.print ("\n\r\t\t\t\tt\t\tnumberOfLines = ");
        // Serial.println(numberOfLines);
        //  Serial.println("end of file char idx");

        break;
      }
    }
    // Serial.print("lineBufSafe = ");
    // Serial.println(lineBufSafe);
    // Serial.print("lineBufString = ");
    // Serial.println(lineBufString);

    lineBufString.trim();
    lineBufString.replace(" ", "");
    // Serial.print("$");
    // Serial.print(lineBufString);
    // Serial.println("$");

    if (lineBufString.length() < 3 || lineBufString == " ") {
      numberOfLines = lineIdx;
      // Serial.print ("numberOfLines = ");
      // Serial.println(numberOfLines);
      //  Serial.println("end of file");

      break;
    }
    slicedLines[lineIdx].concat(lineBufString);

    // Serial.print("#");
    // Serial.print(slicedLines[lineIdx]);
    // Serial.println("#");
    slicedLines[lineIdx].replace("\n", "");
    slicedLines[lineIdx].replace("\r", "");

    slicedLines[lineIdx].replace("{", "");
    slicedLines[lineIdx].replace("}", "");
    slicedLines[lineIdx].replace(",", "");
    // slicedLines[lineIdx].trim();
    slicedLines[lineIdx].replace("-", " - ");
    slicedLines[lineIdx].concat(" , ");
    // Serial.print("*");
    // Serial.print(slicedLines[lineIdx]);
    // Serial.println("*");

    lineIdx++;
  }
  timerEnd[1] = millis() - timerStart;
  numberOfLines = lineIdx;
  // Serial.print("numberOfLines = ");
  // Serial.println(numberOfLines);
  // Serial.print("nodeFileString = ");
  // Serial.println(nodeFileString);
  // nodeFileString.clear();
  // nodeFile.close();
  // Serial.println(nodeFileString);
  // Serial.print("lineIdx = ");
  // Serial.println(lineIdx);
  char nodeAsChar[40];
  itoa(node1, nodeAsChar, 10);
  String paddedNode1 = " ";
  String paddedNode2 = " ";

  paddedNode1.concat(nodeAsChar);
  paddedNode1.concat(" ");
  // Serial.print("paddedNode1 = *");
  // Serial.print(paddedNode1);
  // Serial.println("*");

  if (node2 != -1) {
    itoa(node2, nodeAsChar, 10);

    paddedNode2.concat(nodeAsChar);
    paddedNode2.concat(" ");
    // Serial.print("paddedNode2 = *");
    // Serial.print(paddedNode2);
    // Serial.println("*");
  }

  // nodeFile.truncate(0);

  timerEnd[2] = millis() - timerStart;

  if (flashOrLocal == 0) {
    nodeFile.close();
    openFileThreadSafe(w, slot);
    // nodeFile = FatFS.open("nodeFileSlot" + String(slot) + ".txt", "w");
    nodeFile.print("{");
  } else {
    nodeFileString.clear();
    nodeFileString.concat("{ ");
  }
  // nodeFile.print(" { \n\r");
  // Serial.print("numberOfLines = ");
  // Serial.println(numberOfLines);
  int removedLines = 0;

  for (int i = 0; i < numberOfLines; i++) {
    // Serial.print("\n\rslicedLines[");

    // Serial.print(i);
    // Serial.print("] = ");
    // Serial.println(slicedLines[i]);
    // delay(10);
    // Serial.println(millis()-timerStart);
    int remove = 0;

    if (node2 == -1 && slicedLines[i].indexOf(paddedNode1) != -1) {

      if (onlyCheck == 0) {
        remove = 1;
      //slicedLines[i] = "Removed Node";
      }
      // Serial.print("Removed Node ");
      // Serial.println(paddedNode1);
      
      removedLines++;

    } else if (slicedLines[i].indexOf(paddedNode1) != -1 &&
               slicedLines[i].indexOf(paddedNode2) != -1) {

      if (onlyCheck == 0) {
        remove = 1;
     // slicedLines[i] = "Removed Bridge";
      }
      // Serial.print("Removed Bridge ");
      // Serial.print(paddedNode1);
      // Serial.print(" - ");
      // Serial.println(paddedNode2);

      removedLines++;
    } 
    if (remove == 0) {
      remove = 0;
      slicedLines[i].replace(" ", "");

      if (flashOrLocal == 0) {
        nodeFile.print(slicedLines[i]);
      } else {
        // nodeFileString.concat(slicedLines[i]);

        for (int j = 0; j < 39; j++) {

          nodeAsChar[j] = ' ';
        }
        // Serial.print("nodeAsChar1 = ");
        // Serial.println(nodeAsChar);

        slicedLines[i].toCharArray(nodeAsChar, 40);
        // Serial.print("nodeAsChar2 = *");
        // Serial.print(nodeAsChar);
        // Serial.println("*");
        slicedLines[i].replace(",", "");
        slicedLines[i].replace(" ", "");
        slicedLines[i].concat(",");
        nodeFileString.concat(nodeAsChar);
        nodeFileString.replace(" ", "");
        //     Serial.print("sliceLines[i].length() = ");
        //     Serial.println(slicedLines[i].length());
        //         Serial.print("nodeFileString = ");
        // Serial.println(nodeFileString);
        nodeFileString.replace("{ ", "");
        nodeFileString.replace(" } ", "");
        nodeFileString.replace("{", "");
        nodeFileString.replace("}", "");
        nodeFileString.prefix("{ ");
        // nodeFileString.concat(" } ");
        //  Serial.print("nodeFileString = ");
        //  Serial.println(nodeFileString);
      }
    }
    //     Serial.print("slicedLines[");
    //       Serial.print(i);
    //       Serial.print("] = ");
    //  Serial.println(slicedLines[i]);
  }

  if (flashOrLocal == 0) {
    nodeFile.print(" } ");
    nodeFile.close();
  } else {
    nodeFileString.concat(" } ");
    // Serial.print("nodeFileString = ");
    // Serial.println(nodeFileString);
  }

  // for (int i = 0; i <= numberOfLines; i++) {
  //   Serial.print("\n\rslicedLines[");
  //   Serial.print(i);
  //   Serial.print("] = ");
  //   Serial.println(slicedLines[i]);

  // }
  core1busy = false;

  timerEnd[3] = millis() - timerStart;
  // Serial.print("timerEnd[0] = ");

  //   Serial.println(timerEnd[0]);
  //   Serial.print("timerEnd[1] = ");
  //   Serial.println(timerEnd[1]);
  //   Serial.print("timerEnd[3] = ");
  //   Serial.println(timerEnd[3]);
  return removedLines;
}

int addBridgeToNodeFile(int node1, int node2, int slot, int flashOrLocal,
                        int allowDuplicates) {


    // Serial.print("nodeFileStringAdd = ");
    // Serial.println(nodeFileString);
unsigned long timerStart[5];
timerStart[0] = micros();
  if (flashOrLocal == 0) {
    // nodeFile = FatFS.open("nodeFileSlot" + String(slot) + ".txt", "r+");

    openFileThreadSafe(rplus, slot);
    // Serial.println(nodeFile);
    // Serial.print("Slot = ");
    // Serial.println(slot);
    if (!nodeFile) {
      // if (debugFP) {
      Serial.println("Failed to open nodeFile (addBridgeToNodeFile)");
      //  }
      // reateSlots(slot, 0);
      //  delay(10);
      //  nodeFile = FatFS.open("nodeFileSlot" + String(slot) + ".txt",
      //  "w+"); nodeFile.print("{ ");
      openFileThreadSafe(w, slot);
      nodeFile.print("{ } ");
      // return;
    } else {
      if (debugFP)
        Serial.println(
            "\n\ropened nodeFile.txt\n\n\rloading bridges from file\n\r");
    }
    //     while (nodeFile.available()) {
    //     Serial.write(nodeFile.read());
    //   }
    //   nodeFile.seek(0);
    nodeFile.setTimeout(15);
  }

  // Serial.print("flashOrLocal = ");
  // Serial.println(flashOrLocal);

  for (int i = 0; i < 120; i++) {
    slicedLines[i] = " ";
  }
  timerStart[1] = micros();

  int numberOfLines = 0;
  // Serial.print("nodeFileString = ");
  // Serial.println(nodeFileString);
  // nodeFileString.printTo(Serial);
  // Serial.println(" ");
  String lineBufString = "";

  createSafeString(lineBufSafe, 30);
  int lineIdx = 0;
  int charIdx = 0;
  nodeFileString.trim();

  for (int i = 0; i < 120; i++) {

    slicedLines[lineIdx] = ' ';
    // if (i == 0 && nodeFileString.startsWith("{") == -1 && flashOrLocal == 1)
    // {
    //   slicedLines[0].concat("{");

    // }

    if (flashOrLocal == 0) {
      lineBufString = nodeFile.readStringUntil(',');

    } else {
      charIdx = nodeFileString.stoken(lineBufSafe, charIdx, ",");
      // if (charIdx == -1 || lineBufSafe == "}") {
      //   numberOfLines = lineIdx;
      //   break;
      // }
      lineBufString = lineBufSafe.c_str();
    }

    // Serial.print("lineBufSafe = ");
    // Serial.println(lineBufSafe);
    // Serial.print("lineBufString = ");
    // Serial.println(lineBufString);

    lineBufString.trim();
    lineBufString.replace(" ", "");

    if (lineBufString.length() < 3 || lineBufString == " ") {
      // Serial.println("end of file");
      numberOfLines = lineIdx;
      break;
    }
    slicedLines[lineIdx].concat(lineBufString);

    slicedLines[lineIdx].replace("\n", "");
    slicedLines[lineIdx].replace("\r", "");

    slicedLines[lineIdx].replace("{", "");
    slicedLines[lineIdx].replace("}", "");
    slicedLines[lineIdx].replace(",", "");
    slicedLines[lineIdx].concat(",");

    // Serial.print("lineBufString = ");
    // Serial.println(lineBufString);
    // slicedLines[lineIdx].trim();

    // Serial.print("*");
    // Serial.print(slicedLines[lineIdx]);
    // Serial.println("*");

    lineIdx++;
  }
  timerStart[2] = micros();
  numberOfLines = lineIdx;

  // Serial.print("\n\rnumberOfLines = "); 
  // Serial.println(numberOfLines);

  // nodeFileString.clear();
  // nodeFile.close();
  // Serial.println(nodeFileString);

  char nodeAsChar[40];
  itoa(node1, nodeAsChar, 10);
  String addNode1 = "";
  String addNode2 = "";

  addNode1.concat(nodeAsChar);
  addNode1.concat("-");

  itoa(node2, nodeAsChar, 10);

  addNode2.concat(nodeAsChar);
  addNode2.concat(",");

  addNode1.concat(addNode2);

  addNode1.toCharArray(nodeAsChar, addNode1.length() + 1);
  nodeAsChar[addNode1.length()] = '\0';

  //createSafeString(addNodeSafe, 40);

  int duplicateFound = 0;

  if (flashOrLocal == 0) {
    // Serial.println("flash");
    openFileThreadSafe(wplus, slot);
    // nodeFile = FatFS.open("nodeFileSlot" + String(slot) + ".txt", "w+");
    nodeFile.print("{ ");

    for (int i = 0; i < numberOfLines; i++) {

      if (slicedLines[i].indexOf(addNode1) != -1) {
        duplicateFound = 1;
        // Serial.println("Duplicate found (flash)");
      }

      nodeFile.print(slicedLines[i]);
    }

    if (duplicateFound == 0 || allowDuplicates == 1) {
      nodeFile.print(addNode1);
    }
    nodeFile.print(" } ");
    //nodeFile.seek(0);

    nodeFile.close();


  } else {
    //  Serial.println("local");
    // Serial.print("nodeFileString1 = ");
    // Serial.println(nodeFileString);
    // Serial.print("addNode1 = ");
    // Serial.println(addNode1);




    if (nodeFileString.indexOf(nodeAsChar, 0) != -1) {
      duplicateFound = 1;
      //Serial.println("Duplicate found (local)");
    }


    nodeFileString.replace('\n', "");
    nodeFileString.replace('\r', "");
    nodeFileString.replace(' ', "");
    nodeFileString.replace('{', "");
    nodeFileString.replace('}', "");

// Serial.print("\n\n\rduplicateFound = ");
//     Serial.println(duplicateFound);
//     Serial.print("nodeAsChar = ");
//     Serial.println(nodeAsChar);

        if (duplicateFound == 0 || allowDuplicates == 1) {
      // nodeFileString.concat(addNode1);
      nodeFileString.concat(nodeAsChar, addNode1.length());
    }




    nodeFileString.prefix("{ ");
    nodeFileString.concat(" } ");


    //     Serial.print("nodeFileStringAdd = ");
    // Serial.println(nodeFileString);
  }
timerStart[3] = micros();

// for (int i = 0; i < 4; i++) {
//   Serial.print("timerStart[");
//   Serial.print(i);
//   Serial.print("] = ");
//   Serial.println(timerStart[i] - timerStart[0]);
// }
  return duplicateFound;
}

void writeToNodeFile(int slot, int flashOrLocal) {

  /// core1busy = true;

  // nodeFileString.printTo(Serial);
  // nodeFile = FatFS.open("nodeFile" + String(slot) + ".txt", "w");
  openFileThreadSafe(w, slot);

  if (!nodeFile) {
    if (debugFP)
      Serial.println("Failed to open nodeFile");
    return;
  } else {
    if (debugFP)
      Serial.println("\n\rrecreated nodeFile.txt\n\n\rloading bridges from "
                     "wokwi.txt\n\r");
  }
  nodeFile.print("{\n\r");
  for (int i = 0; i < numConnsJson; i++) {
    if (connectionsW[i][0] == "-1" && connectionsW[i][1] != "-1") {
      // lightUpNode(connectionsW[i][0].toInt());
      continue;
    }
    if (connectionsW[i][1] == "-1" && connectionsW[i][0] != "-1") {
      // lightUpNode(connectionsW[i][1].toInt());
      continue;
    }
    if (connectionsW[i][0] == connectionsW[i][1]) {
      // lightUpNode(connectionsW[i][0].toInt());
      continue;
    }

    nodeFile.print(connectionsW[i][0]);
    nodeFile.print("-");
    nodeFile.print(connectionsW[i][1]);
    nodeFile.print(",\n\r");
  }
  nodeFile.print("\n\r}\n\r");

  if (debugFP) {
    Serial.println("wrote to nodeFile.txt");

    Serial.println("nodeFile.txt contents:");
    nodeFile.seek(0);

    while (nodeFile.available()) {
      Serial.write(nodeFile.read());
    }
    Serial.println("\n\r");
  }
  nodeFile.close();
  core1busy = false;
}

void openNodeFile(int slot, int flashOrLocal) {
  timeToFP = millis();
  netsUpdated = false;
  // Serial.println(nodeFileString);
  // Serial.println("opening nodeFileSlot" + String(slot) + ".txt");
  //Serial.println("flashOrLocal = " + String(flashOrLocal));

  //Serial.println(flashOrLocal?"local":"flash");

  // while (core2busy == true) {
  //   // Serial.println("waiting for core2 to finish");
  // }
  // core1busy = true;
  if (flashOrLocal == 0) {
    // multicore_lockout_start_blocking();
    // Serial.println("opening nodeFileSlot" + String(slot) + ".txt");

    // nodeFile = FatFS.open("nodeFileSlot" + String(slot) + ".txt", "r");
    openFileThreadSafe(r, slot);
    if (!nodeFile) {
      if (debugFP)
        Serial.println("Failed to open nodeFile");

      // createSlots(slot, 0);
      openFileThreadSafe(w, slot);
      nodeFile.print("{ } ");
      // core1busy = false;
      // return;
    } else {
      if (debugFP)
        Serial.println("\n\ropened nodeFileSlot" + String(slot) +
                       +".txt\n\n\rloading bridges from file\n\r");
    }

    nodeFileString.clear();
    nodeFileString.read(nodeFile);
    // delay(10);
    // Serial.println(nodeFileString);

    nodeFile.close();

    // multicore_lockout_end_blocking();
  }
  // Serial.println(nodeFileString);
  // Serial.println();
  //   Serial.println("nodeFileString = ");
  // nodeFileString.printTo(Serial);

  splitStringToFields();
  core1busy = false;
  // parseStringToBridges();
}

void splitStringToFields() {
  int openBraceIndex = 0;
  int closeBraceIndex = 0;

  if (debugFP)
    Serial.println("\n\rraw input file\n\r");
  if (debugFP)
    Serial.println(nodeFileString);
  if (debugFP)
    Serial.println("\n\rsplitting and cleaning up string\n\r");
  if (debugFP)
    Serial.println("_");

  openBraceIndex = nodeFileString.indexOf("{");
  closeBraceIndex = nodeFileString.indexOf("}");
  int fIndex = nodeFileString.indexOf("f");

  int foundOpenBrace = -1;
  int foundCloseBrace = -1;
  int foundF = -1;

  if (openBraceIndex != -1) {
    foundOpenBrace = 1;
  }
  if (closeBraceIndex != -1) {
    foundCloseBrace = 1;
  }
  if (fIndex != -1) {
    foundF = 1;
  }

  // Serial.println(openBraceIndex);
  // Serial.println(closeBraceIndex);
  // Serial.println(fIndex);

  if (foundF == 1) {
    nodeFileString.substring(nodeFileString, fIndex + 1,
                             nodeFileString.length());
  }

  if (foundOpenBrace == 1 && foundCloseBrace == 1) {

    nodeFileString.substring(specialFunctionsString, openBraceIndex + 1,
                             closeBraceIndex);
  } else {
    nodeFileString.substring(specialFunctionsString, 0,
                             -1); // nodeFileString.length());
  }
  specialFunctionsString.trim();

  if (debugFP)
    Serial.println(specialFunctionsString);

  if (debugFP)
    Serial.println("^\n\r");
  /*
      nodeFileString.remove(0, closeBraceIndex + 1);
      nodeFileString.trim();

      openBraceIndex = nodeFileString.indexOf("{");
      closeBraceIndex = nodeFileString.indexOf("}");
      //nodeFileString.substring(specialFunctionsString, openBraceIndex + 1,
     closeBraceIndex); specialFunctionsString.trim();
      if(debugFP)Serial.println("_");
      if(debugFP)Serial.println(specialFunctionsString);
      if(debugFP)Serial.println("^\n\r");
      */
  replaceSFNamesWithDefinedInts();
  replaceNanoNamesWithDefinedInts();
  parseStringToBridges();
}

void replaceSFNamesWithDefinedInts(void) {
  specialFunctionsString.toUpperCase();
  if (debugFP) {
    Serial.println("replacing special function names with defined ints\n\r");
    Serial.println(specialFunctionsString);
  }

  specialFunctionsString.replace("GND", "100");
  specialFunctionsString.replace("GROUND", "100");
  specialFunctionsString.replace("TOP_RAIL", "101");
  specialFunctionsString.replace("TOPRAIL", "101");
  specialFunctionsString.replace("BOTTOM_RAIL", "102");
  specialFunctionsString.replace("BOT_RAIL", "102");
  specialFunctionsString.replace("BOTTOMRAIL", "102");
  specialFunctionsString.replace("BOTRAIL", "102");
  specialFunctionsString.replace("SUPPLY_5V", "105");
  specialFunctionsString.replace("SUPPLY_3V3", "103");

  specialFunctionsString.replace("DAC0_5V", "106");
  specialFunctionsString.replace("DAC1_8V", "107");
  specialFunctionsString.replace("DAC0", "106");
  specialFunctionsString.replace("DAC1", "107");

  specialFunctionsString.replace("INA_N", "109");
  specialFunctionsString.replace("INA_P", "108");
  specialFunctionsString.replace("I_N", "109");
  specialFunctionsString.replace("I_P", "108");
  specialFunctionsString.replace("CURRENT_SENSE_MINUS", "109");
  specialFunctionsString.replace("CURRENT_SENSE_PLUS", "108");
  specialFunctionsString.replace("ISENSE_MINUS", "109");
  specialFunctionsString.replace("ISENSE_PLUS", "108");

  specialFunctionsString.replace("EMPTY_NET", "127");

  specialFunctionsString.replace("ADC0_8V", "110");
  specialFunctionsString.replace("ADC1_8V", "111");
  specialFunctionsString.replace("ADC2_8V", "112");
  specialFunctionsString.replace("ADC3_8V", "113");
  specialFunctionsString.replace("ADC4_5V", "114");
  specialFunctionsString.replace("ADC7_PROBE", "115");
  specialFunctionsString.replace("ADC0", "110");
  specialFunctionsString.replace("ADC1", "111");
  specialFunctionsString.replace("ADC2", "112");
  specialFunctionsString.replace("ADC3", "113");
  specialFunctionsString.replace("ADC4", "114");
  specialFunctionsString.replace("ADC7", "115");

  specialFunctionsString.replace("GPIO_1", "138");
  specialFunctionsString.replace("GPIO_2", "139");
  specialFunctionsString.replace("GPIO_3", "140");
  specialFunctionsString.replace("GPIO_4", "141");

  specialFunctionsString.replace("GPIO1", "138");
  specialFunctionsString.replace("GPIO2", "139");
  specialFunctionsString.replace("GPIO3", "140");
  specialFunctionsString.replace("GPIO4", "141");

  specialFunctionsString.replace("GPIO_5", "122");
  specialFunctionsString.replace("GPIO_6", "123");
  specialFunctionsString.replace("GPIO_7", "124");
  specialFunctionsString.replace("GPIO_8", "125");

  specialFunctionsString.replace("GPIO5", "122");
  specialFunctionsString.replace("GPIO6", "123");
  specialFunctionsString.replace("GPIO7", "124");
  specialFunctionsString.replace("GPIO8", "125");

  specialFunctionsString.replace("+5V", "105");
  specialFunctionsString.replace("5V", "105");
  specialFunctionsString.replace("3.3V", "103");
  specialFunctionsString.replace("3V3", "103");

  specialFunctionsString.replace("RP_UART_TX", "116");
  specialFunctionsString.replace("RP_UART_RX", "117");

  specialFunctionsString.replace("UART_TX", "116");
  specialFunctionsString.replace("UART_RX", "117");
}

void replaceNanoNamesWithDefinedInts(
    void) // for dome reason Arduino's String wasn't replacing like 1 or 2 of
          // the names, so I'm using SafeString now and it works
{
  if (debugFP)
    Serial.println("replacing special function names with defined ints\n\r");

  char nanoName[10];
  for (int i = 0; i < 10; i++) {
    nanoName[i] = ' ';
  }

  itoa(NANO_D10, nanoName, 10);
  specialFunctionsString.replace("D10", nanoName);
  for (int i = 0; i < 10; i++) {
    nanoName[i] = ' ';
  }

  itoa(NANO_D11, nanoName, 10);
  specialFunctionsString.replace("D11", nanoName);
  for (int i = 0; i < 10; i++) {
    nanoName[i] = ' ';
  }
  itoa(NANO_D12, nanoName, 10);
  specialFunctionsString.replace("D12", nanoName);
  for (int i = 0; i < 10; i++) {
    nanoName[i] = ' ';
  }
  itoa(NANO_D13, nanoName, 10);
  specialFunctionsString.replace("D13", nanoName);
  for (int i = 0; i < 10; i++) {
    nanoName[i] = ' ';
  }
  itoa(NANO_D0, nanoName, 10);
  specialFunctionsString.replace("D0", nanoName);
  for (int i = 0; i < 10; i++) {
    nanoName[i] = ' ';
  }
  itoa(NANO_D1, nanoName, 10);
  specialFunctionsString.replace("D1", nanoName);
  for (int i = 0; i < 10; i++) {
    nanoName[i] = ' ';
  }
  itoa(NANO_D2, nanoName, 10);
  specialFunctionsString.replace("D2", nanoName);
  for (int i = 0; i < 10; i++) {
    nanoName[i] = ' ';
  }
  itoa(NANO_D3, nanoName, 10);
  specialFunctionsString.replace("D3", nanoName);
  for (int i = 0; i < 10; i++) {
    nanoName[i] = ' ';
  }
  itoa(NANO_D4, nanoName, 10);
  specialFunctionsString.replace("D4", nanoName);
  for (int i = 0; i < 10; i++) {
    nanoName[i] = ' ';
  }
  itoa(NANO_D5, nanoName, 10);
  specialFunctionsString.replace("D5", nanoName);
  for (int i = 0; i < 10; i++) {
    nanoName[i] = ' ';
  }
  itoa(NANO_D6, nanoName, 10);
  specialFunctionsString.replace("D6", nanoName);
  for (int i = 0; i < 10; i++) {
    nanoName[i] = ' ';
  }
  itoa(NANO_D7, nanoName, 10);
  specialFunctionsString.replace("D7", nanoName);
  for (int i = 0; i < 10; i++) {
    nanoName[i] = ' ';
  }
  itoa(NANO_D8, nanoName, 10);
  specialFunctionsString.replace("D8", nanoName);
  for (int i = 0; i < 10; i++) {
    nanoName[i] = ' ';
  }
  itoa(NANO_D9, nanoName, 10);
  specialFunctionsString.replace("D9", nanoName);
  for (int i = 0; i < 10; i++) {
    nanoName[i] = ' ';
  }
  itoa(NANO_RESET, nanoName, 10);
  specialFunctionsString.replace("RESET", nanoName);
  for (int i = 0; i < 10; i++) {
    nanoName[i] = ' ';
  }
  itoa(NANO_AREF, nanoName, 10);
  specialFunctionsString.replace("AREF", nanoName);
  for (int i = 0; i < 10; i++) {
    nanoName[i] = ' ';
  }
  itoa(NANO_A0, nanoName, 10);
  specialFunctionsString.replace("A0", nanoName);
  for (int i = 0; i < 10; i++) {
    nanoName[i] = ' ';
  }
  itoa(NANO_A1, nanoName, 10);
  specialFunctionsString.replace("A1", nanoName);
  for (int i = 0; i < 10; i++) {
    nanoName[i] = ' ';
  }
  itoa(NANO_A2, nanoName, 10);
  specialFunctionsString.replace("A2", nanoName);
  for (int i = 0; i < 10; i++) {
    nanoName[i] = ' ';
  }
  itoa(NANO_A3, nanoName, 10);
  specialFunctionsString.replace("A3", nanoName);
  for (int i = 0; i < 10; i++) {
    nanoName[i] = ' ';
  }
  itoa(NANO_A4, nanoName, 10);
  specialFunctionsString.replace("A4", nanoName);
  for (int i = 0; i < 10; i++) {
    nanoName[i] = ' ';
  }
  itoa(NANO_A5, nanoName, 10);
  specialFunctionsString.replace("A5", nanoName);
  for (int i = 0; i < 10; i++) {
    nanoName[i] = ' ';
  }
  itoa(NANO_A6, nanoName, 10);
  specialFunctionsString.replace("A6", nanoName);
  for (int i = 0; i < 10; i++) {
    nanoName[i] = ' ';
  }
  itoa(NANO_A7, nanoName, 10);
  specialFunctionsString.replace("A7", nanoName);
  for (int i = 0; i < 10; i++) {
    nanoName[i] = ' ';
  }
  // if(debugFP)Serial.println(bridgeString);
  if (debugFP)
    Serial.println(specialFunctionsString);
  if (debugFP)
    Serial.println("\n\n\r");
}

void parseStringToBridges(void) {

  // int bridgeStringLength = bridgeString.length() - 1;

  int specialFunctionsStringLength = specialFunctionsString.length() - 1;

  int readLength = 0;

  newBridgeLength = 0;
  newBridgeIndex = 0;

  if (debugFP) {
    Serial.println("parsing bridges into array\n\r");
  }
  int stringIndex = 0;
  char delimitersCh[] = "[,- \n\r";

  createSafeString(buffer, 10);
  createSafeStringFromCharArray(delimiters, delimitersCh);
  int doneReading = 0;

  for (int i = 0; i <= specialFunctionsStringLength; i++) {

    stringIndex =
        specialFunctionsString.stoken(buffer, stringIndex, delimiters);
    if (stringIndex == -1) {
      break;
    }

    // Serial.print("buffer = ");
    // Serial.println(buffer);

    // Serial.print("stringIndex = ");
    // Serial.println(stringIndex);

    buffer.toInt(path[newBridgeIndex].node1);

    // Serial.print("path[newBridgeIndex].node1 = ");
    // Serial.println(path[newBridgeIndex].node1);

    if (debugFP) {
      Serial.print("node1 = ");
      Serial.println(path[newBridgeIndex].node1);
    }

    stringIndex =
        specialFunctionsString.stoken(buffer, stringIndex, delimiters);

    buffer.toInt(path[newBridgeIndex].node2);

    if (debugFP) {
      Serial.print("node2 = ");
      Serial.println(path[newBridgeIndex].node2);
    }

    readLength = stringIndex;

    if (readLength == -1) {
      doneReading = 1;
      break;
    }
    newBridgeLength++;
    newBridgeIndex++;

    if (debugFP) {
      Serial.print("readLength = ");
      Serial.println(readLength);
      Serial.print("specialFunctionsString.length() = ");
      Serial.println(specialFunctionsString.length());
    }

    if (debugFP)
      Serial.print(newBridgeIndex);
    if (debugFP)
      Serial.print("-");
    if (debugFP)
      Serial.println(newBridge[newBridgeIndex][1]);
  }

  newBridgeIndex = 0;
  if (debugFP)
    for (int i = 0; i < newBridgeLength; i++) {
      Serial.print("[");
      Serial.print(path[newBridgeIndex].node1);
      Serial.print("-");
      Serial.print(path[newBridgeIndex].node2);
      Serial.print("],");
      newBridgeIndex++;
    }
  if (debugFP)
    Serial.print("\n\rbridge pairs = ");
  if (debugFP)
    Serial.println(newBridgeLength);

  // nodeFileString.clear();

  // if(debugFP)Serial.println(nodeFileString);
  timeToFP = millis() - timeToFP;
  if (debugFPtime)
    Serial.print("\n\rtook ");

  if (debugFPtime)
    Serial.print(timeToFP);
  if (debugFPtime)
    Serial.println("ms to open and parse file\n\r");

  // printBridgeArray();

  // printNodeFile();
}

int lenHelper(int x) {
  if (x >= 1000000000)
    return 10;
  if (x >= 100000000)
    return 9;
  if (x >= 10000000)
    return 8;
  if (x >= 1000000)
    return 7;
  if (x >= 100000)
    return 6;
  if (x >= 10000)
    return 5;
  if (x >= 1000)
    return 4;
  if (x >= 100)
    return 3;
  if (x >= 10)
    return 2;
  return 1;
}

int printLen(int x) { return x < 0 ? lenHelper(-x) + 1 : lenHelper(x); }
