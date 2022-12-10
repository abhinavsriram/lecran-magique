#include <Arduino.h>
#include <Stepper.h>

typedef struct {
  int xCoord;
  int yCoord;
} lineInstruction;
lineInstruction lineInstructionsBuffer[255];
int readPointer = 0;
int writePointer = 0;
int totalLinesProcessed = 0;

int lineCoords[100];

const int stepsPerRevolution = 30;
int cursor_x = 0;
int cursor_y = 0;
Stepper myStepper1(stepsPerRevolution, 2, 3, 4, 5);
Stepper myStepper2(stepsPerRevolution, 6, 7, 8, 9);

void setup() {
  // pinMode(6, OUTPUT);
  myStepper1.setSpeed(500); // max speed = 500
  myStepper2.setSpeed(500);
  Serial.begin(9600);
}

void plotLineLow(int x0, int y0, int x1, int y1) {
    int dx = x1 - x0;
    int xi = 1;
    if (dx < 0) {
      xi = -1;
      dx = -dx;
    }
    int dy = y1 - y0;
    int yi = 1;
    if (dy < 0) {
        yi = -1;
        dy = -dy;
    }
    int D = (2 * dy) - dx;
    int y = y0;
    bool hasYChanged = false;
    for (int x = x0; ; x += xi) {
        if (xi == 1) {
          if (x > x1) {
            break;
          }
        }
        if (xi == -1) {
          if (x < x1) {
            break;
          }
        }
        // Serial.println(String(x) + ", " + String(y));
        if (hasYChanged) {
          myStepper2.step(-stepsPerRevolution * yi);
          delay(3);
        }
        if (x != x0) {
          myStepper1.step(stepsPerRevolution * xi);
          delay(3);
        }
        if (D > 0) {
            y = y + yi;
            D = D + (2 * (dy - dx));
            hasYChanged = true;
        } else {
            D = D + 2*dy;
            hasYChanged = false;
        }
    }
}

void plotLineHigh(int x0, int y0, int x1, int y1) {
    int dx = x1 - x0;
    int dy = y1 - y0;
    int xi = 1;
    if (dx < 0) {
        xi = -1;
        dx = -dx;
    }
    int yi = 1;
    if (dy < 0) {
      yi = -1;
      dy = -dy;
    }
    int D = (2 * dx) - dy;
    int x = x0;
    bool hasXChanged = false;

    for (int y = y0; ; y += yi) {
        if (yi == 1) {
          if (y > y1) {
            break;
          }
        }
        if (yi == -1) {
          if (y < y1) {
            break;
          }
        }
        // Serial.println(String(x) + ", " + String(y));
        if (hasXChanged) {
          myStepper1.step(stepsPerRevolution * xi);
          delay(3);
        }
        if (y != y0) {
          myStepper2.step(-stepsPerRevolution * yi);
          delay(3);
        }
        if (D > 0) {
            x = x + xi;
            D = D + (2 * (dx - dy));
            hasXChanged = true;
        } else {
            D = D + 2*dx;
            hasXChanged = false;
        }
    }
}

void plotLine(int x0, int y0, int x1, int y1) {
    if (abs(y1 - y0) < abs(x1 - x0)) {
        if (x0 > x1) {
            // plotLineLow(x1, y1, x0, y0);
            plotLineLow(x0, y0, x1, y1);
        } else {
            // plotLineLow(x0, y0, x1, y1);
            plotLineLow(x0, y0, x1, y1);
        }
    } else {
        if (y0 > y1) {
            // plotLineHigh(x1, y1, x0, y0);
            plotLineHigh(x0, y0, x1, y1);
        } else {
            plotLineHigh(x0, y0, x1, y1);
        }
    }
    cursor_x = x1;
    cursor_y = y1;
}

// the loop routine runs over and over again forever:
void loop() {
//  if (totalLinesProcessed == 122) {
//    Serial.println("122 - readPointer: " + String(readPointer));
//    Serial.println("122 - writePointer: " + String(writePointer));
//  }
  if ((readPointer + 1) % 255 == writePointer) {
//    Serial.println("reaches - readPointer: " + String(readPointer));
//    Serial.println("reaches - writePointer: " + String(writePointer));
    return;
  }
  String currLine;
  while (Serial.available()) {
    currLine += char(Serial.read());
  }
//  if (totalLinesProcessed == 122) {
////    Serial.println("122 - currLine: " + currLine);
//  }
  if (currLine.charAt(0) == 'S') {
    switch (currLine.charAt(1)) {
      case 'D':
        break;
      case 'L':
      {
        int spaceDelim1 = currLine.indexOf(" ");
        int spaceDelim2 = currLine.lastIndexOf(" ");
        int xCoord = currLine.substring(spaceDelim1 + 1, spaceDelim2).toInt();
        int yCoord = currLine.substring(spaceDelim2).toInt();
        lineInstructionsBuffer[writePointer] = (lineInstruction) {xCoord, yCoord};
        writePointer = (writePointer + 1) % 255;
        break;
      }
      case 'R':
        for (int i = 1; i <= 5; i++) {
          delay (1000);
          Serial.println("AR " + String(i * 20));
        }
        break;
      default:
        break;
    }
  }                           
//  else {
//    Serial.println()
//  }
  if (readPointer == writePointer) {
//    Serial.println("readPointer == writePointer");
    return;
  }
  int xCoord = lineInstructionsBuffer[readPointer].xCoord;
  int yCoord = lineInstructionsBuffer[readPointer].yCoord;
  Serial.println("from: " + String(cursor_x) + ", " + String(cursor_y) + " to: " + String(xCoord) + ", " + String(yCoord));
  plotLine(cursor_x, cursor_y, xCoord, yCoord);
  delay(50);
  readPointer = (readPointer + 1) % 255;
  totalLinesProcessed += 1;
  Serial.println("readPointer: " + String(readPointer));
  Serial.println("writePointer: " + String(writePointer));
  Serial.println("AD " + String(totalLinesProcessed));
}
