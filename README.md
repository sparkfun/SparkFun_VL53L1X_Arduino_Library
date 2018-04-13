SparkFun VL53L1X Distance Sensor Library
===========================================================

![SparkFun VL53L1X Distance Sensor](https://cdn.sparkfun.com//assets/parts/1/2/8/6/3/14667-ToF_Laser_Distance_Sensor__Qwiic__-_VL53L1X-01.jpg)

[*SparkX Qwiic Distance Sensor - VL532L1X (SPX-14667)*](https://www.sparkfun.com/products/14667)

The VL53L1X is the latest Time Of Flight (ToF) sensor to be released. It uses a VCSEL (vertical cavity surface emitting laser) to emit a class 1 IR laser and time the reflection to the target. What does all this mean? You can measure the distance to an object up to 4 meters away with millimeter resolution! That’s pretty incredible.

We’re far from done: The VL53L1X is a highly complex sensor with a multitude of options and configurations. We’ve written example sketches that allow you to read the distance, signal rate, and range status. Because ST has chosen not to release a complete datasheet we are forced to reverse engineer the interface from their example code and I2C data stream captures. If you’re into puzzles we could use your help to make the library better!

We’ve found the precision of the sensor to be 1mm but the accuracy is around +/-5mm.

SparkFun labored with love to create this code. Feel like supporting open source hardware? 
Buy a [breakout board](https://www.sparkfun.com/products/14667) from SparkFun!

Library written by Nathan Seidle ([SparkFun](http://www.sparkfun.com)).

Repository Contents
-------------------

* **/examples** - Example sketches for the library (.ino). Run these from the Arduino IDE. 
* **/src** - Source files for the library (.cpp, .h).
* **keywords.txt** - Keywords from this library that will be highlighted in the Arduino IDE. 
* **library.properties** - General library properties for the Arduino package manager. 

Documentation
--------------

* **[Installing an Arduino Library Guide](https://learn.sparkfun.com/tutorials/installing-an-arduino-library)** - Basic information on how to install an Arduino library.
* **[Product Repository](https://github.com/sparkfunX/Qwiic_Distance_VL53L1X)** - Main repository (including hardware files)

License Information
-------------------

This product is _**open source**_! 

Various bits of the code have different licenses applied. Anything SparkFun wrote is beerware; if you see me (or any other SparkFun employee) at the local, and you've found our code helpful, please buy us a round!

Please use, reuse, and modify these files as you see fit. Please maintain attribution to SparkFun Electronics and release anything derivative under the same license.

Distributed as-is; no warranty is given.

- Your friends at SparkFun.
