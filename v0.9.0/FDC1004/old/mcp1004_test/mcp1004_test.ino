// Ausitn Downey
// Arduino IDE 1.6.6

#include <Wire.h>
#include <SPI.h>


// set pin 10 as the slave select for the digital pot:
const int slaveSelectPin = 10;


const int analogPin0 = 0;      // analog in sensor 1 connected to analog pin 0 (temp pin)
const int analogPin1 = 1;      // analog in sensor 1 connected to analog pin 0 (temp pin)


// Varaibles used to set the analog output of the DAC. 
int AnalogOutCh1;
int AnalogOutCh2;
int AnalogOutCh3;
int AnalogOutCh4;

void setup() {
  // put your setup code here, to run once:

  // open the serial port
  Serial.begin(9600);
  //Serial.begin(234000);
  //Serial.begin(250000);

  
   // initialize SPI:
  SPI.begin();
  
  // set the slaveSelectPin as an output:
  pinMode(slaveSelectPin, OUTPUT);
  digitalWrite(slaveSelectPin, HIGH);  // set the slave select pin high, as low in output. 
  delay(1);

}

void loop() {
  // put your main code here, to run repeatedly:

AnalogOutCh1 = 100;
AnalogOutCh2 = 3000;

word AnalogOutCh1Ready = dataPrepA(AnalogOutCh1);
word AnalogOutCh2Ready = dataPrepB(AnalogOutCh2);

      byte  Ch1MSB = highByte(AnalogOutCh1Ready);   //mask LSB, shift 8 bits to the right
      byte  Ch1LSB = lowByte(AnalogOutCh1Ready);    //mask MSB, no need to shift
      byte  Ch2MSB = highByte(AnalogOutCh2Ready);   
      byte  Ch2LSB = lowByte(AnalogOutCh2Ready);    

      // shift the value out to the DAC via SPI here. This only does one channel for mow. 
     
      digitalWrite(slaveSelectPin, LOW);
      SPI.transfer ( Ch1MSB );
      SPI.transfer ( Ch1LSB );
      digitalWrite(slaveSelectPin, HIGH);

      delay(10);

      digitalWrite(slaveSelectPin, LOW);
      SPI.transfer ( Ch2MSB );
      SPI.transfer ( Ch2LSB );
      digitalWrite(slaveSelectPin, HIGH);













      // Here I use anlog input 0 to read the voltage coming from the DAC, this is a temp solution for troubleshooting.
      int val = analogRead(analogPin0);    // read the input pin
      float val2 = (float (val)/1023)*5;
     
      Serial.print ("voltage read   ");
      Serial.print (val2);             // reading back the voltage sent to the dAC

      // Here I use anlog input 0 to read the voltage coming from the DAC, this is a temp solution for troubleshooting.
      val = analogRead(analogPin1);    // read the input pin
      val2 = (float (val)/1023)*5;
     
      Serial.print (",  ");
      Serial.println(val2);             // reading back the voltage sent to the dAC
     
     
 
}


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
