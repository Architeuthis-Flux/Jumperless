
#include "MatrixStateRP2040.h"
#include "NetsToChipConnections.h"
#include "LEDs.h"

#include "JumperlessDefinesRP2040.h"
#include "ArduinoStuff.h"
#include <Arduino.h>
//#include <SoftwareSerial.h>


//SoftwareSerial ardSerial(1, 0);


//SerialPIO ardSerial(1, 0);

void initArduino (void)
{

//Serial1.setRX(1);

//Serial1.setTX(0);


//pinMode (1, OUTPUT);
//pinMode (0, INPUT);
Serial1.begin(115200);
Serial1.println("hello from arduino");






}

void arduinoPrint (void)
{
    if (Serial1.available())
    {
        Serial1.read();
        Serial.write( Serial1.read());
    }

    //Serial1.println("fuck");
}

void uploadArduino (void)
{
    while (!Serial.available());
    
    while (Serial.available())
    {
        Serial1.write(Serial.read());





    }






}