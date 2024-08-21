// Backlight.hpp

// Definition of backlight colors and ratings

// Copyright (C) 2024 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <DebugDefs.h>

enum class rating_step
{
  poor,
  fair,
  good,
  excellent
};

#define RATING_COUNT 4 // modify if changing rating_steps

// holds LED color and the value range
typedef struct
{
  int color;
  double min;
  double max;
} RATING;

class Backlight
{
public:
  Backlight()
  {
    for (int i = 0; i < RATING_COUNT; i++)
    {
      _ratings[i].min = 0;
      _ratings[i].max = 0;
      _ratings[i].color = 0;
    }
    _minLED = -1;
    _maxLED = -1;
  }

  virtual ~Backlight()
  {
  }

  // fill rating structure
  void setRating(rating_step ratingStep, double minValue, double maxValue, int color)
  {
    // check limits
    if ((int)ratingStep >= 0 && (int)ratingStep < RATING_COUNT)
    {
      _ratings[(int)ratingStep].min = minValue;
      _ratings[(int)ratingStep].max = maxValue;
      _ratings[(int)ratingStep].color = color;
    }
  }

  // get LED color for a specific value
  int getColor(double value) const
  {
    int color = 0;
    double rounded = round(value);

    for (int i = 0; i < RATING_COUNT; i++)
    {
      if ((rounded >= _ratings[i].min) && (rounded <= _ratings[i].max))
      {
        return (_ratings[i].color);
      }
    }
    return (color);
  }

  // set the first LED number in the LED array section of the display
  void setMinLED(int minLED)
  {
    _minLED = minLED;
  }

  // set last LED number in the LED array section of the display
  void setMaxLED(int maxLED)
  {
    _maxLED = maxLED;
  }

  // get the first LED number in the LED array section of the display
  int getMinLED() const
  {
    return (_minLED);
  }

  // get last LED number in the LED array section of the display
  int getMaxLED() const
  {
    return (_maxLED);
  }

private:
  RATING _ratings[RATING_COUNT];
  int _minLED;
  int _maxLED;
};