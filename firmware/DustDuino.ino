// AirData v0.1
//
// This software is released as-is, without warranty,
// under the GNU General Public License Version 3.
//
// Based on DustDuino by Matthew Schroyer.
// Modified by Chris Franson (@chrisfranson) for the Particle Photon.
//
// For more information on building a DustDuino, visit:
// http://www.mentalmunition.com/2013/10/measure-air-pollution-with-dustduino-of.html

/*888888888888888888888888888888888888888888888888888888888888888888888888888888888888*/

#include "math.h"

unsigned long starttime;

unsigned long triggerOnP1;
unsigned long triggerOffP1;
unsigned long pulseLengthP1;
unsigned long durationP1;
boolean valP1 = HIGH;
boolean triggerP1 = false;

unsigned long triggerOnP2;
unsigned long triggerOffP2;
unsigned long pulseLengthP2;
unsigned long durationP2;
boolean valP2 = HIGH;
boolean triggerP2 = false;

float ratioP1 = 0;
float ratioP2 = 0;
unsigned long sampletime_ms = 30000;
float countP1;
float countP2;

void setup(){
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  starttime = millis();
  Serial.println("setup complete");
}

void loop(){

  valP1 = digitalRead(5);
  valP2 = digitalRead(6);

  if (valP1 == LOW && triggerP1 == false){
    triggerP1 = true;
    triggerOnP1 = micros();
  }

  if (valP1 == HIGH && triggerP1 == true) {
      triggerOffP1 = micros();
      pulseLengthP1 = triggerOffP1 - triggerOnP1;
      durationP1 = durationP1 + pulseLengthP1;
      triggerP1 = false;
  }

  if (valP2 == LOW && triggerP2 == false) {
    triggerP2 = true;
    triggerOnP2 = micros();
  }

  if (valP2 == HIGH && triggerP2 == true) {
    triggerOffP2 = micros();
    pulseLengthP2 = triggerOffP2 - triggerOnP2;
    durationP2 = durationP2 + pulseLengthP2;
    triggerP2 = false;
  }

  /*Serial.println("RESET?");*/
  /*wdt_reset();*/

    // Function creates particle count and mass concentration
    // from PPD-42 low pulse occupancy (LPO).
    if ((millis() - starttime) > sampletime_ms) {

      // Generates PM10 and PM2.5 count from LPO.
      // Derived from code created by Chris Nafis
      // http://www.howmuchsnow.com/arduino/airquality/grovedust/

      ratioP1 = durationP1/(sampletime_ms*10.0);
      ratioP2 = durationP2/(sampletime_ms*10.0);
      countP1 = 1.1*pow(ratioP1,3)-3.8*pow(ratioP1,2)+520*ratioP1+0.62;
      countP2 = 1.1*pow(ratioP2,3)-3.8*pow(ratioP2,2)+520*ratioP2+0.62;
      float PM10count = countP2;
      float PM25count = countP1 - countP2;

      // Assues density, shape, and size of dust
      // to estimate mass concentration from particle
      // count. This method was described in a 2009
      // paper by Uva, M., Falcone, R., McClellan, A.,
      // and Ostapowicz, E.
      // http://wireless.ece.drexel.edu/research/sd_air_quality.pdf

      // begins PM10 mass concentration algorithm
      double r10 = 2.6*pow(10,-6);
      double pi = 3.14159;
      double vol10 = (4/3)*pi*pow(r10,3);
      double density = 1.65*pow(10,12);
      double mass10 = density*vol10;
      double K = 3531.5;
      float concLarge = (PM10count)*K*mass10;

      // next, PM2.5 mass concentration algorithm
      double r25 = 0.44*pow(10,-6);
      double vol25 = (4/3)*pi*pow(r25,3);
      double mass25 = density*vol25;
      float concSmall = (PM25count)*K*mass25;

      sendData(concLarge, concSmall, PM10count, PM25count);

      durationP1 = 0;
      durationP2 = 0;
      starttime = millis();
      /*Serial.println("RESET?");*/
      /*wdt_reset();*/
    }
  }


void sendData(int PM10Conc, int PM25Conc, int PM10count, int PM25count) {

    String dataString = "{";

    dataString += "\"PM10Conc\":\"" + String(PM10Conc) + "\", ";
    dataString += "\"PM25Conc\":\"" + String(PM25Conc) + "\", ";
    dataString += "\"PM10count\":\"" + String(PM10count) + "\", ";
    dataString += "\"PM25count\":\"" + String(PM25count) + "\"";
    dataString += "}";

    Particle.publish("Particles", dataString);

    Serial.println("PM10Conc: " + String(PM10Conc));
    Serial.println("PM25Conc: " + String(PM25Conc));
    Serial.println("PM10count: " + String(PM10count));
    Serial.println("PM25count: " + String(PM25count));

}
