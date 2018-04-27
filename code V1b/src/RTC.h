

struct RTC_framestruct{
	
uint8_t second;
uint8_t minute;
uint8_t hour;
uint8_t day;
uint8_t month;
uint8_t year;	
	
}RTC_time;	

	
class RTC_clock {
	
	public:
		RTC_clock();
		void init();
		
		void read_clock_register();

};






