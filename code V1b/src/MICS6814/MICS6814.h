

#define ADC_PRECISION 1023 //max values can only be around 1024

#define MICS6814_CALIBRATION_MAX_VARIANCE 2

static const uint16_t REFERENCE_ADC_NH3=555; //649, 630, 616,524,527
static const uint16_t REFERENCE_ADC_CO=560; //577, 549, 525, 551,556
static const uint16_t REFERENCE_ADC_NO2=892; //876, 881, 876, 860,866

struct MICS6814_framestruct {
/*
uint16_t 	nh3_ADC,
			co_ADC,
			no2_ADC,
			vref_ADC;
	*/		
double 		CO,
			NO2,
			NH3,
			C3H8,
			C4H10,
			CH4,
			H2,
			C2H5OH;

} MICS6814_Values;	
	

	

class MICS6814 {

	public:
		uint16_t Ameas_0 ;
		uint16_t Ameas_1 ;
		uint16_t Ameas_2 ;
		double ratio0, ratio1, ratio2;
	
		MICS6814();
		void init();
		void wake();
		void sleep();
		void Calibrate();
		void read_and_calculate();
			
	private:	
		void readCurrentADC();
		void calcGases();
	
};












