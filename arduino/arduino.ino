int totalLinesProcessed = 0;

void setup() {
  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
  // Serial.println(a);
  // a += 1;
  // delay(10000);  // delay in between reads for stability

  String currLine;
  while (Serial.available()) {
    currLine += char(Serial.read());
  }

  // SD, SL <Int> <Int>, SR, AD <Int>, AR <Int>
  if (currLine.charAt(0) == 'S') {
    switch (currLine.charAt(1)) {
      case 'D':
        digitalWrite(6, HIGH);
        delay(3000);
        digitalWrite(6, LOW);
        delay (3000);
        break;
      case 'L':
      {
        int spaceDelim1 = currLine.indexOf(" ");
        int spaceDelim2 = currLine.lastIndexOf(" ");
        int xCoord = currLine.substring(spaceDelim1 + 1, spaceDelim2).toInt();
        int yCoord = currLine.substring(spaceDelim2 + 1).toInt();
        totalLinesProcessed += 1;
        // this will be in another function call...
        Serial.println("AD " + String(totalLinesProcessed));
        break;
      }
      case 'R':
        for (int i = 0; i < 10; i++) {
          digitalWrite(6, HIGH);
          delay(1000);
          digitalWrite(6, LOW);
          delay (1000);
          // this will be in another function call...
          Serial.println("AR " + String(i));
        }
        break;
      default:
        break;
    }
  }
}
