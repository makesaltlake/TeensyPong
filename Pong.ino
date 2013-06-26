
// Define pins used by potentiometer knobs
#define paddle_l 16
#define paddle_r 15

// Define pins used by buttons
#define button_l 22
#define button_r 21
#define button_m 20

// Define pins used by SPI to communicate with LCD display
#define cs   10
#define dc   9
#define rst  8  // you can also connect this to the Arduino reset

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>

// Initialize the display object with the pins we chose
Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, rst);

#define ANALOG_MAX 1024
#define WIDTH 126
#define HEIGHT 158

#define PADDLE_SIZE 30

int player1_Xpos = 2,  player1_Ypos = 0, player1_score = 0;       // horizontal starting position, verticla starting position, starting score
int player2_Xpos = WIDTH-2, player2_Ypos = 0, player2_score = 0;

float ball_x, ball_y;
float ball_xdir, ball_ydir;

void draw_paddle(int x, int y, int color) {
  tft.drawFastVLine(x-1, y-PADDLE_SIZE/2+2, PADDLE_SIZE-4, color);
  tft.drawFastVLine(x+0, y-PADDLE_SIZE/2, PADDLE_SIZE, color);
  tft.drawFastVLine(x+1, y-PADDLE_SIZE/2+2, PADDLE_SIZE-4, color);
}

void draw_ball(int x, int y, int color) {
  tft.fillCircle(x, y, 4, color);
}

void draw_score(int player, int value, int color) {
  tft.setTextColor(color);
  if (player == 1)
    tft.setCursor(7, 0);
  if (player == 2)
    tft.setCursor(WIDTH-10, 0);
  tft.print(value);
}

void reset_ball() {
  ball_x = WIDTH/2;    // center the ball x
  ball_y = HEIGHT/2;   // center the ball y
  ball_xdir = 2.5;     // set the speed to a baseline 2.5
  if(random(2) == 0) ball_xdir *= -1;  // randomizes directon toward the left or right
  ball_ydir = random(0, 5)/2;  // randomizes the up or down movement
}

void setup(void) {
  randomSeed(analogRead(0));
  
  pinMode(button_l, INPUT);
  pinMode(button_r, INPUT);
  pinMode(button_m, INPUT);
  
  tft.initR(INITR_REDTAB);   // alternative: INITR_GREENTAB
  tft.fillScreen(ST7735_BLACK);  // sets background color
  delay(500);

  tft.setTextSize(1);
  
  reset_ball();  // centers ball on screen and sets direction randomly
}

void loop() {
  draw_ball(ball_x, ball_y, ST7735_BLACK);   // draw ball (x point, y point, color)
  draw_paddle(player1_Xpos, player1_Ypos, ST7735_BLACK); // draw paddle (height, left or right player, replacement background)
  draw_paddle(player2_Xpos, player2_Ypos, ST7735_BLACK); // draw paddle (height, left or right player, replacement background)

  draw_score(1, player1_score, ST7735_BLACK);  // draw score (player number, player score, replacement background)  // overwriting old score

  player1_Ypos = HEIGHT - (analogRead(paddle_l) * HEIGHT / ANALOG_MAX);  // read left potentiometer position, based on height set player 1 position
  player2_Ypos = HEIGHT - (analogRead(paddle_r) * HEIGHT / ANALOG_MAX);  // read right potentiometer position, based on height set player 2 position

  // If ball goes to top or bottom, just bounce
  if (ball_y < 4 || ball_y > HEIGHT-4) {
    ball_ydir *= -1;
  }

  // If ball hits player 1's paddle, bounce
  if (ball_x > 4 && ball_x < 8 &&
      ball_y > (player1_Ypos - PADDLE_SIZE/2) &&
      ball_y < (player1_Ypos + PADDLE_SIZE/2)) {
    ball_xdir *= -1;
    ball_ydir += (ball_y - player1_Ypos) / PADDLE_SIZE * 2.5;
  }
  
  ball_x += ball_xdir;
  ball_y += ball_ydir;
  // If ball gets behind player, it's gone
  if (ball_x > WIDTH) {
    player1_score++;
    ball_xdir *= -1;
  }
  if (ball_x < 0) {
    player1_score++;
    reset_ball();
  }
    
  draw_ball(ball_x, ball_y, ST7735_BLUE);
  draw_paddle(player1_Xpos, player1_Ypos, ST7735_WHITE); // paddle height, player side and potentiometer position, paddle color
  draw_paddle(player2_Xpos, player2_Ypos, ST7735_WHITE);
  
  draw_score(1, player1_score, ST7735_RED);
  
  delay(30);
}
