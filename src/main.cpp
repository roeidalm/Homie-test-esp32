#include <Homie.h>
#include <IRremote.h>
#include "dataMembers.h"
#include "secTask.h"
#include "TVrack.h"
#include "sensorData.h"
// #include <EEPROM.h>
// const unsigned long COURSE_TIME = 30 * 1000;
// const float CALIBRATION_RATIO = 0.1;
//const byte SHUTTERS_EEPROM_POSITION = 0;

// Shutters

bool sleepFlag = false;

void onHomieEvent(const HomieEvent &event)
{
  switch (event.type)
  {
  case HomieEventType::ABOUT_TO_RESET:
    reset();
    break;
  default:
    break;
  }
}

void loopHandler()
{

  int doorValue = debouncer_door.read();
  int waterValue = debouncer_water.read();

  if (doorValue != lastDoorValue)
  {
    Homie.getLogger() << "Door is now " << (doorValue ? "open" : "closed") << endl;

    doorNode.setProperty("open").send(doorValue ? "true" : "false");
    lastDoorValue = doorValue;
  }
  if (waterValue != lastWaterValue)
  {
    Homie.getLogger() << "Water sensor state is " << (waterValue ? "Leak Detected" : "clear") << endl;

    waterNode.setProperty("clear").send(waterValue ? "true" : "false");
    lastWaterValue = waterValue;
  }

  if ((RELAY_OPEN && !lastDoorValue) || (RELAY_CLOSE && !lastWaterValue))
  {
    if (RELAY_OPEN)
    {
      //send open ir command
    }
    else
    {
      //send close ir command
    }
  }
}

void advertiseSetup()
{
  doorNode.advertise("open");
  waterNode.advertise("clear");
  shutterOpenNode.advertise("on").setName("On").setDatatype("boolean").settable(RelayOpenOnHandler);
  shutterCloseNode.advertise("on").setName("On").setDatatype("boolean").settable(RelayCloseOnHandler);
  abortNode.advertise("on").setName("On").setDatatype("boolean").settable(AbortRelayOnHandler);
}

void setup()
{
  // EEPROM.begin(4);

  Serial.begin(115200);
  Serial << endl
         << endl;

  xTaskCreatePinnedToCore(
      Task1code, /* Function to implement the task */
      "Task1",   /* Name of the task */
      10000,     /* Stack size in words */
      NULL,      /* Task input parameter */
      0,         /* Priority of the task */
      &Task1,    /* Task handle. */
      0);        /* Core where the task should run */

  Homie.disableResetTrigger();
  sensorSetup();
  Homie_setFirmware("my-sensors", "1.0.0");
  Homie.setLoopFunction(loopHandler);
  Homie.onEvent(onHomieEvent);
  advertiseSetup();
  Homie.setup();
}

void loop()
{
  Homie.loop();
}
