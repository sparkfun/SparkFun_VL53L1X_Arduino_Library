/**
 * @file sfDevVL53L1X.cpp
 * @brief Implementation file for the SparkFun VL53L1X Distance Sensor device driver.
 *
 * @details
 * This file implements the sfDevVL53L1X class methods for configuring and reading data from
 * the VL53L1X Time of Flight sensor. The driver provides a comms-agnostic interface using the SparkFun Toolkit.
 *
 * @author SparkFun Electronics
 * @date 2017-2026
 * @copyright Copyright (c) 2017-2026, SparkFun Electronics Inc. All rights reserved.
 *
 * @section License License
 * SPDX-License-Identifier: MIT
 *
 * @see https://github.com/sparkfun/SparkFun_VL53L1X_Arduino_Library
 */
#include "sfDevVL53L1X.h"

sfTkError_t sfDevVL53L1X::begin(sfTkII2C *theBus)
{
    // Set the internal bus pointer, overriding current bus if it exists.
    if (theBus != nullptr)
        setCommunicationBus(theBus);

    // Nullptr check.
    if (!_theBus)
        return ksfTkErrBusNotInit;

    if (!checkID())
        return ksfTkErrFail;

    return init();
}

void sfDevVL53L1X::setCommunicationBus(sfTkII2C *theBus)
{
    _theBus = theBus;
    _device.dev_i2c = theBus;
}

sfTkError_t sfDevVL53L1X::init(void)
{
    if (!_theBus)
        return ksfTkErrBusNotInit;

    return _device.VL53L1X_SensorInit() == VL53L1_ERROR_NONE ? ksfTkErrOk : ksfTkErrFail;
}

/*Checks the ID of the device, returns true if ID is correct*/

bool sfDevVL53L1X::checkID(void)
{
    uint16_t sensorId = getSensorID();

    return (sensorId == kVL53L1XSensorID) || (sensorId == kVL53L4CDSensorID);
}

/*Gets the software version number of the current library installed.*/

VL53L1X_Version_t sfDevVL53L1X::getSoftwareVersion(void)
{
    VL53L1X_Version_t tempVersion;
    _device.VL53L1X_GetSWVersion(&tempVersion);
    return tempVersion;
}

void sfDevVL53L1X::setI2CAddress(uint8_t addr)
{
    // Note: on success, the ST driver moves the bus to the new address
    _device.VL53L1X_SetI2CAddress(addr);
}

int sfDevVL53L1X::getI2CAddress(void)
{
    // The bus holds the 7-bit address - return the 8-bit form
    return (_theBus ? _theBus->address() : kDefaultVL53L1XAddr) << 1;
}

void sfDevVL53L1X::clearInterrupt(void)
{
    _device.VL53L1X_ClearInterrupt();
}

void sfDevVL53L1X::setInterruptPolarityHigh(void)
{
    _device.VL53L1X_SetInterruptPolarity(1);
}

void sfDevVL53L1X::setInterruptPolarityLow(void)
{
    _device.VL53L1X_SetInterruptPolarity(0);
}

/**
 * This function gets the interrupt polarity\n
 * 1=active high (default), 0=active low
 */

uint8_t sfDevVL53L1X::getInterruptPolarity(void)
{
    uint8_t tmp = 0;
    _device.VL53L1X_GetInterruptPolarity(&tmp);
    return tmp;
}

void sfDevVL53L1X::startRanging(void)
{
    _device.VL53L1X_StartRanging();
}

void sfDevVL53L1X::startOneshotRanging(void)
{
    _device.VL53L1X_StartOneshotRanging();
}

void sfDevVL53L1X::stopRanging(void)
{
    _device.VL53L1X_StopRanging();
}

bool sfDevVL53L1X::checkForDataReady(void)
{
    uint8_t dataReady = 0;
    _device.VL53L1X_CheckForDataReady(&dataReady);
    return (bool)dataReady;
}

void sfDevVL53L1X::setTimingBudgetInMs(uint16_t timingBudget)
{
    _device.VL53L1X_SetTimingBudgetInMs(timingBudget);
}

uint16_t sfDevVL53L1X::getTimingBudgetInMs(void)
{
    uint16_t timingBudget = 0;
    _device.VL53L1X_GetTimingBudgetInMs(&timingBudget);
    return timingBudget;
}

void sfDevVL53L1X::setDistanceModeLong(void)
{
    _device.VL53L1X_SetDistanceMode(DISTANCE_LONG);
}

void sfDevVL53L1X::setDistanceModeShort(void)
{
    _device.VL53L1X_SetDistanceMode(DISTANCE_SHORT);
}

uint8_t sfDevVL53L1X::getDistanceMode(void)
{
    uint16_t distanceMode = 0;
    _device.VL53L1X_GetDistanceMode(&distanceMode);
    return distanceMode;
}

void sfDevVL53L1X::setIntermeasurementPeriod(uint16_t intermeasurement)
{
    _device.VL53L1X_SetInterMeasurementInMs(intermeasurement);
}

uint16_t sfDevVL53L1X::getIntermeasurementPeriod(void)
{
    uint16_t intermeasurement = 0;
    _device.VL53L1X_GetInterMeasurementInMs(&intermeasurement);
    return intermeasurement;
}

bool sfDevVL53L1X::checkBootState(void)
{
    uint8_t bootState = 0;
    _device.VL53L1X_BootState(&bootState);
    return (bool)bootState;
}

uint16_t sfDevVL53L1X::getSensorID(void)
{
    uint16_t id = 0;
    _device.VL53L1X_GetSensorId(&id);
    return id;
}

uint16_t sfDevVL53L1X::getDistance(void)
{
    uint16_t distance = 0;
    _device.VL53L1X_GetDistance(&distance);
    return distance;
}

uint16_t sfDevVL53L1X::getSignalPerSpad(void)
{
    uint16_t temp = 0;
    _device.VL53L1X_GetSignalPerSpad(&temp);
    return temp;
}

uint16_t sfDevVL53L1X::getAmbientPerSpad(void)
{
    uint16_t temp = 0;
    _device.VL53L1X_GetAmbientPerSpad(&temp);
    return temp;
}

uint16_t sfDevVL53L1X::getSignalRate(void)
{
    uint16_t temp = 0;
    _device.VL53L1X_GetSignalRate(&temp);
    return temp;
}

uint16_t sfDevVL53L1X::getSpadNb(void)
{
    uint16_t temp = 0;
    _device.VL53L1X_GetSpadNb(&temp);
    return temp;
}

uint16_t sfDevVL53L1X::getAmbientRate(void)
{
    uint16_t temp = 0;
    _device.VL53L1X_GetAmbientRate(&temp);
    return temp;
}

uint8_t sfDevVL53L1X::getRangeStatus(void)
{
    uint8_t temp = 0;
    _device.VL53L1X_GetRangeStatus(&temp);
    return temp;
}

void sfDevVL53L1X::setOffset(int16_t offset)
{
    _device.VL53L1X_SetOffset(offset);
}

int16_t sfDevVL53L1X::getOffset(void)
{
    int16_t temp = 0;
    _device.VL53L1X_GetOffset(&temp);
    return temp;
}

void sfDevVL53L1X::setXTalk(uint16_t xTalk)
{
    _device.VL53L1X_SetXtalk(xTalk);
}

uint16_t sfDevVL53L1X::getXTalk(void)
{
    uint16_t temp = 0;
    _device.VL53L1X_GetXtalk(&temp);
    return temp;
}

void sfDevVL53L1X::setDistanceThreshold(uint16_t lowThresh, uint16_t hiThresh, uint8_t window)
{
    _device.VL53L1X_SetDistanceThreshold(lowThresh, hiThresh, window, 1);
}

uint16_t sfDevVL53L1X::getDistanceThresholdWindow(void)
{
    uint16_t temp = 0;
    _device.VL53L1X_GetDistanceThresholdWindow(&temp);
    return temp;
}

uint16_t sfDevVL53L1X::getDistanceThresholdLow(void)
{
    uint16_t temp = 0;
    _device.VL53L1X_GetDistanceThresholdLow(&temp);
    return temp;
}

uint16_t sfDevVL53L1X::getDistanceThresholdHigh(void)
{
    uint16_t temp = 0;
    _device.VL53L1X_GetDistanceThresholdHigh(&temp);
    return temp;
}

void sfDevVL53L1X::setROI(uint8_t x, uint8_t y, uint8_t opticalCenter)
{
    _device.VL53L1X_SetROI(x, y, opticalCenter);
}

uint16_t sfDevVL53L1X::getROIX(void)
{
    uint16_t tempX = 0;
    uint16_t tempY = 0;
    _device.VL53L1X_GetROI_XY(&tempX, &tempY);
    return tempX;
}

uint16_t sfDevVL53L1X::getROIY(void)
{
    uint16_t tempX = 0;
    uint16_t tempY = 0;
    _device.VL53L1X_GetROI_XY(&tempX, &tempY);
    return tempY;
}

void sfDevVL53L1X::setSignalThreshold(uint16_t signalThreshold)
{
    _device.VL53L1X_SetSignalThreshold(signalThreshold);
}

uint16_t sfDevVL53L1X::getSignalThreshold(void)
{
    uint16_t temp = 0;
    _device.VL53L1X_GetSignalThreshold(&temp);
    return temp;
}

void sfDevVL53L1X::setSigmaThreshold(uint16_t sigmaThreshold)
{
    _device.VL53L1X_SetSigmaThreshold(sigmaThreshold);
}

uint16_t sfDevVL53L1X::getSigmaThreshold(void)
{
    uint16_t temp = 0;
    _device.VL53L1X_GetSigmaThreshold(&temp);
    return temp;
}

void sfDevVL53L1X::startTemperatureUpdate(void)
{
    _device.VL53L1X_StartTemperatureUpdate();
}

void sfDevVL53L1X::calibrateOffset(uint16_t targetDistanceInMm)
{
    int16_t offset = getOffset();
    _device.VL53L1X_CalibrateOffset(targetDistanceInMm, &offset);
}

void sfDevVL53L1X::calibrateXTalk(uint16_t targetDistanceInMm)
{
    uint16_t xTalk = getXTalk();
    _device.VL53L1X_CalibrateXtalk(targetDistanceInMm, &xTalk);
}

bool sfDevVL53L1X::setThresholdConfig(DetectionConfig *config)
{
    return _device.VL53L1X_SetDistanceMode(config->distanceMode) == VL53L1_ERROR_NONE &&
           _device.VL53L1X_SetDistanceThreshold(config->thresholdLow, config->thresholdHigh,
                                                (uint8_t)config->windowMode,
                                                (uint8_t)config->IntOnNoTarget) == VL53L1_ERROR_NONE;
}

bool sfDevVL53L1X::getThresholdConfig(DetectionConfig *config)
{
    uint16_t temp16 = 0;

    VL53L1X_ERROR error = _device.VL53L1X_GetDistanceMode(&temp16);
    if (error != 0)
        return false;
    else
        config->distanceMode = temp16;

    error = _device.VL53L1X_GetDistanceThresholdWindow(&temp16);
    if (error != 0)
        return false;
    else
        config->windowMode = temp16;

    config->IntOnNoTarget = 1;

    error = _device.VL53L1X_GetDistanceThresholdLow(&temp16);
    if (error != 0)
        return false;
    else
        config->thresholdLow = temp16;

    error = _device.VL53L1X_GetDistanceThresholdHigh(&temp16);
    if (error != 0)
        return false;
    else
        config->thresholdHigh = temp16;

    return true;
}
