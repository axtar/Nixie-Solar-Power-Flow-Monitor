// main.cpp

// Solar Monitor Firmware

// Copyright (C) 2024 highvoltglow
// Licensed under the MIT License

#include <main.h>

void setup()
{
  // init console
  D_Begin(115200);

  D_print("Initializing controller...");
  int status = controller.begin();
  if (status != ERR_SUCCESS)
  {
    D_print("Controller initialization failed. Error: ");
    D_print(status);
    D_print(" ");
    D_println(Errors::getErrorText(status));
    // error in initialization, stop
    while (true)
      ;
  }
  else
  {
    D_println("Controller successfully initialized");
  }
}

void loop()
{
  if (!controller.process())
  {
    // error in controller process, stop
    D_print("Error in controller process, stopping...");
    while (true)
      ;
  }
}
