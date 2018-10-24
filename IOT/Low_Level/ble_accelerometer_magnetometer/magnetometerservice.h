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

#ifndef __BLE_MAGNETOMETER_SERVICE_H__
#define __BLE_MAGNETOMETER_SERVICE_H__

class MAGNETOMETERService {
public:
    const static uint16_t MAGNETOMETER_SERVICE_UUID = 0xA016;
    const static uint16_t MAGNETOMETER_X_CHARACTERISTIC_UUID = 0xA017;
    const static uint16_t MAGNETOMETER_Y_CHARACTERISTIC_UUID = 0xA018;
    const static uint16_t MAGNETOMETER_Z_CHARACTERISTIC_UUID = 0xA019;

    MAGNETOMETERService(BLEDevice &_ble, int16_t initialValueForMAGNETOMETERCharacteristic) :
        ble(_ble), MAGNETOMETERX(MAGNETOMETER_X_CHARACTERISTIC_UUID, &initialValueForMAGNETOMETERCharacteristic),MAGNETOMETERY(MAGNETOMETER_Y_CHARACTERISTIC_UUID, &initialValueForMAGNETOMETERCharacteristic),MAGNETOMETERZ(MAGNETOMETER_Z_CHARACTERISTIC_UUID, &initialValueForMAGNETOMETERCharacteristic)
    {
        GattCharacteristic *charTable[] = {&MAGNETOMETERX,&MAGNETOMETERY,&MAGNETOMETERZ};
        GattService         MAGNETOMETERService(MAGNETOMETER_SERVICE_UUID, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));
        ble.addService(MAGNETOMETERService);
    }

    GattAttribute::Handle_t getValueHandle() const {
        return MAGNETOMETERX.getValueHandle();
    }
    void updateMAGNETOMETERX(uint16_t newValue) {
        ble.gattServer().write(MAGNETOMETERX.getValueHandle(), (uint8_t *)&newValue, sizeof(uint16_t));
    }
    void updateMAGNETOMETERY(uint16_t newValue) {
        ble.gattServer().write(MAGNETOMETERY.getValueHandle(), (uint8_t *)&newValue, sizeof(uint16_t));
    }
    void updateMAGNETOMETERZ(uint16_t newValue) {
        ble.gattServer().write(MAGNETOMETERZ.getValueHandle(), (uint8_t *)&newValue, sizeof(uint16_t));
    }

private:
    BLEDevice &ble;
    ReadOnlyGattCharacteristic<int16_t>  MAGNETOMETERX;
    ReadOnlyGattCharacteristic<int16_t>  MAGNETOMETERY;
    ReadOnlyGattCharacteristic<int16_t>  MAGNETOMETERZ;
};

#endif /* #ifndef __BLE_MAGNETOMETER_SERVICE_H__ */

