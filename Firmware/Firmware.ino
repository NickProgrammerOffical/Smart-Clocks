#include <DHT.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <Adafruit_ST7735.h>
#include <Adafruit_GFX.h>
#include <WiFiUdp.h>
#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeMono18pt7b.h>

#define RST 2
#define DC 5
#define CS 15

#define DHTtype DHT22
#define DHTpin 4

const long utcOffsetInSeconds = 3600;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

String air_temp;
String air_temp_F;
String air_humidity;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

String months[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

String res_time;

int mid_val_of_humidity = 0;
int mid_val_of_temp_C = 0;
int mid_val_of_temp_F = 0;

DHT dht(DHTpin, DHTtype);

Adafruit_ST7735 tft = Adafruit_ST7735(CS, DC, RST);

//SETTINGS
 #define WIFI_SSID "MySSID"
 #define WIFI_PASS "Password"
 String display_time = "24/H"; // 24/H or AM/PM
 int GMT = 7;
//SETTINGS

void disp_centered_text(String text, int h) {
  int16_t x1;
  int16_t y1;
  uint16_t width;
  uint16_t height;

  tft.getTextBounds(text, 0, 0, & x1, & y1, & width, & height);
  tft.setCursor((160 - width) / 2, h);
  tft.print(text);
}

void setup() {
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  
  Serial.begin(115200);
  Serial.println();
  dht.begin();
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting..");
  }
  timeClient.begin();
  Serial.println("Connected");

  air_temp = String(float(dht.readTemperature()))+"°C";
  air_temp_F = String(((float(dht.readTemperature()))*1.8)+32)+"°F";
  air_humidity = String(float(dht.readHumidity()))+"%";
}

void loop() {
  res_time = "";
  timeClient.update();

  tft.fillScreen(0x0000);
  tft.setTextColor(0xffff);
  tft.setTextSize(3);

  if(display_time=="24/H"){
  if(String((timeClient.getHours()+GMT)-1).length()==1){
    res_time += "0"+String((timeClient.getHours()+GMT)-1)+":";
  }else{
    res_time += String((timeClient.getHours()+GMT)-1)+":";
  }
  if(String(timeClient.getMinutes()).length()==1){
    res_time += "0"+String(timeClient.getMinutes())+":";
  }else{
    res_time += String(timeClient.getMinutes())+":";
  }
  if(String(timeClient.getSeconds()).length()==1){
    res_time += "0"+String(timeClient.getSeconds());
  }else{
    res_time += String(timeClient.getSeconds());
  }
  }
  
  if(display_time=="AM/PM"){
  if(timeClient.getHours()+GMT-1<13){
    res_time += String((timeClient.getHours()+GMT)-1)+":";
    if(String(timeClient.getMinutes()).length()==1){
      res_time += "0"+String(timeClient.getMinutes())+"AM";
    }else{
      res_time += String(timeClient.getMinutes())+"AM";
    }
  }
  if(timeClient.getHours()+GMT-1>12){
    res_time += String((timeClient.getHours()+GMT)-13)+":";
    if(String(timeClient.getMinutes()).length()==1){
      res_time += "0"+String(timeClient.getMinutes())+"PM";
    }else{
      res_time += String(timeClient.getMinutes())+"PM";
    }
  }
  }
  
  
  disp_centered_text(res_time, 10);
  tft.setTextSize(1);
  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime); 
  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon+1;
  String currentMonthName = months[currentMonth-1];
  int currentYear = ptm->tm_year+1900;
  disp_centered_text(String(daysOfTheWeek[timeClient.getDay()])+", "+currentMonthName+", "+String(currentYear), 52);

  tft.setTextSize(1);
  air_temp = String(float(dht.readTemperature()))+"C";
  air_temp_F = String(((float(dht.readTemperature()))*1.8)+32)+"F";
  disp_centered_text("Temperature: "+air_temp_F, 80);
  disp_centered_text("             "+air_temp, 95);
  air_humidity = String(float(dht.readHumidity()))+"%";
  disp_centered_text("Humidity: "+air_humidity, 110);
  
  delay(999);
}
