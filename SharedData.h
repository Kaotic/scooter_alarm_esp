#ifndef _SHARED_DATA_H_
#define _SHARED_DATA_H_

#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include "defines.h"


typedef enum
{
	K_DISABLED = 0,
	K_ENABLED = 1,
	TEST_MODE = 5,
    NOTHING_MODE = 6,
	FIRST_TRIGGER_RUNING = 10,
	FIRST_TRIGGER = 15,
	SECOND_TRIGGER_RUNING = 20,
	SECOND_TRIGGER = 25,
	ALARM_RUNING = 30,
} AlarmStates;

typedef enum
{
    S_DISABLE_ALARM = 0,
    S_ENABLE_ALARM = 1,
    S_TEST_MODE = 5,
    S_NOTHING_MODE = 6,
    S_DISABLE_SIREN = 10,
    S_ENABLE_SIREN = 11,
    S_DISABLE_POWER = 20,
    S_ENABLE_POWER = 21,
    S_DISABLE_HEADLIGHTS = 30,
    S_ENABLE_HEADLIGHTS = 31,
    S_DISABLE_FLANGE = 40,
    S_ENABLE_FLANGE = 41,
    S_DISABLE_BATTERY_PROTECTIONS = 60,
    S_ENABLE_BATTERY_PROTECTIONS = 61,

} BLECommandStates;


class SharedData
{
    public:
        SharedData(){};

        // Battery
        bool batteryProtections = BATTERY_PROTECTIONS;
        bool batteryLow = true;
        int batteryPercentage = 0;
        float batteryDischargedVoltage = (BATTERY_DISCHARGED_VOLTAGE * BATTERY_NUMBER_OF_CELLS);
        float batteryChargedVoltage = (BATTERY_CHARGED_VOLTAGE * BATTERY_NUMBER_OF_CELLS);
        float batteryCurrentMax = (BATTERY_CURRENT_MAX * BATTERY_NUMBER_OF_CELLS);

        float actualVoltage = 0;
        float actualVoltageFiltered = 0;
        
        float actualCurrent = 0;
        float actualCurrentFiltered = 0;

        AlarmStates alarmState = K_ENABLED;

        bool isLocked = false;
        bool scooterJacking = false;

        double motionSensitivity = 0.5;
        bool motionDetected = false;

        long lastTriggerTime = 0;

        // Alarm
        sensors_vec_t mpuAcceleration;
        sensors_vec_t mpuGyroscope;
        float mpuTemperature;

        // --- Shared functions ---
        bool isSirenEnabled = false;
        void switchSiren(bool state) {
            if (isSirenEnabled && state == false){
                digitalWrite(RELAY_SIREN_PIN, LOW);
                isSirenEnabled = state;
            }else if (!isSirenEnabled && state == true) {
                digitalWrite(RELAY_SIREN_PIN, HIGH);
                isSirenEnabled = state;
            }
        }

        bool isPowerEnabled = false;
        void switchPower(bool state) {
            if (isPowerEnabled && state == false){
                digitalWrite(RELAY_TROTT_POWER_PIN, LOW);
                isPowerEnabled = state;
            }else if (!isPowerEnabled && state == true) {
                digitalWrite(RELAY_TROTT_POWER_PIN, HIGH);
                isPowerEnabled = state;
            }
        }

        bool isHeadlightsEnabled = false;
        void switchHeadlights(bool state) {
            if (isHeadlightsEnabled && state == false){
                digitalWrite(RELAY_HEADLIGHTS_PIN, LOW);
                isHeadlightsEnabled = state;
            }else if (!isHeadlightsEnabled && state == true) {
                digitalWrite(RELAY_HEADLIGHTS_PIN, HIGH);
                isHeadlightsEnabled = state;
            }
        }

        bool isFlangeEnabled = false;
        void switchFlange(bool state) {
            if (isFlangeEnabled && state == false){
                digitalWrite(RELAY_FLANGE_PIN, LOW);
                isFlangeEnabled = state;
            }else if (!isFlangeEnabled && state == true) {
                digitalWrite(RELAY_FLANGE_PIN, HIGH);
                isFlangeEnabled = state;
            }
        }

        void toggleAlarm(bool state) {
            if(state == true){
                for(int i = 0; i < 2; i++) {
                    switchSiren(true);
                    delay(30);
                    switchSiren(false);
                    delay(30);
                }
                alarmState = K_ENABLED;
            }else{
                switchSiren(true);
                delay(30);
                switchSiren(false);
                alarmState = K_DISABLED;
            }
        }
};

#endif // _SHARED_DATA_H_