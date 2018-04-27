/*!
 * @file Adafruit_BME680.h
 *
 * Adafruit BME680 temperature, humidity, barometric pressure and gas sensor driver
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
 * Written by Ladyada for Adafruit Industries.
 *
 * BSD license, all text here must be included in any redistribution.
 *
 */
 

#include "bme680.h"

#define BME680_ADDRESS 0b11101100
 
//#define BME680_ADDRESS 0x77    


class Adafruit_BME680
{
  public:
	Adafruit_BME680();
   
    bool  begin();
    float readTemperature(void);
    float readPressure(void);
    float readHumidity(void);
    uint32_t readGas(void);
    float readAltitude(float seaLevel);

    bool setTemperatureOversampling(uint8_t os);
    bool setPressureOversampling(uint8_t os);
    bool setHumidityOversampling(uint8_t os);
    bool setIIRFilterSize(uint8_t fs);
    bool setGasHeater(uint16_t heaterTemp, uint16_t heaterTime);

    bool performReading(void);

    /// Temperature (Celsius) assigned after calling performReading() 
    float temperature;  
    /// Pressure (Pascals) assigned after calling performReading() 
    float pressure;
    /// Humidity (RH %) assigned after calling performReading()
    float humidity;
    /// Gas resistor (ohms) assigned after calling performReading()
    float gas_resistance;
  private:

    bool _filterEnabled, _tempEnabled, _humEnabled, _presEnabled, _gasEnabled;

    int32_t _sensorID;
    int8_t _cs;
	uint8_t _i2caddr;
    uint8_t spixfer(uint8_t x);

    struct bme680_dev gas_sensor; //rename bosch provided 
};


