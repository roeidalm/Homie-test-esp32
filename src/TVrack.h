// HomieNode shuttersNode("shutters", "Shutters", "shutters");
HomieNode shutterOpenNode("shutterOpenNode", "ShutterOpenNode", "switch");
HomieNode shutterCloseNode("shutterCloseNode", "ShutterCloseNode", "switch");
HomieNode abortNode("abortNode", "AbortNode", "switch");

bool RELAY_OPEN = false;
bool RELAY_CLOSE = false;

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