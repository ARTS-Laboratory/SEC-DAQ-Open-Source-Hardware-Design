#include <Wire.h>
#include <SPI.h>


// these two functions are for spliting high and low bits
#define lowByte(w) ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))



// Define varaibles for the code

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
int CapOffset = 80; // Cap offset in Pf. Max of 96 pF.   //////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////


// set pin 10 as the slave select for the digital pot:
const int slaveSelectPin = 10;

/*
  * CS - to digital pin 10  (SS pin)
  * SDI - to digital pin 11 (MOSI pin)
  * CLK - to digital pin 13 (SCK pin)
*/

int analogPin = 0;      // analog in sensor 1 connected to analog pin 0 (temp pin)




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


// Varaibles used is setting the cap offset to the measurment registers
int CapDAC;  
int ConfigRegLow;
int ConfigRegHigh;

// Varaibles used to set the bound of the capacitance to ditigal converotor.  + or - 16 pf.  The chip says its 15, but I have found it to be 16.
int MapLow;
int MapHigh;

// Varaibles used to set the analog output of the DAC. 
int AnalogOutCh1;
int AnalogOutCh2;
int AnalogOutCh3;
int AnalogOutCh4;

// set the delay time.  Needs to be at least 1 milli.
int del=1;


void setup()
{
  
  // open the serial port
  Serial.begin(9600);
  
  // initialize the wire libary
  Wire.begin(); //Join the bus as master. 
  
 // Wire.setClock(400000); //Optional - set I2C SCL to High Speed Mode of 400kHz
  
  
  // initialize SPI:
  SPI.begin();
  
  // set the slaveSelectPin as an output:
  pinMode(slaveSelectPin, OUTPUT);

  // Set the CapDAC amount in the Measurement Configuration Registers
  if (CapOffset != 0)
    {
     CapDAC = CapOffset/3.125; // convert the pF value to the interger for a single sided capacitance measurment
     CapDAC = CapDAC <<5;  // shift the set value four to the left to allow for 0s in bits 0:4
   
     // split the capDAC value into low and high bite, to feen into the registor values
     ConfigRegLow  =  lowByte(CapDAC);    
     ConfigRegHigh =  highByte(CapDAC);   

     //Combine the CapDAC offset with the channal assingment for the capacitance measurment channel using a bitwise OR statment
     Ch1Byte1 = Ch1Byte1 | ConfigRegHigh;
     Ch2Byte1 = Ch2Byte1 | ConfigRegHigh;
     Ch3Byte1 = Ch3Byte1 | ConfigRegHigh;
     Ch4Byte1 = Ch4Byte1 | ConfigRegHigh;
  
     // set the low bit to the channel byte 2.  The channels are seperate varaibles incase I ever want a diffrent capdac for each channel
     Ch1Byte2=ConfigRegLow;
     Ch2Byte2=ConfigRegLow;
     Ch3Byte2=ConfigRegLow;
     Ch4Byte2=ConfigRegLow;
  
    // calulate the high and low capacitance bounds used later in mapping to the DAC
    MapLow  = CapOffset - 16;  
    MapHigh = CapOffset + 16;  
    }
  else 
  {
   MapLow  = 0;  
   MapHigh = 16;   
  }
  

}  // end setup





void loop()
{
  
  
  unsigned long time1,time2,time3,time4,time5;
  time1 = millis();
  
  
  delay (del);

  //num1=0;

  /*
  This section sets up, and triggers a communiction with the FDC1004 for every loop of the code. The data varaibles are rewritten
  every time the code is looped.
  
  The code returns a value from each of the 4 channels as Data1 - Data4.  The value is from the offset of the CDAC + or - 2^23.  
  
  
  */
      long Data1,Data2,Data3,Data4;
      
      
      Config(Ch1Register,Ch1Byte1,Ch1Byte2);
      Config(FDC_CONFIG,RegisterValue1,RegisterValueCh1); 
      delay(del);
      Data1=getdata(0x00);
      time2 = millis();
      
      Config(Ch2Register,Ch2Byte1,Ch2Byte2);
      Config(FDC_CONFIG,RegisterValue1,RegisterValueCh2);
      delay(del);
      Data2=getdata(0x02);
 
 
      Config(Ch3Register,Ch3Byte1,Ch3Byte2);
      Config(FDC_CONFIG,RegisterValue1,RegisterValueCh3);
      delay(del);
      Data3=getdata(0x04);
 
 
      Config(Ch4Register,Ch4Byte1,Ch4Byte2);
      Config(FDC_CONFIG,RegisterValue1,RegisterValueCh4);
      delay(del);
      Data4=getdata(0x06);
      
      
      
      // The sensor data is shiffted 8 bits to the right to remove the 8 bits of zeros.           
      Data1 = Data1 >> 8;
      Data2 = Data2 >> 8;
      Data3 = Data3 >> 8;
      Data4 = Data4 >> 8;
      
      
      
      /*
      //Serial.print("data : ");
      Serial.print(Data1,BIN);
      Serial.print(" , ");
      Serial.print(Data2,DEC);
      Serial.print(" , ");
      Serial.print(Data3,DEC);
      Serial.print(" , ");
      Serial.println(Data4,DEC);
      Serial.println("11111111111111111111111111111111111111");
      */
      
      
      /*
      Convert the digital data into the capacatiance data.  This part could be removed to improve the speed of the code
      
      To remove, simply map the long form data from 0 to the top bit of the DAC. 
      
      */
      
      float CapCh1,CapCh2,CapCh3,CapCh4;
      
      CapCh1 = (((float) Data1) / 524288 ) + CapOffset;   // changes data to capacitance.  could remove to speed up code.
      CapCh2 = (((float) Data2) / 524288 ) + CapOffset;   // changes data to capacitance.  could remove to speed up code.
      CapCh3 = (((float) Data3) / 524288 ) + CapOffset;   // changes data to capacitance.  could remove to speed up code.
      CapCh4 = (((float) Data4) / 524288 ) + CapOffset;   // changes data to capacitance.  could remove to speed up code.
      

      /*

      //Serial.print("Cap data : ");
      Serial.print(CapCh1,DEC);
      Serial.print(" , ");
      Serial.print(CapCh2,DEC);
      Serial.print(" , ");
      Serial.print(CapCh3,DEC);
      Serial.print(" , ");
      Serial.println(CapCh4,DEC);
      //Serial.println("11111111111111111111111111111111111111");

      */
      time3 = millis();
      
      /*
      Map the capacitance onto the range of the digital to analog convertor. e.g. a 10 bit convertor maps to 1023
      
      */
      
      AnalogOutCh1 = map(CapCh1, MapLow, MapHigh, 0, 1023);
      if (AnalogOutCh1 > 1023)
           {
            AnalogOutCh1 = 1023;
           }
       AnalogOutCh2 = map(CapCh2, MapLow, MapHigh, 0, 1023);
      if (AnalogOutCh1 > 1023)
           {
            AnalogOutCh2 = 1023;
           }
       AnalogOutCh3 = map(CapCh3, MapLow, MapHigh, 0, 1023);
      if (AnalogOutCh3 > 1023)
           {
            AnalogOutCh3 = 1023;
           }
       AnalogOutCh4 = map(CapCh4, MapLow, MapHigh, 0, 1023);
      if (AnalogOutCh4 > 1023)
           {
            AnalogOutCh4 = 1023;
           }     
      
      /*      

      //Serial.print("Analog Out Data : ");
      Serial.print(AnalogOutCh1,DEC);
      Serial.print(" , ");
      Serial.print(AnalogOutCh2,DEC);
      Serial.print(" , ");
      Serial.print(AnalogOutCh3,DEC);
      Serial.print(" , ");
      Serial.println(AnalogOutCh4,DEC);
      //Serial.println("11111111111111111111111111111111111111");

      */    
      time4 = millis();
      // Send the anlog out daat to data prep.  This adds zeros to the 16 byte word so it can be passed out to the DAC correctly.
     
     int newd = dataPrep(AnalogOutCh1);

     byte  MSB = highByte(newd);   //mask LSB, shift 8 bits to the right
     byte  LSB = lowByte(newd);             //mask MSB, no need to shift

      // shift the value out to the DAC via SPI here. This only does one channel for mow. 
     
      digitalWrite(slaveSelectPin, LOW);
      SPI.transfer ( MSB );
      SPI.transfer ( LSB );
      digitalWrite(slaveSelectPin, HIGH);

      int val = analogRead(analogPin);    // read the input pin
      float val2 = (float (val)/1023)*5;
     
      //Serial.print ("voltage read   ");
      //Serial.println(val2);             // reading back the voltage sent to the dAC
     
     time5 = millis();
     
     
     
     
      Serial.print(time1,DEC);
      Serial.print(" , ");
      Serial.print(time2,DEC);
      Serial.print(" , ");
      Serial.print(time3,DEC);
      Serial.print(" , ");
      Serial.print(time4,DEC);
      Serial.print(" , ");
      Serial.print(time5,DEC);
      Serial.print(" , ");
      Serial.println(val2);
}





int dataPrep(int data){
    int a = data;
    int b = a << 2;   
    int mating_v = 28672;
    int c = b | mating_v;  
    return(c);
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
