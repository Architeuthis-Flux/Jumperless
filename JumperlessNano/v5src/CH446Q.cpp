// SPDX-License-Identifier: MIT

#include "CH446Q.h"
#include "JumperlessDefinesRP2040.h"
#include "LEDs.h"
#include "MatrixStateRP2040.h"
#include "NetsToChipConnections.h"
#include "Peripherals.h"

#include "hardware/pio.h"

#include "ch446.pio.h"
// #include "pio_spi.h"

#define MYNAMEISERIC                                                           \
  0 // on the board I sent to eric, the data and clock lines are bodged to GPIO
    // 18 and 19. To allow for using hardware SPI

// int chipToPinArray[12] = {CS_A, CS_B, CS_C, CS_D, CS_E, CS_F, CS_G, CS_H,
// CS_I, CS_J, CS_K, CS_L};
PIO pio = pio0;

uint sm = pio_claim_unused_sm(pio, true);

volatile int chipSelect = 0;
volatile uint32_t irq_flags = 0;

void isrFromPio(void) {

  // delayMicroseconds(500);
  setCSex(chipSelect, 1);
  //  Serial.println("interrupt from pio  ");
  // Serial.print(chipSelect);
  // Serial.print(" \n\r");
  delayMicroseconds(10);

  setCSex(chipSelect, 0);

  // delayMicroseconds(40);

  irq_flags = pio0_hw->irq;
  pio_interrupt_clear(pio, PIO0_IRQ_0);
  hw_clear_bits(&pio0_hw->irq, irq_flags);
}

struct pathStruct lastPath[MAX_BRIDGES];
//struct pathStruct newPath[MAX_BRIDGES];
int lastPathNumber = 0;
int changedPaths[MAX_BRIDGES];
int changedPathsCount = 0;

void initCH446Q(void) {

  uint dat = 14;
  uint clk = 15;

  // uint cs = 7;

  irq_add_shared_handler(PIO0_IRQ_0, isrFromPio,
                         PICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY);
  irq_set_enabled(PIO0_IRQ_0, true);

  uint offset = pio_add_program(pio, &spi_ch446_multi_cs_program);
  // uint offsetCS = pio_add_program(pio, &spi_ch446_cs_handler_program);

  // Serial.print("offset: ");
  // Serial.println(offset);

  pio_spi_ch446_multi_cs_init(pio, sm, offset, 8, 16, 0, 1, clk, dat);

  for (int i = 0; i < 12; i++) {
    pinMode(28 + i, OUTPUT);
    // digitalWrite(28+i, LOW);
  }
  // pio_spi_ch446_cs_handler_init(pio, smCS, offsetCS, 256, 1, 8, 20, 6);
  // pinMode(CS_A, OUTPUT);
  // digitalWrite(CS_A, HIGH);


  for (int i = 0; i < MAX_BRIDGES; i++) {
    lastPath[i].chip[0] = -1;
    lastPath[i].chip[1] = -1;
    lastPath[i].chip[2] = -1;
    lastPath[i].chip[3] = -1;
    lastPath[i].x[0] = -1;
    lastPath[i].x[1] = -1;
    lastPath[i].x[2] = -1;
    lastPath[i].x[3] = -1;
    lastPath[i].y[0] = -1;
    lastPath[i].y[1] = -1;
    lastPath[i].y[2] = -1;
    lastPath[i].y[3] = -1;
    
  }

}



void sendAllPaths(void) // should we sort them by chip? for now, no
{
  //   digitalWrite(RESETPIN, HIGH);
  // delay(1);
  // digitalWrite(RESETPIN, LOW);
  //   delay(10);
  // MCPIO.write16(0);

 findDifferentPaths();
  lastPathNumber = numberOfPaths;
  for (int i = 0; i < numberOfPaths; i++) {

    lastPath[i] = path[i];

    // if (path[i].skip == true)
    // {
    //   continue;
    // }

    if (changedPaths[i] == -1) {
      //continue;
    }
    sendPath(i, 1);
    // delay(1);
    if (debugNTCC)
    {
      Serial.print("lastPath ");
      Serial.print(i);
      Serial.print(" \n\r");
      printPathType(i);
      Serial.print(" \n\r");
      for (int j = 0; j < 4; j++) {
        printChipNumToChar(lastPath[i].chip[j]);
        Serial.print("  x[");
        Serial.print(j);
        Serial.print("]:");
        Serial.print(lastPath[i].x[j]);
        Serial.print("   y[");
        Serial.print(j);
        Serial.print("]:");
        Serial.print(lastPath[i].y[j]);
        Serial.println(" \t ");
      }
      Serial.print("\n\n\r");

      Serial.print("path ");
      Serial.print(i);
      Serial.print(" \t");
      printPathType(i);
      Serial.print(" \n\r");
      for (int j = 0; j < 4; j++) {
        printChipNumToChar(path[i].chip[j]);
        Serial.print("  x[");
        Serial.print(j);
        Serial.print("]:");
        Serial.print(path[i].x[j]);
        Serial.print("   y[");
        Serial.print(j);
        Serial.print("]:");
        Serial.print(path[i].y[j]);
        Serial.println(" \t ");
      }
      Serial.print("\n\n\r");
    }
  }
}


void findDifferentPaths(void) {
  int numToSearch = 0;
  int numChanged = 0;

  for (int i = 0; i < MAX_BRIDGES; i++) {
    changedPaths[i] = -2;
  }
  if (numberOfPaths > lastPathNumber) {
    numToSearch = numberOfPaths;
  } else {
    numToSearch = lastPathNumber;
  }
  for (int i = 0; i <= numToSearch; i++) {
    for (int j = i; j <= numToSearch; j++) {

      // if (i == j)
      // {
      //   continue;
      // }
      // changedPaths[i] = 1;
      if (lastPath[i].chip[0] == path[j].chip[0] &&
          lastPath[i].chip[1] == path[j].chip[1] &&
          lastPath[i].chip[2] == path[j].chip[2] &&
          lastPath[i].chip[3] == path[j].chip[3] &&
          lastPath[i].x[0] == path[j].x[0] &&
          lastPath[i].x[1] == path[j].x[1] &&
          lastPath[i].x[2] == path[j].x[2] &&
          lastPath[i].x[3] == path[j].x[3] &&
          lastPath[i].y[0] == path[j].y[0] &&
          lastPath[i].y[1] == path[j].y[1] &&
          lastPath[i].y[2] == path[j].y[2] &&
          lastPath[i].y[3] == path[j].y[3]) {
        // Serial.print("path ");
        // Serial.print(i);
        // Serial.print(" == ");
        // Serial.print(j);
        // Serial.print("\n\r");
        if (i == j) {
          // continue;
          changedPaths[i] = -1;
        } else {
          changedPaths[i] = j;
        }

        break;
      } else {
        // Serial.print("path ");
        // Serial.print(i);
        // Serial.print(" is different from path ");
        // Serial.print(j);
        // Serial.print("\n\r");
        // changedPaths[i] = 1;
        // numChanged++;
      }
    }
  }

  // Serial.print("numChanged = ");
  // Serial.print(numChanged);
  // Serial.print("\n\r");
  // Serial.println("changedPaths = ");
  for (int i = 0; i < numToSearch; i++) {
    // if (changedPaths[i] == 1)
    // {
    // Serial.print(i);
    // Serial.print(": ");
    // Serial.println(changedPaths[i]);

    if (changedPaths[i] == -2) {
      sendPath(i, 0, 1);
    }
    // Serial.print(" ");
  }

  // Serial.println("\n\n\r");

  // printPathsCompact();
  // Serial.println("\n\n\n\r");
}

void sendPath(int i, int setOrClear, int newOrLast) {

  uint32_t chAddress = 0;

  int chipToConnect = 0;
  int chYdata = 0;
  int chXdata = 0;
  if (newOrLast == 1) {
    for (int chip = 0; chip < 4; chip++) {
      if (lastPath[i].chip[chip] != -1) {
        chipSelect = lastPath[i].chip[chip];

        chipToConnect = lastPath[i].chip[chip];

        if (lastPath[i].y[chip] == -1 || lastPath[i].x[chip] == -1) {
          if (debugNTCC)
            Serial.print("!");

          continue;
        }

        sendXYraw(chipToConnect, lastPath[i].x[chip], lastPath[i].y[chip], 0);
      }
    } }
    else {
    
    for (int chip = 0; chip < 4; chip++) {
      if (path[i].chip[chip] != -1) {
        chipSelect = path[i].chip[chip];

        chipToConnect = path[i].chip[chip];

        if (path[i].y[chip] == -1 || path[i].x[chip] == -1) {
          if (debugNTCC)
            Serial.print("!");

          continue;
        }

        sendXYraw(chipToConnect, path[i].x[chip], path[i].y[chip], setOrClear);
      }
    }
  }
}

void sendXYraw(int chip, int x, int y, int setOrClear) {
  uint32_t chAddress = 0;
  chipSelect = chip;

  int chYdata = y;
  int chXdata = x;

  chYdata = chYdata << 5;
  chYdata = chYdata & 0b11100000;

  chXdata = chXdata << 1;
  chXdata = chXdata & 0b00011110;

  chAddress = chYdata | chXdata;

  if (setOrClear == 1) {
    chAddress =
        chAddress |
        0b00000001; // this last bit determines whether we set or unset the path
  }

  chAddress = chAddress << 24;

  delayMicroseconds(50);

  pio_sm_put(pio, sm, chAddress);

  delayMicroseconds(80);
  // isrFromPio();
}

void createXYarray(void) {}
