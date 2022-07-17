#ifndef _BUTTONS_H_
#define _BUTTONS_H_


#include <Arduino.h>
#include "utils/SimpleKalmanFilter.h"

#include "defines.h"
#include "SharedData.h"


class Buttons
{
    public:
        Buttons(void);
        ~Buttons(void);
        
        void init();
        void loop();
        
        void setSharedData(SharedData *data);
    private:
        static SharedData *shrd;

        void flangeLoop();
};

#endif // _BUTTONS_H_