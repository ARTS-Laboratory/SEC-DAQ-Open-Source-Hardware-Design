// Ausitn Downey
// Jan 16 - 2015
// Arduino IDE 1.6.6

#include <Wire.h>
#include <SPI.h>
#include <SD.h>

int CapOffset = 81;     // Cap offset in Pf. Max of 96 pF.                  //////////
int SampleSpeed = 400;  // set to 100, 200 or 400 S/s                       //////////
int DynamicRange = 30;  //  Set the dynamic sensor range about the DAC      //////////
const int CS1 = 10;
int FDC=B1010000; 
byte FDC_CONFIG = 0x0C;
byte RegisterValue1 = B00001100;    // 100 S/s = B00000100; 200 S/s = B00001000; 400 S/s = B00001100;  
byte RegisterValueCh1=B10000000;
byte RegisterValueCh2=B01000000;
byte RegisterValueCh3=B00100000;
byte RegisterValueCh4=B00010000;
byte Ch1Register=0x08;
byte Ch1Byte1=B00010000;
byte Ch1Byte2=B00000000;
byte Ch2Register=0x09;
byte Ch2Byte1=B00110000;
byte Ch2Byte2=B00000000;
byte Ch3Register=0x0A;
byte Ch3Byte1=B01010000;
byte Ch3Byte2=B00000000;
byte Ch4Register=0x0B;
byte Ch4Byte1=B01110000;  
byte Ch4Byte2=B00000000;
int CapDAC;  
int ConfigRegLow;
int ConfigRegHigh;
int MapLow;
int MapHigh;
long DyRange;
int del=5;
int FDCWait = 4;
unsigned long SampleTime; // used to obtain a time samp.  millis() will return a value within 2% of the correct time, and will overflow after 50 days.
const int chipSelect = 4;
String filename2;



void setup()
{

  Serial.begin(9600);
  Wire.begin(); //Join the bus as master. 
  Wire.setClock(400000L); //Optional - set I2C SCL to High Speed Mode of 400kHz
  SPI.begin();
  pinMode(CS1, OUTPUT);
  digitalWrite(CS1, HIGH);  // set the slave select pin high, as low is for the SPI output. 
  delay(1);
  digitalWrite(9, HIGH);   // set the LDAC high for normal operations. (This line could be removed if the hardware is updated to remove the LDAC.
  float DyRatio = (DynamicRange/2);       // calualte the ratio of the dynamic range in two steps.
        DyRatio = DyRatio/30;
  float DyRangef = DyRatio * 16777215;    // Mulltiple the range by the total range of a 24 bit unsigned number
  DyRange = (long) DyRangef;              // convert to long to truncate the float number. 
  if(SampleSpeed == 400)
  {
    RegisterValue1 = B00001100;  // 100 S/s = B00000100; 200 S/s = B00001000; 400 S/s = B00001100;  
  }
  if(SampleSpeed == 200)
  {
    RegisterValue1 = B00001000;
  }
  if(SampleSpeed == 100)
  {
    RegisterValue1 = B00000100;
  }
  if (CapOffset != 0)    // check if the CapOffset is greater than zero, and set the correct regesiters.
    {
     CapDAC = CapOffset/3.125; // convert the pF value to the interger for a single sided capacitance measurment
     CapDAC = CapDAC <<5;  // shift the set value four to the left to allow for 0s in bits 0:4
     ConfigRegLow  =  lowByte(CapDAC);    
     ConfigRegHigh =  highByte(CapDAC);   
     Ch1Byte1 = Ch1Byte1 | ConfigRegHigh;
     Ch2Byte1 = Ch2Byte1 | ConfigRegHigh;
     Ch3Byte1 = Ch3Byte1 | ConfigRegHigh;
     Ch4Byte1 = Ch4Byte1 | ConfigRegHigh;
     Ch1Byte2=ConfigRegLow;
     Ch2Byte2=ConfigRegLow;
     Ch3Byte2=ConfigRegLow;
     Ch4Byte2=ConfigRegLow;
    MapLow  = CapOffset - 16;  
    MapHigh = CapOffset + 16;  
    }
  else   // Set the CapDAc range to 0 - 16 if the capdac was set to zero 
  {
   MapLow  = 0;  
   MapHigh = 16;   
  }
Config(Ch1Register,Ch1Byte1,Ch1Byte2);
Config(Ch2Register,Ch2Byte1,Ch2Byte2);
Config(Ch3Register,Ch3Byte1,Ch3Byte2);
Config(Ch4Register,Ch4Byte1,Ch4Byte2);

Serial.println("hello world");




      long Data1,Data2,Data3,Data4;   //If doing math with integers, at least one of the numbers must be followed by an L, forcing it to be a long. See the Integer Constants page for details.
      
      //Config(Ch1Register,Ch1Byte1,Ch1Byte2);
      Config(FDC_CONFIG,RegisterValue1,RegisterValueCh1); 
      waitData(1);  // pass the channel (1-4) to the wait command  Could use a large delay but this helps to speed up the code//
      //delay(FDCWait);
      Data1=getdata(0x00);

      Serial.println(Data1);

float DataArray[251];

int i;
//for(i = 0; i <= 250; i++){
//DataArray[i] = i; 
//Serial.println(i);
//}

Serial.print("Data array");

//float temp;
//for(i = 0; i <= 250; i++){
//temp = DataArray[i]; 
//Serial.println(temp);
//}





//for (i = 0; i < 5; i = i++) {
//  Serial.println(DataArray[i]);
//}



float data[250];


i;
for (i = 0; i < 250; i = i + 1) {
  data[i] = i;
}

Serial.println(data[i]);

//i;
//for (i = 0; i < 250; i = i + 1) {
//  Serial.println(data[i]);
//}














}  // end setup


void loop()
{

unsigned long timeArray[255];
float CH0Array[255];
float CH1Array[255];
float CH2Array[255];
float CH3Array[255];

int i;
for(i = 0; i <= 250; i + 1){

//Serial.println(i);
   /*
      long Data1,Data2,Data3,Data4;   //If doing math with integers, at least one of the numbers must be followed by an L, forcing it to be a long. See the Integer Constants page for details.
      
      //Config(Ch1Register,Ch1Byte1,Ch1Byte2);
      Config(FDC_CONFIG,RegisterValue1,RegisterValueCh1); 
      waitData(1);  // pass the channel (1-4) to the wait command  Could use a large delay but this helps to speed up the code//
      //delay(FDCWait);
      Data1=getdata(0x00);

      Serial.println(Data1);

      /*
      //Config(Ch2Register,Ch2Byte1,Ch2Byte2);
      Config(FDC_CONFIG,RegisterValue1,RegisterValueCh2);
      waitData(2);  // pass the channel (1-4) to the wait command  Could use 
      //delay(FDCWait);
      Data2=getdata(0x02);
 
 
      //Config(Ch3Register,Ch3Byte1,Ch3Byte2);
      Config(FDC_CONFIG,RegisterValue1,RegisterValueCh3);
      waitData(3);  // pass the channel (1-4) to the wait command  Could use 
      //delay(FDCWait);
      Data3=getdata(0x04);
 
 
      //Config(Ch4Register,Ch4Byte1,Ch4Byte2);
      Config(FDC_CONFIG,RegisterValue1,RegisterValueCh4);
      waitData(4);  // pass the channel (1-4) to the wait command  Could use 
      //delay(FDCWait);
      Data4=getdata(0x06);

      
      //time2 = millis();
      
      // Change the data taken from the FDC from a signed 24 bit number with bits[0-7] = 0 to a unsigned long without the byte of zeros.
      long tempdata1 = SetZero(Data1);
      long tempdata2 = SetZero(Data2);
      long tempdata3 = SetZero(Data3);
      long tempdata4 = SetZero(Data4);
      //Serial.println(tempdata2,DEC);

      //  Print to the serial port the binary number form 0 to 2^24,  could be commented out a higher sampling speed is wanted.  

      
      float CapCh1,CapCh2,CapCh3,CapCh4;
      
      CapCh1 = (((float) tempdata1) / 524288 ) + CapOffset-16;   // changes data to capacitance.  could remove to speed up code.
      CapCh2 = (((float) tempdata2) / 524288 ) + CapOffset-16;   
      CapCh3 = (((float) tempdata3) / 524288 ) + CapOffset-16;   
      CapCh4 = (((float) tempdata4) / 524288 ) + CapOffset-16;   

      //SampleTime = millis(); // gets the arduino time in microseconds.  2% error in time, and overflow in 50 days
      SampleTime = micros();   // gets the arduino time in microseconds. 
      //SampleTime = SampleTime/1000;

 timeArray[i] = SampleTime;
 CH0Array[i] = CapCh1;
 CH1Array[i] = CapCh2;
 CH2Array[i] = CapCh3;
 CH3Array[i] = CapCh4;

*/
//Serial.println(CH0Array[i-1]);

//exit(0);

} 


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Send the data to the serial port

/*
      Serial.print(SampleTime,DEC);
      //Serial.print(" , ");
      //Serial.print(" ");
      Serial.print("\t"); 
      Serial.print(CapCh1,DEC);
      //Serial.print(" , ");
      //Serial.print(" ");
      Serial.print("\t"); 
      Serial.print(CapCh2,DEC);
      //Serial.print(" , ");
      //Serial.print(" ");
      Serial.print("\t"); 
      Serial.print(CapCh3,DEC);
      //Serial.print(" , ");
      //Serial.print(" ");
      Serial.print("\t"); 
      Serial.println(CapCh4,DEC);
*/

  
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Libary Functions

word dataPrepA(int data){
   unsigned int c;
   unsigned int a = data;
   c = a | 28672; 
   return(c);
}


word dataPrepB(int data){
  //Serial.println(data,BIN);
   unsigned int c;
   unsigned int a = data;
   unsigned int mask;
   mask = 61440;
   c = a | mask; 
   //Serial.println(c,BIN);
   return(c);
}


unsigned int getinfo(byte pointer)
{
  byte a,b;
  unsigned int c;
  Wire.beginTransmission(FDC);
  Wire.write(pointer);
  Wire.endTransmission(false);  // false for repeated start
  //Wire.onReceive(receiveEvent);
  //while (Wire.requestFrom(FDC,2) <= 1);
  //delay(del);
  Wire.requestFrom(FDC,2);
  //int tt = Wire.available();
  //Serial.print(tt);
  a=Wire.read();
  b=Wire.read();
  //delay(del);
  c=word(a,b);       // high, low  line added by Koepel.  comment out my code
  //delayMicroseconds(1000); 
  //c=a;
  //c=c<<8;
  //c|=b;
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
  //delay(del);
  //delayMicroseconds(800);  //700 does not work, 800 seems to work OK for one channal DAC
  b = getinfo(pointer+1);
  //delay(del);
  //delayMicroseconds(800);
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
  //delay (del);
  //delayMicroseconds(800);
}

//   for use wtih Wire.onReceive(receiveEvent);   from http://wiring.org.co/reference/libraries/Wire/Wire_onReceive_.html

void receiveEvent(int howMany)
{
  while(1 < Wire.available())     // loop through all but the last
  {
  char c = Wire.read();       // read byte as a character
  //Serial.print(c);  // print the character
  }
  // receive byte as an integer
  int x = Wire.read();    
  //Serial.println(x);  // print the integer
}

/*
from the FDC1004 data sheet

8.5.3.3 Wait for Measurement Completion 

Wait for the triggered measurements to complete. When the measurements are complete, the corresponding
DONE_x field (Register 0x0C:bits[3:0]) will be set to 1.
*/

void waitData(int ChReady)
{
  byte a,b,d;
  int c = 0;
  int ReadyBit = 4 - ChReady;  // converts the channel number (1-4) to the correct value for the corresponding DONE_x field (Register 0x0C:bits[3:0])
      Wire.beginTransmission(FDC);
      Wire.write(FDC_CONFIG);
      Wire.endTransmission(false);  // false for repeated start
   while(c==0)
      {
      Wire.requestFrom(FDC,2);
      a=Wire.read();
      b=Wire.read();
      c = bitRead(b,ReadyBit);
      //Serial.print("c  -  ");
      //Serial.println(c);
      }
}

unsigned long SetZero(long a)
{
long b,c,e,f;
unsigned long z;
 //a  = -2147483392;
 //a =    2147483392;
 //c  = -2147483648;
 z = 0x7FFFFF;
 //d = a & b;

if (a<0){
  e = (unsigned long)a >> 8;
  f = e&z;
}
else{
  e = (unsigned long)a >> 8;
  f = e + 8388608;

}
return(f);
}

long RescaleDAC(long DyRange,long tempdata2){

      long ShiftLow = 8388608 - DyRange;
      long tempdy = DyRange*2;
      if(tempdata2 - ShiftLow < 0){
       tempdata2 = 0;
      }
      else{
       tempdata2 = tempdata2 - ShiftLow;
      }
      if(tempdata2 > DyRange*2){
       tempdata2 = DyRange*2;
      }
      return(tempdata2);
}




























