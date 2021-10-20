#include<Wire.h>

// these two functions are for spliting high and low bits
#define lowByte(w) ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))



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



// Define varaibles for the code

// I2C buss address for the FDC1004 used
int FDC=B1010000; 

// address used in the FDC1004 for the FDC Configuration Register, used for triggering and setting the measurment channel
byte FDC_CONFIG = 0x0C;

// regesiter values for the Capacitance to Digital Configuration regester, located at 0x0C
byte RegisterValue1= B00000100;
byte RegisterValueCh1=B10000000;
byte RegisterValueCh2=B01000000;
byte RegisterValueCh3=B00100000;
byte RegisterValueCh4=B00010000;

// Sets the Measurement Configuration Registers values to the correct register.  This does channel 1.
byte Ch1Register=0x08;
byte Ch1Byte1=B00010000;
byte Ch1Byte2=B00000000;

// Sets the Measurement Configuration Registers values to the correct register.  This does channel 2.
byte Ch2Register=0x09;
byte Ch2Byte1=B00110000;
byte Ch2Byte2=B00000000;

// Sets the Measurement Configuration Registers values to the correct register.  This does channel 3.
byte Ch3Register=0x0A;
byte Ch3Byte1=B01010000;
byte Ch3Byte2=B00000000;

// Sets the Measurement Configuration Registers values to the correct register.  This does channel 4.
byte Ch4Register=0x0B;
byte Ch4Byte1=B01110000;  // was B01111100 in the German code, not wure why.
byte Ch4Byte2=B00000000;


// set the value of the capdac offset for the FDC1004 chip. value in pF.
int CapDAC = 80;  // Cap offset in Pf. Max of 96 PF.
int ConfigRegLow;
int ConfigRegHigh;







void setup()
{
  
  // open the serial port
  Serial.begin(9600);
  
  // load the wire libary
  Wire.begin();

  // Set the CapDAC amount in the Measurement Configuration Registers



  CapDAC = CapDAC/3.125; // convert the pF value to the interger for a single sided capacitance measurment
   Serial.print("CapDAC");
  Serial.println(CapDAC,BIN);
  
  CapDAC = CapDAC <<5;  // shift the set value four to the left to allow for 0s in bits 0:4
 
  Serial.println(CapDAC,BIN);
  
  ConfigRegLow  =  lowByte(CapDAC);    
  ConfigRegHigh =  highByte(CapDAC);   

  //Combine the CapDAC offset with the channal assingment for the capacitance measurment channel using a bitwise OR statment
   Serial.println("ConfigRegLow  ");
   Serial.println(ConfigRegLow,BIN);
   
   Serial.println("ConfigRegHigh  ");
   Serial.println(ConfigRegHigh,BIN);
 
  
  Ch1Byte1 = Ch1Byte1 | ConfigRegHigh;
  Ch2Byte1 = Ch2Byte1 | ConfigRegHigh;
  Ch3Byte1 = Ch3Byte1 | ConfigRegHigh;
  Ch4Byte1 = Ch4Byte1 | ConfigRegHigh;
  
 
   Serial.println(Ch1Byte1,BIN);
   Serial.println(Ch2Byte1,BIN);
   Serial.println(Ch3Byte1,BIN);
   Serial.println(Ch4Byte1,BIN);
  
  Ch1Byte2=ConfigRegLow;
  Ch2Byte2=ConfigRegLow;
  Ch3Byte2=ConfigRegLow;
  Ch4Byte2=ConfigRegLow;
  
}



// All this looping is just changing my capdac

void loop()
{

delay (100);

    num1=0;


      unsigned long Data1,Data2,Data3,Data4;
      
      
      Config(Ch1Register,Ch1Byte1,Ch1Byte2);
      Config(FDC_CONFIG,RegisterValue1,RegisterValueCh1); 
      delay(10);
      Data1=getdata(0x00);
      
      Config(Ch2Register,Ch2Byte1,Ch2Byte2);
      Config(FDC_CONFIG,RegisterValue1,RegisterValueCh2);
      delay(10);
      Data2=getdata(0x02);
 
 
      Config(Ch3Register,Ch3Byte1,Ch3Byte2);
      Config(FDC_CONFIG,RegisterValue1,RegisterValueCh3);
      delay(10);
      Data3=getdata(0x04);
 
 
      Config(Ch4Register,Ch4Byte1,Ch4Byte2);
      Config(FDC_CONFIG,RegisterValue1,RegisterValueCh4);
      delay(10);
      Data4=getdata(0x06);
      
      
      
      
      Data1=Data1/256;
      Data2=Data2/256;
      Data3=Data3/256;
      Data4=Data4/256;
      
      
      
      //Serial.print("Messung: ");
      Serial.print(Data1,DEC);
      Serial.print(" , ");
      //Serial.print(Data2,DEC);
      //Serial.print(" , ");
      //Serial.print(Data3,DEC);
      //Serial.print(" , ");
      //Serial.println(Data4,DEC);
      //Serial.println("11111111111111111111111111111111111111");


 

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
