#include "UserCode.h"
#include "Commands.h"
#include "Graphics.h"
#include "JumperlessDefinesRP2040.h"
#include "MachineCommands.h"
#include "MatrixStateRP2040.h"
#include "NetManager.h"
#include "Peripherals.h"
#include "Probing.h"
#include <Arduino.h>

#include "ArduinoStuff.h"
#include "NetsToChipConnections.h"
#include "PersistentStuff.h"

void sketchOne(void) {

  int countLoop = 0;
  int countMult = 18;
  measureModeActive = 1;

  while (Serial.available() == 0) {

    for (int i = 1; i < 96; i++) {


      if (i == 84 || i == NANO_RESET_0 || i == NANO_RESET_1) {
        continue;
        }
      if (i > 60 && i < 70) {
        continue;
        }
      struct rowLEDs currentRow = getRowLEDdata(i);

      //b.printRawRow(0b00100, i - 1, 0x100010, 0xFFFFFF);

      float measuredVoltage = measureVoltage(2, i, true);

      if (measuredVoltage == 0xFFFFFFFF) {
        // Serial.println("floating");
        } else {
        printNodeOrName(i);

        Serial.print("\t = ");
        Serial.print(measuredVoltage);
        Serial.println(" V");
        }
      //
      // delay(50);

      setRowLEDdata(i, currentRow);

      if (Serial.available() > 0) {
        break;
        }
      // showLEDsCore2 = 2;
      }
    Serial.println("\n\r");
    countLoop++;
    if (countLoop * countMult > 95) {
      // break;
      countLoop = 0;
      countMult -= 2;
      }

    }
  refreshConnections(-1, 1);
  measureModeActive = 0;
  }

void sketchTwo(void) { }