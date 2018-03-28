/* Qiufeng Du
  No: 1439484
  Section: A2
  */
  #include <Arduino.h>
  #include <Adafruit_GFX.h>
  #include <Adafruit_ST7735.h>
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

#define MILLIS_PER_FRAME 50 // 20fps



Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

uint16_t g_cursor_initX = 64; // cursor pixel position (initially at the center of the screen)
uint16_t g_cursor_initY = 80;
uint16_t g_cursor_preX;
uint16_t g_cursor_preY; // previously drawn position of the cursorY
uint16_t g_cursor_img_prex, g_cursor_img_prey;
uint16_t g_img_init_x = 600;
uint16_t g_img_init_y = 680;
uint16_t g_cursor_img_x = g_img_init_x + g_cursor_initX;
uint16_t g_cursor_img_y = g_img_init_y + g_cursor_initY;

lcd_image_t map_image = { "yeg-big.lcd", 2048, 2048 };

/** These constants are for the 2048 by 2048 map. */
const int16_t map_width = 2048;
const int16_t map_height = 2048;
const int32_t lat_north = 5361858;
const int32_t lat_south = 5340953;
const int32_t lon_west = -11368652;
const int32_t lon_east = -11333496;


// (1024, 1024) = coordinate of the middle of Edmonton picture
// uint16_t icol = 1024;
// uint16_t irow = 1024;


//functions used in this program
void scanJoystick();
void updateScreen();
void setup();
void MapBound();

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

  // //Fill the screen with black first
  // tft.fillScreen(ST7735_BLACK);

  // draw the small edmonton map above the black background
  lcd_image_draw(&map_image, &tft,g_img_init_x,g_img_init_y, 0,0, TFT_WIDTH, TFT_HEIGHT);

  }

  void MapEdge(){
    if ( g_cursor_initY < 0 ){
      //top
  		if (g_cursor_img_y <= TFT_HEIGHT) {
  			lcd_image_draw(&map_image, &tft, (g_cursor_img_x-g_cursor_initX),0, 0, 0, TFT_WIDTH, TFT_HEIGHT);
  			g_cursor_initY = g_cursor_img_y;
  		}
  		else {
  			g_cursor_initY = TFT_HEIGHT-3;
  			lcd_image_draw(&map_image, &tft, (g_cursor_img_x-g_cursor_initX), (g_cursor_img_y+TFT_HEIGHT),0,0,TFT_WIDTH,TFT_HEIGHT);
  			g_cursor_img_y -= 3;
  		}
  	}
    //bottom
  	else if (g_cursor_initY > (TFT_HEIGHT-3) ){
  		if ((map_height - g_cursor_img_y) <= TFT_HEIGHT) {
  			lcd_image_draw(&map_image, &tft, (g_cursor_img_x-g_cursor_initX), (map_height - TFT_HEIGHT), 0, 0, TFT_WIDTH, TFT_HEIGHT);
  			g_cursor_initY = g_cursor_img_y - (map_height - TFT_HEIGHT);
  		}
  		else {
  			g_cursor_initY = 0;
  			lcd_image_draw(&map_image, &tft, (g_cursor_img_x-g_cursor_initX),(g_cursor_img_y+3),0,0,TFT_WIDTH,TFT_HEIGHT);
  			g_cursor_img_y += 3;
  		}
  	}

    //left
    if ( g_cursor_initX < 0 ){
  		if (g_cursor_img_x <= TFT_WIDTH) {
  			lcd_image_draw(&map_image, &tft, 0, (g_cursor_img_y-g_cursor_initY), 0, 0, TFT_WIDTH, TFT_HEIGHT);
  			g_cursor_initX = g_cursor_img_x;
  		}
  		else {
  			g_cursor_initX = TFT_WIDTH-3;
  			lcd_image_draw(&map_image, &tft, (g_cursor_img_x-TFT_WIDTH), (g_cursor_img_y-g_cursor_initY), 0, 0, TFT_WIDTH, TFT_HEIGHT);
  			g_cursor_img_x -= 3;
  		}
  	}
    //right
  	else if ( g_cursor_initX > (TFT_WIDTH-3) ){
  		if ((map_width - g_cursor_img_x) <= TFT_WIDTH) {
  			lcd_image_draw(&map_image, &tft, (map_width - TFT_WIDTH), (g_cursor_img_y-g_cursor_initY), 0, 0, TFT_WIDTH, TFT_HEIGHT);
  			g_cursor_initX = g_cursor_img_x - (map_width - TFT_WIDTH);
  		}
  		else {
  			g_cursor_initX = 0;
  			lcd_image_draw(&map_image, &tft, (g_cursor_img_x+3),(g_cursor_img_y-g_cursor_initY),0,0,TFT_WIDTH,TFT_HEIGHT);
  			g_cursor_img_x += 3;
  		}
  	}
  }


void setup_map(){
  int init_vert = analogRead(JOY_VERT_ANALOG);
  int init_horiz = analogRead(JOY_HORIZ_ANALOG);

  while(true){
  g_cursor_preX = g_cursor_initX;
  g_cursor_preY = g_cursor_initY;
  g_cursor_img_prex = g_cursor_img_x;
  g_cursor_img_prey = g_cursor_img_y;

  int vert = analogRead(JOY_VERT_ANALOG);
  int horiz = analogRead(JOY_HORIZ_ANALOG);
  int select = digitalRead(JOY_SEL);

  int delta_vert = vert - init_vert;
  int delta_horiz = horiz - init_horiz;

  g_cursor_initX = g_cursor_initX + delta_horiz/100;
  g_cursor_initY = g_cursor_initY + delta_vert/100;
  g_cursor_img_x = g_cursor_img_x + delta_horiz/100;
  g_cursor_img_y = g_cursor_img_y + delta_vert/100;

  g_cursor_img_x = constrain(g_cursor_img_x, 0, 2045);
  g_cursor_img_y = constrain(g_cursor_img_y, 0, 2045);

  if (g_cursor_img_x != g_cursor_img_prex || g_cursor_img_y != g_cursor_img_prey){
    lcd_image_draw(&map_image, &tft, g_cursor_img_prex, g_cursor_img_prey, g_cursor_preX, g_cursor_preY, 3, 3 );
  }


  MapEdge();


  tft.fillRect(g_cursor_initX, g_cursor_initY, 3, 3, 0x0000;
}
}


int main() {
  setup();

  pinMode(JOY_SEL, INPUT);
  digitalWrite(JOY_SEL, HIGH); // enables pull-up resistor - required!
  // Serial.println("initialized!");

  int startTime = millis();
  while (true) {
    setup_map();
  }
  return 0;
}
