#include <IRremoteESP8266.h>
#include <IRsend.h>

#define sender_pin 27  // any empty pin

IRsend irsend(sender_pin);

uint16_t raw_data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

void setup(void) {
    Serial.begin(115200);
    Serial.println("Starting ESP32 and Infrared...");
    irsend.begin();
}

void loop(void) {
    Serial.println("Sending RAW data...");
    irsend.sendRaw(raw_data, 10, 38);
    delay(2000);
}
