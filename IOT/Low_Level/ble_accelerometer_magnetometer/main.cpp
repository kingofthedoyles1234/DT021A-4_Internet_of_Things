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
#include "accelservice.h"
#include "magnetometerservice.h"

/* 
 * All the LEDs on the micro:bit are part of the LED Matrix,
 * In order to get simple blinking behaviour, we set column 0
 * to be permanently at ground. If you want to use the LEDs as
 * a screen, there is a display driver in the micro:bit 'DAL',
 */
DigitalOut col1(P0_4, 0);
DigitalOut alivenessLED(P0_13, 0);
DigitalOut actuatedLED(P0_14, 0);

const static char     DEVICE_NAME[] = "MrPotatoHead";
static const uint16_t uuid16_list[] = {LEDService::LED_SERVICE_UUID,ACCELService::ACCEL_SERVICE_UUID,MAGNETOMETERService::MAGNETOMETER_SERVICE_UUID};

LEDService *ledServicePtr;
ACCELService *AccelServicePtr;
MAGNETOMETERService *MAGNETOMETERServicePtr;
Ticker ticker;


// Accelerometer : MMA8653FC.  I2C address
I2C i2c(P0_30, P0_0); // SDA is on P0_30, SCL is on P0_0
const int MMA8653_ADDRESS = (0x1d<<1); 
const int MMA8653_ID = 0x5a;
void updateAccelerations(void);

// MAGNETOMETER : MAG3110  I2C address

const int MAG3110_ADDRESS = (0x0E<<1); 
const int MAG3110_ID = 0xC4;



void updateMagVal(void);
void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *params)
{
    BLE::Instance().gap().startAdvertising();
}

void periodicCallback(void)
{
    alivenessLED = !alivenessLED; /* Do blinky on LED1 to indicate system aliveness. */
    updateAccelerations();
    updateMagVal();
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
    AccelServicePtr = new ACCELService(ble,0);
    MAGNETOMETERServicePtr = new MAGNETOMETERService(ble,0);
    /* setup advertising */
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *)uuid16_list, sizeof(uuid16_list));
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME));
    ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.gap().setAdvertisingInterval(1000); /* 1000ms. */
    ble.gap().startAdvertising();
}
void updateAccelerations()
{
	char Data[8]; // Declare a buffer for data transfer    
	int Status;
        int16_t X;
        Data[0]=0x01; // Register number 1 has the X data (2 bytes)
        Status = i2c.write(MMA8653_ADDRESS,Data,1,true);  // Write register number
        Status = i2c.read(MMA8653_ADDRESS,Data,2); // Read register contents
        X = Data[0];
        X = (X << 8) + Data[1];
        X = X >> 6; // only 10 bits of data are available
        
        int16_t Y;
        Data[0]=0x03; // Register number 3 has the Y data (2 bytes)
        Status = i2c.write(MMA8653_ADDRESS,Data,1,true);  // Write register number
        Status = i2c.read(MMA8653_ADDRESS,Data,2); // Read register contents
        Y = Data[0];
        Y = (Y << 8) + Data[1];
        Y = Y >> 6; // only 10 bits of data are available        
        
        int16_t Z;
        Data[0]=0x05; // Register number 1 has the Z data (2 bytes)
        Status = i2c.write(MMA8653_ADDRESS,Data,1,true);  // Write register number
        Status = i2c.read(MMA8653_ADDRESS,Data,2); // Read register contents
        Z = Data[0];
        Z = (Z << 8) + Data[1];
        Z = Z >> 6; // only 10 bits of data are available
        
        AccelServicePtr->updateAccelX(X);
        AccelServicePtr->updateAccelY(Y);
        AccelServicePtr->updateAccelZ(Z);
}
void updateMagVal()
{
	char Data[8]; // Declare a buffer for data transfer    
	int Status;
        int16_t X;
        Data[0]=0x01; // Register number 1 has the X data (2 bytes)
        Status = i2c.write(MAG3110_ADDRESS,Data,1,true);  // Write register number
        Status = i2c.read(MAG3110_ADDRESS,Data,2); // Read register contents
        X = Data[0];
        X = (X << 8) + Data[1];
        
        int16_t Y;
        Data[0]=0x03; // Register number 3 has the Y data (2 bytes)
        Status = i2c.write(MAG3110_ADDRESS,Data,1,true);  // Write register number
        Status = i2c.read(MAG3110_ADDRESS,Data,2); // Read register contents
        Y = Data[0];
        Y = (Y << 8) + Data[1];     
        
        int16_t Z;
        Data[0]=0x05; // Register number 5 has the Z data (2 bytes)
        Status = i2c.write(MAG3110_ADDRESS,Data,1,true);  // Write register number
        Status = i2c.read(MAG3110_ADDRESS,Data,2); // Read register contents
        Z = Data[0];
        Z = (Z << 8) + Data[1];
        
        MAGNETOMETERServicePtr->updateMAGNETOMETERX(X);
        MAGNETOMETERServicePtr->updateMAGNETOMETERY(Y);
        MAGNETOMETERServicePtr->updateMAGNETOMETERZ(Z);
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

   // Wake the accelerometer from sleep mode by writing 1 to register number 0x2a    
    Data[0]=0x2a; 
    Data[1]=1;
    Status = i2c.write(MMA8653_ADDRESS,Data,2);  // Write data to register   

   // Wake the magnetometer from sleep mode by writing 1 to register number 0x10    
    Data[0]=0x10; 
    
    Data[1]=1;

    Status = i2c.write(MAG3110_ADDRESS,Data,2);  // Write data to register     
    
    while (true) {        
        ble.waitForEvent();
       
    }
}
