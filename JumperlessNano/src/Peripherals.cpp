

#include "Peripherals.h"

#include "Adafruit_INA219.h"

#include "NetManager.h"

#include "LEDs.h"

#include <Arduino.h>
#include "hardware/adc.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "ADCInput.h"
#include "pico/cyw43_arch.h"

#include "mcp4725.hpp"
#include "MCP_DAC.h"

#include <SPI.h>

#define DAC_RESOLUTION 9

int revisionNumber = 0;

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
int amplitude[3] = {4095, 3763, 0};
int offset[3] = {2047, 2380, 2047};
int calib[3] = {-10, 0, 0};

MCP4725_PICO dac0_5V(5.0);
MCP4725_PICO dac1_8V(18.0);

MCP4822 dac_rev3; // A is dac0  B is dac1

INA219 INA0(0x40);
INA219 INA1(0x41);

uint16_t count;
uint32_t lastTime = 0;

// LOOKUP TABLE SINE
uint16_t sine0[360];
uint16_t sine1[360];

// ADCInput adc(A3);

void initADC(void)
{

  pinMode(ADC0_PIN, INPUT);
  pinMode(ADC1_PIN, INPUT);
  pinMode(ADC2_PIN, INPUT);
  pinMode(ADC3_PIN, INPUT);

  gpio_set_function(ADC3_PIN, GPIO_FUNC_NULL);
  adc_gpio_init(ADC3_PIN);
  adc_select_input(3);

  adc_fifo_setup(true, false, 0, false, false);
  adc_run(true);
  analogReadResolution(12);
}

void initDAC(void)
{

   Wire.begin();
  if (dac1_8V.begin(MCP4725A1_Addr_A01, i2c0, 400, 4, 5) == true)
  {
    revisionNumber = 2;

    amplitude[1] = 3500;

    offset[1] = 1932;

    calib[1] = 150;

    dac0_5V.begin(MCP4725A1_Addr_A00, i2c0, 400, 4, 5);

    //
    delay(1);
    dac0_5V.setVoltage(0.00, MCP4725_EEPROM_Mode, MCP4725_PowerDown_Off);
    delay(1);
    dac0_5V.setVoltage(0.00);
    // setDac0_5VinputCode(0,MCP4725_EEPROM_Mode,MCP4725_PowerDown_Off);
    delay(1);
    dac1_8V.setInputCode(offset[1] + calib[1], MCP4725_EEPROM_Mode, MCP4725_PowerDown_Off);
    delay(1);
    dac1_8V.setInputCode(offset[1] + calib[1]);
  }
  else
  {

    revisionNumber = 3;
    // dac_rev3.begin();

    // setRX(pin_size_t pin);
    SPI.setRX(0);
    SPI.setCS(1);
    SPI.setSCK(2);
    SPI.setTX(3);
    
    SPI.begin();
     //dac_rev3.maxValue = 4095;
     delay(5);
    dac_rev3.setGain(2);
     delay(5);
    dac_rev3.begin(1);

    delay(5);
    setDac0_5Vvoltage(0.00);
   // setDac0_5VinputCode(4095);

    setDac1_8VinputCode(4095);
  }
}

void initINA219(void)
{

  // delay(3000);
  // Serial.println(__FILE__);
  // Serial.print("INA219_LIB_VERSION: ");
  // Serial.println(INA219_LIB_VERSION);

  // Wire.begin();
  // Wire.setClock(1000000);
  Wire.begin();

  if (!INA0.begin() || !INA1.begin())
  {
    Serial.println("Failed to find INA219 chip");
  }

  INA0.setMaxCurrentShunt(1, 2);
  INA1.setMaxCurrentShunt(1, 2);

  Serial.println(INA0.setBusVoltageRange(16));
  Serial.println(INA1.setBusVoltageRange(16));
}

void dacSine(int resolution)
{
  uint16_t i;
  switch (resolution)
  {
  case 0 ... 5:
    for (i = 0; i < 32; i++)
    {
      setDac1_8VinputCode(DACLookup_FullSine_5Bit[i]);
    }
    break;
  case 6:
    for (i = 0; i < 64; i++)
    {
      setDac1_8VinputCode(DACLookup_FullSine_6Bit[i]);
    }
    break;
  case 7:
    for (i = 0; i < 128; i++)
    {
      setDac1_8VinputCode(DACLookup_FullSine_7Bit[i]);
    }
    break;

  case 8:
    for (i = 0; i < 256; i++)
    {
      setDac1_8VinputCode(DACLookup_FullSine_8Bit[i]);
    }
    break;

  case 9 ... 12:
    for (i = 0; i < 512; i++)
    {
      setDac1_8VinputCode(DACLookup_FullSine_9Bit[i]);
    }
    break;
  }
}

uint16_t lastInputCode0 = 0;
uint16_t lastInputCode1 = offset[1] + calib[1];

void setDac0_5Vvoltage(float voltage)
{
  if (revisionNumber == 2)
  {
    dac0_5V.setVoltage(voltage);
  }
  else
  {
    int voltageCode = voltage * 4095 / 5;

    
    // dac_rev3.analogWrite((uint16_t)voltageCode, 0);
    dac_rev3.fastWriteA((uint16_t)voltageCode);
    lastInputCode0 = voltageCode;
    //dac_rev3.fastWriteB(lastInputCode1);
  }
}

void setDac0_5VinputCode(uint16_t inputCode)
{
  if (revisionNumber == 2)
  {
    dac0_5V.setInputCode(inputCode);
  }
  else
  {
    dac_rev3.analogWrite(inputCode, 0);
    dac_rev3.fastWriteA(inputCode);
    lastInputCode0 = inputCode;
    //dac_rev3.fastWriteB(lastInputCode1);
  }
}

void setDac1_8Vvoltage(float voltage)
{
  if (revisionNumber == 2)
  {
    dac1_8V.setVoltage(voltage);
  }
  else
  {

    int voltageCode = voltage * 4095 / 16;
    voltageCode = voltageCode + 2048;

    // dac_rev3.analogWrite((uint16_t)voltageCode, 1);
    dac_rev3.fastWriteB((uint16_t)voltageCode);
    ///lastInputCode1 = voltageCode;
   // dac_rev3.fastWriteA(lastInputCode0);
  }
}

void setDac1_8VinputCode(uint16_t inputCode)
{
  if (revisionNumber == 2)
  {
    dac1_8V.setInputCode(inputCode);
  }
  else
  {

    // Serial.println(inputCode);
    // dac_rev3.analogWrite(inputCode, 1);
    dac_rev3.fastWriteB(inputCode);
    //lastInputCode1 = inputCode;
   // dac_rev3.fastWriteA(lastInputCode0);
  }
}

int readAdc(int channel, int samples)
{
  int adcReadingAverage = 0;
  for (int i = 0; i < samples; i++)
  {
    adcReadingAverage += analogRead(channel);
    delay(1);
  }

  int adc3Reading = adcReadingAverage / samples;
  // Serial.print(adc3Reading);

  // float adc3Voltage = (adc3Reading - 2528) / 220.0; // painstakingly measured
  return adc3Reading;
}

int waveGen(void)
{
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

  setDac0_5Vvoltage(0.0);

  setDac1_8VinputCode(offset[1] + calib[1]);
  // setDac1_8VinputCode(8190);
  // Serial.println(dac_rev3.getGain());

  Serial.println(dac_rev3.maxValue());
  Serial.print("Revision = ");
  Serial.println(revisionNumber);
  Serial.println("\n\r\t\t\t\t     waveGen\t\n\n\r\toptions\t\t\twaves\t\t\tadjust frequency\n\r");
  Serial.println("\t5/0 = dac 0 0-5V (togg)\tq = square\t\t+ = frequency++\n\r");
  Serial.println("\t8/1 = dac 1 +-8V (togg)\ts = sine\t\t- = frequency--\n\r");
  Serial.println("\ta = set amplitude (p-p)\tw = sawtooth\t\t* = frequency*2\n\r");
  Serial.println("\to = set offset\t\tt = triangle\t\t/ = frequency/2\n\r");
  Serial.println("\tv = voltage\t\tr = random\t\t \n\r");
  Serial.println("\th = show this menu\tx = exit\t\t \n\r");

  period[activeDac] = 1e6 / (freq[activeDac] / 10);
  halvePeriod[activeDac] = period[activeDac] / 2;
  int chars = 0;

  chars = 0;

  int firstCrossFreq0 = 0;
  int firstCrossFreq1 = 0;

  while (1)
  {
    yield();
    uint32_t now = micros();

    count++;

    // float adc3Voltage = (adc3Reading - 2528) / 220.0; //painstakingly measured
    // float adc0Voltage = ((adc0Reading) / 400.0) - 0.69; //- 0.93; //painstakingly measured

    int adc0Reading = 0;
    int brightness0 = 0;
    int hueShift0 = 0;
    //firstCrossFreq0 = 1;

    if (dacOn[0] == 1&& freq[0] < 33)
    {
      //adc0Reading = INA1.getBusVoltage_mV();
      // adc0Reading = dac0_5V.getInputCode();

      if (c == 'q')
      {
      } else {

      adc0Reading = readAdc(26, 1);
      adc0Reading = abs(adc0Reading);
      hueShift0 = map(adc0Reading, 0, 5000, -90, 0);
      brightness0 = map(adc0Reading, 0, 5000, 4, 100);

      lightUpNet(4, -1, 1, brightness0, hueShift0);
      showLEDsCore2 = 1;
      firstCrossFreq0 = 1;

      }
    }
    else
    {
      if (firstCrossFreq0 == 1)
      {
      lightUpNet(4);
      showLEDsCore2 = 1;
      firstCrossFreq0 = 0;
      }

     
    }

    int adc1Reading = 0;
    int brightness1 = 0;
    int hueShift1 = 0;

    if (dacOn[1] == 1 && freq[1] < 17)
    {
      adc1Reading = readAdc(29, 1);
        hueShift1 = map(adc1Reading, -2048, 2048, -50, 45);
        adc1Reading = adc1Reading - 2048;

        adc1Reading = abs(adc1Reading);

        brightness1 = map(adc1Reading, 0, 2050, 4, 100);



      lightUpNet(5, -1, 1, brightness1, hueShift1);
      showLEDsCore2 = 1;
      firstCrossFreq1 = 1;
    }
    else
    {
      if (firstCrossFreq1 == 1)
      {
      lightUpNet(5);
      showLEDsCore2 = 1;
      firstCrossFreq1 = 0;
      }
    }

    if (now - lastTime > 100000)
    {
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

      if (Serial.available() == 0)
      {
        // break;
      }
      else
      {
        c = Serial.read();
        switch (c)
        {
        case '+':
          if (freq[activeDac] >= 1.0)
          {

            freq[activeDac]++;
          }
          else
          {
            freq[activeDac] += 0.1;
          }
          break;
        case '-':

          if (freq[activeDac] > 1.0)
          {
            freq[activeDac]--;
          }
          else if (freq[activeDac] > 0.1)
          {
            freq[activeDac] -= 0.1;
          }
          else
          {
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
          if (activeDac == 0)
          {
            setDac0_5Vvoltage(0.0);
            dacOn[0] = 0;
          }

          if (activeDac != 3)
            dacOn[1] = !dacOn[1];

          activeDac = 1;

          if (dacOn[1] == 0)
          {
            setDac1_8VinputCode(offset[1] + calib[1]);
          }

          break;
        case '5':
          if (activeDac == 1)
          {
            setDac1_8VinputCode(offset[1] + calib[1]);
            dacOn[1] = 0;
          }

          if (activeDac != 3)
            dacOn[0] = !dacOn[0];

          activeDac = 0;
          if (dacOn[activeDac] == 0)
          {
            setDac0_5Vvoltage(0.0);
          }
          break;
        case 'c':
          // freq[activeDac] = 0;
          break;

        case 's':
        {
          if (mode[2] == 'v')
          {
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
        case 'f':
        {
          if (mode[0] != 'v')
          {
            setDac0_5Vvoltage(0.0);
          }
          if (mode[1] != 'v')
          {
            setDac1_8VinputCode(offset[1]);
          }
          return 0;
        }

        case 'x':
        {
          if (mode[0] != 'v')
          {
            setDac0_5Vvoltage(0.0);
          }
          if (mode[1] != 'v')
          {

            setDac1_8VinputCode(offset[1]);
          }

          return 1;
        }
        default:
          break;
        }
        period[activeDac] = 1e6 / freq[activeDac];
        halvePeriod[activeDac] = period[activeDac] / 2;
        if (activeDac == 0)
        {
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
        }
        else if (activeDac == 1)
        {
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
    switch (mode[activeDac])
    {
    case 'q':
      if (t < halvePeriod[activeDac])
      {
        if (activeDac == 0 && dacOn[activeDac] == 1)
        {
          setDac0_5VinputCode(amplitude[activeDac]);
          lightUpNet(4, -1, 1, DEFAULTSPECIALNETBRIGHTNESS, 12);
          showLEDsCore2 = 1;
      }
        else if (activeDac == 1 && dacOn[activeDac] == 1)
        {
          setDac1_8VinputCode(amplitude[activeDac]);
          
          showLEDsCore2 = 1;
        }

      }
      else
      {
        if (activeDac == 0 && dacOn[activeDac] == 1)
        {
          setDac0_5VinputCode(0);
lightUpNet(4, -1, 1, 2, 12);
      }

        else if (activeDac == 1 && dacOn[activeDac] == 1)
        {
          setDac1_8VinputCode(offset[activeDac]);
        }
      }
      break;
    case 'w':
      if (activeDac == 0 && dacOn[activeDac] == 1)
        setDac0_5VinputCode(t * amplitude[activeDac] / period[activeDac]);
      else if (activeDac == 1 && dacOn[activeDac] == 1)
        setDac1_8VinputCode(t * amplitude[activeDac] / period[activeDac]);
      break;
    case 't':
      if (activeDac == 0 && dacOn[activeDac] == 1)
      {

        if (t < halvePeriod[activeDac])
          setDac0_5VinputCode(((t * amplitude[activeDac]) / halvePeriod[activeDac]));
        else
          setDac0_5VinputCode((((period[activeDac] - t) * (amplitude[activeDac]) / halvePeriod[activeDac])));
      }
      else if (activeDac == 1 && dacOn[activeDac] == 1)
      {
        if (t < halvePeriod[activeDac])
          setDac1_8VinputCode(t * amplitude[activeDac] / halvePeriod[activeDac]);
        else
          setDac1_8VinputCode((period[activeDac] - t) * amplitude[activeDac] / halvePeriod[activeDac]);
      }
      break;
    case 'r':
      if (activeDac == 0 && dacOn[activeDac] == 1)
        setDac0_5VinputCode(random(amplitude[activeDac]));
      else if (activeDac == 1 && dacOn[activeDac] == 1)
      {
        setDac1_8VinputCode(random(amplitude[activeDac]));
      }
      break;
    case 'z': // zero
      if (activeDac == 0)
        setDac0_5Vvoltage(0);
      else if (activeDac == 1)
        setDac1_8VinputCode(offset[activeDac]);
      break;
    case 'h': // high
      Serial.println("\n\r\t\t\t\t     waveGen\t\n\n\r\toptions\t\t\twaves\t\t\tadjust frequency\n\r");
      Serial.println("\t5/0 = dac 0 0-5V (togg)\tq = square\t\t+ = frequency++\n\r");
      Serial.println("\t8/1 = dac 1 +-8V (togg)\ts = sine\t\t- = frequency--\n\r");
      Serial.println("\ta = set amplitude (p-p)\tw = sawtooth\t\t* = frequency*2\n\r");
      Serial.println("\to = set offset\t\tt = triangle\t\t/ = frequency/2\n\r");
      Serial.println("\tv = voltage\t\tr = random\t\t \n\r");
      Serial.println("\th = show this menu\tx = exit\t\t \n\r");
      mode[activeDac] = mode[2];
      break;
    case 'm': // mid
      // setDac1_8VinputCode(2047);
      break;
    case 'a':
    {
      float newAmplitudeF = 0;
      int newAmplitude = 0;
      int input = 0;
      char aC = 0;
      int a = 0;
      if (activeDac == 0)
      {

        Serial.print("\n\renter amplitude (0-5): ");
        while (Serial.available() == 0)
          ;
        aC = Serial.read();
        if (aC == 'a')
          aC = Serial.read();
        a = aC;

        Serial.print(aC);

        if (a >= 48 && a <= 53)
        {

          input = a - 48;
          newAmplitude = input * 819;
          Serial.print(".");
          while (Serial.available() == 0)
            ;
          a = Serial.read();

          if (a == '.')
          {
            while (Serial.available() == 0)
              ;

            a = Serial.read();
          }

          if (a >= 48 && a <= 57)
          {
            Serial.print((char)a);
            input = a - 48;
            newAmplitude += input * 81.9;

            amplitude[activeDac] = newAmplitude;
            Serial.print("\tamplitude: ");
            Serial.print((float)(amplitude[activeDac]) / 819);
            Serial.println("V");
            if ((offset[activeDac] - (amplitude[activeDac] / 2)) < 10 || ((amplitude[activeDac] / 2) - offset[activeDac]) < 10)
            {
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
      }
      else if (activeDac == 1)
      {

        Serial.print("\n\renter peak amplitude (0-7.5): ");
        while (Serial.available() == 0)
          ;
        aC = Serial.read();
        if (aC == 'o')
          aC = Serial.read();
        a = aC;

        Serial.print(aC);

        if (a >= 48 && a <= 55)
        {

          input = a - 48;
          newAmplitude = input * 276;
          Serial.print(".");
          while (Serial.available() == 0)
            ;
          a = Serial.read();

          if (a == '.')
          {
            while (Serial.available() == 0)
              ;

            a = Serial.read();
          }

          if (a >= 48 && a <= 57)
          {
            Serial.print((char)a);
            input = a - 48;
            newAmplitude += input * 27.6;
            newAmplitude *= 2;

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
    case 'o':
    {

      int newOffset = 0;
      int input = 0;
      char aC = 0;
      int o = 0;
      if (activeDac == 0)
      {

        Serial.print("\n\renter offset (0-5): ");
        while (Serial.available() == 0)
          ;
        aC = Serial.read();
        if (aC == 'o')
          aC = Serial.read();

        o = aC;

        Serial.print(aC);

        if (o >= 48 && o <= 53)
        {

          input = o - 48;
          newOffset = input * 819;
          Serial.print(".");
          while (Serial.available() == 0)
            ;
          o = Serial.read();

          if (o == '.')
          {
            while (Serial.available() == 0)
              ;

            o = Serial.read();
          }

          if (o >= 48 && o <= 57)
          {
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
      }
      else if (activeDac == 1)
      {
        int negative = 0;

        Serial.print("\n\rEnter offset (-7 - 7): ");
        while (Serial.available() == 0)
          ;
        aC = Serial.read();
        if (aC == '-')
        {
          Serial.print('-');
          negative = 1;
          while (Serial.available() == 0)
            ;
          aC = Serial.read();
        }

        o = aC;

        Serial.print(aC);

        if (o >= 48 && o <= 55)
        {

          input = o - 48;
          newOffset = input * 276;

          if (input == '7')
          {
            Serial.print(".00");
          }
          else
          {

            Serial.print(".");
            while (Serial.available() == 0)
              ;
            o = Serial.read();
            if (o == '.')
            {
              while (Serial.available() == 0)
                ;
              o = Serial.read();
            }

            if (o >= 48 && o <= 57)
            {
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
    case 'v':
    {
      if (activeDac == 0 && mode[2] != 'v')
      {
        // freq[activeDac] = 0;
        setDac0_5Vvoltage(amplitude[activeDac] / 819);
        mode[2] = 'v';
      }
      else if (activeDac == 1 && mode[2] != 'v')
      {
        // freq[activeDac] = 0;
        // refillTable(0, offset[activeDac] + calib[1], 1);
        setDac1_8Vvoltage(((amplitude[activeDac] + calib[1]) / 276) - ((offset[activeDac] / 276) - 7));
        mode[2] = 'v';
      }
      else if (mode[2] == 'v')
      {
        // mode[2] = 's';
      }

      break;
    }

    default:
    case 's':
      // reference
      // float f = ((PI * 2) * t)/period;
      // setDac1_8VinputCode(2047 + 2047 * sin(f));
      //
      if (mode[activeDac] != 'v')
      {
        int idx = (360 * t) / period[activeDac];
        if (activeDac == 0 && dacOn[activeDac] == 1)
          setDac0_5VinputCode(sine0[idx]); // lookuptable
        else if (activeDac == 1 && dacOn[activeDac] == 1)
          setDac1_8VinputCode(sine1[idx]); // lookuptable
      }
      break;
    }
  }
}

void refillTable(int amplitude, int offset, int dac)
{
  // int offsetCorr = 0;
  if (dac == 0)
  {
    // offset = amplitude / 2;
  }

  for (int i = 0; i < 360; i++)
  {
    if (dac == 0)
    {
      sine0[i] = offset + round(amplitude / 2 * sin(i * PI / 180));
    }
    else if (dac == 1)
    {
      sine1[i] = offset + round((amplitude - (offset - 2047)) / 2 * sin(i * PI / 180));
    }
    else if (dac == 2)
    {
      sine0[i] = offset + round(amplitude / 2 * sin(i * PI / 180));
      sine1[i] = offset + round(amplitude / 2 * sin(i * PI / 180));
    }
  }
}
void GetAdc29Status(int i)
{
  gpio_function gpio29Function = gpio_get_function(29);
  Serial.print("GPIO29 func: ");
  Serial.println(gpio29Function);

  bool pd = gpio_is_pulled_down(29);
  Serial.print("GPIO29 pd: ");
  Serial.println(pd);

  bool h = gpio_is_input_hysteresis_enabled(29);
  Serial.print("GPIO29 h: ");
  Serial.println(h);

  gpio_slew_rate slew = gpio_get_slew_rate(29);
  Serial.print("GPIO29 slew: ");
  Serial.println(slew);

  gpio_drive_strength drive = gpio_get_drive_strength(29);
  Serial.print("GPIO29 drive: ");
  Serial.println(drive);

  int irqmask = gpio_get_irq_event_mask(29);
  Serial.print("GPIO29 irqmask: ");
  Serial.println(irqmask);

  bool out = gpio_is_dir_out(29);
  Serial.print("GPIO29 out: ");
  Serial.println(out);
  Serial.printf("(%i) GPIO29 func: %i, pd: %i, h: %i, slew: %i, drive: %i, irqmask: %i, out: %i\n", i, gpio29Function, pd, h, slew, drive, irqmask, out);
}
