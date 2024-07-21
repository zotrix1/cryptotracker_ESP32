#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <TFT_eSPI.h>
#include <background.h>

//SPI Pin Declaration is in User_Setup.h


//crypto prices databases
const String crypto_server = "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin,ethereum,solana&vs_currencies=usd";
String payload = "";


DynamicJsonDocument doc(1024);

const char* ssid = "Test_wifi";  //Wi-Fi SSID
const char* password = "1234567888"; //Wi-Fi password

const uint32_t price_refresh_time = 600000; //delay of price refresh (in ms)
uint32_t time_marker = 0;

float bitcoinPrice = 0.0;
float ethereumPrice = 0.0;
float solanaPrice = 0.0;

const uint16_t  gmtOffset_sec = 3600; //GMT Offset (in seconds)
const uint16_t   daylightOffset_sec = 3600; //Set it to 3600 if your country practise daylight saving time, otherwise set to 0
const char* ntpServer = "pool.ntp.org"; //address of NTP server

TFT_eSPI screen;

HTTPClient http;

void displayBackground() {
  uint16_t width = 320;
  uint16_t height = 240;

  screen.startWrite();
  screen.setAddrWindow(0, 0, width, height); // Set the area to be updated

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      screen.pushColor(image_data_solana_bmp[y * width + x]);
    }
  }
  
  screen.endWrite();
}
// void rising(int x, int y){
//   for(int i=0;i<49;i++){
//     if(rising_icon[i] == 1){
//       screen.drawPixel(x,y,TFT_GREEN);
//     }
//   }
// }
void network_status_bar() {
  screen.setTextSize(1);
  screen.drawString("Network status:",0,230);

  switch (WiFi.status()){
  case 0:
    screen.drawString("IDLE MODE        ",90,230);
    break;
  case 1:
    screen.drawString("No valid SSID    ",90,230);
    break;
  case 2:
    screen.drawString("Scan completed   ",90,230);
    break;
  case 3:
    screen.drawString("Connected        ",90,230);
    break;
  case 4:
    screen.drawString("Connection failed",90,230);
    break;
  case 5:
    screen.drawString("Connection lost  ",90,230);
    break;
  case 6:
    screen.drawString("Disconnected     ",90,230);
    break;
  case 255:
    screen.drawString("No WiFiShield det",90,230);
    break;
  default:
    screen.drawString("Unknown error    ",90,230);
    break;
  }
}
void time_bar() {
  
configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

struct tm timeinfo;

  if(!getLocalTime(&timeinfo)){
    Serial.println("Cant receive information about time");
    return;
  }
  screen.setTextSize(1);
  screen.drawString("Time:" + (String)(timeinfo.tm_hour/10)+ (String)(timeinfo.tm_hour%10) + ":" + (String)(timeinfo.tm_min/10) + (String)(timeinfo.tm_min%10) + ":" + (String)(timeinfo.tm_sec/10) + (String)(timeinfo.tm_sec%10),0,0);
}

void get_price(){
  

  if((millis()-time_marker) >= price_refresh_time || time_marker == 0) {

        http.begin(crypto_server.c_str());

      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
      
        payload = http.getString();
        //Serial.println(payload);
        DeserializationError err_BTC = deserializeJson(doc, payload);

        screen.setTextSize(3);

        bitcoinPrice = doc["bitcoin"]["usd"];
        ethereumPrice = doc["ethereum"]["usd"];
        solanaPrice = doc["solana"]["usd"];
        

        screen.drawString((String)bitcoinPrice + "$", 30, 30);
        screen.drawString((String)ethereumPrice + "$", 30, 110);
        screen.drawString((String)solanaPrice + "$", 30, 190);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      http.end();
      time_marker = millis();
     }
      
  }

void setup() {
  
  Serial.begin(9600);

  screen.begin();
  screen.setRotation(1);
  screen.fillScreen(TFT_BLACK);

  displayBackground();
  
  
  WiFi.begin(ssid,password);

}
void loop() {
  network_status_bar();
  if(WiFi.status() == WL_CONNECTED){
  time_bar();
  get_price();
  }
}

// put function definitions here:

// int myFunction(int x, int y) {
//   return x + y;
