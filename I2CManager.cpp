#include "I2CManager.h"

SharedData *I2CManager::shrd;
Adafruit_MPU6050 mpu;
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);


I2CManager::I2CManager(void){}
I2CManager::~I2CManager(void){}

void I2CManager::setSharedData(SharedData *data) {
	shrd = data;
}

void I2CManager::loop()
{
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    shrd->mpuAcceleration = a.acceleration;
    shrd->mpuGyroscope = g.gyro;
    shrd->mpuTemperature = temp.temperature;

    #if OLED_DISPLAY
        display.clearDisplay();
        display.setCursor(0, 0);

        display.print(shrd->actualVoltageFiltered, 2);
        display.print("v");
        display.print(" | ");
        display.print(shrd->actualCurrentFiltered, 2);
        display.print("A");
        display.print(" | ");
        display.print(shrd->batteryPercentage);
        display.print("%");
        display.println("");

        display.print("ENABLED: ");
        display.print(shrd->alarmState == 0 ? "OFF" : "ON");
        display.println("");

        display.print("STATE: ");
        display.print(shrd->alarmState);
        display.println("");

        display.print("MOTION: ");
        display.print(shrd->motionDetected ? "YES" : "NO");
        display.println("");
        display.println("");

        display.print("A : ");
        display.print(shrd->mpuAcceleration.x, 1);
        display.print(", ");
        display.print(shrd->mpuAcceleration.y, 1);
        display.print(", ");
        display.print(shrd->mpuAcceleration.z, 1);
        display.println("");

        display.print("G : ");
        display.print(shrd->mpuGyroscope.x, 1);
        display.print(", ");
        display.print(shrd->mpuGyroscope.y, 1);
        display.print(", ");
        display.print(shrd->mpuGyroscope.z, 1);
        display.println("");

        display.display();
    #endif
    
    vTaskDelay(150);
}

void I2CManager::init()
{
    if (!mpu.begin()) {
		Serial.println("Sensor : MPU-6050 allocation failed");
		for (;;)
			; // Don't proceed, loop forever
	}
	Serial.println("Sensor : MPU-6050 found");


    #if OLED_DISPLAY
        if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
            Serial.println(F("Display : SSD1306 allocation failed"));
            for (;;)
                ; // Don't proceed, loop forever
        }
        Serial.println("Display : SSD1306 found");

        display.display();
        delay(500);
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setRotation(0);
    #endif
}
