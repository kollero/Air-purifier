#include "CCS811.h"

Adafruit_CCS811 CCS811 = Adafruit_CCS811();

Adafruit_CCS811::Adafruit_CCS811() 
{
	
}


bool Adafruit_CCS811::init()
{

	Wake();
	delay_ms(10);
	SWReset();
	delay_ms(100);
	
	//check that the HW id is correct
	if(read8(CCS811_HW_ID) != CCS811_HW_ID_CODE)
		return false;
	
	//try to start the app
	write(CCS811_BOOTLOADER_APP_START, NULL, 0);
	delay_ms(100);
	
	//make sure there are no errors and we have entered application mode
	if(checkError()) return false;
	if(!_status.FW_MODE) return false;
	
	disableInterrupt();
	
	//default to read every second
	setDriveMode(CCS811_DRIVE_MODE_250MS);
	
	return true;
}


void Adafruit_CCS811::setDriveMode(uint8_t mode)
{
	_meas_mode.DRIVE_MODE = mode;
	write8(CCS811_MEAS_MODE, _meas_mode.get());
}

void Adafruit_CCS811::enableInterrupt()
{
	_meas_mode.INT_DATARDY = 1;
	write8(CCS811_MEAS_MODE, _meas_mode.get());
}

void Adafruit_CCS811::disableInterrupt()
{
	_meas_mode.INT_DATARDY = 0;
	write8(CCS811_MEAS_MODE, _meas_mode.get());
}

bool Adafruit_CCS811::available()
{
	_status.set(read8(CCS811_STATUS));
	if(!_status.DATA_READY)
		return false;
	else return true;
}


void Adafruit_CCS811::setEnvironmentalData(double humidity, double temperature)
{
	/* Humidity is stored as an unsigned 16 bits in 1/512%RH. The
	default value is 50% = 0x64, 0x00. As an example 48.5%
	humidity would be 0x61, 0x00.*/
	
	/* Temperature is stored as an unsigned 16 bits integer in 1/512
	degrees; there is an offset: 0 maps to -25°C. The default value is
	25°C = 0x64, 0x00. As an example 23.5 temperature would be
	0x61, 0x00.
	The internal algorithm uses these values (or default values if
	not set by the application) to compensate for changes in
	relative humidity and ambient temperature.*/
			
	uint16_t hum_perc = (humidity*10 + 2) / 5;
	uint16_t temp_perc = (temperature*10 + 2) / 5;
	temp_perc +=50; //25c offset
	
	uint8_t buf[] = {(uint8_t)hum_perc, 0x00, (uint8_t)temp_perc, 0x00};
	
	write(CCS811_ENV_DATA, buf, 4);

}

void Adafruit_CCS811::setThresholds(uint16_t low_med, uint16_t med_high, uint8_t hysteresis)
{
	uint8_t buf[] = {(uint8_t)((low_med >> 8) & 0xFF), (uint8_t)(low_med & 0xFF),
	(uint8_t)((med_high >> 8) & 0xFF), (uint8_t)(med_high & 0xFF), hysteresis};
	
	write(CCS811_THRESHOLDS, buf, 5);
}

void Adafruit_CCS811::SWReset()
{
	//reset sequence from the datasheet
	uint8_t seq[] = {0x11, 0xE5, 0x72, 0x8A};
	write(CCS811_SW_RESET, seq, 4);
}

bool Adafruit_CCS811::checkError()
{
	_status.set(read8(CCS811_STATUS));
	return _status.ERROR;
}

void Adafruit_CCS811::write8(uint8_t reg, uint8_t value)
{
	 uint8_t data[3]={CCS811_ADDRESS,reg,value};	  
	 i2c_TransmitData(data,2);
}

uint8_t Adafruit_CCS811::read8(uint8_t reg)
{
	uint8_t data[2]={CCS811_ADDRESS,reg};
	i2c_ReadData(data, 1, 1);
	return i2cReceiveBuffer[0];

}

void Adafruit_CCS811::read(uint8_t reg, uint8_t *buf, uint8_t num)
{
	uint8_t data[2]={CCS811_ADDRESS,reg};
	i2c_ReadData(data, 1, num);
	
	for(int i=0; i<num; i++)
	{
		buf[i] = i2cReceiveBuffer[i];
	}
}	
	

void Adafruit_CCS811::write(uint8_t reg, uint8_t *buf, uint8_t num)
{
	
	uint8_t data[num+2];
	data[0]=CCS811_ADDRESS;
	data[1]=reg;
	for(int i=2; i<num+2; i++)
	{
		data[i]=buf[i-2];
	}
	
	i2c_TransmitData(data,num+2);
	
}


uint8_t Adafruit_CCS811::readData()
{
	if(!available())
		return false;
	else{
		uint8_t buf[8];
		read(CCS811_ALG_RESULT_DATA, buf, 8);

		_eCO2 = ((uint16_t)buf[0] << 8) | ((uint16_t)buf[1]);
		_TVOC = ((uint16_t)buf[2] << 8) | ((uint16_t)buf[3]);
		
		if(_status.ERROR)
			return buf[5];
			
		else return 0;
	}
}

void Adafruit_CCS811::Wake()
{
	//pb02
	REG_PORT_OUTCLR1=(1<<2); //for pin low	
}
void Adafruit_CCS811::Sleep()
{
	
	REG_PORT_OUTSET1=(1<<2); //for pin high	
}

















