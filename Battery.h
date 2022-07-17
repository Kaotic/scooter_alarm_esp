#ifndef _BATTERY_H_
#define _BATTERY_H_


#include <Arduino.h>
#include "utils/SimpleKalmanFilter.h"

#include "defines.h"
#include "SharedData.h"


class Battery
{
    public:
        Battery(void);
        ~Battery(void);
        
        void init();
        void loop();
        
        void setSharedData(SharedData *data);
    private:
        static SharedData *shrd;
};

#endif // _BATTERY_H_