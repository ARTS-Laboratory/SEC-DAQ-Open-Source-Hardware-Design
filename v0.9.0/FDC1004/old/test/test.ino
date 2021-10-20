void setup() {
  // put your setup code here, to run once
  Serial.begin(250000);


   int data = 2303;
   unsigned int c;
   int a = data;
unsigned int mask;
mask = 61440;
   Serial.println(a,BIN);
   Serial.println(mask,BIN);
   c = a | mask; 
   Serial.println(c,BIN);
}

void loop() {
  // put your main code here, to run repeatedly:

}
