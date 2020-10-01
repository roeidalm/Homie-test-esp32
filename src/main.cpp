#include <Homie.h>
#include <IRremote.h>

// #include <EEPROM.h>
// const unsigned long COURSE_TIME = 30 * 1000;
// const float CALIBRATION_RATIO = 0.1;
//const byte SHUTTERS_EEPROM_POSITION = 0;

bool RELAY_OPEN = false;
bool RELAY_CLOSE = false;

const int PIN_DOOR = 1;
const int PIN_WATER = 2;
const int LED_PIN = 5;

bool sleepFlag = false;

Bounce debouncer_door = Bounce();  // Bounce is built into Homie, so you can use it without including it first
Bounce debouncer_water = Bounce(); // Bounce is built into Homie, so you can use it without including it first

int lastDoorValue = -1;
int lastWaterValue = -1;

HomieNode doorNode("door", "door", "endstop");
HomieNode waterNode("water", "water", "endstop");

// HomieNode shuttersNode("shutters", "Shutters", "shutters");
HomieNode shutterOpenNode("shutterOpenNode", "ShutterOpenNode", "switch");
HomieNode shutterCloseNode("shutterCloseNode", "ShutterCloseNode", "switch");
HomieNode abortNode("abortNode", "AbortNode", "switch");

#if defined(ESP32)
int IR_RECEIVE_PIN = 15;
#else
int IR_RECEIVE_PIN = 11;
#endif
IRrecv irrecv(IR_RECEIVE_PIN);
decode_results results;

// Shutters
void shuttersHalt()
{
  RELAY_OPEN = false;
  RELAY_CLOSE = false;
}

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
bool AbortRelayOnHandler(const HomieRange &range, const String &value)
{
  if (value != "true" && value != "false")
    return false;

  if (value == "true")
  {
    shuttersHalt();
    shutterOpenNode.setProperty("on").send(value);
    shutterCloseNode.setProperty("on").send(value);
    Homie.getLogger() << "!!!ABORT!!!" << endl;
  }

  return true;
}

TaskHandle_t Task1;
void Task1code(void *parameter)
{
  for (;;)
  {
    if (irrecv.decode(&results))
    {
      Homie.getLogger() << "recieve IR: " << results.value << endl;

      irrecv.resume(); // Receive the next value
    }
    delay(100);
  }
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
  abortNode.advertise("on").setName("On").setDatatype("boolean").settable(AbortRelayOnHandler);

  Homie.onEvent(onHomieEvent);
  Homie.setup();
}

void loop()
{
  Homie.loop();
}
