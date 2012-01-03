#include <Wire.h>
#include <nmea.h>
#include<stdlib.h>
#include <SD.h>

/*
 Programm to measure and transmit Humidity and Temperature 
 via HYT228 on request via Serial Communication.
 
 Not optimized for high speed measurement. A minimum dely 
 of 100 ms is currently hard coded in the getData() function
 
 TODO:
 - Add functionalty for high speed measurement. The HYT221
 indicates if a new measurement is ready via the status/stall bit
 see documentation of HYT221
 - Do not use a global variable for temperature and humidty. Use
 a function with an array to return the data.
 - Think on reducing number of variables used.
 
 */



// I2C adress of Temp & Humidity sensor: 0x28
#define HYT221_ADDR 0x28
const int chipSelect = 4;

// for Temperature and Humidity
double Humidity;
double Temperature;

void getData(){
  // initiate measurement
  Wire.beginTransmission(HYT221_ADDR);
  Wire.write(1);
  Wire.available();
  int Ack = Wire.read(); // receive a byte

    Wire.endTransmission(); 
  // delay(100);

  // READ DATA from here on
  Wire.beginTransmission(HYT221_ADDR);
  Wire.requestFrom(HYT221_ADDR,4);  //Reade 1 byte
  Wire.available();
  int b1 = Wire.read(); // receive a byte
  int b2 = Wire.read(); // receive a byte
  int b3 = Wire.read(); // receive a byte
  int b4 = Wire.read(); // receive a byte
  Wire.write(1); //NACK

  // combine the bits
  int RawHumidBin = b1 << 8 | b2;
  // compound bitwise to get 14 bit measurement first two bits
  // are status/stall bit (see intro text)
  RawHumidBin =  (RawHumidBin &= 0x3FFF); 
  Humidity = 100.0/pow(2,14)*RawHumidBin;

  b4 = (b4 &= 0x3F); //Mask away 2 least sign. bits see HYT 221 doc
  int RawTempBin = b3 << 6 | b4;
  Temperature = 165.0/pow(2,14)*RawTempBin-40;

  Wire.endTransmission();
}

// create a GPS data connection to GPRMC sentence type
NMEA gps(ALL);

//setup
void setup(){
  // Setup Serial connection
  Serial.begin(19200); // Serial to PC
  Serial1.begin(19200); // UART o FLARM
  Wire.begin();             // join I2C bus for Temp & Humidity
  
  // Initialize the SD card
  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
}

void loop(){

  // make a string for assembling the data to log:
  String dataString = "";
  char test[15];
  
  if (Serial1.available() > 0 ) {
    // read incoming character from GPS
    getData();

    char c = Serial1.read();
    // Serial.print(c);
    if (gps.decode(c)) {
      // Serial.println(gps.decode(c));
      // check if GPS positioning was active
      if (gps.gprmc_status() == 'A') {
        // Compile string with restuls
        // GPS time in UTC
        dtostrf(gps.gprmc_utc(),6,0,test);
        dataString += test;
        dataString += " ";
        // Lat.
        dtostrf(gps.gpgga_latitude(),9,6,test);
        dataString += test;
        dataString += " ";
        // Lon.
        dtostrf(gps.gpgga_longitude(),9,6,test);
        dataString += test;
        dataString += " ";
        // GPS alt in meter
        dtostrf(gps.gpgga_alt(),4,0,test);
        dataString += test;
        dataString += " ";
        // Pressure alt from FLARM in feet
        dtostrf(gps.pgrmz_alt(),5,0,test);
        dataString += test;
        dataString += " ";
       // relative Humitity in % 
        dtostrf(Humidity,6,2,test);
        dataString += test;
        dataString += " ";
       // Temperature in deg C 
        dtostrf(Temperature,5,2,test);
        dataString += test;
        dataString += " ";
       
        // Serial.print(gps.gprmc_utc(),0);
        // Serial.print(" ");
        // Serial.print(gps.gpgga_latitude(),6);
        // Serial.print(" ");
        // Serial.print(gps.gpgga_longitude(),6);
        // Serial.print(" ");
        // Serial.print(gps.gpgga_alt(),0);
        // Serial.print(" ");
        // Serial.print(gps.pgrmz_alt(),0);
        // Serial.print(" ");
        // Serial.print(Humidity);
        // Serial.print(" ");
        // Serial.println(Temperature);
        
        // open the file. note that only one file can be open at a time,
        // so you have to close this one before opening another.
        File dataFile = SD.open("datalog.txt", FILE_WRITE);

        // if the file is available, write to it:
        if (dataFile) {
          dataFile.println(dataString);
          dataFile.close();
          // print to the serial port too:
          Serial.println(dataString);
        }  
          // if the file isn't open, pop up an error:
        else {
          Serial.println("error opening datalog.txt");
        } 
        
      }
    } else {
        // Serial.print(c);
    }
  }
  

}
