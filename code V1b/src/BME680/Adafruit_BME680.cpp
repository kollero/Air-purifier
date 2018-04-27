/*!
 * @file Adafruit_BME680.cpp
 *
 * @mainpage Adafruit BME680 temperature, humidity, barometric pressure and gas sensor driver
 *
 * @section intro_sec Introduction
 *
 * This is the documentation for Adafruit's BME680 driver for the
 * Arduino platform.  It is designed specifically to work with the
 * Adafruit BME680 breakout: https://www.adafruit.com/products/3660
 *
 * These sensors use I2C to communicate, 2 pins (SCL+SDA) are required
 * to interface with the breakout.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * @section author Author
 *
 * Written by Ladyada for Adafruit Industries.
 *
 * @section license License
 *
 * BSD license, all text here must be included in any redistribution.
 *
 */
#include "bme680.c"
#include "Adafruit_BME680.h"

Adafruit_BME680 bme680 = Adafruit_BME680();

Adafruit_BME680::Adafruit_BME680() 
{
	
}

int8_t _BME680_SoftwareSPI_MOSI; ///< Global SPI MOSI pin
int8_t _BME680_SoftwareSPI_MISO; ///< Global SPI MISO pin
int8_t _BME680_SoftwareSPI_SCK;  ///< Globak SPI Clock pin

static int8_t i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
static int8_t i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
static void delay_msec(uint32_t ms);


/**************************************************************************/
/*!
    @brief Initializes the sensor

    Hardware ss initialized, verifies it is in the I2C or SPI bus, then reads
    calibration data in preparation for sensor reads.

    @param  addr Optional parameter for the I2C address of BME680. Default is 0x77
    @return True on sensor initialization success. False on failure.
*/
/**************************************************************************/
bool Adafruit_BME680::begin() {
	_BME680_SoftwareSPI_MOSI = -1;
  _BME680_SoftwareSPI_MISO = -1;
  _BME680_SoftwareSPI_SCK = -1;
	
	
_i2caddr = BME680_ADDRESS;
  _filterEnabled = _tempEnabled = _humEnabled = _presEnabled = _gasEnabled = false;
  
    gas_sensor.dev_id = BME680_ADDRESS;
    gas_sensor.intf = BME680_I2C_INTF;
    gas_sensor.read = &i2c_read;
    gas_sensor.write = &i2c_write;
	gas_sensor.delay_ms = delay_msec;

  int8_t rslt = BME680_OK;
  rslt = bme680_init(&gas_sensor);

  if (rslt != BME680_OK) 
    return false;

  setTemperatureOversampling(BME680_OS_8X);
  setHumidityOversampling(BME680_OS_2X);
  setPressureOversampling(BME680_OS_4X);
  setIIRFilterSize(BME680_FILTER_SIZE_3);
  setGasHeater(320, 150); // 320*C for 150 ms
  
  // don't do anything till we request a reading
  gas_sensor.power_mode = BME680_FORCED_MODE;

  return true;
}

/**************************************************************************/
/*!
    @brief Performs a reading and returns the ambient temperature.
    @return Temperature in degrees Centigrade
*/
/**************************************************************************/
float Adafruit_BME680::readTemperature(void) {
  performReading();
  return temperature;
}

/**************************************************************************/
/*!
    @brief Performs a reading and returns the barometric pressure.
    @return Barometic pressure in Pascals
*/
/**************************************************************************/
float Adafruit_BME680::readPressure(void) {
  performReading();
  return pressure;
}


/**************************************************************************/
/*!
    @brief Performs a reading and returns the relative humidity.
    @return Relative humidity as floating point
*/
/**************************************************************************/
float Adafruit_BME680::readHumidity(void) {
  performReading();
  return humidity;
}

/**************************************************************************/
/*!
    @brief Calculates the resistance of the MOX gas sensor. 
    @return Resistance in Ohms
*/
/**************************************************************************/
uint32_t Adafruit_BME680::readGas(void) {
  performReading();
  return gas_resistance;
}



/**************************************************************************/
/*!
    @brief Performs a full reading of all 4 sensors in the BME680. 

    Assigns the internal Adafruit_BME680#temperature, Adafruit_BME680#pressure, Adafruit_BME680#humidity 
    and Adafruit_BME680#gas_resistance member variables

    @return True on success, False on failure
*/
/**************************************************************************/
bool Adafruit_BME680::performReading(void) {
  uint8_t set_required_settings = 0;
  struct bme680_field_data data;
  int8_t rslt;

  /* Select the power mode */
  /* Must be set before writing the sensor configuration */
  gas_sensor.power_mode = BME680_FORCED_MODE;

  /* Set the required sensor settings needed */
  if (_tempEnabled)
    set_required_settings |= BME680_OST_SEL;
  if (_humEnabled)
    set_required_settings |= BME680_OSH_SEL;
  if (_presEnabled)
    set_required_settings |= BME680_OSP_SEL;
  if (_filterEnabled)
    set_required_settings |= BME680_FILTER_SEL;
  if (_gasEnabled) 
    set_required_settings |= BME680_GAS_SENSOR_SEL;

  /* Set the desired sensor configuration */
  rslt = bme680_set_sensor_settings(set_required_settings, &gas_sensor);
  if (rslt != BME680_OK) 
    return false;
  
  /* Set the power mode */
  rslt = bme680_set_sensor_mode(&gas_sensor);
  if (rslt != BME680_OK) 
    return false;

  /* Get the total measurement duration so as to sleep or wait till the
   * measurement is complete */
  uint16_t meas_period;
  bme680_get_profile_dur(&meas_period, &gas_sensor);
  delay_msec(meas_period * 2); /* Delay till the measurement is ready */
 
  rslt = bme680_get_sensor_data(&data, &gas_sensor);
  if (rslt != BME680_OK) 
    return false;

  if (_tempEnabled) {
    //Serial.print("Temp: "); Serial.println(data.temperature / 100.0, 2);
    temperature = data.temperature / 100.0;
  } else {
    temperature = NAN;
  }

  if (_humEnabled) {
    //Serial.print("Hum:  "); Serial.println(data.humidity / 1000.0, 2);
    humidity = data.humidity / 1000.0;
  } else {
    humidity = NAN;
  }

  if (_presEnabled) {
    //Serial.print("Pres: "); Serial.println(data.pressure / 100.0, 2);
    pressure = data.pressure;
  } else {
    pressure = NAN;
  }

  /* Avoid using measurements from an unstable heating setup */
  if (_gasEnabled) {
    if (data.status & BME680_HEAT_STAB_MSK) {
   
      gas_resistance = data.gas_resistance;
    } else {
      gas_resistance = 0;
  
    }
  }

  return true;
}

/**************************************************************************/
/*!
    @brief  Enable and configure gas reading + heater
    @param  heaterTemp Desired temperature in degrees Centigrade
    @param  heaterTime Time to keep heater on in milliseconds
    @return True on success, False on failure
*/
/**************************************************************************/
bool Adafruit_BME680::setGasHeater(uint16_t heaterTemp, uint16_t heaterTime) {
  gas_sensor.gas_sett.heatr_temp = heaterTemp;
  gas_sensor.gas_sett.heatr_dur = heaterTime;

  if ( (heaterTemp == 0) || (heaterTime == 0) ) {
    // disabled!
    gas_sensor.gas_sett.run_gas = BME680_DISABLE_GAS_MEAS;
    _gasEnabled = false;
  } else {
    gas_sensor.gas_sett.run_gas = BME680_ENABLE_GAS_MEAS;
    _gasEnabled = true;
  }
  return true;
}


/**************************************************************************/
/*!
    @brief  Setter for Temperature oversampling
    @param  oversample Oversampling setting, can be BME680_OS_NONE (turn off Temperature reading), 
    BME680_OS_1X, BME680_OS_2X, BME680_OS_4X, BME680_OS_8X or BME680_OS_16X
    @return True on success, False on failure
*/
/**************************************************************************/

bool Adafruit_BME680::setTemperatureOversampling(uint8_t oversample) {
  if (oversample > BME680_OS_16X) return false;

  gas_sensor.tph_sett.os_temp = oversample;

  if (oversample == BME680_OS_NONE)
    _tempEnabled = false;
  else
    _tempEnabled = true;

  return true;
}


/**************************************************************************/
/*!
    @brief  Setter for Humidity oversampling
    @param  oversample Oversampling setting, can be BME680_OS_NONE (turn off Humidity reading), 
    BME680_OS_1X, BME680_OS_2X, BME680_OS_4X, BME680_OS_8X or BME680_OS_16X
    @return True on success, False on failure
*/
/**************************************************************************/

bool Adafruit_BME680::setHumidityOversampling(uint8_t oversample) {
  if (oversample > BME680_OS_16X) return false;

  gas_sensor.tph_sett.os_hum = oversample;

  if (oversample == BME680_OS_NONE)
    _humEnabled = false;
  else
    _humEnabled = true;

  return true;
}


/**************************************************************************/
/*!
    @brief  Setter for Pressure oversampling
    @param  oversample Oversampling setting, can be BME680_OS_NONE (turn off Pressure reading), 
    BME680_OS_1X, BME680_OS_2X, BME680_OS_4X, BME680_OS_8X or BME680_OS_16X
    @return True on success, False on failure
*/
/**************************************************************************/
bool Adafruit_BME680::setPressureOversampling(uint8_t oversample) {
  if (oversample > BME680_OS_16X) return false;

  gas_sensor.tph_sett.os_pres = oversample;

  if (oversample == BME680_OS_NONE)
    _presEnabled = false;
  else
    _presEnabled = true;

  return true;
}

/**************************************************************************/
/*!
    @brief  Setter for IIR filter.
    @param filtersize Size of the filter (in samples). Can be BME680_FILTER_SIZE_0 (no filtering), 
	BME680_FILTER_SIZE_1, BME680_FILTER_SIZE_3, BME680_FILTER_SIZE_7, BME680_FILTER_SIZE_15, 
	BME680_FILTER_SIZE_31, BME680_FILTER_SIZE_63, BME680_FILTER_SIZE_127
    @return True on success, False on failure
    
*/
/**************************************************************************/
bool Adafruit_BME680::setIIRFilterSize(uint8_t filtersize) {
  if (filtersize > BME680_FILTER_SIZE_127) return false;

  gas_sensor.tph_sett.filter = filtersize;

  if (filtersize == BME680_FILTER_SIZE_0)
    _filterEnabled = false;
  else
    _filterEnabled = true;

  return true;
}


int8_t i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len) {

	uint8_t data[2]={dev_id,reg_addr};
	i2c_ReadData(data, 1, len); //
	
for(uint8_t i=0;i< len;i++) {
	*reg_data=i2cReceiveBuffer[i]; //copy data from receive buffer
	reg_data++;
}
  return 0;
}


int8_t i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len) {
	
	uint8_t data[len+3];
	data[0]=dev_id;
	data[1]=reg_addr;
	
	for(uint8_t i=2;i< len+2;i++) {
		data[i]=	*reg_data;
		reg_data++;
	}
	i2c_TransmitData(data, len+2); 
	
  return 0;
}


static void delay_msec(uint32_t ms){
  delay_ms(ms);
}


float Adafruit_BME680::readAltitude(float seaLevel)
{
    // Equation taken from BMP180 datasheet (page 16):
    //  http://www.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf

    // Note that using the equation from wikipedia can give bad results
    // at high altitude. See this thread for more information:
    //  http://forums.adafruit.com/viewtopic.php?f=22&t=58064

    float atmospheric = readPressure() / 100.0F;
    return 44330.0 * (1.0 - pow(atmospheric / seaLevel, 0.1903));
}






