// PIR.h

// provides PIR functionality

// Copyright (C) 2024 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <DebugDefs.h>

class PIR
{

public:
  PIR(uint8_t pinPIR, int pirDelay) : _pinPIR(pinPIR), _pirDelay(pirDelay * 1000 * 60)
  {
    _pirTimestamp = 0;
  }

  virtual ~PIR()
  {
  }

  void IRAM_ATTR handlePIRTimeout()
  {
    _pirTimestamp = millis();
  }

  // provides PIR status
  bool process()
  {
    bool result = true;
    if (_pirDelay > 0)
    {
      if (millis() - _pirTimestamp > (_pirDelay))
      {
        result = false;
      }
    }
    return (result);
  }

private:
  uint8_t _pinPIR;
  volatile unsigned long _pirTimestamp;
  unsigned long _pirDelay;
};