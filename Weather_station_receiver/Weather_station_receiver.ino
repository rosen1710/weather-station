#include "SPI.h"
#include "nRF24L01.h"
#include "RF24.h"
#include "dht.h"
#include "LiquidCrystal.h"

#define CONNECTION_TIMEOUT_SECONDS 10

#define DHT_SENSOR_DATA_PIN 8

#define LCD_PIN_DB4 2
#define LCD_PIN_DB5 3
#define LCD_PIN_DB6 4
#define LCD_PIN_DB7 5
#define LCD_PIN_E   6
#define LCD_PIN_RS  7

#define LED_PIN 13

#define CE_RADIO_PIN 48
#define CSN_RADIO_PIN 53

dht dht_sensor;

LiquidCrystal lcd(LCD_PIN_RS, LCD_PIN_E, LCD_PIN_DB4, LCD_PIN_DB5, LCD_PIN_DB6, LCD_PIN_DB7);

RF24 radio(CE_RADIO_PIN, CSN_RADIO_PIN); // CE_PIN, CSN_PIN

struct Data {
  double temperature;
  double pressure;
};

const byte readingAddress[6] = "00001";

struct Data data;

int timeout = 0;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, readingAddress);
  radio.setPALevel(RF24_PA_MAX); // RF24_PA_MIN or RF24_PA_LOW or RF24_PA_HIGH or RF24_PA_MAX
  radio.startListening();
  lcd.begin(16, 2);
}

void loop() {
  if(radio.available()) {
    radio.read(&data, sizeof(struct Data));

    Serial.print("\nReceived (");
    Serial.print(millis() / 1000);
    Serial.print(" seconds since start):");
    Serial.print("\n\tOutside temperature: ");
    Serial.print(data.temperature);
    Serial.print("°C");
    Serial.print("\n\tOutside pressure: ");
    Serial.print(data.pressure);
    Serial.println(" hPa (mb)");

    lcd.setCursor(0, 1);
    lcd.print("Out: ");
    lcd.print((int) data.temperature);
    lcd.print((char) 223); // °
    lcd.print(" ");
    lcd.print((int) data.pressure);
    lcd.print("hPa  ");

    timeout = CONNECTION_TIMEOUT_SECONDS;

    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(600);
  }

  dht_sensor.read11(DHT_SENSOR_DATA_PIN);

  Serial.print("\nMeasured (");
  Serial.print(millis() / 1000);
  Serial.print(" seconds since start):");
  Serial.print("\n\tInside temperature: ");
  Serial.print((int) dht_sensor.temperature);
  Serial.print("°C");
  Serial.print("\n\tInside humidity: ");
  Serial.print((int) dht_sensor.humidity);
  Serial.println("%");

  lcd.setCursor(0, 0);
  lcd.print("In:  ");
  lcd.print((int) dht_sensor.temperature);
  lcd.print((char) 223); // °
  lcd.print("  ");
  lcd.print((int) dht_sensor.humidity);
  lcd.print("%    ");

  timeout--;

  if(timeout <= 0) {
    lcd.setCursor(0, 1);
    lcd.print("Connection lost!");
    timeout = 0;
  }
  
  delay(1000);
}