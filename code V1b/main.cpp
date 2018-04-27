/*
 * Air Purifier V1b.cpp
 *
 * Created: 3.2.2018 20.44.52
 * Author : kei
 */ 

 //PID value coefficients, change accordingly
static const double P_val=1; 
static const double I_val=0.2;
static const double D_val=0.1;
//max min values for PID's integral
#define I_err_max 50
#define I_err_min -50

#define UPDATE_AQ_AVG_INTERVAL 3 //calculate new air quality averages every n seconds interval
#define EXTRA_TIME_AFTER_HEATING 5 //extra time in seconds to before starting to set motor pwm, so no high pwm start
#define STATISTIC_INTERVAL 10 //update statistics every x seconds
#define START_UP_TIME 30 //time to start in seconds from power up, heating etc. to get correct air quality values
#define FAN_PWM_VALUE_NIGHT_MAX 5 //0-99, max night setting
#define FAN_PWM_VALUE_STARTUP 10
#define FAN_PWM_MAX 40


#define HUMIDIFIER_SAMPLES 60
static const double HUMIDIFIER_SLOPE_AVERAGE=0.05; //at least this much of slope or not set to humidifier mode

//factors for cleaniness
#define PM1_0_FACTOR 1
#define PM2_5_FACTOR 2
#define PM10_FACTOR 1
#define TVCO_FACTOR 1

//all values added together and then divided to not affect P_val
double PM_FACTOR_total=(PM1_0_FACTOR+PM2_5_FACTOR+PM10_FACTOR+TVCO_FACTOR);

//target PID 
//change according to desired reference, avg of pm1_0, pm2_5, pm10 and TVOC
double target_cleaniness=5; 

volatile bool	ADC_done=false, //in ADC interrupt
				extra_values_time=false, //for extra seconds, motor pwm check EXTRA_TIME_AFTER_HEATING
				allow_display_update=true, //used when calibrating
				update_time=true; //in RTC when it's time to update
				

#include "samd21g18a.h"

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdalign.h>
#include <stdbool.h>

#include "confs.h"			
#include "functions.cpp"

#include "PMS7003.cpp"
#include "MH-Z19.cpp"
#include "i2c.c"
#include "BME280.cpp"
#include "Adafruit_BME680.cpp"

#include "GDEW027W3.cpp"
#include "MICS6814.cpp"
#include "RTC.cpp"
#include "imagedata.cpp"

//GLOBALS HERE

double			temp=0,
				pressure=0,
				humidity=0,
				TVCO=0,
				old_humidity[HUMIDIFIER_SAMPLES],
				slope_sum=0,
				humidity_avg_slope=0,
				lasts_avg=0,
				firsts_avg;


//PID values
double 			err_old=0,
				err=0,
				P_err=0,
				I_err=0,
				D_err=0,
				MOTOR_PID=0,
				current_cleaniness=0;

uint32_t	motor_ping=0,
			MOTOR_RPM=0;

uint16_t	timer_1s=0,
			timer_seconds=0,
			xminute_passed=0,
			timer_humidifier=0,
			timer_x_seconds=0,
			timer_update_screen=0,
			extra_values_timer=0,
			MANUAL_PWM=0; //0-10 strength

uint16_t FAN_PWM_VALUE=0;
			
volatile bool 	second_passed=true, //take CO2 and short avg PM measurements every second
				x_second_passed=false, //calculate new aqi's every n seconds
				LED_PWM_UPDOWN=true, //startup LEDs updown counters direction
				update_time_startup=false, //startup timers bool, false until sensors are heated up
				startup_running=true, //startup timers helper
				humidifier_sampling_finished=false; //one shot
							
//for overriding modes
bool	NIGHT_MODE=false,
		MANUAL_MODE=false,
		MOTOR_PID_CONTROL=true,
		humidifier_detected=false;
		
char string_sec[10]={0,0,0,0,0,0,0,0,0,0};
char string_min[10]={0,0,0,0,0,0,0,0,0,0};
char string_hour[10]={0,0,0,0,0,0,0,0,0,0};
char string_day[10]={0,0,0,0,0,0,0,0,0,0};
char string_month[10]={0,0,0,0,0,0,0,0,0,0};
char string_year[10]={0,0,0,0,0,0,0,0,0,0};					
//END OF GLOBALS

//volatile uint16_t temp_counter=0; //testing

//purifier modes in state
typedef enum {
	MODE_AUTOMATIC,
	MODE_NIGHT,
	MODE_MANUAL
} purifier_mode_t;
purifier_mode_t mode_flag = MODE_AUTOMATIC;

typedef enum {
	helper_MODE_AUTOMATIC,
	helper_MODE_NIGHT,
	helper_MODE_MANUAL
} helper_purifier_mode_t;
helper_purifier_mode_t helper_mode_flag =  helper_MODE_AUTOMATIC; //used to keep track of which mode was if overwritten

#include "display_functions.cpp"


int main(void)
{
	REG_PM_AHBMASK &= ~PM_AHBMASK_USB; //disable usb clock

    SystemInit();    // Initialize the SAM system 
	
	PinConfigs(); //set pins
	ClockConfig(); //setting clocks and set up 48mhz factory set values
	
	ConfigureInterrupts(); //interrupts
	PWM_FAN_init(); //fan PWM init TCC0.2
	PWM_LED_init(); //power button led init TCC2.0
	
	init_4ms_counter(); //interrupts every 4ms
	init_ADC();
	init_sercoms();
	init_RTC(); //init to mode2
	
	rtc_clock.init();
	
	pms7003.wake();
	mhz19.wake();
	//mhz19.change_to_2000ppm(); //don't need probably
	
	
	
	bme680.begin(); 
	mics6814.init(); 
	
	helper_mode_flag =  helper_MODE_AUTOMATIC;
	mode_flag = MODE_AUTOMATIC;	
	
	gde027w3.init_GDEW027W3();
	gde027w3.ClearFrame();
	
	
	
	//humidity=bme280.readHumidity();
	draw_humidity_data();
	draw_MODE();
	//mics_adc_test();
	//draw_time();
	gde027w3.DisplayFrame();
	
	for(uint16_t jj=0; jj< HUMIDIFIER_SAMPLES;jj++)
	{
		old_humidity[jj]=0;
	}

	
    while(1) 
    {
		
		
		if(second_passed)
		{
			second_passed=false;
			pms7003.AQ_calc_avg_short();

			mhz19.request_data();
			if(co2_concentration >= 1800 && co2_concentration < 4800 )
			{
				//mhz19.change_to_5000ppm();
			}
			else if (co2_concentration < 1800)
			{
				//mhz19.change_to_2000ppm();
			}
			
		}
		
		if(x_second_passed) //calculate new values that are used in motor PWM
		{
			x_second_passed=false;
			
			//add TVOC here
				//to check if there's an ultrasonic humidifier on and then force night mode on
				humidity=bme680.humidity;
					
				firsts_avg=0;
				lasts_avg=0;
				for(uint16_t i=0; i<= HUMIDIFIER_SAMPLES-2;i++)
				{
					old_humidity[i]=old_humidity[i+1]; //move values left by one
					if(i <= 4)
					{ 
						firsts_avg+=old_humidity[i];
					}
					if(i > (HUMIDIFIER_SAMPLES-6) ) lasts_avg+=old_humidity[i];
					
				}
				old_humidity[HUMIDIFIER_SAMPLES-1]=humidity;
				lasts_avg+=old_humidity[HUMIDIFIER_SAMPLES-1];
				firsts_avg/=(double)5; //5 samples
				lasts_avg/=(double)5;
				
				if(humidifier_sampling_finished)
				{	
					if(((lasts_avg-firsts_avg) > HUMIDIFIER_SLOPE_AVERAGE) && (pms7003.old_avg_PM1_0_CF1_short >= 200 || 
					pms7003.old_avg_PM2_5_CF1_short >= 200 ||pms7003.old_avg_PM10_CF1_short >= 200 )) //if humidifier is detected
					{
						mode_flag=MODE_NIGHT;
						humidifier_detected=true;
					}
					else if(helper_mode_flag == helper_MODE_AUTOMATIC )
					{	
						mode_flag=MODE_AUTOMATIC;
						humidifier_detected=false;
					}
					else if(helper_mode_flag == helper_MODE_NIGHT )
					{
						mode_flag=MODE_NIGHT;
						humidifier_detected=false;
					}
					else if(helper_mode_flag == helper_MODE_MANUAL)
					{
						mode_flag=MODE_MANUAL;
						humidifier_detected=false;
					}
				}
			
		}
		

		switch(mode_flag){
			case MODE_AUTOMATIC:
					NIGHT_MODE=false;
					MOTOR_PID_CONTROL=true;
					MANUAL_MODE=false;
			break;
			case MODE_NIGHT:
					NIGHT_MODE=true;
					MOTOR_PID_CONTROL=true;
					MANUAL_MODE=false;
			break;
			case MODE_MANUAL:
				MANUAL_MODE=true;
				MOTOR_PID_CONTROL=false;
				NIGHT_MODE=false;
				FAN_PWM_VALUE=10*MANUAL_PWM; //0-10 manual pwm
			break;
		}
		
		if(allow_display_update && update_time_startup && update_time) //every 30 seconds
		{
			update_time=false; //in RTC when it's time to update
			mics6814.read_and_calculate();
			pms7003.AQI_calc_indexes();
			while(gde027w3.update_inprogress);			
			
			//pms7003.AQI_calc_indexes();
			gde027w3.ClearFrame();
			draw_RPM();	//motor ping here
			//draw_time();	
			draw_MODE();
			draw_humidity_data();
			
			//char string_h[10]={0,0,0,0,0,0,0,0,0,0};
			//ftoan((lasts_avg-firsts_avg),string_h,2);
			//gde027w3.drawString( 120, 130 , string_h, GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
					
			if(humidifier_detected)
			{
				gde027w3.drawChar(160, 161, 'H',  GDEW027W3_BLACK,GDEW027W3_WHITE, 1);
			}
			
			draw_co2_data(); //in ppm
			draw_air_quality_data();

			draw_mics_data();
			//mics_adc_test();
			
			gde027w3.DisplayFrame(); //update whole display
			
			//mics6814.Calibrate(); //calibrate mics, copy values after to the corresponding static const's to MICS6814.h file

		}
					
    }
}


//4ms interrupt
void TC4_Handler()
{
	// Check for match interrupt
	if (TC4->COUNT8.INTFLAG.bit.MC0 == 1)
	{
		
		
		timer_1s++;	
		if(timer_1s >= 250)	{	//250*4ms=1s
			timer_seconds++; //for calculating how many seconds have passed
			timer_update_screen++;
			timer_x_seconds++;
			timer_1s=0;	
			
			second_passed=true;
			REG_RTC_READREQ=RTC_READREQ_RREQ;
			while (RTC->MODE2.STATUS.bit.SYNCBUSY == 1 );
			
			if(timer_x_seconds >= UPDATE_AQ_AVG_INTERVAL)
			{
				timer_x_seconds=0;
				x_second_passed=true;
				
				if(!humidifier_sampling_finished)
				{
					timer_humidifier++;
				}
				if(!humidifier_sampling_finished && timer_humidifier >=HUMIDIFIER_SAMPLES)
				{
					timer_humidifier=0;
					humidifier_sampling_finished=true;
				}
			}

			if(timer_update_screen >= 30) 
			{
				timer_update_screen=0;
				update_time_startup=true; //startup timers bool
			}
			
			if(startup_running && timer_seconds >= START_UP_TIME)
			{
				startup_running=false;
				update_time_startup=true; //startup timers bool
			}
			if(!extra_values_time && update_time_startup )
			{
				extra_values_timer++;
			}
			
			if(!extra_values_time && update_time_startup &&  extra_values_timer >= EXTRA_TIME_AFTER_HEATING)
			{
				extra_values_time=true;
			}
			
			
			if(timer_seconds >= 60*STATISTIC_INTERVAL)
			{
				xminute_passed=1; //xminutes has passed
				timer_seconds=0;
			}
		}
		
		TC4->COUNT8.INTFLAG.bit.MC0 = 1; //write 1 to clear
	}
}

//fan PWM interrupt routine TCC0.2, 0-99 values
void TCC0_Handler()
{
	if (TCC0->INTFLAG.bit.OVF == 1)
	{
		/*
		//testing purposes only, increases pwm on every round 
		temp_counter++;
		if(temp_counter >= 100){
			temp_counter=0;
			if( REG_TCC0_CCB2<99){
				REG_TCC0_CCB2++; //set new pwm duty
			}
			else REG_TCC0_CCB2=0; //set new pwm duty
		}*/
		
		//calculate motor PID PWM every second, after startup period and extra values calculating
		if(MOTOR_PID_CONTROL && !NIGHT_MODE && !MANUAL_MODE)
		{
			err_old=err;
			//add TVOC here
			current_cleaniness=PM1_0_FACTOR*pms7003.old_avg_PM1_0_CF1_short+PM2_5_FACTOR*pms7003.old_avg_PM2_5_CF1_short+
			PM10_FACTOR*pms7003.old_avg_PM10_CF1_short+TVCO_FACTOR*TVCO;
			current_cleaniness/=PM_FACTOR_total;
			
			//P value here
			err=round((double)current_cleaniness-(double)target_cleaniness);
			I_err+=err; //integral
			if(I_err > I_err_max){
				I_err=I_err_max;
			}
			else if(I_err < I_err_min){
				I_err=I_err_min;
			}
			P_err=err; //Proportional is directly the difference times P_val (bang bang value)
			D_err=err-err_old;
			
			MOTOR_PID=(double)P_val*P_err+(double)I_val*I_err+(double)D_val*D_err;
			if(MOTOR_PID<0){
				MOTOR_PID=0;
			}
			FAN_PWM_VALUE=(uint16_t)round(MOTOR_PID);
		}
		
		if(NIGHT_MODE && !MANUAL_MODE && FAN_PWM_VALUE>FAN_PWM_VALUE_NIGHT_MAX)
		{
			FAN_PWM_VALUE=FAN_PWM_VALUE_NIGHT_MAX;
		}
		
		if(startup_running  ) //|| !extra_values_time
		{
			FAN_PWM_VALUE=FAN_PWM_VALUE_STARTUP;
		}
		
		if(FAN_PWM_VALUE >FAN_PWM_MAX) FAN_PWM_VALUE=FAN_PWM_MAX; //clip max PWM
		if(FAN_PWM_VALUE >99) FAN_PWM_VALUE=99; //safeties
		if(FAN_PWM_VALUE <0) FAN_PWM_VALUE=0; //safeties
				
		REG_TCC0_CCB2=FAN_PWM_VALUE; //set new pwm duty
		//REG_TCC0_CCB2=0; //override
		while (TCC0->SYNCBUSY.bit.CCB2==1);
		TCC0->INTFLAG.bit.OVF = 1;
	}
}

//LED PWM interrupt routine TCC2.0
void TCC2_Handler()
{
	if (TCC2->INTFLAG.bit.OVF == 1)
	{
		if(startup_running){
			if(LED_PWM_UPDOWN && REG_TCC2_CCB0<255){
				REG_TCC2_CCB0++; //set new pwm duty
			}
			else if(LED_PWM_UPDOWN && REG_TCC2_CCB0==255)
			{
				LED_PWM_UPDOWN=false;
			}
			if(!LED_PWM_UPDOWN && REG_TCC2_CCB0>15){
				REG_TCC2_CCB0--; //set new pwm duty
			}
			else if(!LED_PWM_UPDOWN && REG_TCC2_CCB0<=15)
			{
				LED_PWM_UPDOWN=true;
			}
		}
		else if (NIGHT_MODE)
		{
			REG_TCC2_CCB0=0;
		}
		else
		{
			REG_TCC2_CCB0=ceil((double)FAN_PWM_VALUE*256/100); //calculate led pwm value based on PID RPM's
		}
		
		while (TCC2->SYNCBUSY.bit.CCB0==1);
		TCC2->INTFLAG.bit.OVF = 1;
	}
}


//external interrupt handler all interrupts in here, pb03=extint3,pb22=extint6
void EIC_Handler()
{
	if (EIC->INTFLAG.bit.EXTINT6 && EIC->INTENSET.bit.EXTINT6){
		motor_ping++; //div by 3 to get actual Rounds per x
		EIC->INTFLAG.reg = EIC_INTFLAG_EXTINT6;     
	}

	if (EIC->INTFLAG.bit.EXTINT13 && EIC->INTENSET.bit.EXTINT13){
		if(gde027w3.update_inprogress) {
			gde027w3.update_inprogress=false; 
		}
		EIC->INTFLAG.reg = EIC_INTFLAG_EXTINT13;     
	}


} 



void SERCOM0_Handler() //i2c
{
}












