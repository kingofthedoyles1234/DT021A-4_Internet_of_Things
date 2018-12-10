/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mbed.h"
#include "ble/BLE.h"
#include "LEDService.h"
#include "tempservice.h"

/* 
 * All the LEDs on the micro:bit are part of the LED Matrix,
 * In order to get simple blinking behaviour, we set column 0
 * to be permanently at ground. If you want to use the LEDs as
 * a screen, there is a display driver in the micro:bit 'DAL',
 */
 
#define TEMP_BASE 0x4000C000
#define TEMP_START (*(volatile uint32_t *)(TEMP_BASE + 0))
#define TEMP_STOP (*(volatile uint32_t *)(TEMP_BASE + 4))
#define TEMP_DATARDY (*(volatile uint32_t *)(TEMP_BASE + 0x100))
#define TEMP_READING (*(volatile uint32_t *)(TEMP_BASE + 0x508))
 
DigitalOut col1(P0_4, 0);
DigitalOut alivenessLED(P0_13, 0);
DigitalOut actuatedLED(P0_14, 0);

const static char     DEVICE_NAME[] = "Dixon";
static const uint16_t uuid16_list[] = {LEDService::LED_SERVICE_UUID,TEMPService::TEMP_SERVICE_UUID};

LEDService *ledServicePtr;
TEMPService *TEMPServicePtr;
Ticker ticker;


void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *params)
{
    BLE::Instance().gap().startAdvertising();
}

void periodicCallback(void)
{
    alivenessLED = !alivenessLED; /* Do blinky on LED1 to indicate system aliveness. */
	updateTemperature();
}

/**
 * This callback allows the LEDService to receive updates to the ledState Characteristic.
 *
 * @param[in] params
 *     Information about the characterisitc being updated.
 */
void onDataWrittenCallback(const GattWriteCallbackParams *params) {
    if ((params->handle == ledServicePtr->getValueHandle()) && (params->len == 1)) {
        actuatedLED = *(params->data);
    }
}

void onDataReadCallback(const GattReadCallbackParams *params) {
   
}
/**
 * This function is called when the ble initialization process has failed
 */
void onBleInitError(BLE &ble, ble_error_t error)
{
    /* Initialization error handling should go here */
}

/**
 * Callback triggered when the ble initialization process has finished
 */
void bleInitComplete(BLE::InitializationCompleteCallbackContext *params)
{
    BLE&        ble   = params->ble;
    ble_error_t error = params->error;

    if (error != BLE_ERROR_NONE) {
        /* In case of error, forward the error handling to onBleInitError */
        onBleInitError(ble, error);
        return;
    }

    /* Ensure that it is the default instance of BLE */
    if(ble.getInstanceID() != BLE::DEFAULT_INSTANCE) {
        return;
    }
 
    ble.gap().onDisconnection(disconnectionCallback);
    ble.gattServer().onDataWritten(onDataWrittenCallback);
    ble.gattServer().onDataRead(onDataReadCallback);

    bool initialValueForLEDCharacteristic = false;
    ledServicePtr = new LEDService(ble, initialValueForLEDCharacteristic);
    TEMPServicePtr = new TEMPService(ble,0);
    /* setup advertising */
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *)uuid16_list, sizeof(uuid16_list));
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME));
    ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.gap().setAdvertisingInterval(1000); /* 1000ms. */
    ble.gap().startAdvertising();
}

void updateTemperature(){
	int32_t Temperature;
	TEMP_START = 1; // enable the temperature sensor
    while (TEMP_DATARDY == 0);
    Temperature = TEMP_READING/4;
	
	TEMPServicePtr->updateTEMP(Temperature);
}

int main(void)
{    
    char Data[8]; // Declare a buffer for data transfer    
    int Status;
    ticker.attach(periodicCallback, 1); /* Blink LED every second */

    BLE &ble = BLE::Instance();
    ble.init(bleInitComplete);

    /* SpinWait for initialization to complete. This is necessary because the
     * BLE object is used in the main loop below. */
    while (ble.hasInitialized()  == false) { /* spin loop */ }   
    
    while (true) {        
        ble.waitForEvent();
       
    }
}
