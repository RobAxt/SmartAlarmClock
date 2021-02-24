#ifndef AlarmNode_hpp
#define AlarmNode_hpp

#include "Homie.hpp"
#include "ESPDateTime.h"
#include "ESP8266TimeAlarms.h"
#include "AlarmInterface.hpp"

class AlarmNode : public HomieNode, AlarmInterface {
    public:
        AlarmNode(const char* id, const char* name, const char* type);
        
    protected:
        virtual void setup() override;
        virtual void loop() override;
        virtual void onReadyToOperate() override;
        virtual bool handleInput(const HomieRange& range, const String& property, const String& value);

    private:
        HomieSetting<long> *_brightness;
        HomieSetting<long> *_speed;
        HomieSetting<long> *_color; 
        alarmSettings currentAlarm;  
        void getSetting(int id);     
        void sendProperties(void);
};
#endif //AlarmNode_hpp
