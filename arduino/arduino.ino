int a = 0;

void setup() {
  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
  Serial.println(a);
  a += 1;
  delay(10000);  // delay in between reads for stability
}
