/*

# Homie enabled Sonoff Dual shutters

Requires the Shutters library:
https://github.com/marvinroger/arduino-shutters
and the SonoffDual library:
https://github.com/marvinroger/arduino-sonoff-dual

## Features

* Do a short press to close shutters
if level != 0 or open shutters if level == 0
* Do a long press to reset

*/

#include <Homie.h>

#include <EEPROM.h>

const unsigned long COURSE_TIME = 30 * 1000;
const float CALIBRATION_RATIO = 0.1;

bool RELAY_OPEN = false;
bool RELAY_CLOSE = false;

const int PIN_DOOR = 1;
const int PIN_WATER = 2;
const byte SHUTTERS_EEPROM_POSITION = 0;
const int LED_PIN = 5;

bool sleepFlag = false;

Bounce debouncer_door = Bounce();  // Bounce is built into Homie, so you can use it without including it first
Bounce debouncer_water = Bounce(); // Bounce is built into Homie, so you can use it without including it first

int lastDoorValue = -1;
int lastWaterValue = -1;

HomieNode doorNode("door", "door", "door");
HomieNode waterNode("water", "water", "water");

// HomieNode shuttersNode("shutters", "Shutters", "shutters");
HomieNode shutterOpenNode("shutterOpenNode", "ShutterOpenNode", "switch");
HomieNode shutterCloseNode("shutterCloseNode", "ShutterCloseNode", "switch");

// Shutters

void shuttersOpen()
{
  RELAY_OPEN = true;
}

void shuttersClose()
{
  RELAY_CLOSE = true;
}

void shuttersHalt()
{
  RELAY_OPEN = false;
  RELAY_CLOSE = false;
}

// uint8_t shuttersGetState()
// {
//   return EEPROM.read(SHUTTERS_EEPROM_POSITION);
// }

// void shuttersSetState(uint8_t state)
// {
//   EEPROM.write(SHUTTERS_EEPROM_POSITION, state);
//   EEPROM.commit();
// }

// void onShuttersLevelReached(uint8_t level)
// {
//   if (shutters.isIdle())
//     Homie.setIdle(true); // if idle, we've reached our target
//   if (Homie.isConnected())
//     shuttersNode.setProperty("level").send(String(level));
// }

void reset()
{
  shuttersHalt();
  //TODO
}
// Homie

void onHomieEvent(const HomieEvent &event)
{
  switch (event.type)
  {
  case HomieEventType::ABOUT_TO_RESET:
    reset();
    break;
    // case HomieEventType::MQTT_READY:
    //   Homie.getLogger() << "MQTT connected, preparing for deep sleep..." << endl;
    //   Homie.prepareToSleep();
    //   break;
    // case HomieEventType::READY_TO_SLEEP:
    //   Homie.getLogger() << "Ready to sleep" << endl;
    //   Homie.doDeepSleep();
    //   break;
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

bool RelayOpenOnHandler(const HomieRange &range, const String &value)
{
  if (value != "true" && value != "false")
    return false;

  RELAY_OPEN = (value == "true");
  shutterOpenNode.setProperty("on").send(value);
  Homie.getLogger() << "Light is " << (RELAY_OPEN ? "on" : "off") << endl;

  return true;
}
bool RelayCloseOnHandler(const HomieRange &range, const String &value)
{
  if (value != "true" && value != "false")
    return false;

  RELAY_CLOSE = (value == "true");
  shutterCloseNode.setProperty("on").send(value);
  Homie.getLogger() << "Light is " << (RELAY_CLOSE ? "on" : "off") << endl;

  return true;
}
void setup()
{
  EEPROM.begin(4);

  Serial.begin(115200);
  Serial << endl
         << endl;
  Homie.disableResetTrigger();
  pinMode(LED_PIN, INPUT);
  digitalWrite(LED_PIN, HIGH);
  pinMode(PIN_DOOR, INPUT);
  pinMode(PIN_WATER, INPUT);
  digitalWrite(PIN_DOOR, HIGH);
  digitalWrite(PIN_WATER, HIGH);
  debouncer_door.attach(PIN_DOOR);
  debouncer_water.attach(PIN_WATER);
  debouncer_door.interval(50);
  debouncer_water.interval(50);
  Homie_setFirmware("my-sensors", "1.0.0");
  Homie.setLoopFunction(loopHandler);
  doorNode.advertise("open");
  waterNode.advertise("clear");
  shutterOpenNode.advertise("on").setName("On").setDatatype("boolean").settable(RelayOpenOnHandler);
  shutterCloseNode.advertise("on").setName("On").setDatatype("boolean").settable(RelayCloseOnHandler);

  Homie.onEvent(onHomieEvent);
  Homie.setup();
}

void loop()
{
  Homie.loop();
  // if (false)
  // {
  //   Homie.reset();
  //   Homie.setIdle(false);
  // }
}
