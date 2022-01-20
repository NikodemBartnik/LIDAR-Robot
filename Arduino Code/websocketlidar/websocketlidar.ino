#include <ArduinoHttpClient.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h"

//Max lidar resolution is 752, to make it easier to send and display in pygame program reduces it to 188 (752/4 = 188)
#define LIDAR_RESOLUTION 80
#define FRAME_LENGTH 120

#define DIR_OUT1 6
#define DIR_OUT2 7

//### WIFI
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

char serverAddress[] = "192.168.0.107";  // server address (check with ipconfig (windows) or ifconfig (mac, linux))
int port = 8084;

WiFiClient wifi;
WebSocketClient client = WebSocketClient(wifi, serverAddress, port);
int status = WL_IDLE_STATUS;


//### LIDAR
int lidar_frame[FRAME_LENGTH];
int distances[LIDAR_RESOLUTION];
int signals[LIDAR_RESOLUTION];

void setup() {
  pinMode(DIR_OUT1, OUTPUT);
  pinMode(DIR_OUT2, OUTPUT);

  //Serial monitor
  Serial.begin(9600);
  //LIDAR serial
  Serial1.begin(230400);
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);  // print the network name (SSID);
    status = WiFi.begin(ssid, pass);
  }

  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  //setting all distance in the array to 0
  for (int a = 0; a < LIDAR_RESOLUTION; a++) {
    distances[a] = 0;
    signals[a] = 50;
  }
}

long last_send = 0;
void loop() {
  Serial.println("Working");
  Serial.println("starting WebSocket client");
  client.begin();
  while (client.connected()) {
    if (Serial1.read() == 170) {
      GetLidarData();
      if(lidar_frame[2] == 1 && lidar_frame[3] == 97){
      int circleSegment = ((lidar_frame[10]*256+lidar_frame[11])/2250);
      int points = (lidar_frame[6] - 5) / 3;
      if(points > 30 && circleSegment < 16){
        for(int p = 0; p < 5; p++){
            int d = (lidar_frame[13 + p*18]*256 + lidar_frame[14 + p*18]) * 0.25;
            if(d < 8000){           
              signals[circleSegment*5+p] = lidar_frame[12+p*18];
              distances[circleSegment*5+p] = d;
            }
          
        }
      }
    }

        
       if((millis() - last_send) > 50){
         String toPython = "";
        for (int a = 0; a < LIDAR_RESOLUTION; a++) {
          toPython.concat(distances[a]);
          toPython.concat(",");
          toPython.concat(signals[a]);
          toPython.concat(",");
        }
        client.beginMessage(TYPE_TEXT);
        client.print(toPython);
        client.endMessage();
        last_send = millis();
       }
      }

    // check if a message is available to be received
    int messageSize = client.parseMessage();

    if (messageSize > 0) {
      String dir = client.readString();
      Serial.println("Received a message:");
      Serial.println(dir);
      if(dir == "left"){
        Serial.println("Turning left");
        digitalWrite(DIR_OUT1, HIGH);
        digitalWrite(DIR_OUT2, LOW);
      }
      if(dir == "right"){
        Serial.println("Turning right");
        digitalWrite(DIR_OUT1, LOW);
        digitalWrite(DIR_OUT2, HIGH);
      }
      if(dir == "forward"){
        Serial.println("Going forward");
        digitalWrite(DIR_OUT1, HIGH);
        digitalWrite(DIR_OUT2, HIGH);
      }
      if(dir == "stop"){
        Serial.println("stopping");
        digitalWrite(DIR_OUT1, LOW);
        digitalWrite(DIR_OUT2, LOW);
      }
    }
}
Serial.println("disconnected");
delay(5000);
}

void GetLidarData() {
  for (int a = 0; a < FRAME_LENGTH; a++) {
    while (!Serial1.available());
    lidar_frame[a] = Serial1.read();
  }
}
