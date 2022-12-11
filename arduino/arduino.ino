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
int totalLinesToDraw = 0;
int pwm_val_1 = 0;
int pwm_val_2 = 0;
//int pwm_val_3 = 0;
int LED_PIN_1 = 10;
int LED_PIN_2 = 1;
//int LED_PIN_3 = 10;
int phase_size;
int increment_size;
int end_sequence = 0;

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

  pinMode(0, INPUT);
  attachInterrupt(digitalPinToInterrupt(0), interruptServiceRoutine, RISING);
}

void interruptServiceRoutine() {
  Serial.println("trigerred");
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);  
  digitalWrite(9, LOW);
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
        //  WDT->CLEAR.reg = 0xA5;
          delay(3);
        }
        if (x != x0) {
          myStepper1.step(stepsPerRevolution * xi);
        //  WDT->CLEAR.reg = 0xA5;
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
        //  WDT->CLEAR.reg = 0xA5;
          delay(3);
        }
        if (y != y0) {
          myStepper2.step(-stepsPerRevolution * yi);
        //  WDT->CLEAR.reg = 0xA5;
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

void updateProgressBar(){
  if (totalLinesProcessed <= phase_size){
       analogWrite(LED_PIN_1, pwm_val_1);
       pwm_val_1 += increment_size;
   }else if (totalLinesProcessed < phase_size*2){
       analogWrite(LED_PIN_2, pwm_val_2);
       pwm_val_2 += increment_size;
   } else if (totalLinesProcessed >= phase_size*2 && end_sequence <= 5){
       analogWrite(LED_PIN_1, 0);
       analogWrite(LED_PIN_2, 0);
//       analogWrite(LED_PIN_3, 0);
       delay(500);
       analogWrite(LED_PIN_1, 255);
       analogWrite(LED_PIN_2, 255);
//       analogWrite(LED_PIN_3, 255);
       delay(500);
       end_sequence++;
   }
}

void WDT_Handler(){
  WDT->INTFLAG.bit.EW = 1;
  Serial.println("Warning: watchdog reset imminent!");
}

// the loop routine runs over and over again forever:
void loop() {
//  Serial.println(digitalRead(10));
  WDT->CLEAR.reg = 0xA5;
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
      {
        int spaceDelim = currLine.indexOf(" ");
        totalLinesToDraw = currLine.substring(spaceDelim + 1).toInt(); // could be wrong parsing
        Serial.println(String(totalLinesToDraw));
        phase_size = totalLinesToDraw/3; 
        increment_size = 255/phase_size;
        
        // Watch this, Dog!

        // Clear and enable WDT
        NVIC_DisableIRQ(WDT_IRQn);
        NVIC_ClearPendingIRQ(WDT_IRQn);
        NVIC_SetPriority(WDT_IRQn, 0);
        NVIC_EnableIRQ(WDT_IRQn);
      
        // Configure and enable WDT GCLK:
         GCLK->GENDIV.reg = GCLK_GENDIV_DIV(4) | GCLK_GENDIV_ID(5);
         while (GCLK->STATUS.bit.SYNCBUSY);
         GCLK->GENCTRL.reg = GCLK_GENCTRL_DIVSEL | GCLK_GENCTRL_ID(5) | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC(3);
         while (GCLK->STATUS.bit.SYNCBUSY);
         GCLK->CLKCTRL.reg = GCLK_CLKCTRL_GEN(5) | GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_ID(3);
        // Configure and enable WDT:
         WDT->EWCTRL.reg = WDT_EWCTRL_EWOFFSET(8);
         WDT->CONFIG.reg = WDT_CONFIG_PER(9);
         WDT->CTRL.reg = WDT_CTRL_ENABLE;
         while (WDT->STATUS.bit.SYNCBUSY);
      
        // Enable early warning interrupts on WDT:
         WDT->INTENSET.reg = WDT_INTENSET_EW;

        break;
      }
      case 'L':
      {
        if (totalLinesProcessed == totalLinesToDraw || totalLinesProcessed == totalLinesToDraw - 1 || totalLinesProcessed == totalLinesToDraw - 2) {
          Serial.println("INTENCLR Register");
           WDT->INTENCLR.reg = WDT_INTENCLR_EW;
        }
        WDT->CLEAR.reg = 0xA5;
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
      case 'S':
        digitalWrite(0, HIGH);
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
  updateProgressBar();
  Serial.println("readPointer: " + String(readPointer));
  Serial.println("writePointer: " + String(writePointer));
  Serial.println("AD " + String(totalLinesProcessed));

}
