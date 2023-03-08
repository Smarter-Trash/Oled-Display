#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_SSD1331.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using I2C
#define OLED_RESET     -1 // Reset pin
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void intro(){
  display.print("HELLO ");
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(30,24);
  display.print("HELLO ");
  display.write(2);
  display.display();
  Serial.print("bb");
  delay(2000);
  //display.clearDisplay();

  display.setCursor(0,10);
  display.print("Welcome to our trash bin ");
  display.write(3);
  display.display();
  delay(2000);
  //display.clearDisplay();
}

void quantity(){
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("I'm still hugry, give me more trash!");
  display.setCursor(32, 10);
  display.drawRect(33, 20, 20, 40, WHITE);
  display.drawRect(60, 20, 20, 40, WHITE);
  display.fillRect(33, 50, 20, 10, WHITE);
  display.fillRect(60, 40, 20, 20, WHITE);
  //Show quantity of Metal
  display.setCursor(0, 30);
  display.print("10%");
  display.setCursor(0, 38);
  display.print("Metal");
  //Show quantity of Plastic
  display.setCursor(85, 30);
  display.print("60%");
  display.setCursor(85, 38);
  display.print("Plastic");
  display.display();
  delay(2000);
  display.clearDisplay();
}

void cost(){
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 0);
  display.print("Thank you");
  display.setCursor(12, 18);
  display.print("for your");
  display.setCursor(8, 36);
  display.print("feeding!");
  display.write(2);
  display.write(15);
  display.display();
  delay(2000);
  display.clearDisplay();

  display.setCursor(16,0);
  display.print("You get");
  display.setCursor(8,16);
  display.print("reward for");
  display.setCursor(3,35);
  display.print("100 Baht!!");
  display.display();
  delay(2000);
  display.clearDisplay();
}

void setup()
{
  Serial.begin(9600);
  
  // initialize the OLED object
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Clear the buffer.
  display.clearDisplay();

  
}

void loop() {
  display.print("HELLO ");
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(30,24);
  display.print("HELLO ");
  display.write(2);
  display.display();
  Serial.print("bb");
  delay(2000);
  //display.clearDisplay();

  display.setCursor(0,10);
  display.print("Welcome to our trash bin ");
  display.write(3);
  display.display();
  delay(2000);
  /*intro();
  delay(1000);
  Serial.print("aa");*/
}
