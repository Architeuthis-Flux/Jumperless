#include <Arduino.h>
#include "JumperlessDefines.h"
#include "jMatrixControl.h"
#include "MatrixState.h"


jMatrixControl j;

//I would do this in PlatformIO but dxCore is outdated there and doesn't support DD series chips. And I spent some time on a workaround but gave up


nanoStatus nano;

void setup() {

  //DAC0.CTRLA |= (DAC_OUTEN_bm | DAC_ENABLE_bm); // make sure the DAC is outputting 2.5V at rest so it doesn't heat the op amp trying to if it's unused
  //DAC0.DATA = (1023 << 6);
  //DAC0.DATA = (500 << 6);

  //pinMode(PIN_PA2, OUTPUT);
  // pinMode(PIN_PA3, OUTPUT);
  //digitalWrite(PIN_PA2,LOW);

  Serial.pins(PIN_PA2, PIN_PA3);
  Serial.begin(115200);

  pinMode(AY0, OUTPUT);
  digitalWrite(AY0, LOW);
  pinMode(AY1, OUTPUT);
  digitalWrite(AY1, LOW);
  pinMode(AY2, OUTPUT);
  digitalWrite(AY2, LOW);

  pinMode(AX0, OUTPUT);
  digitalWrite(AX0, LOW);
  pinMode(AX1, OUTPUT);
  digitalWrite(AX1, LOW);
  pinMode(AX2, OUTPUT);
  digitalWrite(AX2, LOW);
  pinMode(AX3, OUTPUT);
  digitalWrite(AX3, LOW);

  pinMode(CS_A, OUTPUT);
  digitalWrite(CS_A, LOW);

  pinMode(CS_B, OUTPUT);
  digitalWrite(CS_B, LOW);

  pinMode(CS_C, OUTPUT);
  digitalWrite(CS_C, LOW);

  pinMode(CS_D, OUTPUT);
  digitalWrite(CS_D, LOW);

  pinMode(CS_E, OUTPUT);
  digitalWrite(CS_E, LOW);

  pinMode(CS_F, OUTPUT);
  digitalWrite(CS_F, LOW);

  pinMode(CS_G, OUTPUT);
  digitalWrite(CS_G, LOW);

  pinMode(CS_H, OUTPUT);
  digitalWrite(CS_H, LOW);

  pinMode(CS_I, OUTPUT);
  digitalWrite(CS_I, LOW);

  pinMode(CS_J, OUTPUT);
  digitalWrite(CS_J, LOW);

  pinMode(CS_K, OUTPUT);
  digitalWrite(CS_K, LOW);

  pinMode(DATAPIN, OUTPUT);
  pinMode(STROBE, OUTPUT);

  pinMode(RESET, OUTPUT);  // all of this just sets up all these pins as outputs and drives them LOW
                           // digitalWrite(RESET, HIGH); // I'm pretty sure Arduino IDE does this automatically but I wouldn't count on it
                           // delayMicroseconds(380);
  digitalWrite(RESET, LOW);
  j.clearAllConnections();
  // j.connectDumbMode(1,5,'A',1);
}

int outVoltage = 0;

void loop() {

  j.connectDumbMode(8, 1, 'E', 1);  //right now all we have is dumb mode, there's a pathfinging function in the old repo but it needs to be redone in a more understandable way
  j.connectDumbMode(14, 4, 'I', 1);

  j.connectDumbMode(7, 6, 'D', 1);
  j.connectDumbMode(12, 3, 'J', 1);


  j.connectDumbMode(8, 2, 'D', 1);
  j.connectDumbMode(6, 2, 'E', 1);



  delay(1000);

  j.connectDumbMode(6, 2, 'E', 0);

  delay(1000);


  printConnections();
  delay(1000);
}




void printConnections(void) {

  Serial.println("\n");

  Serial.printf("Pin Name\tSF Chip Connections\n\r");
  for (int i = 0; i < 24; i++)  //just checking if I run out of space
  {

    Serial.print(nano.pinNames[i]);
    Serial.print("\t\t");

    if (nano.mapI[i] >= 0) {
      Serial.print(j.chipIntToChar(nano.mapI[i]));
      Serial.print(" x");
      Serial.print(nano.xMapI[i]);
      Serial.print("\t");
    }
    if (nano.mapJ[i] >= 0) {
      Serial.print(j.chipIntToChar(nano.mapJ[i]));
      Serial.print(" x");
      Serial.print(nano.xMapJ[i]);
      Serial.print("\t");
    }
    if (nano.mapK[i] >= 0) {
      Serial.print(j.chipIntToChar(nano.mapK[i]));
      Serial.print(" x");
      Serial.print(nano.xMapK[i]);
      Serial.print("\t");
    }

    Serial.println(" ");
  }
  Serial.println("\n\n");


  Serial.println("\tX connections \t\t\t\t\t\t Y connections");
  for (int i = 0; i < 11; i++)  //just checking if I run out of space
  {

    if (i == 8) Serial.println(' ');

    Serial.print(mt[i].chipChar);
    Serial.print("\t ");

    for (int j = 0; j < 16; j++) {
      //mt[i].xStatus[j] = -1;

      ///int ch = (int) mt[i].xMap[j];
      if (i < 8) {
        Serial.print(mt[mt[i].xMap[j]].chipChar);  //here we're using the value of yMap as the index to return chipChar on that chipStatus struct
      } else {

        switch (mt[i].xMap[j]) {
          case GND:
            Serial.print("GND");
            break;

          case TOP_RAIL:
            Serial.print("TOP");
            break;

          case BOTTOM_RAIL:
            Serial.print("BOT");
            break;

          case DAC0TO5V:
            Serial.print("05V");
            break;

          case DACPLUSMINUS9V:
            Serial.print("D9V");
            break;

          default:
            //Serial.print(mt[i].xMap[j]);
            Serial.print(nano.pinNames[nano.reversePinMap[mt[i].xMap[j]]]);
            break;
        }
      }
      if (j < 15) Serial.print(", ");
    }
    Serial.print("\t\t");
    if (i > 7) Serial.print(" ");

    for (int j = 0; j < 8; j++) {
      //chips[i].yStatus[j] = j;


      if (mt[i].yMap[j] > 0 && mt[i].yMap[j] < 10) Serial.print(' ');  //padding
      if (i > 7) {
        Serial.print(mt[mt[i].yMap[j]].chipChar);
      } else {
        Serial.print(mt[i].yMap[j]);
      }
      if (j < 7) Serial.print(',');
    }
    Serial.println("\t <- Map");
    Serial.print("\t");

    for (int j = 0; j < 16; j++) {
      //mt[i].xStatus[j] = -1;
      if (i > 7) Serial.print(" ");
      Serial.print(mt[i].xStatus[j]);
      if (mt[i].xStatus[j] > 0 && mt[i].xStatus[j] < 10) Serial.print(' ');  //padding
      if (i > 7) Serial.print(" ");
      if (j < 15) Serial.print(',');
    }
    Serial.print("\t\t");
    for (int j = 0; j < 8; j++) {
      //chips[i].yStatus[j] = j;
      Serial.print(mt[i].yStatus[j]);
      if (mt[i].yStatus[j] > 0 && mt[i].yStatus[j] < 10) Serial.print(' ');  //padding
      if (j < 7) Serial.print(',');
    }

    Serial.println("\t <- Status\n");
  }
}