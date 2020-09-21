#include <Homie.h>

#define firmwareVersion "1.0.0"
const int PIN_RELAY = 2;

HomieNode lightNode("light", "Light", "switch");

bool lightOnHandler(const HomieRange& range, const String& value) {
  if (value != "true" && value != "false") return false;

  bool on = (value == "true");
  digitalWrite(PIN_RELAY, on ? HIGH : LOW);
  lightNode.setProperty("dmid").send(value);
  // lightNode.setProperty("on").send(value);
  Homie.getLogger() << "Light is " << (on ? "on" : "off") << endl;

  return true;
}

void setup() {
  Serial.begin(115200);
  Serial << endl << endl;
  pinMode(PIN_RELAY, OUTPUT);
  digitalWrite(PIN_RELAY, HIGH);

  Homie_setFirmware("awesome-relay", firmwareVersion);
// setName-you chiuse the name of the controller like frindly name
  lightNode.advertise("dmid").setName("board led").setDatatype("boolean").settable(lightOnHandler);
  // lightNode.advertise("on").setName("On").setDatatype("boolean").settable(lightOnHandler);

  Homie.setup();
}

void loop() {
  Homie.loop();
}
