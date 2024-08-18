// Settings.h

// adjustable settings

// Copyright (C) 2024 highvoltglow
// Licensed under the MIT License

#pragma once

// HV shutdown delay
#define PIR_DELAY 5 // in minutes, 0 disables PIR

// cathode poisoning
#define ROTATION_INTERVAL 5       // in minutes
#define ROTATION_STEPINTERVAL 250 // in ms

// the solar API V1 allows a polling interval down to 4 seconds, don't go below this
#define INVERTER_POLLINGINTERVAL 5 // in seconds

// IP address of the inverter
#define INVERTER_IPADDRESS "xxx.xxx.xxx.xxx"

// Inverter connection port
#define INVERTER_PORT 80

// values in the defined range are set to 0 to keep the display quieter
// set this value to 0 if you dont want to round towards zero
#define POWER_ROUND_TO_ZERO_RANGE 10 // in watts

// methods to get the load power value
#define LOADPOWER_CALCULATE 1    // calculate the load power using the grid, solar and battery power values
#define LOADPOWER_FROMINVERTER 2 // take the load power value provided by the inverter, always slightly lower as calculated

// used method to get the load power value
#define GET_LOADPOWER_METHOD LOADPOWER_CALCULATE