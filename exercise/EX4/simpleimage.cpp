/* Qiufeng Du
  No: 1439484
  Section: A2
  */
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <SD.h>

#include "lcd_image.h"

#define SD_CS    5  // Chip select line for SD card
#define TFT_CS   6  // Chip select line for TFT display
#define TFT_DC   7  // Data/command line for TFT
#define TFT_RST  8  // Reset line for TFT (or connect to +5V)

#define JOY_VERT_ANALOG 0
#define JOY_HORIZ_ANALOG 1
#define JOY_SEL 9
#define JOY_DEADZONE 64
#define JOY_CENTRE 512

#define TFT_WIDTH 128
#define TFT_HEIGHT 160

#define JOY_STEPS_PER_PIXEL 64

#define MILLIS_PER_FRAME 50 // 20fps



Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

int g_cursorX = TFT_WIDTH/2; // cursor pixel position (initially at the center of the screen)
int g_cursorY = TFT_HEIGHT/2;
int g_prevX = (TFT_WIDTH/2)-1; // previously drawn position of the cursor
int g_prevY = (TFT_HEIGHT/2)-1;

lcd_image_t map_image = { "yeg-sm.lcd", 128, 128 };

// (1024, 1024) = coordinate of the middle of Edmonton picture
uint16_t icol = 1024;
uint16_t irow = 1024;

//functions used in this program
void scanJoystick();
void updateScreen();
void setup();


/* setup the arduino and LCD
draw the black background first and draw the map above the black background
*/
void setup() {
  init();

  Serial.begin(9600);

  tft.initR(INITR_BLACKTAB);   // initialize a ST7735R chip, black tab

  // Initializing SD card
  // Show whether the Initializing is successful in serial-mon
  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("failed!");
    return;
  }
  Serial.println("OK!");

  //Fill the screen with black first
  tft.fillScreen(ST7735_BLACK);

  // draw the small edmonton map above the black background
  lcd_image_draw(&map_image, &tft,
    icol, irow,
    0, 0,
    TFT_WIDTH, TFT_HEIGHT);

  }
  /* set joystick as input and set the delay time equally*/
  int main() {
    setup();

    pinMode(JOY_SEL, INPUT);
    digitalWrite(JOY_SEL, HIGH); // enables pull-up resistor - required!
    // Serial.println("initialized!");
    int startTime = millis();


    while (true) {
      //set the delay time between each step to be same
      scanJoystick();
      updateScreen();
      int curTime = millis();
      if (curTime - startTime < MILLIS_PER_FRAME) {
        delay(MILLIS_PER_FRAME - (curTime-startTime));
      }
      startTime = millis();
    }
    return 0;
  }


  // read the joystick and see if we should nudge the screen
  // change by 64 pixels each step
  void scanJoystick() {
    int vert = analogRead(JOY_VERT_ANALOG);
    int horiz = analogRead(JOY_HORIZ_ANALOG);
    int select = digitalRead(JOY_SEL);

    // set the deadzons, make sure the joystick will not move when nobody is touching it .
    // set the boundaries, so the cursor would stop when hits walls.
    if (abs(horiz - JOY_CENTRE) > JOY_DEADZONE) {
      int delta = (horiz - JOY_CENTRE) / JOY_STEPS_PER_PIXEL;
      g_cursorX = constrain(g_cursorX + delta, 0, TFT_WIDTH-1);
    }

    if (abs(vert - JOY_CENTRE) > JOY_DEADZONE) {

      // update = 1;
      int delta = (vert - JOY_CENTRE) / JOY_STEPS_PER_PIXEL;
      g_cursorY = constrain(g_cursorY + delta, 0, 128-3);
    }
  }
  /* set the cursor to be a 3x3 black square
   * redraw the map to cover the tails of the moving cursor
   */
  void updateScreen() {

    lcd_image_draw(&map_image, &tft,
      g_prevX, g_prevY,
      g_prevX, g_prevY,
      3, 3);

      g_prevX = g_cursorX;
      g_prevY = g_cursorY;

      tft.drawRect(g_prevX, g_prevY, 3, 3, ST7735_BLACK);
      tft.fillRect(g_prevX, g_prevY, 3, 3, ST7735_BLACK);
    }
