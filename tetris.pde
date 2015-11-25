import processing.serial.*;
Serial myPort;
String val;

int score;
int lines;
int[][] board;
int gridWidth;
int gridHeight;
int cellSize = 25;
boolean usedSave = false;
Tetromino activeshape;
Tetromino pendingshape;
Tetromino savedshape;


void setup()
{
 noStroke();
 myPort = new Serial(this, "COM00", 9600);
 size(250, 500);
 // set grid size
 gridWidth = floor(width/cellSize);
 gridHeight = floor(height/cellSize);
 // instigate the array
 board = new int[gridWidth][gridHeight];
 reset();
}


class Tetromino {
  int rotation;
  int type;
  int[][] shape;
  public int x, y;
  
  int[][][] shapes = 
        {{{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}},
        {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
        {{0, 0, 0, 0}, {0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}},
        {{0, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 1, 1, 0}},
        {{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 0}},
        {{0, 0, 0, 0}, {0, 0, 1, 0}, {0, 1, 1, 0}, {0, 1, 0, 0}},
        {{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 0}}};
   
   void rotate() {
    int[][] new_shape = {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}};
    for (int j = 0; j < 4; j++){
      for (int i = 0; i < 4; i++) {
        new_shape[i][j] = this.shape[4 - j - 1][i];
      }
    }
    this.shape = new_shape;
   }
   
   void initialize() {
     int newblock = (int) floor(random(shapes.length));
     int rotations = floor(4);
     this.type = newblock;
     this.shape = this.shapes[newblock];
     for (int i = 0; i < rotations; i++) {
       this.rotate();  
     }
  }
  Tetromino() {
   rotation = this.rotation;
   shape = this.shape;
  }
  
  public int left() {
    for (int x = 0; x < 4; x++) {
      for (int y = 0; y < 4; y++){
        if (this.shape[y][x] == 1) {
          return x;  
        }
      }
    }
    return 0;
  }
  public int right() {
    for (int x = 3; x >= 0; x--){
      for (int y = 0; y < 4; y++){
        if (this.shape[y][x] == 1) {
            return x;
        }
      }
    }
  return 0;
  }
  public int bottom() {
    for (int y = 3; y >= 0; y--){
      for (int x = 0; x < 4; x++){
        if (this.shape[y][x] == 1) {
            return y;
        }
      }
    }
  return 0;
  }
}

void reset(){
  savedshape = null;
  myPort.write (-1);
  for (int x = 0; x < gridWidth; x ++) {
    for (int y = 0; y < gridHeight; y ++) {
      board[x][y] = 0;
    }
  }
  score = 0;
  lines = 0;
  pendingshape = new Tetromino();
  pendingshape.initialize();
  addshape();
}

void save() {
  if (savedshape == null) {
   savedshape = activeshape;
   addshape();
  } else {
      Tetromino t = savedshape;
      savedshape = activeshape;
      activeshape = t;
      t.x = gridWidth / 2 - 2;
      t.y = 0;
  }
  myPort.write (savedshape.type);
}
  
void addshape() {
  activeshape = pendingshape;
  activeshape.x = gridWidth / 2 - 2;
  activeshape.y = 0;
  pendingshape = new Tetromino();
  pendingshape.initialize();
  myPort.write (pendingshape.type);
  //if (iscollision(activeshape)){
  //  reset();
  //}
  if (checkCollision(activeshape)) {
    reset();
  }
}

boolean iscollision(Tetromino shape) {
  for (int y = 0; y < 4; y ++) {
    for (int x = 0; x < 4; x ++) {
      if (y + shape.y < 0) {
        continue;
      }
      if (shape.shape[x][y] == 1 && board[x + shape.x][y + shape.y] == 1) return true;
    }
  }
  return false;
}
 
boolean checkCollision (Tetromino shape) {
  int newR = shape.x;
  int newC = shape.y;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (shape.shape[i][j] == 1) {
        if (newR + i >= 0 && newR + i < gridWidth && newC + j >= 0 && newC + j < gridHeight) {
          if (board[newR+i][newC + j] == 1) return true;
        } 
        else {
          return true;
        }
      }
    }
  }
  return false;
}

 
Tetromino clone(Tetromino Object) {
   Tetromino s = new Tetromino();
   s.shape = Object.shape;
   s.x = Object.x;
   s.y = Object.y;
   return s;
}

boolean rotateShape() {
  Tetromino rotated_shape = clone(activeshape);
  rotated_shape.rotate();
  if (rotated_shape.left() + rotated_shape.x < 0) {
   return false;
  }
  else if (rotated_shape.right() + rotated_shape.x >= gridWidth) {
   return false;
  }
  if (rotated_shape.bottom() + rotated_shape.y > gridHeight) {
   return false;
  }
  if (!checkCollision(rotated_shape)) {
    activeshape = rotated_shape;
  }
  return true;
};

boolean outOfBounds() {
  if (activeshape.x + activeshape.left() < 0){
   return true;   
  }
  else if (activeshape.x + activeshape.right() >= gridWidth){
    return true;  
  }
  return false;
};

boolean checkBottom() {
  return (activeshape.y + activeshape.bottom() >= gridHeight);
}

boolean lineTest() {
  for (int y = gridHeight - 1; y >= 0; y--) {
    int count = 0;
    for (int x = 0; x < gridWidth; x++) {
      if (board[x][y] == 1) {
        count++;
      }
    }
    if (count == gridWidth) {
      processLine(y);
      return true;
    }
  }
  return false;
}

void processLine(int lines) {
  lines++;
  score++;
   for (int y = lines - 1; y>= 0; y--) {
     for (int x = 0; x < gridWidth; x++) {
       board[y + 1][x] = board[y][x];
     }
   }
};

void shiftDown(int row) {
  for (int i = row; i > 0; i --) {
   for (int j = 0; j < gridWidth; j ++) {
     board[j][i] = board[j][i-1];
   }
  }
  for (int k = 0; k < gridWidth; k ++) {
    board[k][0] = 0;
  }
}

void checkLines() {
  for (int i = 0; i < gridHeight; i ++) {
    boolean good = true;
    for (int j = 0; j < gridWidth; j++) {
      if (board[j][i] == 0) good = false;
    }
    if (good) shiftDown(i);
  }
}

int shapeFall() {
 int lines = 0;
 for (int x = 0; x < 4; x ++) {
   for (int y = 0; y < 4; y ++) {
     int a = x + activeshape.x;
     int b = y + activeshape.y;
     if (a < 0 || a >= gridWidth || b < 0 || b >= gridHeight) {
       continue;
     }
     if (activeshape.shape[x][y] == 1) {
       board[a][b] = 1;
     }
   }
 }
 checkLines();
 return lines;
}

void updateBoard() {
  moveDown();
}

void drawSquare(int x, int y) {
 rectMode(CORNER);
 int originx = x * cellSize;
 int originy = y * cellSize;
 fill(255);
 rect(originx, originy, cellSize, cellSize);
}

boolean moveRight() {
  activeshape.x++;
  if (checkCollision(activeshape)) {
    activeshape.x--;
    return false;
  }
  return true;
}

boolean moveLeft() {
  activeshape.x--;
  if (checkCollision(activeshape)) {
    activeshape.x++;
    return false;
  }
  return true;
}

boolean moveDown() {
  activeshape.y++;
  if (checkCollision(activeshape)) {
    activeshape.y--;
    shapeFall();
    addshape();
    usedSave = false; 
    return false;
  }
  return true;
}

void displayGrid() {
 // cycle through the grid and draw the true values
 for (int x = 0; x < gridWidth; x++) {
   for (int y = 0; y < gridHeight; y++) {
     if (board[x][y] == 1) {
       drawSquare(x, y);
     }
   }  
 }
 for (int x = activeshape.x; x < activeshape.x + 4; x ++) {
   for (int y = activeshape.y; y < activeshape.y + 4; y ++) {
     if (activeshape.shape[x - activeshape.x][y - activeshape.y] == 1) {
       drawSquare(x,y);
     }
   }
 }
}
int counter = 0;



void draw() {
 background(0);
 frameRate(30);
 int slowness = 10;
 counter++;
 if (counter%slowness == 0) {
    updateBoard();
 }
 if (counter > 10) counter = 0;
 displayGrid();
}

void keyReleased() {
  if (keyCode == DOWN) {
    moveDown();
  }
  if (keyCode == LEFT) {
    moveLeft();
  }
  if (keyCode == RIGHT) {
    moveRight();
  }
  if (keyCode == UP)
    rotateShape();
  if (key == 'c') {
    if(!usedSave) {
      save();
      usedSave = true;
    }
  }
}

void keyPressed() {
  if (keyCode == DOWN) {
    moveDown();
  }
}

void serialEvent() {
  int method = int(myPort.readStringUntil('\n'));
  if (method == 0) moveLeft();
  if (method == 1) moveRight();
  if (method == 2) rotateShape();
}