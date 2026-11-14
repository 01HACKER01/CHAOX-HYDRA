#pragma once
#include <Arduino.h>

namespace Pins {
  constexpr int TFT_CS = 1;
  constexpr int TFT_DC = 0;
  constexpr int TFT_RST = 25;
  constexpr int SD_CS = 7;

  constexpr int I2C_SDA = 23;
  constexpr int I2C_SCL = 24;

  constexpr int IR_RX = 11;
  constexpr int IR_TX = 12;

  constexpr int SPI_SCK = 8;
  constexpr int SPI_MISO = 9;
  constexpr int SPI_MOSI = 10;

  constexpr uint8_t MCP23017_ADDR = 0x20;
  constexpr uint8_t LIS3DH_ADDR = 0x18;

  constexpr uint8_t RADIO_CS = 0;
  constexpr uint8_t RADIO_GDO0 = 1;
  constexpr uint8_t RADIO_GDO2 = 2;
  constexpr uint8_t BUZZER = 3;

  constexpr uint8_t BUTTON_FIRST = 8;
  constexpr uint8_t BUTTON_COUNT = 7;
}
