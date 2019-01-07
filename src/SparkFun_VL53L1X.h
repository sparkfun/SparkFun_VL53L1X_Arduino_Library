#pragma once

#include "Arduino.h"
#include "Wire.h"
#include "RangeSensor.h"
#include "vl53l1_error_codes.h"
#include "vl53l1x_class.h"

class SFEVL53L1X
{
	public:
	SFEVL53L1X(TwoWire &i2cPort = Wire, int shutdownPin = -1, int interruptPin = -1); //Constructs our Distance sensor without an interrupt or shutdown pin
	bool init(); //Deprecated version of begin
	bool begin(); //Initialization of sensor
	bool checkID(); //Check the ID of the sensor, returns true if ID is correct
	void sensorOn(); //Toggles shutdown pin to turn sensor on and off
    void sensorOff(); //Toggles shutdown pin to turn sensor on and off
	VL53L1X_Version_t getSoftwareVersion(); //Get's the current ST software version
	void setI2CAddress(uint8_t addr); //Set the I2C address
	int getI2CAddress(); //Get the I2C address
	void clearInterrupt(); // Clear the interrupt flag
	void setInterruptPolarityHigh(); //Set the polarity of an active interrupt to High
	void setInterruptPolarityLow(); //Set the polarity of an active interrupt to Low
	uint8_t getInterruptPolarity(); //get the current interrupt polarity
	void startRanging(); //Begins taking measurements
	void stopRanging(); //Stops taking measurements
	bool checkForDataReady(); //Checks the to see if data is ready
	void setTimingBudgetInMs(uint16_t timingBudget); //Set the timing budget for a measurement
	uint16_t getTimingBudgetInMs(); //Get the timing budget for a measurement
	void setDistanceModeLong(); //Set to 4M range
	void setDistanceModeShort(); //Set to 1.3M range
	uint8_t getDistanceMode(); //Get the distance mode, returns 1 for short and 2 for long
	void setIntermeasurementPeriod(uint16_t intermeasurement); //Set time between measurements in ms
	uint16_t getIntermeasurementPeriod(); //Get time between measurements in ms
	bool checkBootState(); //Check if the VL53L1X has been initialized
	uint16_t getSensorID(); //Get the sensor ID
	uint16_t getDistance(); //Returns distance
	uint16_t getSignalPerSpad(); //Returns the average signal rate per SPAD (The sensitive pads that detect light, the VL53L1X has a 16x16 array of these) in kcps/SPAD, or kilo counts per second per SPAD.
	uint16_t getAmbientPerSpad(); //Returns the ambient noise when not measuring a signal in kcps/SPAD.
	uint16_t getSignalRate(); //Returns the signal rate in kcps. All SPADs combined.
	uint16_t getSpadNb(); //Returns the current number of enabled SPADs
	uint16_t getAmbientRate(); // Returns the total ambinet rate in kcps. All SPADs combined.
	uint8_t getRangeStatus(); //Returns the range status, which can be any of the following. 0 = no error, 1 = signal fail, 2 = sigma fail, 7 = wrapped target fail
	void setOffset(int16_t offset); //Manually set an offset in mm
	int16_t getOffset(); //Get the current offset in mm
	void setXTalk(uint16_t xTalk); //Manually set the value of crosstalk in counts per second (cps), which is interference from any sort of window in front of your sensor.
	uint16_t getXTalk(); //Returns the current crosstalk value in cps.
	void setDistanceThreshold(uint16_t lowThresh, uint16_t hiThresh, uint8_t window);//Set bounds for the interrupt. lowThresh and hiThresh are the bounds of your interrupt while window decides when the interrupt should fire. The options for window are shown below.
	//0: Interrupt triggered on measured distance below lowThresh.
	//1: Interrupt triggered on measured distance above hiThresh.
	//2: Interrupt triggered on measured distance outside of bounds.
	//3: Interrupt triggered on measured distance inside of bounds.
	uint16_t getDistanceThresholdWindow(); //Returns distance threshold window option
	uint16_t getDistanceThresholdLow(); //Returns lower bound in mm.
	uint16_t getDistanceThresholdHigh(); //Returns upper bound in mm
	void setROI(uint16_t x, uint16_t y); //Set the height and width of the ROI in SPADs, lowest possible option is 4. ROI is always centered.
	uint16_t getROIX(); //Returns the width of the ROI in SPADs
	uint16_t getROIY(); //Returns the height of the ROI in SPADs
	void setSignalThreshold(uint16_t signalThreshold); //Programs the necessary threshold to trigger a measurement. Default is 1024 kcps.
	uint16_t getSignalThreshold(); //Returns the signal threshold in kcps
	void setSigmaThreshold(uint16_t sigmaThreshold); //Programs a new sigma threshold in mm. (default=15 mm)
	uint16_t getSigmaThreshold(); //Returns the current sigma threshold.
	void startTemperatureUpdate(); //Recalibrates the sensor for temperature changes. Run this any time the temperature has changed by more than 8°C
	void calibrateOffset(uint16_t targetDistanceInMm); //Autocalibrate the offset by placing a target a known distance away from the sensor and passing this known distance into the function.
	void calibrateXTalk(uint16_t targetDistanceInMm); //Autocalibrate the crosstalk by placing a target a known distance away from the sensor and passing this known distance into the function.
	private:
	TwoWire *_i2cPort;
	int _shutdownPin;
	int _interruptPin;
	int _i2cAddress = 0x52;
	VL53L1X* _device;
};