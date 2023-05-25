#include <Arduino.h>
#include "JumperlessDefinesRP2040.h"
#include "MatrixStateRP2040.h"


nanoStatus nano;
const char* definesToChar (int); //i really need to find a way to not need to forward declare fuctions with this setup, i hate it

//void printConnections();

void setup() {

  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT_12MA);

  
}

void loop() {

digitalWrite(LED_BUILTIN,HIGH);
delay(millis()/100);
digitalWrite(LED_BUILTIN,LOW);
delay(millis()/100);


for (int i = 0; i < 9; i++) //this is just to check that the structs are being set up correctly
{
Serial.print("\n\r");
Serial.print (net[i].name);
Serial.print ("\t");
Serial.print (net[i].number);
Serial.print ("\t");
Serial.print (definesToChar(net[i].specialFunction));
if (i == 1) Serial.print ("\t"); //padding for "GND"
Serial.print ("\t{");


for (int k = 0; k < 8; k++)
{
      if (net[i].doNotIntersectNodes[k] != 0) 
    {
      
Serial.print (definesToChar(net[i].doNotIntersectNodes[k]));
Serial.print (",");
    }

}
Serial.print ("}\t");

  for (int j = 0 ; j < MAX_NODES; j++)
  {
    if (net[i].nodes[j] != 0) 
    {
      
      Serial.print (definesToChar(net[i].nodes[j]));
        Serial.print (",");
    }
     
  }
  Serial.println("\n\n\n\n\r");
 

}
delay(10000);

}

const char* definesToChar (int defined) //converts the internally used #defined numbers into human readable strings
{

const char *defNanoToChar[26] = {"NANO_D0", "NANO_D1", "NANO_D2", "NANO_D3", "NANO_D4", "NANO_D5", "NANO_D6", "NANO_D7", "NANO_D8", "NANO_D9", "NANO_D10", "NANO_D11", "NANO_D12", "NANO_D13", "NANO_RESET", "NANO_AREF", "NANO_A0", "NANO_A1", "NANO_A2", "NANO_A3", "NANO_A4", "NANO_A5", "NANO_A6", "NANO_A7"};
const char *defSpecialToChar[12] = {"GND","NOT_DEFINED","NOT_DEFINED","SUPPLY_3V3","NOT_DEFINED","SUPPLY_5V","DAC_0_+5V","DAC_1_+-8V","CURRENT_SENSE +","CURRENT_SENSE -"};
const char *emptyNet[] = {"EMPTY_NET", "?"};

if (defined >= 70 && defined <= 93)
{
return defNanoToChar[defined-70];
} else if (defined >= 100 && defined <= 110)
  {
    return defSpecialToChar[defined-100];
  } else if (defined == EMPTY_NET)
  {
    return emptyNet[0];
  }else {
    return emptyNet[1];
  }


}

/*

void printConnections(void) {

  Serial.println("\n");

  Serial.printf("Pin Name\tSF Chip Connections\n\r");
  for (int i = NANO_PIN_D0; i <= NANO_PIN_A7; i++)  //just checking if I run out of space
  {
    Serial.print(nano.pinNames[i]);
    Serial.print(" \t");
    }

    Serial.println(" ");
    for (int i = NANO_PIN_D0; i <= NANO_PIN_A7; i++)  //just checking if I run out of space
  {
    Serial.print(nano.pinMap[i]);
    Serial.print(" \t");
    }

    Serial.println(" ");
    for (int i = NANO_PIN_D0; i <= NANO_PIN_A7; i++)  //just checking if I run out of space
  {
    Serial.print(nano.numConns[i]);
    Serial.print(" \t");
    }

    Serial.println(" ");
        for (int i = NANO_PIN_D0; i <= NANO_PIN_A7; i++)  //just checking if I run out of space
  {
    Serial.print(nano.mapI[i]);
    Serial.print(" \t");
    }

    Serial.println(" ");
        for (int i = NANO_PIN_D0; i <= NANO_PIN_A7; i++)  //just checking if I run out of space
  {
    Serial.print(nano.xMapI[i]);
    Serial.print(" \t");
    }

    Serial.println(" ");
            for (int i = NANO_PIN_D0; i <= NANO_PIN_A7; i++)  //just checking if I run out of space
  {
    Serial.print(nano.xStatusI[i]);
    Serial.print(" \t");
    }

    Serial.println(" ");
            for (int i = NANO_PIN_D0; i <= NANO_PIN_A7; i++)  //just checking if I run out of space
  {
    Serial.print(nano.mapJ[i]);
    Serial.print(" \t");
    }

    Serial.println(" ");
        for (int i = NANO_PIN_D0; i <= NANO_PIN_A7; i++)  //just checking if I run out of space
  {
    Serial.print(nano.xMapJ[i]);
    Serial.print(" \t");
    }

    Serial.println(" ");
            for (int i = NANO_PIN_D0; i <= NANO_PIN_A7; i++)  //just checking if I run out of space
  {
    Serial.print(nano.xStatusJ[i]);
    Serial.print(" \t");
    }

    Serial.println(" ");
            for (int i = NANO_PIN_D0; i <= NANO_PIN_A7; i++)  //just checking if I run out of space
  {
    Serial.print(nano.mapK[i]);
    Serial.print(" \t");
    }

    Serial.println(" ");
        for (int i = NANO_PIN_D0; i <= NANO_PIN_A7; i++)  //just checking if I run out of space
  {
    Serial.print(nano.xMapK[i]);
    Serial.print(" \t");
    }

    Serial.println(" ");
            for (int i = NANO_PIN_D0; i <= NANO_PIN_A7; i++)  //just checking if I run out of space
  {
    Serial.print(nano.xStatusK[i]);
    Serial.print(" \t");
    }

    Serial.println(" ");
            for (int i = NANO_PIN_D0; i <= NANO_PIN_A7; i++)  //just checking if I run out of space
  {
    Serial.print(nano.mapL[i]);
    Serial.print(" \t");
    }

    Serial.println(" ");
        for (int i = NANO_PIN_D0; i <= NANO_PIN_A7; i++)  //just checking if I run out of space
  {
    Serial.print(nano.xMapL[i]);
    Serial.print(" \t");
    }

    Serial.println(" ");
            for (int i = NANO_PIN_D0; i <= NANO_PIN_A7; i++)  //just checking if I run out of space
  {
    Serial.print(nano.xStatusL[i]);
    Serial.print(" \t");
    }

    Serial.println(" ");
  
  Serial.println("\n\n");
*/
/*
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
}*/