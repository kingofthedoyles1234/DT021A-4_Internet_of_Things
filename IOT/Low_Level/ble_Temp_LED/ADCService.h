// Additional service to extend the basic LED example for the BBC microbit.
#ifndef __BLE_ADC_SERVICE_H__
#define __BLE_ADC_SERVICE_H__

class ADCService {
public:
    const static uint16_t ADC_SERVICE_UUID              = 0xA002;
    const static uint16_t ADC_VALUE_CHARACTERISTIC_UUID = 0xA003;

    ADCService(BLEDevice &_ble, uint16_t initialValueForADCCharacteristic) :
        ble(_ble), ADCValue(ADC_VALUE_CHARACTERISTIC_UUID, &initialValueForADCCharacteristic)
    {
        GattCharacteristic *charTable[] = {&ADCValue};
        GattService         ADCService(ADC_SERVICE_UUID, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));
        ble.addService(ADCService);
    }

    GattAttribute::Handle_t getValueHandle() const {
        return ADCValue.getValueHandle();
    }
    void updateADCValue(uint16_t newValue) {
        ble.gattServer().write(ADCValue.getValueHandle(), (uint8_t *)&newValue, sizeof(uint16_t));
    }

private:
    BLEDevice                         &ble;
    ReadWriteGattCharacteristic<uint16_t>  ADCValue;
};

#endif /* #ifndef __BLE_ADC_SERVICE_H__ */
