/*********************************************************************
 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 Copyright (c) 2019 Ha Thach for Adafruit Industries
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/

// This sketch is enumerated as USB MIDI device with multiple ports
// and how to set their name

#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include <MIDI.h>

// USB MIDI object with 3 ports
Adafruit_USBD_MIDI usb_midi(3);

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

#if defined(ARDUINO_ARCH_MBED) && defined(ARDUINO_ARCH_RP2040)
  // Manual begin() is required on core without built-in support for TinyUSB such as mbed rp2040
  TinyUSB_Device_Init(0);
#endif

  // Set name for each cable, must be done before usb_midi.begin()
  usb_midi.setCableName(1, "Keyboard");
  usb_midi.setCableName(2, "Drum Pads");
  usb_midi.setCableName(3, "Lights");

  usb_midi.begin();
}

void loop()
{
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);

  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}
