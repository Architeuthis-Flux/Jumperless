// SPDX-License-Identifier: MIT

#include "ArduinoStuff.h"
#include "MatrixStateRP2040.h"
#include "NetsToChipConnections.h"
#include "LEDs.h"

// #include "JumperlessDefinesRP2040.h"

// #include <Arduino.h>
// #include <SoftwareSerial.h>

// SoftwareSerial ardSerial(1, 0);

// SerialPIO ardSerial(1, 0);

// SerialPIO routableUART(16,17,32);
// SoftwareSerial routableUART(16,17);

void initArduino(void) // if the UART is set up, the Arduino won't flash from it's own USB port
{

        Serial1.setRX(17);

        Serial1.setTX(16);

        Serial1.begin(115200);

        delay(1);
}

void setBaudRate(int baudRate)
{
        Serial1.end();
        delay(1);
    Serial1.begin(baudRate);
        delay(1);
}

void arduinoPrint(void)
{
}

void uploadArduino(void)
{
}
