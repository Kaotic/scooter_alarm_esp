#ifndef _BLE_H_
#define _BLE_H_

#include "defines.h"
#include "SharedData.h"

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

typedef enum
{
	K_NOT_READY = 0,
	K_READY = 1,
	K_CONNECTED = 5,
	K_DISCONNECTED = 10
} BLE_STATE;

class BLE
{
	public:
		BLE(void);
		~BLE(void);

		void setSharedData(SharedData *data);

		int countAuthedDevices();
		void init();
		void loop();

		void setData(std::string);
		void setCommand(std::string);
		std::string getData();
		std::string getCommand();

	private:
		static SharedData *shrd;
		void initSecurity();
};

#endif