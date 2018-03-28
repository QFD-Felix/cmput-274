/** Qiufeng Du  Wenchen Zhang
No: 1439484  1465149
Section: A2  A1
final project
The Greedy Snake
*/
//main

#include <Arduino.h>
#include <EEPROM.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>


#define SD_CS    5  // Chip select line for SD card
#define TFT_CS   6  // Chip select line for TFT display
#define TFT_DC   7  // Data/command line for TFT
#define TFT_RST  8  // Reset line for TFT (or connect to +5V)

// Joystick
#define JOYSTICK_VERT  0   // input A0 - vertical
#define JOYSTICK_HORIZ 1   // input A1 - horizontal
int button_pin = 9;        // input pin 9 - button
int button_value;
int score;
int length;

/** ========================= Function Declare begins =========================*/
void setup();
void start();  // Welcome page
void main_game();
void game_over();
void highscore();
void name();
void printHighscore();


/** ========================= Function Declare ends =========================*/

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

/**==define colour*/
#define BLUE 0x01FF
#define BLACK 0x0000
#define BODY 0xFFFF
#define FOOD 0x01FF
#define GREEN 0x07E0
#define WHITE 0xFFFF

void setup(){

  init();
  Serial.begin(9600);

  pinMode(button_pin, INPUT);
  digitalWrite(button_pin, HIGH);
  tft.initR(INITR_BLACKTAB);
  Serial.print("set up is good");

}


typedef struct {
	int y;
	int x;
	int dir;
  /**direction descibtion:
 	  1 = up
	  2 = down
	  3 = left
	  4 = right
	 */
} movement;

typedef struct {
	uint16_t y;
	uint16_t x;
} coordinates;


struct Table{
  char name[10];
  uint32_t score;
};
Table customerVar;

movement head[500];
movement fill;


coordinates random_food() {
  Serial.println("random food begin");
	//int i = 1;
	coordinates food;

  do {
    food.x = ((rand()%120)*4)%120;// those number need to be mod by 4
  } while(food.x <=0 || food.x >= 122);// just keep food in screen;

  do {
    food.y = ((rand()%140)*4)%140;//*4 and second mod 140 to setboundary[4,120]
  } while(food.y <= 0 || food.y >= 140);

  for(int i =1; i<length/4;i++){
    Serial.println("check in case food is on the snake's body");
		//check in case food is on the snake's body
		if (food.x == head[i].x && food.y == head[i].y) {
			food.x = ((rand() %120) *4) %120;
			food.y = ((rand() %140) *4) % 140;
      break;
		}
    else{
      break;
  }
}


	return food;
}


void printTitle(){
  //print greedy snake
  tft.fillRect(0, 0, 128, 160, 0x00E5);// background color
  tft.fillRect(10, 10, 108, 34, 0xFFFF); // white highlight
  tft.setCursor(20, 16);
  tft.setTextColor (0x8811);
  tft.setTextSize(3);
  tft.println("Snake");

  tft.setCursor(16, 51);
  tft.setTextSize(1);
  tft.setTextColor(0x3333FF);
  tft.println("Please Input Your");
  tft.setCursor(40, 62);
  tft.println("Name on ");
  tft.setCursor(20, 73);
  tft.print("Serial Monitor");
  Serial.println("Input Printable Characters as Your Name");
  Serial.println("Name Will be Stored When Unprintable Characters Are Pressed");
  Serial.println( "Your Name: " );
  tft.setTextSize(1);
  tft.setTextColor(0xFFFF);
  tft.setCursor(25,88);
  tft.print("Your name is");
}
void name(){
  printTitle();
  for (int i = 0; i < 11; ++i) {
    customerVar.name[i] = Serial.read();
    while (customerVar.name[i] == -1) customerVar.name[i] = Serial.read();
    if (!isprint(customerVar.name[i]))
    {customerVar.name[i] = '\0';
    break;
  }
  tft.setTextSize(1);
  tft.setTextColor(0xFFFF);
  tft.setCursor(48+6*i, 99);
  Serial.print(customerVar.name[i]);
  tft.print(customerVar.name[i]);
}
  Serial.println("");
  Serial.println("Name Stored, Go back to Start the Game");
}

void start(){
  name();
  Serial.println("start");



  //print greedy snake on the screen
  tft.fillRect(0, 0, 128, 160, 0x00E5);// background color
  tft.fillRect(10, 10, 108, 34, 0xFFFF); // white highlight
  tft.setCursor(20, 16);
  tft.setTextColor (0x8811);
  tft.setTextSize(3);
  tft.println("Snake");

  // print PRESS BUTTON TO CONTINUE
  tft.setCursor(29, 51);
  tft.setTextSize(1);
  tft.setTextColor(0xF000);
  tft.println("PRESS BUTTON");
  tft.setCursor(32, 62);
  tft.print("TO CONTINUE");

  //print START
  tft.fillRect(46, 90, 40, 13, 0xFFFF);
  tft.setTextSize(1);
  tft.setTextColor(0x8811);
  tft.setCursor(52, 93);
  tft.print("START");

  //print harry
  tft.setTextSize(1);
  tft.setTextColor(GREEN);
  tft.setCursor(53, 125);
  tft.println("2016");
  tft.setCursor(11, 138);
  tft.println("Copyright@ Harry");
  tft.setCursor(20, 148);
   tft.println("and Qiufeng");

  // read button
  while(true) {
    button_value = digitalRead(button_pin);

    if (button_value == LOW) {
      button_value = HIGH;

      Serial.println("call main game");

      main_game();

    }
  }
}

void start_again(){
  Serial.println("start");



  //print greedy snake on the screen
  tft.fillRect(0, 0, 128, 160, 0x00E5);// background color
  tft.fillRect(10, 10, 108, 34, 0xFFFF); // white highlight
  tft.setCursor(20, 16);
  tft.setTextColor (0x8811);
  tft.setTextSize(3);
  tft.println("Snake");

  // print PRESS BUTTON TO CONTINUE
  tft.setCursor(29, 51);
  tft.setTextSize(1);
  tft.setTextColor(0xF000);
  tft.println("PRESS BUTTON");
  tft.setCursor(32, 62);
  tft.print("TO CONTINUE");

  //print START
  tft.fillRect(46, 90, 40, 13, 0xFFFF);
  tft.setTextSize(1);
  tft.setTextColor(0x8811);
  tft.setCursor(52, 93);
  tft.print("START");

  //print harry
  tft.setTextSize(1);
  tft.setTextColor(GREEN);
  tft.setCursor(53, 125);
  tft.println("2016");
  tft.setCursor(11, 138);
  tft.println("Copyright@ Wenchen");
  tft.setCursor(25, 148);
   tft.println("and Qiufeng");

  // read button
  while(true) {
    button_value = digitalRead(button_pin);

    if (button_value == LOW) {
      button_value = HIGH;

      Serial.println("call main game");

      main_game();

    }
  }
}


void main_game(){

  int m;
  button_value = HIGH;
  tft.fillRect(0, 0, 128, 160, BLACK);
  Serial.println("main game begin");

  // /**===test begin, delete later */
  // tft.fillRect(0, 0, 128, 160, BLACK);// black screen
  // Serial.println("main game");
  // Serial.println(button_value);
  // tft.setTextSize(1);
  // tft.setTextColor(WHITE);
  // tft.setCursor(53, 125);
  // tft.println("gameon");
  // /**===test ends*/


	 length = 8;//the initial snake body length

	int delayTime;

	//int m;

	head[0].x = 64;	// initial head position
	head[0].y = 80;
	head[0].dir = 3;

//initial position of joystick
	int init_vert = 502;
	int init_horiz = 498;

  // Serial.println("init_vert");
  // Serial.println(init_vert);
  // Serial.println("init_horiz");
  // Serial.println(init_horiz);

  score = 0;//set the initial score

//clear screen
	//tft.initR(INITR_BLACKTAB);
	tft.fillRect(0, 0, 128, 160, BLACK);

//try later to solve flash problem
  tft.fillRect(0, 147, 128, 13, WHITE);
  tft.setCursor(3, 150);
  tft.setTextColor(BLACK);
  tft.print("Score: ");
  tft.print(score);

	//chooseing the position of the food
	coordinates choose_food = random_food();


	while(true) {

    // score bar : showing the current score
    tft.fillRect(40, 147, 128, 13, BODY);
    tft.setCursor(42, 150);
    tft.setTextColor(BLACK);
    //tft.print("Score: ");
    tft.print(score);

		tft.fillRect(choose_food.x, choose_food.y, 4, 4, FOOD);


		int vert, horiz, select;

		vert = analogRead(JOYSTICK_VERT);       // will be 0-1023
		horiz = analogRead(JOYSTICK_HORIZ);     // will be 0-1023

		int vertical, horizontal;

    //pervent the body move diagonally
    if (vert != init_vert) {
      vertical = vert;
      horizontal = init_horiz;
    }

    else if(horiz != init_horiz) {
      horizontal = horiz;
      vertical = init_vert;
    }

		int i;
		int j;
		int buffer_x[length/4];
		int buffer_y[length/4];

		/** going up **/
		if(head[0].dir == 1) {
			i = 0;
			while(i < (length/4)) {
				buffer_x[i] = head[i].x;
				buffer_y[i] = head[i].y;
				i++;
			}
			head[0].y = head[0].y - 4;
			tft.fillRect(head[0].x, head[0].y, 4, 4, 0xFFFF);

			j = 0;
			while(j<(length/4)-1) {
				head[j+1].x = buffer_x[j];
				head[j+1].y = buffer_y[j];
				tft.fillRect(head[j+1].x, head[j+1].y, 4, 4, 0xFFFF);
				j++;
			}
			tft.fillRect(buffer_x[(length/4)-1], buffer_y[(length/4)-1], 4, 4, BLACK);

			//left
			if ((horizontal < init_horiz - 10) && (vertical == init_vert)) {
				j = 0;
				while(j<(length/4)-1) {
					head[j+1].x = buffer_x[j];
					head[j+1].y = buffer_y[j];
					tft.fillRect(head[j+1].x, head[j+1].y, 4, 4, 0xFFFF);
					j++;
				}

				//gameover condition: when snake hits body
				m = 1;
				while(m < length/4) {
					if ((head[0].x == head[m].x + 4) && (head[0].y == head[m].y)) {
						printHighscore();
					}
					m++;
				}

				head[0].dir = 3; //the direction will now go left
			}


			//right
			else if((horizontal > init_horiz + 10) && (vertical == init_vert)) {
				j = 0;
				while(j<(length/4)-1) {
					head[j+1].x = buffer_x[j];
					head[j+1].y = buffer_y[j];
					tft.fillRect(head[j+1].x, head[j+1].y, 4, 4, 0xFFFF);
					j++;
				}

				m = 1;
				while(m < length/4) {
					if ((head[0].x + 4 == head[m].x) && (head[0].y == head[m].y)) {
						printHighscore();
					}
					m++;
				}

				//gameover condition: when snake hits body
				m = 1;
				while(m < length/4) {
					if ((head[0].y == head[m].y + 4) && (head[0].x == head[m].x)){
						printHighscore();
					}
					m++;
				}

				head[0].dir = 4; //the direction will now go to the right
				tft.fillRect(buffer_x[(length/4)-1], buffer_y[(length/4)-1], 4, 4, BLACK);
			}
			delay(20);
		}

		/** going down **/
		else if(head[0].dir == 2) {
			i = 0;
			while(i < (length/4)) {
				buffer_x[i] = head[i].x;
				buffer_y[i] = head[i].y;
				i++;
			}

			head[0].y = head[0].y + 4;
			tft.fillRect(head[0].x, head[0].y, 4, 4, 0xFFFF);

			j = 0;
			while(j<(length/4)-1) {
				head[j+1].x = buffer_x[j];
				head[j+1].y = buffer_y[j];
				tft.fillRect(head[j+1].x, head[j+1].y, 4, 4, 0xFFFF);
				j++;
			}
			tft.fillRect(buffer_x[(length/4)-1], buffer_y[(length/4)-1], 4, 4, BLACK);

			//left
			if((horizontal < init_horiz - 10) && (vertical == init_vert)) {
				j = 0;
				while(j<(length/4)-1) {
					head[j+1].x = buffer_x[j];
					head[j+1].y = buffer_y[j];
					tft.fillRect(head[j+1].x, head[j+1].y, 4, 4, 0xFFFF);
					j++;
				}

				//gameover condition: when snake hits body
				m = 1;
				while(m < length/4) {
					if ((head[0].x == head[m].x + 4) && (head[0].y == head[m].y)) {
						printHighscore();
					}
					m++;
				}

				head[0].dir = 3; //the direction will now go left
				tft.fillRect(buffer_x[(length/4)-1], buffer_y[(length/4)-1], 4, 4, BLACK);
			}


			//right
			else if((horizontal > init_horiz + 10) && (vertical == init_vert)) {
				j = 0;
				while(j<(length/4)-1) {
					head[j+1].x = buffer_x[j];
					head[j+1].y = buffer_y[j];
					tft.fillRect(head[j+1].x, head[j+1].y, 4, 4, 0xFFFF);
					j++;
				}

				//gameover condition: when snake hits body
				m = 1;
				while(m < length/4) {
					if ((head[0].x + 4 == head[m].x) && (head[0].y == head[m].y)) {
						printHighscore();
					}
					m++;
				}

				head[0].dir = 4; //the direction will now go to the right
				tft.fillRect(buffer_x[(length/4)-1], buffer_y[(length/4)-1], 4, 4, BLACK);

			}

			//game over condition when snake hits its body
			m = 1;
			while(m < length/4) {
				if ((head[0].y + 4 == head[m].y) && (head[0].x == head[m].x)) {
					printHighscore();
				}
				m++;
			}
			delay(20);
		}

		/** going left **/
		else if(head[0].dir == 3) {
			i = 0;
			while (i < (length/4)) {
				buffer_x[i] = head[i].x;
				buffer_y[i] = head[i].y;
				i++;
			}

			// move head[0] to the left
			head[0].x = head[0].x - 4;
			tft.fillRect(head[0].x, head[0].y, 4, 4, 0xFFFF);

			//the rest of the body follows
			j = 0;
			while(j<(length/4)-1) {
				head[j+1].x = buffer_x[j];
				head[j+1].y = buffer_y[j];
				tft.fillRect(head[j+1].x, head[j+1].y, 4, 4, 0xFFFF);
				j++;
			}
			tft.fillRect(buffer_x[(length/4)-1], buffer_y[(length/4)-1], 4, 4, BLACK);

			//up
			if((vertical < init_vert - 10) && (horizontal == init_horiz)) {
				j = 0;
				while(j<(length/4)-1) {
					head[j+1].x = buffer_x[j];
					head[j+1].y = buffer_y[j];
					tft.fillRect(head[j+1].x, head[j+1].y, 4, 4, 0xFFFF);
					j++;
				}

				//gameover condition: when snake hits body
				m = 1;
				while(m < length/4) {
					if ((head[0].y == head[m].y + 4) && (head[0].x == head[m].x)){
					printHighscore();
					}
				m++;
				}

				head[0].dir = 1; //the direction will now go up
				tft.fillRect(buffer_x[(length/4)-1], buffer_y[(length/4)-1], 4, 4, BLACK);
			}

			//down
			else if((vertical > init_vert + 10) && (horizontal == init_horiz)) {
				j = 0;
				while(j<(length/4)-1) {
					head[j+1].x = buffer_x[j];
					head[j+1].y = buffer_y[j];
					tft.fillRect(head[j+1].x, head[j+1].y, 4, 4, 0xFFFF);
					j++;
				}

				//gameover condition: when snake hits body
				m = 1;
				while(m < length/4) {
					if ((head[0].y + 4 == head[m].y) && (head[0].x == head[m].x)){
					printHighscore();
					}
				m++;
				}

				head[0].dir = 2; //the direction will now go down
				tft.fillRect(buffer_x[(length/4)-1], buffer_y[(length/4)-1], 4, 4, BLACK);
			}
					//game over condition when snake hitsits body
			m = 1;
			while(m < length/4) {
				if ((head[0].x == head[m].x + 4) && (head[0].y == head[m].y)) {
					printHighscore();
				}
				m++;
			}
		delay(20);
		}

		/** going right **/
		else if(head[0].dir == 4) {
			i = 0;
			while (i < (length/4)) {
				buffer_x[i] = head[i].x;
				buffer_y[i] = head[i].y;
				i++;
			}

			// move head[0] to the right
			head[0].x = head[0].x + 4;
			tft.fillRect(head[0].x, head[0].y, 4, 4, 0xFFFF);

			//the rest of the body follows
			j = 0;
			while(j<(length/4)-1) {
				head[j+1].x = buffer_x[j];
				head[j+1].y = buffer_y[j];
				tft.fillRect(head[j+1].x, head[j+1].y, 4, 4, 0xFFFF);
				j++;
			}
			tft.fillRect(buffer_x[(length/4)-1], buffer_y[(length/4)-1], 4, 4, BLACK);

			//up
			if((vertical < init_vert - 10) && (horizontal == init_horiz)) {
				j = 0;
				while(j<(length/4)-1) {
					head[j+1].x = buffer_x[j];
					head[j+1].y = buffer_y[j];
					tft.fillRect(head[j+1].x, head[j+1].y, 4, 4, 0xFFFF);
					j++;
				}

				//gameover condition: when snake hits body
				m = 1;
				while(m < length/4) {
					if ((head[0].y == head[m].y + 4) && (head[0].x == head[m].x)){
					printHighscore();
					}
				m++;
				}

				head[0].dir = 1; //the direction will now go up
				tft.fillRect(buffer_x[(length/4)-1], buffer_y[(length/4)-1], 4, 4, BLACK);
			}

			//down
			else if((vertical > init_vert + 10) && (horizontal == init_horiz)) {
				j = 0;
				while(j<(length/4)-1) {
					head[j+1].x = buffer_x[j];
					head[j+1].y = buffer_y[j];
					tft.fillRect(head[j+1].x, head[j+1].y, 4, 4, 0xFFFF);
					j++;
				}

				//gameover condition: when snake hits body
				m = 1;
				while(m < length/4) {
					if ((head[0].y + 4 == head[m].y) && (head[0].x == head[m].x)){
					printHighscore();
					}
				m++;
				}

				head[0].dir = 2; //the direction will now go down
				tft.fillRect(buffer_x[(length/4)-1], buffer_y[(length/4)-1], 4, 4, BLACK);
			}

			//game over condition when snake hits its body
			m = 1;
			while(m < length/4) {
				if ((head[0].x + 4 == head[m].x) && (head[0].y == head[m].y)) {
					printHighscore();
				}
				m++;
			}

			delay(20);
		}

		/**EATING THE food**/
		if (choose_food.x == head[0].x && choose_food.y == head[0].y) {
			length = length + 6;
			choose_food = random_food();
			score++;
      customerVar.score = score;
		}

		/**GAME OVER CONDITION: when snake hits the wall*/
		if (head[0].x == 0 || head[0].x == 128) {
			printHighscore();
			break;
		}

		if (head[0].y == 0 || head[0].y == 144) {
			printHighscore();
			break;
		}



    delayTime =14000/(100+score);


		delay(delayTime);
		/** print out the values */
		Serial.print("delay ");
		Serial.print(delayTime);
		Serial.print("score:");
		Serial.println(score);
	}



  // press button to exit game
  // exit game useful
  // while(true){
  //   button_value = digitalRead(button_pin);
  //   if (button_value == LOW) {
  //     delay(500);
  //     button_value = HIGH;
  //     Serial.println("call game over");
  //     game_over();
  //   }
  // }



}

void highscore (){
  uint32_t j = 5;
  Serial.println("EEPROM");
  Serial.println(customerVar.score);
  Serial.println("test0000000000000000");
  int eeAddress = 0; //EEPROM address to start reading from
  if (customerVar.score >= EEPROM.get(10,j)){
    EEPROM.put(eeAddress, customerVar);}
}

void printHighscore(){


  highscore();
  Serial.println("game over");
  button_value = HIGH;

  tft.fillRect(0, 0, 128, 160, BLACK);//whole black background
  tft.setCursor(11, 16);
  tft.setTextColor(GREEN);//GREEN
  tft.setTextSize(2.1);
  tft.println("GAME OVER");

  tft.fillRect(11,40,106, 14, 0xFFFF);
  tft.setCursor(20, 43);
  tft.setTextColor(0xF000);//black
  tft.setTextSize(1.9);
  tft.println("HIGHEST RECORD");

  uint32_t j = 5;
  tft.setCursor(11,60);
  tft.setTextSize(1.5);
	tft.setTextColor(0xF766);
  Serial.println("The Highest score:");
  tft.println("The Highest score");
  tft.setCursor(11,63);
  tft.println("__________________");
  tft.setCursor(11,74);
  tft.print("Name: ");
  for (int i = 0; i < EEPROM.length() && isprint(EEPROM[i]); ++i) {
    Serial.print((char)EEPROM[i] );
    tft.print((char)EEPROM[i]);
  }
  Serial.print(" ");
  tft.println("");
  tft.setCursor(11,85);
  tft.print("Score: ");
  EEPROM.get(10, j);
  Serial.println(j);
  tft.println(j);
  tft.println("");
  tft.setCursor(11,90);
  tft.print("__________________");

  tft.setCursor(11, 104);
  tft.setTextSize(1.7);
  tft.setTextColor(0xFF07F);// yellow
  tft.print("Your name: ");
  Serial.print("Your name: ");
  tft.println(customerVar.name);
  Serial.println(customerVar.name);
  tft.setCursor(11,116);
  tft.print("Your score: ");
  Serial.print("Your score: ");
  tft.println(customerVar.score);
  Serial.println(customerVar.score);

  tft.setCursor(14, 132);
  tft.setTextColor(0xF803);//red
  tft.setTextSize(1.1);
  tft.print("_______________");
  tft.setCursor(11, 140);
  tft.setTextColor(0xF803);//red
  tft.setTextSize(1.1);
  tft.print("| Press button |");
  tft.setCursor(11, 149);
  tft.setTextColor(0xF803);//red
  tft.setTextSize(1.1);
  tft.print("| to Continue  |");
  tft.setCursor(14, 151);
  tft.print("_______________");
  while(true){
    button_value = digitalRead(button_pin);
    if (button_value == LOW) {
      delay(500);
      button_value = HIGH;
      game_over();
    }
  }
}

void game_over(){

    Serial.println("game over");
    button_value = HIGH;

    tft.fillRect(0, 0, 128, 160, BLACK);//whole black background
  	tft.setCursor(11, 16);
  	tft.setTextColor(GREEN);//GREEN
  	tft.setTextSize(2.1);
  	tft.println("GAME OVER");

  	tft.setCursor(30, 61);
  	tft.setTextSize(1.7);
  	tft.setTextColor(0xF766);// yellow
  	tft.print("Want To Play");
    tft.setCursor(48, 72);
    tft.print("Again?");

  	tft.fillRect(30, 86, 64, 26, 0xFFFF);
  	tft.setCursor(36, 89);
  	tft.setTextColor(0x00FF);//black
  	tft.setTextSize(1.5);
  	tft.println("Press the ");
    tft.setCursor(46, 100);
    tft.println("button");

  	tft.setCursor(21, 132);
  	tft.setTextColor(0xF803);//red
  	tft.setTextSize(1.1);
  	tft.println("Press button to");
  	tft.setCursor(41, 143);
  	tft.setTextSize(1.1);
  	tft.println("Continue");


    while(true){
      button_value = digitalRead(button_pin);
      if (button_value == LOW) {
        delay(500);
        button_value = HIGH;
        Serial.println("call start");
        start_again();
      }
    }

  }



int main(){

  setup();

  Serial.println("int main begin ");

Serial.println("12.6test");

  start();

  return 0;
}
