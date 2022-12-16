/*        
 * Helper function for printing states.
 */
char* s2str(state s) {
  switch(s) {
    case sWAITING:
    return "(1) sWAITING";
    case sRESET:
    return "(2) sRESET";
    case sWAIT_TO_CLEAR:
    return "(3) sWAIT_TO_CLEAR";
    case sCLEARED:
    return "(4) sCLEARED";
    case sDRAWING:
    return "(5) sDRAWING";
    case sREQ_INSTR:
    return "(6) sREQ_INSTR";
    default:
    return "???";
  }
}

/*
 * Given a start state, inputs, and starting values for state 
 * variables, tests that updateFSM returns the correct end state 
 * and updates the state variables correctly returns true if this 
 * is the case (test passed) and false otherwise (test failed).
 * 
 * Need to use "verbos" instead of "verbose" because verbose is apparently a keyword
 */
bool test_transition(state start_state, state end_state, state_inputs test_state_inputs, state_vars start_state_vars, state_vars end_state_vars, bool verbos) {
  readPointer= start_state_vars.readPointer;
  writePointer = start_state_vars.writePointer;
  totalLinesProcessed = start_state_vars.totalLinesProcessed;
  totalLinesToDraw = start_state_vars.totalLinesToDraw;
  cursorX = start_state_vars.cursorX;
  cursorY = start_state_vars.cursorY;
  latestX = start_state_vars.latestX;
  latestY = start_state_vars.latestY;
  state result_state = updateFSM(start_state, test_state_inputs.msg);
  bool passed_test = (end_state == result_state and
                      readPointer == end_state_vars.readPointer and
                      writePointer == end_state_vars.writePointer and
                      totalLinesProcessed == end_state_vars.totalLinesProcessed and
                      totalLinesToDraw == end_state_vars.totalLinesToDraw and
                      cursorX == end_state_vars.cursorX and
                      cursorY == end_state_vars.cursorY and
                      latestX == end_state_vars.latestX and
                      latestY == end_state_vars.latestY);
  if (!verbos) {
    return passed_test;
  } else if (passed_test) {
    char s_to_print[200];
    sprintf(s_to_print, "Test from %s to %s PASSED", s2str(start_state), s2str(end_state));
    Serial.println(s_to_print);
    return true;
  } else {
    char s_to_print[200];
    Serial.println(s2str(start_state));
    sprintf(s_to_print, "Test from %s to %s FAILED", s2str(start_state), s2str(end_state));
    Serial.println(s_to_print);
    sprintf(s_to_print, "End state expected: %s | actual: %s", s2str(end_state), s2str(result_state));
    Serial.println(s_to_print);
    sprintf(s_to_print, "Inputs: msg %s", test_state_inputs.msg);
    Serial.println(s_to_print);
    sprintf(s_to_print, "         %2s | %5s | %3s | %3s | %5s | %9s | %11s | %9s", "readPointer", "writePointer", "totalLinesProcessed", "totalLinesToDraw", "cursorX", "cursorY", "latestX", "latestY");
    Serial.println(s_to_print);
    sprintf(s_to_print, "expected:  %2d | %5d | %3d | %3d | %5d | %9d | %11d | %9d", end_state_vars.readPointer, end_state_vars.writePointer, end_state_vars.totalLinesProcessed, end_state_vars.totalLinesToDraw, end_state_vars.cursorX, end_state_vars.cursorY, end_state_vars.latestX, end_state_vars.latestY);
    Serial.println(s_to_print);
    sprintf(s_to_print, "actual:    %2d | %5d | %3d | %3d | %5d | %9d | %11d | %9d", readPointer, writePointer, totalLinesProcessed, totalLinesToDraw, cursorX, cursorY, latestX, latestY);
    Serial.println(s_to_print);
    return false;
  }
}

const state test_states_in[10] = {(state) 1, (state) 2, (state) 3, (state) 4, (state) 5, (state) 6, (state) 5, (state) 5, (state) 5, (state) 5};
const state test_states_out[10] = {(state) 2, (state) 3, (state) 4, (state) 5, (state) 1, (state) 5, (state) 6, (state) 6, (state) 6, (state) 6};
const state_inputs test_input[10] = {{"SR"},{},{"SD 200"},{"SF 0 0"},{}, {"SL 20 20"}, {}, {}, {}, {}};
const state_vars test_in_vars[10] = {{0, 0, 1, 0, 0, 0, 0, 0},  {0, 0, 1, 0, 0 , 0, 0, 0},  {0, 0, 1, 0, 0, 0, 0, 0},  {0, 0, 1, 50, 0, 0, 0, 0}, {0, 0, 87, 87, 6, 0, 5, 10}, {4, 3, 10, 20, 10, 10, 10, 10}, {1, 2, 10, 20, 30, 30, 30, 30}, {3, 4, 10, 20, 30, 40, 30, 40},  {5, 6, 10, 20, 50, 50, 50, 50}, {7, 8, 10, 20, 50, 50, 50, 50}};
const state_vars test_out_vars[10] = {{0, 0, 1, 0, 0 , 0, 0, 0}, {0, 0, 1, 0, 0 , 0, 0, 0}, {0, 0, 1, 200, 0, 0, 0, 0}, {0, 1, 2, 50, 0, 0, 0, 0}, {0, 0, 87, 87, 6, 0, 5, 10}, {4, 4, 11, 20, 10, 10, 10,10}, {2, 2, 10, 20, 20, 10, 20, 10}, {4, 4, 10, 20, 50, 50, 50, 50},  {6, 6, 10, 20, 60, 30, 60, 30},  {8, 8, 10, 20, 30, 60, 30, 60}};
const int num_tests = 10;

#ifndef TESTING
void testing_state_edit() { }
bool test_all_tests() { }
 
#else
/*
 * Runs through all the test cases defined above.
 */
bool test_all_tests() {
//  for (int i = 0; i < num_tests; i++) {
//    Serial.print("Running test ");
//    Serial.println(i);
//    if (!test_transition(test_states_in[i], test_states_out[i], test_input[i], test_in_vars[i], test_out_vars[i], true)) {
//      return false;
//    }
//    Serial.println();
//  }
  delay(5000);
  state current_test_state = sWAITING; 
  for (int j = 1; j < 8; j++) {
    Serial.println("TEST_SCENARIO_1 " + String(j));
    delay(100);
    String test_msg = "";
    while (Serial.available()) {
      test_msg += char(Serial.read());
    }
    // pass those messages as input to FSM
    Serial.println("CURRENT_STATE: " + String(current_test_state));
    Serial.println("MESSAGE: " + String(test_msg));
    current_test_state = updateFSM(current_test_state, test_msg);
    delay(100);
  }
  Serial.println("All tests passed!");
  return true;
}

/*
 * Loads the lineInstructionBuffer with predetermined 
 * values for the first 8 indices. 
 */
void testing_state_edit() {
//  CURRENT_STATE = sWAITING;
//  for (int i = 0; i < 255; i++) {
//    lineInstructionsBuffer[i] = {0, 0};
//  }
//  // both -
//  lineInstructionsBuffer[0] = extractLineInstruction("SL 30 30");
//  lineInstructionsBuffer[1] = extractLineInstruction("SL 20 10");
//  // both +
//  lineInstructionsBuffer[2] = extractLineInstruction("SL 30 40");
//  lineInstructionsBuffer[3] = extractLineInstruction("SL 50 50");
//  // + x, - y
//  lineInstructionsBuffer[4] = extractLineInstruction("SL 50 50");
//  lineInstructionsBuffer[5] = extractLineInstruction("SL 60 30");
//  // - x, - y
//  lineInstructionsBuffer[6] = extractLineInstruction("SL 50 50");
//  lineInstructionsBuffer[7] = extractLineInstruction("SL 30 60");
}
#endif
