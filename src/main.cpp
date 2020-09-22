#include <Homie.h>

#define firmwareVersion "1.0.0"
const int PIN_RELAY = 2;

HomieNode lightNode("light", "Light", "switch");

bool lightOnHandler(const HomieRange &range, const String &value)
{
  if (value != "true" && value != "false")
    return false;

  bool on = (value == "true");
  digitalWrite(PIN_RELAY, on ? HIGH : LOW);
  lightNode.setProperty("dmid").send(value);
  // lightNode.setProperty("on").send(value);

  return true;
}

bool lightblink(const HomieRange &range, const String &value)
{
  if (value != "true" && value != "false")
    return false;

  bool on = (value == "true");
  for (int i = 0; i < 5; i++)
  {
    digitalWrite(PIN_RELAY, HIGH);
    delay(250);
    digitalWrite(PIN_RELAY, LOW);
    delay(250);
  }
  lightNode.setProperty("blincker").send(value);

  // Homie.getLogger() << "im blinkinggg" << endl;

  return true;
}
void setup()
{
  Serial.begin(115200);
  Serial << endl
         << endl;

  Homie.disableResetTrigger();
  pinMode(PIN_RELAY, OUTPUT);
  digitalWrite(PIN_RELAY, HIGH);

  Homie_setFirmware("awesome-relay", firmwareVersion);
  // setName-you chiuse the name of the controller like frindly name
  //the advertise is the topic inside the node
  lightNode.advertise("on").setName("board led").setDatatype("boolean").settable(lightOnHandler);
  //mqtt command
  //devices/kitchen-light/light/on/set  -> true/false
  // lightNode.advertise("on").setName("On").setDatatype("boolean").settable(lightOnHandler);
  lightNode.advertise("blinker").setName("board led blinker").setDatatype("boolean").settable(lightblink);

  Homie.setup();
}

void loop()
{
  Homie.loop();
}
