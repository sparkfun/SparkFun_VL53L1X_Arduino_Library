/*
  This is a library written for the VL53L1X
  SparkFun sells these at its website: www.sparkfun.com
  Do you like this library? Help support SparkFun. Buy a board!
  https://www.sparkfun.com/products/14667

  Written by Nathan Seidle @ SparkFun Electronics, April 12th, 2018

  The VL53L1X is the latest Time Of Flight (ToF) sensor to be released. It uses
  a VCSEL (vertical cavity surface emitting laser) to emit a class 1 IR laser
  and time the reflection to the target. What does all this mean? You can measure
  the distance to an object up to 4 meters away with millimeter resolution!

  We’ve found the precision of the sensor to be 1mm but the accuracy is around +/-5mm.

  This library handles the initialization of the VL53L1X and is able to query the sensor
  for different readings.

  Because ST has chosen not to release a complete datasheet we are forced to reverse
  engineer the interface from their example code and I2C data stream captures.
  For ST's reference code see STSW-IMG007

  The device operates as a normal I2C device. There are *many* registers. Reading and 
  writing happens with a 16-bit address. The VL53L1X auto-increments the memory
  pointer with each read or write.

  Development environment specifics:
  Arduino IDE 1.8.5

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "SparkFun_VL53L1X_Arduino_Library.h"

//This is 135 bytes to be written every time to the VL53L1X to initiate a measurement
//0x29 is written to memory location 0x01, which is the register for the I2C address which
//is indeed 0x29. So this makes sense. We could evaluate the default register settings of a given
//device against this set of bytes and write only the ones that are different but it's faster,
//easier, and perhaps fewer code words to write the config block as is.
//The block was obtained via inspection of the ST P-NUCLEO-53L1A1
uint8_t configBlock[] = {
  0x29, 0x02, 0x10, 0x00, 0x28, 0xBC, 0x7A, 0x81, //8
  0x80, 0x07, 0x95, 0x00, 0xED, 0xFF, 0xF7, 0xFD, //16
  0x9E, 0x0E, 0x00, 0x10, 0x01, 0x00, 0x00, 0x00, //24
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x34, 0x00, //32
  0x28, 0x00, 0x0D, 0x0A, 0x00, 0x00, 0x00, 0x00, //40
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, //48
  0x02, 0x00, 0x02, 0x08, 0x00, 0x08, 0x10, 0x01, //56
  0x01, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x02, //64
  0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x0B, 0x00, //72
  0x00, 0x02, 0x0A, 0x21, 0x00, 0x00, 0x02, 0x00, //80
  0x00, 0x00, 0x00, 0xC8, 0x00, 0x00, 0x38, 0xFF, //88
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x91, 0x0F, //96
  0x00, 0xA5, 0x0D, 0x00, 0x80, 0x00, 0x0C, 0x08, //104
  0xB8, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x10, 0x00, //112
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x0F, //120
  0x0D, 0x0E, 0x0E, 0x01, 0x00, 0x02, 0xC7, 0xFF, //128
  0x8B, 0x00, 0x00, 0x00, 0x01, 0x01, 0x40 //129 - 135 (0x81 - 0x87)
};

//Check to see if sensor is responding
//Set sensor up for 2.8/3.3V I2C
//Return true if succesful
boolean VL53L1X::begin(uint8_t deviceAddress, TwoWire &wirePort)
{
  _deviceAddress = deviceAddress; //If provided, store the I2C address from user
  _i2cPort = &wirePort; //Grab which port the user wants us to use

  //We expect caller to begin their I2C port, with the speed of their choice external to the library
  //But if they forget, we start the hardware here.
  _i2cPort->begin();

  //Check the device ID
  uint16_t modelID = readRegister16(VL53L1_IDENTIFICATION__MODEL_ID);
  if (modelID != 0xEACC) return (false);

  softReset();

  //Polls the bit 0 of the FIRMWARE__SYSTEM_STATUS register to see if the firmware is ready
  int counter = 0;
  while (readRegister16(VL53L1_FIRMWARE__SYSTEM_STATUS) & 0x01 == 0)
  {
    if (counter++ == 100) return (false); //Sensor timed out
    delay(10);
  }

  //Set I2C to 2.8V mode. In this mode 3.3V I2C is allowed.
  uint16_t result = readRegister16(VL53L1_PAD_I2C_HV__EXTSUP_CONFIG);
  result = (result & 0xFE) | 0x01;
  writeRegister16(VL53L1_PAD_I2C_HV__EXTSUP_CONFIG, result);

  //Gets trim resistors from chip
  for (uint16_t i = 0; i < 36; i++) {
	  uint8_t regVal = readRegister(i + 1);
	  configBlock[i] = regVal;
  }
  startMeasurement();

  return (true); //Sensor online!
}

//Write the configuration block with a max of I2C_BUFFER_LENGTH bytes at a time
//Offset allows us to start at a location within the configBlock array
//This is the main function that setups up the VL53L1X to take measurements
//This was obtained by inspecting the example software from ST and by
//capturing I2C trace on ST Nucleo demo board
void VL53L1X::startMeasurement(uint8_t offset)
{
  uint8_t address = 1 + offset; //Start at memory location 0x01, add offset
  uint8_t leftToSend = sizeof(configBlock) - offset;
  while (leftToSend > 0)
  {
    uint16_t toSend = I2C_BUFFER_LENGTH - 2; //Max I2C buffer on Arduino is 32, and we need 2 bytes for address
    if (toSend > leftToSend) toSend = leftToSend;

    _i2cPort->beginTransmission(_deviceAddress);

    _i2cPort->write(0); //We're only in lower address space. No MSB needed.
    _i2cPort->write(address);

    for (byte x = 0 ; x < toSend ; x++)
      _i2cPort->write(configBlock[address + x - 1 - offset]);

    _i2cPort->endTransmission();

    leftToSend -= toSend;
    address += toSend;
  }
}

//Polls the measurement completion bit
boolean VL53L1X::newDataReady()
{
  if (readRegister(VL53L1_GPIO__TIO_HV_STATUS) != 0x03) return(true); //New measurement!
  return(false); //No new data
}

//Reset sensor via software
void VL53L1X::softReset()
{
  writeRegister(VL53L1_SOFT_RESET, 0x00); //Reset
  delay(1); //Driver uses 100us
  writeRegister(VL53L1_SOFT_RESET, 0x01); //Exit reset
}

//The sensor has 44 bytes of various datums.
//See VL53L1_i2c_decode_system_results() in vl53l1_register_funcs.c for the decoder ringer
//Start from memory address VL53L1_RESULT__INTERRUPT_STATUS
//0: result__interrupt_status,
//1: result__range_status, VL53L1_RESULT__RANGE_STATUS
//2: result__report_status, VL53L1_RESULT__REPORT_STATUS
//3: result__stream_count
//4: result__dss_actual_effective_spads_sd0
//6: result__peak_signal_count_rate_mcps_sd0
//8: result__ambient_count_rate_mcps_sd0
//10: result__sigma_sd0
//12: result__phase_sd0
//14: result__final_crosstalk_corrected_range_mm_sd0
//16: result__peak_signal_count_rate_crosstalk_corrected_mcps_sd0
//18: result__mm_inner_actual_effective_spads_sd0
//20: result__mm_outer_actual_effective_spads_sd0
//22: result__avg_signal_count_rate_mcps_sd0
//24: result__dss_actual_effective_spads_sd1
//26: result__peak_signal_count_rate_mcps_sd1
//28: result__ambient_count_rate_mcps_sd1
//30: result__sigma_sd1
//32: result__phase_sd1
//34: result__final_crosstalk_corrected_range_mm_sd1
//36: result__spare_0_sd1
//38: result__spare_1_sd1
//40: result__spare_2_sd1
//42: result__spare_3_sd1
//44: result__thresh_info

//Get the 'final' results from measurement
uint16_t VL53L1X::getDistance()
{
  return (readRegister16(VL53L1_RESULT__FINAL_CROSSTALK_CORRECTED_RANGE_MM_SD0));
}

//Get signal rate
//This seems to be a number representing the quality of the measurement, the number of SPADs used perhaps
uint16_t VL53L1X::getSignalRate()
{
  //From vl53l1_api.c line 2041
  uint16_t reading = readRegister16(VL53L1_RESULT__PEAK_SIGNAL_COUNT_RATE_CROSSTALK_CORRECTED_MCPS_SD0);// << 9; //FIXPOINT97TOFIXPOINT1616
  //float signalRate = (float)reading/65536.0;
  return (reading);
}


//Adapted from ST API
//0 is short range, 1 is mid range, 2 is long range.
void VL53L1X::setDistanceMode(uint8_t mode)
{
	uint8_t periodA;
	uint8_t periodB;
	uint8_t phaseHigh;
	uint8_t phaseInit;
	switch (mode)
	{
		case 0:
		  periodA = 0x07;
		  periodB = 0x05;
		  phaseHigh = 0x38;
		  phaseInit = 6;
		  break;
		case 1:
		  periodA = 0x0B;
		  periodB = 0x09;
	  	  phaseHigh = 0x78;
	  	  phaseInit = 10;
		  break;
		case 2:
		  periodA = 0x0F;
		  periodB = 0x0D;
		  phaseHigh = 0xB8;
		  phaseInit = 14;
		  break;
		//If user inputs wrong range, we default to long range
		default:
		  periodA = 0x0F;
		  periodB = 0x0D;
		  phaseHigh = 0xB8;
		  phaseInit = 14;
		  break;
	}
	//timing
	writeRegister(VL53L1_RANGE_CONFIG__VCSEL_PERIOD_A, periodA);
	writeRegister(VL53L1_RANGE_CONFIG__VCSEL_PERIOD_B , periodB);
	writeRegister(VL53L1_RANGE_CONFIG__VALID_PHASE_HIGH, phaseHigh);
	
	//dynamic
	writeRegister(VL53L1_SD_CONFIG__WOI_SD0 , periodA);
	writeRegister(VL53L1_SD_CONFIG__WOI_SD1, periodB);
	writeRegister(VL53L1_SD_CONFIG__INITIAL_PHASE_SD0, phaseInit);
	writeRegister(VL53L1_SD_CONFIG__INITIAL_PHASE_SD1, phaseInit);
	
	_distanceMode = mode;
}

uint8_t VL53L1X::getDistanceMode()
{
	return _distanceMode;
}

//Set a custom zone from the array of sensors.  Minimum of 4x4, maximum of 16x16.
//lower left corner of the array is (0, 0) and upper right is (15, 15)
void VL53L1X::setUserRoi(UserRoi *roi)
{
	uint8_t centerX = (roi->topLeftX + roi->bottomRightX + 1) / 2;
	uint8_t centerY = (roi->topLeftY + roi->bottomRightY + 1) / 2;
	uint8_t width = roi->bottomRightX - roi->topLeftX;
	uint8_t height = roi->topLeftY - roi->bottomRightY;
	
	//Check boundary conditions, if incorrect set to default values.
	if (width < 3 || height < 3){
		setCenter((uint8_t)8, (uint8_t)8);
		setZoneSize((uint8_t)15, (uint8_t)15);
	}
	else{
		setCenter(centerX, centerY);
		setZoneSize(width, height);
	}
}

void VL53L1X::setCenter(uint8_t centerX, uint8_t centerY){
	uint8_t centerValue;
	
	if (centerY > 7){
		centerValue = 128 + (centerX << 3) + (15 - centerY);
	} 
	else {
		centerValue = ((15 - centerX) << 3) + centerY;
	}
	
	writeRegister(VL53L1_ROI_CONFIG__USER_ROI_CENTRE_SPAD , centerValue);
}

void VL53L1X::setZoneSize(uint8_t width, uint8_t height){
	uint8_t dimensions = (height << 4)  + width;
	writeRegister(VL53L1_ROI_CONFIG__USER_ROI_REQUESTED_GLOBAL_XY_SIZE, dimensions);
}

UserRoi* VL53L1X::getUserRoi(){
	UserRoi* roi = new UserRoi();
	
	uint8_t center = readRegister(VL53L1_ROI_CONFIG__USER_ROI_CENTRE_SPAD);
	uint8_t row = 0;
	uint8_t col = 0;
	if (center > 127){
		row = 8 + ((255-center) & 0x07);
		col = (center - 128) >> 3;
	} else {
		row = center & 0x07;
		col = (127 - center) >> 3;
	}
	
	uint8_t dimensions = readRegister(VL53L1_ROI_CONFIG__USER_ROI_REQUESTED_GLOBAL_XY_SIZE);
	uint8_t height = dimensions >> 4;
	uint8_t width = dimensions & 0x0F;

	 
	roi->topLeftX = (2 * col - width) >> 1;
	roi->topLeftY = (2 * row - height) >> 1;
	roi->bottomRightX = (2 * col + width) >> 1;
	roi->bottomRightY = (2 * row + height) >> 1;
	
	return roi;
}

//The sensor returns a range status that needs to be re-mapped to one of 9 different statuses
//This does that.
uint8_t VL53L1X::getRangeStatus()
{
#define VL53L1_DEVICEERROR_VCSELCONTINUITYTESTFAILURE  ( 1)
#define VL53L1_DEVICEERROR_VCSELWATCHDOGTESTFAILURE    ( 2)
#define VL53L1_DEVICEERROR_NOVHVVALUEFOUND             ( 3)
#define VL53L1_DEVICEERROR_MSRCNOTARGET                ( 4)
#define VL53L1_DEVICEERROR_RANGEPHASECHECK             ( 5)
#define VL53L1_DEVICEERROR_SIGMATHRESHOLDCHECK         ( 6)
#define VL53L1_DEVICEERROR_PHASECONSISTENCY            ( 7)
#define VL53L1_DEVICEERROR_MINCLIP                     ( 8)
#define VL53L1_DEVICEERROR_RANGECOMPLETE               ( 9)
#define VL53L1_DEVICEERROR_ALGOUNDERFLOW               ( 10)
#define VL53L1_DEVICEERROR_ALGOOVERFLOW                ( 11)
#define VL53L1_DEVICEERROR_RANGEIGNORETHRESHOLD        ( 12)
#define VL53L1_DEVICEERROR_USERROICLIP                 ( 13)
#define VL53L1_DEVICEERROR_REFSPADCHARNOTENOUGHDPADS   ( 14)
#define VL53L1_DEVICEERROR_REFSPADCHARMORETHANTARGET   ( 15)
#define VL53L1_DEVICEERROR_REFSPADCHARLESSTHANTARGET   ( 16)
#define VL53L1_DEVICEERROR_MULTCLIPFAIL                ( 17)
#define VL53L1_DEVICEERROR_GPHSTREAMCOUNT0READY        ( 18)
#define VL53L1_DEVICEERROR_RANGECOMPLETE_NO_WRAP_CHECK ( 19)
#define VL53L1_DEVICEERROR_EVENTCONSISTENCY            ( 20)
#define VL53L1_DEVICEERROR_MINSIGNALEVENTCHECK         ( 21)
#define VL53L1_DEVICEERROR_RANGECOMPLETE_MERGED_PULSE  ( 22)

#define VL53L1_RANGESTATUS_RANGE_VALID       0 /*!<The Range is valid. */
#define VL53L1_RANGESTATUS_SIGMA_FAIL        1 /*!<Sigma Fail. */
#define VL53L1_RANGESTATUS_SIGNAL_FAIL       2 /*!<Signal fail. */
#define VL53L1_RANGESTATUS_RANGE_VALID_MIN_RANGE_CLIPPED 3 /*!<Target is below minimum detection threshold. */
#define VL53L1_RANGESTATUS_OUTOFBOUNDS_FAIL      4 /*!<Phase out of valid limits -  different to a wrap exit. */
#define VL53L1_RANGESTATUS_HARDWARE_FAIL     5 /*!<Hardware fail. */
#define VL53L1_RANGESTATUS_RANGE_VALID_NO_WRAP_CHECK_FAIL  6 /*!<The Range is valid but the wraparound check has not been done. */
#define VL53L1_RANGESTATUS_WRAP_TARGET_FAIL     7 /*!<Wrapped target - no matching phase in other VCSEL period timing. */
#define VL53L1_RANGESTATUS_PROCESSING_FAIL      8 /*!<Internal algo underflow or overflow in lite ranging. */
#define VL53L1_RANGESTATUS_XTALK_SIGNAL_FAIL      9 /*!<Specific to lite ranging. */
#define VL53L1_RANGESTATUS_SYNCRONISATION_INT     10 /*!<1st interrupt when starting ranging in back to back mode. Ignore data. */
#define VL53L1_RANGESTATUS_RANGE_VALID_MERGED_PULSE   11 /*!<All Range ok but object is result of multiple pulses merging together.*/
#define VL53L1_RANGESTATUS_TARGET_PRESENT_LACK_OF_SIGNAL  12 /*!<Used  by RQL  as different to phase fail. */
#define VL53L1_RANGESTATUS_MIN_RANGE_FAIL     13 /*!<User ROI input is not valid e.g. beyond SPAD Array.*/
#define VL53L1_RANGESTATUS_RANGE_INVALID      14 /*!<lld returned valid range but negative value ! */
#define VL53L1_RANGESTATUS_NONE        255 /*!<No Update. */

  //Read status
  uint8_t measurementStatus = readRegister(VL53L1_RESULT__RANGE_STATUS) & 0x1F;

  //Convert status from one to another - From vl53l1_api.c
  switch (measurementStatus) {
    case VL53L1_DEVICEERROR_GPHSTREAMCOUNT0READY:
      measurementStatus = VL53L1_RANGESTATUS_SYNCRONISATION_INT;
      break;
    case VL53L1_DEVICEERROR_RANGECOMPLETE_NO_WRAP_CHECK:
      measurementStatus = VL53L1_RANGESTATUS_RANGE_VALID_NO_WRAP_CHECK_FAIL;
      break;
    case VL53L1_DEVICEERROR_RANGEPHASECHECK:
      measurementStatus = VL53L1_RANGESTATUS_OUTOFBOUNDS_FAIL;
      break;
    case VL53L1_DEVICEERROR_MSRCNOTARGET:
      measurementStatus = VL53L1_RANGESTATUS_SIGNAL_FAIL;
      break;
    case VL53L1_DEVICEERROR_SIGMATHRESHOLDCHECK:
      measurementStatus = VL53L1_RANGESTATUS_SIGMA_FAIL;
      break;
    case VL53L1_DEVICEERROR_PHASECONSISTENCY:
      measurementStatus = VL53L1_RANGESTATUS_WRAP_TARGET_FAIL;
      break;
    case VL53L1_DEVICEERROR_RANGEIGNORETHRESHOLD:
      measurementStatus = VL53L1_RANGESTATUS_XTALK_SIGNAL_FAIL;
      break;
    case VL53L1_DEVICEERROR_MINCLIP:
      measurementStatus = VL53L1_RANGESTATUS_RANGE_VALID_MIN_RANGE_CLIPPED;
      break;
    case VL53L1_DEVICEERROR_RANGECOMPLETE:
      measurementStatus = VL53L1_RANGESTATUS_RANGE_VALID;
      break;
    default:
      measurementStatus = VL53L1_RANGESTATUS_NONE;
  }

  return measurementStatus;
}

//Reads one byte from a given location
//Returns zero on error
uint8_t VL53L1X::readRegister(uint16_t addr)
{
  _i2cPort->beginTransmission(_deviceAddress);
  _i2cPort->write(addr >> 8); //MSB
  _i2cPort->write(addr & 0xFF); //LSB
  if (_i2cPort->endTransmission() != 0) //Send a restart command. Do not release bus.
    return (0); //Sensor did not ACK

  _i2cPort->requestFrom((uint8_t)_deviceAddress, (uint8_t)1);
  if (_i2cPort->available())
    return (_i2cPort->read());

  return (0); //Error: Sensor did not respond
}

//Reads two consecutive bytes from a given location
//Returns zero on error
uint16_t VL53L1X::readRegister16(uint16_t addr)
{
  _i2cPort->beginTransmission(_deviceAddress);
  _i2cPort->write(addr >> 8); //MSB
  _i2cPort->write(addr & 0xFF); //LSB
  if (_i2cPort->endTransmission() != 0) //Send a restart command. Do not release bus.
    return (0); //Sensor did not ACK

  _i2cPort->requestFrom((uint8_t)_deviceAddress, (uint8_t)2);
  if (_i2cPort->available())
  {
    uint8_t msb = _i2cPort->read();
    uint8_t lsb = _i2cPort->read();
    return ((uint16_t)msb << 8 | lsb);
  }

  return (0); //Error: Sensor did not respond
}

//Write a byte to a spot
boolean VL53L1X::writeRegister(uint16_t addr, uint8_t val)
{
  _i2cPort->beginTransmission(_deviceAddress);
  _i2cPort->write(addr >> 8); //MSB
  _i2cPort->write(addr & 0xFF); //LSB
  _i2cPort->write(val);
  if (_i2cPort->endTransmission() != 0)
    return (0); //Error: Sensor did not ACK
  return (1); //All done!
}

//Write two bytes to a spot
boolean VL53L1X::writeRegister16(uint16_t addr, uint16_t val)
{
  _i2cPort->beginTransmission(_deviceAddress);
  _i2cPort->write(addr >> 8); //MSB
  _i2cPort->write(addr & 0xFF); //LSB
  _i2cPort->write(val >> 8); //MSB
  _i2cPort->write(val & 0xFF); //LSB
  if (_i2cPort->endTransmission() != 0)
    return (0); //Error: Sensor did not ACK
  return (1); //All done!
}
