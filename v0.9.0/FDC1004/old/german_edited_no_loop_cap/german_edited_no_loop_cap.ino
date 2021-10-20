#include<Wire.h>

int FDC=B1010000;
byte FDC_CONFIG = 0x0C;
byte RegisterValue1= B00000100;
byte RegisterValue21=B10000000;
byte RegisterValue22=B01000000;
byte RegisterValue23=B00100000;
byte RegisterValue24=B00010000;
//byte MessConf1=0x08;
byte MessConfval11=B00010000;
byte MessConfval21=B00000000;
byte MessConf2=0x09;
byte MessConfval12=B00110000;
byte MessConfval22=B00000000;
byte MessConf3=0x0A;
byte MessConfval13=B01010000;
byte MessConfval23=B00000000;
byte MessConf4=0x0B;
byte MessConfval14=B01111100;
byte MessConfval24=B00000000;
int num =0;
int num1 =0;
int del=5;


void setup()
{
  Serial.begin(9600);
  Wire.begin();

}
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
  if (b==(B00001111))
  {

    return (0);
  }
  else return (1);
}


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


void Config(byte addresse, byte val1, byte val2)
{
  Wire.beginTransmission(FDC);
  Wire.write(addresse);
  Wire.write(val1);
  Wire.write(val2);
  Wire.endTransmission();
  delay (del);

}


// All this looping is just changing my capdac

void loop()
{

delay (100);

    num1=0;
    MessConfval21=0;
    MessConfval22=0;
    MessConfval23=0;
    MessConfval24=0;

      unsigned long Mess1,Mess2,Mess3,Mess4;
      Config(0x08,MessConfval11,MessConfval21);
      Config(FDC_CONFIG,RegisterValue1,RegisterValue21); // always the same, only updates the enabled measurment every time for the foccert measurment.
      
      /*
      Serial.print("MessConfval11: ");
      Serial.println(MessConfval11,BIN);
      Serial.print("MessConfval21: ");
      Serial.println(MessConfval21,BIN);
      */
      
      delay(10);
      Mess1=getdata(0x00);
      Config(MessConf2,MessConfval12,MessConfval22);
      Config(FDC_CONFIG,RegisterValue1,RegisterValue22);
      delay(10);
      Mess2=getdata(0x02);
      Config(MessConf3,MessConfval13,MessConfval23);
      Config(FDC_CONFIG,RegisterValue1,RegisterValue23);
      delay(10);
      Mess3=getdata(0x04);
      Config(MessConf4,MessConfval14,MessConfval24);
      Config(FDC_CONFIG,RegisterValue1,RegisterValue24);
      delay(10);
      Mess4=getdata(0x06);
      Mess1=Mess1/256;
      Mess2=Mess2/256;
      Mess3=Mess3/256;
      Mess4=Mess4/256;
      //Serial.print("Messung: ");
      Serial.print(Mess1,DEC);
      Serial.print(" , ");
      Serial.print(Mess2,DEC);
      Serial.print(" , ");
      Serial.print(Mess3,DEC);
      Serial.print(" , ");
      Serial.println(Mess4,DEC);



 

}
