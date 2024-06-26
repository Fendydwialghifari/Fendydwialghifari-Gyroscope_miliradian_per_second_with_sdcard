//Library sensor gyro
#include <Wire.h>
#include <L3G.h>
// Library SD Card
#include <SPI.h>
#include <SD.h>

L3G gyro;

const int chipSelect = D8;

int numSamples = 500;  // Jumlah sampel yang akan digunakan untuk kalibrasi
float xOffset = 0.0, yOffset = 0.0, zOffset = 0.0;

unsigned long previousMillis = 0;
const long intervaltime = 1000;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);

  if (!gyro.init()) {
    Serial.println("Failed to autodetect gyro type!");
    while (1)
      ;
  }

  gyro.enableDefault();

  // Lakukan kalibrasi
  calibrateGyroOffsets();

  Serial.print("Initializing SD card...");

  //pesan status SDcard
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1)
      ;
  }
  Serial.println("card initialized.");
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= intervaltime) {
    float time_stamp = currentMillis / 1000.00;
    previousMillis = currentMillis;
    gyro.read();

    // Aplikasikan offset kalibrasi
    float xDPS = ((gyro.g.x * 8.75) / 1000) - xOffset;
    float yDPS = ((gyro.g.y * 8.75) / 1000) - yOffset;
    float zDPS = ((gyro.g.z * 8.75) / 1000) - zOffset;

    float xRAD = xDPS * (3.14159 / 180);
    float yRAD = yDPS * (3.14159 / 180);
    float zRAD = zDPS * (3.14159 / 180);

    float xmRAD = xRAD * 1000;
    float ymRAD = yRAD * 1000;
    float zmRAD = zRAD * 1000;

    Serial.print(time_stamp);
    Serial.print(",");
    Serial.print(xmRAD);
    Serial.print(",");
    Serial.print(ymRAD);
    Serial.print(",");
    Serial.println(zmRAD);


    File dataFile = SD.open("datalog.txt", FILE_WRITE);
    if (dataFile) {
      digitalWrite(D3, LOW);
      digitalWrite(D4, HIGH);
      dataFile.print(time_stamp);
      dataFile.print(",");
      dataFile.print(xmRAD);
      dataFile.print(",");
      dataFile.print(ymRAD);
      dataFile.print(",");
      dataFile.println(zmRAD);
      dataFile.close();
    } else {
      Serial.println("error opening datalog.txt");
      digitalWrite(D4, LOW);
      digitalWrite(D3, HIGH);
    }
  }
}

void calibrateGyroOffsets() {
  Serial.println("Starting Gyro Calibration...");
  delay(3000);  // Delay untuk memastikan sensor stabil setelah power-on

  gyro.read();  // Baca nilai pertama untuk memulai kalibrasi

  float xSum = 0, ySum = 0, zSum = 0;

  for (int i = 0; i < numSamples; ++i) {
    gyro.read();
    xSum += gyro.g.x;
    ySum += gyro.g.y;
    zSum += gyro.g.z;
    delay(10);  // Delay antara pembacaan sampel
  }

  xOffset = (xSum / numSamples) * (8.75 / 1000);
  yOffset = (ySum / numSamples) * (8.75 / 1000);
  zOffset = (zSum / numSamples) * (8.75 / 1000);

  Serial.print("Offsets: ");
  Serial.print(xOffset);
  Serial.print(", ");
  Serial.print(yOffset);
  Serial.print(", ");
  Serial.println(zOffset);

  Serial.println("Gyro Calibration Complete.");
}
