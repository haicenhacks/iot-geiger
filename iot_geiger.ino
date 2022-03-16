#include <TimeLib.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h> 
#include "wifi_details.h"

int old_min=-1;
int current_min=0;
bool prev_status;
bool curr_status;
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);
int pulse_count=0;


WiFiClientSecure client;
void send_data()
{  
  client.setInsecure(); //the magic line, use with caution
  client.connect(api_url, 443);
     
  
  Serial.println("sending data");
  

  String postData = "pass="+String(api_key) + "&device_id="+String(SEN_ID)+"&cpm=" + String(pulse_count); 
   
  Serial.println(postData);
  HTTPClient http;
  http.begin(client, api_url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded"); 
  auto httpCode = http.POST(postData); 
  Serial.println(httpCode); //Print HTTP return code 

}
    
void setup() {
  // put your setup code here, to run once:
  pinMode(15, INPUT);
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK );    //WiFi connection
  
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)

  delay(1000);
  display.display();
  delay(1000);

  // Clear the buffer.
  display.clearDisplay();
  display.display();



  // text display tests
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Wait");
  display.display();
  


}

void loop() {
  // put your main code here, to run repeatedly:
  time_t t = now();
  current_min = minute(t);
  if (current_min != old_min)
  {
    display.clearDisplay();
    display.display();

    display.setCursor(0,0);
    display.print("CPM: ");
    display.println(pulse_count);
    display.display();
    send_data();
    pulse_count = 0;
    old_min = current_min;
  }
  curr_status = digitalRead(15);
  if (curr_status != prev_status)
  {
    if (curr_status & !prev_status)
    {
      // rising
      prev_status = curr_status;
      pulse_count++;
      
    }
    else //falling
    {
      prev_status = curr_status;
    }
  }
    
  
}
