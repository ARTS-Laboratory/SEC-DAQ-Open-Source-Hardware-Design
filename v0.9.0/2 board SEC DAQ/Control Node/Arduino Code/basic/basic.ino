


void setup() {
   // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
//String data = String(String(100,5) + "\t" + String(100,5) + "\t" + String(CapCh3,5) + "\t" + String(CapCh4,5));

// print out the value you read:
  Serial.println("100 \t 110 \t 120 \t 130 ");
  delay(500);
}
