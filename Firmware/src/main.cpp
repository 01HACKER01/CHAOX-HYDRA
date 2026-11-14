#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_MCP23X17.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>
#include "board_pins.h"

Adafruit_MCP23X17 mcp;
Adafruit_LIS3DH lis;

bool mcpPresent = false;
bool lisPresent = false;
uint32_t lastStatus = 0;
uint16_t previousButtons = 0xFFFF;

void scanI2C() {
  Serial.println("\nI2C scan:");
  uint8_t found = 0;

  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);

    if (Wire.endTransmission() == 0) {
      Serial.printf("Found device at 0x%02X\n", addr);
      found++;
    }
  }

  if (!found) Serial.println("No I2C devices detected.");
}

void initMCP() {
  mcpPresent = mcp.begin_I2C(Pins::MCP23017_ADDR, &Wire);

  if (!mcpPresent) {
    Serial.println("MCP23017 not found at 0x20");
    return;
  }

  mcp.pinMode(Pins::RADIO_CS, OUTPUT);
  mcp.digitalWrite(Pins::RADIO_CS, HIGH);

  mcp.pinMode(Pins::RADIO_GDO0, INPUT);
  mcp.pinMode(Pins::RADIO_GDO2, INPUT);

  mcp.pinMode(Pins::BUZZER, OUTPUT);
  mcp.digitalWrite(Pins::BUZZER, LOW);

  for (uint8_t i = 0; i < Pins::BUTTON_COUNT; i++) {
    mcp.pinMode(Pins::BUTTON_FIRST + i, INPUT_PULLUP);
  }

  Serial.println("MCP23017 initialized.");
}

void initLIS3DH() {
  lisPresent = lis.begin(Pins::LIS3DH_ADDR);

  if (lisPresent) {
    lis.setRange(LIS3DH_RANGE_2_G);
    Serial.println("LIS3DH detected at 0x18");
  } else {
    Serial.println("LIS3DH not detected at 0x18");
  }
}

void reportButtons() {
  if (!mcpPresent) return;

  uint16_t state = 0;

  for (uint8_t i = 0; i < Pins::BUTTON_COUNT; i++) {
    if (mcp.digitalRead(Pins::BUTTON_FIRST + i) == LOW) {
      state |= (1U << i);
    }
  }

  if (state != previousButtons) {
    Serial.print("Button state: ");

    for (int i = 6; i >= 0; i--) {
      Serial.print((state >> i) & 1);
    }

    Serial.println();
    previousButtons = state;
  }
}

void reportSensors() {
  if (!lisPresent) {
    Serial.println("LIS3DH unavailable.");
    return;
  }

  sensors_event_t event;
  lis.getEvent(&event);

  Serial.printf(
    "Acceleration (m/s^2): X=%.2f Y=%.2f Z=%.2f\n",
    event.acceleration.x,
    event.acceleration.y,
    event.acceleration.z
  );
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n=== CHAOX-HYDRA Diagnostics ===");

  Wire.begin(Pins::I2C_SDA, Pins::I2C_SCL);
  Wire.setClock(100000);

  scanI2C();
  initMCP();
  initLIS3DH();

  SPI.begin(
    Pins::SPI_SCK,
    Pins::SPI_MISO,
    Pins::SPI_MOSI
  );

  pinMode(Pins::IR_RX, INPUT);
  pinMode(Pins::IR_TX, OUTPUT);
  digitalWrite(Pins::IR_TX, LOW);

  Serial.println("SPI initialized.");
  Serial.println("Display driver not configured.");
  Serial.println("Radio and IR transmission disabled.");
  Serial.println("Commands: i = I2C scan, s = sensor, h = help");
}

void loop() {
  reportButtons();

  if (Serial.available()) {
    char c = Serial.read();

    if (c == 'i' || c == 'I') scanI2C();
    if (c == 's' || c == 'S') reportSensors();

    if (c == 'h' || c == 'H') {
      Serial.println("i = I2C scan");
      Serial.println("s = sensor reading");
      Serial.println("h = help");
    }
  }

  if (millis() - lastStatus >= 5000) {
    lastStatus = millis();

    Serial.printf(
      "Status: MCP23017=%s, LIS3DH=%s\n",
      mcpPresent ? "OK" : "NOT FOUND",
      lisPresent ? "OK" : "NOT FOUND"
    );
  }

  delay(10);
}
