// Controller.hpp

// main logic

// Copyright (C) 2024 highvoltglow
// Licensed under the MIT License

#pragma once

#include <esp_mac.h>
#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Adafruit_NeoPixel.h>
#include <Button.hpp>
#include <DebugDefs.h>
#include <Helper.hpp>
#include <Display.hpp>
#include <PIR.hpp>
#include <Inverter.hpp>
#include <Errors.hpp>
#include <Settings.h>

// IO pins
#define PIN_BUTTON1 34
#define PIN_PIR 35
#define PIN_GPSRX 32
#define PIN_GPSTX 33
#define PIN_TEMP 25
#define PIN_HVENABLE 26
#define PIN_HVLED 27
#define PIN_LEDCTL 14
#define PIN_NETACT 12
#define PIN_BLANK 13
#define PIN_DATA 4
#define PIN_STORE 16
#define PIN_SHIFT 17
#define PIN_CS 5
#define PIN_SCK 18
#define PIN_MISO 19
#define PIN_SDA 21
#define PIN_SCL 22
#define PIN_MOSI 23

// shift register store transition
#define STORE_BEGIN LOW
#define STORE_COMMIT HIGH

// number of display boards
#define DISPLAY_COUNT 5

// display number with the overall status
#define OVERALL_DISPLAY_NUMBER 4

enum class backlight_mode
{
  off,
  overall_only,
  full
};

class Controller
{
public:
  Controller() : _button(PIN_BUTTON1)
  {
    _highVoltageOn = true;
    _lastRequestTimestamp = 0;
    _backLight = backlight_mode::off;
    _backLightState = true;
    _lastRotationTimestamp = millis();
    _lastRotationStepTimestamp = 0;
    _rotationStep = 0;
    _rotation = false;

    // initialize displays
    // solar power display
    _displays[0] = new Display(display_type::solar_power, value_type ::watts,
                               PIN_DATA, PIN_STORE, PIN_SHIFT,
                               PIN_BLANK, PIN_LEDCTL);
    _displays[0]->getBacklight()->setRating(rating_step::poor, 0, 250, Helper::rgbToInt(255, 0, 0));
    _displays[0]->getBacklight()->setRating(rating_step::fair, 251, 1500, Helper::rgbToInt(255, 100, 0));
    _displays[0]->getBacklight()->setRating(rating_step::good, 1501, 5000, Helper::rgbToInt(0, 255, 0));
    _displays[0]->getBacklight()->setRating(rating_step::excellent, 5001, 999999, Helper::rgbToInt(0, 153, 153));
    _displays[0]->getBacklight()->setMinLED(24);
    _displays[0]->getBacklight()->setMaxLED(29);

    // battery power display
    _displays[1] = new Display(display_type::battery_power, value_type ::watts,
                               PIN_DATA, PIN_STORE, PIN_SHIFT,
                               PIN_BLANK, PIN_LEDCTL);
    _displays[1]->getBacklight()->setRating(rating_step::poor, 1501, 999999, Helper::rgbToInt(255, 0, 0));
    _displays[1]->getBacklight()->setRating(rating_step::fair, 0, 1500, Helper::rgbToInt(255, 100, 0));
    _displays[1]->getBacklight()->setRating(rating_step::good, -3001, 1, Helper::rgbToInt(0, 255, 0));
    _displays[1]->getBacklight()->setRating(rating_step::excellent, -999999, -3000, Helper::rgbToInt(0, 153, 153));
    _displays[1]->getBacklight()->setMinLED(18);
    _displays[1]->getBacklight()->setMaxLED(23);

    // grid power display
    _displays[2] = new Display(display_type::grid_power, value_type ::watts,
                               PIN_DATA, PIN_STORE, PIN_SHIFT,
                               PIN_BLANK, PIN_LEDCTL);
    _displays[2]->getBacklight()->setRating(rating_step::poor, 1501, 999999, Helper::rgbToInt(255, 0, 0));
    _displays[2]->getBacklight()->setRating(rating_step::fair, 0, 1500, Helper::rgbToInt(255, 100, 0));
    _displays[2]->getBacklight()->setRating(rating_step::good, -3001, 1, Helper::rgbToInt(0, 255, 0));
    _displays[2]->getBacklight()->setRating(rating_step::excellent, -999999, -3000, Helper::rgbToInt(0, 153, 153));
    _displays[2]->getBacklight()->setMinLED(12);
    _displays[2]->getBacklight()->setMaxLED(17);

    // load power display
    _displays[3] = new Display(display_type::load_power, value_type ::watts,
                               PIN_DATA, PIN_STORE, PIN_SHIFT,
                               PIN_BLANK, PIN_LEDCTL);
    _displays[3]->getBacklight()->setRating(rating_step::poor, -999999, -1501, Helper::rgbToInt(255, 0, 0));
    _displays[3]->getBacklight()->setRating(rating_step::fair, -1500, -501, Helper::rgbToInt(255, 100, 0));
    _displays[3]->getBacklight()->setRating(rating_step::good, -500, -251, Helper::rgbToInt(0, 255, 0));
    _displays[3]->getBacklight()->setRating(rating_step::excellent, -250, 0, Helper::rgbToInt(0, 153, 153));
    _displays[3]->getBacklight()->setMinLED(6);
    _displays[3]->getBacklight()->setMaxLED(11);

    // battery charge display
    _displays[4] = new Display(display_type::battery_charge, value_type ::battery_charge,
                               PIN_DATA, PIN_STORE, PIN_SHIFT,
                               PIN_BLANK, PIN_LEDCTL);
    _displays[4]->getBacklight()->setRating(rating_step::poor, 0, 10, Helper::rgbToInt(255, 0, 0));
    _displays[4]->getBacklight()->setRating(rating_step::fair, 11, 50, Helper::rgbToInt(255, 100, 0));
    _displays[4]->getBacklight()->setRating(rating_step::good, 51, 90, Helper::rgbToInt(0, 255, 0));
    _displays[4]->getBacklight()->setRating(rating_step::excellent, 91, 100, Helper::rgbToInt(0, 153, 153));
    _displays[4]->getBacklight()->setMinLED(0);
    _displays[4]->getBacklight()->setMaxLED(3);

    // this display has also the overall status indicator with separate settings for the rating
    _displays[4]->getOverallBacklight()->setRatingColor(overall_rating::poor, Helper::rgbToInt(255, 0, 0));
    _displays[4]->getOverallBacklight()->setRatingColor(overall_rating::fair, Helper::rgbToInt(255, 100, 0));
    _displays[4]->getOverallBacklight()->setRatingColor(overall_rating::good, Helper::rgbToInt(0, 255, 0));
    _displays[4]->getOverallBacklight()->setRatingColor(overall_rating::excellent, Helper::rgbToInt(0, 153, 153));
    _displays[4]->getOverallBacklight()->setMinLED(5);
    _displays[4]->getOverallBacklight()->setMaxLED(5);

    _ledCount = 0;
    for (int i = 0; i < DISPLAY_COUNT; i++)
    {
      _ledCount += _displays[i]->getLedCount();
    }
    _leds = new Adafruit_NeoPixel(_ledCount, PIN_LEDCTL, NEO_GRB + NEO_KHZ800);
    _leds->begin();

    _pir = new PIR(PIN_PIR, PIR_DELAY);
  }

  virtual ~Controller()
  {
    delete (_pir);
    for (int i = 0; i < DISPLAY_COUNT; i++)
    {
      delete (_displays[i]);
      delete (_leds);
    }
  }

  // initialization
  int begin()
  {
    int result = ERR_SUCCESS;
    // init console
    D_Begin(115200);

    // clear leds
    clearLEDs();

    // set high voltage off
    pinMode(PIN_HVENABLE, OUTPUT);
    pinMode(PIN_HVLED, OUTPUT);
    hvOFF();

    // define pin modes
    pinMode(PIN_PIR, INPUT);
    pinMode(PIN_DATA, OUTPUT);
    pinMode(PIN_STORE, OUTPUT);
    pinMode(PIN_SHIFT, OUTPUT);
    pinMode(PIN_BLANK, OUTPUT);
    pinMode(PIN_BUTTON1, INPUT);

    // set blank line to high
    digitalWrite(PIN_BLANK, HIGH);

    clearDisplays();

    // initialize PIR
    _pir->begin();

    // high voltage on
    hvON();

    // initialize ethernet
    int err = initNetwork();
    if (err != ERR_SUCCESS)
    {
      result = err;
    }
    else
    {
      initClient();
    }
    if (result != ERR_SUCCESS)
    {
      // error, shutdown high voltage
      hvOFF();
    }
    return (result);
  }

  // loop
  bool process()
  {
    // check if it is time to rotate symbols to avoid cathode poisoning
    if (millis() - _lastRotationTimestamp > ROTATION_INTERVAL * 60 * 1000)
    {
      rotate();
    }
    else
    {
      // check if it is time to request new values
      if (millis() - _lastRequestTimestamp > INVERTER_POLLINGINTERVAL * 1000)
      {
        // renew DHCP lease if needed
        Ethernet.maintain();
        if (isHVON()) // request values only if HV is on
        {
          // request inverter values
          if (_inverter.requestValues(&_client))
          {
            D_print("PV power: ");
            D_println(_inverter.getSolarPower());
            D_print("Battery Power: ");
            D_println(_inverter.getBatteryPower());
            D_print("Grid Power: ");
            D_println(_inverter.getGridPower());
            D_print("Load Power: ");
            D_println(_inverter.getLoadPower());
            D_print("Battery Charge % ");
            D_println(_inverter.getBatteryCharge());

            // set values and leds
            for (int i = 0; i < DISPLAY_COUNT; i++)
            {
              double value = getValueByDisplayType(_displays[i]->getDisplayType());
              setDisplayValue(i, value, _displays[i]->getValueType());
              switch (_backLight)
              {
              case backlight_mode::off:
                clearLEDs();
                break;

              case backlight_mode::overall_only:
                setOverallBacklight(i);
                break;

              case backlight_mode::full:
                setBacklight(i, value);
                setOverallBacklight(i);
                break;
              }
            }
            // now update all displays
            updateDisplays();

            // update LEDs
            if (_backLight != backlight_mode::off)
            {
              _leds->show();
              _backLightState = true;
            }
          }
          _lastRequestTimestamp = millis();
        }
      }
    }

    // check if button is pressed
    _button.process();
    if (_button.isPressed())
    {
      D_println("Button pressed");
      // jump to next backlight mode
      switch (_backLight)
      {
      case backlight_mode::off:
        _backLight = backlight_mode::overall_only;
        break;

      case backlight_mode::overall_only:
        _backLight = backlight_mode::full;
        break;

      case backlight_mode::full:
        _backLight = backlight_mode::off;
        break;
      }

      if (_backLight != backlight_mode::off)
      {
        if (isHVON())
        {
          setLEDs();
        }
      }
      else
      {
        clearLEDs();
      }
    }
    // check PIR status
    if (_pir->process())
    {
      if (!isHVON())
      {
        // waking up, clear stale values
        clearDisplays();
      }
      hvON();
    }
    else
    {
      hvOFF();
      clearLEDs();
    }
    return (true);
  }

  // rotate symbols
  void rotate()
  {
    if (millis() - _lastRotationStepTimestamp > ROTATION_STEPINTERVAL)
    {
      switch (_rotationStep % 10)
      {
      case 0:
        setSymbols(register_type::mu_sign, register_type::mu_sign, register_type::W_sign);
        break;
      case 1:
        setSymbols(register_type::P_sign, register_type::P_sign, register_type::F_sign);
        break;
      case 2:
        setSymbols(register_type::minus_sign, register_type::minus_sign, register_type::A_sign);
        break;
      case 3:
        setSymbols(register_type::plus_sign, register_type::plus_sign, register_type::overall_plus_sign);
        break;
      case 4:
        setSymbols(register_type::m_sign, register_type::m_sign, register_type::H_sign);
        break;
      case 5:
        setSymbols(register_type::M_sign, register_type::M_sign, register_type::V_sign);
        break;
      case 6:
        setSymbols(register_type::k_sign, register_type::k_sign, register_type::S_sign);
        break;
      case 7:
        setSymbols(register_type::pi_sign, register_type::pi_sign, register_type::F_sign);
        break;
      case 8:
        setSymbols(register_type::percent_sign, register_type::percent_sign, register_type::omega_sign);
        break;
      case 9:
        setSymbols(register_type::n_sign, register_type::n_sign, register_type::A_sign);
        break;
      }
      for (int i = 0; i < DISPLAY_COUNT; i++)
      {
        _displays[i]->setSymbolRegisters();
      }
      _lastRotationStepTimestamp = millis();
      _rotationStep++;
      if (_rotationStep > 20)
      {
        _rotationStep = 0;
        _lastRotationTimestamp = millis();
        updateDisplays();
      }
    }
  }

  // set symbols for rotation
  void setSymbols(register_type symbol0, register_type symbol4, register_type symbol5)
  {
    for (int i = 0; i < DISPLAY_COUNT; i++)
    {
      _displays[i]->setSymbols(symbol0, symbol4, symbol5);
    }
  }

  // get current inverter value by display type
  double getValueByDisplayType(display_type displayType)
  {
    double value = 0.0;
    switch (displayType)
    {
    case display_type::solar_power:
      value = _inverter.getSolarPower();
      break;

    case display_type::battery_power:
      value = _inverter.getBatteryPower();
      break;

    case display_type::grid_power:
      value = _inverter.getGridPower();
      break;

    case display_type::load_power:
      value = _inverter.getLoadPower();
      break;

    case display_type::battery_charge:
      value = _inverter.getBatteryCharge();
      break;
    }
    return (value);
  }

  // turn on high voltage
  void hvON()
  {
    if (!_highVoltageOn)
    {
      _highVoltageOn = true;
      digitalWrite(PIN_HVENABLE, HIGH);
      digitalWrite(PIN_HVLED, HIGH);
    }
  }

  // shutdown high voltage
  void hvOFF()
  {
    if (_highVoltageOn)
    {
      _highVoltageOn = false;
      digitalWrite(PIN_HVENABLE, LOW);
      digitalWrite(PIN_HVLED, LOW);
    }
  }

  // set LED colors for all displays
  void setLEDs()
  {
    for (int i = 0; i < DISPLAY_COUNT; i++)
    {
      switch (_backLight)
      {
      case backlight_mode::overall_only:
        setOverallBacklight(i);
        break;

      case backlight_mode::full:
        double value = getValueByDisplayType(_displays[i]->getDisplayType());
        setBacklight(i, value);
        break;
      }
    }
    _leds->show();
    _backLightState = true;
  }

  // turn of all LEDs
  void clearLEDs()
  {
    if (_backLightState)
    {
      _leds->clear();
      _leds->show();
    }
    _backLightState = false;
  }

  // set the color of all LEDs of a specific display according to a value
  void setBacklight(int displayNumber, double value)
  {
    int color = _displays[displayNumber]->getBacklight()->getColor(value);
    for (int i = _displays[displayNumber]->getBacklight()->getMinLED(); i <= _displays[displayNumber]->getBacklight()->getMaxLED(); i++)
    {
      _leds->setPixelColor(i, color);
    }
  }

  // set the overall status LED color
  void setOverallBacklight(int displayNumber)
  {
    // check if correct display
    if (displayNumber == OVERALL_DISPLAY_NUMBER)
    {
      overall_rating rating;

      // rating rules
      if (_inverter.getGridPower() <= 0 && _inverter.getBatteryCharge() > 99 && _inverter.getBatteryPower() <= 0)
      {
        rating = overall_rating::excellent;
      }
      else if (_inverter.getGridPower() <= 0 && _inverter.getBatteryPower() < 0)
      {
        rating = overall_rating::good;
      }
      else if (_inverter.getGridPower() <= 0 && _inverter.getBatteryPower() >= 0)
      {
        rating = overall_rating::fair;
      }
      else
      {
        rating = overall_rating::poor;
      }

      // set color for the overall status LED according to the rating
      int color = _displays[displayNumber]->getOverallBacklight()->getRatingColor(rating);
      for (int i = _displays[displayNumber]->getOverallBacklight()->getMinLED(); i <= _displays[displayNumber]->getOverallBacklight()->getMaxLED(); i++)
      {
        _leds->setPixelColor(i, color);
      }
    }
  }

  // returns if the high voltage is turned on
  bool isHVON()
  {
    return (_highVoltageOn);
  }

  // sets the value to display on the given display board
  void setDisplayValue(int displayNumber, double value, value_type valueType)
  {
    DISPLAY_VALUE displayValue = {0};

    _displays[displayNumber]->clear();
    switch (valueType)
    {
    case value_type::watts:
      Helper::convertDoubleToDisplayValue(value, displayValue);
      _displays[displayNumber]->setValues(displayValue);
      break;

    case value_type::battery_charge:
      Helper::convertDoubleToDisplayValue(value, displayValue, true);
      displayValue.overallStatusPlusFlag = _inverter.getOverallStatus();
      displayValue.overallStatusMinusFlag = !displayValue.overallStatusPlusFlag;
      _displays[displayNumber]->setValues(displayValue);
      break;

    case value_type::number:
      Helper::convertDoubleToNumericDisplayValue(value, displayValue);
      _displays[displayNumber]->setValues(displayValue);
    }
  }

  // clear the value to display on the given display board
  void clearDisplayValue(int displayNumber)
  {
    _displays[displayNumber]->clear();
  }

  // clear all display boards
  void clearDisplays()
  {

    digitalWrite(PIN_STORE, STORE_BEGIN);
    for (int i = 0; i < DISPLAY_COUNT; i++)
    {
      _displays[i]->clear();
      _displays[i]->clearRegisters();
    }
    digitalWrite(PIN_STORE, STORE_COMMIT);
  }

  // updates all display boards
  void updateDisplays()
  {
    digitalWrite(PIN_STORE, STORE_BEGIN);
    for (int i = 0; i < DISPLAY_COUNT; i++)
    {
      _displays[i]->setRegisters();
    }
    digitalWrite(PIN_STORE, STORE_COMMIT);
  }

private:
  bool _highVoltageOn;
  byte _mac[6];
  EthernetClient _client;
  Inverter _inverter;
  unsigned long _lastRequestTimestamp;
  unsigned long _lastRotationTimestamp;
  unsigned long _lastRotationStepTimestamp;
  int _rotationStep;
  bool _rotation;
  backlight_mode _backLight;
  bool _backLightState;
  Button _button;
  Display *_displays[DISPLAY_COUNT];
  Adafruit_NeoPixel *_leds;
  uint8_t _ledCount;
  PIR *_pir;

  // initializes the network hardware
  uint8_t initNetwork()
  {
    int result = ERR_SUCCESS;

    D_println("Initializing network...");
    D_println("Getting mac address");
    // get mac address, this ESP32 type provides an ethernet mac address
    esp_err_t err = esp_read_mac(_mac, ESP_MAC_ETH);
    if (err == ESP_OK)
    {
      char buffer[18] = {0};
      sprintf(buffer, "%2X:%2X:%2X:%2X:%2X:%2X",
              _mac[0], _mac[1], _mac[2], _mac[3], _mac[4], _mac[5]);
      D_print("MAC Address: ");
      D_println(buffer);

      // init Ethernet lib using DHCP
      D_println("Initializing ethernet port...");
      Ethernet.init(PIN_CS);
      D_println("Getting IP address using DHCP...");
      if (Ethernet.begin(_mac) != 0)
      {
        // give time to initalize
        delay(1000);
        D_print("DHCP assigned IP: ");
        D_println(Ethernet.localIP());
        D_print("Gateway: ");
        D_println(Ethernet.gatewayIP());
        D_print("DNS Server ");
        D_println(Ethernet.dnsServerIP());
        // display IP address
        setDisplayValue(0, (double)Ethernet.localIP()[0], value_type::number);
        setDisplayValue(1, (double)Ethernet.localIP()[1], value_type::number);
        setDisplayValue(2, (double)Ethernet.localIP()[2], value_type::number);
        setDisplayValue(3, (double)Ethernet.localIP()[3], value_type::number);
        clearDisplayValue(4);
        updateDisplays();
        delay(1000);
        result = ERR_SUCCESS;
      }
      else
      {
        D_println("Failed to initialize network using DHCP");
        if (Ethernet.hardwareStatus() == EthernetNoHardware)
        {
          D_println("Ethernet port not found");
          result = ERR_ETHERNET_NOHARDWARE;
        }
        else
        {
          if (Ethernet.linkStatus() == LinkOFF)
          {
            D_println("Ethernet cable not connected");
            result = ERR_ETHERNET_LINKOFF;
          }
        }
      }
    }
    else
    {
      D_println("Failed to retrieve mac address");
      result = ERR_ETHERNET_NOMAC;
    }
    return (result);
  }

  // initialize ethernet client
  bool initClient()
  {
    _client.setTimeout(10000);
    return (true);
  }
};
