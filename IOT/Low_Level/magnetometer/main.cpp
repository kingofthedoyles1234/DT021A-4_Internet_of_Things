#include "mbed.h"


// Magnetometer : MAG3110  I2C address
I2C i2c(P0_30, P0_0); // SDA is on P0_30, SCL is on P0_0
const int MAG3110_ADDRESS = (0x0E<<1); 
const int MAG3110_ID = 0xC4;


// Enable UART for PC communication
Serial pc(USBTX, USBRX);
int main() 
{    
    char Data[8]; // Declare a buffer for data transfer    
    int Status;


    // Verify the device is present on the I2C bus by reading the WHOAMI byte
    Data[0]=0x07;
    Status = i2c.write(MAG3110_ADDRESS,Data,1,true);  // Write register number
    Status = i2c.read(MAG3110_ADDRESS,Data,1); // Read register contents
    pc.printf("Data read=%d\r\n",(int) Data[0]);        
    if (Data[0]==MAG3110_ID)
    {
        pc.printf("MAG3110 Found on I2C Bus\r\n");
    }
    else
    {
        pc.printf("MAG3110 Not present\r\n");
        while(1); // can't proceed
    }


    // Wake the magnetometer from standby mode by writing 1 to register number 0x10    
    Data[0]=0x10; 
    Data[1]=1;
    Status = i2c.write(MAG3110_ADDRESS,Data,2);  // Write data to register
    while(1)
    {
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
        pc.printf("Magnetometer Read Out\r\n");
        pc.printf("X = %d,  Y = %d,  Z = %d\r\n",X,Y,Z);          
        wait(0.5);             
    }
}
