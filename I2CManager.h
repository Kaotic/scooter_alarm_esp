#ifndef _I2C_MANAGER_H_
#define _I2C_MANAGER_H_


#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_SSD1306.h>

#include "SharedData.h"


class I2CManager
{
    public:
        I2CManager(void);
        ~I2CManager(void);
        
        void init();
        void loop();
        
        void setSharedData(SharedData *data);
    private:
        static SharedData *shrd;
};

#endif // _I2C_MANAGER_H_