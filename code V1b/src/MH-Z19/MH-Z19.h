//ndir mh-z19 co2 sensor

//PA12 is MH-Z19 sleep/wake pin, pull low for sleep mode
//REG_PORT_OUTSET0=(1<<12); //for pin high
//REG_PORT_OUTCLR0=(1<<12); //for pin low

#define MHZ19_read_buffer_length 9

volatile uint8_t MHZ19_rx_buffer[MHZ19_read_buffer_length];
volatile uint8_t MHZ19_tx_buffer[MHZ19_read_buffer_length];
volatile uint8_t rx_data_MHZ19_index=0;
volatile uint8_t tx_data_MHZ19_index=0;
volatile uint8_t need_to_send_MHZ19=0;

volatile uint8_t MHZ19_checksum=0;
volatile uint16_t co2_concentration=0;


//command to read 0x86	
class MHZ19 {
	public:
		MHZ19();
		void init();
		void wake();
		void sleep();
		void change_to_2000ppm();
		void change_to_5000ppm();
		void request_data();
	private:	
		void reset_readings();

};

const uint8_t MHZ19_change_to_2000ppm[]={0xFF, 0x01,0x99,0x07,0xD0,0x00,0x00,0x00,0x90};//0x0171=113 checksum is 1+153+7+208=369=113->256-113=143,143+1=144=0x90
const uint8_t MHZ19_change_to_5000ppm[]={0xFF, 0x01,0x99,0x13,0x88,0x00,0x00,0x00,0xCC};//0x0135= dec= //1+153+19+136=309=53->256-53=203,203+1=204=0xCC
const uint8_t MHZ19_read_data[]=	{0xFF, 0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79};

//8 bits, 1 stop bit, no parity
//inc reading is
//byte 0: 0xff 
//byte 1: 0x86 
//byte 2: concentration high byte  
//byte 3: concentration low byte  

//bytes 4-8: dont care
//byte 9: checksum, 0xff-all bytes added together



















