// Ausitn Downey




int ledPin = 13; // LED connected to digital pin 13


// define the input pin locations

int range_0 = 2;   
int range_1 = 3; 
int channel_0 = 4;   
int channel_1 = 5;  
int sample_0 = 6;   
int sample_1 = 7; 
int trigger = 8;   

// define the variables for the input pins, start as zero
byte val_range = 0; 
byte val_channel = 0; 
byte val_sample = 0; 
int val_range_0 = 0;   
int val_range_1 = 0; 
int val_channel_0 = 0;   
int val_channel_1 = 0;  
int val_sample_0 = 0;   
int val_sample_1 = 0;   
int val_catch = 0;  

void setup()
{

  Serial.begin(9600);      // open the serial port at 9600 bps:    

  
  //sets all the digital pins as inputs and reads the values
  pinMode(range_0, INPUT);
  pinMode(range_1, INPUT);
  pinMode(channel_0, INPUT);
  pinMode(channel_1, INPUT);
  pinMode(sample_0, INPUT);
  pinMode(sample_1, INPUT);
  pinMode(trigger, INPUT);
  
}

void loop()
{

 // read the input pins
val_range = BinaryThing(range_0,range_1);
val_channel = BinaryThing(channel_0,channel_1);
val_sample = BinaryThing(sample_0,sample_1);
val_catch = digitalRead(trigger);


Serial.print(val_range);
Serial.print("\t");
Serial.print(val_channel);
Serial.print("\t");
Serial.print(val_sample);
Serial.print("\t");
Serial.println(val_catch);


/*
Serial.print(val_range_0);
Serial.print("\t");
Serial.print(val_range_1);
Serial.print("\t");

Serial.print(val_channel_0);
Serial.print("\t");
Serial.print(val_channel_1);
Serial.print("\t");

Serial.print(val_sample_0);
Serial.print("\t");
Serial.print(val_sample_1);
Serial.print("\t");
Serial.println(val_trigger);
*/

}



// Libary Functions


int BinaryThing(int Pin0,int Pin1)
{
  int Sum = 0;

  if (digitalRead(Pin0) == HIGH)
    Sum += 1;
  if (digitalRead(Pin1) == HIGH)
    Sum += 2;
  return(Sum);
}
