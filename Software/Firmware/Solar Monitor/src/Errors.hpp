// Errors.hpp

// Definition of error codes

// Copyright (C) 2024 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>

// error codes
#define ERR_SUCCESS 0
#define ERR_ETHERNET_NOMAC 1
#define ERR_ETHERNET_NOHARDWARE 2
#define ERR_ETHERNET_LINKOFF 3

class Errors
{
public:
  Errors() 
  {
  };

  // returns error description
  static String getErrorText(int errorCode)
  {
    String text;

    switch (errorCode)
    {
    case ERR_SUCCESS:
      text = "Success";
      break;

    case ERR_ETHERNET_NOMAC:
      text = "No mac address";
      break;

    case ERR_ETHERNET_NOHARDWARE:
      text = "No ethernet hardware";
      break;

    case ERR_ETHERNET_LINKOFF:
      text = "Ethernet link down0";
      break;
    }
    return (text);
  }
};
