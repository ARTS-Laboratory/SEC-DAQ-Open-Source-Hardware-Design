// Ausitn Downey




int ledPin = 13; // LED connected to digital pin 13


// define the input pin locations

int channel_0 = 4;   
int channel_1 = 5;  
int range_0 = 2;   
int range_1 = 3;   
int sample_0 = 6;   
int sample_1 = 7; 
int trigger = 8;   

// define the variables for the input pins, start as zero
int val_channel_0 = 0;   
int val_channel_1 = 0;  
int val_range_0 = 0;   
int val_range_1 = 0;   
int val_sample_0 = 0;   
int val_sample_1 = 0;   
int val_trigger = 0;  

void setup()
{

  Serial.begin(9600);      // open the serial port at 9600 bps:    

  
  //sets all the digital pins as inputs and reads the values
  pinMode(channel_0, INPUT);
  pinMode(channel_1, INPUT);
  pinMode(range_0, INPUT);
  pinMode(range_1, INPUT);
  pinMode(sample_0, INPUT);
  pinMode(sample_1, INPUT);
  pinMode(trigger, INPUT);
  
}

void loop()
{

val_channel_0 = digitalRead(channel_0);   // read the input pin
val_channel_1 = digitalRead(channel_1);   // read the input pin          
val_range_0 = digitalRead(range_0);   // read the input pin
val_range_1 = digitalRead(range_1);   // read the input pin   
val_sample_0 = digitalRead(sample_0);   // read the input pin
val_sample_1 = digitalRead(sample_1);   // read the input pin   
val_trigger = digitalRead(trigger);   // read the input pin   


Serial.print(val_channel_0);
Serial.print("\t");
Serial.print(val_channel_1);
Serial.print("\t");

Serial.print(val_range_0);
Serial.print("\t");
Serial.print(val_range_1);
Serial.print("\t");

Serial.print(val_sample_0);
Serial.print("\t");
Serial.print(val_sample_1);
Serial.print("\t");
Serial.println(val_trigger);

}
