#ifndef AlarmInterface_hpp
#define AlarmInterface_hpp

#include <Homie.hpp>
#include "ESPDateTime.h"
#include "ESP8266TimeAlarms.h"
#include "ArduinoJson.h"
#include "FS.h"


class AlarmInterface
{
    protected:
        typedef struct {
            int id;
            bool enable;
            int hour;
            int minute;
            char dayOfWeek[10];
            char fxName[10];
        } alarmSettings; 
    
        alarmSettings _settings[dtNBR_ALARMS];
        
        typedef struct {
                 String fxName;
                 std::function<void()> fx;
        } fxFunction;

        fxFunction _fxs[5] = { { "none",    [this](){ Homie.getLogger() << "  ◦ Do Nothing Alarm!!" << endl;} },
                               { "system",  [this](){ Alarm.alarmRepeat(dowSaturday,6,0,0,[](){
                                                           Homie.getLogger() << "Adquiring new Time" << Serial.println(DateTime.toString()) << " from " << DateTime.getServer() << endl;
                                                           DateTime.begin();} );} },   
                               { "sunrise", [this](){ Homie.getLogger() << "  ◦ Alarm Ticked: Hello SunShine!!" << endl;} },   
                               { "option2", [this](){ Homie.getLogger() << "  ◦ Alarm Ticked: Hello Option2!!" << endl;} },
                               { "option3", [this](){ Homie.getLogger() << "  ◦ Alarm Ticked: Hello Option3!!" << endl;} }
                             };
        int addAlarm(alarmSettings &currentAlarm);

    public:
        AlarmInterface();
        void setupInterface(void);
        void loopInterface(void);
        int createAlarm(alarmSettings &currentAlarm);
        
        int deleteAlarm(int id);
        void enableAlarm(int id);
        void disableAlarm(int id);

    private:
        const char* JSONFILE = "/homie/state.json";
        bool openFileSettings(const char* mode);
        File jsonFile;
        void loadJSONSettings(void);
        void saveJSONSettings(void);
        void saveNewSettings(void);
        std::function<void()> findFX(const char* fxName);
};
#endif //AlarmInterface_hpp