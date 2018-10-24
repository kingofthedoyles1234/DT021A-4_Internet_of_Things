#include "mbed.h"

DigitalOut col0(P0_4, 0);

DigitalOut myled(P0_13);
Serial pc(USBTX, USBRX);
#define TEMP_BASE 0x4000C000
#define TEMP_START (*(volatile uint32_t *)(TEMP_BASE + 0))
#define TEMP_STOP (*(volatile uint32_t *)(TEMP_BASE + 4))
#define TEMP_DATARDY (*(volatile uint32_t *)(TEMP_BASE + 0x100))
#define TEMP_READING (*(volatile uint32_t *)(TEMP_BASE + 0x508))
int readTemperature()
{
    TEMP_START = 1; // enable the temperature sensor
    while (TEMP_DATARDY == 0);
    return TEMP_READING/4;
}
int main() {
    int Temperature;
    char Message[20];
    while(1) {
        Temperature=readTemperature();
        sprintf(Message,"Temperature is [%02d Degrees Celsius]",Temperature);
        pc.printf("%s\r\n",Message );
        myled = 1;
        wait(0.25);  
        myled = 0;
        wait(0.25);
    }
}
