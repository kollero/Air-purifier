
#define DELAY_LOOP_CYCLES 3.0 // cycles per loop

#define DELAY_US_LOOPS(US) ((uint32_t)((double)(US) * F_CPU / DELAY_LOOP_CYCLES / 1000000.0))
#define DELAY_MS_LOOPS(MS) ((uint32_t)((double)(MS) * F_CPU / DELAY_LOOP_CYCLES / 1000.0))
#define DELAY_S_LOOPS (S)  ((uint32_t)((double)(S)  * F_CPU / DELAY_LOOP_CYCLES))
#define _delay_us( US ) delay_loops( DELAY_US_LOOPS(US) )
#define _delay_ms( MS ) delay_loops( DELAY_MS_LOOPS(MS) )
#define _delay_s ( S )  delay_loops( DELAY_S_LOOPS(S) )

void delay_loops(uint32_t loops) {
	asm(".syntax unified");
	asm volatile (
		"1: SUBS %[loops], %[loops], #1 \n"
		"   BNE 1b \n"
		: [loops] "+r"(loops)
	);
	asm(".syntax divided");
}


void delay_us(uint16_t us){
	for (uint16_t i=0; i < us; i++){
		_delay_us(1);
	}	
}

void delay_ms(uint16_t ms){
	for (uint16_t i=0; i < ms; i++){
		_delay_ms(1);
	}	
}

/*
//descending both values
double dsp_lookup(double (*table)[2], double x, int16_t array_size){
	int16_t i=0;
	double m=0.0;	
	while(i < array_size){  
		//find the point where adc value is bigger than table value for the first time
		if ( x == table[i][0] ){   //make sure the point isn't past the end of the table
			return table[i][1];//no need for further calculations %
		}
		if(x > table[i][0]){
			break;//found it, remember that the pointer is actually -1 to the value
		}
		i++;
	}
	if ( i == array_size ){   //make sure the point isn't past the end of the table
		return table[array_size-1][1];
	}
	if ( i == 0 ){  //make sure the point isn't before the beginning of the table
		return (double)table[i][1]; //return first value
	}
	//slope k=(y2-y1)/(x2-x1)
	m =(double) (table[i-1][1]-table[i][1]) / ( table[i-1][0] - table[i][0]); //keep slope positive
	//y=kx+b	
	return (double) round((m * (x - table[i][0]))+ table[i][1]); //this is the solution to the point slope formula	
}
*/
//to get adc results 
uint16_t readADC(uint8_t muxpos) 
{
	ADC_done=false;
	//REG_ADC_SWTRIG=ADC_SWTRIG_FLUSH;
	//set mux from input
	//REG_ADC_INPUTCTRL= (uint32_t)ADC_INPUTCTRL_MUXNEG_GND|ADC_INPUTCTRL_GAIN_DIV2|muxpos;
	
	//REG_ADC_AVGCTRL= ADC_AVGCTRL_SAMPLENUM_16|ADC_AVGCTRL_ADJRES(4); //
	REG_ADC_INPUTCTRL= (uint32_t)muxpos|ADC_INPUTCTRL_MUXNEG_IOGND;//negative reference 	
	while (ADC->STATUS.bit.SYNCBUSY==1);			
	REG_ADC_SWTRIG=ADC_SWTRIG_START; //starts a conversion
	//while (ADC->INTFLAG.bit.RESRDY == 0);
	//ADC->INTFLAG.bit.RESRDY = 1;
	while(!ADC_done); //wait till adc is done
	/*
if(ADC->RESULT.reg >= 0x8000 )
{
	 return (0xffff-ADC->RESULT.reg);	
}*/
  return ADC->RESULT.reg;
}

void ADC_Handler()
{
	if(ADC->INTFLAG.bit.RESRDY==1)
	{
		ADC_done=true;
		ADC->INTFLAG.bit.RESRDY=1; //clear flags
	}
}

/*
//0 for battery adc, and 1 for charging current
double read_battery_value(void)
{
	uint16_t battery_ADC;
	double battery_voltage, bat_percent;
	
	battery_ADC=readADC(0);
	battery_voltage=(double)BATTERY_VOLTAGE_DIVISION*battery_ADC;
		
	//bat_percent=dsp_lookup(lipo_ADC_to_percentages, battery_voltage, LIPO_ARRAY_LENGTH);
	
	return bat_percent;
}


double read_charging_current(void)
{
	uint16_t battery_ADC;
	double battery_char_cur;
	
	battery_ADC=readADC(1);
	battery_char_cur=(double)BATTERY_CHARGING_CURRENT_DIVISION*battery_ADC;

	return battery_char_cur;
}
*/

// reverses a string 'str' of length 'len'
void reverse(char *str, int len){
	int i=0, j=len-1, temp;
	while (i<j){
		temp = str[i];
		str[i] = str[j];
		str[j] = temp;
		i++; j--;
	}
}
// Converts a given integer x to string str[].  d is the number
// of digits required in output. If d is more than the number
// of digits in x, then 0s are added at the beginning.
int intToStr(int x, char str[], int d) {

	int i = 0;
	while (x) {
		str[i++] = (x%10) + '0';
		x = x/10;
	}
	// If number of digits required is more, then
	// add 0s at the beginning
	while (i < d)
	str[i++] = '0';
	reverse(str, i);
	str[i] = '\0';
	
	
	return i;
}
// Converts a floating point number to string.
void ftoa(float n, char *res, int afterpoint) {
	// Extract integer part
	int ipart = (int)n;
	// Extract floating part
	float fpart =(float) (n - (float)ipart);
	// convert integer part to string
	//int i = intToStr(ipart, res, 0);
	int i = intToStr(ipart, res, 1); //forces to show 0 at the beginning
	// check for display option after point
	if (afterpoint != 0){
		res[i] = '.';  // add dot
		// Get the value of fraction part upto given no.
		// of points after dot. The third parameter is needed
		// to handle cases like 233.007
		fpart = fpart * pow(10, afterpoint);
		intToStr((int)fpart, res + i + 1, afterpoint);
	}
}
// rounds the floating part
void ftoad(double n, char *res, int afterpoint)
{
	if(afterpoint != 0)
	{
		n=(double)round((double)n*(double)pow((double)10,(double)afterpoint));
		n/=(double)pow((double)10,(double)afterpoint);
	}
	else n=round(n);
	// Extract integer part
	int  ipart = (int)n;
	
	// Extract floating part
	double fpart =(double) ((double)(n) - (double)ipart);
	
	// convert integer part to string
	//int i = intToStr(ipart, res, 0);
	uint16_t  i = intToStr(ipart, res, 1); //forces to show 0 at the beginning

	// check for display option after point
	if (afterpoint != 0)
	{
		res[i] = '.';  // add dot
		
		//fpart=round(10*fpart);
		// Get the value of fraction part upto given no.
		// of points after dot. The third parameter is needed
		// to handle cases like 233.007
		fpart = fpart* pow(10, afterpoint);
		if(fpart==10)fpart=1;
		intToStr((int)fpart, res + i + 1, afterpoint);
	}
}

// Converts a floating point number to string, works with negative values
void ftoan(double n, char *res, int afterpoint){
	
	uint8_t negative=0;
		
	if(n<0){ 
		n=-n;
		res[0]='-';
		negative=1;
	}
	// Extract integer part
	int16_t ipart = (int16_t)n;
	// Extract floating part
	double fpart =(double) (n - (double)ipart);
	
	int i = intToStr(ipart, res+negative, 1); //forces to show 0 at the beginning

	// check for display option after point
	if (afterpoint != 0  )
	{
		res[i+negative] = '.';  // add dot
		
		// Get the value of fraction part upto given no.
		// of points after dot. The third parameter is needed
		// to handle cases like 233.007
		fpart = fpart * pow(10, afterpoint);
		
		intToStr((int16_t)fpart, res + i + 1+negative, afterpoint);
	}
}



