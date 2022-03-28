#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
  
#include <Arduino.h>
#include <string.h>
#include "DHT.h"
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
SoftwareSerial NodeMCU(D2,D3);
WebSocketsClient webSocket;
int mode = -1; //obstacle avoider

unsigned long messageInterval = 1000;
bool connected = false;
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_TEXT:
            if(!strcmp((char *)payload,"right")) 
            mode = 0;

            if(!strcmp((char *)payload,"left")) 
            mode = 1;

            if(!strcmp((char *)payload,"forward")) 
            mode = 2;
                        
            if(!strcmp((char *)payload,"backward")) 
            mode = 3;
            break; 
        case WStype_BIN:
            hexdump(payload, length);
            break;
    }
 
}
char char_array_user[255];
char char_array_pass[255];
char *data[]={char_array_user,char_array_pass};
char* serial_tochar(int choose_data) {
    while(Serial.available()==0) { }
    String str =Serial.readString();
    str.toCharArray(data[choose_data], str.length());
    return data[choose_data];
}

void connect_wifi() {
    char * username;
    Serial.println("Please enter the username: ");
    username = strtok(serial_tochar(0), " ");
    char * password;
    Serial.println("Please enter the password: ");
    password = strtok(serial_tochar(1), " ");
    WiFi.begin(username, password);

    uint8_t i = 0;
    while(WiFi.status() != WL_CONNECTED && i < 20) {
      Serial.print(".");
      delay(500);
      i++;
    }
}

void print_networks() {
    int networks_number = WiFi.scanNetworks();
    for(int i=0;i<networks_number;i++) 
    {
      Serial.print("ssid: ");
      Serial.print(WiFi.SSID(i));
      Serial.print(", strength: ");
      Serial.print(WiFi.RSSI(i));
      Serial.print("\n");
    }
}
void setup(){
	Serial.begin(9600);
	NodeMCU.begin(4800);
	pinMode(D2,INPUT);
	pinMode(D3,OUTPUT);
    for(uint8_t t = 4; t > 0; t--) {
        Serial.printf("Pease wait for %d seconds\n", t);
        Serial.flush();
        delay(1000);
    }
    while(WiFi.status() !=  WL_CONNECTED )
    {
        print_networks();
        connect_wifi();
    }
    Serial.print("Local IP: "); Serial.println(WiFi.localIP());
    // server address, port and URL
    webSocket.begin("damp-spire-67518.herokuapp.com", 80, "/sensor","text");
 
    // event handler
    webSocket.onEvent(webSocketEvent);
}
unsigned long lastUpdate = millis();
  int i = 120;

void loop(){
  webSocket.loop();
  if (lastUpdate+messageInterval<millis()){
	NodeMCU.print(mode);
	NodeMCU.println("\n");
	delay(30);
  lastUpdate = millis();
    }
}
