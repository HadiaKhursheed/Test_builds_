// Automatic playing stickman
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); // -1 if no reset pin

// I²C pin setup for NodeMCU/ESP8266 (common assignments)
#define SDA_PIN D2 // GPIO4
#define SCL_PIN D1 // GPIO5

// --- Game State Variables ---
int stickFigureX = 20;     // Initial X position of the stick figure
int stickFigureY = 0;      // Y position relative to the ground (0 means on ground)
int groundLevel = SCREEN_HEIGHT - 10; // Y coordinate of the ground line

float velocityY = 0;       // Vertical velocity for jumping
bool isJumping = false;    // True if the stick figure is currently jumping

int runFrame = 0;          // Used to cycle through running animation frames (0 or 1)
unsigned long lastAnimationUpdateTime = 0; // To control animation speed
const unsigned long ANIMATION_FRAME_DELAY = 100; // Milliseconds per animation frame

int obstacleX = SCREEN_WIDTH; // X position of the obstacle
int obstacleWidth = 10;
int obstacleHeight = 15;
const int OBSTACLE_SPEED = 2; // Pixels per frame the obstacle moves

long score = 0;             // Player score
bool gameOver = false;      // Game over state

// --- Function Prototypes ---
void drawStickFigure(int x, int y, int frame, bool jumping);
void drawGround();
void drawObstacle();
void drawScore();
void handleJumpLogic();
void handleObstacleLogic();
void resetGame();

void setup() {
  // Initialize Serial communication for debugging
  Serial.begin(115200);
  Serial.println("Starting Stick Figure Runner...");

  // Initialize I2C communication
  Wire.begin(SDA_PIN, SCL_PIN);

  // Initialize OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  Serial.println("OLED display initialized.");

  // Clear display and show initial message (optional)
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Loading...");
  display.display();
  delay(1000); // Give user time to see it

  resetGame(); // Set initial game state
  Serial.println("Game reset.");
}

void loop() {
  if (!gameOver) {
    // --- Update Game State ---
    handleJumpLogic();
    handleObstacleLogic();

    // Update running animation frame
    if (millis() - lastAnimationUpdateTime > ANIMATION_FRAME_DELAY) {
      if (!isJumping) { // Only animate running when not jumping
        runFrame = 1 - runFrame; // Toggle between 0 and 1
      }
      lastAnimationUpdateTime = millis();
    }

    // --- Drawing ---
    display.clearDisplay(); // Clear entire screen

    drawGround();
    drawObstacle();
    drawStickFigure(stickFigureX, groundLevel - stickFigureY, runFrame, isJumping);
    drawScore();

    display.display(); // Show updated buffer on screen

    // --- Debugging Output (Optional: Comment out for performance once working) ---
    // Serial.print("SF_Y: "); Serial.print(stickFigureY);
    // Serial.print(", VelY: "); Serial.print(velocityY);
    // Serial.print(", isJumping: "); Serial.print(isJumping ? "TRUE" : "FALSE");
    // Serial.print(", Obst_X: "); Serial.print(obstacleX);
    // Serial.print(", Obst_H: "); Serial.print(obstacleHeight);
    // Serial.print(", Score: "); Serial.println(score);

  } else {
    // Game Over screen
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(20, 15);
    display.println("GAME OVER");
    display.setTextSize(1);
    display.setCursor(30, 40);
    display.print("Score: ");
    display.println(score);
    display.display();

    // You might want to add a button press to reset the game here
    // For now, it just stays on the game over screen.
    // delay(3000); // Optional: stay on game over screen for a few seconds
    // resetGame(); // Optional: automatically restart after a delay
  }
}

// --- Drawing Functions ---
void drawStickFigure(int x, int y, int frame, bool jumping) {
  // y is the base of the stick figure (feet level)
  // frame for running animation: 0 or 1

  // Head
  display.drawCircle(x, y - 20, 4, WHITE);

  // Body
  display.drawLine(x, y - 16, x, y - 5, WHITE); // Body

  if (jumping) {
    // Jump pose arms (upwards)
    display.drawLine(x, y - 12, x - 5, y - 18, WHITE); // Left arm
    display.drawLine(x, y - 12, x + 5, y - 18, WHITE); // Right arm
    // Jump pose legs (bent)
    display.drawLine(x, y - 5, x - 3, y + 2, WHITE); // Left leg (bent)
    display.drawLine(x, y - 5, x + 3, y + 2, WHITE); // Right leg (bent)
  } else {
    // Running pose arms
    if (frame == 0) { // Arm back, other forward
      display.drawLine(x, y - 12, x - 6, y - 8, WHITE);  // Left arm (back)
      display.drawLine(x, y - 12, x + 6, y - 16, WHITE); // Right arm (forward)
    } else { // Opposite arm positions
      display.drawLine(x, y - 12, x - 6, y - 16, WHITE); // Left arm (forward)
      display.drawLine(x, y - 12, x + 6, y - 8, WHITE);  // Right arm (back)
    }
    // Running pose legs
    if (frame == 0) { // One leg forward, other back
      display.drawLine(x, y - 5, x - 4, y + 2, WHITE); // Left leg (back)
      display.drawLine(x, y - 5, x + 4, y + 5, WHITE); // Right leg (forward)
    } else { // Opposite leg positions
      display.drawLine(x, y - 5, x - 4, y + 5, WHITE); // Left leg (forward)
      display.drawLine(x, y - 5, x + 4, y + 2, WHITE); // Right leg (back)
    }
  }
}

void drawGround() {
  display.drawLine(0, groundLevel, SCREEN_WIDTH - 1, groundLevel, WHITE);
}

void drawObstacle() {
  display.fillRect(obstacleX, groundLevel - obstacleHeight, obstacleWidth, obstacleHeight, WHITE);
}

void drawScore() {
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(SCREEN_WIDTH - 50, 5); // Set cursor position
  display.print("SCORE: ");
  display.println(score);
}

// --- Game Logic Functions ---
void handleJumpLogic() {
  const float JUMP_VELOCITY = 4.5; // Initial upward velocity
  const float GRAVITY = 0.2;       // Downward acceleration

  if (isJumping) {
    stickFigureY += velocityY; // Apply current vertical velocity
    velocityY -= GRAVITY;      // Apply gravity (reduce upward velocity, then increase downward)

    // Land on ground
    if (stickFigureY <= 0) {
      stickFigureY = 0;       // Ensure it's exactly on the ground
      velocityY = 0;          // Stop vertical movement
      isJumping = false;      // Not jumping anymore
    }
  } else {
    // Auto-jump condition (tuned for better responsiveness)
    // Trigger jump if:
    // 1. Stick figure is on the ground (stickFigureY == 0)
    // 2. Obstacle's left edge is between stickFigureX - 10 (slightly past figure)
    //    and stickFigureX + 25 (well ahead of figure)
    // This provides a wider window for the jump to trigger.
    if (stickFigureY == 0 &&
        obstacleX < (stickFigureX + 25) && // Obstacle is approaching from the right, not too close yet
        obstacleX > (stickFigureX - 10)) { // Obstacle hasn't completely passed the figure
        isJumping = true;
        velocityY = JUMP_VELOCITY;
        // Serial.println("AUTO JUMP TRIGGERED!"); // Debugging
    }
  }
}

void handleObstacleLogic() {
  obstacleX -= OBSTACLE_SPEED; // Move obstacle to the left

  // If obstacle goes off screen, reset it and increase score
  if (obstacleX + obstacleWidth < 0) {
    obstacleX = SCREEN_WIDTH + random(50, 150); // New obstacle position, random gap
    obstacleHeight = random(10, 25); // Randomize obstacle height a bit
    score++; // Increment score for passing an obstacle
    // Serial.print("Obstacle cleared! New score: "); Serial.println(score); // Debugging
  }

  // Collision detection:
  // Check if stick figure is horizontally overlapping with obstacle
  // stickFigureX is center, so width is ~10 pixels (x-5 to x+5)
  bool horizontalOverlap = (stickFigureX + 5 > obstacleX) && (stickFigureX - 5 < obstacleX + obstacleWidth);
  // Check if stick figure is vertically overlapping (i.e., not high enough to clear it)
  // stickFigureY is height *above* ground. If stickFigureY is LESS THAN obstacleHeight, there's overlap.
  bool verticalOverlap = (stickFigureY < obstacleHeight);

  if (horizontalOverlap && verticalOverlap) {
    if (!gameOver) { // Only print "Collision!" once per game over
      Serial.println("COLLISION DETECTED! Game Over.");
    }
    gameOver = true; // Collision detected!
  }
}

void resetGame() {
  stickFigureX = 20;
  stickFigureY = 0;
  velocityY = 0;
  isJumping = false;
  runFrame = 0;
  obstacleX = SCREEN_WIDTH;
  obstacleWidth = 10;
  obstacleHeight = 15;
  score = 0;
  gameOver = false;
  lastAnimationUpdateTime = millis(); // Reset timer
  randomSeed(analogRead(0)); // Good practice for random numbers
}
