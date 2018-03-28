#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SD.h>

#define SD_CS   5
#define TFT_CS  6
#define TFT_DC  7
#define TFT_RST 8

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

Sd2Card card;

const uint32_t REST_START_BLOCK = 4000000;
const int NUM_REST = 1066;

struct restaurant {
  uint32_t lat;
  uint32_t lon;
  uint8_t rating;
  char name[55];
};  // semicolon is needed

void setup() {
  init();
  Serial.begin(9600);

  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST7735_BLACK);

  // initialize SPI (serial peripheral interface)
  // communication between the Arduino and the SD controller

  Serial.print("Initializing SPI communication for raw reads...");
  if (!card.init(SPI_HALF_SPEED, SD_CS)) {
    Serial.println("failed!");
    while (true) {}
  }
  else {
    Serial.println("OK!");
  }
}

// get the i'th restaurant and store it in the
// memory address pointed to by ptr
void get_restaurant(restaurant* ptr, int i) {
  uint32_t block = REST_START_BLOCK + i/8;
  restaurant buffer[8];

  // fill the restaurant buffer with the block
  // because of how things are aligned,
  // restaurant[j] is already the j'th restaurant
  // of the block
  while (!card.readBlock(block, (uint8_t*) buffer)) {
    Serial.println("readBlock failed, trying again!");
  }

  *ptr = buffer[i%8];
}
int check = 0;

void get_restaurant_fast(restaurant* ptr2, int i){
  uint32_t block = REST_START_BLOCK + i/8;
  restaurant buffer[8];

  // fill the restaurant buffer with the block
  // because of how things are aligned,
  // restaurant[j] is already the j'th restaurant
  // of the block
  if (i == check){
  while (!card.readBlock(block, (uint8_t*) buffer)) {
    Serial.println("readBlock failed, trying again!");
  }
  check += 8;// the card will not be read twice from i = 0, to i = 7
}

  *ptr2 = buffer[i%8];
}



int main() {
  setup();
  restaurant r;
  //running time by using the fast function
  int preTime = millis();
  for (int i = 0; i < NUM_REST; ++i) {
    get_restaurant_fast(&r, i);
  }
  int curTime = millis();
  int Time = curTime - preTime;
  Serial.print("Time used by using fast function: ");
  Serial.println(Time);

  //running time by using the slower function
  int preTime1 = millis();
  for (int i = 0; i < NUM_REST; ++i) {
    get_restaurant(&r, i);
  }
  int curTime1 = millis();
  int Time1 = curTime1 - preTime1;
  Serial.print("Time used by using slower function: ");
  Serial.println(Time1);

  Serial.end();

  return 0;
}
