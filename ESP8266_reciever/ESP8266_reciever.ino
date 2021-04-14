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
    irrecv.blink13(true);
}

void loop(void) {
    if (irrecv.decode(&results)) {
        dump(&results);
        serialPrintUint64(results.value, HEX);
        Serial.println("");
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
