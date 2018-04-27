
#include "BME280.h"

Adafruit_BME280 bme280 = Adafruit_BME280();

Adafruit_BME280::Adafruit_BME280() 
{
	
}


bool Adafruit_BME280::init()
{
read8(BME280_REGISTER_CHIPID);

// check if sensor, i.e. the chip ID is correct
    if ( i2cReceiveBuffer[0]!= 0x60)
return false;

    // reset the device using soft-reset
    // this makes sure the IIR is off, etc.
    write8(BME280_REGISTER_SOFTRESET, 0xB6);

	 delay_ms(300);
	
	
    // if chip is still reading calibration, delay
    while (isReadingCalibration())
delay_ms(100);
	
  readCoefficients(); // read trimming parameters, see DS 4.2.2

    setSampling(); // use defaults

return true;	
}	
	
void Adafruit_BME280::setSampling(sensor_mode       mode,
		 sensor_sampling   tempSampling,
		 sensor_sampling   pressSampling,
		 sensor_sampling   humSampling,
		 sensor_filter     filter,
		 standby_duration  duration) 
{
    _measReg.mode     = mode;
    _measReg.osrs_t   = tempSampling;
    _measReg.osrs_p   = pressSampling;
        
    
    _humReg.osrs_h    = humSampling;
    _configReg.filter = filter;
    _configReg.t_sb   = duration;

    
    // you must make sure to also set REGISTER_CONTROL after setting the
    // CONTROLHUMID register, otherwise the values won't be applied (see DS 5.4.3)
    write8(BME280_REGISTER_CONTROLHUMID, _humReg.get());
    write8(BME280_REGISTER_CONFIG, _configReg.get());
    write8(BME280_REGISTER_CONTROL, _measReg.get());
}

void Adafruit_BME280::write8(uint8_t reg, uint8_t value) {
	   uint8_t data[3]={BME280_ADDRESS,reg,value};	  
	   i2c_TransmitData(data,2);
    
}

uint8_t Adafruit_BME280::read8(uint8_t reg) {
	
	uint8_t data[2]={BME280_ADDRESS,reg};
	i2c_ReadData(data, 1, 1);
	return i2cReceiveBuffer[0];
}

uint16_t Adafruit_BME280::read16(uint8_t reg)
{
	uint8_t data[2]={BME280_ADDRESS,reg};
	i2c_ReadData(data, 1, 2);
	return (i2cReceiveBuffer[1]|(i2cReceiveBuffer[0]<<8));

}

uint16_t Adafruit_BME280::read16_LE(uint8_t reg) {
	 uint16_t temp = read16(reg);
	return (temp >> 8) | (temp << 8);
}	
int16_t Adafruit_BME280::readS16(uint8_t reg)
{
    return (int16_t)read16(reg);
}
int16_t Adafruit_BME280::readS16_LE(uint8_t reg)
{
    return (int16_t)read16_LE(reg);
}
uint32_t Adafruit_BME280::read24(uint8_t reg)
{
	uint8_t data[2]={BME280_ADDRESS,reg};
	i2c_ReadData(data, 1, 3);
	return (i2cReceiveBuffer[2]|(i2cReceiveBuffer[1]<<8)|(i2cReceiveBuffer[0]<<16));
	
}

void Adafruit_BME280::takeForcedMeasurement()
{   
    // it will take the next measurement and then return to sleep again.
    if (_measReg.mode == MODE_FORCED) {
        // set to forced mode, i.e. "take next measurement"
        write8(BME280_REGISTER_CONTROL, _measReg.get());
        // wait until measurement has been completed, otherwise we would read
        // the values from the last measurement
        while (read8(BME280_REGISTER_STATUS) & 0x08)
		delay_ms(1);
    }
}

void Adafruit_BME280::readCoefficients(void)
{
    _bme280_calib.dig_T1 = read16_LE(BME280_REGISTER_DIG_T1);
    _bme280_calib.dig_T2 = readS16_LE(BME280_REGISTER_DIG_T2);
    _bme280_calib.dig_T3 = readS16_LE(BME280_REGISTER_DIG_T3);

    _bme280_calib.dig_P1 = read16_LE(BME280_REGISTER_DIG_P1);
    _bme280_calib.dig_P2 = readS16_LE(BME280_REGISTER_DIG_P2);
    _bme280_calib.dig_P3 = readS16_LE(BME280_REGISTER_DIG_P3);
    _bme280_calib.dig_P4 = readS16_LE(BME280_REGISTER_DIG_P4);
    _bme280_calib.dig_P5 = readS16_LE(BME280_REGISTER_DIG_P5);
    _bme280_calib.dig_P6 = readS16_LE(BME280_REGISTER_DIG_P6);
    _bme280_calib.dig_P7 = readS16_LE(BME280_REGISTER_DIG_P7);
    _bme280_calib.dig_P8 = readS16_LE(BME280_REGISTER_DIG_P8);
    _bme280_calib.dig_P9 = readS16_LE(BME280_REGISTER_DIG_P9);

    _bme280_calib.dig_H1 = read8(BME280_REGISTER_DIG_H1);
    _bme280_calib.dig_H2 = readS16_LE(BME280_REGISTER_DIG_H2);
    _bme280_calib.dig_H3 = read8(BME280_REGISTER_DIG_H3);
    _bme280_calib.dig_H4 = (read8(BME280_REGISTER_DIG_H4) << 4) | (read8(BME280_REGISTER_DIG_H4+1) & 0xF);
    _bme280_calib.dig_H5 = (read8(BME280_REGISTER_DIG_H5+1) << 4) | (read8(BME280_REGISTER_DIG_H5) >> 4);
    _bme280_calib.dig_H6 = (int8_t)read8(BME280_REGISTER_DIG_H6);
}

bool Adafruit_BME280::isReadingCalibration(void)
{
	
  uint8_t const rStatus=read8(BME280_REGISTER_STATUS);
  
  return (rStatus & (1 << 0)) != 0;
}


double Adafruit_BME280::readTemperature(void)
{
    int32_t var1, var2;
    int32_t adc_T = read24(BME280_REGISTER_TEMPDATA);
    if (adc_T == 0x800000) // value in case temp measurement was disabled
        return NAN;
    adc_T >>= 4;

    var1 = ((((adc_T>>3) - ((int32_t)_bme280_calib.dig_T1 <<1))) *
            ((int32_t)_bme280_calib.dig_T2)) >> 11;
             
    var2 = (((((adc_T>>4) - ((int32_t)_bme280_calib.dig_T1)) *
              ((adc_T>>4) - ((int32_t)_bme280_calib.dig_T1))) >> 12) *
            ((int32_t)_bme280_calib.dig_T3)) >> 14;

    t_fine = var1 + var2;

    double T = (t_fine * 5 + 128) >> 8;
    return T/100;
}


double Adafruit_BME280::readPressure(void) {
    int64_t var1, var2, p;

    readTemperature(); // must be done first to get t_fine

    int32_t adc_P = read24(BME280_REGISTER_PRESSUREDATA);
    if (adc_P == 0x800000) // value in case pressure measurement was disabled
        return NAN;
    adc_P >>= 4;

    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)_bme280_calib.dig_P6;
    var2 = var2 + ((var1*(int64_t)_bme280_calib.dig_P5)<<17);
    var2 = var2 + (((int64_t)_bme280_calib.dig_P4)<<35);
    var1 = ((var1 * var1 * (int64_t)_bme280_calib.dig_P3)>>8) +
           ((var1 * (int64_t)_bme280_calib.dig_P2)<<12);
    var1 = (((((int64_t)1)<<47)+var1))*((int64_t)_bme280_calib.dig_P1)>>33;

    if (var1 == 0) {
        return 0; // avoid exception caused by division by zero
    }
    p = 1048576 - adc_P;
    p = (((p<<31) - var2)*3125) / var1;
    var1 = (((int64_t)_bme280_calib.dig_P9) * (p>>13) * (p>>13)) >> 25;
    var2 = (((int64_t)_bme280_calib.dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t)_bme280_calib.dig_P7)<<4);
    return (double)p/256;
}

double Adafruit_BME280::readHumidity(void) {
    readTemperature(); // must be done first to get t_fine

    int32_t adc_H = read16(BME280_REGISTER_HUMIDDATA);
    if (adc_H == 0x8000) // value in case humidity measurement was disabled
        return NAN;
        
    int32_t v_x1_u32r;

    v_x1_u32r = (t_fine - ((int32_t)76800));

    v_x1_u32r = (((((adc_H << 14) - (((int32_t)_bme280_calib.dig_H4) << 20) -
                    (((int32_t)_bme280_calib.dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) *
                 (((((((v_x1_u32r * ((int32_t)_bme280_calib.dig_H6)) >> 10) *
                      (((v_x1_u32r * ((int32_t)_bme280_calib.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) +
                    ((int32_t)2097152)) * ((int32_t)_bme280_calib.dig_H2) + 8192) >> 14));

    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
                               ((int32_t)_bme280_calib.dig_H1)) >> 4));

    v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
    v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;
    double h = (v_x1_u32r>>12);
    return  h / 1024.0;
}


double Adafruit_BME280::readAltitude(double seaLevel)
{
    // Equation taken from BMP180 datasheet (page 16):
    //  http://www.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf

    // Note that using the equation from wikipedia can give bad results
    // at high altitude. See this thread for more information:
    //  http://forums.adafruit.com/viewtopic.php?f=22&t=58064

    double atmospheric = readPressure() / 100.0F;
    return 44330.0 * (1.0 - pow(atmospheric / seaLevel, 0.1903));
}












