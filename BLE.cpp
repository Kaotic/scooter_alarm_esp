#include "BLE.h"
#include <ArduinoJson.h>


SharedData *BLE::shrd;
BLE_STATE k_state;
BLE_STATE k_previousState = K_NOT_READY;

BLEServer *pServer = NULL;
BLEService *pService = NULL;
BLECharacteristic *pReadCharacteristic = NULL;
BLECharacteristic *pWriteCharacteristic = NULL;

std::map<uint16_t, esp_ble_auth_cmpl_t> k_authClients = {};

std::string mLastData = "{\"s\":100,\"m\":0,\"b\":{\"l\":0,\"p\":0,\"v\":0,\"c\":0},\"r\":{\"s\":0,\"p\":0,\"h\":0,\"f\":0},\"a\":{\"x\":0,\"y\":0,\"z\":0},\"g\":{\"x\":0,\"y\":0,\"z\":0}}";
std::string mLastCommand;


BLE::BLE(void){}
BLE::~BLE(void){}

void BLE::setSharedData(SharedData *data) {
	shrd = data;
}

int BLE::countAuthedDevices() {
	return k_authClients.size();
}

void BLE::initSecurity(){
	esp_ble_auth_req_t auth_req = ESP_LE_AUTH_REQ_SC_MITM_BOND;
	esp_ble_io_cap_t iocap = ESP_IO_CAP_OUT;          
	uint8_t key_size = 16;     
	uint8_t init_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
	uint8_t rsp_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
	uint32_t passkey = BLUETOOTH_PASSKEY;
	uint8_t auth_option = ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_DISABLE;
	esp_ble_gap_set_security_param(ESP_BLE_SM_SET_STATIC_PASSKEY, &passkey, sizeof(uint32_t));
	esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, &auth_req, sizeof(uint8_t));
	esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &iocap, sizeof(uint8_t));
	esp_ble_gap_set_security_param(ESP_BLE_SM_MAX_KEY_SIZE, &key_size, sizeof(uint8_t));
	esp_ble_gap_set_security_param(ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH, &auth_option, sizeof(uint8_t));
	esp_ble_gap_set_security_param(ESP_BLE_SM_SET_INIT_KEY, &init_key, sizeof(uint8_t));
	esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY, &rsp_key, sizeof(uint8_t));
}

void macPrint(uint8_t mac) {
	Serial.print(mac, HEX);
	Serial.print(":");
}

void BLE::init(){
	class KServerCallbacks: public BLEServerCallbacks {
		void onConnect(BLEServer* cServer) {
			uint16_t conId = cServer->getConnId();
			BLEClient *pClient = (BLEClient*)pServer->getPeerDevices(true).at(conId).peer_device;
			#if SERIAL_DEBUG && SERIAL_DEBUG_BLE
				Serial.printf("[%d] Device \"%s\" trying to connect", conId, pClient->getPeerAddress().toString().c_str());
			#endif
			BLEDevice::startAdvertising();
		};

		void onDisconnect(BLEServer* cServer) {
			uint16_t conId = cServer->getConnId();

			if(k_authClients.find(conId) != k_authClients.end()) {
				k_authClients.erase(conId);
				#if SERIAL_DEBUG && SERIAL_DEBUG_BLE
					Serial.printf("[%d] Authentified device disconnected", conId);
					Serial.println();
				#endif
				if(k_authClients.size() == 0) {
					#if SERIAL_DEBUG && SERIAL_DEBUG_BLE
						Serial.println("No more devices connected, stopping notifications");
						Serial.println();
					#endif
					k_state = K_DISCONNECTED;
				}
			}else{
				#if SERIAL_DEBUG && SERIAL_DEBUG_BLE
					Serial.printf("[%d] Device disconnected\n", conId);
					Serial.println();
				#endif
			}
		}
	};

	class KSecurityCallback : public BLESecurityCallbacks {
		uint32_t onPassKeyRequest(){
			return 000000;
		}

		void onPassKeyNotify(uint32_t pass_key){}

		bool onConfirmPIN(uint32_t pass_key){
			return true;
		}

		bool onSecurityRequest(){
			return true;
		}

		void onAuthenticationComplete(esp_ble_auth_cmpl_t cmpl){
			if(cmpl.success){
				#if SERIAL_DEBUG && SERIAL_DEBUG_BLE
					Serial.println(" - KSecurityCallback - Authentication Success");
				#endif
				k_state = K_CONNECTED;
				k_authClients.insert(std::pair<uint16_t, esp_ble_auth_cmpl_t>(pServer->getConnId(), cmpl));
			}else{
				#if SERIAL_DEBUG && SERIAL_DEBUG_BLE
					Serial.println(" - KSecurityCallback - Authentication Failure");
				#endif
				pServer->removePeerDevice(pServer->getConnId(), true);
			}

			BLEDevice::startAdvertising();
		}
	};

	// Trott to devices
	class KReadCharacteristicCallback: public BLECharacteristicCallbacks {
		void onWrite(BLECharacteristic* pCharacteristic) {}

		void onRead(BLECharacteristic* pCharacteristic) {
			std::string msg = pCharacteristic->getValue();
			#if SERIAL_DEBUG && SERIAL_DEBUG_BLE
				Serial.printf("BLE sending: %s\n", msg.c_str());
			#endif

			StaticJsonDocument<384> docData;

			docData["s"] = shrd->alarmState;
			docData["m"] = shrd->motionDetected ? 1 : 0;
			
			JsonObject battery = docData.createNestedObject("b");
			battery["l"] = shrd->batteryLow ? 1 : 0;
			battery["p"] = shrd->batteryPercentage;
			battery["v"] = shrd->actualVoltageFiltered;
			battery["c"] = shrd->actualCurrentFiltered;

			JsonObject relays = docData.createNestedObject("r");
			relays["s"] = shrd->isSirenEnabled ? 1 : 0;
			relays["p"] = shrd->isPowerEnabled ? 1 : 0;
			relays["h"] = shrd->isHeadlightsEnabled ? 1 : 0;
			relays["f"] = shrd->isFlangeEnabled ? 1 : 0;

			JsonObject accelerometer = docData.createNestedObject("a");
			accelerometer["x"] = shrd->mpuAcceleration.x;
			accelerometer["y"] = shrd->mpuAcceleration.y;
			accelerometer["z"] = shrd->mpuAcceleration.z;

			JsonObject gyroscope = docData.createNestedObject("g");
			gyroscope["x"] = shrd->mpuGyroscope.x;
			gyroscope["y"] = shrd->mpuGyroscope.y;
			gyroscope["z"] = shrd->mpuGyroscope.z;

			std::string jsonOutput;
			serializeJson(docData, jsonOutput);

			pCharacteristic->setValue(jsonOutput);
			pCharacteristic->notify();
		}
	};

	// Devices to trott
	class KWriteCharacteristicCallback: public BLECharacteristicCallbacks {
		void onWrite(BLECharacteristic* pCharacteristic) {
			std::string msg = pCharacteristic->getValue();
			#if SERIAL_DEBUG && SERIAL_DEBUG_BLE
				Serial.printf("BLE writing: %s\n", msg.c_str());
			#endif

			DynamicJsonDocument docCommand(200);
			deserializeJson(docCommand, msg);

			// c : command
			if(docCommand.containsKey("c")) {
				std::string command = docCommand["c"];
				
				// cs : change state
				if(command == "cs" && docCommand.containsKey("s")){
					int state = docCommand["s"];

					if(state == S_DISABLE_ALARM) {
						shrd->toggleAlarm(false);
					}else if(state == S_ENABLE_ALARM) {
						shrd->toggleAlarm(true);
					} else if(state == S_TEST_MODE) {
						shrd->alarmState = TEST_MODE;
					} else if(state == S_NOTHING_MODE) {
						shrd->alarmState = NOTHING_MODE;
					} else if(state == S_DISABLE_SIREN) {
						shrd->switchSiren(false);
					} else if(state == S_ENABLE_SIREN) {
						shrd->switchSiren(true);
					} else if(state == S_DISABLE_POWER) {
						shrd->switchPower(false);
					} else if(state == S_ENABLE_POWER) {
						shrd->switchPower(true);
					} else if(state == S_DISABLE_HEADLIGHTS) {
						shrd->switchHeadlights(false);
					} else if(state == S_ENABLE_HEADLIGHTS) {
						shrd->switchHeadlights(true);
					} else if(state == S_DISABLE_FLANGE) {
						shrd->switchFlange(false);
					} else if(state == S_ENABLE_FLANGE) {
						shrd->switchFlange(true);
					} else if(state == S_DISABLE_BATTERY_PROTECTIONS) {
						shrd->batteryProtections = false;
					} else if(state == S_ENABLE_BATTERY_PROTECTIONS) {
						shrd->batteryProtections = true;
					}
				}
			}
		}

		void onRead(BLECharacteristic* pCharacteristic) {}
	};

    Serial.println("Initializing BLE...");
	BLEDevice::init(BLUETOOTH_DEVICE_NAME);

	pServer = BLEDevice::createServer();
	pServer->setCallbacks(new KServerCallbacks());

	BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT);
	BLEDevice::setSecurityCallbacks(new KSecurityCallback());

	BLEService *pService = pServer->createService(BLUETOOTH_SERVICE_UUID);

	BLE2902 *readDescriptor = new BLE2902();
	readDescriptor->setNotifications(true);
	readDescriptor->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);

	pReadCharacteristic = pService->createCharacteristic(
		BLUETOOTH_CHARACTERISTIC_UUID_READ,
		BLECharacteristic::PROPERTY_READ |
		BLECharacteristic::PROPERTY_NOTIFY
	);

	pReadCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
	pReadCharacteristic->setCallbacks(new KReadCharacteristicCallback());
	pReadCharacteristic->setValue(mLastData);
	pReadCharacteristic->addDescriptor(readDescriptor);

	BLE2902 *writeDescriptor = new BLE2902();
	writeDescriptor->setNotifications(true);
	writeDescriptor->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);

	pWriteCharacteristic = pService->createCharacteristic(
		BLUETOOTH_CHARACTERISTIC_UUID_WRITE,
		BLECharacteristic::PROPERTY_WRITE
	);

	pWriteCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
	pWriteCharacteristic->setCallbacks(new KWriteCharacteristicCallback());
	pWriteCharacteristic->setValue("{}");
	pWriteCharacteristic->addDescriptor(writeDescriptor);

	pService->start();
	
	BLEAdvertising *pAdvertising = pServer->getAdvertising();
	pAdvertising->addServiceUUID(pService->getUUID());
	pAdvertising->setAppearance(0x0980);
	//pAdvertising->setScanResponse(false);
	//pAdvertising->setMinPreferred(0x0); // set value to 0x00 to not advertise this parameter

	initSecurity();

	pAdvertising->start();

	Serial.println("BLE initialized, waiting for connection...");
}

void BLE::loop(){
}

void BLE::setData(std::string data){
	mLastData = data;
}

void BLE::setCommand(std::string command){
	mLastCommand = command;
}

std::string BLE::getData(){
	return mLastData;
}

std::string BLE::getCommand(){
	return mLastCommand;
}
