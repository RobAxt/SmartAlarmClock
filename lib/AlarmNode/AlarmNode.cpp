#include "AlarmNode.hpp"

AlarmNode::AlarmNode(const char* id, const char* name, const char* type) : HomieNode(id,name,type) {
     
}

void 
AlarmNode::setup() {
    Homie.getLogger() << F("Calling Node Setup...") << endl;
    setRunLoopDisconnected(false);
    advertise("action").setName("Action")
                   .setDatatype("string")
                   .setFormat("none,add,delete")
                   .settable([this](const HomieRange& range, const String& value) {
                               if(value == "none") return true;
                               if(value == "add") getSetting(createAlarm(currentAlarm));
                               if(value == "delete") getSetting(deleteAlarm(currentAlarm.id));
                               sendProperties();
                               return true;
                             });
    advertise("id").setName("Id")
                   .setDatatype("integer")
                   .setFormat("1-19")
                   .settable([this](const HomieRange& range, const String& value) {
                              getSetting(value.toInt());
                              sendProperties();
                              return true;
                             });
    advertise("enable").setName("Enable")
                   .setDatatype("bool")
                   .setFormat("true:false")
                   .settable([this](const HomieRange& range, const String& value) {
                              value == "true"? enableAlarm(currentAlarm.id) : disableAlarm(currentAlarm.id);
                              getSetting(currentAlarm.id);
                              sendProperties();
                              return true;
                             });
    advertise("hour").setName("Hour")
                   .setDatatype("integer")
                   .setFormat("0-23")
                   .settable([this](const HomieRange& range, const String& value) {
                              currentAlarm.hour = value.toInt();
                              sendProperties();
                              return true;

                             });
    advertise("minute").setName("Minute")
                   .setDatatype("integer")
                   .setFormat("0-59")
                   .settable([this](const HomieRange& range, const String& value) {
                              currentAlarm.minute = value.toInt();
                              sendProperties();
                              return true;
                             });
    advertise("dayOfWeek").setName("Day of Week")
                   .setDatatype("string")
                   .setFormat("all,sunday,monday,tuesday,wednesday,thursday,friday,saturday,none")
                   .settable([this](const HomieRange& range, const String& value) {
                              strlcpy(currentAlarm.dayOfWeek, value.c_str(), strlen(value.c_str())+1);
                              sendProperties();
                              return true;
                             });
    advertise("fx").setName("fx")
                   .setDatatype("string")
                   .setFormat("none,sunrise")
                   .settable([this](const HomieRange& range, const String& value) {
                              strlcpy(currentAlarm.fxName, value.c_str(), strlen(value.c_str())+1);
                              sendProperties();
                              return true;
                             });
}

void 
AlarmNode::loop() {
    loopInterface();
}

void 
AlarmNode::onReadyToOperate() {
    Homie.getLogger() << F("Calling Ready To Operate... ") << endl;
    Homie.getLogger() << F("  ◦ Node Name: ") << getName() << endl;
   
    setupInterface();

    getSetting(1);
    sendProperties();
 }

bool 
AlarmNode::handleInput(const HomieRange& range, const String& property, const String& value) {
    Homie.getLogger() << F("Calling Node Handle Input...") << endl;
    if(property != "action" && property != "id" && property != "enable" && property != "hour" && 
       property != "minute" && property != "dayOfWeek" && property != "fx") {
        Homie.getLogger() << F("  ✖ Error: property not handle: ") << property << endl; 
        return true;
    }
    if(property == "action" && value != "add" && value != "change" && value != "delete") {
        Homie.getLogger() << F("  ✖ Error: wrong value for command property: ") << value << endl; 
        return true;
    }
    if(property == "id" && (value.toInt() < 1 || value.toInt() > dtNBR_ALARMS)) {
        Homie.getLogger() << F("  ✖ Error: wrong value for id property: ") << value << endl; 
        return true;
    }
    if(property == "enable" && value != "true" && value != "false") {
        Homie.getLogger() << F("  ✖ Error: wrong value for command enable: ") << value << endl; 
        return true;
    }
    if(property == "hour" && (value.toInt() < 0 || value.toInt() > 23)) {
        Homie.getLogger() << F("  ✖ Error: wrong value for hour property: ") << value << endl; 
        return true;
    }
    if(property == "minute" && (value.toInt() < 0 || value.toInt() > 59)) {
        Homie.getLogger() << F("  ✖ Error: wrong value for minute property: ") << value << endl; 
        return true;
    }
    if(property == "dayOfWeek" && value != "all" && value != "sunday" && value != "monday" &&
       value != "tuesday" && value != "wednesday" && value != "thursday" && value != "friday" && value != "saturday") {
        Homie.getLogger() << F("  ✖ Error: wrong value for dayOfWeek property: ") << value << endl; 
        return true;
    }
    Homie.getLogger() << F("  ✔ Receive Property/Value: ") << property  << F(" ━► ") << value << endl;
    return false;
}

void
AlarmNode::getSetting(int id) {
    if(id < dtNBR_ALARMS) {
         currentAlarm.id = _settings[id].id;
         currentAlarm.enable = _settings[id].enable;
         currentAlarm.hour = _settings[id].hour;
         currentAlarm.minute = _settings[id].minute;
         strlcpy(currentAlarm.dayOfWeek, _settings[id].dayOfWeek, strlen(_settings[id].dayOfWeek)+1);
         strlcpy(currentAlarm.fxName, _settings[id].fxName, strlen(_settings[id].fxName)+1);
    }
}

void
AlarmNode::sendProperties() {
    setProperty("action").overwriteSetter(false).send("none");
    setProperty("id").overwriteSetter(false).send(String(currentAlarm.id).c_str());
    setProperty("enable").overwriteSetter(false).send(currentAlarm.enable?"true":"false");
    setProperty("hour").overwriteSetter(false).send(String(currentAlarm.hour).c_str());
    setProperty("minute").overwriteSetter(false).send(String(currentAlarm.minute).c_str());
    setProperty("dayOfWeek").overwriteSetter(false).send(currentAlarm.dayOfWeek);
    setProperty("fx").overwriteSetter(false).send(currentAlarm.fxName);   
}