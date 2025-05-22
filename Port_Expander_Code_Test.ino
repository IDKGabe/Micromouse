#include <Adafruit_MCP23X17.h>

#define LED_PIN 0
#define BUTTON_PIN 16

Adafruit_MCP23X17 mcp;

void setup() {
Serial.begin(9600);

if (!mcp.begin_I2C()) {
  Serial.println("Error initializing MCP23X17!");
  while (1);
}

mcp.pinMode(LED_PIN, OUTPUT);
mcp.pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
if (mcp.digitalRead(BUTTON_PIN) != LOW) {
  Serial.println("Button pressed!");
} else {
  Serial.println("Button not pressed!");
}

mcp.digitalWrite(LED_PIN, !mcp.digitalRead(BUTTON_PIN));
//delay (500);
}
