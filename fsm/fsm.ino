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
  initializeStateVariables();
  test_all_tests();
}

void loop() {
//  // read in messages received via Serial
//  msg = "";
//  while (Serial.available()) {
//   msg += char(Serial.read());
//  }
//  // pass those messages as input to FSM
//  CURRENT_STATE = update_fsm(CURRENT_STATE, msg);
//  delay(10);
}

state update_fsm(state curState, String msg){
  // next state will be the current state unless a transition is made
  state nextState = curState;
  switch(curState) {
    case sWAITING:
      // check the guard
      if (msg == "SR") { // transition 1-2 
        // set the output
        resetSystem();
        // transition to next state
        nextState = sRESET;
      }
      break;
    case sRESET:
      // check the guard
      if (cursorX == 0 && cursorY == 0) { // transition 2-3 
        // set the output
        Serial.println("AR ");
        // transition to next state
        nextState = sWAIT_TO_CLEAR;
      }
      break;
    case sWAIT_TO_CLEAR:
      // check the guard
      if (msg.charAt(0) == 'S' && msg.charAt(1) == 'D') { // transition 3-4 
        // set the output
        enableWatchdog();
        // update variables
        totalLinesToDraw = msg.substring(msg.indexOf(" ") + 1).toInt();
        phaseSize = totalLinesToDraw / 2; 
        incrementSize = 255 / phaseSize;
        // transition to next state
        nextState = sCLEARED;
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
      }
      break;
    case sDRAWING:
      // check the guard
      if (totalLinesProcessed == totalLinesToDraw) { // transition 5-1
        // set the output
        disableWatchdog();
        updateProgressBar();
        // transition to next state
        nextState = sWAITING;
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
      }
      break;
    default:
      nextState = sWAITING;
      break;
  }
  return nextState;
}
