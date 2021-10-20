// This code is a test bed to extract all 4 cap measurments from the FDC 1004 by building 1 function



#include<Wire.h>
#include <SPI.h>


#define lowByte(w) ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))

// set pin 10 as the slave select for the digital pot:
const int slaveSelectPin = 10;
/*

  * CS - to digital pin 10  (SS pin)
  * SDI - to digital pin 11 (MOSI pin)
  * CLK - to digital pin 13 (SCK pin)
  
*/

int analogPin = 0;      // analog in sensor 1 connected to analog pin 0
int data;
float val = 0;           // variable to store the value read


// From the ti FDC1004 data sheet. 

// 1. Configure measurements (for details, refer to Measurement Configuration).
// 2. Trigger a measurement set (for details, refer to Triggering Measurements).
// 3. Wait for measurement completion (for details, refer to Wait for Measurement Completion).
// 4. Read measurement data (for details, refer to Read of Measurement Result).

// Capacitance (pf) = ((Two's Complement (measurement [23:0])) / 2^19 ) + Coffset

//  2^19 = 524288

// Set point for SEC base 

int base = 100;

// define bytes to uses in 
int FDC=B1010000;
int del=5;
int Offsett;
byte Value1=B00010000;
byte Value2=B00000000;
byte Value3=B00001001;
byte Value4=B10000000;
byte pt1=0x08;
byte pt2 = 0x0C;
float Cap1;
unsigned long Mess1;
int new_data; 
byte MSB;
byte LSB;
float val2;

int mapLow;
int mapHigh;

int speedPin = 7;      // analog in sensor 1 connected to analog pin 0
boolean speedPinState;

//int sensorPin = 0;      // analog out sensor 1 connected to analog pin 0

void setup()
{
  
  
  
  Serial.begin(9600);  // open serial communcition
  Wire.begin();        // initialize I2C libary
  SPI.begin();         // initialize SPI libary
 
 
 
  

  Config(0x08,01110011,00100000);  // Configure Measurment channel 1 
  
  delay (100);

  Config(0x0C,00000101,10010000);  // Trigger a measurement


//  Config(0x09,00010011,00100000);  // Configure Measurment channel 1 
//  Config(0x0C,00000101,01000000);  // Trigger a measurement
 
   delay(500);
}


void loop()
{

//delay (100);

int pointer = 0x00;


  unsigned int a,b;
  unsigned long c=0;
 
   
  a = getinfo(pointer);
  delay(del);
  b = getinfo(pointer+1);
  delay(del);
  c=a;
  c=c<<16;
  c|=b;
  Serial.print("data  -  ");
  Serial.println(c);
  


delay(500);

}








/////////////////////////////////////////////////////////////////
boolean conversion()
{
  byte b=0;
  Wire.beginTransmission(FDC);
  Wire.write(0x0c);
  Wire.endTransmission();
  delay(del);
  Wire.requestFrom(FDC,2);
  b=Wire.read();
  b=Wire.read();
  delay(del);
  b&=B00001111;
  Serial.print("conversion: ");
  Serial.print(b);
  if (b==(B00001111))
  {

    return (0);
  }
  else return (1);
}

///////////////////////////////////////////////////////////////////
unsigned int getinfo(byte pointer)
{
  byte a,b;
  unsigned int c;
  Wire.beginTransmission(FDC);
  Wire.write(pointer);
  Wire.endTransmission();
  delay(del);
  Wire.requestFrom(FDC,2);
  a=Wire.read();
  b=Wire.read();
  delay(del);
  c=a;
  c=c<<8;
  c|=b;
  // Serial.print(a,HEX);
  // Serial.print(b,HEX);
  // Serial.print(c,HEX);
  return(c);
}

/////////////////////////////////////////////////////////////////////
unsigned long getdata(byte pointer)
{
  unsigned int a,b;
  unsigned long c=0;
  a = getinfo(pointer);
  delay(del);
  b = getinfo(pointer+1);
  delay(del);
  c=a;
  c=c<<16;
  c|=b;
  // Serial.print(a,HEX);
  // Serial.print(b,HEX);
  // Serial.print(c,HEX);
  return(c);

}

////////////////////////////////////////////////////////////////////////
void Config(byte addresse, byte val1, byte val2)
{
  Wire.beginTransmission(FDC);
  Wire.write(addresse);
  Wire.write(val1);
  Wire.write(val2);
  Wire.endTransmission();
  delay (del);

}

////////////////////////////////////////////////////////////////////////
void digitalOutWrite(int w1, int w2) {
  // take the SS pin low to select the chip:
  digitalWrite(slaveSelectPin, LOW);
  //  send in the address and value via SPI:
  SPI.transfer(w1);
  //SPI.transfer(w2);
  // take the SS pin high to de-select the chip:
  digitalWrite(slaveSelectPin, HIGH);
}


////////////////////////////////////////////////////////////////////////

int dataPrep(int data){
    int a = data;
    int b = a << 2;   // binary: 0000000000101000, or 40 in decimal
    int mating_v = 28672;
    //int d =  92;    // in binary: 0000000001011100
    int c = b | mating_v;  // result:    0000000001000100, or 68 in decimal.
    return(c);
}










