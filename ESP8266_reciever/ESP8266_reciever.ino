#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

#define reciever_pin D5  // GPIO 14

IRrecv irrecv(reciever_pin);

decode_results results;

void setup(void) {
    Serial.begin(115200);
    Serial.println("Starting ESP8266 and Infrared...");
    irrecv.enableIRIn();
}

void loop(void) {
    if (irrecv.decode(&results)) {
        serialPrintUint64(results.value, HEX);
        Serial.println("");
        irrecv.resume();
    }
    delay(100);
}
