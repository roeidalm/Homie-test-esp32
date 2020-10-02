#include <EEPROM.h>

// HomieNode shuttersNode("shutters", "Shutters", "shutters");
HomieNode tvRackOpenNode("tvRackOpenNode", "TVRackOpenNode", "switch");
HomieNode tvRackCloseNode("tvRackCloseNode", "TVRackCloseNode", "switch");
HomieNode abortNode("abortNode", "AbortNode", "switch");

bool RELAY_OPEN = false;
bool RELAY_CLOSE = false;

const byte OPEN_IR_COMMANE_EEPROM = 0;
const byte CLOSE_IR_COMMANE_EEPROM = 8;

uint64_t tvRackOpenGetState()
{
    return EEPROM.readLong64(OPEN_IR_COMMANE_EEPROM);
}

uint64_t tvRackCloseGetState()
{
    return EEPROM.readLong64(CLOSE_IR_COMMANE_EEPROM);
}

void tvRackSetState(int addr, uint64_t state)
{
    EEPROM.write(addr, state);
    EEPROM.commit();
}

void tvRackHalt()
{
    RELAY_OPEN = false;
    RELAY_CLOSE = false;
}

void reset()
{
    tvRackHalt();
    //TODO
}

bool RelayOpenOnHandler(const HomieRange &range, const String &value)
{
    if (value != "true" && value != "false")
        return false;

    RELAY_OPEN = (value == "true");
    tvRackOpenNode.setProperty("on").send(value);
    Homie.getLogger() << "RELAY_OPEN " << (RELAY_OPEN ? "on" : "off") << endl;

    return true;
}
bool RelayCloseOnHandler(const HomieRange &range, const String &value)
{
    if (value != "true" && value != "false")
        return false;

    RELAY_CLOSE = (value == "true");
    tvRackCloseNode.setProperty("on").send(value);
    Homie.getLogger() << "RELAY_CLOSE " << (RELAY_CLOSE ? "on" : "off") << endl;

    return true;
}
bool AbortRelayOnHandler(const HomieRange &range, const String &value)
{
    if (value != "true" && value != "false")
        return false;

    if (value == "true")
    {
        tvRackHalt();
        tvRackOpenNode.setProperty("on").send(value);
        tvRackCloseNode.setProperty("on").send(value);
        Homie.getLogger() << "!!!ABORT!!!" << endl;
    }

    return true;
}

bool tvRackOpenSetStateOnHandler(const HomieRange &range, const String &value)
{
    long longValue = atol(value.c_str());
    tvRackSetState(OPEN_IR_COMMANE_EEPROM, longValue);
    Homie.getLogger() << "tvRack Open cmd is now: " << longValue << endl;
    return true;
}
bool tvRackCloseSetStateOnHandler(const HomieRange &range, const String &value)
{
    long longValue = atol(value.c_str());
    tvRackSetState(CLOSE_IR_COMMANE_EEPROM, longValue);
    Homie.getLogger() << "tvRack Close cmd is now: " << longValue << endl;
    return true;
}

void TVRackAdvertiseSetup()
{
    tvRackOpenNode.advertise("openCmd").setName("Open Cmd").setDatatype("boolean").settable(RelayOpenOnHandler);
    tvRackOpenNode.advertise("openEEPROMCmd").setName("Open EEPROM Cmd").setDatatype("float").settable(tvRackOpenSetStateOnHandler);
    tvRackCloseNode.advertise("closeCmd").setName("close Cmd").setDatatype("boolean").settable(RelayCloseOnHandler);
    tvRackOpenNode.advertise("closeEEPROMCmd").setName("close EEPROM Cmd").setDatatype("float").settable(tvRackCloseSetStateOnHandler);
    abortNode.advertise("abortCmd").setName("Abort Cmd").setDatatype("boolean").settable(AbortRelayOnHandler);
}