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

#include "tusb_option.h"

// ESP32 will use the arduino-esp32 core initialization and Serial
#if CFG_TUD_ENABLED && !defined(ARDUINO_ARCH_ESP32)

#include "Adafruit_TinyUSB.h"
#include "Arduino.h"

//--------------------------------------------------------------------+
// MACRO TYPEDEF CONSTANT ENUM DECLARATION
//--------------------------------------------------------------------+
extern "C" {

void TinyUSB_Device_Init(uint8_t rhport) {
  // Init USB Device controller and stack
  TinyUSBDevice.begin(rhport);
}

// RP2040 has its own implementation since it needs mutex for dual core
#ifndef ARDUINO_ARCH_RP2040
void TinyUSB_Device_Task(void) {
  // Run tinyusb device task
  tud_task();
}
#endif

void TinyUSB_Device_FlushCDC(void) {
  uint8_t const cdc_instance = Adafruit_USBD_CDC::getInstanceCount();
  for (uint8_t instance = 0; instance < cdc_instance; instance++) {
    tud_cdc_n_write_flush(instance);
  }
}

// Debug log with Serial1
#if CFG_TUSB_DEBUG && defined(CFG_TUSB_DEBUG_PRINTF)
__attribute__((used)) int CFG_TUSB_DEBUG_PRINTF(const char *__restrict format,
                                                ...) {
  static bool ser1_inited = false;
  if (!ser1_inited) {
    ser1_inited = true;
    Serial1.begin(115200);
  }

  char buf[256];
  int len;
  va_list ap;
  va_start(ap, format);
  len = vsnprintf(buf, sizeof(buf), format, ap);
  Serial1.write(buf);
  va_end(ap);
  return len;
}
#endif

} // extern C

#endif
