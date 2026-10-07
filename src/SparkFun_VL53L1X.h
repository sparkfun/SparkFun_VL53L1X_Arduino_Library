/**
 * @file SparkFun_VL53L1X.h
 * @brief Arduino-specific implementation for the SparkFun VL53L1X 4m Laser Distance Sensor.
 *
 * @details
 * This file provides the Arduino-specific implementation of the VL53L1X driver class.
 * The SFEVL53L1X class inherits from sfDevVL53L1X and implements the I2C communication
 * interface using Arduino's Wire library via the SparkFun Toolkit, as well as the optional
 * shutdown pin.
 *
 * Originally written by Andy England @ SparkFun Electronics, October 17th, 2017
 *
 * @section Class SFEVL53L1X Class
 * - begin(): Initializes I2C communication and the sensor
 * - isConnected(): Verifies sensor connection
 * - sensorOn()/sensorOff(): Drives the (optional) shutdown pin
 *
 * @section Dependencies Dependencies
 * - Arduino.h
 * - SparkFun_Toolkit.h
 * - sfDevVL53L1X.h
 *
 * @author SparkFun Electronics
 * @date 2017-2026
 * @copyright Copyright (c) 2017-2026, SparkFun Electronics Inc. All rights reserved.
 *
 * @section License License
 * SPDX-License-Identifier: MIT
 *
 * @section Product_Links Product Links
 * - SEN-14722: https://www.sparkfun.com/products/14722
 * - SEN-18993: https://www.sparkfun.com/products/18993
 *
 * @see https://github.com/sparkfun/SparkFun_VL53L1X_Arduino_Library
 */

#pragma once

// clang-format off
#include <SparkFun_Toolkit.h>
#include "sfTk/sfDevVL53L1X.h"
#include <Arduino.h>
// clang-format on

/**
 * @class SFEVL53L1X
 * @brief Arduino I2C implementation for the VL53L1X distance sensor.
 *
 * Example usage:
 * @code
 * SFEVL53L1X distanceSensor;
 * if (distanceSensor.begin() != 0) {
 *     // Sensor failed to initialize
 * }
 * @endcode
 *
 * @note begin() returns 0 (ksfTkErrOk) on success, matching prior versions of this library.
 *
 * @see sfDevVL53L1X
 * @see TwoWire
 */
class SFEVL53L1X : public sfDevVL53L1X
{
  public:
    /**
     * @brief Constructs our Distance sensor
     *
     * The I2C bus is not set up - begin() sets it up using the default port (Wire).
     */
    SFEVL53L1X() : _busIsSetup{false}, _shutdownPin{-1}, _interruptPin{-1}
    {
    }

    /**
     * @brief Constructs our Distance sensor on the given I2C port
     *
     * @param i2cPort TwoWire instance to use for I2C communication
     * @param shutdownPin Pin connected to the sensor's shutdown (XSHUT) pin, -1 if not connected
     * @param interruptPin Pin connected to the sensor's interrupt (GPIO1) pin, -1 if not connected
     */
    SFEVL53L1X(TwoWire &i2cPort, int shutdownPin = -1, int interruptPin = -1)
        : _busIsSetup{false}, _shutdownPin{shutdownPin}, _interruptPin{interruptPin}
    {
        // Setting up the bus only records the port and address - no I/O takes place here.
        // It is done now so the sensor methods are usable before begin(), as in prior versions.
        setupBus(i2cPort, kDefaultVL53L1XAddr);

        if (_shutdownPin >= 0)
            pinMode(_shutdownPin, OUTPUT);
    }

    /**
     * @brief Initializes the VL53L1X sensor.
     *
     * Sets up the I2C bus on the default port (Wire) if it is not already set up, verifies the
     * sensor is connected, then loads the default configuration.
     *
     * @return 0 (ksfTkErrOk) if successful, an error code otherwise.
     */
    sfTkError_t begin(void)
    {
        if (!_busIsSetup && !setupBus(Wire, kDefaultVL53L1XAddr))
            return ksfTkErrBusNotInit;

        if (!isConnected())
            return ksfTkErrFail;

        return sfDevVL53L1X::init();
    }

    /**
     * @brief Initializes the VL53L1X sensor on the given I2C port.
     *
     * @param i2cPort TwoWire instance to use for I2C communication
     *
     * @return 0 (ksfTkErrOk) if successful, an error code otherwise.
     */
    sfTkError_t begin(TwoWire &i2cPort)
    {
        // The toolkit bus only accepts a port when it has none, so reset it first.
        uint8_t address = _i2cBus.address();
        _busIsSetup = false;
        _i2cBus = sfTkArdI2C();
        setupBus(i2cPort, address);

        return begin();
    }

    /**
     * @brief Checks if the VL53L1X sensor is connected and responding.
     *
     * @return true If the device responds to a ping and returns a correct sensor ID
     * @return false If the bus is not set up, communication fails or the sensor ID is incorrect
     */
    bool isConnected(void)
    {
        if (!_busIsSetup)
            return false;

        if (_i2cBus.ping() != ksfTkErrOk)
            return false;

        return checkID();
    }

    /**
     * @brief Turns the sensor on if the shutdown pin is connected
     */
    void sensorOn(void)
    {
        if (_shutdownPin >= 0)
            digitalWrite(_shutdownPin, HIGH);

        delay(10);
    }

    /**
     * @brief Turns the sensor off if the shutdown pin is connected
     */
    void sensorOff(void)
    {
        if (_shutdownPin >= 0)
            digitalWrite(_shutdownPin, LOW);

        delay(10);
    }

  private:
    /**
     * @brief Configures the toolkit I2C bus and connects it to the driver
     *
     * @param i2cPort TwoWire instance to use for I2C communication
     * @param address The 7-bit I2C address of the device
     * @return true if the bus was set up, false otherwise
     */
    bool setupBus(TwoWire &i2cPort, uint8_t address)
    {
        _busIsSetup = false;

        if (_i2cBus.init(i2cPort, address) != ksfTkErrOk)
            return false;

        // Device supports repeat starts, enable it.
        _i2cBus.setStop(false);

        setCommunicationBus(&_i2cBus);

        _busIsSetup = true;
        return true;
    }

    /** Arduino I2C bus interface instance for the VL53L1X sensor. */
    sfTkArdI2C _i2cBus;

    /** True once the I2C bus has been set up and connected to the driver. */
    bool _busIsSetup;

    int _shutdownPin;
    int _interruptPin;
};
