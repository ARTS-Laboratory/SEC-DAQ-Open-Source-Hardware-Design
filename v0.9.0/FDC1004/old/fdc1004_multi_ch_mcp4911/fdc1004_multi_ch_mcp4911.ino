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
 // Serial.begin(9600);
  Wire.begin();


//////////////////////////////////

// bits [9:5] Offset b00000 0pF (minimum programmable offset)

      Config(pt1,00010011,00100000);  // Configure measurement
      Config(pt2,00001001,10000000);  // Trigger a measurement
      
     // pinMode(0, OUTPUT);   // sets the pin as output
      //pinMode(1, OUTPUT);   // sets the pin as output

      // set the slaveSelectPin as an output:
      pinMode(slaveSelectPin, OUTPUT);
      pinMode(speedPin, OUTPUT);
     
      // initialize SPI:
      SPI.begin();
      Serial.println("test");


      int mapLow = base - 5;
      int mapHigh = base + 5;
      
      
      speedPinState = true;
      

}


void loop()
{

//delay (100);


     
      Mess1=getdata(0x00);
   
      Mess1=Mess1/256;  //  a poor way to remove the last four digits, could also   >> 8
      Cap1 = ((float) Mess1 / 524288 )*3.125 + 25*3.125;   // changes data to capacitance.  could remove to speed up code.
     
      //Serial.print("data - Mess1 : ");
      //Serial.println(Mess1,DEC);
      //Serial.print("data - Cap1  : ");
      //Serial.println(Cap1,DEC);
   
      
 
      //data = map(Cap1, mapLow, mapHigh, 0, 1023);
      data = map(Cap1, 90, 120, 0, 1023);
      
     if (data > 1023)
           {
     data = 1023;
           }
   
      
     // Serial.print("data -  : ");
      //Serial.println(data,DEC);
      
      new_data = dataPrep(data);

      MSB = highByte(new_data);   //mask LSB, shift 8 bits to the right
      LSB = lowByte(new_data);             //mask MSB, no need to shift
      
      //Serial.print ("MSB  ");
     // Serial.println(MSB ,BIN);   
   
    //  Serial.print ("LSB  ");
    //  Serial.println(LSB ,BIN);   
   
      digitalWrite(slaveSelectPin, LOW);
      SPI.transfer ( MSB );
      SPI.transfer ( LSB );
      digitalWrite(slaveSelectPin, HIGH);


     // val = analogRead(analogPin);    // read the input pin
  
      

      // val2 = (val/1023)*5;

     // Serial.print ("data read   ");
    //  Serial.println(val);             // debug value
   
      //Serial.print ("voltage read   ");
      //Serial.println(val2);             // debug value
      //delay(10);
     // Serial.println("sssssssssssssssssssssssssssssssssssssss");             // debug value




if (speedPinState == false)
{
  digitalWrite(speedPin,LOW);
  speedPinState = true;
}
else
{
  digitalWrite(speedPin,HIGH);
  speedPinState = false;
}
//Serial.println(speedPinState);

delay(1);

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










