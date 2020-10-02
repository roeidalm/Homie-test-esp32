HomieNode doorNode("door", "door", "endstop");
HomieNode waterNode("water", "water", "endstop");

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

void checkDoorsensorState()
{
    int doorValue = debouncer_door.read();

    if (doorValue != lastDoorValue)
    {
        Homie.getLogger() << "Door is now " << (doorValue ? "open" : "closed") << endl;

        doorNode.setProperty("open").send(doorValue ? "true" : "false");
        lastDoorValue = doorValue;
    }
}
void checkWatersensorState()
{
    int waterValue = debouncer_water.read();

    if (waterValue != lastWaterValue)
    {
        Homie.getLogger() << "Water sensor state is " << (waterValue ? "Leak Detected" : "clear") << endl;

        waterNode.setProperty("clear").send(waterValue ? "true" : "false");
        lastWaterValue = waterValue;
    }
}
void checkSensorStatus()
{
    checkDoorsensorState();
    checkWatersensorState();
}

bool getDoorState()
{
    return lastDoorValue;
}

bool getwaterState()
{
    return lastWaterValue;
}

void sensorAdvertiseSetup()
{
    doorNode.advertise("open");
    waterNode.advertise("clear");
}