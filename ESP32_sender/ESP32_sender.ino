#include "IRremoteESP8266.h"
#include "IRsend.h"

#include "WiFi.h"
#include "AsyncTCP.h"
#include "WiFiClient.h"
#include "ESPAsyncWebServer.h"

#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

#define SERVER_NAME "ESP32_infared_server"
#define SERVER_PASSWORD "2A(2v_!!*qaL"

#define sender_pin 27  // any empty pin

AsyncWebServer server(80);

IRsend irsend(sender_pin);

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

bool player_started = false;

void processRequest(AsyncWebServerRequest *request) {
    AsyncWebParameter* param = request->getParam(0);
    if (param->value() == "1") {
        if (player_started) {
            myDFPlayer.start();
        } else {
            myDFPlayer.loop(1);
            player_started = true;
        }
    } else {
        myDFPlayer.pause();
    }
    Serial.print("param value recieved: ");
    Serial.println(param->value());

    request->send(200, "text/plain", "success");
}

void setup(void) {
    Serial.begin(115200);
    Serial.println("Starting ESP32 and Infrared...");
    irsend.begin();

    WiFi.mode(WIFI_AP);
    WiFi.softAP(SERVER_NAME, SERVER_PASSWORD);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

    server.on("/", HTTP_GET, processRequest);
    server.begin();

    myDFPlayer.begin(mySoftwareSerial);
    Serial.println("DFPlayer Mini online");
    myDFPlayer.setTimeOut(100);  // timeout - 100ms
    myDFPlayer.volume(10);  // set volume value (0-30).
}

void loop(void) {
    static unsigned long timer = millis();
    if (millis() - timer > 500) {
        timer = millis();
        irsend.sendNEC(0x000105FFUL);  // data - 0105FF
    }
}
