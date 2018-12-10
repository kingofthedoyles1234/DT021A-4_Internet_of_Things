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

#ifndef __BLE_TEMP_SERVICE_H__
#define __BLE_TEMP_SERVICE_H__

class TEMPService {
public:
    const static uint32_t TEMP_SERVICE_UUID = 0xA016;
    const static uint32_t TEMP_CHARACTERISTIC_UUID = 0xA017;
    
    TEMPService(BLEDevice &_ble, int32_t initialValueForTEMPCharacteristic) :
        ble(_ble), TEMP(TEMP_CHARACTERISTIC_UUID, &initialValueForTEMPCharacteristic)
		{
        GattCharacteristic *charTable[] = {&TEMP};
        GattService         TEMPService(TEMP_SERVICE_UUID, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));
        ble.addService(TEMPService);
    }

    GattAttribute::Handle_t getValueHandle() const {
        return TEMP.getValueHandle();
    }
    void updateTEMP(uint32_t newValue) {
        ble.gattServer().write(TEMP.getValueHandle(), (uint8_t *)&newValue, sizeof(uint32_t));
    }
    
private:
    BLEDevice &ble;
    ReadOnlyGattCharacteristic<int32_t>  TEMP;
};

#endif /* #ifndef __BLE_TEMP_SERVICE_H__ */

