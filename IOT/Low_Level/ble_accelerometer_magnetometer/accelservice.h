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

#ifndef __BLE_ACCEL_SERVICE_H__
#define __BLE_ACCEL_SERVICE_H__

class ACCELService {
public:
    const static uint16_t ACCEL_SERVICE_UUID = 0xA012;
    const static uint16_t ACCEL_X_CHARACTERISTIC_UUID = 0xA013;
    const static uint16_t ACCEL_Y_CHARACTERISTIC_UUID = 0xA014;
    const static uint16_t ACCEL_Z_CHARACTERISTIC_UUID = 0xA015;

    ACCELService(BLEDevice &_ble, int16_t initialValueForACCELCharacteristic) :
        ble(_ble), AccelX(ACCEL_X_CHARACTERISTIC_UUID, &initialValueForACCELCharacteristic),AccelY(ACCEL_Y_CHARACTERISTIC_UUID, &initialValueForACCELCharacteristic),AccelZ(ACCEL_Z_CHARACTERISTIC_UUID, &initialValueForACCELCharacteristic)
    {
        GattCharacteristic *charTable[] = {&AccelX,&AccelY,&AccelZ};
        GattService         AccelService(ACCEL_SERVICE_UUID, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));
        ble.addService(AccelService);
    }

    GattAttribute::Handle_t getValueHandle() const {
        return AccelX.getValueHandle();
    }
    void updateAccelX(uint16_t newValue) {
        ble.gattServer().write(AccelX.getValueHandle(), (uint8_t *)&newValue, sizeof(uint16_t));
    }
    void updateAccelY(uint16_t newValue) {
        ble.gattServer().write(AccelY.getValueHandle(), (uint8_t *)&newValue, sizeof(uint16_t));
    }
    void updateAccelZ(uint16_t newValue) {
        ble.gattServer().write(AccelZ.getValueHandle(), (uint8_t *)&newValue, sizeof(uint16_t));
    }

private:
    BLEDevice &ble;
    ReadOnlyGattCharacteristic<int16_t>  AccelX;
    ReadOnlyGattCharacteristic<int16_t>  AccelY;
    ReadOnlyGattCharacteristic<int16_t>  AccelZ;
};

#endif /* #ifndef __BLE_ACCEL_SERVICE_H__ */

