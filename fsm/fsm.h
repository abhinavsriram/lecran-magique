#include <Stepper.h>

/*
 * Declare stepper motors.
 */
const int stepsPerRevolution = 30;
Stepper horzStepper(stepsPerRevolution, 2, 3, 4, 5);
Stepper vertStepper(stepsPerRevolution, 6, 7, 8, 9);

/*
 * Define enum for FSM states.
 */
typedef enum {
  sWAITING = 1,
  sRESET = 2,
  sWAIT_TO_CLEAR = 3,
  sCLEARED = 4,
  sDRAWING = 5,
  sREQ_INSTR = 6,
} state;

/*
 * Define struct for a line instruction.
 */
typedef struct {
  int xCoord;
  int yCoord;
} lineInstruction;

/*
 * Declare variable to keep track of input to FSM.
 */
String msg;

/*
 * I/O pins.
 */
int BUTTON_PIN = 0;
int LED_1_PIN = 1;
int LED_2_PIN = 10;

/*
 * PWM progress bar LED variables.
 */
int LED_1_PWM = 0;
int LED_2_PWM = 0;
int phaseSize;
int incrementSize;

/*
 * Setup functions.
 */
void initializeSystem();

/*
 * ISR.
 */
void interruptServiceRoutine();

/*
 * Watchdog helper functions.
 */
void enableWatchdog();
void disableWatchdog();
void WDT_Handler();

/*
 * LED progress bar helper function.
 */
void updateProgressBar();

/*
 * System reset helper function.
 */
void reset();

/*
 * Line plotting helper functions.
 */
lineInstruction extractLineInstruction(String msg);
void plotLine(int x0, int y0, int x1, int y1);
void plotLineLow(int x0, int y0, int x1, int y1);
void plotLineHigh(int x0, int y0, int x1, int y1);
