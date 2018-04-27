/************************************************************************************

GDEW027W3

e-paper waveshare display, black and white, 4-wire SPI, 264x176 pixels, 2.7inch
24pin, 0.5mm PP connector, refresh rate 6s,

IL91874 driver


************************************************************************************/
//avr-gcc libary to work on atmel studio
#include "GDEW027W3.h"
#include "GDEW027W3_data_functions.c"

//fonts
#include "glcdfont.c"
#include "MS_Sans_Serif10x18.h" //for numbers and small letters
#include "MS_Sans_Serif11x18.h" //for capital letters



const uint8_t GDEW027W3::lut_20_vcomDC[] =
{
0x00  ,0x00 ,
0x00  ,0x0F ,0x0F ,0x00 ,0x00 ,0x05,    
0x00  ,0x32 ,0x32 ,0x00 ,0x00 ,0x02,    
0x00  ,0x0F ,0x0F ,0x00 ,0x00 ,0x05,    
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,    
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,    
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,    
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
};
//R21H
const uint8_t GDEW027W3::lut_21_ww[] =
{
0x50  ,0x0F ,0x0F ,0x00 ,0x00 ,0x05,
0x60  ,0x32 ,0x32 ,0x00 ,0x00 ,0x02,
0xA0  ,0x0F ,0x0F ,0x00 ,0x00 ,0x05,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
};
//R22H  r
const uint8_t GDEW027W3::lut_22_bw[] =
{
0x50  ,0x0F ,0x0F ,0x00 ,0x00 ,0x05,
0x60  ,0x32 ,0x32 ,0x00 ,0x00 ,0x02,
0xA0  ,0x0F ,0x0F ,0x00 ,0x00 ,0x05,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
};
//R23H  w
const uint8_t GDEW027W3::lut_23_wb[] =
{
0xA0  ,0x0F ,0x0F ,0x00 ,0x00 ,0x05,
0x60  ,0x32 ,0x32 ,0x00 ,0x00 ,0x02,
0x50  ,0x0F ,0x0F ,0x00 ,0x00 ,0x05,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
};
//R24H  b
const uint8_t GDEW027W3::lut_24_bb[] =
{
0xA0  ,0x0F ,0x0F ,0x00 ,0x00 ,0x05,
0x60  ,0x32 ,0x32 ,0x00 ,0x00 ,0x02,
0x50  ,0x0F ,0x0F ,0x00 ,0x00 ,0x05,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
};


GDEW027W3 gde027w3 = GDEW027W3();

GDEW027W3::GDEW027W3() 
{
}

bool GDEW027W3::NOT_BUSY() {
	if(update_inprogress){
		return false;
	}
	return true;
}
void GDEW027W3::set_rotation(uint8_t rot) {
	rotation=rot;
}	


void GDEW027W3::init_GDEW027W3(void) {
	
	_width    = GDEW027W3_WIDTH;
	_height   = GDEW027W3_HEIGHT;
	set_rotation(3);
//4 wire spi, with busy, reset, DC, CS

	
	GDEW027W3_CS_LOW;
	GDEW027W3_RESET_ACTIVE;
	delay_ms(100);
	GDEW027W3_RESET_NOT; 
	delay_ms(100);
	GDEW027W3_CS_HIGH;


//power settings register
IL91874_sendCmd(0x01);
IL91874_sendData(0x03);
IL91874_sendData(0x00);
IL91874_sendData(0x2b);
IL91874_sendData(0x2b);
IL91874_sendData(0x09);

	//booster soft start
	IL91874_sendCmd(0x06);
	IL91874_sendData(0x07);
	IL91874_sendData(0x07);
	IL91874_sendData(0x17); //17 org, 04 in datasheet

	//power optimization
	IL91874_sendCmd(0xF8);
	IL91874_sendData(0x60); 	
	IL91874_sendData(0xa5);
	IL91874_sendCmd(0xF8);
	IL91874_sendData(0x89); 	
	IL91874_sendData(0xa5);
	IL91874_sendCmd(0xF8);
	IL91874_sendData(0x90); 	
	IL91874_sendData(0x00);
	IL91874_sendCmd(0xF8);
	IL91874_sendData(0x93); 	
	IL91874_sendData(0x2a);
	IL91874_sendCmd(0xF8);
	IL91874_sendData(0xa0); 	
	IL91874_sendData(0xa5);
	IL91874_sendCmd(0xF8);
	IL91874_sendData(0xa1); 	
	IL91874_sendData(0x00);
	IL91874_sendCmd(0xF8);
	IL91874_sendData(0x73);	
	IL91874_sendData(0x41);

	//partial display refresh
	IL91874_sendCmd(0x16); 	
	IL91874_sendData(0x00);
	
	//power on
	IL91874_sendCmd(0x04);
	update_inprogress=true;
	while(update_inprogress);//wait till busy rises, shouldn't take long
	
	//panel settings
	IL91874_sendCmd(0x00);
	//IL91874_sendData(0xbf); //black and white, LUT from register
	//IL91874_sendData(0x1f); //black and white, by OTP LUT, 320x240
	IL91874_sendData(0xaf); //KW-BF   KWR-AF    BWROTP 0f

	//pll control
	IL91874_sendCmd(0x30);
	IL91874_sendData(0x3a);
	
	//VCM_DC_SETTING_REGISTER
	IL91874_sendCmd(0x82);
	IL91874_sendData(0x12);
	
	delay_ms(2);
	SetLut();
}	

//set LUTS
void GDEW027W3::SetLut(void) {
uint16_t count=0;
    IL91874_sendCmd(0x20);							//vcom
    for (count = 0; count < 44; count++)
    {
      IL91874_sendData(lut_20_vcomDC[count]);
    }

    IL91874_sendCmd(0x21);							//white to white lut
    for (count = 0; count < 42; count++)
    {
      IL91874_sendData(lut_21_ww[count]);
    }

    IL91874_sendCmd(0x22);							//black to white lut
    for (count = 0; count < 42; count++)
    {
      IL91874_sendData(lut_22_bw[count]);
    }

    IL91874_sendCmd(0x23);							//white to black lut
    for (count = 0; count < 42; count++)
    {
      IL91874_sendData(lut_23_wb[count]);
    }

    IL91874_sendCmd(0x24);							//black to black lut
    for (count = 0; count < 42; count++)
    {
      IL91874_sendData(lut_24_bb[count]);
    }
}
	
//power save mode have to reset and re-initialize
void GDEW027W3::SLEEP_GDEW027W3(void) {
	//deep sleep
	IL91874_sendCmd(0x07);
	IL91874_sendData(0xa5);
}

//updates and refreshes the whole display
void GDEW027W3::update_screen(void)
{
	IL91874_sendCmd(0x10); //
	delay_ms(2);
	for (uint32_t i = 0; i < GDEW027W3_BUFFER_SIZE; i++)
	{
		IL91874_sendData(0xFF); //white
	}
	
	delay_ms(2);
	IL91874_sendCmd(0x13); //
	delay_ms(2);
	for (uint32_t j = 0; j < GDEW027W3_HEIGHT; j++)
	{
		for (uint32_t i = 0; i < GDEW027W3_WIDTH/8; i++)
		{
			IL91874_sendData(buffer[i][j]);
		}
	}
	delay_ms(2);
	IL91874_sendCmd(0x12); //display refresh/update command
	//delay_ms(10);
	update_inprogress=true; 
}

//clears the SRAM and buffer
void GDEW027W3::ClearFrame(void) {
	IL91874_sendCmd(0x10);
	delay_ms(2);
	for (uint32_t i = 0; i < GDEW027W3_BUFFER_SIZE; i++)
	{
		IL91874_sendData(0xFF); //white
	}
	delay_ms(2);
	IL91874_sendCmd(0x13);
	delay_ms(2);
	for (uint32_t i = 0; i < GDEW027W3_BUFFER_SIZE; i++)
	{
		IL91874_sendData(0xFF); //white
	}
	delay_ms(2);
}

//fills buffer, not screen
void GDEW027W3::fillBuffer(uint16_t color)
{
	uint8_t data = (color == GDEW027W3_BLACK) ?  0x00:0xFF ; //black is 0x0000, white 0xFFFF
	
	for (uint32_t j = 0; j < GDEW027W3_HEIGHT; j++)
	{
		for (uint32_t i = 0; i < GDEW027W3_WIDTH/8; i++)
		{
			buffer[i][j]=data;
		}
	}
}

//updates and refreshes the whole display
void GDEW027W3::show_image(const uint8_t* frame_buffer)
{
	IL91874_sendCmd(0x10); //
	delay_ms(2);
	for (uint32_t i = 0; i < GDEW027W3_BUFFER_SIZE; i++)
	{
		IL91874_sendData(0xFF); //white
	}
	
	delay_ms(2);
	IL91874_sendCmd(0x13); //
	delay_ms(2);
	
	for (uint32_t i = 0; i < GDEW027W3_BUFFER_SIZE; i++)
	{
		IL91874_sendData(frame_buffer[i]);
	}
	
	delay_ms(2);
	IL91874_sendCmd(0x12); //display refresh/update command
	//delay_ms(10);
	update_inprogress=true;
}


//partial functions
//refreshes partial area of display
void GDEW027W3::RefreshPartial(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
	
	 uint16_t h_hel=w;
   uint16_t x_hel=y;
	switch(rotation)
  {
    case 1:
     // swap(x, y);
      x = _width - x - 1;
      break;
	case 2:
     // swap(x, y);
      y = _height - y - 1;
      break;  
    case 3:
		y=x;
		x=x_hel;
		//x = _height - x - 1;
		y = _width - y - 1;
	  w=h;
	  h=h_hel;
      break;
 }
	uint8_t add=0;
	uint16_t w_helper=w%8;
	if(w_helper>0) add=8;
	
    IL91874_sendCmd(0x16); 
    IL91874_sendData(x >> 8);
    IL91874_sendData(x & 0xf8);     // x should be the multiple of 8, the last 3 bit will always be ignored
    IL91874_sendData(y >> 8);        
    IL91874_sendData(y & 0xff);
    IL91874_sendData((w+add) >> 8);
    IL91874_sendData((w+add) & 0xf8);     // w (width) should be the multiple of 8, the last 3 bit will always be ignored
    IL91874_sendData(h >> 8);        
    IL91874_sendData(h & 0xff);
   
    update_inprogress=true;
}

//transmit partial data to SRAM
void GDEW027W3::TransmitPartialData( uint16_t x, uint16_t y, uint16_t w, uint16_t l) {
   
        IL91874_sendCmd(0x15);
        IL91874_sendData(x >> 8);
        IL91874_sendData(x & 0xf8);     // x should be the multiple of 8, the last 3 bit will always be ignored
        IL91874_sendData(y >> 8);        
        IL91874_sendData(y & 0xff);
        IL91874_sendData(w >> 8);
        IL91874_sendData(w & 0xff);     // w (width) should be the multiple of 8, the last 3 bit will always be ignored
        IL91874_sendData(l >> 8);        
        IL91874_sendData(l & 0xff);
        delay_ms(2);	
	for (int16_t j = y; j < w; j++)
	{
		for (int16_t i = x/8; i < 8 * l; i++)
		{
			
			IL91874_sendData(buffer[i][j]);
		}
	}
	delay_ms(2);
}

//update whole display
void GDEW027W3::DisplayFrame(void) {
   IL91874_sendCmd(0x12); 
   update_inprogress=true;
}

 

void GDEW027W3::fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
  
  uint8_t data = (color == GDEW027W3_BLACK) ? 0x00 : 0xFF; //black is 0x0000, white 0xFFFF
  
 
  uint16_t h_hel=w;
   uint16_t x_hel=y;
  switch(rotation)
  {
    case 1:
     // swap(x, y);
      x = _width - x - 1;
      break;
	case 2:
     // swap(x, y);
      y = _height - y - 1;
      break;  
    case 3:
		y=x;
		x=x_hel;
		//x = _height - x - 1;
		y = _width - y - 1;
	  w=h;
	  h=h_hel;
      break;
 }
 uint8_t add=0;
	uint16_t h_helper=w%8;
	if(h_helper>0) add=8;
	
	uint8_t sifter = (color == GDEW027W3_BLACK) ? (0xff>>h_helper): (~h_helper);
	
	IL91874_sendCmd(0x15);
	IL91874_sendData((x >> 8)&0x01);
    IL91874_sendData(x & 0xf8);     // x should be the multiple of 8, the last 3 bit will always be ignored
    IL91874_sendData(y >> 8 );        
    IL91874_sendData(y & 0xff);  //y everything is there
	IL91874_sendData(((w+add) >> 8)&0x01);
    IL91874_sendData((w+add) & 0xf8);    
	IL91874_sendData(h >> 8);        
    IL91874_sendData(h & 0xff);
    delay_ms(2); 
	
	for(uint16_t j = 0; j < h; j++) {
		//IL91874_sendData(data);
		
		for(uint16_t i = 0; i < w/8 ;i++) {	
			IL91874_sendData(data);
		}
		if(h_helper>0)
		{
			IL91874_sendData(sifter);	
		}
	}
	delay_ms(2); 
	
}


// Draw a character
void GDEW027W3::drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color,uint16_t bg, uint8_t size) 
{
/*
//basic 5x7 font
if((x >= _width)            || // Clip right
       (y >= _height)           || // Clip bottom
       ((x + 6 * size - 1) < 0) || // Clip left
       ((y + 8 * size - 1) < 0))   // Clip top
      return;
*/
    if( c >= 176) c++; // Handle 'classic' charset behavior

	uint16_t h=5*size; 	uint16_t w=8*size;
	//uint16_t h_helper=w%8;
	/*if(h_helper>0)
	{
		w+=8;
	}*/
	
	  //uint16_t h_hel=w;
   uint16_t x_hel=y;
	if(size==1)
	{
  switch(rotation)
  {
    case 1:
     // swap(x, y);
      x = _width - x - 1;
      break;
	case 2:
     // swap(x, y);
      y = _height - y - 1;
      break;  
    case 3:
		y=x;
		x=x_hel;
		//x = _height - x - 1;
		y = _width - y - 1;
	 // w=h;
	  //h=h_hel;
      break;
 }	
	
	IL91874_sendCmd(0x15);
	IL91874_sendData((x >> 8)&0x01);
	IL91874_sendData(x & 0xf8);     // x should be the multiple of 8, the last 3 bit will always be ignored
	IL91874_sendData(y >> 8 );
	IL91874_sendData(y & 0xff);  //y everything is there
	IL91874_sendData((w >> 8)&0x01);
	IL91874_sendData(w & 0xf8);
	IL91874_sendData(h >> 8);
	IL91874_sendData(h & 0xff);
	delay_ms(2);
	}
  
    for(int8_t i=5; i>=0; i-- ) {
      uint8_t line;uint8_t line2;
      if(i < 5) line =  font[(c*5)+i]; //take line by line
      else      line = 0x0;
	   
	     line2=0;
	     for(uint8_t i=0;i< 8 ;i++){
		     line2 |= ((line>>i) & 1)<<(7-i);
	     }
		 line2=~line2;
	   if(size == 1)
	   {
		IL91874_sendData(line2);
	   }
	   else
	   {
      for(int8_t j=0; j<8; j++, line >>= 1) //right shift by one
	   {
        if(line & 0x1){ //check if 1 or 0
          if(size >= 2 && size < 8 )
		  {		
			//fillRect(x+(i*size), y+(j*size), size, size, color);
			//drawPixel(x+i, y+j, color);
		  }
		  else   
		  {       
		  fillRect(x+(i*size), y+(j*size), size, size, color);
		  }
        } 
		}
	  }
    }
	delay_ms(2);
}

void GDEW027W3::drawString( int16_t x, int16_t y, char stri[], uint16_t color,uint16_t bg, uint8_t size)
{
	char k;
	uint8_t len =strlen(stri);
	for(int16_t i= len;i>0;i--)
	{	
		k=stri[i-1];
		drawChar(x-(len*size*6)+((i-1)*(size)*6), y, k, color,bg, size); //
	} 
	
	
	/*
	uint8_t i=0;
	char k;
	while(i < strlen(stri)){
		k=stri[i];
		drawChar(x+(i*(size)*6), y, k, color,bg, size);
		i++;
	}*/
	
}

void GDEW027W3::drawChar_CAPI(int16_t x, int16_t y, unsigned char c, uint16_t color,uint16_t bg, uint8_t size) {
	//gfx font
	c=c-0x20; //offset
	
	int16_t  w  = 18,
	h  = 11,
	line_width=34;
	
	int16_t x_hel=y;
	
	int8_t h_helper=w%8;
	if(h_helper >0){
		w+=8-h_helper;
	}
	
	int16_t calc_helper=ceil(w/8);
	//int16_t h_hel=0;
	switch(rotation)
	{
		case 1:
		// swap(x, y);
		x = _width - x - 1;
		break;
		case 2:
		// swap(x, y);
		y = _height - y - 1;
		break;
		case 3:
		y=x;
		x=x_hel;
		//x = _height - x - 1;
		y = _width - y - 1;
		//w=h;
		//h=h_hel;
		break;
	}
	
	// x+=xo;
	// y+=yo;
	IL91874_sendCmd(0x15);
	IL91874_sendData((x >> 8)&0x01);
	IL91874_sendData(x & 0xf8);     // x should be the multiple of 8, the last 3 bit will always be ignored
	IL91874_sendData(y >> 8 );
	IL91874_sendData(y & 0xff);  //y everything is there
	IL91874_sendData((w >> 8)&0x01);
	IL91874_sendData(w & 0xf8);
	IL91874_sendData(h >> 8);
	IL91874_sendData(h & 0xff);
	delay_ms(2);

	uint8_t line=0;
	uint8_t line2=0;

	for(int16_t yy=h-1; yy>=0; yy--) {
		for(int16_t j=1; j<=calc_helper; j++)
		{
			line =MS_Sans_Serif11x18[c*line_width+calc_helper*yy+j];
			line2=0;
			for(uint8_t i=0;i< 8 ;i++){
				line2 |= ((line>>i) & 1)<<(7-i);
			}
			line2=~line2;
			IL91874_sendData(line2);
		}
	}
	
	delay_ms(2);
}

void GDEW027W3::drawChar_num(int16_t x, int16_t y,unsigned char c, uint16_t color,uint16_t bg, uint8_t size) {
	//gfx font
	c=c-0x20; //offset 
 	
    int16_t  w  = 18,
             h  = 10,
			 line_width=25;
            
     int16_t x_hel=y;
   
   int8_t h_helper=w%8;
   if(h_helper >0){
	   w+=8-h_helper;
   }
   
   int16_t calc_helper=ceil(w/8);
	//int16_t h_hel=0;
  switch(rotation)
  {
    case 1:
     // swap(x, y);
      x = _width - x - 1;
      break;
	case 2:
     // swap(x, y);
      y = _height - y - 1;
      break;  
    case 3:
		y=x;
		x=x_hel;
		//x = _height - x - 1;
		y = _width - y - 1;
	  //w=h;
	  //h=h_hel;
      break;
 }	
// x+=xo;
// y+=yo;
	IL91874_sendCmd(0x15);
	IL91874_sendData((x >> 8)&0x01);
	IL91874_sendData(x & 0xf8);     // x should be the multiple of 8, the last 3 bit will always be ignored
	IL91874_sendData(y >> 8 );
	IL91874_sendData(y & 0xff);  //y everything is there
	IL91874_sendData((w >> 8)&0x01);
	IL91874_sendData(w & 0xf8);
	IL91874_sendData(h >> 8);
	IL91874_sendData(h & 0xff);
	delay_ms(2);

uint8_t line=0;
uint8_t line2=0;

	for(int16_t yy=h-3; yy>=0; yy--) {
		for(int16_t j=1; j<=calc_helper; j++) 
		{
			line = MS_Sans_Serif8x18[c*line_width+calc_helper*yy+j];
			line2=0;
			for(uint8_t i=0;i< 8 ;i++){
				 line2 |= ((line>>i) & 1)<<(7-i);
			}
			line2=~line2;
			IL91874_sendData(line2);
		}
    }
	 
  delay_ms(2);
}

void GDEW027W3::drawString_gfx_C( int16_t x, int16_t y, char stri[], uint16_t color,uint16_t bg, uint8_t size)
{	
	uint8_t ii=0;
	char kk;
	while(ii < strlen(stri)){
		kk=stri[ii];
		drawChar_CAPI(x+(ii*(size)*11), y, kk, color,bg, size);
		ii++;
	}
}

void GDEW027W3::drawString_gfx_n( int16_t x, int16_t y,char stri[], uint16_t color,uint16_t bg, uint8_t size, uint8_t right)
{
	
	if(right==1) //right shifted values
	{
	char k;
	uint8_t frac=0;
	uint8_t len =strlen(stri);
	for(int16_t i= len;i>0;i--)
	{
		k=stri[i-1];
		if(i >0 && stri[i-1]=='.') //if '.'
		{
			frac=4;
			drawChar(x-(len*size*10)+((i-1)*(size)*10), y+5, k, color,bg, size);
		}
		else drawChar_num(x-(len*size*10)+((i-1)*(size)*10+frac), y, k, color,bg, size); 
	}
	
	}
	else //left shifted values
	{
		

	
	uint8_t ii=0;
	uint8_t frac=0;
	unsigned char kk;
	while(ii < strlen(stri)){
		kk=stri[ii];
		
		if(ii>0 && stri[ii-1]==46) //if '.'
		{
			frac=5;
		}
		
		drawChar_num(x+(ii*(size)*10-frac), y, kk, color,bg, size);
		ii++;
	}
	
}
}



/*
void GDEW027W3::drawChar_gfx(int16_t x, int16_t y, unsigned char c, uint16_t color,uint16_t bg, uint8_t size) {
	//gfx font
	c=c-0x20; //offset 
 	 
    int16_t  w  =22,
             h  =21; 

   int16_t x_hel=y;
   
   int8_t h_helper=w%8;
   if(h_helper >0){
	   w+=8-h_helper;
   }
   
   int16_t calc_helper=ceil(w/8);
	//int16_t h_hel=0;
  switch(rotation)
  {
    case 1:
     // swap(x, y);
      x = _width - x - 1;
      break;
	case 2:
     // swap(x, y);
      y = _height - y - 1;
      break;  
    case 3:
		y=x;
		x=x_hel;
		//x = _height - x - 1;
		y = _width - y - 1;
	  //w=h;
	  //h=h_hel;
      break;
 }	
 
	IL91874_sendCmd(0x15);
	IL91874_sendData((x >> 8)&0x01);
	IL91874_sendData(x & 0xf8);     // x should be the multiple of 8, the last 3 bit will always be ignored
	IL91874_sendData(y >> 8 );
	IL91874_sendData(y & 0xff);  //y everything is there
	IL91874_sendData((w >> 8)&0x01);
	IL91874_sendData(w & 0xf8);
	IL91874_sendData(h >> 8);
	IL91874_sendData(h & 0xff);
	delay_ms(2);


uint8_t line =0;	
uint8_t line2=0;

	for(int16_t yy=h-1; yy>0; yy--) {
		for(int16_t j=1; j<=calc_helper; j++) 
		{
			line =  MS_Sans_Serif21x22[(c*64)+calc_helper*yy+j];
			line2=0;
			for(uint8_t i=0;i< 8 ;i++){
				 line2 |= ((line>>i) & 1)<<(7-i);
			}
			line2=~line2;
			IL91874_sendData(line2);
		}
    }
	 
  delay_ms(2);
}


void GDEW027W3::drawString_gfx( int16_t x, int16_t y, char stri[], uint16_t color,uint16_t bg, uint8_t size){
	
	
	uint8_t ii=0;
	char kk;
	while(ii < strlen(stri)){
	kk=stri[ii];
	drawChar_gfx(x+(ii*(size)*12), y, kk, color,bg, size);
	ii++;
	} 
}
*/

void GDEW027W3::drawFastVLine(int16_t x, int16_t y, int16_t length, uint16_t color) {
fillRect(x, y, 1, length, color);
}

void GDEW027W3::drawFastHLine(int16_t x, int16_t y, int16_t length, uint16_t color) {
   fillRect(x, y, length, 1, color);
}




/*
void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,uint16_t color) {
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep != 0) {
	int16_t tmpvar=0;
	tmpvar = x0;
	x0=y0;
	y0=tmpvar;
	
	tmpvar=0;	
	tmpvar = x1;
	x1=y1;
	y1=tmpvar;	
 
  }

  if (x0 > x1) {
	int16_t tmpvar2=0;
	
	tmpvar2 = x0;
	x0=x1;
	x1=tmpvar2;
	
	tmpvar2=0;	
	tmpvar2 = y0;
	y0=y1;
	y1=tmpvar2;	

  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0<=x1; x0++) {
    if (steep) {
      drawPixel(y0, x0, color);
    } else {
      drawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

void fillScreen(uint16_t color) {
	fillRect(0, 0, _width , _height , color);
   //setAddrWindow(0, 0, _width, _height);
   //int32_t len = (int32_t)_width * _height;
   // for (uint32_t t=len; t>0; t--){
   //     IL91874_sendData16(color);
	//}

    //setAddrWindow(0, 0, _width - 1, _height - 1);
	//flood(color, (long)_width * (long)_height);
}



*/



/*




void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
  drawFastVLine(x0, y0-r, 2*r+1, color);
  fillCircleHelper(x0, y0, r, 3, 0, color);
}

void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color) {

  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;

    if (cornername & 0x1) {
      drawFastVLine(x0+x, y0-y, 2*y+1+delta, color);
      drawFastVLine(x0+y, y0-x, 2*x+1+delta, color);
    }
    if (cornername & 0x2) {
      drawFastVLine(x0-x, y0-y, 2*y+1+delta, color);
      drawFastVLine(x0-y, y0-x, 2*x+1+delta, color);
    }
  }
}

void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  drawFastHLine(x, y, w, color);
  drawFastHLine(x, y+h-1, w, color);
  drawFastVLine(x, y, h, color);
  drawFastVLine(x+w-1, y, h, color);
}

// Draw a rounded rectangle
void drawRoundRect(int16_t x, int16_t y, int16_t w,int16_t h, int16_t r, uint16_t color) {
  // smarter version
  drawFastHLine(x+r  , y    , w-2*r, color); // Top
  drawFastHLine(x+r  , y+h-1, w-2*r, color); // Bottom
  drawFastVLine(x    , y+r  , h-2*r, color); // Left
  drawFastVLine(x+w-1, y+r  , h-2*r, color); // Right
  // draw four corners
  drawCircleHelper(x+r    , y+r    , r, 1, color);
  drawCircleHelper(x+w-r-1, y+r    , r, 2, color);
  drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
  drawCircleHelper(x+r    , y+h-r-1, r, 8, color);
}

void drawCircleHelper( int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color) {
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;
    if (cornername & 0x4) {
      drawPixel(x0 + x, y0 + y, color);
      drawPixel(x0 + y, y0 + x, color);
    }
    if (cornername & 0x2) {
      drawPixel(x0 + x, y0 - y, color);
      drawPixel(x0 + y, y0 - x, color);
    }
    if (cornername & 0x8) {
      drawPixel(x0 - y, y0 + x, color);
      drawPixel(x0 - x, y0 + y, color);
    }
    if (cornername & 0x1) {
      drawPixel(x0 - y, y0 - x, color);
      drawPixel(x0 - x, y0 - y, color);
    }
  }
}
*/

