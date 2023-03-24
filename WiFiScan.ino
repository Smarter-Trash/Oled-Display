#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Bounce2.h>
#include <esp_now.h>
#include <WiFi.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define BUTTONA 25
#define BUTTONB 27
#define RED 0xF800

Bounce debouncerA = Bounce();
Bounce debouncerB = Bounce();

int state = 0;

// REPLACE WITH THE RECEIVER'S MAC Address
uint8_t FaiAddress[] = {0x24, 0x6F, 0x28, 0x25, 0x86, 0xDC};
uint8_t NunAddress[] = {0xA4, 0xCF, 0x12, 0x8F, 0xBA, 0x18};
uint8_t PatAddress[] = {0x3C, 0x61, 0x05, 0x03, 0x68, 0x74};
uint8_t ViewAddress[] = {0xA4, 0xCF, 0x12, 0x8F, 0xCA, 0x28};

TaskHandle_t TaskA = NULL;
TaskHandle_t TaskB = NULL;

// Declaration for SSD1306 display connected using I2C
#define OLED_RESET     -1 // Reset pin
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Structure example to send data
// Must match the receiver structure
typedef struct send_state {
    int state;
} send_state;
send_state State; // Create a struct_message called myData

typedef struct tsh_status {
    int state;
    int plastic;
    int metal;
} tsh_status;
tsh_status Tsh_Status;

typedef struct percent_tsh {
  int state;
  int plastic;
  int metal;
} percent_tsh;
percent_tsh Percent_Tsh;

typedef struct recieve_debt {
  int state;
  int debt;
} recieve_debt;
recieve_debt Debt;

typedef struct pay_coin {
  int state;
  int cost;
} pay_coin;
pay_coin Cost;

// Create peer interface
esp_now_peer_info_t peerInfo1;
esp_now_peer_info_t peerInfo2;
esp_now_peer_info_t peerInfo3;

bool compareMac(const uint8_t * a, const uint8_t * b){
  for(int i=0;i<6;i++){
    if(a[i]!=b[i])
      return false;    
  }
  return true;
}

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  if (compareMac(mac, PatAddress)){
    memcpy(&Tsh_Status, incomingData, sizeof(Tsh_Status));
    memcpy(&Cost, incomingData, sizeof(Cost));
    if (state == 1){
      state = Tsh_Status.state;
      Serial.print("x: ");
      Serial.println(Tsh_Status.state);
      Serial.println(Tsh_Status.plastic);
      Serial.println(Tsh_Status.metal);
    }else if (state == 3){
      state = Cost.state;
      Serial.print("x: ");
      Serial.println(Cost.state);
      Serial.println(Cost.cost);
    }
  }else if (compareMac(mac, FaiAddress)){
    memcpy(&State, incomingData, sizeof(State));
    state = State.state;
    Serial.print("x: ");
    Serial.println(State.state);
  }else if (compareMac(mac, ViewAddress)){
    memcpy(&Percent_Tsh, incomingData, sizeof(Percent_Tsh));
    Serial.println("sssssssssss");
    state = Percent_Tsh.state;
    Serial.print("x: ");
    Serial.println(Percent_Tsh.state);
  }else if (compareMac(mac, NunAddress)){
    memcpy(&Debt, incomingData, sizeof(Debt));
    state = Debt.state;
    Serial.print("x: ");
    Serial.println(Debt.state);
    Serial.println(Debt.debt);
  }

  Serial.print("Bytes received: ");
  Serial.println(len);
  
  // Serial.print("y: ");
  // Serial.println(myData.y);
  // Serial.println();
}

void intro(){
  display.setTextSize(2);
  display.setCursor(28,24);
  display.print("HELLO ");
  display.write(2);
  display.display();
  delay(2000);
  display.clearDisplay();

  display.setCursor(30,0);
  display.print("Press");
  display.setCursor(10,16);
  display.print(" button A");
  display.setCursor(50,32);
  display.print("to");
  display.setCursor(10,50);
  display.print("--BEGIN--");
  display.display();
  delay(2000);
  display.clearDisplay();
}

void choose(){
  display.setTextSize(2);
  display.setCursor(25, 0);
  display.print("Welcome");
  display.setCursor(28, 18);
  display.print("to our");
  display.setCursor(2, 36);
  display.print("trash bin");  
  display.write(3);
  display.display();
  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("PRESS button A to \n\n Feed me trash!");
  display.setCursor(0,36);
  display.print("PRESS button B to \n\n Claim your reward!");
  display.display();  
  delay(2000);
  display.clearDisplay();
}

void trash_status(){
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Smarter Trash Status");
  display.setCursor(0,16);
  display.printf("Plastic : %d", Tsh_Status.plastic);
  display.setCursor(0,32);
  display.printf("Metal : %d", Tsh_Status.metal);
  display.setCursor(0,48);
  display.print("press A when finish");
  display.display();
  delay(2000);
  display.clearDisplay();
}

void trash_quantity(){
  display.setTextSize(1);
  display.setCursor(0,0);
  if (Percent_Tsh.plastic + Percent_Tsh.metal >= 175){
    display.print("I'm so full!!!");    
  } else {
    display.print("I'm still hugry, give me more trash!");
  }
  
  display.setCursor(32, 10);
  display.drawRect(33, 20, 20, 40, WHITE); //Plastic
  display.drawRect(60, 20, 20, 40, WHITE); //Metal
  if (Percent_Tsh.Plastic == 0){
    display.fillRect(33, 60, 0, 0, WHITE);
  }else if (Percent_Tsh.plastic > 0 && Percent_Tsh.plastic <= 10){
    display.fillRect(33, 56, 20, 4, WHITE);
  }else if (Percent_Tsh.plastic > 10 && Percent_Tsh.plastic <= 20){
    display.fillRect(33, 52, 20, 8, WHITE);
  }else if (Percent_Tsh.plastic > 20 && Percent_Tsh.plastic <= 30){
    display.fillRect(33, 48, 20, 12, WHITE);
  }else if (Percent_Tsh.plastic > 30 && Percent_Tsh.plastic <= 40){
    display.fillRect(33, 44, 20, 16, WHITE);
  }else if (Percent_Tsh.plastic > 40 && Percent_Tsh.plastic <= 50){
    display.fillRect(33, 40, 20, 20, WHITE);
  }else if (Percent_Tsh.plastic > 50 && Percent_Tsh.plastic <= 60){
    display.fillRect(33, 36, 20, 24, WHITE);
  }else if (Percent_Tsh.plastic > 60 && Percent_Tsh.plastic <= 70){
    display.fillRect(33, 32, 20, 28, WHITE);
  }else if (Percent_Tsh.plastic > 70 && Percent_Tsh.plastic <= 80){
    display.fillRect(33, 28, 20, 32, WHITE);
  }else if (Percent_Tsh.plastic > 80 && Percent_Tsh.plastic <= 90){
    display.fillRect(33, 24, 20, 36, WHITE);
  }else if (Percent_Tsh.plastic > 90 && Percent_Tsh.plastic <= 100){
    display.fillRect(33, 20, 20, 40, WHITE);
  }
  if (Percent_Tsh.metal == 0){
    display.fillRect(33, 60, 0, 0, WHITE);
  }else if (Percent_Tsh.metal > 0 && Percent_Tsh.metal <= 10){
    display.fillRect(33, 56, 20, 4, WHITE);
  }else if (Percent_Tsh.metal > 10 && Percent_Tsh.metal <= 20){
    display.fillRect(33, 52, 20, 8, WHITE);
  }else if (Percent_Tsh.metal > 20 && Percent_Tsh.metal <= 30){
    display.fillRect(33, 48, 20, 12, WHITE);
  }else if (Percent_Tsh.metal > 30 && Percent_Tsh.metal <= 40){
    display.fillRect(33, 44, 20, 16, WHITE);
  }else if (Percent_Tsh.metal > 40 && Percent_Tsh.metal <= 50){
    display.fillRect(33, 40, 20, 20, WHITE);
  }else if (Percent_Tsh.metal > 50 && Percent_Tsh.metal <= 60){
    display.fillRect(33, 36, 20, 24, WHITE);
  }else if (Percent_Tsh.metal > 60 && Percent_Tsh.metal <= 70){
    display.fillRect(33, 32, 20, 28, WHITE);
  }else if (Percent_Tsh.metal > 70 && Percent_Tsh.metal <= 80){
    display.fillRect(33, 28, 20, 32, WHITE);
  }else if (Percent_Tsh.metal > 80 && Percent_Tsh.metal <= 90){
    display.fillRect(33, 24, 20, 36, WHITE);
  }else if (Percent_Tsh.metal > 90 && Percent_Tsh.metal <= 100){
    display.fillRect(33, 20, 20, 40, WHITE);
  }
  //Show quantity of Metal
  display.setCursor(0, 30);
  //display.print("25%");
  display.printf("%d %", Percent_Tsh.plastic);
  display.setCursor(0, 38);
  display.print("Plastic");
  //Show quantity of Plastic
  display.setCursor(85, 30);
  //display.print("60%");
  display.printf("%d %", Percent_Tsh.metal);
  display.setCursor(85, 38);
  display.print("Metal");
  display.display();
  delay(2000);
  display.clearDisplay();
}

void cost(){
  display.setTextSize(2);
  display.setCursor(10, 0);
  display.print("Thank you");
  display.setCursor(12, 18);
  display.print("for your");
  display.setCursor(8, 36);
  display.print("feeding!");
  display.write(2);
  display.write(15);
  display.display();
  delay(3000);
  display.clearDisplay();

  display.setCursor(16,0);
  display.print("You get");
  display.setCursor(8,16);
  display.print("reward for");
  display.setCursor(3,35);
  display.printf("%d Baht!!", Debt.debt);
  display.display();
  delay(3000);
  display.clearDisplay();
}

void claim(){
  display.setTextSize(2);
  display.setCursor(5,5);
  display.print("Enter your");
  display.setCursor(35,25);
  display.print("phone ");
  display.setCursor(30,40);
  display.print("number");  
  display.display();
  delay(2000);
  display.clearDisplay();
}

void not_enough_coin( ){
  display.setTextSize(2);
  display.setCursor(10,0);
  display.print("Sorry T_T");
  display.setTextSize(1);
  display.setCursor(0,23);
  display.print("We don't have enough coins.");              
  display.setCursor(0, 46);
  display.print("Please,come next time");               
  display.display();
  delay(2000);
  display.clearDisplay();
}

void show_claim_debt(){
  display.setTextSize(2);
  display.setCursor(5,15);
  display.print("You'll get");
  display.setCursor(30,40);
  display.printf("%d Baht", Debt.debt);
  display.display();
  delay(2000);
  display.clearDisplay();
}

void show_debt(){
  display.setTextSize(2);
  display.setCursor(0,0);
  display.print("You'll get");
  display.setCursor(30,20);
  display.printf("%d Baht", Debt.debt);
  display.setCursor(30,40);
  display.print("later!");
  display.display();
  delay(2000);
  display.clearDisplay();
}

void collect_debt(){
  display.setTextSize(1);
  display.setCursor(10, 20);
  display.print("Please enter your ");
  display.setCursor(25, 30);
  display.print("phone number");
  display.setCursor(0, 40);
  display.print("to collect your debt.");
  display.display();
  delay(2000);
  display.clearDisplay();
}

void Thank_you(){
  display.setTextSize(2);
  display.setCursor(35, 10);
  display.print("Thank");
  display.setCursor(35, 30);
  display.print("You!");
  display.setCursor(35, 50);
  display.write(3);
  display.display();
  delay(2000);
  
}

void setup(){
  Serial.begin(115200);
  Serial.println("BUTTON");
  debouncerA.attach(BUTTONA, INPUT_PULLUP);
  debouncerB.attach(BUTTONB, INPUT_PULLUP);
  debouncerA.interval(25);
  debouncerB.interval(25); 

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  // Register peer
  peerInfo1.channel = 0;
  peerInfo1.encrypt = false;
  peerInfo2.channel = 0;
  peerInfo2.encrypt = false;
  peerInfo3.channel = 0;
  peerInfo3.encrypt = false;
  memcpy(peerInfo1.peer_addr, NunAddress, 6);
  memcpy(peerInfo2.peer_addr, PatAddress, 6);
  memcpy(peerInfo3.peer_addr, ViewAddress, 6);
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo1) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  if (esp_now_add_peer(&peerInfo2) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  if (esp_now_add_peer(&peerInfo3) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  // initialize the OLED objec
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Clear the buffer.
  display.clearDisplay();
  display.setTextColor(WHITE);

  //Multitasking
  //xTaskCreatePinnedToCore(Button_Task, "Button_Task", 10000, NULL, 1, &TaskA, 0);
  xTaskCreatePinnedToCore(Oled_Task, "Oled_Task", 2048, NULL, 0, &TaskB, 0);
}
//esp_err_t result;
void loop() {
  while(1){
    debouncerA.update();
    debouncerB.update();
    if(state == 0){
      if (debouncerA.fell()) { 
        state = 15;
        State.state = 15;
        Serial.println(state);
      }
    }else if(state == 15){
      if (debouncerA.fell()) { 
        state = 1;
        State.state = 1;
        esp_err_t result = esp_now_send(PatAddress, (uint8_t *) &State, sizeof(State));
        if (result == ESP_OK) {
          Serial.println("Sent to Pat with success");
        } else {
          Serial.println("Error sending the data");
        }
        Serial.println(state);
      }else if (debouncerB.fell()){
        state = 7;
        State.state = 7;
        esp_err_t result = esp_now_send(NunAddress, (uint8_t *) &State, sizeof(State));
        if (result == ESP_OK) {
          Serial.println("Sent to Nun with success");
        } else {
          Serial.println("Error sending the data");
        }
        Serial.println(state);
      }
    }else if (state == 2){ //รับข้อมูลตอนทิ้งขยะ
      if (debouncerA.fell()) { 
        state = 3; //ส่งข้อมูลจบการทำงาน
        State.state = 3;
        esp_err_t result = esp_now_send(PatAddress, (uint8_t *) &State, sizeof(State));
        if (result == ESP_OK) {
          Serial.println("Sent to Pat with success");
        } else {
          Serial.println("Error sending the data");
        }
        Serial.println(state);
      }
    }else if(state == 5 || state == 9){
      delay(5000);
      state = 0;
    }else if(state == 11){
      delay(5000);
      state = 14;
      State.state = 14; 
      esp_err_t result = esp_now_send(ViewAddress, (uint8_t *) &State, sizeof(State));
        if (result == ESP_OK) {
          Serial.println("Sent to View with success");
        } else {
          Serial.println("Error sending the data");
        }
        Serial.println(state);
    }
  }
}


void Oled_Task (void *param){
  //show_claim_debt();
  //collect_debt();
  //Thank_you();
  while(1){
    if(state == 0){
      intro();
      trash_quantity();
    }else if (state == 1 || state == 2 || state == 3){
      trash_status();
    } else if(state == 4){
      cost(); //แสดงผลจำนวนเงินที่จะได้
    }else if(state == 5){
      show_claim_debt();
    }else if(state == 6){ 
      collect_debt(); //แสดงผลรับเบอร์
    } else if(state == 7){
      claim();
    } else if (state == 8){
      show_debt();
    }else if(state == 9){
      not_enough_coin(); //แสดงผลว่าเงินไม่พอ
      Thank_you();
    } else if (state == 10){
      show_debt();
      delay(5000);
    } else if(state == 11){
      Thank_you(); //show thank you
    } else if(state == 15){
      choose();
    } 
  }
}

