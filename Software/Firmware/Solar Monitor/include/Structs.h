// Structs.h

// struct definitions

// Copyright (C) 2024 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>

// structure for holding the display value
typedef struct
{
  bool errorFlag;
  bool minusFlag;
  bool plusFlag;
  bool percentageFlag;
  bool kiloFlag;
  bool wattFlag;
  bool overallStatusPlusFlag;
  bool overallStatusMinusFlag;
  uint8_t decimalIndex;
  uint8_t digits[3];
} DISPLAY_VALUE;