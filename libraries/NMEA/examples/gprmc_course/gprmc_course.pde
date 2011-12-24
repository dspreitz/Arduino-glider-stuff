// This example illustates basic use of the NMEA library.
// It assumes that a GPS receiver is connected to serial
// port 'Serial1' at 4800 bps, and that 2 LEDs are connected
// to digital i/o pins 8 and 9.
//
// A GPS data connection of type GPRMC is created, and
// used to show direction to a destination, relative to
// your own direction of movement. The LEDs light up as
// follows. If the destination is
// - more than 5 degrees to your left, pin 8 lights up
// - more than 5 degrees to your right, pin 9 lights up
// - otherwise, both LEDs light up.

#include <nmea.h>

NMEA gps(GPRMC);  // GPS data connection to GPRMC sentence type
float d;          // relative direction to destination

// destination coordinates in degrees-decimal
float dest_latitude = 48.858342;
float dest_longitude = 2.294522;

void setup() {
  Serial1.begin(4800);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
}

void loop() {
  if (Serial1.available() > 0 ) {
    // read incoming character from GPS
    char c = Serial1.read();

    // check if the character completes a valid GPS sentence
    if (gps.decode(c)) {
      // check if GPS positioning was active
      if (gps.gprmc_status() == 'A') {
        // calculate relative direction to destination
        d = gps.gprmc_course_to(dest_latitude, dest_longitude) - gps.gprmc_course();
        if (d < 0) { d += 360; }
        if (d > 180) { d -= 360; }
        // set LEDs accordingly
        if (d < 5) {
          digitalWrite(8, HIGH);
        } else {
          digitalWrite(8, LOW);
        }
        if (d > -5) {
          digitalWrite(9, HIGH);
        } else {
          digitalWrite(9, LOW);
        }
      }
    }
  }
}
