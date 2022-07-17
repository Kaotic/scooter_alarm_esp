#include "Alarm.h"

SharedData *Alarm::shrd;


Alarm::Alarm(void){}
Alarm::~Alarm(void){}

void Alarm::setSharedData(SharedData *data) {
	shrd = data;
}

void Alarm::loop() {
    if(shrd->mpuGyroscope.x > shrd->motionSensitivity || shrd->mpuGyroscope.x < -shrd->motionSensitivity || shrd->mpuGyroscope.y > shrd->motionSensitivity || shrd->mpuGyroscope.y < -shrd->motionSensitivity || shrd->mpuGyroscope.z > shrd->motionSensitivity || shrd->mpuGyroscope.z < -shrd->motionSensitivity) {
        shrd->motionDetected = true;
    } else {
        shrd->motionDetected = false;
    }

    switch (shrd->alarmState)
    {
        case K_DISABLED:
            shrd->lastTriggerTime = 0;
            shrd->switchSiren(false);
            shrd->switchPower(true);
            break;
        case K_ENABLED:
            shrd->switchPower(false);
            if(shrd->motionDetected) {
                shrd->alarmState = FIRST_TRIGGER_RUNING;
                shrd->lastTriggerTime = millis();
                shrd->switchSiren(true);
            }
            break;
        case TEST_MODE:
            shrd->switchSiren(true);
            shrd->switchPower(false);
            shrd->switchHeadlights(true);
            shrd->switchFlange(false);
            break;
        case NOTHING_MODE:
            break;
        case FIRST_TRIGGER_RUNING:
            if((millis() - shrd->lastTriggerTime) > 1000) {
                if(!shrd->motionDetected){
                    shrd->switchSiren(false);
                }
                shrd->alarmState = FIRST_TRIGGER;
                shrd->lastTriggerTime = millis();
            }else{
                shrd->switchSiren(false);
                delay(100);
                shrd->switchSiren(true);
            }
            break;
        case FIRST_TRIGGER:
            if(shrd->motionDetected) {
                shrd->alarmState = SECOND_TRIGGER_RUNING;
                shrd->lastTriggerTime = millis();
                shrd->switchSiren(true);
            }else{
                if((millis() - shrd->lastTriggerTime) > 30000) {
                    shrd->alarmState = K_ENABLED;
                    shrd->lastTriggerTime = 0;
                    shrd->switchSiren(false);
                }
            }
            break;
        case SECOND_TRIGGER_RUNING:
            if((millis() - shrd->lastTriggerTime) > 5000) {
                if(!shrd->motionDetected){
                    shrd->switchSiren(false);
                }
                shrd->alarmState = SECOND_TRIGGER;
                shrd->lastTriggerTime = millis();
            }
            break;
        case SECOND_TRIGGER:
            if(shrd->motionDetected) {
                shrd->alarmState = ALARM_RUNING;
                shrd->lastTriggerTime = millis();
                shrd->switchSiren(true);
            }else{
                if((millis() - shrd->lastTriggerTime) > 30000) {
                    shrd->alarmState = K_ENABLED;
                    shrd->lastTriggerTime = 0;
                    shrd->switchSiren(false);
                }
            }
            break;
        case ALARM_RUNING:
            if(shrd->motionDetected){
                shrd->lastTriggerTime = millis();
            }else{
                if((millis() - shrd->lastTriggerTime) > 60000) {
                    shrd->alarmState = K_ENABLED;
                    shrd->lastTriggerTime = 0;
                    shrd->switchSiren(false);
                }
            }
            break;
        default:
            break;
    }

    #if SERIAL_DEBUG && SERIAL_DEBUG_MPU
        Serial.print("Accelerometer ");
        Serial.print("X: ");
        Serial.print(shrd->mpuAcceleration.x, 1);
        Serial.print(" m/s^2, ");
        Serial.print("Y: ");
        Serial.print(shrd->mpuAcceleration.y, 1);
        Serial.print(" m/s^2, ");
        Serial.print("Z: ");
        Serial.print(shrd->mpuAcceleration.z, 1);
        Serial.println(" m/s^2");

        Serial.print("Gyroscope ");
        Serial.print("X: ");
        Serial.print(shrd->mpuGyroscope.x, 1);
        Serial.print(" rps, ");
        Serial.print("Y: ");
        Serial.print(shrd->mpuGyroscope.y, 1);
        Serial.print(" rps, ");
        Serial.print("Z: ");
        Serial.print(shrd->mpuGyroscope.z, 1);
        Serial.println(" rps");
    #endif

    vTaskDelay(100);
}

void Alarm::init()
{
    
}