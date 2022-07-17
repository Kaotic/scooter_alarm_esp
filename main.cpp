#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>

#include "SharedData.h"
#include "I2CManager.h"
#include "Battery.h"
#include "BLE.h"
#include "Alarm.h"
#include "Buttons.h"
#include "defines.h"

TaskHandle_t xI2CManagerTaskHandle;
TaskHandle_t xBatteryTaskHandle;
TaskHandle_t xAlarmTaskHandle;
TaskHandle_t xButtonsTaskHandle;

SharedData sharedData;
I2CManager k_I2CManager;
Battery k_battery;
BLE k_ble;
Alarm k_alarm;
Buttons k_buttons;


// --- THREADS FUNCTIONS ---

void hI2CManagerTask(void *pvParameters) {
	Serial.println("Thread : I2CManager task started");

	for(;;) {
		k_I2CManager.loop();
	}
}

void hBatteryTask(void *pvParameters) {
	Serial.println("Thread : Battery task started");

    for(;;) {
		k_battery.loop();
	}
}

void hAlarmTask(void *pvParameters) {
	Serial.println("Thread : Alarm task started");

    for(;;) {
		k_alarm.loop();
	}
}

void hButtonsTask(void *pvParameters) {
	Serial.println("Thread : Buttons task started");

    for(;;) {
		k_buttons.loop();
	}
}

// --- THREADS FUNCTIONS ---

void initMac() {
	byte newMac[6] = BLUETOOTH_MAC_ADDRESS;
	const auto newMacResult = esp_base_mac_addr_set(newMac);
	if(newMacResult != ESP_OK) {
		Serial.printf("Failed to set new MAC address: %s\n", esp_err_to_name(newMacResult));
	} else {
		Serial.printf("New MAC: %02X:%02X:%02X:%02X:%02X:%02X\n", newMac[0], newMac[1], newMac[2], newMac[3], newMac[4], newMac[5]);
	}

	byte efuseMac[6];
	const auto efuseMacResult = esp_efuse_mac_get_default(efuseMac);
	if(efuseMacResult != ESP_OK) {
		Serial.printf("Failed to get efuse MAC address: %s\n", esp_err_to_name(efuseMacResult));
	} else {
		Serial.printf("E-Fuse MAC: %02X:%02X:%02X:%02X:%02X:%02X\n", efuseMac[0], efuseMac[1], efuseMac[2], efuseMac[3], efuseMac[4], efuseMac[5]);
	}

	byte actualMac[6];
	const auto baseMacResult = esp_base_mac_addr_get(actualMac);
	if(baseMacResult != ESP_OK) {
		Serial.printf("Failed to get base MAC address: %s\n", esp_err_to_name(baseMacResult));
	} else {
		Serial.printf("Base MAC: %02X:%02X:%02X:%02X:%02X:%02X\n", actualMac[0], actualMac[1], actualMac[2], actualMac[3], actualMac[4], actualMac[5]);
	}

	/*
		byte customMac[6];
		const auto customMacResult = esp_efuse_mac_get_custom(customMac);
		if(customMacResult != ESP_OK) {
			Serial.printf("Failed to get custom MAC address: %s\n", esp_err_to_name(customMacResult));
		} else {
			Serial.printf("Custom MAC: %02X:%02X:%02X:%02X:%02X:%02X\n", customMac[0], customMac[1], customMac[2], customMac[3], customMac[4], customMac[5]);
		}
	*/
}

void initPins() {
	pinMode(RELAY_SIREN_PIN, OUTPUT);
	pinMode(RELAY_TROTT_POWER_PIN, OUTPUT);
	pinMode(RELAY_FLANGE_PIN, OUTPUT);
	pinMode(RELAY_HEADLIGHTS_PIN, OUTPUT);
	pinMode(BUTTON_FLANGE_PIN, INPUT_PULLUP);
}

void setup() {
	Serial.begin(115200);
	Serial.println("Starting Trotalarm...");

	initMac();
	initPins();

	sharedData.alarmState = K_ENABLED;

	k_I2CManager.setSharedData(&sharedData);
	k_I2CManager.init();

	k_battery.setSharedData(&sharedData);
	k_battery.init();

	k_ble.setSharedData(&sharedData);
	k_ble.init();

	k_alarm.setSharedData(&sharedData);
	k_alarm.init();

	k_buttons.setSharedData(&sharedData);
	k_buttons.init();

	// Creating threads
	Serial.println("Thread : Creating I2CManager task");
    xTaskCreatePinnedToCore(hI2CManagerTask, "hI2CManagerTask", 2048, NULL, tskIDLE_PRIORITY, &xI2CManagerTaskHandle, 1);

	Serial.println("Thread : Creating Battery task");
    xTaskCreatePinnedToCore(hBatteryTask, "hBatteryTask", 2048, NULL, tskIDLE_PRIORITY, &xBatteryTaskHandle, 1);
	
	Serial.println("Thread : Creating Alarm task");
    xTaskCreatePinnedToCore(hAlarmTask, "hAlarmTask", 2048, NULL, tskIDLE_PRIORITY, &xAlarmTaskHandle, 1);

	Serial.println("Thread : Creating Buttons task");
    xTaskCreatePinnedToCore(hButtonsTask, "hButtonsTask", 2048, NULL, tskIDLE_PRIORITY, &xButtonsTaskHandle, 1);
}

void loop() {
	#if DEBUG_HEAP
		uint32_t hm1 = uxTaskGetStackHighWaterMark(NULL);
		uint32_t hm2 = uxTaskGetStackHighWaterMark(xI2CManagerTaskHandle);
		uint32_t hm3 = uxTaskGetStackHighWaterMark(xBatteryTaskHandle);
		uint32_t hm4 = uxTaskGetStackHighWaterMark(xAlarmTaskHandle);
		uint32_t hm5 = uxTaskGetStackHighWaterMark(xButtonsTaskHandle);

	    Serial.printf("RAM left = %d / HM idle = %d / HM I2CManager = %d / HM battery = %d / HM alarm = %d / HM buttons = %d\n", esp_get_free_heap_size(), hm1, hm2, hm3, hm4, hm5);
	#endif
	delay(100);
}