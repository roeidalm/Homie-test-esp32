
const int PIN_DOOR = 1;
const int PIN_WATER = 2;
const int LED_PIN = 5;

Bounce debouncer_door = Bounce();  // Bounce is built into Homie, so you can use it without including it first
Bounce debouncer_water = Bounce(); // Bounce is built into Homie, so you can use it without including it first

int lastDoorValue = -1;
int lastWaterValue = -1;

void sensorSetup()
{
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
}