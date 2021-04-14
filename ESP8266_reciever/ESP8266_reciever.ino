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

String previously_sent;

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
    uint64_t recieved_data = NAN;
    if (irrecv.decode(&results)) {
        dump(&results);
        serialPrintUint64(results.value, HEX);
        Serial.println("");
        recieved_data = results.value;
        irrecv.resume();
    }

    String server_adress = SERVER_ACCESS;
    server_adress += "?connected=";
    server_adress += (recieved_data == 0xFFE01F) ? "1" : "0";

    if (WiFi.status() == WL_CONNECTED && previously_sent != server_adress) {
        WiFiClient client;
        HTTPClient http;
        http.begin(client, server_adress);
        int httpCode = http.GET();
        if (httpCode > 0) {
            previously_sent = server_adress;
            Serial.println("Success");
        } else {
            Serial.print("Error: ");
            Serial.println(httpCode);
        }
        http.end();
    }

    delay(500);
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
        Serial.print("NOOO... MY DECODING COLLECTION: ");
        Serial.print(results->decode_type);
        Serial.print(" ");
    }
}
