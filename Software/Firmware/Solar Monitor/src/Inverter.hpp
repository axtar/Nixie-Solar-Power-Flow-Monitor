// Inverter.hpp

// provides values from the inverter

// Copyright (C) 2024 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Ethernet.h>
#include <DebugDefs.h>
#include <Settings.h>

class Inverter
{
public:
  Inverter()
  {
    resetValues();
  }

  virtual ~Inverter()
  {
  }

  // returns the defined request interval
  int getRequestInterval() const
  {
    return (INVERTER_POLLINGINTERVAL);
  }

  // returns the battery charge value
  double getBatteryCharge() const
  {
    return (_SOC);
  }

  // returns the battery power value
  double getBatteryPower() const
  {
    return (_P_Akku);
  }

  // returns the grid power value
  double getGridPower() const
  {
    return (_P_Grid);
  }

  // returns the load power value
  double getLoadPower() const
  {
    if (GET_LOADPOWER_METHOD == LOADPOWER_FROMINVERTER)
    {
      return (_P_Load);
    }
    else
    {
      return ((_P_Akku + _P_PV + _P_Grid) * (-1));
    }
  }

  // returns the solar power value
  double getSolarPower() const
  {
    return (_P_PV);
  }

  // returns the overall status
  bool getOverallStatus() const
  {
    if (getGridPower() > 0)
    {
      return (false);
    }
    else
    {
      return (true);
    }
  }

  // gets values from the inverter
  bool requestValues(EthernetClient *client) 
  {
    bool result = false;
    resetValues();
    // connect to the inverter if not connected
    if (!client->connected())
    {
      D_println("Client is disconnected");
      if (!client->connect(INVERTER_IPADDRESS, INVERTER_PORT))
      {
        D_println("Failed to connect to the inverter");
      }
    }
    if (client->connected())
    {
      // send HTTP request
      D_println("Connected to the inverter");
      client->println(F("GET /solar_api/v1/GetPowerFlowRealtimeData.fcgi HTTP/1.1"));
      client->print(F("Host: "));
      client->println(F(INVERTER_IPADDRESS));
      client->println(F("Connection: close"));
      if (client->println() != 0)
      {
        // check HTTP status
        char status[32] = {0};
        client->readBytesUntil('\r', status, sizeof(status));
        if (strcmp(status + 9, "200 OK") == 0)
        {
          D_println("Received status OK");
          // Skip HTTP headers
          char endOfHeaders[] = "\r\n\r\n";
          if (client->find(endOfHeaders))
          {
            D_println("Received response");
            if (!decodeJSON(client))
            {
              result = true;
            }
            // read till end
            while (client->available())
            {
              client->read();
            }
            client->stop();
          }
          else
          {
            D_println(F("Invalid response"));
            client->stop();
          }
        }
        else
        {
          D_print("Received wrong status: ");
          D_println(status);
          client->stop();
        }
      }
      else
      {
        D_println("Failed to send request");
        client->stop();
      }
    }
    return (result);
  }

private:
  double _SOC;    // battery charge
  double _P_Akku; // bettery power
  double _P_Grid; // grid power
  double _P_Load; // load power
  double _P_PV;   // solar power

  // reads and decodes the JSON response
  DeserializationError decodeJSON(EthernetClient *client)
  {
    JsonDocument doc;

    DeserializationError error = deserializeJson(doc, *client);

    if (error)
    {
      D_print("deserializeJson() failed: ");
      D_println(error.c_str());
    }
    else
    {
      JsonObject Body_Data = doc["Body"]["Data"];

      JsonObject Body_Data_Inverters_1 = Body_Data["Inverters"]["1"];
      _SOC = Body_Data_Inverters_1["SOC"];

      JsonObject Body_Data_Site = Body_Data["Site"];
      _P_Akku = powerRoundToZero(Body_Data_Site["P_Akku"]);
      _P_Grid = powerRoundToZero(Body_Data_Site["P_Grid"]);
      _P_Load = powerRoundToZero(Body_Data_Site["P_Load"]);
      _P_PV = powerRoundToZero(Body_Data_Site["P_PV"]);
    }
    return (error);
  }

  // sets values to 0 within a defined range
  double powerRoundToZero(double value) const
  {
    if (abs(value) < POWER_ROUND_TO_ZERO_RANGE)
    {
      return (0);
    }
    else
    {
      return (value);
    }
  }

  // sets all values to zero
  void resetValues()
  {
    _SOC = 0.0;
    _P_Akku = 0.0;
    _P_Grid = 0.0;
    _P_Load = 0.0;
    _P_PV = 0.0;
  }
};