/*********************************************************************
 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 Copyright (c) 2019 Ha Thach for Adafruit Industries
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/

#include "Adafruit_TinyUSB.h"

/* This sketch demonstrates USB HID mouse
 * Press button pin will move
 * - mouse toward bottom right of monitor
 *
 * Depending on the board, the button pin
 * and its active state (when pressed) are different
 */
#if defined(ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS) || defined(ARDUINO_NRF52840_CIRCUITPLAY)
  const int pin = 4; // Left Button
  bool activeState = true;

#elif defined(ARDUINO_FUNHOUSE_ESP32S2)
  const int pin = BUTTON_DOWN;
  bool activeState = true;

#elif defined PIN_BUTTON1
  const int pin = PIN_BUTTON1;
  bool activeState = false;

#else
  const int pin = 12;
  bool activeState = false;
#endif


// HID report descriptor using TinyUSB's template
// Single Report (no ID) descriptor
uint8_t const desc_hid_report[] =
{
  TUD_HID_REPORT_DESC_MOUSE()
};

// USB HID object. For ESP32 these values cannot be changed after this declaration
// desc report, desc len, protocol, interval, use out endpoint
Adafruit_USBD_HID usb_hid(desc_hid_report, sizeof(desc_hid_report), HID_ITF_PROTOCOL_MOUSE, 2, false);

// the setup function runs once when you press reset or power the board
void setup()
{
#if defined(ARDUINO_ARCH_MBED) && defined(ARDUINO_ARCH_RP2040)
  // Manual begin() is required on core without built-in support for TinyUSB such as mbed rp2040
  TinyUSB_Device_Init(0);
#endif

  // Set up button, pullup opposite to active state
  pinMode(pin, activeState ? INPUT_PULLDOWN : INPUT_PULLUP);

  // Notes: following commented-out functions has no affect on ESP32
  // usb_hid.setBootProtocol(HID_ITF_PROTOCOL_MOUSE);
  // usb_hid.setPollInterval(2);
  // usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  // usb_hid.setStringDescriptor("TinyUSB Mouse");

  usb_hid.begin();

  Serial.begin(115200);

  // wait until device mounted
  while( !TinyUSBDevice.mounted() ) delay(1);

  Serial.println("Adafruit TinyUSB HID Mouse example");
}

void loop()
{
  // poll gpio once each 10 ms
  delay(10);

  // Whether button is pressed
  bool btn_pressed = (digitalRead(pin) == activeState);

  // nothing to do if button is not pressed
  if (!btn_pressed) return;

  // Remote wakeup
  if ( TinyUSBDevice.suspended() )
  {
    // Wake up host if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by host
    TinyUSBDevice.remoteWakeup();
  }

  if ( usb_hid.ready() )
  {
    uint8_t const report_id = 0; // no ID
    int8_t const delta = 5;
    usb_hid.mouseMove(report_id, delta, delta); // right + down
  }
}
