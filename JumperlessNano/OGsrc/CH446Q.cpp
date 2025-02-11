// SPDX-License-Identifier: MIT

#include "CH446Q.h"
#include "MatrixStateRP2040.h"
#include "NetsToChipConnections.h"
#include "LEDs.h"
#include "Peripherals.h"
#include "JumperlessDefinesRP2040.h"

#include "hardware/pio.h"

#include "spi.pio.h"
#include "pio_spi.h"

#define MYNAMEISERIC 0 // on the board I sent to eric, the data and clock lines are bodged to GPIO 18 and 19. To allow for using hardware SPI

int chipToPinArray[12] = {CS_A, CS_B, CS_C, CS_D, CS_E, CS_F, CS_G, CS_H, CS_I, CS_J, CS_K, CS_L};
PIO pio = pio0;

uint sm = pio_claim_unused_sm(pio, true);

volatile int chipSelect = 0;
volatile uint32_t irq_flags = 0;

void isrFromPio(void)
{
  switch (chipSelect)
  {
  case CHIP_A:
  {
    digitalWrite(CS_A, HIGH);
    break;
  }
  case CHIP_B:
  {
    digitalWrite(CS_B, HIGH);
    break;
  }
  case CHIP_C:
  {
    digitalWrite(CS_C, HIGH);
    break;
  }
  case CHIP_D:
  {
    digitalWrite(CS_D, HIGH);
    break;
  }
  case CHIP_E:
  {
    digitalWrite(CS_E, HIGH);
    break;
  }
  case CHIP_F:
  {
    digitalWrite(CS_F, HIGH);
    break;
  }
  case CHIP_G:
  {
    digitalWrite(CS_G, HIGH);
    break;
  }
  case CHIP_H:
  {
    digitalWrite(CS_H, HIGH);
    break;
  }
  case CHIP_I:
  {
    digitalWrite(CS_I, HIGH);
    break;
  }
  case CHIP_J:
  {
    digitalWrite(CS_J, HIGH);
    break;
  }
  case CHIP_K:
  {
    digitalWrite(CS_K, HIGH);
    break;
  }
  case CHIP_L:
  {
    digitalWrite(CS_L, HIGH);
    break;
  }
  }

  delayMicroseconds(6);
  digitalWrite(CS_A, LOW);
  digitalWrite(CS_B, LOW);
  digitalWrite(CS_C, LOW);
  digitalWrite(CS_D, LOW);
  digitalWrite(CS_E, LOW);
  digitalWrite(CS_F, LOW);
  digitalWrite(CS_G, LOW);

  digitalWrite(CS_H, LOW);
  digitalWrite(CS_I, LOW);
  digitalWrite(CS_J, LOW);
  digitalWrite(CS_K, LOW);
  digitalWrite(CS_L, LOW);
  delayMicroseconds(4);
  irq_flags = pio0_hw->irq;
  pio_interrupt_clear(pio, PIO0_IRQ_0);
  hw_clear_bits(&pio0_hw->irq, irq_flags);
}

void initCH446Q(void)
{

  uint dat = 14;
  uint clk = 15;

  if (MYNAMEISERIC)
  {
    dat = 18;
    clk = 19;
  }

  uint cs = 7;

  irq_add_shared_handler(PIO0_IRQ_0, isrFromPio, PICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY);
  irq_set_enabled(PIO0_IRQ_0, true);

  uint offset = pio_add_program(pio, &spi_ch446_multi_cs_program);
  // uint offsetCS = pio_add_program(pio, &spi_ch446_cs_handler_program);

  // Serial.print("offset: ");
  // Serial.println(offset);

  pio_spi_ch446_multi_cs_init(pio, sm, offset, 8, 16, 0, 1, clk, dat);
  // pio_spi_ch446_cs_handler_init(pio, smCS, offsetCS, 256, 1, 8, 20, 6);
  // pinMode(CS_A, OUTPUT);
  // digitalWrite(CS_A, HIGH);

  pinMode(CS_A, OUTPUT_8MA);
  pinMode(CS_B, OUTPUT_8MA);
  pinMode(CS_C, OUTPUT_8MA);
  pinMode(CS_D, OUTPUT_8MA);
  pinMode(CS_E, OUTPUT_8MA);
  pinMode(CS_F, OUTPUT_8MA);
  pinMode(CS_G, OUTPUT_8MA);
  pinMode(CS_H, OUTPUT_8MA);
  pinMode(CS_I, OUTPUT_8MA);
  pinMode(CS_J, OUTPUT_8MA);
  pinMode(CS_K, OUTPUT_8MA);
  pinMode(CS_L, OUTPUT_8MA);

  digitalWrite(CS_A, LOW);
  digitalWrite(CS_B, LOW);
  digitalWrite(CS_C, LOW);
  digitalWrite(CS_D, LOW);
  digitalWrite(CS_E, LOW);
  digitalWrite(CS_F, LOW);
  digitalWrite(CS_G, LOW);
  digitalWrite(CS_H, LOW);
  digitalWrite(CS_I, LOW);
  digitalWrite(CS_J, LOW);
  digitalWrite(CS_K, LOW);
  digitalWrite(CS_L, LOW);

  pinMode(RESETPIN, OUTPUT_12MA);

  digitalWrite(RESETPIN, HIGH);
  delay(3);
  digitalWrite(RESETPIN, LOW);
}

void resetArduino(void)
{
  int lastPath = MAX_BRIDGES - 1;
  path[lastPath].chip[0] = CHIP_I;
  path[lastPath].chip[1] = CHIP_I;
  path[lastPath].x[0] = 11;
  path[lastPath].y[0] = 0;
  path[lastPath].x[1] = 15;
  path[lastPath].y[1] = 0;

  sendPath(lastPath, 1);
  delay(15);
  sendPath(lastPath, 0);
}
void sendAllPaths(void) // should we sort them by chip? for now, no
{

  for (int i = 0; i < numberOfPaths; i++)
  {

    if (path[i].skip == true)
    {
      continue;
    }
    sendPath(i, 1);
    if (debugNTCC)
    {
      Serial.print("path ");
      Serial.print(i);
      Serial.print(" \t");
      printPathType(i);
      Serial.print(" \n\r");
      for (int j = 0; j < 4; j++)
      {
        printChipNumToChar(path[i].chip[j]);
        Serial.print("  x[");
        Serial.print(j);
        Serial.print("]:");
        Serial.print(path[i].x[j]);
        Serial.print("   y[");
        Serial.print(j);
        Serial.print("]:");
        Serial.print(path[i].y[j]);
        Serial.print(" \t ");
      }
      Serial.print("\n\n\r");
    }
  }
}

uint8_t restoreConnections[12][16][2]; // [chip][index][pairs 0=x, 1=y]
    
  


int clearAllConnectionsOnChip(int chip, int clearOrRestore)
{

int numCleared = 0;
uint8_t netCleared = 0;

if (clearOrRestore == 0)
{
  for (int x = 0; x < 16; x++)
  {
    if (ch[chip].xStatus[x] != -1)
    {
      netCleared = ch[chip].xStatus[x];

      for (int y = 0; y < 8; y++)
      {
        if (ch[chip].yStatus[y] == netCleared)
        {
          
          restoreConnections[chip][numCleared][0] = x;
          restoreConnections[chip][numCleared][1] = y;
          numCleared++;
          Serial.print("chip ");
          Serial.print(chip);
          Serial.print(" x:");
          Serial.print(x);
          Serial.print(" y:");
          Serial.print(y);
          Serial.print(" net:");
          Serial.print(netCleared);
          Serial.print("\n\r");
          sendXYraw(chip, x, y, 0);
        }
      }
    }
  }
  } else {
    for (int i = 0; i < numCleared; i++)
    {
      sendXYraw(chip, restoreConnections[chip][i][0], restoreConnections[chip][i][1], 1);
    }
  }
  return numCleared;

}

void sendPath(int i, int setOrClear)
{

  uint32_t chAddress = 0;

  int chipToConnect = 0;
  int chYdata = 0;
  int chXdata = 0;

  for (int chip = 0; chip < 4; chip++)
  {
    if (path[i].chip[chip] != -1)
    {
      chipSelect = path[i].chip[chip];

      chipToConnect = path[i].chip[chip];

      if (path[i].y[chip] == -1 || path[i].x[chip] == -1)
      {
        if (debugNTCC)
          Serial.print("!");

        continue;
      }

      sendXYraw(chipToConnect, path[i].x[chip], path[i].y[chip], setOrClear);

      // chYdata = path[i].y[chip];
      // chXdata = path[i].x[chip];

      // chYdata = chYdata << 5;
      // chYdata = chYdata & 0b11100000;

      // chXdata = chXdata << 1;
      // chXdata = chXdata & 0b00011110;

      // chAddress = chYdata | chXdata;

      // if (setOrClear == 1)
      // {
      //   chAddress = chAddress | 0b00000001; // this last bit determines whether we set or unset the path
      // }

      // chAddress = chAddress << 24;

      // // delayMicroseconds(50);

      // delayMicroseconds(20);

      // pio_sm_put(pio, sm, chAddress);

      // delayMicroseconds(40);
      // //}
    }
  }
}

void sendXYraw(int chip, int x, int y, int setOrClear)
{
  uint32_t chAddress = 0;
  chipSelect = chip;

  int chYdata = y;
  int chXdata = x;

  chYdata = chYdata << 5;
  chYdata = chYdata & 0b11100000;

  chXdata = chXdata << 1;
  chXdata = chXdata & 0b00011110;

  chAddress = chYdata | chXdata;

  if (setOrClear == 1)
  {
    chAddress = chAddress | 0b00000001; // this last bit determines whether we set or unset the path
  }

  chAddress = chAddress << 24;

  delayMicroseconds(50);

  pio_sm_put(pio, sm, chAddress);

  delayMicroseconds(80);
}

void createXYarray(void)
{
}
