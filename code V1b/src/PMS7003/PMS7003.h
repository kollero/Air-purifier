//plantower pms7003 sensor
//9600bps, 32bytes, no check bit, 1 stop bit
//errors are ±10% within +10 to +40 C° 

//PA07 is pms7003 sleep/wake pin, pull low for sleep mode
//REG_PORT_OUTSET0=(1<<7); //for pin high
//REG_PORT_OUTCLR0=(1<<7); //for pin low

#define PMS7003_read_buffer_length 32

volatile uint8_t PMS7003_values_ready=1;	//global	
volatile uint8_t PMS7003_buffer[PMS7003_read_buffer_length]; //input buffer can change anytime

#define OVERSAMPLING_PMS 20 //should be close to 20


#define WRITE_PMS7003_length 7

#define Start_Char_1 0x42
#define Start_Char_2 0x4d	
#define CMD_READ 0xe2

#define PMS7003_sample_num_LONG 20
#define PMS7003_sample_num_SHORT 4

//for receiving
volatile uint8_t read_data_PMS7003_index;

uint8_t PMS7003_sampling_done;
uint8_t samples;
uint8_t	sample_done;
//for transmitting
//uint8_t request_data_PMS7003[7]={Start_Char_1,Start_Char_2,CMD_READ,0x00,0x00,0x00,0x0a};
//uint8_t request_data_PMS7003_length;
//uint8_t request_data_PMS7003_index=0;	

#define LPF_FIR_BUFFER_LENGTH 5
uint16_t LPF_FIR_BUFFER_PM1_0[LPF_FIR_BUFFER_LENGTH];
uint16_t LPF_FIR_BUFFER_PM2_5[LPF_FIR_BUFFER_LENGTH];
uint16_t LPF_FIR_BUFFER_PM10[LPF_FIR_BUFFER_LENGTH];
float BesselLowpassSecondOrder004_buffer_PM1_0[LPF_FIR_BUFFER_LENGTH];
float BesselLowpassSecondOrder004_buffer_PM2_5[LPF_FIR_BUFFER_LENGTH];
float BesselLowpassSecondOrder004_buffer_PM10[LPF_FIR_BUFFER_LENGTH];




struct PMS7003_framestruct {
uint8_t values_ok;

const uint8_t PMS7003_frameHeader[3]={0x42,0x4d,0x1c}; //0x4d
//framelength here 2 bytes	
uint16_t concPM1_0_CF1;
uint16_t concPM2_5_CF1;
uint16_t concPM10_0_CF1;
uint16_t concPM1_0_amb;
uint16_t concPM2_5_amb;
uint16_t concPM10_0_amb;
uint16_t rawGt0_3um;
uint16_t rawGt0_5um;
uint16_t rawGt1_0um;
uint16_t rawGt2_5um;
uint16_t rawGt5_0um;
uint16_t rawGt10_0um;
uint8_t version;
uint8_t errorCode;

uint16_t checksum;
} PMS7003_Values;	
	

uint32_t pms10;
uint32_t pms25;	
uint32_t pms100;	
uint32_t pms10cf;	
uint32_t pms25cf;	
uint32_t pms100cf;			

//void get_many_readings();	
	
class PMS7003 {
	
	volatile uint32_t avg_PM1_0_CF1;
	volatile double max_PM2_5_CF1;
	volatile double min_PM2_5_CF1;
	
	volatile uint32_t avg_PM2_5_CF1;

	volatile uint32_t avg_PM10_CF1;
	volatile uint16_t min_PM10_CF1;
	volatile uint16_t max_PM10_CF1;
	

	double old_max_PM2_5_CF1;
	double old_min_PM2_5_CF1;
	
	uint16_t old_min_PM10_CF1;
	uint16_t old_max_PM10_CF1;
	uint16_t aqi_index;
	
	volatile uint16_t aqi_index_short;
	volatile uint32_t avg_PM1_0_CF1_short;
	volatile uint32_t avg_PM2_5_CF1_short;
	volatile uint32_t avg_PM10_CF1_short;
	
	
	
	public:
	
		uint32_t old_avg_PM1_0_CF1_short;
		uint32_t old_avg_PM2_5_CF1_short;
		uint32_t old_avg_PM10_CF1_short;
		
	
		PMS7003();
		void init();
		void wake();
		void sleep();
		void values_calc_helper();
		void AQI_calc_indexes();
		void AQ_calc_avg_short();
		//void move_to_globals();
		
		uint32_t old_avg_PM1_0_CF1;
		uint32_t old_avg_PM2_5_CF1;
		uint32_t old_avg_PM10_CF1;
		uint16_t Iaq2_5;
		uint16_t Iaq10;
		
		char AQI_CATEGORY_PM2_5_CF1[14];
		char AQI_CATEGORY_PM10_CF1[14];
		bool pms7003_values_done;
		
		
		
	private:	
		void BesselLowpassSecondOrder004_PM1_0();
		void BesselLowpassSecondOrder004_PM2_5();
		void BesselLowpassSecondOrder004_PM10();
		bool first_time;
		
	
	
		//void reset_readings();
		//void SERCOM5_Handler();
	
};

//reading is done by: {Start_Char_1,Start_Char_2,CMD_READ,x,x,x1,x1} x=don't care bytes,
//x1=verifybytes add all bytes except verify bytes
//normal read is {Start_Char_1,Start_Char_2,CMD_READ,0x00,0x00,0x00,0x0a} 
// 0x42=2bits, 0x4d=4bits, 0xe2=4bits, total=0b00001010

// buffer is as follows
// first 2 bytes are for start characters, 0x42,0x4d
// next 2 bytes for frame length=2x13+2(data+check bytes)
// 
// rest are given in index number: (high byte,low byte)
//
// 	4, 5: pm1.0 concentration unit (μg/m^3) high and low bytes with CF=1
// 	6,7: pm2.5 concentration unit high and low bytes with CF=1
// 	8,9: pm10 concentration unit high and low bytes with CF=1
// 
// 	10,11: pm1.0 concentration unit high and low bytes under atmospheric environment
//	12,13: pm2.5 concentration unit high and low bytes under atmospheric environment
//	14,15: pm10 concentration unit high and low bytes under atmospheric environment
//	
//	16,17: number of particles above 0.3um in 0.1 liter
//	18,19: number of particles above 0.5um in 0.1 liter 
//	20,21: number of particles above 1um in 0.1 liter 

//	22,23: number of particles above 2.5um in 0.1 liter 
//	24,25: number of particles above 5um in 0.1 liter 
//	26,27: number of particles above 10um in 0.1 liter 

//	28: version number
//	29: error code

//	30,31: hash: Start character 1+ Start character 2+……..+reserved_low_bits
//	
//

//pm(low,high), corresponding AQI(low,high)
static const double AQI_INDEXES_PM2_5[][4]={
	{0,12,0,50},
	{12.1,35.4,51,100},
	{35.5,55.4,101,150},
	{55.5,150.4,151,200},
	{150.5,250.4,201,300},
	{250.5,350.4,301,400},
	{350.5,500.4,401,500}

};

//pm(low,high), corresponding AQI(low,high)
static const uint16_t AQI_INDEXES_PM10[][4]={
	{0,54,0,50},
	{55,154,51,100},
	{155,254,101,150},
	{255,354,151,200},
	{355,424,201,300},
	{425,504,301,400},
	{505,604,401,500}
};

static const char *AQI_CATEGORY[7]={
	"Good",
	"Moderate",
	"Unhealthy",	
	"Unhealthy",
	"Very Unhealthy",
	"Hazardous",
	"Hazardous"
};















