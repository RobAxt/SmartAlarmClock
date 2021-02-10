#ifndef AlarmInterface_hpp
#define AlarmInterface_hpp

#include <Homie.hpp>
#include <WS2812FX.h>
#include "ESPDateTime.h"
#include "ESP8266TimeAlarms.h"
#include "ArduinoJson.h"
#include "FS.h"

#define LED_COUNT 20
#define LED_PIN D7

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
                               { "rainbow", [this](){ ws2812fx.setMode(FX_MODE_RAINBOW_CYCLE);
                                                      Homie.getLogger() << "  ⏰  Alarm Ticked: Rainbow!!" << endl;} },   
                               { "blink", [this](){ ws2812fx.setMode(FX_MODE_BLINK_RAINBOW);
                                                            Homie.getLogger() << "  ⏰  Alarm Ticked: Blink Rainbow!!" << endl;} },
                               { "random", [this](){ ws2812fx.setMode(FX_MODE_RANDOM_COLOR);
                                                           Homie.getLogger() << "  ⏰  Alarm Ticked: Random Color!!" << endl;} }
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
        WS2812FX ws2812fx;
        void loadJSONSettings(void);
        void saveJSONSettings(void);
        void saveNewSettings(void);
        std::function<void()> findFX(const char* fxName);
};
#endif //AlarmInterface_hpp