#ifndef _DEFINES_H
#define _DEFINES_H

// https://btprodspecificationrefs.blob.core.windows.net/assigned-values/16-bit%20UUID%20Numbers%20Document.pdf

#define DEBUG_HEAP                          false
#define SERIAL_DEBUG 						false
#define SERIAL_DEBUG_MPU 					false
#define SERIAL_DEBUG_BLE 					false
#define OLED_DISPLAY						true

#define BLUETOOTH_DEVICE_NAME 				"MyScooterAlarm"
#define BLUETOOTH_PASSKEY 					123456
#define BLUETOOTH_MAC_ADDRESS    			{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define BLUETOOTH_SERVICE_UUID 				"00000000-0000-1001-8000-00805F9B34FB"
#define BLUETOOTH_CHARACTERISTIC_UUID_READ  "00000000-0000-1002-8000-00805F9B34FB"
#define BLUETOOTH_CHARACTERISTIC_UUID_WRITE "00000000-0000-1003-8000-00805F9B34FB"

#define SENSOR_CURRENT_PIN                  34
#define SENSOR_VOLTAGE_PIN                  35
#define RELAY_SIREN_PIN						23
#define RELAY_TROTT_POWER_PIN				18
#define RELAY_FLANGE_PIN                    17
#define RELAY_HEADLIGHTS_PIN                5

#define BUTTON_FLANGE_PIN                   19
#define BUTTON_FLANGE_PRESSED_TIME          1000 // ms

#define CURRENT_MV_PER_AMP                  0.066 // 66 mV per amp
#define CURRENT_V_REF                       5.0 // 5.0V reference
#define CURRENT_ADC_SCALE                   1023.0 // 1023.0 ADC scale
#define CURRENT_ZERO_CALIBRATION            485 // 485 mV zero calibration

#define VOLTAGE_SENSOR_R1                   30000.0f // 30kOhm - R3012
#define VOLTAGE_SENSOR_R2                   7500.0f // 7.5kOhm - R7501
#define VOLTAGE_V_REF                       3.3f // 3.3V reference
#define VOLTAGE_V_REF_SCALED                VOLTAGE_V_REF / 4095.0f // 3.3V reference scaled to 4095

#define BATTERY_DISCHARGED_VOLTAGE          3.4f // 3.3V + 0.1V margin
#define BATTERY_CHARGED_VOLTAGE             4.2f // 4.2V
#define BATTERY_CURRENT_MAX                 5.0f // 5A
#define BATTERY_NUMBER_OF_CELLS             3 // 3 cells
#define BATTERY_PROTECTIONS                 true // Power off everything if battery is discharged - Power off everything if current is too high

#endif