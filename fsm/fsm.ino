#include "fsm.h"

// FSM Variables
state CURRENT_STATE;
lineInstruction lineInstructionsBuffer[255];
int readPointer, writePointer;
int totalLinesToDraw, totalLinesProcessed;
int cursorX, cursorY, latestX, latestY;

void setup() {
  Serial.begin(9600);
  while(!Serial);
  initializeSystem();
  // initialize all FSM variables
  CURRENT_STATE = sWAITING;
  for (int i = 0; i < 255; i++) {
    lineInstructionsBuffer[i] = {0, 0};
  }
  readPointer = 0;
  writePointer = 0;
  totalLinesToDraw = 0;
  totalLinesProcessed = 1;
  cursorX = 0;
  cursorY = 0;
  latestX = 0;
  latestY = 0;
}

void loop() {
  // read in messages received via Serial
  msg = "";
  while (Serial.available()) {
   msg += char(Serial.read());
  }
  // pass those messages as input to FSM
  CURRENT_STATE = update_fsm(CURRENT_STATE, msg);
  delay(10);
}

state update_fsm(state curState, String msg){
  state nextState;
  switch(curState) {
    case sWAITING:
      // check the guard
      if (msg == "SR") { // transition 1-2 
        // set the output
        resetSystem();
        // transition to next state
        nextState = sRESET;
        return nextState;
      } else {
        nextState = curState;
        return nextState;
      }
      break;
    case sRESET:
      // check the guard
      if (cursorX == 0 && cursorY == 0) { // transition 2-3 
        // set the output
        Serial.println("AR ");
        // transition to next state
        nextState = sWAIT_TO_CLEAR;
        return nextState;
      } else {
        nextState = curState;
        return nextState;
      }
      break;
    case sWAIT_TO_CLEAR:
      // check the guard
      if (msg.charAt(0) == 'S' && msg.charAt(1) == 'D') { // transition 3-4 
        // set the output
        enableWatchdog();
        int spaceDelim = msg.indexOf(" ");
        // update variables
        totalLinesToDraw = msg.substring(spaceDelim + 1).toInt();
        phaseSize = totalLinesToDraw / 2; 
        incrementSize = 255 / phaseSize;
        // transition to next state
        nextState = sCLEARED;
        return nextState;
      } else {
        nextState = curState;
        return nextState;
      }
      break;
    case sCLEARED:
      // check the guard
      if (msg.charAt(0) == 'S' && msg.charAt(1) == 'F') { // transition 4-5
        // set the output
        Serial.println("AD " + String(totalLinesProcessed));
        // update variables
        lineInstructionsBuffer[writePointer] = extractLineInstruction(msg);
        writePointer = (writePointer + 1) % 255;
        totalLinesProcessed += 1;
        // transition to next state
        nextState = sDRAWING;
        return nextState;
      } else {
        nextState = curState;
        return nextState;
      }
      break;
    case sDRAWING:
      // check the guard
      if (totalLinesProcessed == totalLinesToDraw) { // transition 5-1
        // set the output
        disableWatchdog();
        // transition to next state
        nextState = sWAITING;
        return nextState;
      } else if (cursorX == latestX && cursorY == latestY && totalLinesProcessed != totalLinesToDraw) { // transition 5-6
        // set the output
        int xCoord = lineInstructionsBuffer[readPointer].xCoord;
        int yCoord = lineInstructionsBuffer[readPointer].yCoord;
        plotLine(cursorX, cursorY, xCoord, yCoord);
        updateProgressBar();
        // update variables
        readPointer = (readPointer + 1) % 255;
        latestX = xCoord;
        latestY = yCoord;
        // transition to next state
        nextState = sREQ_INSTR;
        return nextState;
      } else {
        nextState = curState;
        return nextState;
      }
      break;
    case sREQ_INSTR:
      if (msg.charAt(0) == 'S' && msg.charAt(1) == 'L') { // transition 6-5
        // set the output
        Serial.println("AD " + String(totalLinesProcessed));
        // update variables
        lineInstructionsBuffer[writePointer] = extractLineInstruction(msg);
        writePointer = (writePointer + 1) % 255;
        totalLinesProcessed += 1;
        // transition to next state
        nextState = sDRAWING;
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
