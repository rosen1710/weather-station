#include "SPI.h"
#include "nRF24L01.h"
#include "RF24.h"
#include "SFE_BMP180.h"

#define CONNECTION_TIMEOUT_SECONDS 600

#define CE_RADIO_PIN 9
#define CSN_RADIO_PIN 10

RF24 radio(CE_RADIO_PIN, CSN_RADIO_PIN); // CE_PIN, CSN_PIN

SFE_BMP180 sensor; // BMP180 I2C device

struct Data {
  double temperature;
  double pressure;
};

const byte writingAddress[6] = "00001";

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(writingAddress);
  radio.setPALevel(RF24_PA_MAX); // RF24_PA_MIN or RF24_PA_LOW or RF24_PA_HIGH or RF24_PA_MAX
  sensor.begin();
}

void loop() {
  struct Data data;

  delay(sensor.startTemperature());
  sensor.getTemperature(data.temperature);

  delay(sensor.startPressure(3));
  sensor.getPressure(data.pressure, data.temperature);

  radio.write(&data, sizeof(struct Data));

  Serial.print("\nSend (");
  Serial.print(millis() / 1000);
  Serial.print(" seconds since start):");
  Serial.print("\n\tTemperature: ");
  Serial.print(data.temperature);
  Serial.print("°C");
  Serial.print("\n\tPressure: ");
  Serial.print(data.pressure);
  Serial.println(" hPa (mb)");

  delay(CONNECTION_TIMEOUT_SECONDS * 1000);
}