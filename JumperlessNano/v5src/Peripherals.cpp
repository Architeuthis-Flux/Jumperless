// SPDX-License-Identifier: MIT

#include "Peripherals.h"

#include "Adafruit_INA219.h"

#include "LEDs.h"
#include "MatrixStateRP2040.h"
#include "NetManager.h"

#include "hardware/adc.h"
#include "hardware/gpio.h"
// #include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include <Arduino.h>
#include <stdio.h>

#include "MCP_DAC.h"
#include "mcp4725.hpp"
// #include "AdcUsb.h"

#include <Adafruit_MCP4728.h>

// #include <SPI.h>
// #include "MCP23S17.h"
//  #include "SPI.h"
// #include "pio_spi.h"

#include "PersistentStuff.h"
// #include <Adafruit_MCP23X17.h>
#include "FileParsing.h"
#include <Wire.h>

#include "CH446Q.h"
#include "Commands.h"
#include "Graphics.h"
#include "Probing.h"
#include "hardware/adc.h"
// #define CS_PIN 17

// MCP23S17 MCPIO(17, 16, 19, 18, 0x27); //  SW SPI address 0x00

// MCP23S17 MCPIO(17, 7,  &SPI); //  HW SPI address 0x00 //USE HW SPI
// Adafruit_MCP23X17 MCPIO;

// MCP23S17 MCPIO(17, 16, 19, 18, 0x7); //  SW SPI address 0x
#define CSI Serial.write("\x1B\x5B");
// #define CSI Serial.write("\033");

#define DAC_RESOLUTION 9

float adcRange[8][2] = {{-8, 8}, {-8, 8}, {-8, 8}, {-8, 8}, {0, 5}};
float dacOutput[2] = {0, 0};
float railVoltage[2] = {0, 0};

float dacSpread[4] = {20.2, 20.2, 20.2, 20.2};
int dacZero[4] = {1650, 1650, 1650, 1650};

float adcSpread[8] = {18.28, 18.28, 18.28, 18.28, 5.0, 17.28, 17.28, 17.28};
float adcZero[8] = {8.0, 8.0, 8.0, 8.0, 0.0, 8.0, 8.0, 8.0};

uint8_t gpioState[10] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
}; // 0 = output low, 1 = output high, 2 = input, 3 = input
   // pullup, 4 = input pulldown
uint8_t gpioReading[10] = {
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3}; // 0 = low, 1 = high 2 = floating 3 = unknown

int gpioNet[10] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

int revisionNumber = 0;

int showReadings = 0;

int showADCreadings[8] = {1, 1, 1, 1};
uint32_t adcReadingColors[8] = {0x010101, 0x010101, 0x010101,
                                0x010101, 0x010101, 0x010101};
float adcReadingRanges[8][2] = {
    {-8.0, 8.0}, {-8.0, 8.0}, {-8.0, 8.0}, {-8.0, 8.0}, {0.0, 5.0},
};

int inaConnected = 0;
int showINA0[3] = {1, 1, 1}; // 0 = current, 1 = voltage, 2 = power
int showINA1[3] = {0, 0, 0}; // 0 = current, 1 = voltage, 2 = power

int showDAC0 = 0;
int showDAC1 = 0;

int adcCalibration[6][3] = {{0, 0, 0},
                            {0, 0, 0},
                            {0, 0, 0},
                            {0, 0, 0}}; // 0 = min, 1 = middle, 2 = max,
int dacCalibration[4][3] = {{0, 0, 0},
                            {0, 0, 0},
                            {0, 0, 0},
                            {0, 0, 0}}; // 0 = min, 1 = middle, 2 = max,

float freq[3] = {1, 1, 0};
uint32_t period[3] = {0, 0, 0};
uint32_t halvePeriod[3] = {0, 0, 0};

// q = square
// s = sinus
// w = sawtooth
// t = stair
// r = random
char mode[3] = {'z', 'z', 'z'};

int dacOn[3] = {0, 0, 0};
int amplitude[3] = {0, 0, 0};
int offset[3] = {2048, 1650, 0};
int calib[3] = {0, 0, 0};

Adafruit_MCP4728 mcp;

// MCP4725_PICO dac0_5V(5.0);
// MCP4725_PICO dac1_8V(railSpread);

// MCP4822 dac_rev3; // A is dac0  B is dac1

INA219 INA0(0x40);
INA219 INA1(0x41);

uint16_t count;
uint32_t lastTime = 0;

// LOOKUP TABLE SINE
uint16_t sine0[360];
uint16_t sine1[360];

void initGPIO(void) {
  for (int i = 0; i < 20; i++) {
    pinMode(20 + i, OUTPUT);
    // digitalWrite(20 + i, LOW);
  }
}
int reads2[30][2];
int readIndex2 = 0;

int foundRolloff = 0;

int lastReading = 0;
int dacSetting = 0;
int reads[30][2];
int readIndex = 0;

void initADC(void) {

  // pinMode(ADC0_PIN, INPUT);
  // pinMode(ADC1_PIN, INPUT);
  // pinMode(ADC2_PIN, INPUT);
  // pinMode(ADC3_PIN, INPUT);
  // pinMode(ADC4_PIN, INPUT);
  // pinMode(ADC5_PIN, INPUT);
  // pinMode(ADC6_PIN, INPUT);
  // pinMode(ADC7_PIN, INPUT);

  analogReadResolution(12);
}

void initDAC(void) {
  initGPIO();
  Wire.setSDA(4);
  Wire.setSCL(5);
  Wire.setClock(1000000);
  Wire.begin();

  delay(5);
  // Try to initialize!
  if (!mcp.begin()) {
    delay(3000);
    Serial.println("Failed to find MCP4728 chip");
    // while (1)
    // {
    //   delay(10);
    // }
  }
  // delay(1000);
  // Serial.println("MCP4728 Found!");
  /*
   * @param channel The channel to update
   * @param new_value The new value to assign
   * @param new_vref Optional vref setting - Defaults to `MCP4728_VREF_VDD`
   * @param new_gain Optional gain setting - Defaults to `MCP4728_GAIN_1X`
   * @param new_pd_mode Optional power down mode setting - Defaults to
   * `MCP4728_PD_MOOE_NORMAL`
   * @param udac Optional UDAC setting - Defaults to `false`, latching (nearly).
   * Set to `true` to latch when the UDAC pin is pulled low
   *
   */
  pinMode(LDAC, OUTPUT);

  // saveVoltages(railVoltage[0], railVoltage[1], dacOutput[0], dacOutput[1]);
  digitalWrite(LDAC, HIGH);

  // // Vref = MCP_VREF_VDD, value = 0, 0V
  mcp.setChannelValue(MCP4728_CHANNEL_A, 1650);
  mcp.setChannelValue(MCP4728_CHANNEL_B, 1650);
  mcp.setChannelValue(MCP4728_CHANNEL_C, 1660);
  mcp.setChannelValue(MCP4728_CHANNEL_D, 1641); // 1650 is roughly 0V
  digitalWrite(LDAC, LOW);

  delay(10);

  if (mcp.saveToEEPROM() == false) {
    // delay(3000);
    Serial.println("Failed to save DAC settings to EEPROM");
  }
  // delay(100);
  setRailsAndDACs();
  delay(10);
}

int initI2C(int sdaPin, int sclPin, int speed) {



  static int i2c1Pins[3] = {22, 23, 100000};
  static int i2c0Pins[3] = {4, 5, 100000};
  // pin, {0=SDA, 1=SCL}, {I2C0, I2C1}


int gpioI2Cmap[15][3] = {
    {0,  0, 0}, {1,  1, 0},
    {4,  0, 0}, {5,  1, 0},
    {6,  0, 1}, {7,  1, 1},
    {20, 0, 0}, {21, 1, 0}, 
    {22, 0, 1}, {23, 1, 1}, 
    {24, 0, 0}, {25, 1, 0}, 
    {26, 0, 1}, {27, 1, 1}, 
};
//I2C0 is taken by current sensors and dac
//maybe bitbang I2C0?
int sdaFound = 0;
int sclFound = 0;

int portFound = 1; // right now only I2C1 is available
for (int i = 0; i < 15; i++) {
  if (gpioI2Cmap[i][0] == sdaPin && gpioI2Cmap[i][2] == 1) {
   int sdaFound = i;
    
  }
  if (gpioI2Cmap[i][0] == sclPin && gpioI2Cmap[i][2] == 1) {

    int sclFound = i;
  }
  if (sdaFound != 0 && sclFound != 0) {
    break;
  }
}

Serial.print("sdaFound: ");
Serial.println(sdaFound);
Serial.print("sclFound: ");
Serial.println(sclFound);

Serial.print("portFound: ");
Serial.println(portFound);



if (sdaFound == 0 || sclFound == 0) {
  return -1;
} else if (portFound == 0) {
  

  Wire.setSDA(sdaPin);
  Wire.setSCL(sclPin);
  Wire.setClock(speed);
  Wire.begin();
    if (i2c0Pins[0] == sdaPin && i2c0Pins[1] == sclPin && i2c0Pins[2] == speed) {
    return gpioI2Cmap[sdaFound][2] + 10; //returns 10 if the pins are already set
  }
  i2c0Pins[0] = sdaPin;
  i2c0Pins[1] = sclPin;
  i2c0Pins[2] = speed;

  return gpioI2Cmap[sdaFound][2];
} else if (portFound == 1) {
  Wire1.setSDA(sdaPin);
  Wire1.setSCL(sclPin);
  Wire1.setClock(speed);
  Wire1.begin();
  if (i2c1Pins[0] == sdaPin && i2c1Pins[1] == sclPin && i2c1Pins[2] == speed) {
    return gpioI2Cmap[sdaFound][2] + 10; //returns 11 if the pins are already set
  }
  i2c1Pins[0] = sdaPin;
  i2c1Pins[1] = sclPin;
  i2c1Pins[2] = speed;

  return gpioI2Cmap[sdaFound][2];
}




return -1;  

  
  // pinMode(22, INPUT_PULLUP);
  // pinMode(23, INPUT_PULLUP);
  // Wire1.setSDA(22);
  // Wire1.setSCL(23);
  // Wire1.begin();
}



void setGPIO(void) {
  //     // Serial.print(19+i);
  //     // Serial.print(" ");
  //     // Serial.println(gpioState[i]);
  return;
  //     Serial.println("\n\n\n\rsetGPIO\n\n\n\n\n\r");
  // return;
  for (int i = 0; i < 4; i++) {
    // Serial.println(gpioState[i]);

    switch (gpioState[i]) {
    case 0:
      pinMode(20 + i, OUTPUT);
      digitalWrite(20 + i, LOW);
      break;
    case 1:
      pinMode(20 + i, OUTPUT);
      digitalWrite(19 + i, HIGH);

      break;
    case 2:
      pinMode(20 + i, INPUT);
      break;
    case 3:
      pinMode(20 + i, INPUT_PULLUP);
      break;
    case 4:
      pinMode(20 + i, INPUT_PULLDOWN);
      break;
    }
  }
  Serial.println(" ");

  // for (int i = 4; i <= 8; i++) {
  //   switch (gpioState[i]) {
  //   case 0:
  //     MCPIO.pinMode(i, OUTPUT);
  //     MCPIO.digitalWrite(i, LOW);
  //     break;
  //   case 1:
  //     MCPIO.pinMode(i, OUTPUT);
  //     MCPIO.digitalWrite(i, HIGH);
  //     break;
  //   case 2:
  //     MCPIO.pinMode(i, INPUT);
  //     break;
  //   case 3:
  //     MCPIO.pinMode(i, INPUT_PULLUP);

  //     break;
  //   }
  //}
  rgbColor onColor = {254, 40, 35};
  rgbColor offColor = {0, 254, 48};

  for (int i = 1; i <= 8; i++) {
    if (gpioNet[i] != -1 && gpioState[i] != 2 && gpioState[i] != 3 &&
        gpioState[i] != 4) {

      net[gpioNet[i]].color = (gpioState[i] == 1 ? onColor : offColor);
      // net[gpioNet[i]].machine = 1;
      // net[gpioNet[i]].color = (gpioState[i] == 1 ? onColor : offColor);
      lightUpNet(gpioNet[i], -1, 1, 05, 0, 0,
                 packRgb(net[gpioNet[i]].color.r, net[gpioNet[i]].color.g,
                         net[gpioNet[i]].color.b));
    }
  }
}
uint32_t gpioReadingColors[10] = {0x010101, 0x010101, 0x010101, 0x010101,
                                  0x010101, 0x010101, 0x010101, 0x010101,
                                  0x010101, 0x010101};

void readGPIO(void) {
  // Serial.println("\n\n\n\rreadGPIO\n\n\n\n\n\r");
  // return;
  for (int i = 0; i < 4; i++) {
    if (gpioNet[i] != -1 &&
        (gpioState[i] == 2 || gpioState[i] == 3 || gpioState[i] == 4)) {
      int reading = digitalRead(GPIO_1_PIN + i); // readFloatingOrState(20 + i);
      switch (reading) {

      case 0:
        gpioReading[i] = 0;
        break;
      case 1:

        gpioReading[i] = 1;
        break;
      case 2:

        gpioReading[i] = 2;
        break;
      }
    } else {
      gpioReading[i] = 3;
    }
    // gpioReading[i] = digitalRead(20 + i);
  }

  // for (int i = 0; i < 4; i++) {

  //   if (gpioNet[i + 4] != -1 &&
  //       (gpioState[i + 4] == 2 || gpioState[i + 4] == 3 ||
  //        gpioState[i + 4] == 4)) {
  //     int reading = 0;//readFloatingOrStateMCP(i);
  //     switch (reading) {

  //     case 0:
  //       gpioReading[i + 4] = 2;
  //       break;
  //     case 1:

  //       gpioReading[i + 4] = 1;
  //       break;
  //     case 2:

  //       gpioReading[i + 4] = 0;
  //       break;
  //     }
  //   } else {
  //     gpioReading[i + 4] = 3;
  //   }
  // }

  for (int i = 0; i < 8; i++) {

    if (gpioNet[i] != -1) {
      if (gpioReading[i] == 0) {
        // lightUpNet(gpioNet[i], -1, 1, 5, 0, 0, 0x000f05);
        //         net[gpioNet[i]].color = {0x00, 0x0f, 0x05};
        //         netColors[gpioNet[i]] = {0x00, 0x0f, 0x05};
        // lightUpNet(gpioNet[i], -1, 1, 22, 0, 0, 0x000f05);

        gpioReadingColors[i] = 0x000f05;
      } else if (gpioReading[i] == 1) {
        // lightUpNet(gpioNet[i], -1, 1, 22, 0, 0, 0x220005);
        //         net[gpioNet[i]].color = {0x22, 0x00, 0x05};
        //         netColors[gpioNet[i]] = {0x22, 0x00, 0x05};
        // lightUpNet(gpioNet[i], -1, 1, 22, 0, 0, 0x220005);
        gpioReadingColors[i] = 0x220005;

      } else {
        // lightUpNet(gpioNet[i], -1, 1, 5, 0, 0, 0x000005);
        //  net[gpioNet[i]].color = {0x00, 0x00, 0x05};
        //  netColors[gpioNet[i]] = {0x00, 0x00, 0x05};
        //  lightUpNet(gpioNet[i]);
        gpioReadingColors[i] = 0x000005;
      }
    }
    Serial.print(gpioReading[i]);
    Serial.print(" ");
  }
  showLEDsCore2 = 2;
  Serial.println();
}

void setRailsAndDACs(int saveEEPROM) {
  setTopRail(railVoltage[0], 1, saveEEPROM);
  // delay(10);
  setBotRail(railVoltage[1], 1, saveEEPROM);
  // delay(10);
  setDac0voltage(dacOutput[0], 1, saveEEPROM);
  // delay(10);
  setDac1voltage(dacOutput[1], 1, saveEEPROM);
  // delay(10);
}
void setTopRail(float value, int save, int saveEEPROM) {

  int dacValue = (value * 4095 / dacSpread[2]) + dacZero[2];

  if (dacValue > 4095) {
    dacValue = 4095;
  } else if (dacValue < 0) {
    dacValue = 0;
  }

  digitalWrite(LDAC, HIGH);
  mcp.setChannelValue(MCP4728_CHANNEL_C, dacValue);
  digitalWrite(LDAC, LOW);
  if (save) {
    railVoltage[0] = value;
  }
  if (saveEEPROM) {

    saveVoltages(railVoltage[0], railVoltage[1], dacOutput[0], dacOutput[1]);
  }
}

void setBotRail(float value, int save, int saveEEPROM) {

  int dacValue = (value * 4095 / dacSpread[3]) + dacZero[3];

  if (dacValue > 4095) {
    dacValue = 4095;
  } else if (dacValue < 0) {
    dacValue = 0;
  }

  digitalWrite(LDAC, HIGH);
  mcp.setChannelValue(MCP4728_CHANNEL_D, dacValue);
  digitalWrite(LDAC, LOW);
  if (save) {
    railVoltage[1] = value;
  }
  if (saveEEPROM) {

    saveVoltages(railVoltage[0], railVoltage[1], dacOutput[0], dacOutput[1]);
  }
}

// void setTopRail(int value, int save, int saveEEPROM) {
//   if (value > 4095) {
//     value = 4095;
//   }
//   if (value < 0) {
//     value = 0;
//   }
//   mcp.setChannelValue(MCP4728_CHANNEL_C, value);
// }
// void setBotRail(int value, int save, int saveEEPROM) {
//   if (value > 4095) {
//     value = 4095;
//   }
//   if (value < 0) {
//     value = 0;
//   }
//   mcp.setChannelValue(MCP4728_CHANNEL_D, value);
// }
// uint16_t lastInputCode0 = 0;
// uint16_t lastInputCode1 = offset[1] + calib[1];

void setDac0voltage(float voltage, int save, int saveEEPROM) {
  // int dacValue = (voltage * 4095 / 19.8) + 1641;
  int dacValue = (voltage * 4095 / dacSpread[0]) + dacZero[0];
  // Serial.print(dacSpread[0]);
  // Serial.print(" ");
  // Serial.print(dacZero[0]);
  // Serial.print(" ");

  // Serial.print(voltage);
  // Serial.print(" ");
  // Serial.print(dacValue);
  // Serial.println(" ");
  if (dacValue > 4095) {
    dacValue = 4095;
  }
  if (dacValue < 0) {
    dacValue = 0;
  }

  digitalWrite(LDAC, HIGH);
  // delay(10);
  if (mcp.setChannelValue(MCP4728_CHANNEL_A, dacValue) == false) {
    // delay(3000);
    Serial.println("Failed to set DAC0 value");
  }
  // delay(10);
  digitalWrite(LDAC, LOW);
  if (save) {

    dacOutput[0] = voltage;
  }

  if (saveEEPROM) {

    saveVoltages(railVoltage[0], railVoltage[1], dacOutput[0], dacOutput[1]);
  }
}

void setDac0voltage(uint16_t inputCode) {
  digitalWrite(LDAC, HIGH);
  mcp.setChannelValue(MCP4728_CHANNEL_A, inputCode);
  digitalWrite(LDAC, LOW);
}

void setDac1voltage(float voltage, int save, int saveEEPROM) {

  int dacValue = (voltage * 4095 / dacSpread[1]) + dacZero[1];

  if (dacValue > 4095) {
    dacValue = 4095;
  }
  if (dacValue < 0) {
    dacValue = 0;
  }
  // Serial.print(voltage);
  // Serial.print(" ");
  // Serial.print(dacValue);
  // Serial.print(" ");
  digitalWrite(LDAC, HIGH);
  mcp.setChannelValue(MCP4728_CHANNEL_B, dacValue);
  digitalWrite(LDAC, LOW);
  if (save) {

    dacOutput[1] = voltage;
  }

  if (saveEEPROM) {

    saveVoltages(railVoltage[0], railVoltage[1], dacOutput[0], dacOutput[1]);
  }
}

void setDac1voltage(uint16_t inputCode) {
  digitalWrite(LDAC, HIGH);
  mcp.setChannelValue(MCP4728_CHANNEL_B, inputCode);
  digitalWrite(LDAC, LOW);
}

uint8_t csToPin[16] = {8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7};
// uint8_t csToPin[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
// 15};
uint16_t chipMask[16] = {
    0b0000000000000001, 0b0000000000000010, 0b0000000000000100,
    0b0000000000001000, 0b0000000000010000, 0b0000000000100000,
    0b0000000001000000, 0b0000000010000000, 0b0000000100000000,
    0b0000001000000000, 0b0000010000000000, 0b0000100000000000,
    0b0001000000000000, 0b0010000000000000, 0b0100000000000000,
    0b1000000000000000};

void setCSex(int chip, int value) {

  // if (value > 0) {

  //   MCPIO.write16(chipMask[chip]);

  //   delayMicroseconds(20);
  // } else {
  //   MCPIO.write16(0b0000000000000000);
  //   delayMicroseconds(20);

  // }

  if (chip > 11) {
    return;
  }

  if (value > 0) {
    digitalWrite(chip + 28, HIGH);
    // Serial.println(chip+28);
  } else {
    digitalWrite(chip + 28, LOW);
    // Serial.println(chip+28);
  }
}

void writeGPIOex(int value, uint8_t pin) {}

void initINA219(void) {

  // delay(3000);
  // Serial.println(__FILE__);
  // Serial.print("INA219_LIB_VERSION: ");
  // Serial.println(INA219_LIB_VERSION);

  // Wire.begin();
  // Wire.setClock(1000000);
  Wire.begin();

  if (!INA0.begin() || !INA1.begin()) {
    delay(3000);
    Serial.println("Failed to find INA219 chip");
  }

  INA0.setMaxCurrentShunt(1, 2.0);
  INA1.setMaxCurrentShunt(1, 2.0);

  INA0.setBusVoltageRange(16);
  INA1.setBusVoltageRange(16);

  //  calibrateDacs();
}


void setDacByNumber(int dac, float voltage, int save, int saveEEPROM) {
  switch (dac) {
  case 0:
    setDac0voltage(voltage, save, saveEEPROM);
    break;
  case 1:
    setDac1voltage(voltage, save, saveEEPROM);
    break;
  case 2:
    setTopRail(voltage, save, saveEEPROM);
    break;
  case 3:
    setBotRail(voltage, save, saveEEPROM);
    break;
  }
}

void dacSine(int resolution) {
  uint16_t i;
  switch (resolution) {
  case 0 ... 5:
    for (i = 0; i < 32; i++) {
      setDac1voltage(DACLookup_FullSine_5Bit[i]);
    }
    break;
  case 6:
    for (i = 0; i < 64; i++) {
      setDac1voltage(DACLookup_FullSine_6Bit[i]);
    }
    break;
  case 7:
    for (i = 0; i < 128; i++) {
      setDac1voltage(DACLookup_FullSine_7Bit[i]);
    }
    break;

  case 8:
    for (i = 0; i < 256; i++) {
      setDac1voltage(DACLookup_FullSine_8Bit[i]);
    }
    break;

  case 9 ... 12:
    for (i = 0; i < 512; i++) {
      setDac1voltage(DACLookup_FullSine_9Bit[i]);
    }
    break;
  }
}

void chooseShownReadings(void) {
  showADCreadings[0] = 0;
  showADCreadings[1] = 0;
  showADCreadings[2] = 0;
  showADCreadings[3] = 0;
  showADCreadings[4] = 0;
  showADCreadings[5] = 0;
  showADCreadings[6] = 0;
  showADCreadings[7] = 0;

  showINA0[0] = 0;
  showINA0[1] = 0;
  showINA0[2] = 0;

  inaConnected = 0;

  for (int i = 0; i <= newBridgeIndex; i++) {

    if (path[i].node1 == ADC0 || path[i].node2 == ADC0) {
      showADCreadings[0] = path[i].net;
    }

    if (path[i].node1 == ADC1 || path[i].node2 == ADC1) {
      showADCreadings[1] = path[i].net;
    }

    if (path[i].node1 == ADC2 || path[i].node2 == ADC2) {
      showADCreadings[2] = path[i].net;
    }

    if (path[i].node1 == ADC3 || path[i].node2 == ADC3) {
      showADCreadings[3] = path[i].net;
    }

    if (path[i].node1 == ADC4 || path[i].node2 == ADC4) {
      showADCreadings[4] = path[i].net;
    }

    if (path[i].node1 == ADC7 || path[i].node2 == ADC7) {
      showADCreadings[7] = path[i].net;
    }
    if (path[i].node1 == ROUTABLE_BUFFER_IN ||
        path[i].node2 == ROUTABLE_BUFFER_IN) { // routable buffer in is
                                               // hardwired to probe sense adc
      showADCreadings[7] = path[i].net;
    }

    // Serial.println(newBridgeIndex);
    //     for (int j = 0; j < 4; j++)
    //     {
    //       Serial.print("ADC");
    //       Serial.print(j);
    //       Serial.print(": ");
    //       Serial.print(showADCreadings[j]);
    //       Serial.print(" ");

    //     }

    //     Serial.println(" ");

    if (path[i].node1 == ISENSE_PLUS || path[i].node1 == ISENSE_PLUS ||
        path[i].node2 == ISENSE_MINUS || path[i].node2 == ISENSE_MINUS) {
      // Serial.println(showReadings);

      inaConnected = 1;

      switch (showReadings) {
      case 0:
        break;

      case 1:
        showINA0[0] = 1;
        showINA0[1] = 0;
        showINA0[2] = 0;
        break;
      case 2:
        showINA0[0] = 1;
        showINA0[1] = 1;
        showINA0[2] = 0;
        break;
      case 3:

        showINA0[0] = 1;
        showINA0[1] = 1;
        showINA0[2] = 1;
        break;
      }

      // Serial.println(showINA0[0]);
      // Serial.println(showINA0[1]);
      // Serial.println(showINA0[2]);
      // Serial.println(" ");
    }
  }
  if (inaConnected == 0) {
    showINA0[0] = 0;
    showINA0[1] = 0;
    showINA0[2] = 0;
    // showReadings = 3;
  }
}

float railSpread = 17.88;

void showLEDmeasurements(void) {
  int samples =16;

  //int adc0ReadingUnscaled;
  float adcReading;

  // int adc1ReadingUnscaled;
  // float adc1Reading;

  // int adc2ReadingUnscaled;
  // float adc2Reading;

  // int adc3ReadingUnscaled;
  // float adc3Reading;

  // int adc4ReadingUnscaled;
  // float adc4Reading;

  // int adc7ReadingUnscaled;
  // float adc7Reading;

  int bs = 0;

  int numReadings = 0;

  uint32_t color = 0x000000;


for (int i = 0; i < 8; i++) {

  if (showADCreadings[i] > 0 && showADCreadings[i] <= numberOfNets) {
    numReadings++;
    adcReading = readAdcVoltage(i, samples);
    color = measurementToColor(adcReading, adcRange[i][0], adcRange[i][1]);

    int brightness = LEDbrightnessSpecial + (int)abs(adcReading*5.0);//map(abs((adcReading*10)), 0, 80, -LEDbrightnessSpecial, 150);
    if (brightness <= 4) {
      brightness = 4;
    }

    if (displayMode == 0 || numberOfShownNets > MAX_NETS_FOR_WIRES) {
      lightUpNet(showADCreadings[i], -1, 1, brightness, 0, 0, color);
    }

    color = scaleBrightness(color, map(brightness, LEDbrightnessSpecial, LEDbrightnessSpecial+45, -90, 100));
    // Serial.println((color, map(brightness, LEDbrightnessSpecial, LEDbrightnessSpecial+45, -90, 100)));
    netColors[showADCreadings[i]] = unpackRgb(color);
    adcReadingColors[i] = color;


    net[showADCreadings[i]].color = unpackRgb(color);
     //drawWires(showADCreadings[0]);
     //showLEDsCore2 = 2;
  }
}


}


uint32_t measurementToColor(float measurement, float min, float max) {
  uint32_t color = 0;
  hsvColor hsv;
  int minInt = -80;
  int maxInt = 80;
  int measurementInt = measurement * 10;

  
  if (measurement < min) {
    measurement = min;
  } else if (measurement > max) {
    measurement = max;
  }


int shift = 228;
  hsv.h = map(measurementInt, minInt, maxInt, 210, 10);
  hsv.h += shift;
  hsv.s = 255;
 
  if (measurement < -0.7) {
    hsv.h += 10;
    
    if (measurement < -5.4) {
      hsv.s -= abs(measurement + 5.4) * 48;
      if (hsv.s < 0) {
        hsv.s = 0;
      }
      // Serial.println(measurement + 5.4);
      // Serial.println(hsv.s);
    } else {
      hsv.s = 230;
    }
    } else if (measurement > 0.5) {
    hsv.h += 239;

    if (measurement > 5.4) {
      hsv.s -= (measurement - 5.4) * 48;
      
      if (hsv.s < 0) {
        hsv.s = 0;
      }
    } 
  } else {
    //hsv.h += 254 - (measurement - 0.0) * 64;
  }
 // hsv.s = 255;

  hsv.h = hsv.h % 256;
  hsv.v = 255;
 // Serial.println(hsv.h);
  // int measurementInt = measurement * 10;
  // int distance = abs(0 - measurementInt);
  // hsv.v = map(distance, min*11, max*11, 0, 100);
  rgbColor rgb = HsvToRgb(hsv);
  color = packRgb(rgb);

  return color;
}

void showMeasurements(int samples, int printOrBB) {
  unsigned long startMillis = millis();
  int printInterval = 250;
  while (Serial.available() == 0 && Serial1.available() == 0 &&
         checkProbeButton() == 0)

  {

    Serial.print("\r                                                           "
                 "            \r");
    int adc0ReadingUnscaled;
    float adc0Reading;

    int adc1ReadingUnscaled;
    float adc1Reading;

    int adc2ReadingUnscaled;
    float adc2Reading;

    int adc3ReadingUnscaled;
    float adc3Reading;

    int adc4ReadingUnscaled;
    float adc4Reading;

    int adc7ReadingUnscaled;
    float adc7Reading;

    int bs = 0;

    if (showADCreadings[0] != 0) {
      //       if (readFloatingOrState(ADC0_PIN, -1) == floating)//this doesn't
      //       work because it's buffered
      // {

      //  bs += Serial.print("Floating  ");
      // }
      // adc0ReadingUnscaled = readAdc(0, samples);

      // adc0Reading = (adc0ReadingUnscaled) * (railSpread / 4095);
      // adc0Reading -= railSpread / 2; // offset
      adc0Reading = readAdcVoltage(0, samples);
      bs += Serial.print("ADC 0: ");
      bs += Serial.print(adc0Reading);
      bs += Serial.print("V\t");
      // int mappedAdc0Reading = map(adc0ReadingUnscaled, 0, 4095, -40, 40);
      // int hueShift = 0;

      // if (mappedAdc0Reading < 0) {
      //   hueShift = map(mappedAdc0Reading, -40, 0, 0, 200);
      //   mappedAdc0Reading = abs(mappedAdc0Reading);
      // }
    }
    if (showADCreadings[1] != 0) {
      //       if (readFloatingOrState(ADC0_PIN, -1) == floating)//this doesn't
      //       work because it's buffered
      // {

      //  bs += Serial.print("Floating  ");
      // }
      // adc1ReadingUnscaled = readAdc(1, samples);

      // adc1Reading = (adc1ReadingUnscaled) * (railSpread / 4095);
      // adc1Reading -= railSpread / 2; // offset

      adc1Reading = readAdcVoltage(1, samples);
      bs += Serial.print("ADC 1: ");
      bs += Serial.print(adc1Reading);
      bs += Serial.print("V\t");
      // int mappedAdc1Reading = map(adc1ReadingUnscaled, 0, 4095, -40, 40);
      // int hueShift = 0;

      // if (mappedAdc1Reading < 0) {
      //   hueShift = map(mappedAdc1Reading, -40, 0, 0, 200);
      //   mappedAdc1Reading = abs(mappedAdc1Reading);
      // }
    }

    if (showADCreadings[2] != 0) {

      adc2ReadingUnscaled = readAdc(2, samples);
      adc2Reading = (adc2ReadingUnscaled) * (railSpread / 4095);
      adc2Reading -= railSpread / 2; // offset
      bs += Serial.print("ADC 2: ");
      bs += Serial.print(adc2Reading);
      bs += Serial.print("V\t");
      int mappedAdc2Reading = map(adc2ReadingUnscaled, 0, 4095, -40, 40);
      int hueShift = 0;

      if (mappedAdc2Reading < 0) {
        hueShift = map(mappedAdc2Reading, -40, 0, 0, 200);
        mappedAdc2Reading = abs(mappedAdc2Reading);
      }
    }

    if (showADCreadings[3] != 0) {

      adc3ReadingUnscaled = readAdc(3, samples);
      adc3Reading = (adc3ReadingUnscaled) * (railSpread / 4095);
      adc3Reading -= railSpread / 2; // offset
      bs += Serial.print("ADC 3: ");
      bs += Serial.print(adc3Reading);
      bs += Serial.print("V\t");
      int mappedAdc3Reading = map(adc3ReadingUnscaled, 0, 4095, -40, 40);
      int hueShift = 0;

      if (mappedAdc3Reading < 0) {
        hueShift = map(mappedAdc3Reading, -40, 0, 0, 200);
        mappedAdc3Reading = abs(mappedAdc3Reading);
      }
    }

    if (showADCreadings[7] != 0) {

      adc7ReadingUnscaled = readAdc(7, samples);
      adc7Reading = (adc7ReadingUnscaled) * (railSpread / 4095);
      adc7Reading -= railSpread / 2; // offset
      bs += Serial.print("ADC 7: ");
      bs += Serial.print(adc7Reading);
      bs += Serial.print("V\t");
      int mappedAdc7Reading = map(adc7ReadingUnscaled, 0, 4095, -40, 40);
      int hueShift = 0;

      if (mappedAdc7Reading < 0) {
        hueShift = map(mappedAdc7Reading, -40, 0, 0, 200);
        mappedAdc7Reading = abs(mappedAdc7Reading);
      }
    }

    if (showADCreadings[4] != 0) {

      adc4ReadingUnscaled = readAdc(4, samples);
      adc4Reading = (adc4ReadingUnscaled) * (5.0 / 4095);
      // adc1Reading -= 0.1; // offset
      bs += Serial.print("ADC 4: ");
      bs += Serial.print(adc4Reading);
      bs += Serial.print("V\t");
    }

    // if (showINA0[0] == 1 || showINA0[1] == 1 || showINA0[2] == 1) {
    //   bs += Serial.print("   INA219: ");
    // }

    if (showINA0[0] == 1) {
      bs += Serial.print("INA 0: ");
      bs += Serial.print(INA0.getCurrent_mA());
      bs += Serial.print("mA\t");
      bs += Serial.print("\tINA 1: ");
      bs += Serial.print(INA1.getCurrent_mA());
      bs += Serial.print("mA\t");
    }

    if (showINA0[1] == 1) {
      bs += Serial.print(" V: ");
      bs += Serial.print(INA0.getBusVoltage());
      bs += Serial.print("V\t");
    }
    if (showINA0[2] == 1) {
      bs += Serial.print("P: ");
      bs += Serial.print(INA0.getPower_mW());
      bs += Serial.print("mW\t");
    }
    // Serial.print(digitalRead(buttonPin));
    bs += Serial.print("      \r");
    rotaryEncoderStuff();
    if (encoderButtonState != IDLE) {
      // showReadings = 0;
      return;
    }
    while (millis() - startMillis < printInterval &&
           (Serial.available() == 0 && checkProbeButton() == 0)) {

      showLEDmeasurements();
      delayMicroseconds(5000);
    }
    startMillis = millis();
  }
}

float readAdcVoltage(int channel, int samples) {
  // adcSpread[0] = 17.88;
  // adcSpread[1] = 17.88;
  // adcSpread[2] = 17.88;
  // adcSpread[3] = 17.88;
  // adcSpread[4] = 5.0;
  // adcSpread[5] = 17.88;
  // adcSpread[6] = 17.88;
  // adcSpread[7] = 17.88;
  if (channel < 0 || channel > 7) {
    return 0;
  }

  int adcReadingUnscaled = readAdc(channel, samples);

  float adcReading = (adcReadingUnscaled) * (adcSpread[channel] / 4095);
  adcReading -= adcSpread[channel] / 2; // offset

  // float adcReading = (adcReadingUnscaled) * (adcSpread[channel] / 4095);
  // adcReading -= adcZero[channel]; // offset
  // adcReading -= adcSpread[channel]/2; // offset

  return adcReading;
}

int readAdc(int channel, int samples) {
  int adcReadingAverage = 0;
  // if (channel == 0) { // I have no fucking idea why this works //future me:
  // the op amps were untamed

  //   pinMode(ADC1_PIN, OUTPUT);
  //   digitalWrite(ADC1_PIN, LOW);
  // }

  if (channel > 7) {
    return 0;
  }
  unsigned long timeoutTimer = micros();

  for (int i = 0; i < samples; i++) {
    if (micros() - timeoutTimer > 5000) {
      break;
    }
    adcReadingAverage += analogRead(ADC0_PIN + channel); //(int)adc_read();
    delayMicroseconds(25);
  }

  int adcReading = adcReadingAverage / samples;

  // float adc3Voltage = (adc3Reading - 2528) / 220.0; // painstakingly measured

  // if (channel == 0) {
  //   pinMode(ADC1_PIN, INPUT);
  // }
  // Serial.println(adcReading);
  return adcReading;
}

int waveGen(void) {
  // Serial.println("\n\n\rComing back soon in a less janky way!\n\r");
  // return 1;
  int loopCounter = 0;
  int c = 0;
  listSpecialNets();
  listNets();

  int activeDac = 3;

  mode[0] = 's';
  mode[1] = 's';
  mode[2] = 's';

  refillTable(amplitude[0], offset[0] + calib[0], 0);
  refillTable(amplitude[1], offset[1] + calib[1], 1);

  setDac0voltage((float)0.0);

  // setDac1voltage(offset[1] + calib[1]);
  setDac1voltage((float)0.0);
  digitalWrite(8, HIGH);
  // setDac1voltage(8190);
  // Serial.println(dac_rev3.getGain());

  // Serial.println(dac_rev3.maxValue());
  // Serial.print("Revision = ");
  // Serial.println(revisionNumber);
  Serial.println(
      "\n\r\t\t\t\t     waveGen\t\n\n\r\toptions\t\t\twaves\t\t\tadjust "
      "frequency\n\r");
  Serial.println(
      "\t5/0 = dac 0 0-5V (togg)\tq = square\t\t+ = frequency++\n\r");
  Serial.println("\t8/1 = dac 1 +-8V (togg)\ts = sine\t\t- = frequency--\n\r");
  Serial.println(
      "\ta = set amplitude (p-p)\tw = sawtooth\t\t* = frequency*2\n\r");
  Serial.println("\to = set offset\t\tt = triangle\t\t/ = frequency/2\n\r");
  Serial.println("\tv = voltage\t\tr = random\t\t \n\r");
  Serial.println("\th = show this menu\tx = exit\t\t \n\r");

  period[activeDac] = 1e6 / (freq[activeDac] / 10);
  halvePeriod[activeDac] = period[activeDac] / 2;
  int chars = 0;

  chars = 0;

  int firstCrossFreq0 = 0;
  int firstCrossFreq1 = 0;

  while (1) {
    yield();
    uint32_t now = micros();

    count++;

    // float adc3Voltage = (adc3Reading - 2528) / 220.0; //painstakingly
    // measured float adc0Voltage = ((adc0Reading) / 400.0) - 0.69; //- 0.93;
    // //painstakingly measured

    int adc0Reading = 0;
    int brightness0 = 0;
    int hueShift0 = 0;
    // firstCrossFreq0 = 1;

    if (dacOn[0] == 1 && freq[0] < 33) {
      // adc0Reading = INA1.getBusVoltage_mV();
      //  adc0Reading = dac0_5V.getInputCode();

      if (c == 'q') {
      } else {

        adc0Reading = readAdc(26, 1);
        adc0Reading = abs(adc0Reading);
        hueShift0 = map(adc0Reading, 0, 5000, -90, 0);
        brightness0 = map(adc0Reading, 0, 5000, 4, 100);

        // lightUpNet(4, -1, 1, brightness0, hueShift0);
        showLEDsCore2 = 1;
        firstCrossFreq0 = 1;
      }
    } else {
      if (firstCrossFreq0 == 1) {
        // lightUpNet(4);
        showLEDsCore2 = 1;
        firstCrossFreq0 = 0;
      }
    }

    int adc1Reading = 0;
    int brightness1 = 0;
    int hueShift1 = 0;

    if (dacOn[1] == 1 && freq[1] < 17) {
      adc1Reading = readAdc(29, 1);
      hueShift1 = map(adc1Reading, -2048, 2048, -50, 45);
      adc1Reading = adc1Reading - 2048;

      adc1Reading = abs(adc1Reading);

      brightness1 = map(adc1Reading, 0, 2050, 4, 100);

      // lightUpNet(5, -1, 1, brightness1, hueShift1);
      // showLEDsCore2 = 1;
      // showLEDmeasurements();
      firstCrossFreq1 = 1;
    } else {
      if (firstCrossFreq1 == 1) {
        // lightUpNet(5);
        // showLEDsCore2 = 1;
        firstCrossFreq1 = 0;
      }
    }

    if (now - lastTime > 100000) {
      loopCounter++;
      //
      // int adc0Reading = analogRead(26);
      // if (activeDac == 0)
      // {
      // for (int i = 0; i < (analogRead(27)/100); i++)
      // {
      // Serial.print('.');

      // }
      // Serial.println(' ');
      // }
      // else if (activeDac == 1)
      // {

      // for (int i = 0; i < (analogRead(29)/100); i++)
      // {
      // Serial.print('.');

      // }
      // Serial.println(' ');
      // }

      lastTime = now;
      // Serial.println(count); // show # updates per 0.1 second
      count = 0;

      if (Serial.available() == 0) {
        // break;
      } else {
        c = Serial.read();
        switch (c) {
        case '+':
          if (freq[activeDac] >= 1.0) {

            freq[activeDac]++;
          } else {
            freq[activeDac] += 0.1;
          }
          break;
        case '-':

          if (freq[activeDac] > 1.0) {
            freq[activeDac]--;
          } else if (freq[activeDac] > 0.1) {
            freq[activeDac] -= 0.1;
          } else {
            freq[activeDac] = 0.0;
          }

          break;
        case '*':
          freq[activeDac] *= 2;
          break;
        case '/':
          freq[activeDac] /= 2;
          break;
        case '8':
          if (activeDac == 0) {
            setDac0voltage((float)0.0);
            dacOn[0] = 0;
          }

          if (activeDac != 3)
            dacOn[1] = !dacOn[1];

          activeDac = 1;

          if (dacOn[1] == 0) {
            setDac1voltage((uint16_t)(offset[1] + calib[1]));
          }

          break;
        case '5':
          if (activeDac == 1) {
            setDac1voltage((uint16_t)(offset[1] + calib[1]));
            dacOn[1] = 0;
          }

          if (activeDac != 3)
            dacOn[0] = !dacOn[0];

          activeDac = 0;
          if (dacOn[activeDac] == 0) {
            setDac0voltage((float)0.0);
          }
          break;
        case 'c':
          // freq[activeDac] = 0;
          break;

        case 's': {
          if (mode[2] == 'v') {
            refillTable(amplitude[activeDac], offset[activeDac] + calib[1], 1);
            mode[2] = 's';
          }

          mode[activeDac] = c;
          break;
        }

        case 'q':

        case 'w':
        case 't':
        case 'r':
        case 'z':
        case 'm':

          //
          mode[2] = mode[activeDac];
          mode[activeDac] = c;
          break;

        case 'v':
        case 'h':
        case 'a':
        case 'o':

          mode[2] = mode[activeDac];

          mode[activeDac] = c;
          break;
        case '{':
        case 'f': {
          if (mode[0] != 'v') {
            setDac0voltage((float)0.0);
          }
          if (mode[1] != 'v') {
            setDac1voltage((uint16_t)offset[1]);
          }
          return 0;
        }

        case 'x': {
          if (mode[0] != 'v') {
            setDac0voltage((float)0.0);
          }
          if (mode[1] != 'v') {

            setDac1voltage((uint16_t)offset[1]);
          }

          return 1;
        }
        default:
          break;
        }
        period[activeDac] = 1e6 / freq[activeDac];
        halvePeriod[activeDac] = period[activeDac] / 2;
        if (activeDac == 0) {
          Serial.print("dac 0:   ");
          Serial.print("ampl: ");
          Serial.print((float)(amplitude[activeDac]) / 819);
          Serial.print("V\t");
          Serial.print("offset: ");
          Serial.print((float)(offset[activeDac]) / 819);
          Serial.print("V\t\t");
          Serial.print("mode: ");
          Serial.print(mode[activeDac]);
          Serial.print("\t\t");
          Serial.print("freq: ");
          Serial.print(freq[activeDac]);

          // Serial.print("\t\n\r");
        } else if (activeDac == 1) {
          Serial.print("dac 1:   ");
          Serial.print("ampl: ");
          Serial.print((float)(amplitude[activeDac]) / 276);
          Serial.print("V\t");
          Serial.print("offset: ");
          Serial.print(((float)(offset[activeDac]) / 276) - 7);
          Serial.print("V\t\t");
          Serial.print("mode: ");
          Serial.print(mode[activeDac]);
          Serial.print("\t\t");
          Serial.print("freq: ");
          Serial.print(freq[activeDac]);
          // Serial.print("\t\n\r");
        }
        /*
        Serial.print("\tdacon");
        for (int i = 0; i < 3; i++)
        {
          Serial.print("\t");

          Serial.print(dacOn[i]);
        }*/
        Serial.println();
      }
    }

    uint32_t t = now % period[activeDac];
    // if (dacOn[activeDac] == 1 )
    //{
    switch (mode[activeDac]) {
    case 'q':
      if (t < halvePeriod[activeDac]) {
        if (activeDac == 0 && dacOn[activeDac] == 1) {
          setDac0voltage((uint16_t)amplitude[activeDac]);
          lightUpNet(4, -1, 1, DEFAULTSPECIALNETBRIGHTNESS, 12);
          showLEDsCore2 = 1;
        } else if (activeDac == 1 && dacOn[activeDac] == 1) {
          setDac1voltage((uint16_t)amplitude[activeDac]);

          showLEDsCore2 = 1;
        }
      } else {
        if (activeDac == 0 && dacOn[activeDac] == 1) {
          setDac0voltage((float)0.0);
          lightUpNet(4, -1, 1, 2, 12);
        }

        else if (activeDac == 1 && dacOn[activeDac] == 1) {
          setDac1voltage((uint16_t)offset[activeDac]);
        }
      }
      break;
    case 'w':
      if (activeDac == 0 && dacOn[activeDac] == 1)
        setDac0voltage(
            (uint16_t)(t * amplitude[activeDac] / period[activeDac]));
      else if (activeDac == 1 && dacOn[activeDac] == 1)
        setDac1voltage(
            (uint16_t)(t * amplitude[activeDac] / period[activeDac]));
      break;
    case 't':
      if (activeDac == 0 && dacOn[activeDac] == 1) {

        if (t < halvePeriod[activeDac])
          setDac0voltage(
              (uint16_t)((t * amplitude[activeDac]) / halvePeriod[activeDac]));
        else
          setDac0voltage(
              (uint16_t)(((period[activeDac] - t) * (amplitude[activeDac]) /
                          halvePeriod[activeDac])));
      } else if (activeDac == 1 && dacOn[activeDac] == 1) {
        if (t < halvePeriod[activeDac])
          setDac1voltage(
              (uint16_t)(t * amplitude[activeDac] / halvePeriod[activeDac]));
        else
          setDac1voltage(
              (uint16_t)((period[activeDac] - t) * amplitude[activeDac] /
                         halvePeriod[activeDac]));
      }
      break;
    case 'r':
      if (activeDac == 0 && dacOn[activeDac] == 1)
        setDac0voltage((uint16_t)random(amplitude[activeDac]));
      else if (activeDac == 1 && dacOn[activeDac] == 1) {
        setDac1voltage((uint16_t)random(amplitude[activeDac]));
      }
      break;
    case 'z': // zero
      if (activeDac == 0)
        setDac0voltage((float)0.0);
      else if (activeDac == 1)
        setDac1voltage((uint16_t)(offset[activeDac]));
      break;
    case 'h': // high
      Serial.println(
          "\n\r\t\t\t\t     waveGen\t\n\n\r\toptions\t\t\twaves\t\t\tadjust "
          "frequency\n\r");
      Serial.println(
          "\t5/0 = dac 0 0-5V (togg)\tq = square\t\t+ = frequency++\n\r");
      Serial.println(
          "\t8/1 = dac 1 +-8V (togg)\ts = sine\t\t- = frequency--\n\r");
      Serial.println(
          "\ta = set amplitude (p-p)\tw = sawtooth\t\t* = frequency*2\n\r");
      Serial.println("\to = set offset\t\tt = triangle\t\t/ = frequency/2\n\r");
      Serial.println("\tv = voltage\t\tr = random\t\t \n\r");
      Serial.println("\th = show this menu\tx = exit\t\t \n\r");
      mode[activeDac] = mode[2];
      break;
    case 'm': // mid
      // setDac1voltage(2047);
      break;
    case 'a': {
      float newAmplitudeF = 0;
      int newAmplitude = 0;
      int input = 0;
      char aC = 0;
      int a = 0;
      if (activeDac == 0) {

        Serial.print("\n\renter amplitude (0-5): ");
        while (Serial.available() == 0)
          ;
        aC = Serial.read();
        if (aC == 'a')
          aC = Serial.read();
        a = aC;

        Serial.print(aC);

        if (a >= 48 && a <= 53) {

          input = a - 48;
          newAmplitude = input * 819;
          unsigned long timeoutTimer = micros();
          Serial.print(".");
          while (Serial.available() == 0) {
            if (micros() - timeoutTimer > 5000) {
              a = '0';
              break;
            }
          }

          a = Serial.read();

          if (a == '.') {

            while (Serial.available() == 0) {
              if (micros() - timeoutTimer > 5000) {
                break;
              }
            }

            a = Serial.read();
          }

          if (a >= 48 && a <= 57) {
            Serial.print((char)a);
            input = a - 48;
            newAmplitude += input * 81.9;

            amplitude[activeDac] = newAmplitude;
            Serial.print("\tamplitude: ");
            Serial.print((float)(amplitude[activeDac]) / 819);
            Serial.println("V");
            if ((offset[activeDac] - (amplitude[activeDac] / 2)) < 10 ||
                ((amplitude[activeDac] / 2) - offset[activeDac]) < 10) {
              offset[activeDac] = (amplitude[activeDac] / 2) - 1;
            }
            {
              offset[activeDac] = (amplitude[activeDac] / 2) - 2;
            }
            refillTable(amplitude[activeDac], offset[activeDac], 0);

            mode[activeDac] = mode[2];
            mode[2] = '0';
            break;
          }
        }
      } else if (activeDac == 1) {

        Serial.print("\n\renter peak amplitude (0-7.5): ");
        while (Serial.available() == 0)
          ;
        aC = Serial.read();
        if (aC == 'o')
          aC = Serial.read();
        a = aC;

        Serial.print(aC);

        if (a >= 48 && a <= 55) {

          input = a - 48;
          newAmplitude = input * 276;
          Serial.print(".");
          while (Serial.available() == 0)
            ;
          a = Serial.read();

          if (a == '.') {
            while (Serial.available() == 0)
              ;

            a = Serial.read();
          }

          if (a >= 48 && a <= 57) {
            Serial.print((char)a);
            input = a - 48;
            newAmplitude += input * 27.6;
            // newAmplitude *= 2;

            amplitude[activeDac] = newAmplitude;
            Serial.print("\tamplitude: ");
            Serial.print((amplitude[activeDac]));
            Serial.println(" ");
            Serial.print((float)(amplitude[activeDac]) / 276);
            Serial.println("V");

            refillTable(amplitude[activeDac], offset[activeDac] + calib[1], 1);

            mode[activeDac] = mode[2];
            mode[2] = '0';
            break;
          }
        }
      }
    }
    case 'o': {

      int newOffset = 0;
      int input = 0;
      char aC = 0;
      int o = 0;
      if (activeDac == 0) {

        Serial.print("\n\renter offset (0-5): ");
        while (Serial.available() == 0)
          ;
        aC = Serial.read();
        if (aC == 'o')
          aC = Serial.read();

        o = aC;

        Serial.print(aC);

        if (o >= 48 && o <= 53) {

          input = o - 48;
          newOffset = input * 819;
          Serial.print(".");
          while (Serial.available() == 0)
            ;
          o = Serial.read();

          if (o == '.') {
            while (Serial.available() == 0)
              ;

            o = Serial.read();
          }

          if (o >= 48 && o <= 57) {
            Serial.print((char)o);
            input = o - 48;
            newOffset += input * 81.9;

            offset[activeDac] = newOffset;
            Serial.print("\toffset: ");
            Serial.print((float)(offset[activeDac]) / 819);
            Serial.println("V");

            refillTable(amplitude[activeDac], offset[activeDac], 0);

            mode[activeDac] = mode[2];
            break;
          }
        }
      } else if (activeDac == 1) {
        int negative = 0;

        Serial.print("\n\rEnter offset (-7 - 7): ");
        while (Serial.available() == 0)
          ;
        aC = Serial.read();
        if (aC == '-') {
          Serial.print('-');
          negative = 1;
          while (Serial.available() == 0)
            ;
          aC = Serial.read();
        }

        o = aC;

        Serial.print(aC);

        if (o >= 48 && o <= 55) {

          input = o - 48;
          newOffset = input * 276;

          if (input == '7') {
            Serial.print(".00");
          } else {

            Serial.print(".");
            while (Serial.available() == 0)
              ;
            o = Serial.read();
            if (o == '.') {
              while (Serial.available() == 0)
                ;
              o = Serial.read();
            }

            if (o >= 48 && o <= 57) {
              Serial.print((char)o);
              input = o - 48;
              newOffset += input * 27.6;
            }
          }

          if (negative == 1)
            newOffset *= -1;

          newOffset += (7 * 276);

          offset[activeDac] = newOffset;
          Serial.print("\toffset: ");
          Serial.print(((float)(offset[activeDac]) / 276) - 7);
          Serial.print("  ");
          Serial.print(offset[activeDac]);
          Serial.println("V");

          refillTable(amplitude[activeDac], offset[activeDac] + calib[1], 1);

          mode[activeDac] = mode[2];
          break;
        }
      }
    }
    case 'v': {
      if (activeDac == 0 && mode[2] != 'v') {
        // freq[activeDac] = 0;
        setDac0voltage((uint16_t)(amplitude[activeDac] / 819));
        mode[2] = 'v';
      } else if (activeDac == 1 && mode[2] != 'v') {
        // freq[activeDac] = 0;
        // refillTable(0, offset[activeDac] + calib[1], 1);
        setDac1voltage((uint16_t)(((amplitude[activeDac] + calib[1]) / 276) -
                                  ((offset[activeDac] / 276) - 7)));
        mode[2] = 'v';
      } else if (mode[2] == 'v') {
        // mode[2] = 's';
      }

      break;
    }

    default:
    case 's':
      // reference
      // float f = ((PI * 2) * t)/period;
      // setDac1voltage(2047 + 2047 * sin(f));
      //
      if (mode[activeDac] != 'v') {
        int idx = (360 * t) / period[activeDac];
        if (activeDac == 0 && dacOn[activeDac] == 1)
          setDac0voltage(sine0[idx]); // lookuptable
        else if (activeDac == 1 && dacOn[activeDac] == 1)
          setDac1voltage(sine1[idx]); // lookuptable
      }
      break;
    }
  }
}

void refillTable(int amplitude, int offset, int dac) {
  // int offsetCorr = 0;
  if (dac == 0) {
    // offset = amplitude / 2;
  }

  for (int i = 0; i < 360; i++) {
    if (dac == 0) {
      sine0[i] = offset + round(amplitude / 2 * sin(i * PI / 180));
    } else if (dac == 1) {
      sine1[i] =
          offset + round((amplitude - (offset - 2047)) / 2 * sin(i * PI / 180));
    } else if (dac == 2) {
      sine0[i] = offset + round(amplitude / 2 * sin(i * PI / 180));
      sine1[i] = offset + round(amplitude / 2 * sin(i * PI / 180));
    }
  }
}
// void GetAdc29Status(int i) {
//   gpio_function gpio29Function = gpio_get_function(29);
//   Serial.print("GPIO29 func: ");
//   Serial.println(gpio29Function);

//   bool pd = gpio_is_pulled_down(29);
//   Serial.print("GPIO29 pd: ");
//   Serial.println(pd);

//   bool h = gpio_is_input_hysteresis_enabled(29);
//   Serial.print("GPIO29 h: ");
//   Serial.println(h);

//   gpio_slew_rate slew = gpio_get_slew_rate(29);
//   Serial.print("GPIO29 slew: ");
//   Serial.println(slew);

//   gpio_drive_strength drive = gpio_get_drive_strength(29);
//   Serial.print("GPIO29 drive: ");
//   Serial.println(drive);

//   int irqmask = gpio_get_irq_event_mask(29);
//   Serial.print("GPIO29 irqmask: ");
//   Serial.println(irqmask);

//   bool out = gpio_is_dir_out(29);
//   Serial.print("GPIO29 out: ");
//   Serial.println(out);
//   Serial.printf("(%i) GPIO29 func: %i, pd: %i, h: %i, slew: %i, drive: %i, "
//                 "irqmask: %i, out: %i\n",
//                 i, gpio29Function, pd, h, slew, drive, irqmask, out);
// }
