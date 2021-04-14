#include "IRremoteESP8266.h"
#include "IRrecv.h"
#include "IRutils.h"

#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "WiFiClient.h"

#define reciever_pin D5  // GPIO 14

#define SERVER_NAME "ESP32_infared_server"
#define SERVER_PASSWORD "2A(2v_!!*qaL"

#define SERVER_ACCESS "http://192.168.4.1/"  // server address

IRrecv irrecv(reciever_pin);

decode_results results;

void setup(void) {
    Serial.begin(115200);
    Serial.println("Starting ESP8266 and Infrared...");
    irrecv.enableIRIn();
    /* irrecv.blink13(true); */

    WiFi.mode(WIFI_STA);
    WiFi.begin(SERVER_NAME, SERVER_PASSWORD);
    Serial.print("Connecting...");
    while (WiFi.status() != WL_CONNECTED) { 
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
}

void loop(void) {
    if (irrecv.decode(&results)) {
        dump(&results);
        serialPrintUint64(results.value, HEX);
        Serial.println("");

        if (WiFi.status() == WL_CONNECTED) {
            WiFiClient client;
            HTTPClient http;
            String server_adress = SERVER_ACCESS;
            server_adress += "?connected=";
            server_adress += (results.value == 0x0105FF) ? "1" : "0";  // TODO: check if it actually works
            http.begin(client, server_adress);
            int httpCode = http.GET();
            if (httpCode > 0) {
                Serial.println("Success");
            } else {
                Serial.print("Error: ");
                Serial.println(httpCode);
            }
            http.end();
        }

        irrecv.resume();
    }
    delay(100);
}

void dump(decode_results *results) {
    int count = results->rawlen;
    if (results->decode_type == UNKNOWN) {
        Serial.print("Unknown encoding: ");
    }
    else if (results->decode_type == NEC) {
        Serial.print("Decoded NEC: ");
    }
    else if (results->decode_type == SONY) {
        Serial.print("Decoded SONY: ");
    }
    else if (results->decode_type == RC5) {
        Serial.print("Decoded RC5: ");
    }
    else if (results->decode_type == RC6) {
        Serial.print("Decoded RC6: ");
    } else {
        Serial.print("NOOO... MY DECODING COLLECTION ");
        Serial.print(results->decode_type);
        Serial.print(" ");
    }
}
