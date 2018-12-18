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

#include "SparkFun_VL53L1X_Arduino_Library.h"
VL53L1X distanceSensor;

uint8_t incoming;
void setup(void)
{
  Wire.begin();

  Serial.begin(9600);
  Serial.println("VL53L1X Qwiic Test");

  if (distanceSensor.begin() == false)
    Serial.println("Sensor offline!");

}

uint8_t OFFSET_MM = 140;

void loop(void)
{
  distanceSensor.startMeasurement(); //Write configuration bytes to initiate measurement

  //Poll for completion of measurement. Takes 40-50ms.
  while (distanceSensor.newDataReady() == false)
  delay(5);
    
  while (incoming != 'c')
  {
    Serial.print("Calibrating, current offset is ");
    Serial.print(distanceSensor.calibrateOffset(OFFSET_MM));
    Serial.println(" mm");
    if(Serial.available())
    {
      incoming = Serial.read();
    }
  };
  Serial.print("Calibrated");
 
  int distance = distanceSensor.getCalibratedDistance(); //Get the result of the measurement from the sensor

  Serial.print("DistanceCal(mm): ");
  Serial.print(distance);

  float distanceInches = distance * 0.0393701;
  float distanceFeet = distanceInches / 12.0;

  Serial.print("\tDistance(ft): ");
  Serial.print(distanceFeet, 2);

  Serial.println();
}

