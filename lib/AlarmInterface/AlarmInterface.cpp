#include "AlarmInterface.hpp"

AlarmInterface::AlarmInterface() :  ws2812fx(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800){
    memset(_settings, 0, sizeof(_settings));
    DateTime.setTimeZone(-3);
    DateTime.setServer("south-america.pool.ntp.org");
}

void
AlarmInterface::setupInterface(int brightness, int speed, unsigned long color) { 
    
    DateTime.begin();
    Homie.getLogger() << "  ✔ TimeSync from " << DateTime.getServer() << endl;
    Homie.getLogger() << "  ✔ Date: " << DateTime.format(DateFormatter::DATE_ONLY) << endl;
    Homie.getLogger() << "  ✔ Time: " << DateTime.format(DateFormatter::TIME_ONLY) << endl;
    
    ws2812fx.init();
    ws2812fx.setBrightness(brightness);
    ws2812fx.setSpeed(speed);
    ws2812fx.setColor(color);
    ws2812fx.start(); 
}

void 
AlarmInterface::loopInterface(void) {
    if (!DateTime.isTimeValid()) {
        Serial.println("  ✖ Failed to get time from server, retry.");
        DateTime.begin();
        Homie.getLogger() << "  ✔ Date: " << DateTime.format(DateFormatter::DATE_ONLY) << endl;
        Homie.getLogger() << "  ✔ Time: " << DateTime.format(DateFormatter::TIME_ONLY) << endl;

    if(openFileSettings("r")) {
        Homie.getLogger() << F("  ✔ Loading JSONFILE...") << endl;
        loadJSONSettings();
    
        for(int i = 0; i < dtNBR_ALARMS ;i++) {    
            Homie.getLogger() << "  ◦ id " << _settings[i].id 
                              << " enable " << (_settings[i].enable? "true" : "false") 
                              << " hour " << _settings[i].hour  
                              << " minute " << _settings[i].minute 
                              << " day of week " << _settings[i].dayOfWeek 
                              << " fxname " <<  _settings[i].fxName << endl;
            addAlarm(_settings[i]);
        }
        jsonFile.close();
        SPIFFS.end();
    }

    }
    Alarm.delay(0);
    ws2812fx.service();
}

int 
AlarmInterface::addAlarm(AlarmInterface::alarmSettings &currentAlarm) {
    int tmpId = 258;
    if(currentAlarm.enable == true || (currentAlarm.enable == false && strcmp(currentAlarm.dayOfWeek,"none") != 0 && strcmp(currentAlarm.fxName,"none") != 0) ) 
        if(strcmp(currentAlarm.dayOfWeek,"all") == 0) 
            tmpId = Alarm.alarmRepeat(currentAlarm.hour, currentAlarm.minute, 0, findFX(currentAlarm.fxName));
        else if(strcmp(currentAlarm.dayOfWeek,"sunday") == 0) 
                 tmpId = Alarm.alarmRepeat(dowSunday, currentAlarm.hour, currentAlarm.minute, 0, findFX(currentAlarm.fxName));
             else if(strcmp(currentAlarm.dayOfWeek,"monday") == 0) 
                      tmpId = Alarm.alarmRepeat(dowMonday, currentAlarm.hour, currentAlarm.minute, 0, findFX(currentAlarm.fxName));
                  else if(strcmp(currentAlarm.dayOfWeek,"tuesday") == 0) 
                           tmpId = Alarm.alarmRepeat(dowTuesday, currentAlarm.hour, currentAlarm.minute, 0, findFX(currentAlarm.fxName));
                       else if(strcmp(currentAlarm.dayOfWeek,"wednesday") == 0)     
                                tmpId = Alarm.alarmRepeat(dowWednesday, currentAlarm.hour, currentAlarm.minute, 0, findFX(currentAlarm.fxName));
                            else if(strcmp(currentAlarm.dayOfWeek,"thursday") == 0) 
                                     tmpId = Alarm.alarmRepeat(dowThursday, currentAlarm.hour, currentAlarm.minute, 0, findFX(currentAlarm.fxName));
                                 else if(strcmp(currentAlarm.dayOfWeek,"friday") == 0) 
                                          tmpId = Alarm.alarmRepeat(dowFriday, currentAlarm.hour, currentAlarm.minute, 0, findFX(currentAlarm.fxName));
                                      else if(strcmp(currentAlarm.dayOfWeek,"saturday") == 0) 
                                               tmpId = Alarm.alarmRepeat(dowSaturday, currentAlarm.hour, currentAlarm.minute, 0, findFX(currentAlarm.fxName));
                                           else if(strcmp(currentAlarm.dayOfWeek,"none") == 0) 
                                                   tmpId = 256;
                                                else 
                                                    tmpId =257;

    switch(tmpId) {
        case 255:
        case 256: 
        case 257: {
            Homie.getLogger() << "  ✖ Fail to add alarm,(id:" << currentAlarm.id <<" code:" << tmpId << ")" << endl;
            _settings[currentAlarm.id].id = currentAlarm.id;
            _settings[currentAlarm.id].enable = false;
            _settings[currentAlarm.id].hour = 0;
            _settings[currentAlarm.id].minute = 0;
            strlcpy(_settings[currentAlarm.id].dayOfWeek, "none", 5);
            strlcpy(_settings[currentAlarm.id].fxName, "none", 5);
            break;
        }
        case 258: {
            Homie.getLogger() << "  ✖ Current Alarm not used (id:" << currentAlarm.id << ")" << endl;
            break;
        }
        default: {
            _settings[tmpId].id = tmpId;
            _settings[tmpId].enable = currentAlarm.enable;
            _settings[tmpId].hour = currentAlarm.hour;
            _settings[tmpId].minute = currentAlarm.minute;
            strlcpy(_settings[tmpId].dayOfWeek, currentAlarm.dayOfWeek, strlen(currentAlarm.dayOfWeek)+1);
            strlcpy(_settings[tmpId].fxName, currentAlarm.fxName, strlen(currentAlarm.fxName)+1);

            if(currentAlarm.enable == false) 
                Alarm.disable(tmpId);

            Homie.getLogger() << "  ✔ Alarm added (id: " << _settings[tmpId].id  << ")" <<  endl;
        }
    }

    return tmpId;
}
int
AlarmInterface::createAlarm(AlarmInterface::alarmSettings &currentAlarm) {
    int tmpId = 256;
    tmpId = addAlarm(currentAlarm);
    Homie.getLogger() << "Creating Alarm Id: " << tmpId << endl;
    saveNewSettings();
    return tmpId;
}

int 
AlarmInterface::deleteAlarm(int id) {
     Alarm.free(id);
    
    _settings[id].enable = false;
    _settings[id].hour = 0;
    _settings[id].minute = 0;
    strlcpy(_settings[id].dayOfWeek, "none", 5);
    strlcpy(_settings[id].fxName, "none", 5);

    saveNewSettings();

    Homie.getLogger() << "  ✔ Alarm deleted (id: " << id << ")" <<  endl;
    return id;
}

void 
AlarmInterface::enableAlarm(int id) {
    Alarm.enable(id);
    _settings[id].enable = true;
    saveNewSettings();
}

void
AlarmInterface::disableAlarm(int id) {
    Alarm.disable(id);
    _settings[id].enable = false;
    saveNewSettings();
}

std::function<void()>
AlarmInterface::findFX(const char* fxName) {
    for(int i= 0; i < 5; i++) 
        if(_fxs[i].fxName == fxName) 
            return _fxs[i].fx;
    return _fxs[0].fx;
}

void
AlarmInterface::setColor(unsigned long color) {
    if(ws2812fx.getMode() != FX_MODE_STATIC)
        ws2812fx.setMode(FX_MODE_STATIC);
    if(color == 0)
        ws2812fx.strip_off();
    ws2812fx.setColor(color);
}

void 
AlarmInterface::alarmFxMode(int mode) {
     ws2812fx.setMode(mode);
     Homie.getLogger() << "  ⏰  Alarm Ticked: " << ws2812fx.getModeName(mode) << "!!" << endl;
}

void 
AlarmInterface::saveNewSettings(void) {
    if(openFileSettings("w")) {
        Homie.getLogger() << F("  ✔ Loading JSONFILE...") << endl;
        saveJSONSettings();
    }
    jsonFile.close();
    SPIFFS.end();
}

bool
AlarmInterface::openFileSettings(const char* mode) {
    if(SPIFFS.begin()) {
        if(SPIFFS.exists(JSONFILE)) {
            jsonFile = SPIFFS.open(JSONFILE, mode);
            if (!jsonFile) {
                Homie.getLogger() << F("  ✖ Failed to open JSONFILE for Reading")  << endl; 
            } else {
               Homie.getLogger() << F("  ✔ JSONFILE opened... Size: ") << jsonFile.size() << " bytes" << endl; 
               return true;
            }
        } else {
            Homie.getLogger() << F("  ✖ File doesn't exist")  << endl; 
            return false;
        }
    } else {
        Homie.getLogger() << F("  ✖ Failed to mount file system")  << endl; 
        return false;
    }
    return false;
}

void 
AlarmInterface::loadJSONSettings(void) {
    StaticJsonDocument <6144> json;
    DeserializationError error = deserializeJson(json, jsonFile);
    if (!error) {
        for(int i=0; i < dtNBR_ALARMS ;i++) {
            _settings[i].id = json["Alarms"][i]["id"].as<int>();
            _settings[i].enable =  strcmp(json["Alarms"][i]["enable"],"true") == 0 ? true : false;
            _settings[i].hour = json["Alarms"][i]["hour"];
            _settings[i].minute = json["Alarms"][i]["minute"];
            strlcpy(_settings[i].dayOfWeek, json["Alarms"][i]["dayOfWeek"], strlen( json["Alarms"][i]["dayOfWeek"])+1);
            strlcpy(_settings[i].fxName, json["Alarms"][i]["fxName"], strlen(json["Alarms"][i]["fxName"])+1);       
        }
        serializeJson(json, Serial); Homie.getLogger() << endl;
    } else 
        Homie.getLogger() << F("  ✖ Failed to deserialize Json: ") << error.c_str() << endl;
}

void 
AlarmInterface::saveJSONSettings(void) {
    StaticJsonDocument <6144> json; 
    JsonObject root = json.to<JsonObject>();
    JsonArray alarms = root.createNestedArray("Alarms");
    for (uint8_t i = 0; i < dtNBR_ALARMS; i++) {       
        JsonObject alarm = alarms.createNestedObject();
        alarm["id"] = _settings[i].id;
        alarm["enable"] = _settings[i].enable ? "true" : "false";
        alarm["hour"] = _settings[i].hour;
        alarm["minute"] = _settings[i].minute;
        alarm["dayOfWeek"] = _settings[i].dayOfWeek;
        alarm["fxName"] = _settings[i].fxName;
      }	
    serializeJson(json, jsonFile);
    serializeJson(json, Serial);   Homie.getLogger() << endl;
}
