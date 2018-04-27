#include "RTC.h"

RTC_clock rtc_clock = RTC_clock();

RTC_clock::RTC_clock()
{
	
}

void RTC_clock::init()
{
	RTC_time.day=19;     // 1 to 31
	RTC_time.month=10;      // 1 to 12
	RTC_time.year=17;     // year-99
	RTC_time.hour=14;
	RTC_time.minute=58;
	RTC_time.second=52;
	
//read from eeprom


//save to RTC
RTC->MODE2.CLOCK.bit.SECOND=RTC_time.second;
RTC->MODE2.CLOCK.bit.MINUTE=RTC_time.minute;
RTC->MODE2.CLOCK.bit.HOUR=RTC_time.hour;
RTC->MODE2.CLOCK.bit.DAY=RTC_time.day;
RTC->MODE2.CLOCK.bit.MONTH=RTC_time.month;	
RTC->MODE2.CLOCK.bit.YEAR=RTC_time.year;

}


void RTC_clock::read_clock_register()
{

RTC_time.second=RTC->MODE2.CLOCK.bit.SECOND;//(uint8_t)0x3f&REG_RTC_MODE2_CLOCK;
RTC_time.minute=RTC->MODE2.CLOCK.bit.MINUTE;//(uint8_t)0x3f& (uint32_t)(REG_RTC_MODE2_CLOCK>>6);
RTC_time.hour=RTC->MODE2.CLOCK.bit.HOUR;//(uint8_t)0x1f& (uint32_t)(REG_RTC_MODE2_CLOCK>>12);
RTC_time.day=RTC->MODE2.CLOCK.bit.DAY;//(uint8_t)0x1f& (uint32_t)(REG_RTC_MODE2_CLOCK>>17);
RTC_time.month=RTC->MODE2.CLOCK.bit.MONTH;//(uint8_t)0x0f& (uint32_t)(REG_RTC_MODE2_CLOCK>>22);
RTC_time.year=RTC->MODE2.CLOCK.bit.YEAR;//(uint8_t)0x3f& (uint32_t)(REG_RTC_MODE2_CLOCK>>26);
}

void RTC_Handler()
{
	if(RTC->MODE2.INTFLAG.bit.SYNCRDY)
	{
		rtc_clock.read_clock_register();
		if(!gde027w3.update_inprogress &&(RTC_time.second==0 || RTC_time.second==30)){
			update_time=true;
		}
		RTC->MODE2.INTFLAG.bit.SYNCRDY=1; //clear flag
	}
}
