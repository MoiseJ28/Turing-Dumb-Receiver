/*********
   MTuring Insigths
 Moise
 *********/

#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>     //OLED Display
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <esp_task_wdt.h>
Adafruit_SSD1306 display(128, 64, &Wire, -1); // Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
          
//define the pins used by the transceiver module
#define ss 5
#define rst 14
#define dio0 2

#define Amber 25
#define Green 26
#define Red  27

        

float GVM;
int bin_1;
int bin_2;
float ax31;
float td1;
float td2;
float td3;

String id;
String idRx = "H10";
          
int flash_time = 6;  
                          

      
String LoRaData;          
          
#define WDT_TIMEOUT 10
        
 void setup() {
 //initialize Serial Monitor
 Serial.begin(9600);
 delay(1000);
 while (!Serial);
 Serial.println("LoRa Receiver");
  //setup LoRa transceiver module
 LoRa.setPins(ss, rst, dio0);
            
 while (!LoRa.begin(868E6)) { // Frequency
 Serial.println(".");
 delay(1);
            }
 LoRa.setSyncWord(0xf3);  // ranges from 0-0xFF (ID)
 Serial.println("LoRa Initializing OK!");
   
 // Display
 display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  //initialize with the I2C addr 0x3C
 display.clearDisplay();   // Clear the buffer.
 // Welcome Turing Device
display.clearDisplay();
display.setTextSize(1);
display.setTextColor(WHITE);
display.setCursor(0,28);
display.println("Turing device ON...");
display.display();
delay(100);
display.clearDisplay();
//LoadLight            
pinMode(Amber, OUTPUT);
pinMode(Green, OUTPUT);
pinMode(Red, OUTPUT);

//Watchdog Timer
            
esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
esp_task_wdt_add(NULL); //add current thread to WDT watch
      
loadlight_off(); //turn off RX if not receiving data
           }
        
void loop(){
          
int packetSize = LoRa.parsePacket();// try to parse packet
          
if (packetSize){
 Serial.print("Received packet: "); // received a packet
 while (LoRa.available()) {  //read packet
      receive_data();
      esp_task_wdt_reset();
  if (id == idRx) {
      ProcessData();
      display_screen();}
  else {
          return;
          }
        }
      }
  }
      
void ProcessData(){
  
    switch (bin_1) {
        case 0:
            loadlight_off();
            break;
        case 1:
             amber_solid();
            break;
        case 2:
            amber_flash();
            break;
        case 3:
            green_solid();
            break;
        case 4:
            green_flash();
            break; 
        case 5:
           red_solid();
            break;                       
        default:
           
            break;
    }
  
}

void receive_data(){
 LoRaData = LoRa.readString();
      Serial.println(LoRaData);
      int pos1 = LoRaData.indexOf(';');
      int pos2 = LoRaData.indexOf('@');
      int pos3 = LoRaData.indexOf('#'); 
//      int pos4 = LoRaData.indexOf('$');
//      int pos5 = LoRaData.indexOf('%'); 
//      int pos6 = LoRaData.indexOf('&');
//      int pos7 = LoRaData.indexOf('*');
    
      String str_GVM = LoRaData.substring(0, pos1);
      String str_id = LoRaData.substring(pos1+1, pos2);
      String str_ax11 = LoRaData.substring(pos2+1, pos3);
      String str_ax21 = LoRaData.substring(pos3+1, LoRaData.length());

      
      GVM = str_GVM.toFloat();
      id = str_id;
      
      bin_1 = str_ax11.toInt();
      bin_2 = str_ax21.toFloat();
//      ax31 = str_ax31.toFloat();
//      td1 = str_td1.toFloat();
//      td2 = str_td2.toFloat();
//      td3 = str_td3.toFloat();
      yield();
}

 void display_screen(){
      display.clearDisplay();
      display.setCursor(0,0);
      display.setTextSize(1);
      display.print("GVM:");
      display.setCursor(50,0);
      display.print(GVM);
//
//      display.setCursor(0,15);
//      display.print("Ax1: ");
//      display.setCursor(50,15);
//      display.print(td1);
//
      display.setCursor(0,30);
      display.print("id: ");
      display.setCursor(50,30);
      display.print(id);

      display.setCursor(0,45);
      display.print("Code:");
      display.setCursor(50,45);
      display.print(bin_1);

      display.setCursor(100,30);
      display.print(LoRa.packetRssi());
      display.display();
      yield();
      
 }

 void amber_solid(){
      digitalWrite(Green,LOW);
      digitalWrite(Red,LOW);
      digitalWrite(Amber,HIGH);
      yield();
 }

 void green_solid(){
      digitalWrite(Red,LOW);
      digitalWrite(Amber,LOW);
      digitalWrite (Green,HIGH);
      yield();
 }

void green_flash(){
      digitalWrite(Amber,LOW);
      digitalWrite(Red,LOW);
      for (int i = 0; i < flash_time; i++){
      blinkLed(500, Green);}
      yield();
}

void red_flash(){
      digitalWrite(Amber,LOW);
      digitalWrite(Green,LOW);
      for (int i = 0; i < flash_time; i++){
      blinkLed(500, Red);}
      yield();
}

void amber_flash(){
      digitalWrite(Red,LOW);
      digitalWrite(Green,LOW);
      for (int i = 0; i < flash_time; i++){
      blinkLed(500, Amber);}
      yield();
}

void red_solid(){
     digitalWrite(Green, LOW);
     digitalWrite(Amber,LOW);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
     digitalWrite(Red, HIGH);
     yield();
}
void loadlight_off(){
      digitalWrite(Amber, LOW);
      digitalWrite(Green, LOW);
      digitalWrite(Red, LOW);
      display.clearDisplay();
}

void blinkLed(int delayTime, int led) {
  digitalWrite(led, HIGH);   
  delay(delayTime);              
  digitalWrite(led, LOW);    
  delay(delayTime);            
}
