#include "mbed.h"


// Accelerometer : MMA8653FC.  I2C address
I2C i2c(P0_30, P0_0); // SDA is on P0_30, SCL is on P0_0
const int MMA8653_ADDRESS = (0x1d<<1); 
const int MMA8653_ID = 0x5a;

// Magnetometer : MAG3110  I2C address
const int MAG3110_ADDRESS = (0x0E<<1); 
const int MAG3110_ID = 0xC4;

DigitalOut col0(P0_4, 0);

DigitalOut myled(P0_13);
#define TEMP_BASE 0x4000C000
#define TEMP_START (*(volatile uint32_t *)(TEMP_BASE + 0))
#define TEMP_STOP (*(volatile uint32_t *)(TEMP_BASE + 4))
#define TEMP_DATARDY (*(volatile uint32_t *)(TEMP_BASE + 0x100))
#define TEMP_READING (*(volatile uint32_t *)(TEMP_BASE + 0x508))


// Enable UART for PC communication
Serial pc(USBTX, USBRX);

int readTemperature()
{
    TEMP_START = 1; // enable the temperature sensor
    while (TEMP_DATARDY == 0);
    return TEMP_READING/4;
}

int main() 
{    
    char Data[8]; // Declare a buffer for data transfer    
    int Status;


    // Verify the accelerometer is present on the I2C bus by reading the WHOAMI byte
    Data[0]=0x0d;
    Status = i2c.write(MMA8653_ADDRESS,Data,1,true);  // Write register number
    Status = i2c.read(MMA8653_ADDRESS,Data,1); // Read register contents
    pc.printf("Data read=%d\r\n",(int) Data[0]);        
    if (Data[0]==MMA8653_ID)
    {
        pc.printf("MMA8653 Found on I2C Bus\r\n");
    }
    else
    {
        pc.printf("MMA8653 Not present\r\n");
        while(1); // can't proceed
    }
        
    // Verify the magnetometer is present on the I2C bus by reading the WHOAMI byte
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


    // Wake the accelerometer from sleep mode by writing 1 to register number 0x2a    
    Data[0]=0x2a; 
    Data[1]=1;
    Status = i2c.write(MMA8653_ADDRESS,Data,2);  // Write data to register
    
    // Wake the magnetometer from standby mode by writing 1 to register number 0x10    
    Data[0]=0x10; 
    Data[1]=1;
    Status = i2c.write(MAG3110_ADDRESS,Data,2);  // Write data to register
    
    int Temperature;
    char Message[20];
    
    while(1)
    {
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
        Data[0]=0x05; // Register number 5 has the Z data (2 bytes)
        Status = i2c.write(MMA8653_ADDRESS,Data,1,true);  // Write register number
        Status = i2c.read(MMA8653_ADDRESS,Data,2); // Read register contents
        Z = Data[0];
        Z = (Z << 8) + Data[1];
        Z = Z >> 6; // only 10 bits of data are available
        
        int16_t A;
        Data[0]=0x01; // Register number 1 has the A data (2 bytes)
        Status = i2c.write(MAG3110_ADDRESS,Data,1,true);  // Write register number
        Status = i2c.read(MAG3110_ADDRESS,Data,2); // Read register contents
        A = Data[0];
        A = (X << 8) + Data[1];       
        int16_t B;
        Data[0]=0x03; // Register number 3 has the B data (2 bytes)
        Status = i2c.write(MAG3110_ADDRESS,Data,1,true);  // Write register number
        Status = i2c.read(MAG3110_ADDRESS,Data,2); // Read register contents
        B = Data[0];
        B = (Y << 8) + Data[1];   
        int16_t C;
        Data[0]=0x05; // Register number 5 has the C data (2 bytes)
        Status = i2c.write(MAG3110_ADDRESS,Data,1,true);  // Write register number
        Status = i2c.read(MAG3110_ADDRESS,Data,2); // Read register contents
        C = Data[0];
        C = (Z << 8) + Data[1];  
        
        Temperature=readTemperature();
        
        pc.printf("Accelerometer:\tX = %d,\t\tY = %d,\tZ = %d\r\n",X,Y,Z);
        pc.printf("Magnetometer:\tX = %d,\tY = %d,\tZ = %d\r\n",A,B,C); 
        sprintf(Message,"Temperature:\t%02d Degrees Celsius",Temperature);
        pc.printf("%s\r\n\n",Message );
        
        myled = 1;
        wait(0.25);  
        myled = 0;
        wait(0.25);           
    }
}
