#include <Wire.h>

// Define AHT10 register addresses
#define AHT10_ADDR 0x38
#define CMD_MEASURE 0xAC
#define CMD_SOFTRESET 0xBA
#define STATUS_BUSY_MASK 0x80

// Define I2C0 pins
#define SDA_PIN 21
#define SCL_PIN 22

// Global variables for sensor data
float temperature = 0.0;
float humidity = 0.0;

// Function to read AHT10 sensor
void readAHT10() {
  // Send measurement command to AHT10
  Wire.beginTransmission(AHT10_ADDR);
  Wire.write(CMD_MEASURE);
  Wire.write(0x33);
  Wire.write(0x00);
  Wire.endTransmission();
  
  // Wait for measurement to complete
  while (true) {
    Wire.beginTransmission(AHT10_ADDR);
    Wire.write(0x71);
    Wire.endTransmission(false);
    Wire.requestFrom(AHT10_ADDR, 1);
    if ((Wire.read() & STATUS_BUSY_MASK) == 0) break;
    delay(10);
  }

  // Read measurement data from AHT10
  Wire.beginTransmission(AHT10_ADDR);
  Wire.write(0x00);
  Wire.endTransmission(false);
  Wire.requestFrom(AHT10_ADDR, 6);
  uint8_t msb = Wire.read();
  uint8_t lsb = Wire.read();
  uint8_t cksum = Wire.read();
  uint32_t raw_data = ((msb << 16) | (lsb << 8) | cksum) >> 4;

  // Convert raw data to temperature and humidity
  temperature = (float)(raw_data * 200.0 / 1048576.0) - 50.0;
  humidity = (float)(raw_data * 100.0 / 1048576.0);
}

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Initialize I2C communication on I2C0 with custom SDA and SCL pins
  Wire.begin(SDA_PIN, SCL_PIN);
  
  // Soft reset AHT10
  Wire.beginTransmission(AHT10_ADDR);
  Wire.write(CMD_SOFTRESET);
  Wire.endTransmission();
  delay(20);
}

void loop() {
  // Read data from AHT10
  readAHT10();

  // Print temperature and humidity values to serial monitor
  Serial.print("Temperature: ");
  Serial.print(temperature, 2); // round to 2 decimal places
  Serial.print(" C, Humidity: ");
  Serial.print(humidity, 2); // round to 2 decimal places
  Serial.println(" %");

  // Wait 1 second before taking another measurement
  delay(1000);
}
