#ifndef _ALARM_H_
#define _ALARM_H_


#include <Arduino.h>

#include "defines.h"
#include "SharedData.h"


class Alarm
{
    public:
        Alarm(void);
        ~Alarm(void);
        
        void init();
        void loop();
        
        void setSharedData(SharedData *data);
    private:
        static SharedData *shrd;
};

#endif // _ALARM_H_