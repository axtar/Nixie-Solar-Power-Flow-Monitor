// Controller.hpp

// some helper functions

// Copyright (C) 2024 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <Structs.h>
#include <Debugdefs.h>

class Helper
{
public:
  // converts a double value from the inverter to a structure used to set
  // the 3 numeric and the 3 symbol nixies
  static bool convertDoubleToDisplayValue(double inputValue, DISPLAY_VALUE &displayValue, bool percent = false)
  {
    bool result = true;
    double value;

    // set value
    value = inputValue;

    // if near 0 then 0
    if (abs(value) < 0.5)
    {
      value = 0;
    }

    // check for overflow
    if (abs(value) > 999999)
    {
      // overflow, set error flag and ignore the rest
      result = false;
      displayValue.errorFlag = true;
      return (result);
    }
    // check for percent overflow
    if (percent)
    {
      // check for percent overflow
      if ((value > 100) || (value < 0))
      {
        result = false;
        displayValue.errorFlag = true;
        return (result);
      }
    }

    // set the flags
    displayValue.errorFlag = false;
    displayValue.kiloFlag = (abs(value) >= 1000);
    displayValue.minusFlag = (value < 0);
    displayValue.plusFlag = (value > 0);
    displayValue.percentageFlag = percent;
    // if percentage value, plus/minus not allowed
    if (displayValue.percentageFlag)
    {
      displayValue.minusFlag = false;
      displayValue.plusFlag = false;
      displayValue.wattFlag = false;
    }
    else
    {
      displayValue.wattFlag = true;
    }

    // just 3 numeric nixies, if kiloflag set, divide by 1000
    if (displayValue.kiloFlag)
    {
      value /= 1000;
    }

    // set decimals for rounding
    int decimals;
    if (abs(value) >= 100)
    {
      decimals = 0;
    }
    else if (abs(value) >= 10)
    {
      decimals = 1;
    }
    else
    {
      decimals = 2;
    }

    // fill the 3 digits and the decimal point location
    uint8_t index = 0;
    String s = String(abs(value), decimals);
    for (int i = 0; i < s.length(); i++)
    {
      if (index > 2)
      {
        exit;
      }
      switch (s[i])
      {
      case '.':
        displayValue.decimalIndex = index;
        break;

      default:
        displayValue.digits[index] = s[i] - '0';
        index++;
        break;
      }
    }
    return (result);
  }

  // converts a double value to a structure used to set
  // the 3 numeric and the 3 symbol nixies
  // used to display the local IP address
  static void convertDoubleToNumericDisplayValue(double inputValue, DISPLAY_VALUE &displayValue)
  {
    displayValue.decimalIndex = 0;
    displayValue.errorFlag = false;
    displayValue.kiloFlag = false;
    displayValue.minusFlag = false;
    displayValue.plusFlag = false;
    displayValue.wattFlag = false;
    displayValue.overallStatusMinusFlag = false;
    displayValue.overallStatusPlusFlag = false;
    displayValue.digits[0] = 0;
    displayValue.digits[1] = 0;
    displayValue.digits[2] = 0;
    String s = String(abs(inputValue), 0);
    switch (s.length())
    {
    case 1:
      displayValue.digits[2] = s[0] - '0';
      break;

    case 2:
      displayValue.digits[1] = s[0] - '0';
      displayValue.digits[2] = s[1] - '0';
      break;

    case 3:
      displayValue.digits[0] = s[0] - '0';
      displayValue.digits[1] = s[1] - '0';
      displayValue.digits[2] = s[2] - '0';
      break;
    }
  }

  // prints the content of a DISPLAY_VALUE structure
  static void printDisplayValue(DISPLAY_VALUE &value)
  {
    D_print("errorFlag = ");
    D_println(value.errorFlag);
    D_print("kiloFlag = ");
    D_println(value.kiloFlag);
    D_print("negativeFlag = ");
    D_println(value.minusFlag);
    D_print("positiveFlag = ");
    D_println(value.plusFlag);
    D_print("percentageFlag = ");
    D_println(value.percentageFlag);
    D_print("wattFlag = ");
    D_println(value.wattFlag);
    D_print("overallStatusMinusFlag = ");
    D_println(value.overallStatusMinusFlag);
    D_print("overallStatusPlusFlag = ");
    D_println(value.overallStatusPlusFlag);
    D_print("decimalIndex = ");
    D_println(value.decimalIndex);
    D_print("digit[0] = ");
    D_println(value.digits[0]);
    D_print("digit[1] = ");
    D_println(value.digits[1]);
    D_print("digit[2] = ");
    D_println(value.digits[2]);
  }

  // converts rgb values to an integer
  static int rgbToInt(uint8_t red, uint8_t green, uint8_t blue)
  {
    return ((red * 256 * 256) + (green * 256) + blue);
  }

  // converts an integer to rgb values
  static void intToRGB(int value, uint8_t *red, uint8_t *green, uint8_t *blue)
  {
    *blue = value & 255;
    *green = (value >> 8) & 255;
    *red = (value >> 16) & 255;
  }
};