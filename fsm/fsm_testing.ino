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

const state test_states_in[17] = {(state) 1, (state) 2, (state) 3, (state) 4, (state) 5, (state) 6, (state) 5, (state) 5, (state) 5, (state) 5, (state) 5, (state) 1, (state) 2, (state) 3, (state) 4, (state) 5, (state) 6};
const state test_states_out[17] = {(state) 2, (state) 3, (state) 4, (state) 5, (state) 1, (state) 5, (state) 6, (state) 6, (state) 6, (state) 6, (state) 6, (state) 1, (state) 2, (state) 3, (state) 4, (state) 5, (state) 6};
const state_inputs test_input[17] = {{"SR"}, {}, {"SD 200"}, {"SF 0 0"}, {}, {"SL 20 20"}, {}, {}, {}, {}, {}, {}, {}, {"SF"}, {"SD 10"}, {}, {"SF"}};
const state_vars test_in_vars[17] = {{0, 0, 0, 0, 0, 0, 0, 0},  {0, 0, 0, 0, 0 , 0, 0, 0},  {0, 0, 0, 0, 0, 0, 0, 0},  {0, 0, 0, 50, 0, 0, 0, 0}, {0, 0, 87, 87, 6, 0, 5, 10}, {4, 3, 10, 20, 10, 10, 10, 10}, {1, 2, 10, 20, 30, 30, 30, 30}, {3, 4, 10, 20, 30, 40, 30, 40}, {5, 6, 10, 20, 50, 50, 50, 50}, {7, 8, 10, 20, 50, 50, 50, 50}, {9, 10, 5, 7, 10, 10, 10, 10}, {0, 0, 1, 0, 0, 0, 0, 0}, {0, 0, 1, 50, 4, 4, 0, 0}, {0, 0, 1, 50, 0, 0, 0, 0}, {0, 0, 1, 50, 0, 0, 0, 0}, {0, 0, 1, 50, 4, 4, 0, 0}, {0, 0, 1, 50, 0, 0, 0, 0}};
const state_vars test_out_vars[17] = {{0, 0, 0, 0, 0 , 0, 0, 0}, {0, 0, 0, 0, 0 , 0, 0, 0}, {0, 0, 0, 200, 0, 0, 0, 0}, {0, 1, 1, 50, 0, 0, 0, 0}, {0, 0, 87, 87, 6, 0, 5, 10}, {4, 4, 11, 20, 10, 10, 10,10}, {2, 2, 10, 20, 20, 10, 20, 10}, {4, 4, 10, 20, 50, 50, 50, 50}, {6, 6, 10, 20, 60, 30, 60, 30}, {8, 8, 10, 20, 30, 60, 30, 60}, {10, 10, 5, 7, 30, 60, 30, 60}, {0, 0, 1, 0, 0, 0, 0, 0}, {0, 0, 1, 50, 4, 4, 0, 0}, {0, 0, 1, 50, 0, 0, 0, 0}, {0, 0, 1, 50, 0, 0, 0, 0}, {0, 0, 1, 50, 4, 4, 0, 0}, {0, 0, 1, 50, 0, 0, 0, 0}};
const int num_tests = 17;

#ifndef UNIT_TESTING
void testing_state_edit() { }
bool test_all_unit_tests() { }
 
#else
/*
 * Runs through all the test cases defined above.
 */
bool test_all_unit_tests() {
  for (int i = 0; i < num_tests; i++) {
    Serial.print("Running test ");
    Serial.println(i);
    if (!test_transition(test_states_in[i], test_states_out[i], test_input[i], test_in_vars[i], test_out_vars[i], true)) {
      return false;
    }
    Serial.println();
  }
  Serial.println("All unit tests passed!");
}

/*
 * Loads the lineInstructionBuffer with predetermined 
 * values for the first 10 indices. 
 */
void testing_state_edit() {
  CURRENT_STATE = sWAITING;
  for (int i = 0; i < 255; i++) {
    lineInstructionsBuffer[i] = {0, 0};
  }
  // both -
  lineInstructionsBuffer[0] = extractLineInstruction("SL 30 30");
  lineInstructionsBuffer[1] = extractLineInstruction("SL 20 10");
  // both +
  lineInstructionsBuffer[2] = extractLineInstruction("SL 30 40");
  lineInstructionsBuffer[3] = extractLineInstruction("SL 50 50");
  // + x, - y, plotlineLow
  lineInstructionsBuffer[4] = extractLineInstruction("SL 50 50");
  lineInstructionsBuffer[5] = extractLineInstruction("SL 60 30");
  // - x, - y, plotlineHigh
  lineInstructionsBuffer[6] = extractLineInstruction("SL 50 50");
  lineInstructionsBuffer[7] = extractLineInstruction("SL 30 60");
  lineInstructionsBuffer[8] = extractLineInstruction("SL 10 10");
  lineInstructionsBuffer[9] = extractLineInstruction("SL 30 60");
}
#endif

#ifndef INTEGRATION_TESTING
void test_all_integration_tests() { }
 
#else
/*
 * Runs through all the integration tests defined in App.js.
 */
void test_all_integration_tests() {
  delay(10000);
  Serial.println("Initiating Integration Tests");
  integration_test_scenario_1();
  integration_test_scenario_2();
  integration_test_scenario_3();
  integration_test_scenario_4();
}

/*
 * Testing Scenario 1 from the Sequence Diagrams.
 */
void integration_test_scenario_1() {
  Serial.println("Starting INTEGRATION_TEST_SCENARIO_1");
  CURRENT_STATE = sWAITING; 
  for (int j = 1; j < 8; j++) {
    Serial.println("INTEGRATION_TEST_SCENARIO_1 " + String(j));
    delay(500);
    String test_msg = "";
    while (Serial.available()) {
      test_msg += char(Serial.read());
    }
    CURRENT_STATE = updateFSM(CURRENT_STATE, test_msg);
    delay(500);
  }
}

/*
 * Testing Scenario 2 from the Sequence Diagrams.
 */
void integration_test_scenario_2() {
  Serial.println("Starting INTEGRATION_TEST_SCENARIO_2");
  CURRENT_STATE = sWAITING; 
  for (int j = 1; j < 15; j++) {
    Serial.println("INTEGRATION_TEST_SCENARIO_2 " + String(j));
    delay(500);
    String test_msg = "";
    while (Serial.available()) {
      test_msg += char(Serial.read());
    }
    CURRENT_STATE = updateFSM(CURRENT_STATE, test_msg);
    delay(500);
  }
}

/*
 * Testing Scenario 3 from the Sequence Diagrams.
 */
void integration_test_scenario_3() {
  Serial.println("Starting INTEGRATION_TEST_SCENARIO_3");
  CURRENT_STATE = sWAITING; 
  for (int j = 1; j < 13; j++) {
    Serial.println("INTEGRATION_TEST_SCENARIO_3 " + String(j));
    delay(500);
    String test_msg = "";
    while (Serial.available()) {
      test_msg += char(Serial.read());
    }
    CURRENT_STATE = updateFSM(CURRENT_STATE, test_msg);
    if (j == 4) {
      // we test if this was called correctly in case 5 in App.js
      // hence we call it here after case 4
      interruptServiceRoutine();
    }
    delay(500);
  }
}

/*
 * Testing Scenario 4 from the Sequence Diagrams.
 */
void integration_test_scenario_4() {
  Serial.println("Starting INTEGRATION_TEST_SCENARIO_4");
  CURRENT_STATE = sWAITING; 
  for (int j = 1; j < 7; j++) {
    Serial.println("INTEGRATION_TEST_SCENARIO_4 " + String(j));
    delay(500);
    String test_msg = "";
    while (Serial.available()) {
      test_msg += char(Serial.read());
    }
    CURRENT_STATE = updateFSM(CURRENT_STATE, test_msg);
    if (j == 6) {
      // we stop sending messages to the Arduino after case 5 in App.js
      // so we wait for Watchdog to be tripped in case 6
      delay(20000);
    }
    delay(500);
  }
}
#endif
