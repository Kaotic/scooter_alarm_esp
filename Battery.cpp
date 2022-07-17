#include "Battery.h"

SharedData *Battery::shrd;

SimpleKalmanFilter voltageKalman(1, 1, 0.01);
SimpleKalmanFilter currentKalman(2, 2, 1.0);

Battery::Battery(void){}
Battery::~Battery(void){}

void Battery::setSharedData(SharedData *data) {
	shrd = data;
}

float readVoltage() {
	analogRead(SENSOR_VOLTAGE_PIN); // dummy read to clear the ADC buffer

    float V_IN = analogRead(SENSOR_VOLTAGE_PIN) * VOLTAGE_V_REF_SCALED;
	float V_OUT = V_IN / (VOLTAGE_SENSOR_R2 / (VOLTAGE_SENSOR_R1 + VOLTAGE_SENSOR_R2));

    return V_OUT;
}

float readCurrent() {
	int16_t arc = 0;

	for (int i = 0; i < 10; i++) {
		arc += analogRead(SENSOR_CURRENT_PIN) - CURRENT_ZERO_CALIBRATION;
	}

	return ((float)arc / 10.0 / CURRENT_ADC_SCALE * CURRENT_V_REF / CURRENT_MV_PER_AMP) * 2;
}

int readBatteryPercentage(float voltage) {
	double batteryDischargedVoltage = (BATTERY_DISCHARGED_VOLTAGE * BATTERY_NUMBER_OF_CELLS);
	double batteryChargedVoltage = (BATTERY_CHARGED_VOLTAGE * BATTERY_NUMBER_OF_CELLS);

	float percentage = (voltage - batteryDischargedVoltage) / (batteryChargedVoltage - batteryDischargedVoltage) * 100;

	if(percentage < 0) percentage = 0;
	if(percentage > 100) percentage = 100;

	return percentage;
}

void Battery::loop() {
    shrd->actualVoltage = readVoltage();
    shrd->actualVoltageFiltered = voltageKalman.updateEstimate(shrd->actualVoltage);
	
    shrd->actualCurrent = readCurrent();
    shrd->actualCurrentFiltered = currentKalman.updateEstimate(shrd->actualCurrent);

    shrd->batteryPercentage = readBatteryPercentage(shrd->actualVoltageFiltered);
    shrd->batteryLow = shrd->batteryPercentage < 10;

    // Battery discharge protection
    if(shrd->batteryProtections && shrd->actualVoltageFiltered < shrd->batteryDischargedVoltage) {
        shrd->switchSiren(false);
        shrd->switchPower(false);
        shrd->switchHeadlights(false);
        shrd->switchFlange(false);
    }

    // Battery current too high protection
    if(shrd->batteryProtections && shrd->actualCurrent > shrd->batteryCurrentMax) {
        shrd->switchSiren(false);
        shrd->switchPower(false);
        shrd->switchHeadlights(false);
        shrd->switchFlange(false);
    }

	delay(500);
}

void Battery::init()
{
}