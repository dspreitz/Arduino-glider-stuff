// This example illustates basic use of the NMEA library.
// It assumes that a GPS receiver is connected to serial
// port 'Serial1' at 4800 bps, and that a LED is connected
// to digital i/o pin 0.
//
// A GPS data connection of type GPRMC is created, and
// used to get position information. When the GPS position
// is in the state of Colorado (USA), the LED lights up,
// otherwise it is off.

#include <nmea.h>

// create a GPS data connection to GPRMC sentence type
NMEA gps(ALL);

void setup() {
  Serial.begin(19200);
  Serial1.begin(19200);
}

void loop() {
  if (Serial1.available() > 0 ) {
    // read incoming character from GPS
    char c = Serial1.read();
    // Serial.write(c);
    // check if the character completes a valid GPS sentence
    // Serial.println(gps.decode(c));
    // Serial.print(c);
    if (gps.decode(c)) {
      // Serial.println(gps.decode(c));
      // check if GPS positioning was active
      if (gps.gprmc_status() == 'A') {
        Serial.print(gps.gpgga_utc(),0);
        Serial.print(" ");
        Serial.print(gps.gpgga_latitude(),6);
        Serial.print(" ");
        Serial.print(gps.gpgga_longitude(),6);
        Serial.print(" ");
        Serial.print(gps.gpgga_alt(),0);
        Serial.print(" ");
        Serial.println(gps.pgrmz_alt(),0);
      }
    } else {
        // Serial.print(c);
    }
  }
}
