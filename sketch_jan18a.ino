// Arduino UNO
// microSD Card modul SPI - MOSI - pin11, MISO - pin12, SCK - pin13, CS - pin10
// RTC DS3231 AT24C32 - SDA - pinSDA, SCL - SCLpin
// DTH11 - pinA0
// led diod - pin8
#include <DHT.h>
#include <Wire.h>
#include <RTClib.h>
#include <SD.h>

#define DHTTYPE DHT21
const int dhtPin = A0;
DHT dht(dhtPin, DHTTYPE);

RTC_DS3231 rtc;
File dataFile;

const int chipSelect = 10;

void setup() {
  Serial.begin(9600);

  // Initialize DHT sensor
  dht.begin();

  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  // Uncomment the next line to set the RTC to the date & time this sketch was compiled
   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  // Uncomment the next line to set the RTC to a specific date & time
  // rtc.adjust(DateTime(2022, 1, 25, 12, 0, 0));

  // Initialize SD card
  if (SD.begin(chipSelect)) {
    Serial.println("SD card initialized.");

    // Check for existing files with the base name "test"
    int fileIndex = 0;
    String fileNameBase = "test";
    String fileName;

    do {
      fileName = fileNameBase + String(fileIndex) + ".txt";
      fileIndex++;
    } while (SD.exists(fileName));

    // Create and open the new file
    dataFile = SD.open(fileName, FILE_WRITE);
    if (dataFile) {
      Serial.println("File created: " + fileName);
      dataFile.println("Date, Temperature (°C), Humidity (%)"); // Header
    } else {
      Serial.println("Error creating file!");
    }
  } else {
    Serial.println("Error initializing SD card!");
  }
}

void loop() {
  delay(2000);

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Print data to Serial Monitor
  Serial.print("Reading sensor data - ");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C, Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Check if the data file is available for writing
  if (dataFile) {
    // Format the data line
    String dataLine = getFormattedDateTime(rtc.now()) + ", " + String(temperature, 2) + ", " + String(humidity, 2);

    // Print data to Serial Monitor
    Serial.println("Writing to SD card: " + dataLine);

    // Write data to the file
    dataFile.println(dataLine);
    dataFile.flush(); // Ensure data is written to the card

    Serial.println("Write successful!");
  } else {
    Serial.println("Error writing to SD card!");
  }
}

String getFormattedDateTime(DateTime dt) {
  return String(dt.year()) + "/" + String(dt.month()) + "/" + String(dt.day()) + " " +
         String(dt.hour()) + ":" + String(dt.minute()) + ":" + String(dt.second());
}
