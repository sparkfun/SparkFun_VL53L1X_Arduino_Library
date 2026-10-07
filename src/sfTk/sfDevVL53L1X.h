/**
 * @file sfDevVL53L1X.h
 * @brief Header file for the SparkFun 4m Laser Distance Sensor - VL53L1X.
 *
 * This file contains the class definition, constants, and types for interacting with the VL53L1X sensor.
 *
 * @details
 * sfDevVL53L1X is a comms-agnostic driver for the VL53L1X Time of Flight distance sensor that uses the
 * SparkFun Toolkit. It wraps the STMicroelectronics ultra lite driver (see sfTk/st_src/), which in turn uses
 * the Toolkit for all platform specific needs (bus I/O, delays and timing).
 *
 * The SFEVL53L1X class (SparkFun_VL53L1X.h) defines the Arduino specific behavior for initializing and
 * interacting with devices.
 *
 * @section I2C_Addressing I2C Addressing
 * The Toolkit bus uses 7-bit addresses (default 0x29). For compatibility with prior versions of this
 * library, setI2CAddress() and getI2CAddress() use the 8-bit form (default 0x52).
 *
 * @author SparkFun Electronics
 * @date 2017-2026
 * @copyright Copyright (c) 2017-2026, SparkFun Electronics Inc. This project is released under the MIT License.
 *
 * SPDX-License-Identifier: MIT
 *
 * @section Repository Repository
 * https://github.com/sparkfun/SparkFun_VL53L1X_Arduino_Library
 *
 * @section Product_Links Product Links
 * - SEN-14722: https://www.sparkfun.com/products/14722
 * - SEN-18993: https://www.sparkfun.com/products/18993
 *
 */

#pragma once

#include <stdint.h>

// include the sparkfun toolkit headers
#include <sfTk/sfToolkit.h>

// Bus interfaces
#include <sfTk/sfTkII2C.h>

// The STMicroelectronics driver
#include "st_src/vl53l1_error_codes.h"
#include "st_src/vl53l1x_class.h"

///////////////////////////////////////////////////////////////////////////////
// I2C Addressing
///////////////////////////////////////////////////////////////////////////////
const uint8_t kDefaultVL53L1XAddr = 0x29; // 7-bit address. 0x52 in the 8-bit form used by ST.

// Values returned by the model ID register (0x010F) for supported devices
const uint16_t kVL53L1XSensorID = 0xEACC;
const uint16_t kVL53L4CDSensorID = 0xEBAA;

///////////////////////////////////////////////////////////////////////////////
// Settings
///////////////////////////////////////////////////////////////////////////////

#define DISTANCE_SHORT 1
#define DISTANCE_LONG 2
#define WINDOW_BELOW 0
#define WINDOW_ABOVE 1
#define WINDOW_OUT 2
#define WINDOW_IN 3

struct DetectionConfig
{
    uint16_t distanceMode = DISTANCE_SHORT; // distance mode : DISTANCE_SHORT (0) or DISTANCE_LONG (1)
    uint16_t windowMode = WINDOW_IN;        // window mode : WINDOW_BELOW (0), WINDOW_ABOVE (1), WINDOW_OUT (2), WINDOW_IN (3)
    uint8_t IntOnNoTarget = 1;              // = 1 (No longer used - just use 1)
    uint16_t thresholdHigh = 0; // (in mm) :  the threshold above which one the device raises an interrupt if Window = 1
    uint16_t thresholdLow = 0;  // (in mm) : the threshold under which one the device raises an interrupt if Window = 0
};

///////////////////////////////////////////////////////////////////////////////

class sfDevVL53L1X
{
  public:
    sfDevVL53L1X() : _theBus{nullptr}
    {
    }

    /// @brief This method is called to initialize the VL53L1X device through the specified bus. The sensor ID
    /// is verified, then the default configuration is loaded.
    /// @param theBus Pointer to the bus object. If nullptr, the bus set via setCommunicationBus() is used.
    /// @return ksfTkErrOk (0) if successful, an error code otherwise.
    sfTkError_t begin(sfTkII2C *theBus = nullptr);

    /// @brief Sets the communication bus to the specified bus.
    /// @param theBus Bus to set as the communication device.
    void setCommunicationBus(sfTkII2C *theBus);

    /// @brief Deprecated version of begin() - loads the default configuration without checking the sensor ID.
    /// @return ksfTkErrOk (0) if successful, an error code otherwise.
    sfTkError_t init(void);

    /// @brief Check the ID of the sensor
    /// @return True if the ID is a VL53L1X or VL53L4CD, false otherwise.
    bool checkID(void);

    /// @brief Get's the current ST software version
    VL53L1X_Version_t getSoftwareVersion(void);

    /// @brief Set the I2C address of the device. The bus is moved to the new address.
    /// @param addr The new address, in 8-bit form (e.g. 0x52 is the default)
    void setI2CAddress(uint8_t addr);

    /// @brief Get the I2C address
    /// @return The address, in 8-bit form (e.g. 0x52 is the default)
    int getI2CAddress(void);

    /// @brief Clear the interrupt flag
    void clearInterrupt(void);

    /// @brief Set the polarity of an active interrupt to High
    void setInterruptPolarityHigh(void);

    /// @brief Set the polarity of an active interrupt to Low
    void setInterruptPolarityLow(void);

    /// @brief Get the current interrupt polarity
    /// @return 1 = active high (default), 0 = active low
    uint8_t getInterruptPolarity(void);

    /// @brief Begins taking measurements
    void startRanging(void);

    /// @brief Start one-shot ranging
    void startOneshotRanging(void);

    /// @brief Stops taking measurements
    void stopRanging(void);

    /// @brief Checks the to see if data is ready
    bool checkForDataReady(void);

    /// @brief Set the timing budget for a measurement
    void setTimingBudgetInMs(uint16_t timingBudget);

    /// @brief Get the timing budget for a measurement
    uint16_t getTimingBudgetInMs(void);

    /// @brief Set to 4M range
    void setDistanceModeLong(void);

    /// @brief Set to 1.3M range
    void setDistanceModeShort(void);

    /// @brief Get the distance mode
    /// @return 1 for short and 2 for long
    uint8_t getDistanceMode(void);

    /// @brief Set time between measurements in ms
    void setIntermeasurementPeriod(uint16_t intermeasurement);

    /// @brief Get time between measurements in ms
    uint16_t getIntermeasurementPeriod(void);

    /// @brief Check if the VL53L1X has been initialized
    bool checkBootState(void);

    /// @brief Get the sensor ID
    uint16_t getSensorID(void);

    /// @brief Returns distance in mm
    uint16_t getDistance(void);

    /// @brief Returns the average signal rate per SPAD (The sensitive pads that detect light, the VL53L1X has a
    /// 16x16 array of these) in kcps/SPAD, or kilo counts per second per SPAD.
    uint16_t getSignalPerSpad(void);

    /// @brief Returns the ambient noise when not measuring a signal in kcps/SPAD.
    uint16_t getAmbientPerSpad(void);

    /// @brief Returns the signal rate in kcps. All SPADs combined.
    uint16_t getSignalRate(void);

    /// @brief Returns the current number of enabled SPADs
    uint16_t getSpadNb(void);

    /// @brief Returns the total ambient rate in kcps. All SPADs combined.
    uint16_t getAmbientRate(void);

    /// @brief Returns the range status, which can be any of the following:
    /// 0 = no error, 1 = signal fail, 2 = sigma fail, 7 = wrapped target fail
    uint8_t getRangeStatus(void);

    /// @brief Manually set an offset in mm
    void setOffset(int16_t offset);

    /// @brief Get the current offset in mm
    int16_t getOffset(void);

    /// @brief Manually set the value of crosstalk in counts per second (cps), which is interference from any sort
    /// of window in front of your sensor.
    void setXTalk(uint16_t xTalk);

    /// @brief Returns the current crosstalk value in cps.
    uint16_t getXTalk(void);

    /// @brief Set bounds for the interrupt. lowThresh and hiThresh are the bounds of your interrupt while window
    /// decides when the interrupt should fire. The options for window are:
    /// 0: Interrupt triggered on measured distance below lowThresh.
    /// 1: Interrupt triggered on measured distance above hiThresh.
    /// 2: Interrupt triggered on measured distance outside of bounds.
    /// 3: Interrupt triggered on measured distance inside of bounds.
    void setDistanceThreshold(uint16_t lowThresh, uint16_t hiThresh, uint8_t window);

    /// @brief Returns distance threshold window option
    uint16_t getDistanceThresholdWindow(void);

    /// @brief Returns lower bound in mm.
    uint16_t getDistanceThresholdLow(void);

    /// @brief Returns upper bound in mm
    uint16_t getDistanceThresholdHigh(void);

    /**Table of Optical Centers**
     *
     * 128,136,144,152,160,168,176,184,  192,200,208,216,224,232,240,248
     * 129,137,145,153,161,169,177,185,  193,201,209,217,225,233,241,249
     * 130,138,146,154,162,170,178,186,  194,202,210,218,226,234,242,250
     * 131,139,147,155,163,171,179,187,  195,203,211,219,227,235,243,251
     * 132,140,148,156,164,172,180,188,  196,204,212,220,228,236,244,252
     * 133,141,149,157,165,173,181,189,  197,205,213,221,229,237,245,253
     * 134,142,150,158,166,174,182,190,  198,206,214,222,230,238,246,254
     * 135,143,151,159,167,175,183,191,  199,207,215,223,231,239,247,255
     *
     * 127,119,111,103, 95, 87, 79, 71,  63, 55, 47, 39, 31, 23, 15, 7
     * 126,118,110,102, 94, 86, 78, 70,  62, 54, 46, 38, 30, 22, 14, 6
     * 125,117,109,101, 93, 85, 77, 69,  61, 53, 45, 37, 29, 21, 13, 5
     * 124,116,108,100, 92, 84, 76, 68,  60, 52, 44, 36, 28, 20, 12, 4
     * 123,115,107, 99, 91, 83, 75, 67,  59, 51, 43, 35, 27, 19, 11, 3
     * 122,114,106, 98, 90, 82, 74, 66,  58, 50, 42, 34, 26, 18, 10, 2
     * 121,113,105, 97, 89, 81, 73, 65,  57, 49, 41, 33, 25, 17, 9, 1
     * 120,112,104, 96, 88, 80, 72, 64,  56, 48, 40, 32, 24, 16, 8, 0 Pin 1
     *
     * To set the center, set the pad that is to the right and above the exact center of the region you'd like to
     * measure as your opticalCenter
     */

    /// @brief Set the height and width of the ROI(region of interest) in SPADs, lowest possible option is 4. Set
    /// optical center based on above table
    void setROI(uint8_t x, uint8_t y, uint8_t opticalCenter);

    /// @brief Returns the width of the ROI in SPADs
    uint16_t getROIX(void);

    /// @brief Returns the height of the ROI in SPADs
    uint16_t getROIY(void);

    /// @brief Programs the necessary threshold to trigger a measurement. Default is 1024 kcps.
    void setSignalThreshold(uint16_t signalThreshold);

    /// @brief Returns the signal threshold in kcps
    uint16_t getSignalThreshold(void);

    /// @brief Programs a new sigma threshold in mm. (default=15 mm)
    void setSigmaThreshold(uint16_t sigmaThreshold);

    /// @brief Returns the current sigma threshold.
    uint16_t getSigmaThreshold(void);

    /// @brief Recalibrates the sensor for temperature changes. Run this any time the temperature has changed by
    /// more than 8°C
    void startTemperatureUpdate(void);

    /// @brief Autocalibrate the offset by placing a target a known distance away from the sensor and passing this
    /// known distance into the function.
    void calibrateOffset(uint16_t targetDistanceInMm);

    /// @brief Autocalibrate the crosstalk by placing a target a known distance away from the sensor and passing
    /// this known distance into the function.
    void calibrateXTalk(uint16_t targetDistanceInMm);

    /// @brief Sets threshold configuration struct
    /// @return True if successful, false otherwise.
    bool setThresholdConfig(DetectionConfig *config);

    /// @brief Gets current threshold settings. Stores results in pointer to struct argument.
    /// @return False on error, true otherwise.
    bool getThresholdConfig(DetectionConfig *config);

  protected:
    sfTkII2C *_theBus; // Pointer to bus device.

  private:
    VL53L1X _device; // The STMicroelectronics driver
};
