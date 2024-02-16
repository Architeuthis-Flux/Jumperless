/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef ADAFRUIT_USBH_HOST_H_
#define ADAFRUIT_USBH_HOST_H_

#include "Adafruit_USBD_Interface.h"
#include "tusb.h"

#ifdef ARDUINO_ARCH_ESP32
#include "esp32-hal-tinyusb.h"
#endif

class Adafruit_USBH_Host {
private:
public:
  Adafruit_USBH_Host(void);

  bool configure(uint8_t rhport, uint32_t cfg_id, const void *cfg_param);

#ifdef ARDUINO_ARCH_RP2040
  bool configure_pio_usb(uint8_t rhport, const void *cfg_param);
#endif

  bool begin(uint8_t rhport);
  void task(void);

private:
  //  uint16_t const *descrip`tor_string_cb(uint8_t index, uint16_t langid);
  //
  //  friend uint8_t const *tud_descriptor_device_cb(void);
  //  friend uint8_t const *tud_descriptor_configuration_cb(uint8_t index);
  //  friend uint16_t const *tud_descriptor_string_cb(uint8_t index,
  //                                                  uint16_t langid);
};

// extern Adafruit_USBH_Host TinyUSBHost;
//
//// USBHost has a high chance to conflict with other usb stack
//// only define if supported BSP
// #ifdef USE_TINYUSB
// #define USBHost TinyUSBHost
// #endif

#endif /* ADAFRUIT_USBH_HOST_H_ */
