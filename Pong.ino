// Define pins used by potentiometer knobs
#define paddle_l 16
#define paddle_r 15

// Define pins used by buttons
#define button_l 22
#define button_r 21
#define button_m 20

// Define pins used by SPI to communicate with LCD display
#define cs   10  // chip select
#define dc   9  // 
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

int player1_pos = 0, player1_score = 0;

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
  ball_x = WIDTH/2;
  ball_y = HEIGHT/2;
  ball_xdir = 2.5;
  if(random(2) == 0) ball_xdir *= -1;
  ball_ydir = random(0, 5)/2;
}

void setup(void) {
  randomSeed(analogRead(0));
  
  pinMode(button_l, INPUT);
  pinMode(button_r, INPUT);
  pinMode(button_m, INPUT);
  
  tft.initR(INITR_REDTAB);   // alternative: INITR_GREENTAB
  tft.fillScreen(ST7735_BLACK);
  delay(500);

  tft.setTextSize(1);
  
  reset_ball();
}

void loop() {
  draw_ball(ball_x, ball_y, ST7735_BLACK);
  draw_paddle(4, player1_pos, ST7735_BLACK);

  draw_score(1, player1_score, ST7735_BLACK);

  player1_pos = HEIGHT - (analogRead(paddle_l) * HEIGHT / ANALOG_MAX);

  // If ball goes to top or bottom, just bounce
  if (ball_y < 4 || ball_y > HEIGHT-4) {
    ball_ydir *= -1;
  }

  // If ball hits player 1's paddle, bounce
  if (ball_x > 4 && ball_x < 8 &&
      ball_y > (player1_pos - PADDLE_SIZE/2) &&
      ball_y < (player1_pos + PADDLE_SIZE/2)) {
    ball_xdir *= -1;
    ball_ydir += (ball_y - player1_pos) / PADDLE_SIZE * 2.5;
  }
  
  ball_x += ball_xdir;
  ball_y += ball_ydir;
  // If ball gets behind player, it's gone
  if (ball_x > WIDTH) {
    player1_score++;
    reset_ball();
  }
    
  draw_ball(ball_x, ball_y, ST7735_BLUE);
  draw_paddle(4, player1_pos, ST7735_WHITE);
  
  draw_score(1, player1_score, ST7735_RED);
  
  delay(30);
}
