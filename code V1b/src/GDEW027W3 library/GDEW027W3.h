
//IL91874 driver
	
//display dc_command/dc_data=pa15
#define GDEW027W3_DC_DATA 		REG_PORT_OUTSET0=(1<<15) //high
#define GDEW027W3_DC_COMMAND 		REG_PORT_OUTCLR0=(1<<15) //low
//reset display pa14
#define GDEW027W3_RESET_ACTIVE 	REG_PORT_OUTCLR0=(1<<14) //low
#define GDEW027W3_RESET_NOT 		REG_PORT_OUTSET0=(1<<14) //high
//cs_display=pa16
#define  GDEW027W3_CS_HIGH	REG_PORT_OUTSET0=(1<<16) //high
#define  GDEW027W3_CS_LOW  	REG_PORT_OUTCLR0=(1<<16) //low
//busy_display(input)=pa13

//#define  GDEW027W3_BUSY	(REG_PORT_IN0&PORT_PA13)
	
#define GDEW027W3_WIDTH 264
#define GDEW027W3_HEIGHT 176

#define GDEW027W3_BUFFER_SIZE (uint32_t(GDEW027W3_WIDTH) * uint32_t(GDEW027W3_HEIGHT) / 8)


class GDEW027W3{
	
	
int 		 _width=0,
			 _height=0;
uint8_t 	rotation;			  


	 public:
		void set_rotation(uint8_t rot);
		volatile bool update_inprogress=false;
		GDEW027W3(void);
		//uint8_t GDEW027W3_buffer[GDEW027W3_HEIGHT][GDEW027W3_WIDTH]; //whole display LUT buffer
		bool NOT_BUSY();
		void drawPixel(int16_t x, int16_t y);
		void init_GDEW027W3();
		//void WAKE_GDEW027W3();
		//void UPDATE_GDEW027W3();
		void SLEEP_GDEW027W3();
		void update_screen();
		void fillBuffer(uint16_t color);
		void IL91874_sendCmd(uint8_t data);
		void ClearFrame();
		
		void show_image(const uint8_t* frame_buffer);
		
		void RefreshPartial( uint16_t x, uint16_t y, uint16_t w, uint16_t l);
		void TransmitPartialData( uint16_t x, uint16_t y, uint16_t w, uint16_t l);
		void DisplayFrame();
		
		
		//void drawPixel(int16_t x, int16_t y,uint16_t color);
		void fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
		void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color,uint16_t bg, uint8_t size);
		void drawString( int16_t x, int16_t y, char stri[], uint16_t color,uint16_t bg, uint8_t size);
		void drawFastVLine(int16_t x, int16_t y, int16_t length, uint16_t color);
		void drawFastHLine(int16_t x, int16_t y, int16_t length, uint16_t color);
		
		void drawChar_CAPI(int16_t x, int16_t y, unsigned char c, uint16_t color,uint16_t bg, uint8_t size);
		void drawChar_num(int16_t x, int16_t y, unsigned char c, uint16_t color,uint16_t bg, uint8_t size);
		void drawString_gfx_C( int16_t x, int16_t y, char stri[], uint16_t color,uint16_t bg, uint8_t size);
		
		void drawString_gfx_n( int16_t x, int16_t y, char stri[], uint16_t color,uint16_t bg, uint8_t size,uint8_t right);
		
	private:
		
		uint8_t buffer[GDEW027W3_WIDTH/8][GDEW027W3_HEIGHT]; //whole display LUT buffer xy
	
		
		static const uint8_t lut_20_vcomDC[];
		static const uint8_t lut_21_ww[];
		static const uint8_t lut_22_bw[];
		static const uint8_t lut_23_wb[];
		static const uint8_t lut_24_bb[];
		
		void SetLut(void);
		void TransactSPI(uint8_t data);
		//void IL91874_sendCmd(uint8_t data);
		void IL91874_sendData(uint8_t data);
		
};


/*
void init_GDEW027W3(void);
void setRotation(uint8_t x);
void setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void drawPixel(int16_t x, int16_t y, uint16_t color);


void drawFastVLine(int16_t x, int16_t y, int16_t length, uint16_t color);
void drawFastHLine(int16_t x, int16_t y, int16_t length, uint16_t color);
void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,uint16_t color);
 
void fillScreen(uint16_t color);

void drawChar_gfx(int16_t x, int16_t y, unsigned char c, uint16_t color,uint16_t bg, uint8_t size);
void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color,uint16_t bg, uint8_t size);
void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

void drawString( int16_t x, int16_t y, char* stri[], uint16_t color,uint16_t bg, uint8_t size); //char* stri[]
void drawString_gfx( int16_t x, int16_t y, char* stri[], uint16_t color,uint16_t bg, uint8_t size);//char* stri[]
void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);
void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) ;			  
void drawRoundRect(int16_t x, int16_t y, int16_t w,int16_t h, int16_t r, uint16_t color);
void drawCircleHelper( int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color); 
*/


#define GDEW027W3_BLACK     0x0000
#define GDEW027W3_DARKGREY  0x7BEF      // 128, 128, 128 
#define GDEW027W3_LIGHTGREY 0xC618      // 192, 192, 192 
#define GDEW027W3_WHITE     0xFFFF
#define GDEW027W3_RED    	0xF800 // 255,   0,   0 

