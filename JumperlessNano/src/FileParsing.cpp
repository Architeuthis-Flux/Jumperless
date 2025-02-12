#include "FileParsing.h"
#include "ArduinoJson.h"
#include "JumperlessDefinesRP2040.h"
#include "LEDs.h"
#include "LittleFS.h"
#include "MachineCommands.h"
#include "MatrixStateRP2040.h"
#include "NetManager.h"
#include "Probing.h"
#include "RotaryEncoder.h"
#include "SafeString.h"
#include <Arduino.h>
#include <EEPROM.h>

bool debugFP = EEPROM.read(DEBUG_FILEPARSINGADDRESS);
bool debugFPtime = EEPROM.read(TIME_FILEPARSINGADDRESS);

createSafeString(nodeFileString, 3000);

int numConnsJson = 0;
createSafeString(specialFunctionsString, 3000);

char inputBuffer[INPUTBUFFERLENGTH] = { 0 };

ArduinoJson::StaticJsonDocument<8000> wokwiJson;

String connectionsW[MAX_BRIDGES][5];

File nodeFile;

File wokwiFile;

File nodeFileBuffer;

unsigned long timeToFP = 0;

const char rotaryConnectionString[] =
"{AREF-GND,D11-GND,D10-UART_TX,D12-UART_RX,D13-GPIO_0, ";





void createSlots(int slot, int addRotaryConnections) {
  /// delay(2000);
  if (slot == -1) {
    for (int i = 0; i < NUM_SLOTS; i++) {
      int index = 0;
      // int length = 0;
      // nodeFile = LittleFS.open("nodeFileSlot" + String(i) + ".txt", "r+");
      // //nodeFile.seek(0);
      // if (addRotaryConnections == 0)
      // {
      //     while(nodeFile.available())
      //     {
      //         if (rotaryConnectionString[index] ==  nodeFile.read())
      //         {
      //             //nodeFile.write(' ');
      //         index++;

      //         } else
      //         length++;
      //     }
      // }
      // nodeFile.seek(0);
      // nodeFile.write("f { ");
      // for (int i = 0; i < (index-length); i++)
      // {
      //     nodeFile.seek(index+i);
      //     uint8_t c = nodeFile.read();
      //     nodeFile.seek(i);
      //     nodeFile.print('t');
      // }
      // Serial.println(index);

      // Serial.println(length);
      // nodeFile.seek(0);
      // while(nodeFile.available())
      // {
      //     Serial.write(nodeFile.read());
      // }

      // nodeFile.close();
      nodeFile = LittleFS.open("nodeFileSlot" + String(i) + ".txt", "r+");

      // nodeFileString.clear();
      // while (nodeFile.available())
      // {
      //     nodeFileString.read(nodeFile);
      //     //nodeFileString.write(nodeFile.read());
      // }
      // nodeFileString.printTo(Serial);
      nodeFile.seek(0);
      if (nodeFile.size() <= 5) {
        nodeFile = LittleFS.open("nodeFileSlot" + String(i) + ".txt", "w+");

        if (i >= 0) {
          // nodeFile.print("{ 83-103, 81-100, 82-110, 117-110, 85-111, 114-111,
          // 80-112, 116-112, "); nodeFile.print("{ D5-GND, A3-GND, A5-GPIO_0,
          // D4-UART_TX, D6-UART_RX, "); nodeFile.print("{ AREF-D8, D8-ADC0,
          // ADC0-GPIO_0, D11-GND, D10-ADC2, ADC2-UART_TX, D12-ADC1,
          // ADC1-UART_RX, D13-GND,  ");
          if (addRotaryConnections > 0) {
            nodeFile.print(
                "{AREF-GND,D11-GND,D10-UART_TX,D12-UART_RX,D13-GPIO_0,");
            nodeFile.print(i * 4 + 1);
            nodeFile.print("-");
            nodeFile.print(i * 4 + 2);
            nodeFile.print(",}");
            } else {
            nodeFile.print("{ \n\r } \n\r");
            }
          }
        }
      // Serial.println(nodeFile.size());
      nodeFile.close();
      }
    } else {
    nodeFile = LittleFS.open("nodeFileSlot" + String(slot) + ".txt", "w");

    // nodeFile.print("{ D12-D7, D7-ADC0, ADC0-UART_RX, D11-GND, D10-ADC2,
    // ADC2-UART_TX, AREF-ADC1, ADC1-GPIO_0, D13-GND,  ");
    if (addRotaryConnections > 0) {
      nodeFile.print("{AREF-GND,D11-GND,D10-UART_TX,D12-UART_RX,D13-GPIO_0,} ");
      // nodeFile.print(slot * 4 + 1);
      // nodeFile.print("-");
      // nodeFile.print(slot * 4 + 2);
      // nodeFile.print(", } ");
      }
    nodeFile.close();
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
      nodeFile = LittleFS.open("nodeFileSlot" + String(slot) + ".txt", "w");
      break;
    case 1:
      nodeFile = LittleFS.open("nodeFileSlot" + String(slot) + ".txt", "w+");
      break;
    case 2:
      nodeFile = LittleFS.open("nodeFileSlot" + String(slot) + ".txt", "r");
      break;
    case 3:

      nodeFile = LittleFS.open("nodeFileSlot" + String(slot) + ".txt", "r+");
      break;
    case 4:
      nodeFile = LittleFS.open("nodeFileSlot" + String(slot) + ".txt", "a");
      break;
    case 5:

      nodeFile = LittleFS.open("nodeFileSlot" + String(slot) + ".txt", "a+");
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

int rawConn[4]; //connect/clear, chip, x, y
int parseRaw(int connectOrClear) {
  // Serial.println("Parsing raw");
  unsigned long humanTime = millis();

  int shown = 0;

  int connectClear = connectOrClear;
  int chip = -1;
  int x = -1;
  int y = -1;


  while (Serial.available() == 0) {
    if (millis() - humanTime == 300 && shown == 0) {
      Serial.print("\th/H [clear/CONNECT], chip [A-L/0-11], X [0-15], Y [0-7]\n\n\r");
      Serial.print("\t\tExample: H,d,13,5\n\r");
      shown = 1;
      //break; 
      }
    if (millis() - humanTime == 2000 && shown == 1) {
      return 0;
      }
    }

  // nodeFileString.clear();

  // nodeFileString.readUntil(Serial,',');
  // nodeFileString.trim();  
  // Serial.print("\n\rconnectClear: ");
  // nodeFileString.printTo(Serial);

  // if (nodeFileString == "h" || nodeFileString == "H") {
  //   connectClear = 0;
  // } else {
  //   connectClear = 0;
  // }
  nodeFileString.clear();
  nodeFileString.readUntil(Serial, ',');
  nodeFileString.clear();
  nodeFileString.readUntil(Serial, ',');
  nodeFileString.trim();
  nodeFileString.replace(",", "");

  Serial.print("\n\rchip: ");
  nodeFileString.printTo(Serial);
  //Serial.print("  ->  ");

  nodeFileString.replace("\n", "");
  nodeFileString.replace("\r", "");
  nodeFileString.replace("a", "0");
  nodeFileString.replace("b", "1");
  nodeFileString.replace("c", "2");
  nodeFileString.replace("d", "3");
  nodeFileString.replace("e", "4");
  nodeFileString.replace("f", "5");
  nodeFileString.replace("g", "6");
  nodeFileString.replace("h", "7");
  nodeFileString.replace("i", "8");
  nodeFileString.replace("j", "9");
  nodeFileString.replace("k", "10");
  nodeFileString.replace("l", "11");

  nodeFileString.replace("A", "0");
  nodeFileString.replace("B", "1");
  nodeFileString.replace("C", "2");
  nodeFileString.replace("D", "3");
  nodeFileString.replace("E", "4");
  nodeFileString.replace("F", "5");
  nodeFileString.replace("G", "6");
  nodeFileString.replace("H", "7");
  nodeFileString.replace("I", "8");
  nodeFileString.replace("J", "9");
  nodeFileString.replace("K", "10");
  nodeFileString.replace("L", "11");
  nodeFileString.replace(",", "");

  //nodeFileString.printTo(Serial);

  nodeFileString.toInt(chip);
  nodeFileString.clear();

  nodeFileString.readUntil(Serial, ',');
  nodeFileString.replace(",", "");
  nodeFileString.trim();
  Serial.print("\n\rX: ");
  nodeFileString.printTo(Serial);

  nodeFileString.toInt(x);
  nodeFileString.clear();
  nodeFileString.readUntil(Serial, ',');
  nodeFileString.replace(",", "");
  nodeFileString.trim();
  Serial.print("\n\rY: ");
  nodeFileString.printTo(Serial);

  nodeFileString.toInt(y);

  Serial.println();
  // Serial.println(chip);
  // Serial.println(x);
  // Serial.println(y);



  if (chip < 0 || chip > 11 || x < 0 || x > 15 || y < 0 || y > 7)
    {
    Serial.println("Invalid input");
    return 0;
    }

  rawConn[0] = connectClear;
  rawConn[1] = chip;
  rawConn[2] = x;
  rawConn[3] = y;

  return 1;







  }



volatile int abortWait = 0;

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
  while (Serial.available() > 0) {
    uint8_t c = Serial.read();
    if (c == '{' && addRotaryConnections > 0) {
      // startInsertion = 1;
      for (int i = 0; i < 53; i++) {
        nodeFileString.write(rotaryConnectionString[i]);
        // Serial.print(rotaryConnectionString[i]);
        }
      continue;
      }
    nodeFileString.write(c);
    delayMicroseconds(100);
    }
  // nodeFileString.read(Serial);
  //  Serial.println("nodeFileString");
  // Serial.println(nodeFileString);
  int lastTokenIndex = 0;
  int tokenFound = 0;
  uint8_t lastReads[8] = { ' ', ' ', ' ', ' ' };
  uint8_t slotText[8] = { 'S', 'l', 'o', 't', ' ' };
  uint8_t searchFor[3] = { 'f', ' ', '{' };

  createSafeString(nodeFileStringSlot, 1200);
  nodeFileBuffer = LittleFS.open("nodeFileBuffer.txt", "w+");
  nodeFileString.trim();
  if (nodeFileString.endsWith("}") == 0) {
    nodeFileString.concat(" }   \n\r");
    }

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
      nodeFile = LittleFS.open("nodeFileSlot" + String(i) + ".txt", "w");
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

void savePreformattedNodeFile(int source, int slot, int keepEncoder, int ack) {

  nodeFile = LittleFS.open("nodeFileSlot" + String(slot) + ".txt", "w");
  // Serial.println("Slot " + String(slot));

  // Serial.println(nodeFile);

  if (keepEncoder == 1) {
    // nodeFile.print("{ D12-D7, D7-ADC0, ADC0-UART_RX, D11-GND, D10-ADC2,
    // ADC2-UART_TX, AREF-ADC1, ADC1-GPIO_0, D13-GND,  ");
    nodeFile.print("{AREF-GND,D11-GND,D10-UART_TX,D12-UART_RX,D13-GPIO_0,  ");
    // Serial.println(" keeping encoder");
    }

  if (debugFP) {
    Serial.print("source = ");
    Serial.println(source);
    }

  if (source == 0) {
    while (Serial.available() == 0) {// || Serial.peek() == 'f') {
      }

    while (Serial.available() > 0) {
      // nodeFile.write(Serial.read());
      uint8_t c = Serial.read();
      // Serial.print(c);
      if (c != 'f' && c != '}' && c != '{' && c != ' ' && c != '\n' &&
          c != '\r' && c != '\t') {
        nodeFile.write(c);
        }

      delayMicroseconds(800);
      }
    }
  if (source == 1) {
    //nodeFile.print("f 117-71, 116-70,");

    //    nodeFileString.clear();
    //     nodeFileString.read(nodeFile);

    //  Serial.println(nodeFileString);

    while (Serial1.available() == 0) {
      }
    //delayMicroseconds(5000);
    // Serial.println("waiting for Arduino to send file");

    nodeFileString.clear();

    unsigned long timer = millis();

    int readCount = 0;

    char buffer[2000];
    unsigned long timeoutmer = millis();
    unsigned long timeoutTime = 300;

    int countTimeoutMisses = 0;
    int eotFound = 1;
    //Serial1.setTimeout(1000);

    char delim[] = { ',',  '{', '}', '\n', '\r' };
    createSafeStringFromCharArray(delimiters, delim);

    createSafeString(tokens, 100);
    //createSafeStringFromCharArray(delimiters, 10);

   // while (millis() - timeoutmer < 5000) {
      // while (Serial1.available() == 0) {
      // if (Serial1.available() > 0) {
    bool tok = false;

    if (ack == 1&&false) {
      eotFound = 0;
      Serial1.write('\x01');
      // Serial.println("SOH sent");
      tokens.clear();
      while (eotFound == 0)
        {

        while (Serial1.available() == 0) {
          if (millis() - timeoutmer > timeoutTime || abortWait == 1) {
            eotFound = -1;

            //Serial.println("EOT not found");
            break;

            }
          }
        //nodeFileString.readUntilToken(Serial1, tokens, delimiters, tok);
      //tokens.read(Serial1);

        tokens.readUntil(Serial1, delimiters);

        tokens.printTo(nodeFileString);
        //tokens.printTo(Serial);
        Serial1.write('\x06');

        //Serial.println();
        if (tokens.indexOf('\x04') != -1) {
          eotFound = 1;
          //Serial.println("EOT found");
          tokens.clear();
          break;
          }
        tokens.clear();

        if (millis() - timeoutmer > timeoutTime || abortWait == 1) {
          eotFound = -1;
          //Serial.println("EOT not found");
          break;

          }

        }
      } else {
      eotFound = 0;
      //Serial1.write('\x01');
     // Serial.println("SOH sent");
      int readCount = 0;
      tokens.clear();
      Serial1.write('\x01');
      while (eotFound == 0)
        {

        while (Serial1.available() == 0) {
          if (millis() - timeoutmer > timeoutTime * 5 || abortWait == 1) {
            eotFound = -1;

            //Serial.println("EOT not found");
            break;

            }
          }
        //nodeFileString.readUntilToken(Serial1, tokens, delimiters, tok);
      //tokens.read(Serial1);
        // if (Serial1.peek() == 'f' && readCount > 4) {
        //   eotFound = -1;
        //   //Serial.println("EOT found");
        //   nodeFileString.printTo(Serial);
        //   tokens.clear();
        //   break;
        //   }
        tokens.readUntil(Serial1, delimiters);
        readCount++;
        if (tokens.indexOf('f') != -1 && readCount > 4) {
          eotFound = 1;
          tokens.remove(tokens.indexOf('f'));
          //Serial.println("EOT found");
          //tokens.clear();
          //break;
          }

        tokens.printTo(nodeFileString);

        Serial1.write('\x06');
        // if (nodeFileString.lastIndexOf('f') > 2) {
        //   nodeFileString.printTo(Serial);

        //   eotFound = 1;
        //   //Serial.println("EOT found");
        //   tokens.clear();
        //   break;
        //   }
        //tokens.printTo(Serial);
        //Serial1.write('\x06');

        //Serial.println();
        if (tokens.indexOf('\n') != -1 || tokens.indexOf('\x04') != -1) {
          eotFound = 1;

          tokens.clear();
          break;
          }
        tokens.clear();

        if (millis() - timeoutmer > timeoutTime * 5 || abortWait == 1) {
          eotFound = -1;
          //Serial.println("EOT not found");
          break;

          }

        }
      //nodeFileString.printTo(Serial);
      }

    if (eotFound != 1) {
      //Serial.println("EOT not found");

      }
    nodeFileString.replace("\x01", "");
    nodeFileString.replace("\x06", "");
    nodeFileString.replace("\x02", "");
    nodeFileString.replace("\x1c", "");
    nodeFileString.replace("\n", "");
    nodeFileString.replace(" ", "");
    nodeFileString.replace("\n", "");
    nodeFileString.replace("\r", "");
    nodeFileString.replace("\t", "");
    nodeFileString.replace("{", "");
    nodeFileString.replace("}", "");
    nodeFileString.replace("f", "");

    nodeFileString.prefix("f 117-71,116-70,");
    //nodeFileString.concat(" \n\r");
    nodeFile.seek(0);

    // nodeFileString.printTo(Serial);
    // Serial.println();

// nodeFile.write("f 117-71,116-70, ");

    nodeFileString.printTo(nodeFile);
    nodeFileString.printTo(Serial);
    }

  // nodeFile.write("\n\r");

  // nodeFile.seek(0);
  //  nodeFileString.read(nodeFile);
  // Serial.println(nodeFileString);
  if (keepEncoder == 1) {
    nodeFile.print("  }  ");
    // Serial.println(" keeping encoder");
    }

  nodeFile.close();
  // Serial.println("Slot " + String(slot) + " saved");
  // printNodeFile(slot);
  abortWait = 0;
  }

void printNodeFile(int slot) {

  nodeFile = LittleFS.open("nodeFileSlot" + String(slot) + ".txt", "r");
  if (!nodeFile) {
    // if (debugFP)
    // Serial.println("Failed to open nodeFile");
    return;
    } else {
    // if (debugFP)
    // Serial.println("\n\ropened nodeFile.txt\n\n\rloading bridges from
    // file\n\r");
    }
  nodeFileString.clear();

  nodeFileString.read(nodeFile);
  nodeFile.close();
  // Serial.print(nodeFileString);

  //     int newLines = 0;
  // Serial.println(nodeFileString.indexOf(","));
  // Serial.println(nodeFileString.charAt(nodeFileString.indexOf(",")+1));
  // Serial.println(nodeFileString.indexOf(","));
  if (debugFP == 0) {
    // nodeFileString.replace("116-80, 117-82, 114-83, 85-100, 81-100,",
    // "rotEnc_0,");

    nodeFileString.replace("100", "GND");
    nodeFileString.replace("105", "5V");
    nodeFileString.replace("103", "3V3");
    nodeFileString.replace("106", "DAC0");
    nodeFileString.replace("107", "DAC1");
    nodeFileString.replace("108", "I_P");
    nodeFileString.replace("109", "I_N");
    nodeFileString.replace("110", "ADC0");
    nodeFileString.replace("111", "ADC1");
    nodeFileString.replace("112", "ADC2");
    nodeFileString.replace("113", "ADC3");
    nodeFileString.replace("114", "GPIO_0");
    nodeFileString.replace("116", "UART_TX");
    nodeFileString.replace("117", "UART_RX");
    nodeFileString.replace("118", "GPIO_18");
    nodeFileString.replace("119", "GPIO_19");
    nodeFileString.replace("120", "8V_P");
    nodeFileString.replace("121", "8V_N");
    nodeFileString.replace("70", "D0");
    nodeFileString.replace("71", "D1");
    nodeFileString.replace("72", "D2");
    nodeFileString.replace("73", "D3");
    nodeFileString.replace("74", "D4");
    nodeFileString.replace("75", "D5");
    nodeFileString.replace("76", "D6");
    nodeFileString.replace("77", "D7");
    nodeFileString.replace("78", "D8");
    nodeFileString.replace("79", "D9");
    nodeFileString.replace("80", "D10");
    nodeFileString.replace("81", "D11");
    nodeFileString.replace("82", "D12");
    nodeFileString.replace("83", "D13");
    nodeFileString.replace("84", "RESET");
    nodeFileString.replace("85", "AREF");
    nodeFileString.replace("86", "A0");
    nodeFileString.replace("87", "A1");
    nodeFileString.replace("88", "A2");
    nodeFileString.replace("89", "A3");
    nodeFileString.replace("90", "A4");
    nodeFileString.replace("91", "A5");
    nodeFileString.replace("92", "A6");
    nodeFileString.replace("93", "A7");
    }

  if (nodeFileString.charAt(nodeFileString.indexOf(",") + 1) != '\n') {
    nodeFileString.replace(",", ",\n\r");
    nodeFileString.replace("{", "{\n\r");
    }

  // int nodeFileStringLength = nodeFileString.indexOf("}");
  // if (nodeFileStringLength != -1)
  // {
  // //nodeFileString.remove(nodeFileStringLength + 1, -1);
  // }

  // if (nodeFileString.indexOf(",\n\r") == -1)
  // {
  //     nodeFileString.replace(",", ",\n\r");
  //     nodeFileString.replace("{", "{\n\r");
  // }

  // nodeFile.close();

  int nodeFileStringLength = nodeFileString.indexOf("}");
  if (nodeFileStringLength != -1) {
    if (nodeFileString.charAt(nodeFileStringLength + 1) != '\n') {
      nodeFileString.replace("}", "}\n\r");
      }
    nodeFileString.remove(nodeFileStringLength + 2, -1);
    }

  // nodeFileString.readUntilToken(nodeFileString, "{");
  // nodeFileString.removeLast(9);

  Serial.print(nodeFileString);
  // Serial.print('*');
  nodeFileString.clear();
  }

void parseWokwiFileToNodeFile(void) {

  // delay(3000);
  LittleFS.begin();
  timeToFP = millis();

  wokwiFile = LittleFS.open("wokwi.txt", "w+");
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
void clearNodeFile(int slot) {

  nodeFile = LittleFS.open("nodeFileSlot" + String(slot) + ".txt", "w+");

  nodeFile.print("!");
  nodeFile.close();
  }

void removeBridgeFromNodeFile(int node1, int node2, int slot) {

  nodeFile = LittleFS.open("nodeFileSlot" + String(slot) + ".txt", "r+");
  /// Serial.println(nodeFile);

  if (!nodeFile) {
    if (debugFP)
      Serial.println("Failed to open nodeFile");
    return;
    } else {
    if (debugFP)
      Serial.println(
          "\n\ropened nodeFile.txt\n\n\rloading bridges from file\n\r");
    }

  nodeFileString.clear();

  nodeFileString.read(nodeFile);
  nodeFile.close();
  // Serial.println(nodeFileString);
  char nodeAsChar[20];
  itoa(node1, nodeAsChar, 10);
  char paddedChar[21];

  paddedChar[0] = ' ';
  for (int i = 1; i < 20; i++) {
    if (nodeAsChar[i - 1] == '\0') {
      paddedChar[i] = ' ';
      paddedChar[i + 1] = '\0';
      break;
      }
    paddedChar[i] = nodeAsChar[i - 1];
    }

  int numberOfLines = 0;
  // Serial.print(paddedChar);
  // Serial.println("*");

  char lines[100][20];

  int lineIndex = 0;
  int charIndex = 0;

  for (int i = 0; i < 100; i++) {

    if (nodeFileString[charIndex] == '\0') {
      numberOfLines = i;
      break;
      }
    lines[i][0] = ' ';
    for (int j = 1; j < 20; j++) {
      if (nodeFileString[charIndex] == ',') {
        lines[i][j] = ' ';
        lines[i][j + 1] = ',';
        // lines[i][j + 2] = '\n';
        // lines[i][j + 3] = '\r';
        lines[i][j + 2] = '\0';

        charIndex++;
        break;
        } else if (nodeFileString[charIndex] == '-') {
          lines[i][j] = ' ';
          lines[i][j + 1] = '-';
          lines[i][j + 2] = ' ';
          j += 2;
          charIndex++;
          // break;
          } else if (nodeFileString[charIndex] == '\n' ||
                     nodeFileString[charIndex] == '\r' ||
                     nodeFileString[charIndex] == '{' ||
                     nodeFileString[charIndex] == '}') {
            lines[i][j] = ' ';
            charIndex++;
            } else {
            lines[i][j] = nodeFileString[charIndex];
            charIndex++;
            }
      }
    }

  // Serial.println("\n\r********");

  for (int i = 0; i < numberOfLines; i++) {
    if (lines[i][0] == '\0') {
      // break;
      }
    if (strstr(lines[i], paddedChar) != NULL) {
      // Serial.println(lines[i]);
      // delay(1);

      for (int j = 0; j < 18; j++) {
        lines[i][j] = ' ';
        }
      // lines[i][18] = '\n';
      // lines[i][19] = '\r';
      lines[i][0] = '\0';
      }
    }

  nodeFileString.clear();
  nodeFileString.concat("{");
  for (int i = 0; i < numberOfLines; i++) {
    if (lines[i][0] == '\0') {
      continue;
      }
    // Serial.println(i);
    // delay(1);
    for (int j = 0; j < 20; j++) {
      if (lines[i][j] == '\0') {
        break;
        }
      if (lines[i][j] == ' ') {
        continue;
        }
      nodeFileString.concat(lines[i][j]);
      // Serial.print(lines[i][j]);
      // delay(1);
      }
    }
  nodeFileString.concat("}\n\r");

  nodeFile.close();
  nodeFile = LittleFS.open("nodeFileSlot" + String(slot) + ".txt", "w+");

  nodeFile.write(nodeFileString.c_str());

  nodeFile.close();
  }

void addBridgeToNodeFile(int node1, int node2, int slot) {
  nodeFile = LittleFS.open("nodeFileSlot" + String(slot) + ".txt", "r+");
  // Serial.println(nodeFile);

  if (!nodeFile) {
    if (debugFP)
      Serial.println("Failed to open nodeFile");
    return;
    } else {
    if (debugFP)
      Serial.println(
          "\n\ropened nodeFile.txt\n\n\rloading bridges from file\n\r");
    }

  int nodeFileBraceIndex = 0;
  // nodeFileString.clear();
  //   nodeFileString.read(nodeFile);


  //   if ((node1 == RP_UART_RX || node1 == RP_UART_TX) &&
  //       (node2 == NANO_D1|| node2 == NANO_D0) ||
  //       (node2 == RP_UART_RX || node2 == RP_UART_TX) &&
  //       (node1 == NANO_D1 || node1 == NANO_D0)) {

  //         char node1Char[20];
  //         itoa(node1, node1Char, 10);
  //         char node2Char[20];
  //         itoa(node2, node2Char, 10);

  //         char bridgeString[40];
  //         strcpy(bridgeString, "f ");
  //         strcat(bridgeString, node1Char);
  //         strcat(bridgeString, "-");
  //         strcat(bridgeString, node2Char);
  //         strcat(bridgeString, ",  ");

  // nodeFileString.replace("f", "");
  // //nodeFileString.replace("", "");



  // nodeFileString.prefix(bridgeString);

  // nodeFileString.printTo(Serial);
  //     } else {
  //     }




  while (nodeFile.available()) {
    char c = nodeFile.read();
    // Serial.print(c);

    if (c == '}') {
      break;
      } else {
      nodeFileBraceIndex++;
      }

    if (c == '!') {
      nodeFile.seek(0);
      nodeFile.print("{\n\r");
      nodeFile.print(node1);
      nodeFile.print("-");
      nodeFile.print(node2);
      nodeFile.print(",\n\r}\n\r");

      // if (1)
      // {
      //     Serial.println("wrote to nodeFile.txt");

      //     Serial.println("nodeFile.txt contents:\n\r");
      //     nodeFile.seek(0);

      //     while (nodeFile.available())
      //     {
      //         Serial.write(nodeFile.read());
      //     }
      //     Serial.println("\n\r");
      // }
      // Serial.print (nodeFile);
      nodeFile.close();
      return;
      }
    }
  // Serial.println(nodeFileBraceIndex);
  nodeFile.seek(nodeFileBraceIndex);

  nodeFile.print(node1);
  nodeFile.print("-");
  nodeFile.print(node2);
  nodeFile.print(",}\n\r");

  if (debugFP) {
    Serial.println("wrote to nodeFile.txt");

    Serial.println("nodeFile.txt contents:\n\r");
    nodeFile.seek(0);

    while (nodeFile.available()) {
      Serial.write(nodeFile.read());
      }
    Serial.println("\n\r");
    }
  nodeFile.close();
  }

void writeToNodeFile(int slot) {

  nodeFile = LittleFS.open("nodeFile" + String(slot) + ".txt", "w");

  if (!nodeFile) {
    if (debugFP)
      Serial.println("Failed to open nodeFile");
    return;
    } else {
    if (debugFP)
      Serial.println(
          "\n\rrecreated nodeFile.txt\n\n\rloading bridges from wokwi.txt\n\r");
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
  }

void openNodeFile(int slot) {
  timeToFP = millis();

  nodeFile = LittleFS.open("nodeFileSlot" + String(slot) + ".txt", "r");

  if (!nodeFile) {
    if (debugFP)
      Serial.println("Failed to open nodeFile");
    return;
    } else {
    if (debugFP)
      Serial.println("\n\ropened nodeFileSlot" + String(slot) +
                     +".txt\n\n\rloading bridges from file\n\r");
    }

  nodeFileString.clear();
  nodeFileString.read(nodeFile);
  // Serial.println(nodeFileString);

  nodeFile.close();
  splitStringToFields();
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

  specialFunctionsString.replace("ADC0_5V", "110");
  specialFunctionsString.replace("ADC1_5V", "111");
  specialFunctionsString.replace("ADC2_5V", "112");
  specialFunctionsString.replace("ADC3_8V", "113");
  specialFunctionsString.replace("ADC0", "110");
  specialFunctionsString.replace("ADC1", "111");
  specialFunctionsString.replace("ADC2", "112");
  specialFunctionsString.replace("ADC3", "113");

  specialFunctionsString.replace("+5V", "105");
  specialFunctionsString.replace("5V", "105");
  specialFunctionsString.replace("3.3V", "103");
  specialFunctionsString.replace("3V3", "103");

  specialFunctionsString.replace("RP_GPIO_0", "114");
  specialFunctionsString.replace("RP_UART_TX", "116");
  specialFunctionsString.replace("RP_UART_RX", "117");
  specialFunctionsString.replace("GPIO_0", "114");
  specialFunctionsString.replace("UART_TX", "116");
  specialFunctionsString.replace("UART_RX", "117");
  }

void replaceNanoNamesWithDefinedInts(
    void) // for dome reason Arduino's String wasn't replacing like 1 or 2 of
  // the names, so I'm using SafeString now and it works
  {
  if (debugFP)
    Serial.println("replacing special function names with defined ints\n\r");

  char nanoName[5];

  itoa(NANO_D10, nanoName, 10);
  specialFunctionsString.replace("D10", nanoName);

  itoa(NANO_D11, nanoName, 10);
  specialFunctionsString.replace("D11", nanoName);

  itoa(NANO_D12, nanoName, 10);
  specialFunctionsString.replace("D12", nanoName);

  itoa(NANO_D13, nanoName, 10);
  specialFunctionsString.replace("D13", nanoName);

  itoa(NANO_D0, nanoName, 10);
  specialFunctionsString.replace("D0", nanoName);

  itoa(NANO_D1, nanoName, 10);
  specialFunctionsString.replace("D1", nanoName);

  itoa(NANO_D2, nanoName, 10);
  specialFunctionsString.replace("D2", nanoName);

  itoa(NANO_D3, nanoName, 10);
  specialFunctionsString.replace("D3", nanoName);

  itoa(NANO_D4, nanoName, 10);
  specialFunctionsString.replace("D4", nanoName);

  itoa(NANO_D5, nanoName, 10);
  specialFunctionsString.replace("D5", nanoName);

  itoa(NANO_D6, nanoName, 10);
  specialFunctionsString.replace("D6", nanoName);

  itoa(NANO_D7, nanoName, 10);
  specialFunctionsString.replace("D7", nanoName);

  itoa(NANO_D8, nanoName, 10);
  specialFunctionsString.replace("D8", nanoName);

  itoa(NANO_D9, nanoName, 10);
  specialFunctionsString.replace("D9", nanoName);

  itoa(NANO_RESET, nanoName, 10);
  specialFunctionsString.replace("RESET", nanoName);

  itoa(NANO_AREF, nanoName, 10);
  specialFunctionsString.replace("AREF", nanoName);

  itoa(NANO_A0, nanoName, 10);
  specialFunctionsString.replace("A0", nanoName);

  itoa(NANO_A1, nanoName, 10);
  specialFunctionsString.replace("A1", nanoName);

  itoa(NANO_A2, nanoName, 10);
  specialFunctionsString.replace("A2", nanoName);

  itoa(NANO_A3, nanoName, 10);
  specialFunctionsString.replace("A3", nanoName);

  itoa(NANO_A4, nanoName, 10);
  specialFunctionsString.replace("A4", nanoName);

  itoa(NANO_A5, nanoName, 10);
  specialFunctionsString.replace("A5", nanoName);

  itoa(NANO_A6, nanoName, 10);
  specialFunctionsString.replace("A6", nanoName);

  itoa(NANO_A7, nanoName, 10);
  specialFunctionsString.replace("A7", nanoName);

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

  nodeFileString.clear();

  // if(debugFP)Serial.println(nodeFileString);
  timeToFP = millis() - timeToFP;
  if (debugFPtime)
    Serial.print("\n\rtook ");

  if (debugFPtime)
    Serial.print(timeToFP);
  if (debugFPtime)
    Serial.println("ms to open and parse file\n\r");
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

void debugFlagInit(void) {

  if (EEPROM.read(FIRSTSTARTUPADDRESS) == 255) {
    EEPROM.write(FIRSTSTARTUPADDRESS, 0);
    EEPROM.write(DEBUG_FILEPARSINGADDRESS, 0);
    EEPROM.write(TIME_FILEPARSINGADDRESS, 0);
    EEPROM.write(DEBUG_NETMANAGERADDRESS, 0);
    EEPROM.write(TIME_NETMANAGERADDRESS, 0);
    EEPROM.write(DEBUG_NETTOCHIPCONNECTIONSADDRESS, 0);
    EEPROM.write(DEBUG_NETTOCHIPCONNECTIONSALTADDRESS, 0);
    EEPROM.write(DEBUG_LEDSADDRESS, 0);
    EEPROM.write(LEDBRIGHTNESSADDRESS, DEFAULTBRIGHTNESS);
    EEPROM.write(RAILBRIGHTNESSADDRESS, DEFAULTRAILBRIGHTNESS);
    EEPROM.write(SPECIALBRIGHTNESSADDRESS, DEFAULTSPECIALNETBRIGHTNESS);
    EEPROM.write(PROBESWAPADDRESS, 0);
    EEPROM.write(ROTARYENCODER_MODE_ADDRESS, 0);

    EEPROM.commit();
    delay(5);
    }

#ifdef EEPROMSTUFF
  debugFP = EEPROM.read(DEBUG_FILEPARSINGADDRESS);
  debugFPtime = EEPROM.read(TIME_FILEPARSINGADDRESS);

  debugNM = EEPROM.read(DEBUG_NETMANAGERADDRESS);
  debugNMtime = EEPROM.read(TIME_NETMANAGERADDRESS);

  debugNTCC = EEPROM.read(DEBUG_NETTOCHIPCONNECTIONSADDRESS);
  debugNTCC2 = EEPROM.read(DEBUG_NETTOCHIPCONNECTIONSALTADDRESS);

  LEDbrightnessRail = EEPROM.read(RAILBRIGHTNESSADDRESS);
  LEDbrightness = EEPROM.read(LEDBRIGHTNESSADDRESS);
  LEDbrightnessSpecial = EEPROM.read(SPECIALBRIGHTNESSADDRESS);

  debugLEDs = EEPROM.read(DEBUG_LEDSADDRESS);

  rotaryEncoderMode = EEPROM.read(ROTARYENCODER_MODE_ADDRESS);

  probeSwap = EEPROM.read(PROBESWAPADDRESS);
  loadFileOnStart = EEPROM.read(LOADFILEONSTART_ADDRESS);

#else

  debugFP = 1;
  debugFPtime = 1;

  debugNM = 1;
  debugNMtime = 1;

  debugNTCC = 1;
  debugNTCC2 = 1;

  // debugLEDs = 1;
#endif
  if (loadFileOnStart != 0 && loadFileOnStart != 1)
    EEPROM.write(LOADFILEONSTART_ADDRESS, 0);

  if (debugFP != 0 && debugFP != 1)
    EEPROM.write(DEBUG_FILEPARSINGADDRESS, 0);

  if (debugFPtime != 0 && debugFPtime != 1)
    EEPROM.write(TIME_FILEPARSINGADDRESS, 0);

  if (debugNM != 0 && debugNM != 1)
    EEPROM.write(DEBUG_NETMANAGERADDRESS, 0);

  if (debugNMtime != 0 && debugNMtime != 1)
    EEPROM.write(TIME_NETMANAGERADDRESS, 0);

  if (debugNTCC != 0 && debugNTCC != 1)
    EEPROM.write(DEBUG_NETTOCHIPCONNECTIONSADDRESS, 0);

  if (debugNTCC2 != 0 && debugNTCC2 != 1)
    EEPROM.write(DEBUG_NETTOCHIPCONNECTIONSALTADDRESS, 0);

  if (debugLEDs != 0 && debugLEDs != 1)
    EEPROM.write(DEBUG_LEDSADDRESS, 0);

  if (LEDbrightnessRail < 0 || LEDbrightnessRail > 200) {
    EEPROM.write(RAILBRIGHTNESSADDRESS, DEFAULTRAILBRIGHTNESS);

    LEDbrightnessRail = DEFAULTRAILBRIGHTNESS;
    }
  if (LEDbrightness < 0 || LEDbrightness > 200) {
    EEPROM.write(LEDBRIGHTNESSADDRESS, DEFAULTBRIGHTNESS);
    LEDbrightness = DEFAULTBRIGHTNESS;
    }

  if (LEDbrightnessSpecial < 0 || LEDbrightnessSpecial > 200) {
    EEPROM.write(SPECIALBRIGHTNESSADDRESS, DEFAULTSPECIALNETBRIGHTNESS);
    LEDbrightnessSpecial = DEFAULTSPECIALNETBRIGHTNESS;
    }
  if (rotaryEncoderMode != 0 && rotaryEncoderMode != 1) {
    EEPROM.write(ROTARYENCODER_MODE_ADDRESS, 0);
    rotaryEncoderMode = 0;
    }

  EEPROM.commit();
  delay(5);
  }

void debugFlagSet(int flag) {
  int flagStatus;
  switch (flag) {
    case 1: {
    flagStatus = EEPROM.read(DEBUG_FILEPARSINGADDRESS);
    if (flagStatus == 0) {
      EEPROM.write(DEBUG_FILEPARSINGADDRESS, 1);

      debugFP = true;
      } else {
      EEPROM.write(DEBUG_FILEPARSINGADDRESS, 0);

      debugFP = false;
      }

    break;
    }

    case 2: {
    flagStatus = EEPROM.read(DEBUG_NETMANAGERADDRESS);

    if (flagStatus == 0) {
      EEPROM.write(DEBUG_NETMANAGERADDRESS, 1);

      debugNM = true;
      } else {
      EEPROM.write(DEBUG_NETMANAGERADDRESS, 0);

      debugNM = false;
      }
    break;
    }

    case 3: {
    flagStatus = EEPROM.read(DEBUG_NETTOCHIPCONNECTIONSADDRESS);

    if (flagStatus == 0) {
      EEPROM.write(DEBUG_NETTOCHIPCONNECTIONSADDRESS, 1);

      debugNTCC = true;
      } else {
      EEPROM.write(DEBUG_NETTOCHIPCONNECTIONSADDRESS, 0);

      debugNTCC = false;
      }

    break;
    }
    case 4: {
    flagStatus = EEPROM.read(DEBUG_NETTOCHIPCONNECTIONSALTADDRESS);

    if (flagStatus == 0) {
      EEPROM.write(DEBUG_NETTOCHIPCONNECTIONSALTADDRESS, 1);

      debugNTCC2 = true;
      } else {
      EEPROM.write(DEBUG_NETTOCHIPCONNECTIONSALTADDRESS, 0);

      debugNTCC2 = false;
      }
    break;
    }

    case 5: {
    flagStatus = EEPROM.read(DEBUG_LEDSADDRESS);

    if (flagStatus == 0) {
      EEPROM.write(DEBUG_LEDSADDRESS, 1);

      debugLEDs = true;
      } else {
      EEPROM.write(DEBUG_LEDSADDRESS, 0);

      debugLEDs = false;
      }
    break;
    }

    case 6: {
    flagStatus = EEPROM.read(PROBESWAPADDRESS);

    if (flagStatus == 0) {
      EEPROM.write(PROBESWAPADDRESS, 1);

      probeSwap = true;
      } else {
      EEPROM.write(PROBESWAPADDRESS, 0);

      probeSwap = false;
      }
    break;
    }

    case 0: {
    EEPROM.write(DEBUG_FILEPARSINGADDRESS, 0);
    EEPROM.write(TIME_FILEPARSINGADDRESS, 0);
    EEPROM.write(DEBUG_NETMANAGERADDRESS, 0);
    EEPROM.write(TIME_NETMANAGERADDRESS, 0);
    EEPROM.write(DEBUG_NETTOCHIPCONNECTIONSADDRESS, 0);
    EEPROM.write(DEBUG_NETTOCHIPCONNECTIONSALTADDRESS, 0);
    EEPROM.write(DEBUG_LEDSADDRESS, 0);

    debugFP = false;
    debugFPtime = false;
    debugNM = false;
    debugNMtime = false;
    debugNTCC = false;
    debugNTCC2 = false;
    debugLEDs = false;

    break;
    }

    case 9: {
    EEPROM.write(DEBUG_FILEPARSINGADDRESS, 1);
    EEPROM.write(TIME_FILEPARSINGADDRESS, 1);
    EEPROM.write(DEBUG_NETMANAGERADDRESS, 1);
    EEPROM.write(TIME_NETMANAGERADDRESS, 1);
    EEPROM.write(DEBUG_NETTOCHIPCONNECTIONSADDRESS, 1);
    EEPROM.write(DEBUG_NETTOCHIPCONNECTIONSALTADDRESS, 1);
    EEPROM.write(DEBUG_LEDSADDRESS, 1);
    debugFP = true;
    debugFPtime = true;
    debugNM = true;
    debugNMtime = true;
    debugNTCC = true;
    debugNTCC2 = true;
    debugLEDs = true;
    break;
    }
    case 10: {
        {
        EEPROM.write(ROTARYENCODER_MODE_ADDRESS, 0);

        rotaryEncoderMode = 0;
        }
        break;
    }
    case 11: {
        {
        EEPROM.write(ROTARYENCODER_MODE_ADDRESS, 1);

        rotaryEncoderMode = 1;
        }
        break;
    }
    case 7: {
    if (loadFileOnStart == 0) {
      EEPROM.write(LOADFILEONSTART_ADDRESS, 1);

      loadFileOnStart = 1;
      } else {
      EEPROM.write(LOADFILEONSTART_ADDRESS, 0);

      loadFileOnStart = 0;
      }
    break;
    }
    }
  delay(4);
  EEPROM.commit();
  delay(8);
  return;
  }

void runCommandAfterReset(char command) {
  if (EEPROM.read(CLEARBEFORECOMMANDADDRESS) == 1) {
    return;
    } else {

    EEPROM.write(CLEARBEFORECOMMANDADDRESS, 1);
    EEPROM.write(LASTCOMMANDADDRESS, command);
    EEPROM.commit();

    digitalWrite(RESETPIN, HIGH);
    delay(1);
    digitalWrite(RESETPIN, LOW);

    AIRCR_Register = 0x5FA0004; // hard reset
    }
  }

char lastCommandRead(void) {

  Serial.print("last command: ");

  Serial.println((char)EEPROM.read(LASTCOMMANDADDRESS));

  return EEPROM.read(LASTCOMMANDADDRESS);
  }
void lastCommandWrite(char lastCommand) {

  EEPROM.write(LASTCOMMANDADDRESS, lastCommand);
  }
