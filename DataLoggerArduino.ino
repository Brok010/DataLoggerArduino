// Arduino UNO - VCC's connected to 3.3V and GND's to ground

// microSD Card modul SPI - MOSI pin 11, MISO pin 12, SCK pin 13, CS pin 10
// RTC DS3231 AT24C32 - pin SDA-SDA, SCL - SCL, SQW - 2
// DHT21 - pinA0
// Bluetooth modul JDY-33 TTL - TX - pin3, RX - pin4, pwrc - 3.3V
// button - pin 7 to ground

//TODO: use sleep library for sleep mode and then rtc interupt to wake it back up after n amount of time.
//if a button is pushed turn on bluetooth module and send the last file on the SD card to the connected device

#include <DHT.h>
#include <Wire.h>
#include <RTClib.h>
#include <SD.h>
// #include <SoftwareSerial.h>

#define DHTTYPE DHT21

const int dhtPin = A0;
const int chipSelect = 10;
const int buttonPin = 7;
const int bluetoothTx = 3;
const int bluetoothRx = 4;

bool write_flag = true;
DHT dht(dhtPin, DHTTYPE);
RTC_DS3231 rtc;
File dataFile;
// SoftwareSerial bluetooth(bluetoothRx, bluetoothTx);

const int entries_in_hour = 30;

void setup() {
  Serial.begin(9600);
  Serial.println("serial began");

  pinMode(buttonPin, INPUT_PULLUP);
  // Initialize Bluetooth communication
  // bluetooth.begin(9600);

  // Initialize DHT sensor
  dht.begin();

  // Initialize RTC
  initialize_RTC();

  // Initialize SD card
  initialize_SD();
}

void loop() {
  delay(1000);
  if (digitalRead(buttonPin) == LOW) {
    Serial.println("Button pressed. Connecting to Bluetooth and sending last file name.");
    sendFileOverBluetooth();
  }
  write_to_SD();
}

void initialize_RTC() {
  Serial.println("RTC initialization start");
  
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  Serial.println("RTC initialization end");
}

void write_to_SD() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  int step = 60 / entries_in_hour;

  if (rtc.now().minute() % step == 0 && write_flag == true) {
    write_flag = false;
    // Check if the data file is available for writing
    if (dataFile) {
      // Format the data line
      String dataLine = getFormattedDateTime(rtc.now()) + ", " + String(temperature, 2) + ", " + String(humidity, 2);

      // Print data to Serial Monitor
      Serial.println("Writing to SD card: " + dataLine);

      // Write data to the file
      dataFile.println(dataLine);
      dataFile.flush();  // Ensure data is written to the card

      Serial.println("Write successful!");
    } else {
      Serial.println("Error writing to SD card!");
    }
  }
  if (rtc.now().minute() % step != 0) {
    write_flag = true;
  }
}

void initialize_SD() {
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

String getFormattedDateTime(DateTime dt) {
  return String(dt.year()) + "/" + String(dt.month()) + "/" + String(dt.day()) + " " + String(dt.hour()) + ":" + String(dt.minute()) + ":00"; //String(dt.second()
}

void sendFileOverBluetooth() {
  // // Rewind the dataFile to the beginning
  // dataFile.seek(0);

  // // Read the file line by line and send it over Bluetooth
  // while (dataFile.available()) {
  //   bluetooth.write(dataFile.read());
  // }

  // // Close the file
  // dataFile.close();
}
