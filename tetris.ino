// ENERGIA CODE
extern "C" {
#include <delay.h>
#include <FillPat.h>
#include <I2CEEPROM.h>
#include <LaunchPad.h>
#include <OrbitBoosterPackDefs.h>
#include <OrbitOled.h>
#include <OrbitOledChar.h>
#include <OrbitOledGrph.h>
}

boolean board[20][10];
boolean curPiece[4][4];
int curR;
int curC;

char chSwtCur;
char chSwtPrev;
boolean fClearOled;

// OLED Screen Variables
int squareWidth = 13;
int squareHeight = 13;
int shift = 12;

char SQUARE[] = {
  0xFF, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xFF, 
  0xFF, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0xFF};

char SEPERATOR[] = {
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Initialize Variables
// variables will change:
int TI_LEFT_BUTTON_STATE = 0, TI_RIGHT_BUTTON_STATE = 0;   // variable for reading the pushbutton status

long lBtn1, lBtn2; // buttons for the orbit

void launchpadInit() {
  // initialize the LED pin as an output:
  pinMode(GREEN_LED, OUTPUT);      
  pinMode(RED_LED, OUTPUT);      
  // initialize the pushbutton pin as an input:
  pinMode(PUSH1, INPUT_PULLUP); // left btn
  pinMode(PUSH2, INPUT_PULLUP);  // right btn
}

void orbitInit() {
  /*
   * First, Set Up the Clock.
   * Main OSC     -> SYSCTL_OSC_MAIN
   * Runs off 16MHz clock -> SYSCTL_XTAL_16MHZ
   * Use PLL      -> SYSCTL_USE_PLL
   * Divide by 4    -> SYSCTL_SYSDIV_4
   */
  SysCtlClockSet(SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ | SYSCTL_USE_PLL | SYSCTL_SYSDIV_4);

  /*
   * Enable and Power On All GPIO Ports
   */
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  /*
   * Pad Configure.. Setting as per the Button Pullups on
   * the Launch pad (active low).. changing to pulldowns for Orbit
   */
  GPIOPadConfigSet(SWTPort, SWT1 | SWT2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);

  GPIOPadConfigSet(BTN1Port, BTN1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
  GPIOPadConfigSet(BTN2Port, BTN2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);

  GPIOPadConfigSet(LED1Port, LED1, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);
  GPIOPadConfigSet(LED2Port, LED2, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);
  GPIOPadConfigSet(LED3Port, LED3, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);
  GPIOPadConfigSet(LED4Port, LED4, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);

  /*
   * Initialize Switches as Input
   */
  GPIOPinTypeGPIOInput(SWTPort, SWT1 | SWT2);

  /*
   * Initialize Buttons as Input
   */
  GPIOPinTypeGPIOInput(BTN1Port, BTN1);
  GPIOPinTypeGPIOInput(BTN2Port, BTN2);

  /*
   * Initialize LEDs as Output
   */
  GPIOPinTypeGPIOOutput(LED1Port, LED1);
  GPIOPinTypeGPIOOutput(LED2Port, LED2);
  GPIOPinTypeGPIOOutput(LED3Port, LED3);
  GPIOPinTypeGPIOOutput(LED4Port, LED4);

  /*
   * Enable ADC Periph
   */
  SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

  GPIOPinTypeADC(AINPort, AIN);

  /*
   * Enable ADC with this Sequence
   * 1. ADCSequenceConfigure()
   * 2. ADCSequenceStepConfigure()
   * 3. ADCSequenceEnable()
   * 4. ADCProcessorTrigger();
   * 5. Wait for sample sequence ADCIntStatus();
   * 6. Read From ADC
   */
  ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
  ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH0);
  ADCSequenceEnable(ADC0_BASE, 0);

  /*
   * Initialize the OLED
   */
  OrbitOledInit();

  /*
   * Reset flags
   */
  chSwtCur = 0;
  chSwtPrev = 0;
  fClearOled = true;
}
/**
 * Purpose: Returns state of BTN1 on orbit
 * Returns: 1 if pressed, 0 if not pressed
 */
int isBTN1() { // return if BTN1 of orbit is pressed
  lBtn1 = GPIOPinRead(BTN1Port, BTN1);
  return lBtn1 == BTN1;
}

/**
 * Purpose: Returns state of BTN2 on orbit
 * Returns: 1 if pressed, 0 if not pressed
 */
int isBTN2() { // return if BTN1 of orbit is pressed
  lBtn2 = GPIOPinRead(BTN2Port, BTN2);
  return lBtn2 == BTN2;
}

/**
 * Purpose: Returns state of bottom left button on launchpad
 * Returns: 1 if pressed, 0 if not pressed
 */
int isLEFT_BTN() { // return if BTN1 of orbit is pressed
  return !digitalRead(PUSH1);
}

/**
 * Purpose: Returns state of bottom right button on launchpad
 * Returns: 1 if pressed, 0 if not pressed
 */
int isRIGHT_BTN() { // return if BTN1 of orbit is pressed
  return !digitalRead(PUSH2);
}

//void draw4x1(int position) {
//if (position) {
//    x = 6, y = 10;
//  } else {
//    x = 73, y = 10;
//  }
//
//  OrbitOledMoveTo(x, y);
//  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE); // block 1
//
//  OrbitOledMoveTo(shift + x, 0 + y); // block 2
//  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE); // block 1
//
//  OrbitOledMoveTo(2*shift + x, 0 + y); // blk 3
//  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE);
//
//  OrbitOledMoveTo(3*shift + x, 0 + y); // blk4
//  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE);
//
//  OrbitOledUpdate();
//}
//
//
//void drawL1(int position) {
//  if (position) {
//    x = 13, y = 6;
//  } else {
//    x = 84, y = 6;
//  }
//
//  OrbitOledMoveTo(x, y);
//  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE); // block 1
//
//  OrbitOledMoveTo(x, shift + y); // block 2
//  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE); // block 1
//
//  OrbitOledMoveTo(shift + x, shift + y); // blk 3
//  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE);
//
//  OrbitOledMoveTo(2*shift + x, shift + y); // blk4
//  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE);
//
//  OrbitOledUpdate();
//}
//
//void drawL2(int position) {
//  if (position) {
//    x = 13, y = 6;
//  } else {
//    x = 84, y = 6;
//  }
//
//  OrbitOledMoveTo(x, y);
//  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE); // block 1
//
//  OrbitOledMoveTo(x + shift, y); // block 2
//  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE); // block 1
//
//  OrbitOledMoveTo(x + 2*shift, y); // blk 3
//  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE);
//
//  OrbitOledMoveTo(x, y + shift); // blk4
//  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE);
//
//  OrbitOledUpdate();
//}
//
//void drawZ1(int position) {
//  if (position) {
//    x = 13, y = 6;
//  } else {
//    x = 84, y = 6;
//  }
//
//  OrbitOledMoveTo(x + shift, y);
//  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE); // block 1
//
//  OrbitOledMoveTo(x + shift, y + shift); // block 2
//  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE); // block 1
//
//  OrbitOledMoveTo(x + 2*shift, y); // blk 3
//  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE);
//
//  OrbitOledMoveTo(x, y + shift); // blk4
//  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE);
//
//  OrbitOledUpdate();
//}
//
//void drawZ2(int position) {
//  if (position) {
//    x = 13, y = 6;
//  } else {
//    x = 84, y = 6;
//  }
//
//  OrbitOledMoveTo(x, y);
//  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE); // block 1
//
//  OrbitOledMoveTo(x + shift, y + shift); // block 2
//  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE); // block 1
//
//  OrbitOledMoveTo(x + 2*shift, y + shift); // blk 3
//  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE);
//
//  OrbitOledMoveTo(x + shift, y); // blk4
//  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE);
//
//  OrbitOledUpdate();
//}
//
///**
//*
//* Takes in int input 0 for HELD and 1 for NEXT_PIECE
//*/
//void draw2x2(int position) {
//  if (position) {
//    x = 13, y = 6;
//  } else {
//    x = 84, y = 6;
//  }
//
//  OrbitOledMoveTo(x, y);
//  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE); // block 1
//
//  OrbitOledMoveTo(shift + x, 0 + y); // block 2
//  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE); // block 1
//
//  OrbitOledMoveTo(0 + x, shift + y); // blk 3
//  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE);
//
//  OrbitOledMoveTo(shift + x, shift + y); // blk4
//  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE);
//
//  OrbitOledUpdate();
//}
//
//void drawT(int position) {
//  if (position) {
//    x = 13, y = 6;
//  } else {
//    x = 84, y = 6;
//  }
//
//  OrbitOledMoveTo(x, y);
//  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE); // block 1
//
//  OrbitOledMoveTo(shift + x, 0 + y); // block 2
//  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE); // block 1
//
//  OrbitOledMoveTo(2*shift + x, 0 + y); // blk 3
//  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE);
//
//  OrbitOledMoveTo(shift + x, shift + y); // blk4
//  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE);
//
//  OrbitOledUpdate();
//}

void boardInit () {
  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 10; j++) {
      board[i][j] = false;
    }
  }
}

/**
 * Handles drawing of seperator and "H " and "N"
 */
void drawFrame() {
  OrbitOledSetCursor(0, 0);
  OrbitOledPutString("H");
  OrbitOledMoveTo(62, 0); // 128/2 centre of screen
  OrbitOledPutBmp(5, 32, SEPERATOR); 
  OrbitOledMoveTo(73, 0);
  OrbitOledPutString("N");
  OrbitOledUpdate();
}

void setup() {
  Serial.begin(9600);
  
  launchpadInit();
  orbitInit(); // initialize the orbit booster pack
  
  //setUpOutput();
  
  boardInit ();	
}

void printBoard () {
  boolean pb[20][10];
  for (int i = 0; i < 20; i ++) {
     for (int j = 0; j < 10; j++) {
         pb[20][10] = board[i][j];
  }
}
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
       if (curPiece[curR + i][curC + j]) pb[curR + i][curC + j] = true;
    } 
  }
  
  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 10; j++) {
   Serial.print (curPiece[i][j]); 
    }
    Serial.println();
  }
}

void loop() {
  while (true) {
    if (checkLanded ()) // check if the current piece can move anymore
    {
      checkFullLine ();
      if(checkGameOver()) break;
      curPiece = genNewPiece();
    }

    if (isLEFT_BTN()) {
      shiftLeft();
    } 
    else if (isRIGHT_BTN()) {
      shiftRight();
    } 
    // else if () {
    //   shiftStraightDown();
    // } 
    else if (isBTN1()) {
      rotateBlock();
    } 
    else if (isBTN2()) {
      curPiece = save();
    }
    shiftDown();
    updateDisplay();
    delay (10);
  }
}

boolean checkCollision (int newR, int newC) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (curPiece[i][j]) {
        if (newR + i >= 0 && newR + i < 20 && newC + j >= 0 && newC + j < 10) {
          if (board[newR+i][newC + j]) return false;
        } 
        else {
          return false;
        }
      }
    }
  }

  return true;
}

boolean shiftLeft () {
  if (checkCollision (curR, curC - 1)) {
    curC --;
    return true;
  }
  return false;
}

boolean shiftRight () {
  if (checkCollision (curR, curC + 1)) {
    curC ++;
    return true;
  }
  return false;
}

boolean shiftDown () {
  if (checkCollision(curR + 1, curC)) {
    curR ++;
    return true;
  } 
  return false;
}

boolean shiftStraightDown () {
  boolean b = false;
  while (shiftDown()) {
    b = true;
  }
  return b;
}


void rotateBlock(){  
  boolean longpiece[4][4] = {
    {
      0, 0, 0, 0    }
    ,
    {
      0, 0, 0 ,0    }
    ,
    {
      1, 1, 1, 1    }
    ,
    {
      0, 0, 0, 0    }
  };
  boolean tallpiece[4][4] = {
    {
      0, 1, 0, 0    }
    ,
    {
      0, 1, 0 ,0    }
    ,
    {
      0, 1, 0, 0    }
    ,
    {
      0, 1, 0, 0    }
  };
  if (curPiece == longpiece){
    curPiece = tallpiece;
  }
  else if (curPiece == tallpiece){
    curPiece = longpiece;
  }

  else{
    int i;
    boolean temp[4][4];
    for (i = 0; i < 3; i++){
      int j;
      for (j = 2; j >= 0; j--){
        temp[j][i] = curPiece[j][i];
        printf("%d\n", temp[j][i]);
      }
    }
    curPiece = temp;
  }
}

// void loop(){
//   digitalWrite(GREEN_LED, isBTN1());
//   digitalWrite(RED_LED, isBTN2());

//   if(isBTN1() || isRIGHT_BTN()) {
//     draw4x1(6,10); // 84, 6
//     //drawZ2(13,6); 
//     draw4x1(73,10); // 84, 6
//     drawFrame();
//     GPIOPinWrite(LED1Port, LED1, LED1);
//     GPIOPinWrite(LED2Port, LED2, LED2);
//   } 
//   else {
//     GPIOPinWrite(LED1Port, LED1, LOW);
//     GPIOPinWrite(LED2Port, LED2, LOW);
//   }
//   if(isBTN2()) {
//     GPIOPinWrite(LED3Port, LED3, LED3);
//     GPIOPinWrite(LED4Port, LED4, LED4);
//   } 
//   else {
//     GPIOPinWrite(LED3Port, LED3, LOW);
//     GPIOPinWrite(LED4Port, LED4, LOW);
//   }
// }

boolean checkGameOVer() {
  for(int i = 0; i < 10; i++) {
    if(board[0][i]) return true;
  }
  return false;
}
boolean checkLanded() {
  return (checkCollision(curR + 1, curC));
  
}
void checkFullLine() {
  for(int i = 0; i < 20; i++) {
    boolean isFull = true;
    for(j = 0; j < 10 && isFull; j++) {
      if(!board[i][j])
        isFull = false;
    }
    if(isFull) {
      clearLineDown(i);
    }
  }
}
void clearLineDown(int row) {
  for(int i = 1; i <= row; i++) {
    for(j = 0; j < 10; j++) {
      board[i][j] = board[i-1][j];
    }
  }
  for(int l = 0; l <= 10; l++) {
    board[0][l] = false;
  }

}




