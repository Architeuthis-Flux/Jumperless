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

/* This sketch demonstrates multiple USB HID interfaces. Pressing the button will
 * - mouse toward bottom right of monitor
 * - send 'a' key
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

#elif defined PIN_BUTTON
  const int pin = PIN_BUTTON;
  bool activeState = false;

#else
  const int pin = 12;
  bool activeState = false;
#endif

// HID report descriptor using TinyUSB's template
uint8_t const desc_keyboard_report[] =
{
  TUD_HID_REPORT_DESC_KEYBOARD()
};

uint8_t const desc_mouse_report[] =
{
  TUD_HID_REPORT_DESC_MOUSE()
};

// USB HID object. For ESP32 these values cannot be changed after this declaration
// desc report, desc len, protocol, interval, use out endpoint
Adafruit_USBD_HID usb_keyboard(desc_keyboard_report, sizeof(desc_keyboard_report), HID_ITF_PROTOCOL_KEYBOARD, 2, false);
Adafruit_USBD_HID usb_mouse(desc_mouse_report, sizeof(desc_mouse_report), HID_ITF_PROTOCOL_MOUSE, 2, false);

// the setup function runs once when you press reset or power the board
void setup()
{
  // Notes: following commented-out functions has no affect on ESP32
  // usb_keyboard.setPollInterval(2);
  // usb_keyboard.setReportDescriptor();
  // usb_keyboard.setStringDescriptor("TinyUSB HID Composite");

  usb_keyboard.begin();
  usb_mouse.begin();

  // Set up button, pullup opposite to active state
  pinMode(pin, activeState ? INPUT_PULLDOWN : INPUT_PULLUP);

  Serial.begin(115200);

  // wait until device mounted
  //while( !TinyUSBDevice.mounted() ) delay(1);
  Serial.println("Adafruit TinyUSB HID Composite example");
}

void loop()
{
  // poll gpio once each 10 ms
  delay(10);

  // Whether button is pressed
  bool btn_pressed = (digitalRead(pin) == activeState);

  // Remote wakeup
  if ( TinyUSBDevice.suspended() && btn_pressed )
  {
    // Wake up host if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by host
    TinyUSBDevice.remoteWakeup();
  }

  /*------------- Mouse -------------*/
  if (usb_mouse.ready() && btn_pressed )
  {
    int8_t const delta = 5;
    usb_mouse.mouseMove(0, delta, delta); // right + down
  }

  /*------------- Keyboard -------------*/
  if ( usb_keyboard.ready() )
  {
    // use to send key release report
    static bool has_key = false;

    if ( btn_pressed )
    {
      uint8_t keycode[6] = { 0 };
      keycode[0] = HID_KEY_A;

      usb_keyboard.keyboardReport(0, 0, keycode);

      has_key = true;
    }else
    {
      // send empty key report if previously has key pressed
      if (has_key) usb_keyboard.keyboardRelease(0);
      has_key = false;
    }
  }
}