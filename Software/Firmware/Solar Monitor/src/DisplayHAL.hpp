// displayHAL.hpp

// provides information about the installed display hardware

// Copyright (C) 2024 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>

#define REGISTERCOUNT 64    // number of registers per board
#define DIGITCOUNT 3        // number of digits
#define LEDCOUNT 6          // number of LEDS per board
#define DECIMALPOINTCOUNT 2 // number of decimal points per board

// comment out for new display and driver board versions
// #define OLD_BOARDS

// type connected to a shift register output
enum class register_type : uint8_t
{
  unknown,
  not_connected,
  not_used,
  decimal_point,
  number,            // IN-12A
  mu_sign,           // IN-15A
  pi_sign,           // IN-15A
  P_sign,            // IN-15A
  M_sign,            // IN-15A
  m_sign,            // IN-15A
  k_sign,            // IN-15A
  n_sign,            // IN-15A
  percent_sign,      // IN-15A
  plus_sign,         // IN-15A
  minus_sign,        // IN-15A
  omega_sign,        // IN-15B
  H_sign,            // IN-15B
  A_sign,            // IN-15B
  S_sign,            // IN-15B
  V_sign,            // IN-15B
  Hz_sign,           // IN-15B
  F_sign,            // IN-15B
  W_sign,            // IN-15B
  overall_plus_sign, // IN-15A
  overall_minus_sign // IN-15A
};

// translation table contents
typedef struct
{
  register_type rt;
  uint8_t digit;
  uint8_t number;
} TRANSLATION_TABLE_ENTRY;

class DisplayHAL
{
public:
  DisplayHAL()
  {
    initTranslationTable();
  }

  virtual ~DisplayHAL()
  {
    free(_translationTable);
  }

  // returns the number of registers
  uint8_t getRegisterCount() const
  {
    return (REGISTERCOUNT);
  }

  // returns the number of digits
  uint8_t getDigitCount() const
  {
    return (DIGITCOUNT);
  }

  // returns the number of decimal point neons
  uint8_t getDecimalPointCount() const
  {
    return (DECIMALPOINTCOUNT);
  }

  // returns the number of LEDs
  uint8_t getLedCount() const
  {
    return (LEDCOUNT);
  }

  // returns information about what is connected to a specific shift register output
  register_type getRegisterInfo(uint8_t registerNumber, uint8_t *digit, uint8_t *number) const
  {
    *digit = 0;
    *number = 0;

    register_type regType;

    if ((registerNumber < 1) || (registerNumber > REGISTERCOUNT))
    {
      regType = register_type::unknown;
    }
    else
    {
      regType = _translationTable[registerNumber - 1].rt;
      *digit = _translationTable[registerNumber - 1].digit;
      *number = _translationTable[registerNumber - 1].number;
    }
    return (regType);
  }

private:
#ifdef OLD_BOARDS
  // translation table for old board version
  void initTranslationTable()
  {
    _translationTable = (TRANSLATION_TABLE_ENTRY *)malloc(sizeof(TRANSLATION_TABLE_ENTRY) * REGISTERCOUNT);
    _translationTable[0] = {register_type::number, 2, 8};
    _translationTable[1] = {register_type::number, 2, 7};
    _translationTable[2] = {register_type::number, 2, 6};
    _translationTable[3] = {register_type::number, 2, 5};
    _translationTable[4] = {register_type::number, 2, 4};
    _translationTable[5] = {register_type::number, 2, 3};
    _translationTable[6] = {register_type::number, 1, 8};
    _translationTable[7] = {register_type::number, 1, 7};
    _translationTable[8] = {register_type::number, 1, 6};
    _translationTable[9] = {register_type::number, 1, 5};
    _translationTable[10] = {register_type::number, 1, 4};
    _translationTable[11] = {register_type::number, 1, 3};
    _translationTable[12] = {register_type::minus_sign, 0, 0};
    _translationTable[13] = {register_type::plus_sign, 0, 0};
    _translationTable[14] = {register_type::m_sign, 0, 0};
    _translationTable[15] = {register_type::M_sign, 0, 0};
    _translationTable[16] = {register_type::k_sign, 0, 0};
    _translationTable[17] = {register_type::pi_sign, 0, 0};
    _translationTable[18] = {register_type::percent_sign, 0, 0};
    _translationTable[19] = {register_type::n_sign, 0, 0};
    _translationTable[20] = {register_type::mu_sign, 0, 0};
    _translationTable[21] = {register_type::P_sign, 0, 0};
    _translationTable[22] = {register_type::number, 1, 2};
    _translationTable[23] = {register_type::number, 1, 1};
    _translationTable[24] = {register_type::number, 1, 0};
    _translationTable[25] = {register_type::number, 1, 9};
    _translationTable[26] = {register_type::decimal_point, 1, 0};
    _translationTable[27] = {register_type::number, 2, 2};
    _translationTable[28] = {register_type::number, 2, 1};
    _translationTable[29] = {register_type::number, 2, 0};
    _translationTable[30] = {register_type::number, 2, 9};
    _translationTable[31] = {register_type::decimal_point, 2, 0};
    _translationTable[32] = {register_type::number, 3, 2};
    _translationTable[33] = {register_type::number, 3, 1};
    _translationTable[34] = {register_type::number, 3, 0};
    _translationTable[35] = {register_type::number, 3, 9};
    _translationTable[36] = {register_type::not_connected, 0, 0};
    _translationTable[37] = {register_type::not_connected, 0, 0};
    _translationTable[38] = {register_type::percent_sign, 4, 0};
    _translationTable[39] = {register_type::n_sign, 4, 0};
    _translationTable[40] = {register_type::mu_sign, 4, 0};
    _translationTable[41] = {register_type::P_sign, 4, 0};
    _translationTable[42] = {register_type::not_connected, 0, 0};
    _translationTable[43] = {register_type::not_connected, 0, 0};
    _translationTable[44] = {register_type::omega_sign, 5, 0};
    _translationTable[45] = {register_type::A_sign, 5, 0};
    _translationTable[46] = {register_type::W_sign, 5, 0};
    _translationTable[47] = {register_type::F_sign, 5, 0};
    _translationTable[48] = {register_type::overall_plus_sign, 5, 0}; // plus_sign if IN-15A is installed
    _translationTable[49] = {register_type::H_sign, 5, 0};
    _translationTable[50] = {register_type::V_sign, 5, 0};
    _translationTable[51] = {register_type::S_sign, 5, 0};
    _translationTable[52] = {register_type::overall_minus_sign, 4, 0}; // butch wire to (minus_sign, 5, 0)
    _translationTable[53] = {register_type::plus_sign, 4, 0};
    _translationTable[54] = {register_type::m_sign, 4, 0};
    _translationTable[55] = {register_type::M_sign, 4, 0};
    _translationTable[56] = {register_type::k_sign, 4, 0};
    _translationTable[57] = {register_type::pi_sign, 4, 0};
    _translationTable[58] = {register_type::number, 3, 8};
    _translationTable[59] = {register_type::number, 3, 7};
    _translationTable[60] = {register_type::number, 3, 6};
    _translationTable[61] = {register_type::number, 3, 5};
    _translationTable[62] = {register_type::number, 3, 4};
    _translationTable[63] = {register_type::number, 3, 3};
  }

#else
  // translation table for new board versions
  void initTranslationTable()
  {
    _translationTable = (TRANSLATION_TABLE_ENTRY *)malloc(sizeof(TRANSLATION_TABLE_ENTRY) * REGISTERCOUNT);
    _translationTable[0] = {register_type::number, 2, 8};
    _translationTable[1] = {register_type::number, 2, 7};
    _translationTable[2] = {register_type::number, 2, 6};
    _translationTable[3] = {register_type::number, 2, 5};
    _translationTable[4] = {register_type::number, 2, 4};
    _translationTable[5] = {register_type::number, 2, 3};
    _translationTable[6] = {register_type::number, 1, 8};
    _translationTable[7] = {register_type::number, 1, 7};
    _translationTable[8] = {register_type::number, 1, 6};
    _translationTable[9] = {register_type::number, 1, 5};
    _translationTable[10] = {register_type::number, 1, 4};
    _translationTable[11] = {register_type::number, 1, 3};
    _translationTable[12] = {register_type::minus_sign, 0, 0};
    _translationTable[13] = {register_type::plus_sign, 0, 0};
    _translationTable[14] = {register_type::m_sign, 0, 0};
    _translationTable[15] = {register_type::M_sign, 0, 0};
    _translationTable[16] = {register_type::k_sign, 0, 0};
    _translationTable[17] = {register_type::pi_sign, 0, 0};
    _translationTable[18] = {register_type::percent_sign, 0, 0};
    _translationTable[19] = {register_type::n_sign, 0, 0};
    _translationTable[20] = {register_type::mu_sign, 0, 0};
    _translationTable[21] = {register_type::P_sign, 0, 0};
    _translationTable[22] = {register_type::number, 1, 2};
    _translationTable[23] = {register_type::number, 1, 1};
    _translationTable[24] = {register_type::number, 1, 0};
    _translationTable[25] = {register_type::number, 1, 9};
    _translationTable[26] = {register_type::decimal_point, 1, 0};
    _translationTable[27] = {register_type::number, 2, 2};
    _translationTable[28] = {register_type::number, 2, 1};
    _translationTable[29] = {register_type::number, 2, 0};
    _translationTable[30] = {register_type::number, 2, 9};
    _translationTable[31] = {register_type::decimal_point, 2, 0};
    _translationTable[32] = {register_type::number, 3, 2};
    _translationTable[33] = {register_type::number, 3, 1};
    _translationTable[34] = {register_type::number, 3, 0};
    _translationTable[35] = {register_type::number, 3, 9};
    _translationTable[36] = {register_type::not_connected, 0, 0};
    _translationTable[37] = {register_type::not_connected, 0, 0};
    _translationTable[38] = {register_type::percent_sign, 4, 0};
    _translationTable[39] = {register_type::n_sign, 4, 0};
    _translationTable[40] = {register_type::mu_sign, 4, 0};
    _translationTable[41] = {register_type::P_sign, 4, 0};
    _translationTable[42] = {register_type::omega_sign, 5, 0};
    _translationTable[43] = {register_type::A_sign, 5, 0};
    _translationTable[44] = {register_type::W_sign, 5, 0};
    _translationTable[45] = {register_type::F_sign, 5, 0};
    _translationTable[46] = {register_type::overall_minus_sign, 5, 0}; // minus_sign if a IN-15A is installed, not connected for IN-15B
    _translationTable[47] = {register_type::overall_plus_sign, 5, 0};  // plus_sign if an IN-15A is installed
    _translationTable[48] = {register_type::H_sign, 5, 0};
    _translationTable[49] = {register_type::V_sign, 5, 0};
    _translationTable[50] = {register_type::S_sign, 5, 0};
    _translationTable[51] = {register_type::pi_sign, 5, 0}; // if a IN-15A is installed, not connected for IN-15B
    _translationTable[52] = {register_type::minus_sign, 4, 0};
    _translationTable[53] = {register_type::plus_sign, 4, 0};
    _translationTable[54] = {register_type::m_sign, 4, 0};
    _translationTable[55] = {register_type::M_sign, 4, 0};
    _translationTable[56] = {register_type::k_sign, 4, 0};
    _translationTable[57] = {register_type::pi_sign, 4, 0};
    _translationTable[58] = {register_type::number, 3, 8};
    _translationTable[59] = {register_type::number, 3, 7};
    _translationTable[60] = {register_type::number, 3, 6};
    _translationTable[61] = {register_type::number, 3, 5};
    _translationTable[62] = {register_type::number, 3, 4};
    _translationTable[63] = {register_type::number, 3, 3};
  }
#endif
  TRANSLATION_TABLE_ENTRY *_translationTable;
};