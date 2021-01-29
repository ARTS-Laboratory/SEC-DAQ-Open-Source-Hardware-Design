// Ausitn Downey
// Feb. 6 - 2017
// Arduino IDE 1.8.1

/*
   FDC 1004 CDC DAQ measure the capacitance of the SEC sensors.
   This code is programed through the switches on the DAQ board
   Updated March 3rd, 2017
   Austin R.J. Downey
   HAVE FUN!!!!!
*/

#include <Wire.h>

// I2C buss address for the FDC1004 used
int FDC = B1010000;

// Address used in the FDC1004 for the FDC Configuration Register, used for triggering and setting the measurment channel
byte FDC_CONFIG = 0x0C;

// Varaibles used is setting the cap offset to the measurment registers
int CapDAC;
int ConfigRegLow;
int ConfigRegHigh;

// build a time stamp
unsigned long SampleTime; // used to obtain a time stamp.
float TimeStamp; // float version of the micro time stamp converted to seconds for the output

// define the digital input pin locations
int range_0 = 3;
int range_1 = 2;
int channel_0 = 5;
int channel_1 = 4;
int sample_0 = 7;
int sample_1 = 6;
int trigger = 8;

// define the variables for the input pins, start as zero
byte val_range = 0;
byte val_channel = 0;
byte val_sample = 0;
int val_catch = 0;

// define and set the offset and sample speed as default incase of a switch fault
int CapOffset = 25;
int SampleSpeed = 100;

// Regesiter values for the Capacitance to Digital Configuration regester, located at 0x0C,
byte RegisterValue1 = B00001100;    // 100 S/s = B00000100; 200 S/s = B00001000; 400 S/s = B00001100;
byte RegisterValueCh1 = B10000000;
byte RegisterValueCh2 = B01000000;
byte RegisterValueCh3 = B00100000;
byte RegisterValueCh4 = B00010000;

//  Sets all channels to positive input channel to the correct input line, and sets the negatice input channel capacitive
//  to digital converter to CAPDAC.
// Sets the Measurement Configuration Registers values to the correct values.  This does channel 1.
byte Ch1Register = 0x08;
byte Ch1Byte1 = B00010000;
byte Ch1Byte2 = B00000000;

// Sets the Measurement Configuration Registers values to the correct register.  This does channel 2.
byte Ch2Register = 0x09;
byte Ch2Byte1 = B00110000;
byte Ch2Byte2 = B00000000;

// Sets the Measurement Configuration Registers values to the correct register.  This does channel 3.
byte Ch3Register = 0x0A;
byte Ch3Byte1 = B01010000;
byte Ch3Byte2 = B00000000;

// Sets the Measurement Configuration Registers values to the correct register.  This does channel 4.
byte Ch4Register = 0x0B;
byte Ch4Byte1 = B01110000;
byte Ch4Byte2 = B00000000;

void setup()
{
  // open the serial port at the correct baud rate.
  Serial.begin(115200);
  Wire.begin(); //initialize the wire libary, Join the bus as master.
  Wire.setClock(400000L); //Optional - set I2C SCL to High Speed Mode of 400kHz.  Run baby run!!


  //sets all the digital pins as inputs and reads the values
  pinMode(range_0, INPUT);
  pinMode(range_1, INPUT);
  pinMode(channel_0, INPUT);
  pinMode(channel_1, INPUT);
  pinMode(sample_0, INPUT);
  pinMode(sample_1, INPUT);
  pinMode(trigger, INPUT);

  // Read the data inputs from the binary switches
  val_range = BinaryThing(range_0, range_1);
  val_channel = BinaryThing(channel_0, channel_1);
  val_sample = BinaryThing(sample_0, sample_1);
  delay(1); // for good measure


  // set the input offset
  if (val_range == 0)
  {
    CapOffset = 30;
  }
  else if (val_range == 1)
  {
    CapOffset = 50;
  }
  else if (val_range == 2)
  {
    CapOffset = 70;
  }
  else if (val_range == 3)
  {
    CapOffset = 90;
  }

  // set the sample speed
  if (val_sample == 0)
  {
    SampleSpeed = 100;
    RegisterValue1 = B00000100;  // 100 S/s = B00000100; 200 S/s = B00001000; 400 S/s = B00001100;
  }
  else if (val_sample == 1)
  {
    SampleSpeed = 200;
    RegisterValue1 = B00001000;
  }
  else if (val_sample == 2)
  {
    SampleSpeed = 400;
    RegisterValue1 = B00001100;
  }
  else if (val_sample == 3)
  {
    SampleSpeed = 400;
    RegisterValue1 = B00001100;
  }


  // set the capoffset
  CapDAC = CapOffset / 3.125; // convert the pF value to the interger for a single sided capacitance measurment
  CapDAC = CapDAC << 5; // shift the set value four to the left to allow for 0s in bits 0:4

  // split the capDAC value into low and high bite, to feen into the registor values
  ConfigRegLow  =  lowByte(CapDAC);
  ConfigRegHigh =  highByte(CapDAC);

  //Combine the CapDAC offset with the channal assingment for the capacitance measurment channel using a bitwise OR statment
  Ch1Byte1 = Ch1Byte1 | ConfigRegHigh;
  Ch2Byte1 = Ch2Byte1 | ConfigRegHigh;
  Ch3Byte1 = Ch3Byte1 | ConfigRegHigh;
  Ch4Byte1 = Ch4Byte1 | ConfigRegHigh;

  // set the low bit to the channel byte 2.  The channels are seperate varaibles incase I ever want a diffrent capdac for each channel
  Ch1Byte2 = ConfigRegLow;
  Ch2Byte2 = ConfigRegLow;
  Ch3Byte2 = ConfigRegLow;
  Ch4Byte2 = ConfigRegLow;

  // write the channel config values ot the channel spefic regieters. i.e. (0x08, 0x09, 0x0A, 0x0B)
  Config(Ch1Register, Ch1Byte1, Ch1Byte2);
  Config(Ch2Register, Ch2Byte1, Ch2Byte2);
  Config(Ch3Register, Ch3Byte1, Ch3Byte2);
  Config(Ch4Register, Ch4Byte1, Ch4Byte2);

}  // end setup

void loop()
{
  // initialize the variables for the loop code
  long Data1, Data2, Data3, Data4;
  float CapCh1, CapCh2, CapCh3, CapCh4;

  // check the trigger lacth and stopp till trigger catch is off
  val_catch = digitalRead(trigger);
  
  while (val_catch == 1)
  {
    delay(1);
    val_catch = digitalRead(trigger);
  }

  // 1 channel measurment
  if (val_channel == 0)
  {
    SampleTime = micros();   // gets the arduino time in microseconds.
    TimeStamp = (((float)SampleTime) / 1000000);

    //Configure the channel 2 sample
    Config(FDC_CONFIG, RegisterValue1, RegisterValueCh1);
    waitData(1);  // pass the channel (1-4) to the wait command  Could use a large delay but this helps to speed up the code//
    Data1 = getdata(0x00);

    // Change the data taken from the FDC from a signed 24 bit number with bits[0-7] = 0 to a unsigned long without the byte of zeros.
    long tempdata1 = SetZero(Data1);

    CapCh1 = (((float) tempdata1) / 524288 ) + CapOffset - 16; // changes data to capacitance.  could remove to speed up code.

    String dataOutput = String(String(TimeStamp,5) + "\t" + String(CapCh1,5));
    Serial.println(dataOutput);
  }

  // 2 channel measurment
  if (val_channel == 1)
  {
    SampleTime = micros();   // gets the arduino time in microseconds.
    TimeStamp = (((float)SampleTime) / 1000000);

    //Configure the channel 2 sample
    Config(FDC_CONFIG, RegisterValue1, RegisterValueCh1);
    waitData(1);  // pass the channel (1-4) to the wait command  Could use a large delay but this helps to speed up the code//
    Data1 = getdata(0x00);

    //Configure the channel 2 sample
    Config(FDC_CONFIG, RegisterValue1, RegisterValueCh2);
    waitData(2);  // pass the channel (1-4) to the wait command  Could use
    Data2 = getdata(0x02);

    // Change the data taken from the FDC from a signed 24 bit number with bits[0-7] = 0 to a unsigned long without the byte of zeros.
    long tempdata1 = SetZero(Data1);
    long tempdata2 = SetZero(Data2);

    CapCh1 = (((float) tempdata1) / 524288 ) + CapOffset - 16; // changes data to capacitance.  could remove to speed up code.
    CapCh2 = (((float) tempdata2) / 524288 ) + CapOffset - 16;

    String dataOutput = String(String(TimeStamp,5) + "\t" + String(CapCh1,5) + "\t" + String(CapCh2,5));
    Serial.println(dataOutput);
  }

  // 3 channel measurment
  if (val_channel == 2)
  {
    SampleTime = micros();   // gets the arduino time in microseconds.
    TimeStamp = (((float)SampleTime) / 1000000);

    //Configure the channel 2 sample
    Config(FDC_CONFIG, RegisterValue1, RegisterValueCh1);
    waitData(1);  // pass the channel (1-4) to the wait command  Could use a large delay but this helps to speed up the code//
    Data1 = getdata(0x00);

    //Configure the channel 2 sample
    Config(FDC_CONFIG, RegisterValue1, RegisterValueCh2);
    waitData(2);  // pass the channel (1-4) to the wait command  Could use
    Data2 = getdata(0x02);

    //Configure the channel 3 sample
    Config(FDC_CONFIG, RegisterValue1, RegisterValueCh3);
    waitData(3);  // pass the channel (1-4) to the wait command  Could use
    Data3 = getdata(0x04);

    // Change the data taken from the FDC from a signed 24 bit number with bits[0-7] = 0 to a unsigned long without the byte of zeros.
    long tempdata1 = SetZero(Data1);
    long tempdata2 = SetZero(Data2);
    long tempdata3 = SetZero(Data3);

    CapCh1 = (((float) tempdata1) / 524288 ) + CapOffset - 16; // changes data to capacitance.  could remove to speed up code.
    CapCh2 = (((float) tempdata2) / 524288 ) + CapOffset - 16;
    CapCh3 = (((float) tempdata3) / 524288 ) + CapOffset - 16;

    String dataOutput = String(String(TimeStamp,5) + "\t" + String(CapCh1,5) + "\t" + String(CapCh2,5) + "\t" + String(CapCh3,5));
    Serial.println(dataOutput);
    
  }

  // 4 channel measurment
  if (val_channel == 3)
  {
    SampleTime = micros();   // gets the arduino time in microseconds.
    TimeStamp = (((float)SampleTime) / 1000000);

    //Configure the channel 2 sample
    Config(FDC_CONFIG, RegisterValue1, RegisterValueCh1);
    waitData(1);  // pass the channel (1-4) to the wait command  Could use a large delay but this helps to speed up the code//
    Data1 = getdata(0x00);

    //Configure the channel 2 sample
    Config(FDC_CONFIG, RegisterValue1, RegisterValueCh2);
    waitData(2);  // pass the channel (1-4) to the wait command  Could use
    Data2 = getdata(0x02);

    //Configure the channel 3 sample
    Config(FDC_CONFIG, RegisterValue1, RegisterValueCh3);
    waitData(3);  // pass the channel (1-4) to the wait command  Could use
    Data3 = getdata(0x04);

    //Configure the channel 4 sample
    Config(FDC_CONFIG, RegisterValue1, RegisterValueCh4);
    waitData(4);  // pass the channel (1-4) to the wait command  Could use
    Data4 = getdata(0x06);

    // Change the data taken from the FDC from a signed 24 bit number with bits[0-7] = 0 to a unsigned long without the byte of zeros.
    long tempdata1 = SetZero(Data1);
    long tempdata2 = SetZero(Data2);
    long tempdata3 = SetZero(Data3);
    long tempdata4 = SetZero(Data4);

    CapCh1 = (((float) tempdata1) / 524288 ) + CapOffset - 16; // changes data to capacitance.  could remove to speed up code.
    CapCh2 = (((float) tempdata2) / 524288 ) + CapOffset - 16;
    CapCh3 = (((float) tempdata3) / 524288 ) + CapOffset - 16;
    CapCh4 = (((float) tempdata4) / 524288 ) + CapOffset - 16;

    String dataOutput = String(String(TimeStamp,5) + "\t" + String(CapCh1,5) + "\t" + String(CapCh2,5) + "\t" + String(CapCh3,5) + "\t" + String(CapCh4,5));
    Serial.println(dataOutput);
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Libary Functions

// This function is used for checking if data is in the registor
unsigned int getinfo(byte pointer)
{
  byte a, b;
  unsigned int c;
  Wire.beginTransmission(FDC);
  Wire.write(pointer);
  Wire.endTransmission(false);  // false for repeated start
  Wire.requestFrom(FDC, 2);
  a = Wire.read();
  b = Wire.read();
  c = word(a, b);    // high, low  line added by Koepel.  comment out my code
  return (c);
}

// This function is used for getting data from registor
unsigned long getdata(byte pointer)
{
  unsigned int a, b;
  unsigned long c = 0;
  a = getinfo(pointer);
  b = getinfo(pointer + 1);
  c = a;
  c = c << 16;
  c |= b;
  return (c);
}

// This function is used for configuring the registor for the next sample
void Config(byte addresse, byte val1, byte val2)
{
  Wire.beginTransmission(FDC);
  Wire.write(addresse);
  Wire.write(val1);
  Wire.write(val2);
  Wire.endTransmission();
}


/*
  from the FDC1004 data sheet

  8.5.3.3 Wait for Measurement Completion

  Wait for the triggered measurements to complete. When the measurements are complete, the corresponding
  DONE_x field (Register 0x0C:bits[3:0]) will be set to 1.
*/

// This function is to wait till the ch is ready to read
void waitData(int ChReady)
{
  byte a, b, d;
  int c = 0;
  int ReadyBit = 4 - ChReady;  // converts the channel number (1-4) to the correct value for the corresponding DONE_x field (Register 0x0C:bits[3:0])
  Wire.beginTransmission(FDC);
  Wire.write(FDC_CONFIG);
  Wire.endTransmission(false);  // false for repeated start
  while (c == 0)
  {
    Wire.requestFrom(FDC, 2);
    a = Wire.read();
    b = Wire.read();
    c = bitRead(b, ReadyBit);
  }
}

// This function changes the data taken from the FDC from a signed 24 bit number to a unsigned long without the leading zeros.
unsigned long SetZero(long a)
{
  long b, c, e, f;
  unsigned long z;
  z = 0x7FFFFF;

  if (a < 0) {
    e = (unsigned long)a >> 8;
    f = e & z;
  }
  else {
    e = (unsigned long)a >> 8;
    f = e + 8388608;

  }
  return (f);
}

// Read the data inputs on the binary swithces
int BinaryThing(int Pin0, int Pin1)
{
  int Sum = 0;

  if (digitalRead(Pin0) == HIGH)
    Sum += 1;
  if (digitalRead(Pin1) == HIGH)
    Sum += 2;
  return (Sum);
}
