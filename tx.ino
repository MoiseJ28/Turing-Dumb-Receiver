/*****************************************
Mpoyo Turing Insights

******************************************/

#include <SPI.h>
#include <Wire.h>
#include <LoRa.h>
#include <esp_task_wdt.h>

#define ss 5          //define the pins used by the transceiver module
#define rst 14
#define dio0 2

#define LED_BLUE  13

String GVM_msg;
String id = "H10";
String LoRaMessage;
float gvm;


char testInput[200];
float myArray[3][14] = {{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }};
int counter = 0;
int row = 0;

float Amber_on = 20.0;
float Amber_max = 40.0;
float Green_on_min = 45.0;
float Green_on_max = 47.2;
float Green_f = 49.0;

int bin_1, bin_2;
float gt1;

#define WDT_TIMEOUT 10

// Your GPRS credentials (leave empty, if missing)
const char apn[]      = "internet"; // Your APN
const char gprsUser[] = ""; // User
const char gprsPass[] = ""; // Password
const char simPIN[]   = "5643"; // SIM card PIN code, if any

void setup(){           
  pinMode(LED_BLUE, OUTPUT);     
  Serial.begin(9600);
  
  while (!Serial);
  Serial.println("LoRa TX");
  
  LoRa.setPins(ss, rst, dio0);    //setup LoRa transceiver module
  while (!LoRa.begin(868E6)) {//replace the LoRa.begin()with your location's frequency
  Serial.println(".");
  delay(100);
 //Watchdog Timer
  esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL); //add current thread to WDT watch
  }
    
  LoRa.setSyncWord(0xf3);  // The sync word assures you don't get LoRa messages from other LoRa transceivers, (0xF3) to match the receiver
  Serial.println("LoRa Initializing OK!");
  delay(100);
  }

long prevMillis = 0;
int interval = 1000;
boolean ledState = false;

void loop() {
  if (millis() - prevMillis > interval) {
  ledState = !ledState;
  digitalWrite(LED_BLUE, ledState);
  prevMillis = millis();
  }  
  if (Serial.available() > 0){
   readData(); 
  }

  }

void readData(){ 
  
float ax11, ax12, ld1, gl1, td1, d1, wm1, sn1;
float ax21, ax22, ld2, td2, gl2, gt2, d2, wm2, sn2;
float ax31, ax32, ld3, td3, gl3, gt3, d3, wm3, sn3;


 for (int i = 0; i < 4; i++){
   Serial.readBytesUntil('\n', testInput, 200);
   char* pch = strtok(testInput,"US;\r\n");
   while(pch != NULL){
   myArray[i][counter] = atof(pch);
   pch = strtok(NULL,"S;\r\n");
   counter += 1; }
   
   esp_task_wdt_reset();
   counter = 0; 

      
     ax11 = myArray[0][0];
     ax12 = myArray[0][1];
     ld1 = myArray[0][4];
     td1 = myArray[0][5];
     gl1 = myArray[0][6];
     gt1 = myArray[0][7]; 
     d1 = myArray[0][8];
     wm1 = myArray[0][9];
     sn1 = myArray[0][10];      
     
     ax21 = myArray[1][0];
     ax22 = myArray[1][1];
     ld2 = myArray[1][4];
     td2 = myArray[1][5];
     gl2 = myArray[1][6];
     gt2 = myArray[1][7]; 
     d2 = myArray[1][8];
     wm2 = myArray[1][9];
     sn2 = myArray[1][10]; 
  
     ax31 = myArray[2][0];
     ax32 = myArray[2][1];
     ld3 = myArray[2][4];
     td3 = myArray[2][5];
     gl3 = myArray[2][6];
     gt3 = myArray[2][7]; 
     d3 = myArray[2][8];
     wm3 = myArray[2][9];
     sn3 = myArray[2][10];     

 }   

  if(gt1 < Amber_on){
  bin_1 = 0;}

  else if(Amber_on < gt1 && gt1 <= Amber_max){
  bin_1 = 1;}

  else if(Amber_max < gt1 && gt1 <= Green_on_min){
  bin_1 = 2;}

   else if(Green_on_min < gt1 && gt1 <= Green_on_max){
  bin_1 = 4;}
  
  else if (Green_on_max < gt1 && gt1 < Green_f){ 
   bin_1 = 3;}

  else if (gt1> Green_f){ 
   bin_1 = 5;}
   
  else {
   bin_1 = 0;}
 
 LoRaMessage = String(gt1) + ";" + String(id) + '@' +  String(bin_1)+ '#' +  String(bin_2);
 Serial.println(LoRaMessage);
 sendData();
}

void sendData(){
 LoRa.beginPacket();
 LoRa.print(LoRaMessage);
 LoRa.endPacket(); 
 delay(100);  
}
