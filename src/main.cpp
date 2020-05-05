#include <Arduino.h>
#include "Homie.h"
#include "ESPDateTime.h"
#include "ESP8266TimeAlarms.h"

#define FW_NAME "smart-alarm"
#define FW_VERSION "0.0.0"

/* Magic sequence for Autodetectable Binary Upload */
const char *__FLAGGED_FW_NAME = "\xbf\x84\xe4\x13\x54" FW_NAME "\x93\x44\x6b\xa7\x75";
const char *__FLAGGED_FW_VERSION = "\x6a\x3f\x3e\x0e\xe1" FW_VERSION "\xb0\x30\x48\xd4\x1a";
/* End of magic sequence for Autodetectable Binary Upload */

unsigned long ms = millis();
const unsigned long loopInterval = 30 * 1000;
void setup() {
#if DEBUG_APP
  Serial.begin(SERIAL_SPEED);
  Serial << endl << endl;
  Serial << F("Build Date and Time: ") << __DATE__ << " & " << __TIME__ << endl;
#else
  Homie.disableLogging();
#endif

  Homie_setFirmware(FW_NAME, FW_VERSION); // The underscore is not a typo! See Magic bytes

  Homie.setSetupFunction([](){
                                 DateTime.setTimeZone(-3);
                                 DateTime.setServer("south-america.pool.ntp.org");
                                 DateTime.begin();
                                 if (!DateTime.isTimeValid()) 
                                     Serial.println("  ✖ Failed to get time from server.");
                              });
  Homie.setLoopFunction([](){
                                 if (millis() - ms > loopInterval) {
                                     ms = millis();
                                     if (!DateTime.isTimeValid()) {
                                         Serial.println("  ✖ Failed to get time from server, retry.");
                                         DateTime.begin();
                                     }
                                     Homie.getLogger() << "  ◦ Date: " << DateTime.format(DateFormatter::DATE_ONLY) << endl;
                                     Homie.getLogger() << "  ◦ Time: " << DateTime.format(DateFormatter::TIME_ONLY) << endl;
                                 }
                                 Alarm.delay(0);
                             });
  Homie.setup();
}

void loop() {
  Homie.loop();
}

/**
 * More examples and docs see :
 * https://github.com/mcxiaoke/ESPDateTime

  Serial.println(DateTime.now());
  Serial.println(DateTime.getTime());
  Serial.println(DateTime.utcTime());
  Serial.println("--------------------");
  Serial.println(DateTime.toString());
  Serial.println(DateTime.toISOString());
  Serial.println(DateTime.toUTCString());
  Serial.println("--------------------");
  Serial.println(DateTime.format(DateFormatter::COMPAT));
  Serial.println(DateTime.format(DateFormatter::DATE_ONLY));
  Serial.println(DateTime.format(DateFormatter::TIME_ONLY));
  Serial.println("--------------------");
  DateTimeParts p = DateTime.getParts();
  Serial.printf("%04d/%02d/%02d %02d:%02d:%02d %ld %+05d\n", p.getYear(),
                p.getMonth(), p.getMonthDay(), p.getHours(), p.getMinutes(),
                p.getSeconds(), p.getTime(), p.getTimeZone());
  Serial.println("--------------------");
  time_t t = DateTime.now();
  Serial.println(DateFormatter::format("%Y/%m/%d %H:%M:%S", t));
  Serial.println(DateFormatter::format("%x - %I:%M %p", t));
  Serial.println(DateFormatter::format("Now it's %F %I:%M%p.", t));

  p.from(1588624117,-3);
  Serial.println("Another date parts:");
  Serial.printf("%04d/%02d/%02d %02d:%02d:%02d %ld %+05d\n", p.getYear(),
                p.getMonth(), p.getMonthDay(), p.getHours(), p.getMinutes(),
                p.getSeconds(), p.getTime(), p.getTimeZone());

      Serial.printf("Up     Time:   %lu seconds\n", millis() / 1000);
      Serial.printf("Local  Time:   %ld\n", DateTime.now());
      Serial.printf("Local  Time:   %s\n", DateTime.toString().c_str());
      Serial.printf("UTC    Time:   %ld\n", DateTime.utcTime());
      Serial.printf("UTC    Time:   %s\n", DateTime.formatUTC(DateFormatter::SIMPLE).c_str());
*/