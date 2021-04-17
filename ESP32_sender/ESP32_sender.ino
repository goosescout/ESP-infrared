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

/* TUNABLE PARAMETERS */
#define sender_pin 27  // любой свободный пин
#define rx_pin 16  // rx пин, лучше не менять
#define tx_pin 17  // tx пин, лучше не менять

#define volume_level 20  // уровень громкости, 0-30
#define responce_time 500  // время в мс, через которое отправляются запросы по инфракрасному каналу
/* END OF TUNABLE PARAMETERS */

AsyncWebServer server(80);

IRsend irsend(sender_pin);

HardwareSerial mySoftwareSerial(1);
DFRobotDFPlayerMini myDFPlayer;

void printDetail(uint8_t type, int value);

bool player_started = false;

void processRequest(AsyncWebServerRequest *request) {
    AsyncWebParameter* param = request->getParam(0);
    if (param->value() == "1") {
        if (player_started) {
            myDFPlayer.start();
        } else {
            // проигрывание музыки (папка с номером 01):
            myDFPlayer.loopFolder(1);
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
    mySoftwareSerial.begin(9600, SERIAL_8N1, rx_pin, tx_pin);  // speed, type, RX, TX
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
    myDFPlayer.setTimeOut(500);
    myDFPlayer.volume(volume_level);  // громкость (0-30)
}

void loop(void) {
    static unsigned long timer = millis();
    if (millis() - timer > responce_time) {
        timer = millis();
        irsend.sendNEC(0x00FFE01FUL);  // data - FFE01F
    }
    if (myDFPlayer.available()) {
        printDetail(myDFPlayer.readType(), myDFPlayer.read());
    }
}

void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}
