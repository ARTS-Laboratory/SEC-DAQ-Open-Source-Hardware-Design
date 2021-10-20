// Ausitn Downey
// Jan 16 - 2015
// Arduino IDE 1.6.6

#include <Wire.h>
#include <SPI.h>


/*
 * FDC 1004 CDC shield that is used to measure the capacitance of the SEC sensors.  
 * 
 * The shield reads the CAP with the CDC and then uses some bit operators to “compress” the dynamic range of the 
 * CDC (24 bit) onto the 16 bit DAC.  This is than passed to the cRIO and read at 15 bits (as we only use 0 – 5V).  
 * The best way to do this would be to find a way to digitally write to the cRIO
 * 
 * The best way to use this code is to set the CapOffset to as close to the steady state capacitance for the sensor 
 * and then set the dynamic range as small as possible (Max 30) as to capture all of the dynamic sampling on the DAC.  
 * The Dynamic range may not matter if we find a way to shift the FDC results out digitally.
 * 
 * Set the speed in SampleSpeed, know that 100Hz / 4 Channels will read each sensor at 25Hz.  Add to this the time 
 * the microcontroller needs the final sample frequency is around .. 13 Hz.  
 * 
 * 13 Hz --  100S/s Printing cap and time stamp to Serial at 9600 baud.
 * 20.5 Hz --  100S/s Printing cap and time stamp to Serial at 250000 baud.
 * 22.7 Hz --  100S/s Printing time stamp to Serial at 250000 baud.
 * 
 * The resolution of the FDC1004 depends on the sample speed selected.  Use lower speeds for higher resolution.  
 * Only the FDC sample speed (100,200,400) should effect the FDC's resolution. 
 * 
 * 
 * To use this code with the Arduinos serial port only download CoolTerm form  
 * http://freeware.the-meiers.org/
 * 
 * Click the connect button on the top.
 * Change the Baud rate to what you select in Coolterm and turn on the time stamp selection from one of the 
 * drop down menu connection > options. 
 * 
 * connection > options > receive  click the add timestamps to received data
 * 
 * Collect data under connection > capture to text file  
 * 
 * HAVE FUN!!!!!
 */



// Define varaibles for the code

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
int CapOffset = 69;     // Cap offset in Pf. Max of 96 pF.                  //////////
int SampleSpeed = 100;  // set to 100, 200 or 400 S/s                       //////////
int DynamicRange = 10;  //  Set the dynamic sensor range about the DAC      //////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////


// set pin 10 as the slave select for the MAX 5134 DAC:
const int CS1 = 10;


/*
  * CS - to digital pin 10  (SS pin)
  * SDI - to digital pin 11 (MOSI pin)
  * CLK - to digital pin 13 (SCK pin)
*/


// I2C buss address for the FDC1004 used
int FDC=B1010000; 

// Address used in the FDC1004 for the FDC Configuration Register, used for triggering and setting the measurment channel
byte FDC_CONFIG = 0x0C;

// Regesiter values for the Capacitance to Digital Configuration regester, located at 0x0C
byte RegisterValue1 = B00001100;    // 100 S/s = B00000100; 200 S/s = B00001000; 400 S/s = B00001100;  
byte RegisterValueCh1=B10000000;
byte RegisterValueCh2=B01000000;
byte RegisterValueCh3=B00100000;
byte RegisterValueCh4=B00010000;

// Sets the Measurement Configuration Registers values to the correct values.  This does channel 1.
//  Sets all channels to positive input channel to the correct input line, and sets the negatice input channel capacitive to digital converter to CAPDAC.
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
byte Ch4Byte1=B01110000;  
byte Ch4Byte2=B00000000;

// Varaibles used is setting the cap offset to the measurment registers
int CapDAC;  
int ConfigRegLow;
int ConfigRegHigh;

// Varaibles used to set the bound of the capacitance to ditigal converotor.  + or - 16 pf.  The data sheet says its 15, but I have found it to be 16 pF.
int MapLow;
int MapHigh;

// varaibles used for setting the dynamic software range
long DyRange;

// set the delay time.  These may not be needed now that the FDC waits for the processes to complete. 
int del=5;
int FDCWait = 4;

// setting a time stamp
int time6;


void setup()
{
  
  // open the serial port at the correct baud rate. 
  //Serial.begin(9600);
  //Serial.begin(234000);
   Serial.begin(250000);
  
  // initialize the wire libary
  Wire.begin(); //Join the bus as master. 
  
  Wire.setClock(400000L); //Optional - set I2C SCL to High Speed Mode of 400kHz
  
  // initialize SPI:
  SPI.begin();
  
  // set the slaveSelectPin for the DAC as an output, pull high. :
  pinMode(CS1, OUTPUT);
  digitalWrite(CS1, HIGH);  // set the slave select pin high, as low is for the SPI output. 
  delay(1);

  digitalWrite(9, HIGH);   // set the LDAC high for normal operations. (This line could be removed if the hardware is updated to remove the LDAC.


  // Calulate the dynamic range of the sensors.   
  float DyRatio = (DynamicRange/2);       // calualte the ratio of the dynamic range in two steps.
        DyRatio = DyRatio/30;
  float DyRangef = DyRatio * 16777215;    // Mulltiple the range by the total range of a 24 bit unsigned number
  DyRange = (long) DyRangef;              // convert to long to truncate the float number. 

 
  // set the sample speed in RegisterValue1 to the selected sample speed selected at the top of the code
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

 
  // Set the CapDAC amount that will be written to the Measurement Configuration Registers
  if (CapOffset != 0)    // check if the CapOffset is greater than zero, and set the correct regesiters.
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
  else   // Set the CapDAc range to 0 - 16 if the capdac was set to zero 
  {
   MapLow  = 0;  
   MapHigh = 16;   
  }

// write the channel config values ot the channel spefic regieters. i.e. (0x08, 0x09, 0x0A, 0x0B)
Config(Ch1Register,Ch1Byte1,Ch1Byte2);
Config(Ch2Register,Ch2Byte1,Ch2Byte2);
Config(Ch3Register,Ch3Byte1,Ch3Byte2);
Config(Ch4Register,Ch4Byte1,Ch4Byte2);
  
}  // end setup


void loop()
{
    
  //unsigned long time1,time2,time3,time4,time5;  
  //time1 = millis();
  
  /*
  This section sets up, and triggers a communiction with the FDC1004 for every loop of the code. The data varaibles are rewritten
  every time the code is looped.
  
  The code returns a value from each of the 4 channels as Data1 - Data4.  The value is from the offset of the CDAC + or - 2^23.  
    
  */
      long Data1,Data2,Data3,Data4;   //If doing math with integers, at least one of the numbers must be followed by an L, forcing it to be a long. See the Integer Constants page for details.
      
      //Config(Ch1Register,Ch1Byte1,Ch1Byte2);
      Config(FDC_CONFIG,RegisterValue1,RegisterValueCh1); 
      waitData(1);  // pass the channel (1-4) to the wait command  Could use a large delay but this helps to speed up the code//
      //delay(FDCWait);
      Data1=getdata(0x00);
      
      
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
      /*
      Serial.print(tempdata1,DEC);
      Serial.print(" , ");
      Serial.print(tempdata2,DEC);
      Serial.print(" , ");
      Serial.print(tempdata3,DEC);
      Serial.print(" , ");
      Serial.println(tempdata4,DEC);


     /*          
      *   Convert the digital data into the capacatiance data.  This part could be removed to improve the speed of the code 
      *   To remove, simply map the unsigned long form data from 0 to the top bit of the DAC.     
      *   
            *   To do this I do it a little different than the FDC1004 data sheet.  I used the unsigned long, divide my given 
            *   constant and add the CapOffset + the center line for the new number. i.e. 2^23/2^19 = 16.
      */

      
      float CapCh1,CapCh2,CapCh3,CapCh4;
      
      CapCh1 = (((float) tempdata1) / 524288 ) + CapOffset-16;   // changes data to capacitance.  could remove to speed up code.
      CapCh2 = (((float) tempdata2) / 524288 ) + CapOffset-16;   
      CapCh3 = (((float) tempdata3) / 524288 ) + CapOffset-16;   
      CapCh4 = (((float) tempdata4) / 524288 ) + CapOffset-16;   
      
      Serial.print(CapCh1,DEC);
      Serial.print(" , ");
      Serial.print(CapCh2,DEC);
      Serial.print(" , ");
      Serial.print(CapCh3,DEC);
      Serial.print(" , ");
      Serial.println(CapCh4,DEC);

      
   //   time3 = millis();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 // Write the cap data to the DAC

/*
 *  Map the capacitance onto the range of the digital to analog convertor. e.g. a 16 bit convertor maps to 65535.
 *  
 *  Rescale DAC also find over and under limits and resets the numbers to 0 and full (dynamic range value) .
 *  Remember here the numbers are binary values of the 24 or 16 bit range
 */
      
      tempdata1 =  RescaleDAC(DyRange,tempdata1);
      tempdata2 =  RescaleDAC(DyRange,tempdata2);
      tempdata3 =  RescaleDAC(DyRange,tempdata3);
      tempdata4 =  RescaleDAC(DyRange,tempdata4);


//  Find the precent of the CDC measurment of the dynamic range and map that onto the 16 bit DAC

      float precent = float(tempdata1)/(2*DyRange);
      unsigned int AnalogOutCh1 = precent * 65535;

      precent = float(tempdata2)/(2*DyRange);   
      unsigned int AnalogOutCh2 = precent * 65535;

      precent = float(tempdata3)/(2*DyRange);
      unsigned int AnalogOutCh3 = precent * 65535;

      precent = float(tempdata4)/(2*DyRange);
      unsigned int AnalogOutCh4 = precent * 65535;

      //Serial.println(AnalogOutCh2,DEC);
      
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


      // Build the control bits for the DAC, these are used to tell the DAC which regieter to write to.
      byte ControlBitsCh1=B00110001;
      byte ControlBitsCh2=B00110010;
      byte ControlBitsCh3=B00110100;
      byte ControlBitsCh4=B00111000;
         
//      time4 = millis();


      // Build the data bits for the DAC, these will be shifted out after the control bits. 
      byte  Ch1MSB = highByte(AnalogOutCh1);   //mask LSB, shift 8 bits to the right
      byte  Ch1LSB = lowByte(AnalogOutCh1);    //mask MSB, no need to shift
      byte  Ch2MSB = highByte(AnalogOutCh2);   
      byte  Ch2LSB = lowByte(AnalogOutCh2);   
      byte  Ch3MSB = highByte(AnalogOutCh3);  
      byte  Ch3LSB = lowByte(AnalogOutCh3);   
      byte  Ch4MSB = highByte(AnalogOutCh4);   
      byte  Ch4LSB = lowByte(AnalogOutCh4);   


      // shift the value out to the DAC via SPI here. This only does one channel for mow. 
      digitalWrite(CS1, LOW);
      SPI.transfer ( ControlBitsCh1 );
      SPI.transfer ( Ch1MSB );
      SPI.transfer ( Ch1LSB );
      digitalWrite(CS1, HIGH);

      //delay(10);
      digitalWrite(CS1, LOW);
      SPI.transfer ( ControlBitsCh2 );
      SPI.transfer ( Ch2MSB );
      SPI.transfer ( Ch2LSB );
      digitalWrite(CS1, HIGH);

      //delay(10);
      digitalWrite(CS1, LOW);
      SPI.transfer ( ControlBitsCh3 );
      SPI.transfer ( Ch3MSB );
      SPI.transfer ( Ch3LSB );
      digitalWrite(CS1, HIGH);

      //delay(10);
      digitalWrite(CS1, LOW);
      SPI.transfer ( ControlBitsCh4 );
      SPI.transfer ( Ch4MSB );
      SPI.transfer ( Ch4LSB );
      digitalWrite(CS1, HIGH);

     // Time stamp used for checking the speed.  commented out unless I need to check the speed.

     //time6 = millis();
     //Serial.println(time6);
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




























