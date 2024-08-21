// Button.hpp

// class for using OneButton in a class
// based on the FunctionalButton example

// Copyright (C) 2024 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <OneButton.h>

class Button
{
private:
  OneButton _button;
  bool _isPressed;

public:
  explicit Button(uint8_t pin) : _button(pin)
  {
    // attaching callback
    _button.attachClick([](void *scope)
                        { ((Button *)scope)->pressed(); }, this);
  }

  // called if button is pressed
  void pressed()
  {
    _isPressed = true;
  }

  // returns if button has been pressed
  bool isPressed() const
  {
    return (_isPressed);
  }

  // check for button events
  void process()
  {
    _isPressed = false;
    _button.tick();
  }
};