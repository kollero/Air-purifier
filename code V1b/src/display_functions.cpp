
#define TEMPERATURE_x_OFFSET 5
#define TEMPERATURE_y_OFFSET 16

#define CO2_x_OFFSET 0
#define CO2_y_OFFSET 16

#define PM_AQI_x_OFFSET 0
#define PM_AQI_y_OFFSET 16

#define RPM_x_OFFSET 0
#define RPM_y_OFFSET 0

#define PM_x_OFFSET 30
#define PM_y_OFFSET 0

/*
//with bme280
void draw_humidity_data()
{
	
	
	char string_temp[10]={0,0,0,0,0,0,0,0,0,0};
	char string_pressure[10]={0,0,0,0,0,0,0,0,0,0};
	char string_humidity[10]={0,0,0,0,0,0,0,0,0,0};
			
	
	temp = bme280.readTemperature();

	pressure=bme280.readPressure() / 100.0F;
	
	ftoad(temp,string_temp,1);
	//temperature
	gde027w3.drawString_gfx_n( 47+TEMPERATURE_x_OFFSET, 12+TEMPERATURE_y_OFFSET, string_temp, GDEW027W3_BLACK, GDEW027W3_WHITE, 1,1);
	gde027w3.drawChar_num( 50+TEMPERATURE_x_OFFSET, 12+TEMPERATURE_y_OFFSET, 176, GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
	gde027w3.drawChar_num( 55+TEMPERATURE_x_OFFSET, 12+TEMPERATURE_y_OFFSET, 'c', GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
	
	//humidity
	ftoad(humidity,string_humidity,1);
	gde027w3.drawString_gfx_n( 47+TEMPERATURE_x_OFFSET, 27+TEMPERATURE_y_OFFSET, string_humidity, GDEW027W3_BLACK, GDEW027W3_WHITE, 1,1);
	gde027w3.drawChar_CAPI( 55+TEMPERATURE_x_OFFSET, 27+TEMPERATURE_y_OFFSET, '%', GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
	
	//pressure
	ftoad(pressure,string_pressure,0);
	gde027w3.drawString_gfx_n(47+TEMPERATURE_x_OFFSET, 42+TEMPERATURE_y_OFFSET, string_pressure, GDEW027W3_BLACK, GDEW027W3_WHITE, 1,1);
	char string_pa[3]="Pa";
	gde027w3.drawString_gfx_C( 55+TEMPERATURE_x_OFFSET, 42+TEMPERATURE_y_OFFSET, string_pa, GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
}
*/

//with bme680
void draw_humidity_data()
{
	
	char string_temp[10]={0,0,0,0,0,0,0,0,0,0};
	char string_pressure[10]={0,0,0,0,0,0,0,0,0,0};
	char string_humidity[10]={0,0,0,0,0,0,0,0,0,0};
			
	bme680.performReading();
	
	temp =bme680.temperature;
	pressure=bme680.pressure / 100.0;
	humidity=bme680.humidity;
	TVCO=bme680.gas_resistance / 1000.0;
	
	ftoad(temp,string_temp,1);
	//temperature
	gde027w3.drawString_gfx_n( 47+TEMPERATURE_x_OFFSET, 12+TEMPERATURE_y_OFFSET, string_temp, GDEW027W3_BLACK, GDEW027W3_WHITE, 1,1);
	gde027w3.drawChar_num( 50+TEMPERATURE_x_OFFSET, 12+TEMPERATURE_y_OFFSET, 176, GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
	gde027w3.drawChar_num( 55+TEMPERATURE_x_OFFSET, 12+TEMPERATURE_y_OFFSET, 'c', GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
	
	//humidity
	ftoad(humidity,string_humidity,1);
	gde027w3.drawString_gfx_n( 47+TEMPERATURE_x_OFFSET, 27+TEMPERATURE_y_OFFSET, string_humidity, GDEW027W3_BLACK, GDEW027W3_WHITE, 1,1);
	gde027w3.drawChar_CAPI( 55+TEMPERATURE_x_OFFSET, 27+TEMPERATURE_y_OFFSET, '%', GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
	
	//pressure
	ftoad(pressure,string_pressure,0);
	gde027w3.drawString_gfx_n(47+TEMPERATURE_x_OFFSET, 42+TEMPERATURE_y_OFFSET, string_pressure, GDEW027W3_BLACK, GDEW027W3_WHITE, 1,1);
	char string_pa[3]="Pa";
	gde027w3.drawString_gfx_C( 55+TEMPERATURE_x_OFFSET, 42+TEMPERATURE_y_OFFSET, string_pa, GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
	
	
	
	
	
	
	
	
	
}


void draw_co2_data()
{
	char string_co2_ndir[10]={0,0,0,0,0,0,0,0,0,0};
	//char string_co2_css811[10]={0,0,0,0,0,0,0,0,0,0};
	intToStr(co2_concentration,string_co2_ndir,1);
	//intToStr(,string_co2_css811,2);
	char string_co2[5]="CO2:";
	
	gde027w3.drawString_gfx_C( 62+CO2_x_OFFSET, 65+CO2_y_OFFSET, string_co2 ,  GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
	gde027w3.drawString_gfx_n( 135+CO2_x_OFFSET, 65+CO2_y_OFFSET, string_co2_ndir ,  GDEW027W3_BLACK, GDEW027W3_WHITE, 1,1); //in ppm
	
	
	
}



void draw_air_quality_data()
{
	char string_concPM1_0_CF1[10]={0,0,0,0,0,0,0,0,0,0};
	char string_concPM2_5_CF1[10]={0,0,0,0,0,0,0,0,0,0};
	char string_concPM10_0_CF1[10]={0,0,0,0,0,0,0,0,0,0};

	char string_aqiPM2_5_CF1[10]={0,0,0,0,0,0,0,0,0,0};
	char string_aqiPM10_CF1[10]={0,0,0,0,0,0,0,0,0,0};

	
	char string_pm[3]="PM";
	char string_pm1_0[5]="1.0:";
	char string_pm2_5[5]="2.5:";
	char string_pm10_0[4]="10:";
		
	char string_AQI_PM2_5[6]="AQIPM";
	char string_AQI_PM2_5__2[5]="2.5:";
	char string_AQI_PM10[6]="AQIPM";
	char string_AQI_PM10__2[4]="10:";
	
	if(pms7003.pms7003_values_done)
	{
		
	intToStr(pms7003.old_avg_PM1_0_CF1,string_concPM1_0_CF1,1);
	intToStr(pms7003.old_avg_PM2_5_CF1,string_concPM2_5_CF1,1);
	intToStr(pms7003.old_avg_PM10_CF1,string_concPM10_0_CF1,1);
	
	//PM values to the right
	gde027w3.drawString_gfx_n( 200+PM_x_OFFSET, 12+PM_y_OFFSET,string_pm1_0 ,  GDEW027W3_BLACK, GDEW027W3_WHITE, 1,1);
	gde027w3.drawString_gfx_C( 140+PM_x_OFFSET, 12+PM_y_OFFSET,string_pm ,  GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
	gde027w3.drawString_gfx_n( 200+PM_x_OFFSET, 27+PM_y_OFFSET,string_pm2_5,  GDEW027W3_BLACK, GDEW027W3_WHITE, 1,1);
	gde027w3.drawString_gfx_C( 140+PM_x_OFFSET, 27+PM_y_OFFSET,string_pm ,  GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
	gde027w3.drawString_gfx_n( 200+PM_x_OFFSET, 42+PM_y_OFFSET,string_pm10_0 ,  GDEW027W3_BLACK, GDEW027W3_WHITE, 1,1);
	gde027w3.drawString_gfx_C( 140+PM_x_OFFSET, 42+PM_y_OFFSET,string_pm ,  GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
	
	
	gde027w3.drawString_gfx_n( 230+PM_x_OFFSET, 12+PM_y_OFFSET, string_concPM1_0_CF1 ,  GDEW027W3_BLACK, GDEW027W3_WHITE, 1,1);
	gde027w3.drawString_gfx_n( 230+PM_x_OFFSET, 27+PM_y_OFFSET, string_concPM2_5_CF1,  GDEW027W3_BLACK, GDEW027W3_WHITE, 1,1);
	gde027w3.drawString_gfx_n( 230+PM_x_OFFSET, 42+PM_y_OFFSET, string_concPM10_0_CF1,  GDEW027W3_BLACK, GDEW027W3_WHITE, 1,1);
	
	//AQI's, pm2.5
	intToStr(pms7003.Iaq2_5,string_aqiPM2_5_CF1,1);
	intToStr(pms7003.Iaq10,string_aqiPM10_CF1,1);
	gde027w3.drawString_gfx_C( 15+PM_AQI_x_OFFSET, 88+PM_AQI_y_OFFSET, string_AQI_PM2_5,  GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
	gde027w3.drawString_gfx_n( 69+PM_AQI_x_OFFSET, 88+PM_AQI_y_OFFSET, string_AQI_PM2_5__2,  GDEW027W3_BLACK, GDEW027W3_WHITE, 1,0);
	gde027w3.drawString_gfx_C( 15+PM_AQI_x_OFFSET, 104+PM_AQI_y_OFFSET, string_AQI_PM10 ,  GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
	gde027w3.drawString_gfx_n( 74+PM_AQI_x_OFFSET, 104+PM_AQI_y_OFFSET, string_AQI_PM10__2 ,  GDEW027W3_BLACK, GDEW027W3_WHITE, 1,0);
	//values
	gde027w3.drawString_gfx_n( 135+PM_AQI_x_OFFSET, 88+PM_AQI_y_OFFSET, string_aqiPM2_5_CF1 ,  GDEW027W3_BLACK, GDEW027W3_WHITE, 1,1);
	gde027w3.drawString_gfx_n( 135+PM_AQI_x_OFFSET, 104+PM_AQI_y_OFFSET, string_aqiPM10_CF1,  GDEW027W3_BLACK, GDEW027W3_WHITE, 1, 1);
	//Category
	//gde027w3.drawString_gfx_C( 200+PM_x_OFFSET, 82+PM_y_OFFSET,pms7003.AQI_CATEGORY_PM2_5_CF1 ,  GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
	//gde027w3.drawString_gfx_C( 200+PM_x_OFFSET, 97+PM_y_OFFSET,pms7003.AQI_CATEGORY_PM10_CF1 ,  GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
	
	}
	
	
	char string_TVCO[10]={0,0,0,0,0,0,0,0,0,0};
	char string_AQI_TVCO[6]="TVCO";
	char string_TVCO_ex[3]=" :";
	
	//also draw here TVCO data
		gde027w3.drawString_gfx_n( 83+PM_AQI_x_OFFSET, 120+PM_AQI_y_OFFSET, string_TVCO_ex,  GDEW027W3_BLACK, GDEW027W3_WHITE, 1,0);
		gde027w3.drawString_gfx_C( 45+PM_AQI_x_OFFSET, 120+PM_AQI_y_OFFSET, string_AQI_TVCO,  GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
		ftoad(TVCO,string_TVCO,0);
		gde027w3.drawString_gfx_n( 135+PM_AQI_x_OFFSET, 120+PM_AQI_y_OFFSET, string_TVCO ,  GDEW027W3_BLACK, GDEW027W3_WHITE, 1,1);
	
	
	
	//char string_version[10]={0,0,0,0,0,0,0,0,0,0};
	//char string_errorCode[10]={0,0,0,0,0,0,0,0,0,0};
	//intToStr(PMS7003_Values.version,string_version,2);
	//intToStr(PMS7003_Values.errorCode,string_errorCode,2);	
	//gde027w3.drawString( 250+PM_x_OFFSET, 40+PM_y_OFFSET, string_version,  GDEW027W3_BLACK, GDEW027W3_WHITE, 1);//testing
	//gde027w3.drawString( 250+PM_x_OFFSET, 50+PM_y_OFFSET, string_errorCode,  GDEW027W3_BLACK, GDEW027W3_WHITE, 1);//testing
}

//modes are automatic,nocturnal, manual, 
void draw_MODE()
{

char string_MODE[10]={0,0,0,0,0,0,0,0,0,0};
if(mode_flag==MODE_AUTOMATIC)
{
	strncpy(string_MODE, "AUTO", 10);	
}
if(mode_flag==MODE_NIGHT)
{
	strncpy(string_MODE, "NIGHT", 10);	
}
if(mode_flag==MODE_MANUAL)
{
	strncpy(string_MODE, "MANUAL", 10);	

}

gde027w3.drawString( 120+RPM_x_OFFSET, 166+RPM_y_OFFSET, string_MODE ,  GDEW027W3_BLACK, GDEW027W3_WHITE, 1);

char string_MOTOR_PWM[10]={0,0,0,0,0,0,0,0,0,0};
	char string_PWM[10]="PWM:";
	char string_percentage='%';
	double MOTOR_PWM=FAN_PWM_VALUE; //in % 100 is REG_TCC0_PER
	ftoad(MOTOR_PWM,string_MOTOR_PWM,0);
	gde027w3.drawString( 180+RPM_x_OFFSET, 166+RPM_y_OFFSET,string_PWM,  GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
	gde027w3.drawString( 190+RPM_x_OFFSET, 166+RPM_y_OFFSET,string_MOTOR_PWM,  GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
	gde027w3.drawChar( 192+RPM_x_OFFSET, 166+RPM_y_OFFSET,string_percentage,  GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
}



void draw_RPM()
{
	MOTOR_RPM=round(motor_ping/3);
	motor_ping=0;
	char string_RPM[5]="RPM:";
	char string_RPM_value[10]={0,0,0,0,0,0,0,0,0,0};
	intToStr(MOTOR_RPM,string_RPM_value,1);
	
	gde027w3.drawString( 40+RPM_x_OFFSET, 166+RPM_y_OFFSET,string_RPM,  GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
	gde027w3.drawString( 70+RPM_x_OFFSET, 166+RPM_y_OFFSET,string_RPM_value,  GDEW027W3_BLACK, GDEW027W3_WHITE, 1);

	
}

void draw_time()
{
	//264, 176 pixels
	//intToStr(RTC_time.second,string_sec,2);
	intToStr(RTC_time.minute,string_min,2);
	intToStr(RTC_time.hour,string_hour,2);
			
	intToStr(RTC_time.day,string_day,2);
	intToStr(RTC_time.month,string_month,2);
	intToStr(RTC_time.year+2000,string_year,4);	
	
	//pixels are 5x7 xy
	gde027w3.drawChar(240, 161, '.',  GDEW027W3_BLACK,GDEW027W3_WHITE, 1);//+12
	gde027w3.drawString( 257, 161, string_min,  GDEW027W3_BLACK,GDEW027W3_WHITE, 1); //
	gde027w3.drawString( 240, 161, string_hour,  GDEW027W3_BLACK,GDEW027W3_WHITE, 1); //
	
	gde027w3.drawString( 221, 168, string_day,  GDEW027W3_BLACK,GDEW027W3_WHITE, 1);//-19
	gde027w3.drawChar(	220, 168, '.',  GDEW027W3_BLACK,GDEW027W3_WHITE, 1);//+19
	gde027w3.drawString( 239, 168, string_month,  GDEW027W3_BLACK,GDEW027W3_WHITE, 1); //-19
	gde027w3.drawChar(	238, 168, '.',  GDEW027W3_BLACK,GDEW027W3_WHITE, 1);
	gde027w3.drawString( 269, 168, string_year,  GDEW027W3_BLACK,GDEW027W3_WHITE, 1);	
}


void draw_mics_data()
{

char nh3_string_data[10]={0,0,0,0,0,0,0,0,0,0};
char nh3_string[14]="Ammonia (ppm)";
char co_string_data[10]={0,0,0,0,0,0,0,0,0,0};
char co_string[22]="Carbon monoxide";
char no2_string_data[10]={0,0,0,0,0,0,0,0,0,0};	
char no2_string[23]="Nitrogen dioxide";

//double test_nh3=0,test_NO2=0,test_CO=0;
//test_nh3=MICS6814_Values.NH3;
//test_CO=MICS6814_Values.CO;
//test_NO2=MICS6814_Values.NO2;

ftoad(MICS6814_Values.NH3,nh3_string_data,2);
ftoad(MICS6814_Values.CO,co_string_data,2);
ftoad(MICS6814_Values.NO2,no2_string_data,2);

gde027w3.drawString(243, 66, nh3_string, GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
gde027w3.drawString_gfx_n(230, 77, nh3_string_data, GDEW027W3_BLACK, GDEW027W3_WHITE, 1,1);

gde027w3.drawString(255, 98, co_string, GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
gde027w3.drawString_gfx_n(230, 111, co_string_data, GDEW027W3_BLACK, GDEW027W3_WHITE, 1,1);

gde027w3.drawString(260, 129, no2_string, GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
gde027w3.drawString_gfx_n(230, 142, no2_string_data, GDEW027W3_BLACK, GDEW027W3_WHITE, 1,1);
	
}



void mics_adc_test()
{
	
uint16_t temp_nh3_ADC, temp_co_ADC, temp_no2_ADC;
 temp_nh3_ADC=readADC(2); //muxpos input
			temp_co_ADC=readADC(3);
			temp_no2_ADC=readADC(4);

char nh3_string[10]={0,0,0,0,0,0,0,0,0,0};
char co_string[10]={0,0,0,0,0,0,0,0,0,0};
char no2_string[10]={0,0,0,0,0,0,0,0,0,0};

	intToStr(temp_nh3_ADC,nh3_string,2);
	intToStr(temp_co_ADC,co_string,2);
	intToStr(temp_no2_ADC,no2_string,2);
	
	//gde027w3.ClearFrame();
	
	gde027w3.drawString(190, 120, nh3_string, GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
	gde027w3.drawString(190, 130, co_string, GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
	gde027w3.drawString(190, 140, no2_string, GDEW027W3_BLACK, GDEW027W3_WHITE, 1);

}

