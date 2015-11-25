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
int current_state[4] = {0,0,0,0};
int old_state[4] = {0,0,0,0};
long lBtn1, lBtn2; // buttons for the orbit
bool fClearOled;

// OLED Screen Variables
int x, y; // coordinates for displaying on OLED
int squareWidth = 13;
int squareHeight = 13;
int shift = 12;

char SQUARE[] = {
  0xFF, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xFF, 
  0xFF, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0xFF};

char SEPERATOR[] = {
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void setup() {
  Serial.begin(9600); 
  launchpadInit();
  orbitInit();
}

void orbitInit() {
  GPIOPadConfigSet(BTN1Port, BTN1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
  GPIOPadConfigSet(BTN2Port, BTN2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
  GPIOPinTypeGPIOInput(BTN1Port, BTN1);
  GPIOPinTypeGPIOInput(BTN2Port, BTN2);
  
  OrbitOledInit();

  //chSwtCur = 0;
  //chSwtPrev = 0;
  fClearOled = true;
}

void launchpadInit() {
  pinMode(PUSH1, INPUT_PULLUP);  // left
  pinMode(PUSH2, INPUT_PULLUP);  // right
}

void buttonSend(int button, int i){
  if (button && !old_state[i]){
    current_state[i] = 1;
    Serial.println(i);
  } else if (!button) {
    current_state[i] = 0;
  }
  old_state[i] = current_state[i];
}

int isBTN1() { // return if BTN1 of orbit is pressed
  lBtn1 = GPIOPinRead(BTN1Port, BTN1);
  return lBtn1 == BTN1;
}

int isBTN2() { // return if BTN2 of orbit is pressed
  lBtn2 = GPIOPinRead(BTN2Port, BTN2);
  return lBtn2 == BTN2;
}

void draw4x1(int position) {
if (position) {
    x = 6, y = 10;
  } else {
    x = 73, y = 10;
  }

  OrbitOledMoveTo(x, y);
  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE); // block 1

  OrbitOledMoveTo(shift + x, 0 + y); // block 2
  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE); // block 1

  OrbitOledMoveTo(2*shift + x, 0 + y); // blk 3
  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE);

  OrbitOledMoveTo(3*shift + x, 0 + y); // blk4
  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE);

  OrbitOledUpdate();
}


void drawL1(int position) {
  if (position) {
    x = 13, y = 6;
  } else {
    x = 84, y = 6;
  }

  OrbitOledMoveTo(x, y);
  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE); // block 1

  OrbitOledMoveTo(x, shift + y); // block 2
  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE); // block 1

  OrbitOledMoveTo(shift + x, shift + y); // blk 3
  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE);

  OrbitOledMoveTo(2*shift + x, shift + y); // blk4
  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE);

  OrbitOledUpdate();
}

void drawL2(int position) {
  if (position) {
    x = 13, y = 6;
  } else {
    x = 84, y = 6;
  }

  OrbitOledMoveTo(x, y);
  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE); // block 1

  OrbitOledMoveTo(x + shift, y); // block 2
  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE); // block 1

  OrbitOledMoveTo(x + 2*shift, y); // blk 3
  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE);

  OrbitOledMoveTo(x, y + shift); // blk4
  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE);

  OrbitOledUpdate();
}

void drawZ1(int position) {
  if (position) {
    x = 13, y = 6;
  } else {
    x = 84, y = 6;
  }

  OrbitOledMoveTo(x + shift, y);
  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE); // block 1

  OrbitOledMoveTo(x + shift, y + shift); // block 2
  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE); // block 1

  OrbitOledMoveTo(x + 2*shift, y); // blk 3
  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE);

  OrbitOledMoveTo(x, y + shift); // blk4
  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE);

  OrbitOledUpdate();
}

void drawZ2(int position) {
  if (position) {
    x = 13, y = 6;
  } else {
    x = 84, y = 6;
  }

  OrbitOledMoveTo(x, y);
  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE); // block 1

  OrbitOledMoveTo(x + shift, y + shift); // block 2
  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE); // block 1

  OrbitOledMoveTo(x + 2*shift, y + shift); // blk 3
  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE);

  OrbitOledMoveTo(x + shift, y); // blk4
  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE);

  OrbitOledUpdate();
}

/**
*
* Takes in int input 0 for HELD and 1 for NEXT_PIECE
*/
void draw2x2(int position) {
  if (position) {
    x = 13, y = 6;
  } else {
    x = 84, y = 6;
  }

  OrbitOledMoveTo(x, y);
  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE); // block 1

  OrbitOledMoveTo(shift + x, 0 + y); // block 2
  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE); // block 1

  OrbitOledMoveTo(0 + x, shift + y); // blk 3
  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE);

  OrbitOledMoveTo(shift + x, shift + y); // blk4
  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE);

  OrbitOledUpdate();
}

void drawT(int position) {
  if (position) {
    x = 13, y = 6;
  } else {
    x = 84, y = 6;
  }

  OrbitOledMoveTo(x, y);
  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE); // block 1

  OrbitOledMoveTo(shift + x, 0 + y); // block 2
  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE); // block 1

  OrbitOledMoveTo(2*shift + x, 0 + y); // blk 3
  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE);

  OrbitOledMoveTo(shift + x, shift + y); // blk4
  OrbitOledPutBmp(squareWidth, squareHeight, SQUARE);

  OrbitOledUpdate();
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
  if (Serial.available() > 0) {
    if (Serial.read() == 40)
    OrbitOledPutString("N");
  }
  OrbitOledUpdate();
}


void loop() {
  // send data only when you receive data:
	if (Serial.available() > 0) {
              byte in = Serial.read();
      
		switch(in%10) {
                  case 0: // straight line
                    draw4x1(in/10);
                    break;
                  case 1: // sq
                  
                    break;
                  case 2: // T
                  
                    break; 
                  case 3: // L1
                    break;
                    
                  case 4: // L2
                    break;
                  
                  case 5: // Z1
                    break;
                    
                  case 6:
                    break
                }
	}
  drawFrame();
  int leftButton = !digitalRead(PUSH1);
  int rightButton = !digitalRead(PUSH2);

  buttonSend(leftButton, 0);
  buttonSend(rightButton, 1);
  buttonSend(isBTN1(), 2);
  buttonSend(isBTN2(), 3);
  delay(20);
}
