/*
  Reading distance from the laser based VL53L1X
  By: Nathan Seidle
  SparkFun Electronics
  Date: April 4th, 2018
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  SparkFun labored with love to create this code. Feel like supporting open source hardware?
  Buy a board from SparkFun! https://www.sparkfun.com/products/14667

  This example prints the distance to an object.

  Are you getting weird readings? Be sure the vacuum tape has been removed from the sensor.
*/

#include <Wire.h>
//#include "vl53l1x_class.h"
#include "SparkFun_VL53L1X.h"

SFEVL53L1X distanceSensor(Wire, 2, 3);
int distance;

void setup(void)
{
  Wire.begin();

  Serial.begin(9600);
  Serial.println("VL53L1X Qwiic Test");

  if (distanceSensor.init() == 0)
  {
    Serial.println("Sensor online!");
  }
}

void loop(void)
{
  distanceSensor.startRanging(); //Write configuration bytes to initiate measurement
  distance = distanceSensor.getDistance(); //Get the result of the measurement from the sensor
  distanceSensor.stopRanging(); //Write configuration bytes to initiate measurement

  float distanceInches = distance * 0.0393701; //calculate Feet
  float distanceFeet = distanceInches / 12.0;

  Serial.print("Distance(mm): ");
  Serial.print(distance);
  Serial.print("\tDistance(ft): ");
  Serial.print(distanceFeet, 2);

  Serial.println();

}

