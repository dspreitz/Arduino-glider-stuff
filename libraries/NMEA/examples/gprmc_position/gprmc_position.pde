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
NMEA gps(GPRMC);

void setup() {
  Serial1.begin(4800);
  pinMode(0, OUTPUT);
}

void loop() {
  if (Serial1.available() > 0 ) {
    // read incoming character from GPS
    char c = Serial1.read();

    // check if the character completes a valid GPS sentence
    if (gps.decode(c)) {
      // check if GPS positioning was active
      if (gps.gprmc_status() == 'A') {
        // check if you are in Colorado, USA
        boolean inColorado = (gps.gprmc_latitude() > 37.0)
                          && (gps.gprmc_latitude() < 41.0)
                          && (gps.gprmc_longitude() < -102.05)
                          && (gps.gprmc_longitude() > -109.05);

        // set led accordingly
        if (inColorado) {
          digitalWrite(0, HIGH);
        }
        else {
          digitalWrite(0, LOW);
        }
      }
    }
  }
}
