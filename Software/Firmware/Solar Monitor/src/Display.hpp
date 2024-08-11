// Display.hpp

// class for driving the nixie display

// Copyright (C) 2024 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <DisplayHAL.hpp>
#include <Structs.h>
#include <DebugDefs.h>
#include <Backlight.hpp>
#include <OverallBacklight.hpp>

#define DIGIT_OFF 255

// shift transition for shift registers
#define SHIFT_BEGIN HIGH
#define SHIFT_COMMIT LOW

// value types
enum class value_type
{
  watts,
  battery_charge,
  number
};

// types of displays
enum class display_type
{
  solar_power,
  battery_power,
  grid_power,
  load_power,
  battery_charge
};

enum class sign_state
{
  off,
  on
};

enum class decimal_point_state
{
  off,
  on
};

class Display
{
public:
  // constructor sets types and IO pins
  Display(display_type displayType, value_type valueType, uint8_t dataPin, uint8_t storePin,
          uint8_t shiftPin, uint8_t blankPin,
          uint8_t ledCtlPin) : _displayType(displayType),
                               _valueType(valueType),
                               _dataPin(dataPin),
                               _storePin(storePin),
                               _shiftPin(shiftPin),
                               _blankPin(blankPin),
                               _ledCtlPin(ledCtlPin)

  {
    _displayHAL = new DisplayHAL();

    _backlight = new Backlight();

    _ledCount = _displayHAL->getLedCount();
    _digitCount = _displayHAL->getDigitCount();
    _decimalPointCount = _displayHAL->getDecimalPointCount();
    _registerCount = _displayHAL->getRegisterCount();

    // array of digits
    // digit 0 is the most left nixie
    _digits = new uint8_t[_digitCount];

    // array of decimal points
    // decimal points are on the right side of the digits
    _decimalPoints = new decimal_point_state[_decimalPointCount];

    // set signs off
    _minusSign = sign_state::off;
    _plusSign = sign_state::off;
    _kSign = sign_state::off;
    _WSign = sign_state::off;
    _percentSign = sign_state::off;
    _FSign = sign_state::off;
    _overallStatusMinusSign = sign_state::off;
    _overallStatusMinusSign = sign_state::off;

    // on the battery charge display there is also the overall status nixie
    if (_displayType == display_type::battery_charge)
    {
      _overallBacklight = new overallBacklight();
    }
  }

  virtual ~Display()
  {
    // clean up
    delete _displayHAL;
    delete _backlight;
    if (_displayType == display_type::battery_charge)
    {
      delete _overallBacklight;
    }
    delete (_digits);
    delete (_decimalPoints);
  }

  uint8_t getLedCount()
  {
    return (_ledCount);
  }

  // set internal variables
  void setValues(DISPLAY_VALUE &displayValue)
  {

    if (displayValue.errorFlag)
    {
      clearRegisters();
      _FSign = sign_state::on;
    }
    else
    {
      // set signs
      _minusSign = displayValue.minusFlag ? sign_state::on : sign_state::off;
      _plusSign = displayValue.plusFlag ? sign_state::on : sign_state::off;
      _percentSign = displayValue.percentageFlag ? sign_state::on : sign_state::off;
      _kSign = displayValue.kiloFlag ? sign_state::on : sign_state::off;
      _WSign = displayValue.wattFlag ? sign_state::on : sign_state::off;
      _FSign = sign_state::off;
      _overallStatusMinusSign = displayValue.overallStatusMinusFlag ? sign_state::on : sign_state::off;
      _overallStatusPlusSign = displayValue.overallStatusPlusFlag ? sign_state::on : sign_state::off;

      // set decimal points
      switch (displayValue.decimalIndex)
      {
      case 0: // no decimal points
        break;

      default:
        // shifted by one
        _decimalPoints[displayValue.decimalIndex - 1] = decimal_point_state::on;
        break;
      }

      // set digits
      for (int i = 0; i < _digitCount; i++)
      {
        _digits[i] = displayValue.digits[i];
      }
    }
  }

  // used for symbol rotation
  void setSymbols(register_type symbol0, register_type symbol4, register_type symbol5)
  {
    _symbolDigit0 = symbol0;
    _symbolDigit4 = symbol4;
    _symbolDigit5 = symbol5;
  }

  // set registers for symbol rotation, keeps the numeric values untouched
  void setSymbolRegisters()
  {
    register_type regType;
    uint8_t digit = 0;
    uint8_t number = 0;

    for (uint8_t i = _registerCount; i > 0; i--)
    {
      regType = _displayHAL->getRegisterInfo(i, &digit, &number);
      switch (digit)
      {
      case 0:
        if (regType == _symbolDigit0)
        {
          commitBit(HIGH);
        }
        else
        {
          commitBit(LOW);
        }
        break;

      case 4:
        if (regType == _symbolDigit4)
        {
          commitBit(HIGH);
        }
        else
        {
          commitBit(LOW);
        }
        break;

      case 5:
        if (regType == _symbolDigit5)
        {
          commitBit(HIGH);
        }
        else
        {
          commitBit(LOW);
        }
        break;

      case 1:
      case 2:
      case 3:
        if (regType == register_type::decimal_point)
        {
          commitDecimalPoint(digit);
        }
        if (regType == register_type::number)
        {
          commitDigitNumber(digit, number);
        }
      }
    }
  }

  // clears all registers
  void clearRegisters()
  {
    for (uint8_t i = _registerCount; i > 0; i--)
    {
      commitBit(LOW);
    }
  }

  // set registers for digits, decimal points and signs
  void setRegisters()
  {
    register_type regType;
    uint8_t digit = 0;
    uint8_t number = 0;

    for (uint8_t i = _registerCount; i > 0; i--)
    {
      // get information about what is connected to a register
      regType = _displayHAL->getRegisterInfo(i, &digit, &number);
      switch (regType)
      {
      case register_type::unknown:
        break;

      // this signs are for digit 0
      case register_type::percent_sign:
      case register_type::plus_sign:
      case register_type::minus_sign:
        if (digit == 0)
        {
          commitSign(regType);
          // commitBit(LOW);
        }
        else
        {
          commitBit(LOW);
        }
        break;
        // this sign is for digit 4
#ifdef OLD_BOARDS
      case register_type::overall_minus_sign:
#endif
      case register_type::k_sign:
        if (digit == 4)
        {
          commitSign(regType);
        }
        else
        {
          commitBit(LOW);
        }
        break;

      // this sign is for digit 5
      case register_type::overall_plus_sign:
#ifndef OLD_BOARDS
      case register_type::overall_minus_sign:
#endif
      case register_type::W_sign:
      case register_type::F_sign:
        if (digit == 5)
        {
          commitSign(regType);
        }
        else
        {
          commitBit(LOW);
        }
        break;

      case register_type::decimal_point:
        commitDecimalPoint(digit);
        break;

      case register_type::number:
        commitDigitNumber(digit, number);
        break;

      case register_type::not_used:
      case register_type::not_connected:
        commitBit(LOW);
        break;

      default:
        commitBit(LOW);
        break;
      }
    }
  }

  // clear variables
  void clear()
  {
    // clear decimal points
    for (uint8_t i = 0; i < _decimalPointCount; i++)
    {
      _decimalPoints[i] = decimal_point_state::off;
    }
    for (uint8_t i = 0; i < _digitCount; i++)
    {
      _digits[i] = DIGIT_OFF;
    }
    _minusSign = sign_state::off;
    _plusSign = sign_state::off;
    _percentSign = sign_state::off;
    _WSign = sign_state::off;
    _kSign = sign_state::off;
    _FSign = sign_state::off;
    _overallStatusMinusSign = sign_state::off;
    _overallStatusPlusSign = sign_state::off;
  }

  // returns the display type
  display_type getDisplayType()
  {
    return (_displayType);
  }

  // returns the value type
  value_type getValueType()
  {
    return (_valueType);
  }

  // provides access to backlight
  Backlight *getBacklight()
  {
    return (_backlight);
  }

  // provides acces to overall status backlight
  overallBacklight *getOverallBacklight()
  {
    return (_overallBacklight);
  }

private:
  DisplayHAL *_displayHAL;
  Backlight *_backlight;
  overallBacklight *_overallBacklight;
  value_type _valueType;
  display_type _displayType;

  // pins
  uint8_t _dataPin;
  uint8_t _storePin;
  uint8_t _shiftPin;
  uint8_t _blankPin;
  uint8_t _ledCtlPin;

  // display hardware
  uint8_t _ledCount;
  uint8_t _digitCount;
  uint8_t _decimalPointCount;
  uint8_t _registerCount;

  // digits and symbols
  uint8_t *_digits;
  decimal_point_state *_decimalPoints;
  sign_state _minusSign;
  sign_state _plusSign;
  sign_state _kSign;
  sign_state _WSign;
  sign_state _percentSign;
  sign_state _FSign;
  sign_state _overallStatusPlusSign;
  sign_state _overallStatusMinusSign;

  // symbol store for cathode poisoning prevention
  register_type _symbolDigit0;
  register_type _symbolDigit4;
  register_type _symbolDigit5;

  // commit number to shift registers
  void commitDigitNumber(uint8_t digit, uint8_t number)
  {
    // digit is shifted by one
    if (_digits[digit - 1] == number)
    {
      commitBit(HIGH);
    }
    else
    {
      commitBit(LOW);
    }
  }

  // commit decimal point to shift registers
  void commitDecimalPoint(uint8_t decimalPoint)
  {
    if (_decimalPoints[decimalPoint - 1] == decimal_point_state::off)
    {
      commitBit(LOW);
    }
    else
    {
      commitBit(HIGH);
    }
  }

  // commit sign to shift registers
  void commitSign(register_type regType)
  {
    switch (regType)
    {
    case register_type::plus_sign:
      commitBit(_plusSign == sign_state::off ? LOW : HIGH);
      break;

    case register_type::minus_sign:
      commitBit(_minusSign == sign_state::off ? LOW : HIGH);
      break;

    case register_type::percent_sign:
      commitBit(_percentSign == sign_state::off ? LOW : HIGH);
      break;

    case register_type::W_sign:
      commitBit(_WSign == sign_state::off ? LOW : HIGH);
      break;

    case register_type::k_sign:
      commitBit(_kSign == sign_state::off ? LOW : HIGH);
      break;

    case register_type::F_sign:
      commitBit(_FSign == sign_state::off ? LOW : HIGH);
      break;

    case register_type::overall_minus_sign:
      commitBit(_overallStatusMinusSign == sign_state::off ? LOW : HIGH);
      break;

    case register_type::overall_plus_sign:
      commitBit(_overallStatusPlusSign == sign_state::off ? LOW : HIGH);
      break;
    }
  }

  // commit bit to shift registers
  void commitBit(uint8_t value)
  {
    digitalWrite(_shiftPin, SHIFT_BEGIN);
    digitalWrite(_dataPin, value);
    // delayMicroseconds(5);
    digitalWrite(_shiftPin, SHIFT_COMMIT);
  }
};
