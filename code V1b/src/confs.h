

void PinConfigs()
{
	//For pin PA24 and PA25, the GPIO pull-up and pull-down must be disabled
	//before enabling alternative functions on them.
	
	//port A pin outs, GPIO's
	REG_PORT_DIRSET0=(1<<6)|(1<<7)|(1<<12)|(1<<14)|(1<<15)|(1<<16)|(1<<27);

	//lower 16 bits
	
	//inputs pa13
	//PORT->Group[0].WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG | PORT_WRCONFIG_INEN | PORT_WRCONFIG_PINMASK((1<<13));
	//pa13,  A=0, external interrupt
	PORT->Group[0].WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG |PORT_WRCONFIG_WRPMUX| PORT_WRCONFIG_PMUX(0)|PORT_WRCONFIG_PMUXEN|
									PORT_WRCONFIG_PINMASK((1<<13));
	//pa00 pwm output,TCC2, E=4,  (1<<0)
	PORT->Group[0].WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG |PORT_WRCONFIG_WRPMUX| PORT_WRCONFIG_PMUX(4)|PORT_WRCONFIG_PMUXEN| PORT_WRCONFIG_PINMASK(1);
	//(pa08,pa09 sercom0),  C=2
	PORT->Group[0].WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG |PORT_WRCONFIG_WRPMUX| PORT_WRCONFIG_PMUX(2)|PORT_WRCONFIG_PMUXEN|
									PORT_WRCONFIG_PINMASK((1<<8)|(1<<9));
	//(pa10,pa11 sercom 2), D=3								
	PORT->Group[0].WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG |PORT_WRCONFIG_WRPMUX| PORT_WRCONFIG_PMUX(3)|PORT_WRCONFIG_PMUXEN|
									PORT_WRCONFIG_PINMASK((1<<10)|(1<<11));
		
	//higher 16 bits
	//pa18 pwm output,TCC0, F=5, 
	PORT->Group[0].WRCONFIG.reg = PORT_WRCONFIG_HWSEL|PORT_WRCONFIG_WRPINCFG |PORT_WRCONFIG_WRPMUX| PORT_WRCONFIG_PMUX(5)|PORT_WRCONFIG_PMUXEN| 
									PORT_WRCONFIG_PINMASK(1<<(18-16));
	//(pa17,pa19 sercom1),(pa22,pa23 sercom3)  C=2
	PORT->Group[0].WRCONFIG.reg = PORT_WRCONFIG_HWSEL| PORT_WRCONFIG_WRPINCFG |PORT_WRCONFIG_WRPMUX| PORT_WRCONFIG_PMUX(2)|PORT_WRCONFIG_PMUXEN|
									PORT_WRCONFIG_PINMASK((1<<(17-16))|(1<<(19-16))|(1<<(22-16))|(1<<(23-16)));								
	//(pa20,pa21 sercom3) D=3								
	PORT->Group[0].WRCONFIG.reg = PORT_WRCONFIG_HWSEL|PORT_WRCONFIG_WRPINCFG |PORT_WRCONFIG_WRPMUX| PORT_WRCONFIG_PMUX(3)|PORT_WRCONFIG_PMUXEN|
									PORT_WRCONFIG_PINMASK((1<<(20-16))|(1<<(21-16)));															
	//(pa24,pa25 USB), G=6								
	PORT->Group[0].WRCONFIG.reg = PORT_WRCONFIG_HWSEL|PORT_WRCONFIG_WRPINCFG |PORT_WRCONFIG_WRPMUX| PORT_WRCONFIG_PMUX(6)|PORT_WRCONFIG_PMUXEN|
									PORT_WRCONFIG_PINMASK((1<<(24-16))|(1<<(25-16)));
	//port B pin outs, GPIO's
	//outs: 2,23
	REG_PORT_DIRSET1=(1<<2)|(1<<23);
	
	//REG_PORT_OUTSET1=(1<<2); //output high
	
	//lower 16 bits
	//(pb10,pb11 sercom 4) D=3	
	PORT->Group[1].WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG |PORT_WRCONFIG_WRPMUX| PORT_WRCONFIG_PMUX(3)|PORT_WRCONFIG_PMUXEN|
									PORT_WRCONFIG_PINMASK((1<<10)|(1<<11));
	//pb03,  A=0, external interrupt
	//PORT->Group[1].WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG |PORT_WRCONFIG_WRPMUX| PORT_WRCONFIG_PMUX(0)|PORT_WRCONFIG_PMUXEN|
	//								PORT_WRCONFIG_PINMASK((1<<3));
	//higher 16 bits								
	//pb22,  A=0, external interrupt
	PORT->Group[1].WRCONFIG.reg = PORT_WRCONFIG_HWSEL| PORT_WRCONFIG_WRPINCFG |PORT_WRCONFIG_WRPMUX| PORT_WRCONFIG_PMUX(0)|PORT_WRCONFIG_PMUXEN|
									PORT_WRCONFIG_PINMASK((1<<(22-16)));																							
}

void ClockConfig()
{
	//ix. set up OSC32K
	SYSCTRL->OSC32K.bit.EN32K = 1;
	SYSCTRL->OSC32K.bit.CALIB = ((*(uint32_t *) FUSES_OSC32K_CAL_ADDR >> FUSES_OSC32K_CAL_Pos));
	SYSCTRL->OSC32K.bit.ENABLE = 1;
	
	//x. set up dfll48m.
	SYSCTRL->OSC8M.bit.PRESC=0; //no 8 division
	
	//setting up dfll48m
	NVMCTRL->CTRLB.bit.RWS = 2; // Set Flash Wait States to 1 for 3.3V operation
	SYSCTRL->DFLLCTRL.reg = 0;	// See Errata 9905, set dfll ondemand to 0 before configuring rest of dfll
	while ((SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRDY) == 0);	// Wait for DFLL synchronization complete

	SYSCTRL->DFLLMUL.reg = SYSCTRL_DFLLMUL_MUL(48000);
	SYSCTRL->DFLLVAL.reg =  SYSCTRL_DFLLVAL_COARSE((*(uint32_t *) FUSES_DFLL48M_COARSE_CAL_ADDR>>FUSES_DFLL48M_COARSE_CAL_Pos))|
							SYSCTRL_DFLLVAL_FINE((*(uint64_t *) FUSES_DFLL48M_FINE_CAL_ADDR >> FUSES_DFLL48M_FINE_CAL_Pos ));
	
	
	SYSCTRL->DFLLCTRL.reg = SYSCTRL_DFLLCTRL_ENABLE |SYSCTRL_DFLLCTRL_BPLCKC |SYSCTRL_DFLLCTRL_CCDIS |SYSCTRL_DFLLCTRL_STABLE;
	while ((SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRDY) == 0);	// Wait for DFLL synchronization complete					
	
	//SYSCTRL->OSC8M.reg |= osc8m_calib_offset<<16;
	
	//clocks will be: (samd21 has 9 clock generators in total)
	//1. clock 0 will be for system, display spi (sercom 1),		8 division bits max
	//2. clock 1 will be for WDT(watchdog),					16 division bits max
	//3. clock 2 will be for i2c, 8mhz (sercom 0)					5 division bits max=31
	//4. clock 3 will be for RTC, div by 32, 1khz						8 division bits max
	//5. clock 4 will be 400khz,  uart(sercom 2,3,4)				8 division bits max
	//6. clock 5 will be 200khz, target 1khz with BLCD PWM			8 division bits max
	//7. clock 6 will be 100khz, power button in front PWM				8 division bits max
	//8. clock 7 will be 2mhz, EIC, TC4								8 division bits max
	//9. clock 8 will be 2.66mhz, ADC
	
	// 1. Configure the Generic Clock 0 to take the DFLL48M, without division 
	GCLK->GENDIV.reg = GCLK_GENDIV_ID(0) | GCLK_GENDIV_DIV (1);
	while ( GCLK->STATUS.bit.SYNCBUSY == 1);	  
    GCLK->GENCTRL.reg =  GCLK_GENCTRL_IDC |GCLK_GENCTRL_ID(0) |GCLK_GENCTRL_GENEN|GCLK_GENCTRL_SRC_DFLL48M ;              
    while ( GCLK->STATUS.bit.SYNCBUSY == 1 );
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_SERCOM1_CORE| GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_CLKEN; 
    while ( GCLK->STATUS.bit.SYNCBUSY == 1 );
	
	// 2. Configure the Generic Clock 1 to take the OSC32K //max div is 2^16
	GCLK->GENDIV.reg = GCLK_GENDIV_ID(1) | GCLK_GENDIV_DIV (1);
	while ( GCLK->STATUS.bit.SYNCBUSY == 1);
    GCLK->GENCTRL.reg =  GCLK_GENCTRL_IDC |GCLK_GENCTRL_ID(1) | GCLK_GENCTRL_GENEN| GCLK_GENCTRL_SRC_OSC32K;
    while ( GCLK->STATUS.bit.SYNCBUSY == 1 );
	GCLK->CLKCTRL.reg =  GCLK_CLKCTRL_ID_WDT|  GCLK_CLKCTRL_GEN_GCLK1 | GCLK_CLKCTRL_CLKEN;
	while ( GCLK->STATUS.bit.SYNCBUSY == 1 );
	//GCLK->CLKCTRL.reg =  GCLK_CLKCTRL_ID_EVSYS_0|  GCLK_CLKCTRL_GEN_GCLK1 | GCLK_CLKCTRL_CLKEN;
	//while ( GCLK->STATUS.bit.SYNCBUSY == 1 );
	
	// 3. Configure the Generic Clock 2 to take the OSC8M, 8mhz out //max div is 31?!?
	GCLK->GENDIV.reg = GCLK_GENDIV_ID(2) | GCLK_GENDIV_DIV (1);
	while ( GCLK->STATUS.bit.SYNCBUSY == 1);
	GCLK->GENCTRL.reg =  GCLK_GENCTRL_ID(2) | GCLK_GENCTRL_GENEN| GCLK_GENCTRL_SRC_OSC8M; //source clk
    while ( GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY );
	GCLK->CLKCTRL.reg =  GCLK_CLKCTRL_ID_SERCOM0_CORE| GCLK_CLKCTRL_GEN_GCLK2 | GCLK_CLKCTRL_CLKEN;
    while ( GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY );
	
	// 4. Configure the Generic Clock 3 to take the OSC32K, div by 32,
	GCLK->GENDIV.reg =   GCLK_GENDIV_ID(3) | GCLK_GENDIV_DIV(32);
	while ( GCLK->STATUS.bit.SYNCBUSY == 1);
	GCLK->GENCTRL.reg =  GCLK_GENCTRL_ID(3) | GCLK_GENCTRL_GENEN| GCLK_GENCTRL_SRC_OSC32K;
	while ( GCLK->STATUS.bit.SYNCBUSY == 1 );
	GCLK->CLKCTRL.reg =  GCLK_CLKCTRL_ID_RTC|  GCLK_CLKCTRL_GEN_GCLK3 | GCLK_CLKCTRL_CLKEN;
	while ( GCLK->STATUS.bit.SYNCBUSY == 1 );
	
	// 5. Configure the Generic Clock 4 to take the OSC8M, div by 20
	GCLK->GENDIV.reg =   GCLK_GENDIV_ID(4) |GCLK_GENDIV_DIV(20);  //Div by 20, 400khz //max div is 256
    while ( GCLK->STATUS.bit.SYNCBUSY == 1);	  
	GCLK->GENCTRL.reg =  GCLK_GENCTRL_IDC |GCLK_GENCTRL_ID(4) |GCLK_GENCTRL_GENEN|GCLK_GENCTRL_SRC_OSC8M;
    while ( GCLK->STATUS.bit.SYNCBUSY == 1 );
	GCLK->CLKCTRL.reg =  GCLK_CLKCTRL_ID_SERCOM2_CORE| GCLK_CLKCTRL_GEN_GCLK4 | GCLK_CLKCTRL_CLKEN;
	while ( GCLK->STATUS.bit.SYNCBUSY == 1 );
	GCLK->CLKCTRL.reg =  GCLK_CLKCTRL_ID_SERCOM3_CORE| GCLK_CLKCTRL_GEN_GCLK4 | GCLK_CLKCTRL_CLKEN;
	while ( GCLK->STATUS.bit.SYNCBUSY == 1 );
	GCLK->CLKCTRL.reg =  GCLK_CLKCTRL_ID_SERCOM4_CORE| GCLK_CLKCTRL_GEN_GCLK4 | GCLK_CLKCTRL_CLKEN;
	while ( GCLK->STATUS.bit.SYNCBUSY == 1 );
		

	// 6. Configure the Generic Clock 5 to take the OSC8M, div by 40
	GCLK->GENDIV.reg =   GCLK_GENDIV_ID(5) |GCLK_GENDIV_DIV(40);  //Div by 40, 200khz
    while ( GCLK->STATUS.bit.SYNCBUSY == 1);	  
	GCLK->GENCTRL.reg =  GCLK_GENCTRL_IDC |GCLK_GENCTRL_ID(5) |GCLK_GENCTRL_GENEN|GCLK_GENCTRL_SRC_OSC8M;
    while ( GCLK->STATUS.bit.SYNCBUSY == 1 );	
	GCLK->CLKCTRL.reg =  GCLK_CLKCTRL_ID_TCC0_TCC1| GCLK_CLKCTRL_GEN_GCLK5 | GCLK_CLKCTRL_CLKEN;
	while ( GCLK->STATUS.bit.SYNCBUSY == 1 );
	
	// 7. Configure the Generic Clock 6 to take the GCLK_GENCTRL_SRC_OSC8M, div by 80
	GCLK->GENDIV.reg =   GCLK_GENDIV_ID(6) |GCLK_GENDIV_DIV(80);  
    while ( GCLK->STATUS.bit.SYNCBUSY == 1);	  
	GCLK->GENCTRL.reg =  GCLK_GENCTRL_IDC |GCLK_GENCTRL_ID(6) |GCLK_GENCTRL_GENEN|GCLK_GENCTRL_SRC_OSC8M;
    while ( GCLK->STATUS.bit.SYNCBUSY == 1 );	
	GCLK->CLKCTRL.reg =  GCLK_CLKCTRL_ID_TCC2_TC3| GCLK_CLKCTRL_GEN_GCLK6 | GCLK_CLKCTRL_CLKEN;
	while ( GCLK->STATUS.bit.SYNCBUSY == 1 );
	
	// 8. Configure the Generic Clock 7 to take the OSC8M, div by 4
	GCLK->GENDIV.reg =   GCLK_GENDIV_ID(7) | GCLK_GENDIV_DIV(4);  // Div by 4
    while ( GCLK->STATUS.bit.SYNCBUSY == 1);
	GCLK->GENCTRL.reg =  GCLK_GENCTRL_IDC |	GCLK_GENCTRL_ID(7) | GCLK_GENCTRL_GENEN|GCLK_GENCTRL_SRC_OSC8M;
    while ( GCLK->STATUS.bit.SYNCBUSY == 1 );
	//GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_ADC| GCLK_CLKCTRL_GEN_GCLK7 | GCLK_CLKCTRL_CLKEN;       
   // while (GCLK->STATUS.bit.SYNCBUSY == 1 );
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_TC4_TC5|GCLK_CLKCTRL_GEN_GCLK7 | GCLK_CLKCTRL_CLKEN;
	while (GCLK->STATUS.bit.SYNCBUSY == 1 );
	GCLK->CLKCTRL.reg =  GCLK_CLKCTRL_ID_EIC|  GCLK_CLKCTRL_GEN_GCLK7 | GCLK_CLKCTRL_CLKEN; 
	while ( GCLK->STATUS.bit.SYNCBUSY == 1 );
	
	// 9. Configure the Generic Clock 8 to take the OSC8M, div by 3
	GCLK->GENDIV.reg =   GCLK_GENDIV_ID(8) | GCLK_GENDIV_DIV(3);  // Div by 3
    while ( GCLK->STATUS.bit.SYNCBUSY == 1);
	GCLK->GENCTRL.reg =  GCLK_GENCTRL_IDC |	GCLK_GENCTRL_ID(8) | GCLK_GENCTRL_GENEN|GCLK_GENCTRL_SRC_OSC8M;
    while ( GCLK->STATUS.bit.SYNCBUSY == 1 );
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_ADC| GCLK_CLKCTRL_GEN_GCLK8 | GCLK_CLKCTRL_CLKEN;       
    while (GCLK->STATUS.bit.SYNCBUSY == 1 );
	 
}	
 
void PWM_FAN_init()
{
//Generic Clock 4, 4Mhz 
// 1. Enable TCC0 in power manager
//  2. Configure Counter
//  3. Enable Counter Interrupt (TCC0)

// 1. Enable TCC0 in power manager
 REG_PM_APBCMASK |= PM_APBCMASK_TCC0;
// 2. Configure Counter
//REG_TCC0_WAVE |= TCC_WAVE_WAVEGEN_NPWM;//|TCC_WAVE_POL(0xF); 
  REG_TCC0_WAVE |= TCC_WAVE_WAVEGEN_DSTOP|TCC_WAVE_POL(0xF);       // Setup dual slope PWM on TCC0, interrupt at top, update on bot
  while (TCC0->SYNCBUSY.bit.WAVE==1);               // Wait for synchronization 
  // Each timer counts up to a maximum or TOP value set by the PER register,
  // this determines the frequency of the PWM operation: 
  REG_TCC0_PER = 99;         // @, 4mhz/2*100=20khz 
  while(TCC0->SYNCBUSY.bit.PER==1);                // Wait for synchronization
  // Set the PWM signal to output
  REG_TCC0_CC2 = 0;        					//pa18 wo[2]
  while(TCC0->SYNCBUSY.bit.CC2==1);                // Wait for synchronization
  
//3. Enable Counter Interrupt (TCC0)
  //enable interrupts
  TCC0->INTENSET.reg = 0;                 // disable all interrupts
  TCC0->INTENSET.bit.OVF = 1;          // enable overflow
  TCC0->INTFLAG.bit.OVF = 1;

  //enable output
  REG_TCC0_CTRLA |=  TCC_CTRLA_ENABLE;             // Enable the TCC0 output
  while (TCC0->SYNCBUSY.bit.ENABLE==1);              // Wait for synchronization
  NVIC_EnableIRQ(TCC0_IRQn);
}	

void PWM_LED_init()
{
//Generic Clock 6, 8khz 
// 1. Enable TCC2 in power manager
//  2. Configure Counter
//  3. Enable Counter Interrupt (TCC2)

// 1. Enable TCC2 in power manager
 REG_PM_APBCMASK |= PM_APBCMASK_TCC2;
// 2. Configure Counter
//REG_TCC2_WAVE |= TCC_WAVE_WAVEGEN_NPWM;//|TCC_WAVE_POL(0xF); 
  REG_TCC2_WAVE |= TCC_WAVE_WAVEGEN_DSTOP|TCC_WAVE_POL(0xF);       // Setup dual slope PWM on TCC2, interrupt at top, update on bot
  while (TCC2->SYNCBUSY.bit.WAVE==1);               // Wait for synchronization 
  // Each timer counts up to a maximum or TOP value set by the PER register,
  // this determines the frequency of the PWM operation: 
  REG_TCC2_PER = 255;         //@50hz, 8khz/2*80=50hz
  while(TCC2->SYNCBUSY.bit.PER==1);                // Wait for synchronization
  // Set the PWM signal to output
  REG_TCC2_CC0 = 0;        					//pa00 wo[0]
  while(TCC2->SYNCBUSY.bit.CC0==1);                // Wait for synchronization
  
//3. Enable Counter Interrupt (TCC2)
  //enable interrupts
  TCC2->INTENSET.reg = 0;                 // disable all interrupts
  TCC2->INTENSET.bit.OVF = 1;          // enable overflow
  TCC2->INTFLAG.bit.OVF = 1;

  //enable output
  REG_TCC2_CTRLA |=  TCC_CTRLA_ENABLE;             // Enable the TCC2 output
  while (TCC2->SYNCBUSY.bit.ENABLE==1);              // Wait for synchronization
  NVIC_EnableIRQ(TCC2_IRQn);
}

void init_4ms_counter()
{
	//DO NOT USE EVSYS WITH TC's, 
//Generic Clock 7
// 1. Enable TC4 in power manager
//  2. Configure Counter
//  3. Enable Counter Interrupt (TC4)
//  4. Enable Counter

// 1. Enable TC4 in power manager
	REG_PM_APBCMASK |= PM_APBCMASK_TC4;
	TC4->COUNT8.CTRLA.reg &= ~TC_CTRLA_ENABLE;
	
	//using TC4 as 4ms timer
//  2. Configure Counter	
	TC4->COUNT8.CTRLA.reg |= TC_CTRLA_MODE_COUNT8;  // Set Timer counter Mode to 8 bits
	while (TC4->COUNT8.STATUS.bit.SYNCBUSY == 1); // wait for sync
	TC4->COUNT8.CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ; // Set TC as normal match Frq
	while (TC4->COUNT8.STATUS.bit.SYNCBUSY == 1); // wait for sync
	
	TC4->COUNT8.CC[0].reg =124; //125	//2mhz/125/64=4ms
	while (TC4->COUNT8.STATUS.bit.SYNCBUSY == 1);
	
//  3. Enable Counter Interrupt (TC4)	
	 TC4->COUNT8.INTENSET.reg = 0;              // disable all interrupts
	 TC4->COUNT8.INTENSET.bit.MC0 = 1;
	
//  4. Enable Counter	
	TC4->COUNT8.CTRLA.reg |=  TC_CTRLA_PRESCALER_DIV64 |TC_CTRLA_RUNSTDBY|TC_CTRLA_ENABLE;       // Set the counter to 8-bit mode
	// Divide the counter: 2mhz/64/125=4ms
	while (TC4->COUNT8.STATUS.bit.SYNCBUSY == 1);
	NVIC_EnableIRQ(TC4_IRQn);
	
}

void evsys_configuration()
{
	/*REG_PM_APBCMASK |= PM_APBCMASK_EVSYS;
	
	EVSYS->CTRL.reg = EVSYS_CTRL_SWRST;
	EVSYS->CHANNEL.reg = EVSYS_CHANNEL_PATH_ASYNCHRONOUS | EVSYS_CHANNEL_EVGEN(EVSYS_ID_GEN_EIC_EXTINT_3) | EVSYS_CHANNEL_CHANNEL(0); 
	EVSYS->USER.reg = (uint16_t) (EVSYS_USER_USER(EVSYS_ID_USER_TC1_EVU) | EVSYS_USER_CHANNEL(0));
	EVSYS->INTENSET.bit.EVD0 = 1;
	*/
}

void ConfigureInterrupts()
{
	
	//3. EIC Configuration:
	//a. Enable the EIC.
	//b. Configure the EIC to perform edge (rising or falling) detection or level (high or low) detection.
	//c. Enable the External Interrupts so that it is able to cause an interrupt event.
	
	// Enable EIC clock in power manager
	PM->APBAMASK.reg |= PM_APBAMASK_EIC;
	
	//extint3=falling, ccs811
	//extint6=fall, logic inverted, actually rising, motor rpm ping
	REG_EIC_CONFIG0=EIC_CONFIG_SENSE3_FALL|EIC_CONFIG_SENSE6_FALL; //3
	REG_EIC_CONFIG0|=EIC_CONFIG_FILTEN6;
	//extint13=rise, display update finised
	REG_EIC_CONFIG1=EIC_CONFIG_SENSE5_RISE|EIC_CONFIG_FILTEN5; //13,
	
	REG_EIC_CTRL |= EIC_CTRL_ENABLE; //read and enable
	
	//clear all flags here!!
	REG_EIC_INTFLAG=0;
	
	//external interrupts on 3,6,13
	REG_EIC_INTENSET=(1<<3)|(1<<6)|(1<<13);
	
	//4. Enable EIC function in ARM NVIC Interrupt Controller
	while(EIC->STATUS.bit.SYNCBUSY & EIC_STATUS_SYNCBUSY); //wait till syncbusy clears
	NVIC_EnableIRQ( EIC_IRQn );
		
}


void init_ADC()
{
	REG_PM_APBCMASK |= PM_APBCMASK_ADC;
	
	// Load in the fixed device ADC calibration constants 
	ADC->CALIB.reg = ADC_CALIB_BIAS_CAL((*(uint32_t *) ADC_FUSES_BIASCAL_ADDR >> ADC_FUSES_BIASCAL_Pos)) |
					 ADC_CALIB_LINEARITY_CAL((*(uint64_t *) ADC_FUSES_LINEARITY_0_ADDR >> ADC_FUSES_LINEARITY_0_Pos));
	
	//target is 250khz
	//from 2mhz clock
	
	//adc pins 2,3,4 in use
	
	REG_ADC_REFCTRL = ADC_REFCTRL_REFSEL_INTVCC0; //vccana=3.3v/1.48
	REG_ADC_AVGCTRL= ADC_AVGCTRL_SAMPLENUM_256|ADC_AVGCTRL_ADJRES(6); // right shift by 5,take 256 samples, right shifts automatically by 4
	REG_ADC_SAMPCTRL = ADC_SAMPCTRL_SAMPLEN(1);// Sampling time, (sampleEN+1)(clkadc/2);
	while (ADC->STATUS.bit.SYNCBUSY==1);				
	//single ended mode, 
	REG_ADC_CTRLB= ADC_CTRLB_PRESCALER_DIV8|ADC_CTRLB_RESSEL_16BIT;//|ADC_CTRLB_DIFFMODE;//|ADC_CTRLB_CORREN;//divide 2.6667mhz clock by 8 to get 333khz
	while (ADC->STATUS.bit.SYNCBUSY==1);
	REG_ADC_INPUTCTRL=  2|ADC_INPUTCTRL_MUXNEG_IOGND;//|ADC_INPUTCTRL_GAIN_DIV2; //pins2-5 in use, not set yet
	while (ADC->STATUS.bit.SYNCBUSY==1);
	
	
	//interrupt flags clearing
	REG_ADC_INTFLAG=0xf;
    //interrupt enable

	REG_ADC_INTENSET=ADC_INTENSET_RESRDY;
	
	
	//ADC->INTENSET.bit.RESRDY=1; //set interrupt when result ready
	ADC->INTFLAG.bit.RESRDY=1; //clear flag
	
	ADC->DBGCTRL.bit.DBGRUN=1; //run while debugging
	ADC->CTRLA.bit.ENABLE=1;
	
	while (ADC->STATUS.bit.SYNCBUSY==1);
	NVIC_EnableIRQ(ADC_IRQn);
	
}


void init_sercoms()
{
		
	//1. sercom0 init, i2c
	//2. sercom1 init, display spi
	//3. sercom2 init, uart pms7003
	//4. sercom3 init, uart bluetooth
	//5. sercom4 init, uart co2

	
	REG_PM_APBCMASK |=PM_APBCMASK_SERCOM0|PM_APBCMASK_SERCOM1|PM_APBCMASK_SERCOM2|PM_APBCMASK_SERCOM3|PM_APBCMASK_SERCOM4;
	
//1. sercom0 init, i2c, 400khz
	REG_SERCOM0_I2CM_CTRLA=SERCOM_I2CM_CTRLA_SPEED(0x0)| //fast mode up to 400khz
							SERCOM_I2CM_CTRLA_SDAHOLD(0x2)| // hold time
							//SERCOM_I2CM_CTRLA_RUNSTDBY|
							SERCOM_I2CM_CTRLA_MODE_I2C_MASTER; //0x5 to mode
							
	REG_SERCOM0_I2CM_CTRLB=SERCOM_I2CM_CTRLB_SMEN;//smart mode on, sends ack on read						
	while(SERCOM0->I2CM.SYNCBUSY.bit.SYSOP);						
							
	SERCOM0->I2CM.BAUD.reg=SERCOM_I2CM_BAUD_BAUD(5);//set baud to ~400khz, 8mhz/(10+2*5+0.04)						
	while(SERCOM0->I2CM.SYNCBUSY.bit.SYSOP);						
	REG_SERCOM0_I2CM_CTRLA|=SERCOM_I2CM_CTRLA_ENABLE;
	while((SERCOM0->I2CM.SYNCBUSY.reg&SERCOM_I2CM_SYNCBUSY_ENABLE));					
	SERCOM0->I2CM.STATUS.bit.BUSSTATE=0x1; //force to idle mode from unknown state						
	while(SERCOM0->I2CM.SYNCBUSY.bit.SYSOP);
	
	SERCOM0->I2CM.INTENSET.reg=SERCOM_I2CM_INTENSET_MB| SERCOM_I2CM_INTENSET_SB;
	//NVIC_EnableIRQ(SERCOM0_IRQn);		
	
	
//2. display spi 
	REG_SERCOM1_SPI_CTRLA=SERCOM_SPI_CTRLA_DIPO(0x0)|
							SERCOM_SPI_CTRLA_DOPO(0x2)| 
							SERCOM_SPI_CTRLA_MODE_SPI_MASTER; 
	while(SERCOM1->SPI.SYNCBUSY.bit.CTRLB);    
	SERCOM1->SPI.BAUD.reg = 6;   //48mhz/(2*baudreg+1) 
	
	SERCOM1->SPI.CTRLA.reg |= SERCOM_SPI_CTRLA_ENABLE;
    while(SERCOM1->SPI.SYNCBUSY.reg & SERCOM_SPI_SYNCBUSY_ENABLE);
	
//3. sercom2 init, uart pms7003,9600bauds
	// msb first,tx pad0, rx pad1, 16x oversampling, arithmetic baud rate gen
	//Generic Clock 4 is used 
	SERCOM2->USART.CTRLA.reg=
				SERCOM_USART_CTRLA_RXPO(3)|SERCOM_USART_CTRLA_TXPO(1)|SERCOM_USART_CTRLA_SAMPR(0)|
				SERCOM_USART_CTRLA_MODE_USART_INT_CLK|SERCOM_USART_CTRLA_FORM(0)|SERCOM_USART_CTRLA_DORD; //lsb first!!
	
	//S=16(samples),fref=400khz
	//(1-S*FBAUD/fREF)*65536=BAUD
	//BAUD=40370.176;
	SERCOM2->USART.BAUD.reg=40370; //for 9600bauds
	//8bits, enable receiver
	SERCOM2->USART.CTRLB.reg=	SERCOM_USART_CTRLB_CHSIZE(0x0)|SERCOM_USART_CTRLB_RXEN;//|SERCOM_USART_CTRLB_TXEN; //for transmitter
	while(SERCOM2->USART.SYNCBUSY.bit.CTRLB);
	
	SERCOM2->USART.INTENSET.reg=SERCOM_USART_INTFLAG_RXC; //receive interrupt
	SERCOM2->USART.INTFLAG.reg=0xf;
	
	NVIC_EnableIRQ(SERCOM2_IRQn);
	
	SERCOM2->USART.CTRLA.reg|=SERCOM_USART_CTRLA_ENABLE;
	while(SERCOM2->USART.SYNCBUSY.reg&SERCOM_USART_SYNCBUSY_ENABLE);	

	
//4. sercom3 init, uart bluetooth, 4 wire
	
//5. sercom4 init, uart co2,
	// msb first,	tx pad 2, rx pad3, 16x oversampling
	//Generic Clock 4 is used with, 400khz clk
	SERCOM4->USART.CTRLA.reg=	SERCOM_USART_CTRLA_RXPO(3)|SERCOM_USART_CTRLA_TXPO(1)|SERCOM_USART_CTRLA_SAMPR(0)|
								 SERCOM_USART_CTRLA_MODE_USART_INT_CLK|SERCOM_USART_CTRLA_DORD; //lsb first!!
	//FBAUD= ( fREF/ S) (1 –BAUD/65,536)
	//fREF=400khz, S=16,Fbaud=9600
	//FBAUD*( S/fREF)=(1–(BAUD/65536))
	//(1-FBAUD*S/fREF)*65536=BAUD
	//BAUD=40370.176;
	SERCOM4->USART.BAUD.reg=40370; //9600
	//8bits, enable receiver
	SERCOM4->USART.CTRLB.reg=SERCOM_USART_CTRLB_CHSIZE(0x0)|SERCOM_USART_CTRLB_RXEN| SERCOM_USART_CTRLB_TXEN;
	while(SERCOM4->USART.SYNCBUSY.bit.CTRLB);
	//receive interrupt,data register empty flag
	SERCOM4->USART.INTENSET.reg=SERCOM_USART_INTFLAG_RXC| SERCOM_USART_INTFLAG_TXC;
	//SERCOM4->USART.INTFLAG.reg=SERCOM_USART_INTFLAG_RXC| SERCOM_USART_INTFLAG_TXC;
	NVIC_EnableIRQ(SERCOM4_IRQn);
	
	SERCOM4->USART.CTRLA.reg|=SERCOM_USART_CTRLA_ENABLE;
	while(SERCOM4->USART.SYNCBUSY.reg&SERCOM_USART_SYNCBUSY_ENABLE);
	

	
}


void init_RTC()
{
	REG_PM_APBAMASK |= PM_APBAMASK_RTC;
	
	RTC->MODE2.CTRL.reg = RTC_MODE2_CTRL_SWRST; //reset
	while (RTC->MODE2.STATUS.bit.SYNCBUSY ||  RTC->MODE2.CTRL.bit.SWRST);
		
	RTC->MODE2.INTENSET.reg = RTC_MODE2_INTENSET_SYNCRDY;
	//RTC->MODE2.INTFLAG.bit.SYNCRDY=1; //clear flag
	
	//REG_RTC_MODE2_ALARM_MASK0= RTC_MODE2_MASK_SEL_YYMMDDHHMMSS;	
	REG_RTC_READREQ=RTC_READREQ_RREQ; //RTC_READREQ_RCONT;//| 
	//RTC->MODE2.READREQ.reg = RTC_READREQ_RCONT;//|RTC_READREQ_RREQ;  // continuous read sync
	while (RTC->MODE2.STATUS.bit.SYNCBUSY == 1 );
	RTC->MODE2.CTRL.reg =RTC_MODE2_CTRL_MODE_CLOCK| RTC_MODE2_CTRL_PRESCALER_DIV1024 | RTC_MODE2_CTRL_ENABLE; //div by 1024
	while (RTC->MODE2.STATUS.bit.SYNCBUSY == 1 );

	while (RTC->MODE2.STATUS.bit.SYNCBUSY == 1 );
	NVIC_EnableIRQ(RTC_IRQn);
}

























