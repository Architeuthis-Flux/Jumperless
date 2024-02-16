/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 hathach for Adafruit Industries
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

#if CFG_TUD_ENABLED && CFG_TUD_VENDOR

#include "Adafruit_USBD_WebUSB.h"
#include "Arduino.h"

#ifdef ARDUINO_ARCH_ESP32
#include "USB.h"
#endif

//--------------------------------------------------------------------+
// MACRO TYPEDEF CONSTANT ENUM DECLARATION
//--------------------------------------------------------------------+
#define EPOUT 0x00
#define EPIN 0x80
#define EPSIZE 64

enum { VENDOR_REQUEST_WEBUSB = 1, VENDOR_REQUEST_MICROSOFT = 2 };

// TODO multiple instances
static Adafruit_USBD_WebUSB *_webusb_dev = NULL;

//--------------------------------------------------------------------+
// BOS Descriptor
//--------------------------------------------------------------------+

/* Microsoft OS 2.0 registry property descriptor
Per MS requirements
https://msdn.microsoft.com/en-us/library/windows/hardware/hh450799(v=vs.85).aspx
device should create DeviceInterfaceGUIDs. It can be done by driver and
in case of real PnP solution device should expose MS "Microsoft OS 2.0
registry property descriptor". Such descriptor can insert any record
into Windows registry per device/configuration/interface. In our case it
will insert "DeviceInterfaceGUIDs" multistring property.

GUID is freshly generated and should be OK to use.

https://developers.google.com/web/fundamentals/native-hardware/build-for-webusb/
(Section Microsoft OS compatibility descriptors)
*/

#define BOS_TOTAL_LEN                                                          \
  (TUD_BOS_DESC_LEN + TUD_BOS_WEBUSB_DESC_LEN + TUD_BOS_MICROSOFT_OS_DESC_LEN)

#define MS_OS_20_DESC_LEN 0xB2

// BOS Descriptor is required for webUSB
uint8_t const desc_bos[] = {
    // total length, number of device caps
    TUD_BOS_DESCRIPTOR(BOS_TOTAL_LEN, 2),

    // Vendor Code, iLandingPage
    TUD_BOS_WEBUSB_DESCRIPTOR(VENDOR_REQUEST_WEBUSB, 1),

    // Microsoft OS 2.0 descriptor
    TUD_BOS_MS_OS_20_DESCRIPTOR(MS_OS_20_DESC_LEN, VENDOR_REQUEST_MICROSOFT)};

uint8_t desc_ms_os_20[] = {
    // Set header: length, type, windows version, total length
    U16_TO_U8S_LE(0x000A), U16_TO_U8S_LE(MS_OS_20_SET_HEADER_DESCRIPTOR),
    U32_TO_U8S_LE(0x06030000), U16_TO_U8S_LE(MS_OS_20_DESC_LEN),

    // Configuration subset header: length, type, configuration index, reserved,
    // configuration total length
    U16_TO_U8S_LE(0x0008), U16_TO_U8S_LE(MS_OS_20_SUBSET_HEADER_CONFIGURATION),
    0, 0, U16_TO_U8S_LE(MS_OS_20_DESC_LEN - 0x0A),

    // Function Subset header: length, type, first interface, reserved, subset
    // length
    U16_TO_U8S_LE(0x0008), U16_TO_U8S_LE(MS_OS_20_SUBSET_HEADER_FUNCTION),
    0 /*itf num*/, 0, U16_TO_U8S_LE(MS_OS_20_DESC_LEN - 0x0A - 0x08),

    // MS OS 2.0 Compatible ID descriptor: length, type, compatible ID, sub
    // compatible ID
    U16_TO_U8S_LE(0x0014), U16_TO_U8S_LE(MS_OS_20_FEATURE_COMPATBLE_ID), 'W',
    'I', 'N', 'U', 'S', 'B', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, // sub-compatible

    // MS OS 2.0 Registry property descriptor: length, type
    U16_TO_U8S_LE(MS_OS_20_DESC_LEN - 0x0A - 0x08 - 0x08 - 0x14),
    U16_TO_U8S_LE(MS_OS_20_FEATURE_REG_PROPERTY), U16_TO_U8S_LE(0x0007),
    U16_TO_U8S_LE(0x002A), // wPropertyDataType, wPropertyNameLength and
                           // PropertyName "DeviceInterfaceGUIDs\0" in UTF-16
    'D', 0x00, 'e', 0x00, 'v', 0x00, 'i', 0x00, 'c', 0x00, 'e', 0x00, 'I', 0x00,
    'n', 0x00, 't', 0x00, 'e', 0x00, 'r', 0x00, 'f', 0x00, 'a', 0x00, 'c', 0x00,
    'e', 0x00, 'G', 0x00, 'U', 0x00, 'I', 0x00, 'D', 0x00, 's', 0x00, 0x00,
    0x00,
    U16_TO_U8S_LE(0x0050), // wPropertyDataLength
    // bPropertyData: “{975F44D9-0D08-43FD-8B3E-127CA8AFFF9D}”.
    '{', 0x00, '9', 0x00, '7', 0x00, '5', 0x00, 'F', 0x00, '4', 0x00, '4', 0x00,
    'D', 0x00, '9', 0x00, '-', 0x00, '0', 0x00, 'D', 0x00, '0', 0x00, '8', 0x00,
    '-', 0x00, '4', 0x00, '3', 0x00, 'F', 0x00, 'D', 0x00, '-', 0x00, '8', 0x00,
    'B', 0x00, '3', 0x00, 'E', 0x00, '-', 0x00, '1', 0x00, '2', 0x00, '7', 0x00,
    'C', 0x00, 'A', 0x00, '8', 0x00, 'A', 0x00, 'F', 0x00, 'F', 0x00, 'F', 0x00,
    '9', 0x00, 'D', 0x00, '}', 0x00, 0x00, 0x00, 0x00, 0x00};

TU_VERIFY_STATIC(sizeof(desc_ms_os_20) == MS_OS_20_DESC_LEN, "Incorrect size");

//--------------------------------------------------------------------+
// IMPLEMENTATION
//--------------------------------------------------------------------+

#ifdef ARDUINO_ARCH_ESP32
static uint16_t webusb_load_descriptor(uint8_t *dst, uint8_t *itf) {
  // uint8_t str_index = tinyusb_add_string_descriptor("TinyUSB MSC");

  uint8_t ep_in = tinyusb_get_free_in_endpoint();
  uint8_t ep_out = tinyusb_get_free_out_endpoint();
  TU_VERIFY(ep_in && ep_out);
  ep_in |= 0x80;

  uint16_t desc_len = _webusb_dev->getInterfaceDescriptor(0, NULL, 0);

  desc_len = _webusb_dev->makeItfDesc(*itf, dst, desc_len, ep_in, ep_out);

  *itf += 1;
  return desc_len;
}
#endif

Adafruit_USBD_WebUSB::Adafruit_USBD_WebUSB(const void *url) {
  _connected = false;
  _url = (const uint8_t *)url;
  _linestate_cb = NULL;

#ifdef ARDUINO_ARCH_ESP32
  // ESP32 requires setup configuration descriptor within constructor

  // WebUSB requires USB version at least 2.1 (or 3.x)
  USB.usbVersion(0x0210);

  _webusb_dev = this;
  uint16_t const desc_len = getInterfaceDescriptor(0, NULL, 0);
  tinyusb_enable_interface(USB_INTERFACE_VENDOR, desc_len,
                           webusb_load_descriptor);
#endif
}

bool Adafruit_USBD_WebUSB::begin(void) {
  if (!TinyUSBDevice.addInterface(*this)) {
    return false;
  }

  // WebUSB requires USB version at least 2.1 (or 3.x)
  TinyUSBDevice.setVersion(0x0210);

  _webusb_dev = this;
  return true;
}

bool Adafruit_USBD_WebUSB::setLandingPage(const void *url) {
  _url = (const uint8_t *)url;
  return true;
}

void Adafruit_USBD_WebUSB::setLineStateCallback(linestate_callback_t fp) {
  _linestate_cb = fp;
}

uint16_t Adafruit_USBD_WebUSB::makeItfDesc(uint8_t itfnum, uint8_t *buf,
                                           uint16_t bufsize, uint8_t ep_in,
                                           uint8_t ep_out) {
  uint8_t desc[] = {TUD_VENDOR_DESCRIPTOR(itfnum, 0, ep_out, ep_in, EPSIZE)};
  uint16_t const len = sizeof(desc);

  // null buffer for length only
  if (buf) {
    if (bufsize < len) {
      return 0;
    }

    memcpy(buf, desc, len);

    // update the bFirstInterface in MS OS 2.0 descriptor
    // that is binded to WinUSB driver
    desc_ms_os_20[0x0a + 0x08 + 4] = itfnum;
  }

  return len;
}

uint16_t Adafruit_USBD_WebUSB::getInterfaceDescriptor(uint8_t itfnum,
                                                      uint8_t *buf,
                                                      uint16_t bufsize) {
  // usb core will automatically update endpoint number
  return makeItfDesc(itfnum, buf, bufsize, EPIN, EPOUT);
}

bool Adafruit_USBD_WebUSB::connected(void) {
  return tud_vendor_mounted() && _connected;
}

Adafruit_USBD_WebUSB::operator bool() {
  // Add an yield to run usb background in case sketch block wait as follows
  // while( !webusb ) {}
  if (!connected()) {
    yield();
  }

  return connected();
}

int Adafruit_USBD_WebUSB::available(void) {
  uint32_t count = tud_vendor_available();

  // Add an yield to run usb background in case sketch block wait as follows
  // while( !webusb.available() ) {}
  if (!count) {
    yield();
  }

  return count;
}

int Adafruit_USBD_WebUSB::read(void) {
  uint8_t ch;
  return tud_vendor_read(&ch, 1) ? (int)ch : -1;
}

size_t Adafruit_USBD_WebUSB::read(uint8_t *buffer, size_t size) {
  return tud_vendor_read(buffer, size);
}

size_t Adafruit_USBD_WebUSB::write(uint8_t b) { return this->write(&b, 1); }

size_t Adafruit_USBD_WebUSB::write(const uint8_t *buffer, size_t size) {
  size_t remain = size;
  while (remain && _connected) {
    size_t wrcount = tud_vendor_write(buffer, remain);
    remain -= wrcount;
    buffer += wrcount;

    // Write FIFO is full, run usb background to flush
    if (remain) {
      yield();
    }
  }

  return size - remain;
}

int Adafruit_USBD_WebUSB::peek(void) {
  uint8_t ch;
  return tud_vendor_peek(&ch) ? (int)ch : -1;
}

void Adafruit_USBD_WebUSB::flush(void) { tud_vendor_flush(); }

//--------------------------------------------------------------------+
// TinyUSB stack callbacks
//--------------------------------------------------------------------+
extern "C" {

uint8_t const *tud_descriptor_bos_cb(void) { return desc_bos; }

// Invoked when a control transfer occurred on an interface of this class
// Driver response accordingly to the request and the transfer stage
// (setup/data/ack) return false to stall control endpoint (e.g unsupported
// request)
bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage,
                                tusb_control_request_t const *request) {
  if (!_webusb_dev) {
    return false;
  }

  // nothing to with DATA & ACK stage
  if (stage != CONTROL_STAGE_SETUP) {
    return true;
  }

  switch (request->bmRequestType_bit.type) {
  case TUSB_REQ_TYPE_VENDOR:
    switch (request->bRequest) {
    case VENDOR_REQUEST_WEBUSB:
      // match vendor request in BOS descriptor
      // Get landing page url
      if (!_webusb_dev->_url) {
        return false;
      }
      return tud_control_xfer(rhport, request, (void *)_webusb_dev->_url,
                              _webusb_dev->_url[0]);

    case VENDOR_REQUEST_MICROSOFT:
      if (request->wIndex == 7) {
        // Get Microsoft OS 2.0 compatible descriptor
        uint16_t total_len;
        memcpy(&total_len, desc_ms_os_20 + 8, 2);

        return tud_control_xfer(rhport, request, (void *)desc_ms_os_20,
                                total_len);
      } else {
        return false;
      }

    default:
      break;
    }
    break;

  case TUSB_REQ_TYPE_CLASS:
    if (request->bRequest == 0x22) {
      // Webserial simulate the CDC_REQUEST_SET_CONTROL_LINE_STATE (0x22) to
      // connect and disconnect.
      _webusb_dev->_connected = (request->wValue != 0);

      // response with status OK
      tud_control_status(rhport, request);

      // invoked callback if any (TODO should be done at ACK stage)
      if (_webusb_dev->_linestate_cb) {
        _webusb_dev->_linestate_cb(_webusb_dev->_connected);
      }

      return true;
    }
    break;

  default:
    // stall unknown request
    return false;
  }

  return true;
}
}

#endif // CFG_TUD_ENABLED
