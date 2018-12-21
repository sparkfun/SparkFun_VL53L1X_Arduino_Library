#pragma once

#include "Arduino.h"
#include "Wire.h"
#include "RangeSensor.h"
#include "vl53l1_error_codes.h"
#include "vl53l1x_class.h"

class SFEVL53L1X
{
	public:
	SFEVL53L1X(TwoWire &i2cPort = Wire, int shutdownPin = -1, int interruptPin = -1);
	bool init();
	bool checkID();
	void sensorOn();
    void sensorOff(); //virtual void VL53L1_Off(void)
	VL53L1X_Version_t getSoftwareVersion();//VL53L1X_ERROR VL53L1X_GetSWVersion(VL53L1X_Version_t *pVersion);
	void setI2CAddress(uint8_t addr);//VL53L1X_ERROR VL53L1X_SetI2CAddress(uint8_t new_address);
	int getI2CAddress();
	void clearInterrupt();//VL53L1X_ERROR VL53L1X_ClearInterrupt();
	void setInterruptPolarityHigh();//VL53L1X_ERROR VL53L1X_SetInterruptPolarity(uint8_t IntPol);
	void setInterruptPolarityLow();
	uint8_t getInterruptPolarity();//VL53L1X_ERROR VL53L1X_GetInterruptPolarity(uint8_t *pIntPol);
	void startRanging();//Begins taking measurements
	void stopRanging();//Stops taking measurements
	bool checkForDataReady();//VL53L1X_ERROR VL53L1X_CheckForDataReady(uint8_t *isDataReady);
	void setTimingBudgetInMs(uint16_t timingBudget);//VL53L1X_ERROR VL53L1X_SetTimingBudgetInMs(uint16_t TimingBudgetInMs);
	uint16_t getTimingBudgetInMs();//VL53L1X_ERROR VL53L1X_GetTimingBudgetInMs(uint16_t *pTimingBudgetInMs);
	void setDistanceModeLong();//VL53L1X_ERROR VL53L1X_SetDistanceMode(uint16_t DistanceMode);
	void setDistanceModeShort();//VL53L1X_ERROR VL53L1X_GetDistanceMode(uint16_t *pDistanceMode);
	uint8_t getDistanceMode();
	void setIntermeasurementPeriod(uint16_t intermeasurement);//VL53L1X_ERROR VL53L1X_SetInterMeasurementInMs(uint16_t InterMeasurementInMs);
	uint16_t getIntermeasurementPeriod();//VL53L1X_ERROR VL53L1X_GetInterMeasurementInMs(uint16_t * pIM);
	bool checkBootState();//VL53L1X_ERROR VL53L1X_BootState(uint8_t *state);
	uint16_t getSensorID();//VL53L1X_ERROR VL53L1X_GetSensorId(uint16_t *id);
	uint16_t getDistance();//Returns distance
	uint16_t getSignalPerSpad();//VL53L1X_ERROR VL53L1X_GetSignalPerSpad(uint16_t *signalPerSp);
	uint16_t getAmbientPerSpad();//VL53L1X_ERROR VL53L1X_GetAmbientPerSpad(uint16_t *amb);
	uint16_t getSignalRate();//VL53L1X_ERROR VL53L1X_GetSignalRate(uint16_t *signalRate);
	uint16_t getSpadNb();//VL53L1X_ERROR VL53L1X_GetSpadNb(uint16_t *spNb);
	uint16_t getAmbientRate();//VL53L1X_ERROR VL53L1X_GetAmbientRate(uint16_t *ambRate);
	uint8_t getRangeStatus();//VL53L1X_ERROR VL53L1X_GetRangeStatus(uint8_t *rangeStatus);
	void setOffset(int16_t offset);//VL53L1X_ERROR VL53L1X_SetOffset(int16_t OffsetValue);
	int16_t getOffset();//VL53L1X_ERROR VL53L1X_GetOffset(int16_t *Offset);
	void setXTalk(uint16_t xTalk);//VL53L1X_ERROR VL53L1X_SetXtalk(uint16_t XtalkValue);
	uint16_t getXTalk();//VL53L1X_ERROR VL53L1X_GetXtalk(uint16_t *Xtalk);
	void setDistanceThreshold(uint16_t lowThresh, uint16_t hiThresh, uint8_t window);//VL53L1X_ERROR VL53L1X_SetDistanceThreshold(uint16_t ThreshLow,
	//				  uint16_t ThreshHigh, uint8_t Window,
	//				  uint8_t IntOnNoTarget);
	uint16_t getDistanceThresholdWindow();//VL53L1X_ERROR VL53L1X_GetDistanceThresholdWindow(uint16_t *window);
	uint16_t getDistanceThresholdLow();//VL53L1X_ERROR VL53L1X_GetDistanceThresholdLow(uint16_t *low);
	uint16_t getDistanceThresholdHigh();//VL53L1X_ERROR VL53L1X_GetDistanceThresholdHigh(uint16_t *high);
	void setROI(uint16_t x, uint16_t y);//VL53L1X_ERROR VL53L1X_SetROI(uint16_t X, uint16_t Y);
	uint16_t getROIX();//VL53L1X_ERROR VL53L1X_GetROI_XY(uint16_t *ROI_X, uint16_t *ROI_Y);
	uint16_t getROIY();
	void setSignalThreshold(uint16_t signalThreshold);//VL53L1X_ERROR VL53L1X_SetSignalThreshold(uint16_t signal);
	uint16_t getSignalThreshold();//VL53L1X_ERROR VL53L1X_GetSignalThreshold(uint16_t *signal);
	void setSigmaThreshold(uint16_t sigmaThreshold);//VL53L1X_ERROR VL53L1X_SetSigmaThreshold(uint16_t sigma);
	uint16_t getSigmaThreshold();//VL53L1X_ERROR VL53L1X_GetSigmaThreshold(uint16_t *signal);
	void startTemperatureUpdate();//VL53L1X_ERROR VL53L1X_StartTemperatureUpdate();
	void calibrateOffset(uint16_t targetDistanceInMm);//int8_t VL53L1X_CalibrateOffset(uint16_t TargetDistInMm, int16_t *offset);
	void calibrateXTalk(uint16_t targetDistanceInMm);//int8_t VL53L1X_CalibrateXtalk(uint16_t TargetDistInMm, uint16_t *xtalk);
	private:
	TwoWire *_i2cPort;
	int _shutdownPin;
	int _interruptPin;
	int _i2cAddress = 0x52;
	VL53L1X* _device;
};