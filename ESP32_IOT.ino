//Include libraries
#include <HTTPClient.h>               //Download: https://electronoobs.com/eng_arduino_httpclient.php
#include <WiFi.h>                     //Download: https://electronoobs.com/eng_arduino_wifi.php
#include "DHT.h"
#define DHTTYPE DHT11   // DHT 11
//Add WIFI data
const char* ssid = "DIGI_e31d4c";              //Add your WIFI network name 
const char* password =  "913daf51";           //Add WIFI password
#define DHTPIN 4              
// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);                
//Variables used in the code
String Temperature;
String Humidity;   
String data_to_sendTH = "";             //Text data to send to the server
 
unsigned int Actual_Millis, Previous_Millis;
int refresh_time = 200;               //Refresh rate of connection to website (recommended more than 1s)

void setup() {
  delay(10);
  Serial.begin(115200);                   //Start monitor
  dht.begin();                  
  
  WiFi.begin(ssid, password);             //Start wifi connection
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
    delay(500);
    Serial.print(".");
  }

  Serial.print("Connected, my IP: ");
  Serial.println(WiFi.localIP());
  Actual_Millis = millis();               //Save time for refresh loop
  Previous_Millis = Actual_Millis; 
}


void loop() {  
  //We make the refresh loop using millis() so we don't have to sue delay();
  Actual_Millis = millis();
  Temperature = dht.readTemperature(); // Gets the values of the temperature
  Humidity = dht.readHumidity(); // Gets the values of the humidity 
  if(Actual_Millis - Previous_Millis > refresh_time){
    Previous_Millis = Actual_Millis;  
    if(WiFi.status()== WL_CONNECTED){                   //Check WiFi connection status  
      HTTPClient http;                                  //Create new client
      data_to_sendTH = "temp=" + Temperature +"&hum=" + Humidity;  
      //Begin new connection to website       
      http.begin("https://esp32th.000webhostapp.com/esp32_update.php");   //Indicate the destination webpage 
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");         //Prepare the header
      int response_codeTH = http.POST(data_to_sendTH);                               //Send the POST. This will giveg us a response code
      delay(2000);
      //data_to_sendTH = "updateDataTH";
      //int response_codeH = http.POST(data_to_sendTH);
      
      if(response_codeTH > 0){
        Serial.println("HTTP code " + String(response_codeTH)); 
        if(response_codeTH == 200){                                                 //If code is 200, we received a good response and we can read the echo data
          String response_body = http.getString();                                //Save the data comming from the website
          Serial.print("Server reply: ");                                         //Print data to the monitor for debug
          Serial.println(response_body);
        }
      } 
      
      http.end();                                                                 //End the connection
    }//END of WIFI connected
    else{
      Serial.println("WIFI connection error");
    }
  }
}

