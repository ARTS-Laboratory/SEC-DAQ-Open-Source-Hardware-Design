#include<Wire.h>


// From the ti FDC1004 data sheet. 

// 1. Configure measurements (for details, refer to Measurement Configuration).
// 2. Trigger a measurement set (for details, refer to Triggering Measurements).
// 3. Wait for measurement completion (for details, refer to Wait for Measurement Completion).
// 4. Read measurement data (for details, refer to Read of Measurement Result).

// Capacitance (pf) = ((Two's Complement (measurement [23:0])) / 2^19 ) + Coffset

//  2^19 = 524288

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


int sensorPin = 0;      // analog out sensor 1 connected to analog pin 0

void setup()
{
  Serial.begin(9600);
  Wire.begin();


//////////////////////////////////

// bits [9:5] Offset b00000 0pF (minimum programmable offset)

      Config(pt1,00010011,00100000);  // Configure measurement
      Config(pt2,00001001,10000000);  // Trigger a measurement
      
      pinMode(0, OUTPUT);   // sets the pin as output
      pinMode(1, OUTPUT);   // sets the pin as output


}


void loop()
{

//delay (100);

      unsigned long Mess1;
      //float Mess1;
      float Cap1;
      
      Mess1=getdata(0x00);
      //delay(10);
      Mess1=Mess1/256;  //  not sure why this is here
      Cap1 = ((float) Mess1 / 524288 )*3.125 + 25*3.125;
      //Serial.print("data - Mess1 : ");
      //Serial.println(Mess1,DEC);
       Serial.print("data - Cap1  : ");
      Serial.println(Cap1,DEC);
      delay(10);
      
      
      Cap1 = map(Cap1, 80, 100, 0, 255);
      //analogWrite(9, val);
      
      analogWrite(sensorPin, Cap1);  //  analogWrite values from 0 to 255
      analogWrite(1, 255);  //  analogWrite values from 0 to 255
      
      
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





