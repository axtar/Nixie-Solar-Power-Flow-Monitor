// OverallBacklight.hpp

// Definition of backlight colors and ratings for overall status

// Copyright (C) 2024 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <DebugDefs.h>

// ratings for the overall status
enum class overall_rating
{
  poor,
  fair,
  good,
  excellent
};

#define RATING_COUNT 4 // modify if changing overall_rating

typedef struct
{
  int color;
} overall_RATING;

class overallBacklight
{
public:
  overallBacklight()
  {
    for (int i = 0; i < RATING_COUNT; i++)
    {
      _ratings[i].color = 0;
    }
    _minLED = -1;
    _maxLED = -1;
  }

  virtual ~overallBacklight()
  {
  }

  // set LED color for rating
  void setRatingColor(overall_rating overallRating, int color)
  {
    if (((int)overallRating >= 0) && ((int)overallRating <= RATING_COUNT))
    {
      _ratings[(int)overallRating].color = color;
    }
  }

  // returns LED color for rating
  int getRatingColor(overall_rating overallRating)
  {
    int color = 0;
    if (((int)overallRating >= 0) && ((int)overallRating <= RATING_COUNT))
    {
      color = _ratings[(int)overallRating].color;
    }
    return (color);
  }

  // set the first LED number in the LED array section of the overall status display
  void setMinLED(int minLED)
  {
    _minLED = minLED;
  }

  // set the last LED number in the LED array section of the overall status display
  void setMaxLED(int maxLED)
  {
    _maxLED = maxLED;
  }

  // get the first LED number in the LED array section of the overall status display
  int getMinLED()
  {
    return (_minLED);
  }

  // get the first LED number in the LED array section of the overall status display
  int getMaxLED()
  {
    return (_maxLED);
  }

private:
  overall_RATING _ratings[RATING_COUNT];
  int _minLED;
  int _maxLED;
};