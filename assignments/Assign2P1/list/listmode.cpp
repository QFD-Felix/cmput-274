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

// #define JOYSTICK_VERT 0
// #define JOYSTICK_HORIZ 1
// #define JOYSTICK_BUTTON 9

// Joystick pins:
#define JOYSTICK_VERT  0   // Analog input A0 - vertical
#define JOYSTICK_HORIZ 1   // Analog input A1 - horizontal
#define JOYSTICK_BUTTON 9  // Digital input pin 9 for the button

#define JOY_DEADZONE 64
#define JOY_CENTRE 512

#define TFT_WIDTH 128
#define TFT_HEIGHT 160

#define MILLIS_PER_FRAME 50 // 20fps

// Potentiometer and ratings leds
#define RATING_DIAL 2       // Analog input A2 - restaurant dial selector
#define RATING_LED_0 2      // rating leds 0-4
#define RATING_LED_1 3
#define RATING_LED_2 4
#define RATING_LED_3 10
#define RATING_LED_4 11


Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

int g_cursor_initX = 64; // cursor pixel position (initially at the center of the screen)
int g_cursor_initY = 80;
int g_cursor_preX;
int g_cursor_preY; // previously drawn position of the cursorY
int g_cursor_img_prex, g_cursor_img_prey;
int g_img_init_x = 600;
int g_img_init_y = 680;
int g_cursor_img_x = g_img_init_x + g_cursor_initX;
int g_cursor_img_y = g_img_init_y + g_cursor_initY;

lcd_image_t map_image = { "yeg-big.lcd", 2048, 2048 };

/** These constants are for the 2048 by 2048 map. */
const int16_t map_width = 2048;
const int16_t map_height = 2048;
const int32_t lat_north = 5361858;
const int32_t lat_south = 5340953;
const int32_t lon_west = -11368652;
const int32_t lon_east = -11333496;


Sd2Card card;


// set up for the joystick
// const int VERT = 0;		// analog input
// const int HORIZ = 1;	// analog input
// const int SEL = 9;		// digital input





const uint32_t RESTAURANT_START_BLOCK = 4000000;
const int NUM_RESTAURANTS = 1066;


// read the map of Edmonton from the SD card
char rest_list[20][55];

int selection, pre_sel;

void setup(){

	init();
	Serial.begin(9600);


	pinMode(JOYSTICK_BUTTON, INPUT);
	digitalWrite(JOYSTICK_BUTTON, HIGH);

	tft.initR(INITR_BLACKTAB);

	// Initializing SD card
	Serial.print("Initializing SD card...");
	if (!SD.begin(SD_CS)) {
		Serial.println("failed!");
		while(true){};
	}
	else {
		Serial.println("OK!");
	}

	Serial.print("Doing raw initialization...");
	if (!card.init(SPI_HALF_SPEED, SD_CS)) {
		Serial.println("failed!");
		while(true) {}
	}
	else {
		Serial.println("OK!");
	}

}


void init_setup_led(int rate1, int rate2, int rate3, int rate4, int rate5){
  pinMode(RATING_LED_0, OUTPUT);
  pinMode(RATING_LED_1, OUTPUT);
  pinMode(RATING_LED_2, OUTPUT);
  pinMode(RATING_LED_3, OUTPUT);
  pinMode(RATING_LED_4, OUTPUT);


  digitalWrite(RATING_LED_0, rate1);
  digitalWrite(RATING_LED_1, rate2);
  digitalWrite(RATING_LED_2, rate3);
  digitalWrite(RATING_LED_3, rate4);
  digitalWrite(RATING_LED_4, rate5);
}
int rate_inv1 = 1023/6;
int rate_inv2 = 2*1023/6;
int rate_inv3 = 3*1023/6;
int rate_inv4 = 4*1023/6;
int rate_inv5 = 5*1023/6;
int rate;
void setup_led(){
  int actual_rate = analogRead(RATING_DIAL);
  if (actual_rate <= rate_inv1){
    rate = 0;
    init_setup_led(LOW, LOW, LOW, LOW, LOW);
  }
  else if((actual_rate> rate_inv1) && (actual_rate <= rate_inv2)){
    rate = 1;
    init_setup_led(HIGH, LOW, LOW, LOW, LOW);
  }
  else if((actual_rate> rate_inv2) && (actual_rate <= rate_inv3)){
    rate = 2;
    init_setup_led(HIGH, HIGH, LOW, LOW, LOW);
  }
  else if((actual_rate> rate_inv3) && (actual_rate <= rate_inv4)){
    rate = 3;
    init_setup_led(HIGH, HIGH, HIGH, LOW, LOW);
  }
  else if((actual_rate> rate_inv4) && (actual_rate <= rate_inv5)){
    rate = 4;
    init_setup_led(HIGH, HIGH, HIGH, HIGH, LOW);
  }
  else if((actual_rate > rate_inv5)){
    rate = 5;
    init_setup_led(HIGH, HIGH, HIGH, HIGH, HIGH);
  }
}


typedef struct {
	int32_t lat;		// Stored in 1/100,000 degrees
	int32_t lon;		// Stored in 1/100,000 degrees
	uint8_t rating;	// 0-10: 2 = 1 star, 10 = 5 stars
	char name[55];	// always null terminated
} restaurant;

restaurant r;

typedef struct {
	uint16_t index;     // index of restaurant
	uint16_t dist;      // distance to cursor position
} RestDist;
RestDist rest_dist[NUM_RESTAURANTS];



uint32_t previousBlock = RESTAURANT_START_BLOCK;


restaurant buffer[8];


void get_restaurant_fast(restaurant* ptr_r, int i) {
	uint32_t block;
	block = RESTAURANT_START_BLOCK + i/8 ;

	if (previousBlock != block) {
		previousBlock = block;
		while (!card.readBlock(block, (uint8_t*)buffer)) {
			Serial.println("Read block failed! Trying again.");
		}
	}

	else if (i == 0){
		while (!card.readBlock(block, (uint8_t*)buffer)) {
			Serial.println("Read block failed! Trying again.");
		}
	}
	else {}

	*ptr_r = buffer[i%8];
}



/** Swap two restaurants of RestDist struct */
void swap_rest(RestDist *ptr_rest1, RestDist *ptr_rest2) {
	RestDist tmp = *ptr_rest1;
	*ptr_rest1 = *ptr_rest2;
	*ptr_rest2 = tmp;
}


// pick_pivot of an array
int pick_pivot(int len) {
	int pivotIdx = (len-1)/2;
	return pivotIdx;
}


int partition( RestDist* array, int len, int pivot_idx ) {
	int pivot = array[pivot_idx].dist;
	swap_rest(&array[pivot_idx], &array[len-1]);

	// scan small
	int small = 0;
	// scan big
	int big = len-2;

	while (small <= big) {
		while (array[small].dist < pivot){
			small=small+1;
		}
		while (array[big].dist > pivot) {
			big=big-1;
		}
		if (small <= big) {
			swap_rest(&array[small], &array[big]);
			small=small+1;
			big=big-1;
		}
	}
	swap_rest(&array[small], &array[len-1]);

	return small;
}

// qsort from the lecture
void qsort(RestDist* array, int len) {
    if (len > 1) {
		int pivot_idx = pick_pivot(len);
		pivot_idx = partition( array, len, pivot_idx );
		qsort( array, pivot_idx );
		qsort( array+pivot_idx+1, len-pivot_idx-1 );
    }
    else {
		return;
  }
}

// Print restaurant names to the screen
void print_rest(int start, int end) {
	tft.fillScreen(0x0000);	// clear the screen
	tft.setCursor(0,0);
	tft.setTextWrap(false);

	// print  restaurant names
	for (int i = start; i <= end; i++){
		// highlight selected text
		if (i == selection){
			tft.setTextColor(0x0000, 0xFFFF); // black char on white bg
		}

		else {
			tft.setTextColor(0xFFFF, 0x0000); // white char on black bg
		}
		tft.print(rest_list[i]);
		tft.print('\n');
	}
}

// highlight names
void update_rest() {


	tft.setTextColor(0xFFFF, 0x0000);
	tft.setCursor(0, (pre_sel % 20)*8);
	tft.print(rest_list[pre_sel % 20]);


	tft.setTextColor(0x0000, 0xFFFF);
	tft.setCursor(0, (selection % 20)*8);
	tft.print(rest_list[selection % 20]);
}

// update the list when cursor moves
void update_rest_list(){

	int tmp = selection;

	// check for user push the joystick up or down
	if (selection % 20 == 19){
		selection -= 19;
	}

	// store the closest 20 names into an array
	for (int i = 0; i < 20; i++) {
		get_restaurant_fast(&r,rest_dist[selection].index);
		strcpy(rest_list[i],r.name);
		selection += 1;
	}

	selection = tmp;		// the choosen position

	print_rest(0,19);		// print the closest 20 names

}


//change coordinates
int32_t x_to_lon(int16_t x) {
    return map(x, 0, map_width, lon_west, lon_east);
}
int32_t y_to_lat(int16_t y) {
    return map(y, 0, map_height, lat_north, lat_south);
}
int16_t lon_to_x(int32_t lon) {
    return map(lon, lon_west, lon_east, 0, map_width);
}
int16_t lat_to_y(int32_t lat) {
    return map(lat, lat_north, lat_south, 0, map_height);
}





void twoRestBound() {
	/* case 1:  the choosen restaurant is too close to the map edge,
	so the cursor will not be placed at the center of the LCD.
	case 2: choosen restaurant is out of the map
	so cursor will be on screen that is closest to the choosen restaurant.
	*/

	// case 1 for the x coor
	if( g_cursor_img_x <= (0.5 * TFT_WIDTH) && g_cursor_img_x >= 0) {
		g_cursor_initX = g_cursor_img_x;
	}

	else if ((map_width - g_cursor_img_x) <=
	(0.5*TFT_WIDTH) && g_cursor_img_x <=
	(map_width - 3)) {
		g_cursor_initX = TFT_WIDTH - (map_width - g_cursor_img_x);
	}

	// case 2 for the x coor
	else if (g_cursor_img_x < 0) {
		g_cursor_initX = 0;
	}

	else if (g_cursor_img_x > (map_width - 3)) {
		g_cursor_initX = TFT_WIDTH-3;
	}

	// in normal case for the x coor
	else {
		g_cursor_initX = 64;
	}

	// case 1 for the y coor
	if( g_cursor_img_y <= (0.5 * TFT_HEIGHT) && g_cursor_img_y >= 0) {
		g_cursor_initY = g_cursor_img_y;
	}

	else if ((map_height - g_cursor_img_y) <=
	(0.5*TFT_HEIGHT) && g_cursor_img_y <=
	(map_height - 3)) {
		g_cursor_initY = TFT_HEIGHT - (map_height - g_cursor_img_y);
	}

	// case 2 for the y coor
	else if (g_cursor_img_y < 0) {
		g_cursor_initY = 0;
	}

	else if (g_cursor_img_y > (map_height - 3)) {
		g_cursor_initY = TFT_HEIGHT-3;
	}

	// in normal case for the y coor
	else {
		g_cursor_initY = 80;
	}

}



void setup_list();



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
  int init_vert = analogRead(JOYSTICK_VERT);
  int init_horiz = analogRead(JOYSTICK_HORIZ);

  while(true){
  g_cursor_preX = g_cursor_initX;
  g_cursor_preY = g_cursor_initY;
  g_cursor_img_prex = g_cursor_img_x;
  g_cursor_img_prey = g_cursor_img_y;

  int vert = analogRead(JOYSTICK_VERT);
  int horiz = analogRead(JOYSTICK_HORIZ);
  int select = digitalRead(JOYSTICK_BUTTON);

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

	if (select == 0){
		delay(50);
		setup_list();
	}

  tft.fillRect(g_cursor_initX, g_cursor_initY, 3, 3, 0x0000);
}
}




void setup_list() {

	int init_vert_listMode, init_horiz_listMode;
	int select, vertical_listMode;
	select = digitalRead(JOYSTICK_BUTTON);

	// store the restaurant index and distance into the array rest_dist
	int j = 0;
	for (int i = 0; i < NUM_RESTAURANTS; i++) {
		get_restaurant_fast(&r, i);


			rest_dist[j].index = i;

			// calculate distance
			int differenceX = lon_to_x(r.lon) - g_cursor_img_x;

			int differenceY = lat_to_y(r.lat) - g_cursor_img_y;

			rest_dist[j].dist = abs(differenceX) + abs(differenceY);

			j++;


	}

	// quick sort the nearest restaurants
	qsort(rest_dist, j);


	for (int i = 0; i < 20; i++) {
		get_restaurant_fast(&r,rest_dist[i].index);
		strcpy(rest_list[i],r.name);
	}

	selection = 0;	// choose a rest

	print_rest(0,19);

	delay(500);

	init_vert_listMode = analogRead(JOYSTICK_VERT);

	while(true){

		vertical_listMode = analogRead(JOYSTICK_VERT);
		select = digitalRead(JOYSTICK_BUTTON);
		pre_sel = selection;


		//move the list if joystick moves
		if(abs(vertical_listMode-JOY_CENTRE)> JOY_DEADZONE){

				if ((vertical_listMode - init_vert_listMode)<0){

					selection = selection - 1;

					if (selection % 20 == 19) {
						update_rest_list();
					}


					else {
						update_rest();
					}
				}
				else if ((vertical_listMode - init_vert_listMode)>0){

					selection = selection + 1;
					;
					if (selection % 20 == 0 && selection != 0) {
						update_rest_list();
					}
					else {
						update_rest();
					}
					;
				}
				else{
					;
				}

				if (selection < 0){

					selection = 0;

				}
		}

//choose a rest and print the location on the screen
		if (select == 0){

			delay(100);
			get_restaurant_fast(&r, rest_dist[selection].index);

			g_cursor_img_x = lon_to_x(r.lon);
			g_cursor_img_y = lat_to_y(r.lat);

			// two special cases for displaying the choosen restaurant on edge
			twoRestBound();

			lcd_image_draw(&map_image, &tft, g_cursor_img_x-g_cursor_initX, g_cursor_img_y-g_cursor_initY, 0, 0, TFT_WIDTH, TFT_HEIGHT);

			// redraw the bcursor at the position of the selected rest
			tft.fillRect(g_cursor_initX, g_cursor_initY, 3, 3, 0x0000);

			//repeat the beginning procedual
			setup_map();

			break;
		}


		delay(70);
	}
}



int main (){

	setup();


	// draw a portion of the small Edmonton Map
	lcd_image_draw(&map_image, &tft, g_img_init_x, g_img_init_y, 0, 0, TFT_WIDTH, TFT_HEIGHT);

	setup_map();

	return 0;
}
