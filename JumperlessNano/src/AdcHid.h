// SPDX-License-Identifier: MIT

#ifndef JUMPERLESS_HID_H
#define JUMPERLESS_HID_H

// if `setupAdcHidStuff` was called, this always contains somewhat up-to-date ADC readings
extern uint16_t adcReadings[4];

void setupAdcHidStuff();

#endif
