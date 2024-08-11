// main.h

// Solar Monitor Firmware

// Copyright (C) 2024 highvoltglow
// Licensed under the MIT License

#pragma once

//- INCLUDES
#include <Arduino.h>
#include <Controller.hpp>
#include <PIR.hpp>

PIR pir(PIN_PIR, PIR_DELAY);
Controller controller(&pir);
