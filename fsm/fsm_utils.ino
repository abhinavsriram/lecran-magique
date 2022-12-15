/*
 * void initializeSystem() initializes the stepper motors, the stop 
 * button that triggers the ISR, and the progress bar LEDs.
 */
void initializeSystem() {
  horzStepper.setSpeed(500);
  vertStepper.setSpeed(500);
  pinMode(LED_1_PIN, OUTPUT);
  pinMode(LED_2_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), interruptServiceRoutine, RISING);
}

/*
 * void initializeStateVariables() initializes all the variables
 * needed for the FSM.
 */
void initializeStateVariables() {
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

/*
 * void updateProgressBar() updates the luminosity of the progress bar 
 * LEDs using PWM. The first LED lights up with increasing luminosity up 
 * to the first 50% of drawing progress, after which the second LED starts 
 * lighting up. The LEDs flash 5 times after a drawing is complete. 
 * phaseSize is set to half the total number of lines to be drawn and 
 * incrementSize is set to 255 over phaseSize so that the increments to 
 * luminosity are uniform.
 */
void updateProgressBar() {
  if (totalLinesProcessed <= phaseSize) {
    analogWrite(LED_1_PIN, LED_1_PWM);
    LED_1_PWM += incrementSize;
  } else if (totalLinesProcessed < phaseSize * 2) {
    analogWrite(LED_2_PIN, LED_2_PWM);
    LED_2_PWM += incrementSize;
  } else if 
    (
    (totalLinesProcessed % 2 == 0 && totalLinesProcessed >= (phaseSize * 2)) || 
    (totalLinesProcessed % 2 != 0 && totalLinesProcessed > (phaseSize * 2))
    ) {
    for (int i = 0; i < 5; i += 1) {
      analogWrite(LED_1_PIN, 0);
      analogWrite(LED_2_PIN, 0);
      delay(500);
      analogWrite(LED_1_PIN, 255);
      analogWrite(LED_2_PIN, 255);
      delay(500);
    }
   }
}

/*
 * void interruptServiceRoutine() is called when the ISR is trigerred by 
 * pressing the button on the circuit. It only takes effect when the system is
 * in the sDRAWING or sREQ_INSTR states.
 */
void interruptServiceRoutine() {
  if (CURRENT_STATE == sDRAWING || CURRENT_STATE == sREQ_INSTR) {
    disableWatchdog();
    Serial.println("AS ");
    CURRENT_STATE = sWAITING; 
  }
}


/*
 * void enableWatchdog() sets necessary bits in specific registers 
 * to enable the Watchdog. The Watchdog is only enabled when the 
 * Arduino is receiving line instructions from the web server.
 */
void enableWatchdog() {
  NVIC_DisableIRQ(WDT_IRQn);
  NVIC_ClearPendingIRQ(WDT_IRQn);
  NVIC_SetPriority(WDT_IRQn, 0);
  NVIC_EnableIRQ(WDT_IRQn);
  // configure and enable WDT GCLK
  GCLK->GENDIV.reg = GCLK_GENDIV_DIV(4) | GCLK_GENDIV_ID(5);
  while (GCLK->STATUS.bit.SYNCBUSY);
  GCLK->GENCTRL.reg = GCLK_GENCTRL_DIVSEL | GCLK_GENCTRL_ID(5) | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC(3);
  while (GCLK->STATUS.bit.SYNCBUSY);
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_GEN(5) | GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_ID(3);
  // configure and enable WDT
  WDT->EWCTRL.reg = WDT_EWCTRL_EWOFFSET(9);
  WDT->CONFIG.reg = WDT_CONFIG_PER(11);
  WDT->CTRL.reg = WDT_CTRL_ENABLE;
  while (WDT->STATUS.bit.SYNCBUSY);
  // enable early warning interrupts on WDT:
  WDT->INTENSET.reg = WDT_INTENSET_EW;
}

/*
 * void disableWatchdog() sets necessary bits in specific registers 
 * to disable the Watchdog. The Watchdog is disabled upon returning
 * to the sWAITING state or upon the ISR being triggered.
 */
void disableWatchdog() {
  WDT->CTRL.reg = 0;
}

/*
 * void WDT_Handler() provides early warning when Watchdog reset 
 * is imminent.
 */
void WDT_Handler() {
  WDT->INTFLAG.bit.EW = 1;
}

/*
 * void resetSystem() resets all aspects of the system.
 */
void resetSystem() {
  // reset cursor to start point
  plotLine(cursorX, cursorY, 0, 0);
  // reset all vars for progress bar LEDs
  LED_1_PWM = 0;
  LED_2_PWM = 0;
  phaseSize = 0;
  incrementSize = 0;
  analogWrite(LED_1_PIN, LED_1_PWM);
  analogWrite(LED_2_PIN, LED_2_PWM);
  // reset all FSM vars
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

/*
 * lineInstruction extractLineInstruction(String msg) produces a
 * lineInstruction struct given a message of the format "SL x y".
 */
lineInstruction extractLineInstruction(String msg) {
  int spaceDelim1 = msg.indexOf(" ");
  int spaceDelim2 = msg.lastIndexOf(" ");
  int xCoord = msg.substring(spaceDelim1 + 1, spaceDelim2).toInt();
  int yCoord = msg.substring(spaceDelim2).toInt();
  return (lineInstruction) {xCoord, yCoord};
}

/*
 * void plotLineLow(int x0, int y0, int x1, int y1) applies the Bresenham
 * algorithm for a gradual slope to find the points that approximate a 
 * straight line from (x0, y0) to (x1, y1) and calls on the motors to move the 
 * knobs in that direction. It also pets the Watchdog after spinning a motor.
 */
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
    if (hasYChanged) {
      vertStepper.step(-stepsPerRevolution * yi);
      WDT->CLEAR.reg = 0xA5;
      delay(5);
    }
    if (x != x0) {
      horzStepper.step(stepsPerRevolution * xi);
      WDT->CLEAR.reg = 0xA5;
      delay(5);
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

/*
 * void plotLineHigh(int x0, int y0, int x1, int y1) applies the Bresenham
 * algorithm for a steep slope to find the points that approximate a 
 * straight line from (x0, y0) to (x1, y1) and calls on the motors to move the 
 * knobs in that direction. It also pets the Watchdog after spinning a motor.
 */
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
    if (hasXChanged) {
      horzStepper.step(stepsPerRevolution * xi);
      WDT->CLEAR.reg = 0xA5;
      delay(5);
    }
    if (y != y0) {
      vertStepper.step(-stepsPerRevolution * yi);
      WDT->CLEAR.reg = 0xA5;
      delay(5);
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

/*
 * void plotLine(int x0, int y0, int x1, int y1) makes the appropriate call
 * to plotLineLow(x0, y0, x1, y1) or plotLineHigh(x0, y0, x1, y1) depending
 * on whether the slope is gradual or steep. It then updates the cursor's 
 * position to that of (x1, y1).
 */
void plotLine(int x0, int y0, int x1, int y1) {
  if (abs(y1 - y0) < abs(x1 - x0)) {
    plotLineLow(x0, y0, x1, y1);
  } else {
    plotLineHigh(x0, y0, x1, y1);
  }
  cursorX = x1;
  cursorY = y1;
}
