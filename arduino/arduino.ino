#include <Stepper.h>

// States
typedef enum {
  sWAITING = 1,
  sRESET = 2,
  sWAIT_TO_CLEAR = 3,
  sCLEARED = 4,
  sDRAWING = 5,
  sREQ_INSTR = 6,
} state;

int BUTTON_PIN = 0;
const int stepsPerRevolution = 30;
bool isrTriggered;
int prevHorzDir = 0;
int prevVertDir = 0;

int pwm_val_1 = 0;
int pwm_val_2 = 0;
int LED_PIN_1 = 10;
int LED_PIN_2 = 1;

int phase_size;
int increment_size;
int end_sequence = 0;


// Stores x and y coordinates of an SL instruction
typedef struct {
  int xCoord;
  int yCoord;
} lineInstruction;

// Buffer that holds series of SL instructions
lineInstruction lineInstructionsBuffer[255];
// Advances when SL command has executed
int readPointer;
// Advances when SL command is received
int writePointer;
bool resetComplete;

int totalLinesProcessed, totalLinesToDraw, cursorX, cursorY, latestX, latestY;

Stepper horzStepper(stepsPerRevolution, 2, 3, 4, 5);
Stepper vertStepper(stepsPerRevolution, 6, 7, 8, 9);

state CURRENT_STATE;

void setup() {
  pinMode(BUTTON_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), interruptServiceRoutine, RISING);
  horzStepper.setSpeed(500);
  vertStepper.setSpeed(500);
  Serial.begin(9600);
  while(!Serial);

  CURRENT_STATE = (state) 1;
  isrTriggered = false;
  totalLinesToDraw = 0;
  totalLinesProcessed = 1;
  readPointer = 0;
  writePointer = 0;
  cursorX = 0;
  cursorY = 0;
  latestX = 0;
  latestY = 0;
}

void loop() {
  String serialMsg;
  while (Serial.available()) {
   serialMsg += char(Serial.read());
  }
  Serial.println(CURRENT_STATE);
  Serial.println(totalLinesToDraw);
  Serial.println( totalLinesProcessed);
  CURRENT_STATE = update_fsm(CURRENT_STATE, serialMsg);
  delay(10);
  
}

//String receiveMsg(){
//  String serialMsg;
//  while (Serial.available()) {
//   serialMsg += char(Serial.read());
//  }
//  return serialMsg;
//}
void configureWatchDog() {
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
    WDT->EWCTRL.reg = WDT_EWCTRL_EWOFFSET(9);
    WDT->CONFIG.reg = WDT_CONFIG_PER(11);
    WDT->CTRL.reg = WDT_CTRL_ENABLE;
    while (WDT->STATUS.bit.SYNCBUSY);
   // Enable early warning interrupts on WDT:
    WDT->INTENSET.reg = WDT_INTENSET_EW;
}

void disableWatchDog(){
   NVIC_DisableIRQ(WDT_IRQn);
   NVIC_ClearPendingIRQ(WDT_IRQn);
    WDT->CTRL.reg = 0;
   
}

void interruptServiceRoutine() {
  Serial.println("AS ");
  for (int i = 0; i<255; i++){
    lineInstructionsBuffer[i] = {0, 0};
  }
  CURRENT_STATE = (state) 1;
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);  
  digitalWrite(9, LOW);

}

boolean horzDirChange(int dir) {
  if (prevHorzDir != 0) {
    if (dir != prevHorzDir) {
      prevHorzDir = dir;
      return true;
    } else {
      return false;
    }
  } else {
    prevHorzDir = dir;
    return false;
  }
}

boolean vertDirChange(int dir) {
  if (prevVertDir != 0) {
    if (dir != prevVertDir) {
      prevVertDir = dir;
      return true;
    } else {
      return false;
    }
  } else {
    prevVertDir = dir;
    return false;
  }
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

//    if (dx != 0 && dy != 0 && horzDirChange(xi)) {
//      myStepper1.step(stepsPerRevolution * 15);
//    }
//
//    if (dy != 0 && dx != 0 && vertDirChange(yi)) {
//      myStepper2.step(stepsPerRevolution * 15);
//    }

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
        if (hasYChanged) {
          vertStepper.step(-stepsPerRevolution * yi);
          WDT->CLEAR.reg = 0xA5;
          delay(3);
        }
        if (x != x0) {
          horzStepper.step(stepsPerRevolution * xi);
          WDT->CLEAR.reg = 0xA5;
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

//    if (dx != 0 && dy != 0 && horzDirChange(xi)) {
//      myStepper1.step(stepsPerRevolution * 15);
//    }
//
//    if (dy != 0 && dx != 0 && vertDirChange(yi)) {
//      myStepper2.step(stepsPerRevolution * 15);
//    }

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
        if (hasXChanged) {
          horzStepper.step(stepsPerRevolution * xi);
          WDT->CLEAR.reg = 0xA5;
          delay(3);
        }
        if (y != y0) {
          vertStepper.step(-stepsPerRevolution * yi);
          WDT->CLEAR.reg = 0xA5;
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
            plotLineLow(x0, y0, x1, y1);
        } else {
            plotLineLow(x0, y0, x1, y1);
        }
    } else {
        if (y0 > y1) {
            plotLineHigh(x0, y0, x1, y1);
        } else {
            plotLineHigh(x0, y0, x1, y1);
        }
    }
    cursorX = x1;
    cursorY = y1;
//    Serial.println("plotLine executes ---------------------------------------------------------------");
}

void updateProgressBar(){
  if (totalLinesProcessed <= phase_size){
       analogWrite(LED_PIN_1, pwm_val_1);
       pwm_val_1 += increment_size;
   }else if (totalLinesProcessed < phase_size*2){
       analogWrite(LED_PIN_2, pwm_val_2);
       pwm_val_2 += increment_size;
   } else if (totalLinesProcessed >= phase_size*2&& end_sequence <= 5){
       disableWatchDog();
       analogWrite(LED_PIN_1, 0);
       analogWrite(LED_PIN_2, 0);
       delay(500);
       analogWrite(LED_PIN_1, 255);
       analogWrite(LED_PIN_2, 255);
       delay(500);
       end_sequence++;
   }
}

void WDT_Handler(){
  WDT->INTFLAG.bit.EW = 1;
  Serial.println("Warning: watchdog reset imminent!");
}

void reset(){
  plotLine(cursorX, cursorY, 0, 0);
  isrTriggered = false;
  prevHorzDir = 0;
  prevVertDir = 0;
  pwm_val_1 = 0;
  pwm_val_2 = 0;
  phase_size = 0;
  increment_size = 0;
  end_sequence = 0;
  totalLinesProcessed = 1;
  totalLinesToDraw = 0; 
  cursorX = 0;
  cursorY= 0;
  latestX = 0;
  latestY = 0;
  readPointer = 0;
  writePointer = 0;
}




state update_fsm(state curState, String msg){
  state nextState;
  switch(curState){
    case sWAITING:
      if (msg == "SR"){ // 1-2 
         reset();
         resetComplete = true;
         Serial.println("AR ");
         nextState = sRESET;
         return nextState;
        } else {
          nextState = curState;
          return nextState;
        }
      break;
    case sRESET:
      if (resetComplete){ // 2-3 
         resetComplete = false;
         nextState = sWAIT_TO_CLEAR;
         return nextState;
        } else {
          nextState = curState;
          return nextState;
        }
      break;
    case sWAIT_TO_CLEAR:
      if (msg.charAt(0) == 'S' && msg.charAt(1) == 'D') { // 3-4 
        int spaceDelim = msg.indexOf(" ");
             
   
        totalLinesToDraw = msg.substring(spaceDelim + 1).toInt();
              
         phase_size = totalLinesToDraw/2; 
         increment_size = 255/phase_size;
        configureWatchDog();
        nextState = sCLEARED;
        Serial.println("AD " + String(totalLinesProcessed));
        return nextState;
        } else {
          nextState = curState;
          return nextState;
        }
      break;
    case sCLEARED:
      if (cursorX == latestX && cursorY == latestY){ // 4-6
         nextState = sREQ_INSTR;
         return nextState;
      } else {
          nextState = curState;
          return nextState;
        }
      break;
    case sDRAWING:
      if (isrTriggered) { //5-1
        isrTriggered = false;
            nextState = sWAITING;
            return nextState;
      }
      else if  (cursorX == latestX && cursorY == latestY) { // 5-6
         int xCoord = lineInstructionsBuffer[readPointer].xCoord;
         int yCoord = lineInstructionsBuffer[readPointer].yCoord;
         plotLine(cursorX, cursorY, xCoord, yCoord);
         latestX = xCoord;
         latestY = yCoord;
         delay(50);
         readPointer = (readPointer + 1) % 255;
         totalLinesProcessed += 1;
         updateProgressBar();
         Serial.println("AD " + String(totalLinesProcessed));
         if (totalLinesToDraw == totalLinesProcessed){ // 5-1
            nextState = sWAITING;
            return nextState;}
         nextState = sREQ_INSTR;
         return nextState;
      } else if (totalLinesToDraw == totalLinesProcessed || isrTriggered){ // 5-1
        isrTriggered = false;
        nextState = sWAITING;
        return nextState;
      } else {
         nextState = curState;
         return nextState;
      }
      break;
    case sREQ_INSTR:
      if (msg.charAt(0) == 'S' && msg.charAt(1) == 'L'){ // 6-5
         int spaceDelim1 = msg.indexOf(" ");
         int spaceDelim2 = msg.lastIndexOf(" ");
         int xCoord = msg.substring(spaceDelim1 + 1, spaceDelim2).toInt();
         int yCoord = msg.substring(spaceDelim2).toInt();
         lineInstructionsBuffer[writePointer] = (lineInstruction) {xCoord, yCoord};
         writePointer = (writePointer + 1) % 255;
         nextState = sDRAWING;
//         WDT->CLEAR.reg = 0xA5;
         return nextState;
      } else {
          nextState = curState;
          return nextState;
        }
      break;
    default:
         nextState = sWAITING;
      break;
}
}
