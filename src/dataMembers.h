
bool RELAY_OPEN = false;
bool RELAY_CLOSE = false;

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
