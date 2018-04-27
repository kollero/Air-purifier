#include "MICS6814.h"

MICS6814 mics6814 = MICS6814();

MICS6814::MICS6814() 
{
	
}

void MICS6814::init()
{
	wake();
	delay_ms(300); //wait till heated some
	//adc inputs on after some heating has happened otherwise values will be over the scale
	
	//pa04, pa05 to adc
	PORT->Group[0].WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG |PORT_WRCONFIG_WRPMUX| PORT_WRCONFIG_PMUX(1)|
									PORT_WRCONFIG_PMUXEN|PORT_WRCONFIG_PINMASK((1<<4)|(1<<5));
	//(pb08,pb09 ), B=1, ADC inputs								
	PORT->Group[1].WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG |PORT_WRCONFIG_WRPMUX| PORT_WRCONFIG_PMUX(1)|PORT_WRCONFIG_PMUXEN|PORT_WRCONFIG_PINMASK((1<<8)|(1<<9));
	
}

void MICS6814::read_and_calculate()
{
	readCurrentADC();
	calcGases();
	
}

void MICS6814::readCurrentADC()
{
	//adc reference is 3.3v/1.48=2.22973v, which is max voltage
	//nh3=pb08	ain[2]
	//co=pb09	ain[3]
	//no2=pa04 	ain[4]
	//reference voltage =pa05 ain[5]

	Ameas_0=readADC(2); //muxpos input, 
	Ameas_1=readADC(3);
	Ameas_2=readADC(4);
	//MICS6814_Values.vref_ADC=readADC(5);
}


void MICS6814::calcGases()
{
	
//reference values saved either in eeprom or flash
	uint16_t Aref_0 = REFERENCE_ADC_NH3;
	uint16_t Aref_1 = REFERENCE_ADC_CO;
	uint16_t Aref_2 = REFERENCE_ADC_NO2;

//measured values
	 //Ameas_0 =MICS6814_Values.nh3_ADC;
	 //Ameas_1 =MICS6814_Values.co_ADC;
	 //Ameas_2 =MICS6814_Values.no2_ADC;
	
	//get ratios for measured vs references
	ratio0  = (((double)ADC_PRECISION-(double)Aref_0)/((double)ADC_PRECISION-(double)Ameas_0));
	ratio0 *= (double)Ameas_0/(double)Aref_0;
	ratio1  = (((double)ADC_PRECISION-(double)Aref_1)/((double)ADC_PRECISION-(double)Ameas_1));
	ratio1 *= (double)Ameas_1/(double)Aref_1;
	ratio2  = ((double)ADC_PRECISION-(double)Aref_2)/((double)ADC_PRECISION-(double)Ameas_2);
	ratio2 *= (double)Ameas_2/(double)Aref_2;
	
//CO (1/K)=(1000-1)/(0.01-3.3)=-303.64741
//NO2 (1/K)=(6.5-0.01)/(40-0.065)=0.162514
//NH3 K=(150-1)/(0.05-0.65)=-248.333


//co=750k
//no2=18k
//nh3=750k


//CO at  1ppm=  100kohm, 100ppm=1500kohm
//NO2 at 0.5ppm=0.8kohm, 10ppm= 20kohm
//NH3 at 1ppm=  10kohm,  300ppm=1500kohm


MICS6814_Values.NH3 = pow(ratio0, -1.67)/1.47;
MICS6814_Values.CO = pow(ratio1, -1.179)*4.385;
 MICS6814_Values.NO2 = pow(ratio2, 1.007)/6.855;

MICS6814_Values.C3H8 = pow(ratio0, -2.518)*570.164;
MICS6814_Values.C4H10 = pow(ratio0, -2.138)*398.107;
MICS6814_Values.CH4 = pow(ratio1, -4.363)*630.957;
MICS6814_Values.H2 = pow(ratio1, -1.8)*0.73;
MICS6814_Values.C2H5OH = pow(ratio1, -1.552)*1.622;
     
	
}


void MICS6814::wake()
{
	//pa06
	REG_PORT_OUTSET0=(1<<6); //heater on	
}

void MICS6814::sleep()
{
	//pa06
	REG_PORT_OUTCLR0=(1<<6); //heater off
}

void MICS6814::Calibrate()
{
	allow_display_update=false; //main function
	
	REG_PM_APBCMASK =PM_APBCMASK_SERCOM1|PM_APBCMASK_ADC; //shutdown everything else
	
	while(gde027w3.update_inprogress);
	//delay_ms(6000);
	gde027w3.ClearFrame();
	gde027w3.DisplayFrame(); //update whole display
	
	while(gde027w3.update_inprogress);
	//delay_ms(6000);
	
	char nh3_min_string[10]={0,0,0,0,0,0,0,0,0,0};
	char co_min_string[10]={0,0,0,0,0,0,0,0,0,0};
	char no2_min_string[10]={0,0,0,0,0,0,0,0,0,0};
	
	char nh3_max_string[10]={0,0,0,0,0,0,0,0,0,0};
	char co_max_string[10]={0,0,0,0,0,0,0,0,0,0};
	char no2_max_string[10]={0,0,0,0,0,0,0,0,0,0};

	uint16_t temp_nh3_ADC, temp_co_ADC, temp_no2_ADC;
	
	uint16_t temp_nh3_ADC2, temp_co_ADC2, temp_no2_ADC2;
	
	uint16_t temp_nh3_ADC_min=4096, temp_co_ADC_min=4096, temp_no2_ADC_min=4096;
	uint16_t temp_nh3_ADC_max=0, temp_co_ADC_max=0, temp_no2_ADC_max=0;
	
	while(1)
        {
			temp_nh3_ADC_min=4096; temp_co_ADC_min=4096; temp_no2_ADC_min=4096;
			temp_nh3_ADC_max=0; temp_co_ADC_max=0; temp_no2_ADC_max=0;
            temp_nh3_ADC=readADC(2); //muxpos input   13 11
			temp_co_ADC=readADC(3);
			temp_no2_ADC=readADC(4);
            
            int cnt = 0;
            for(uint16_t i=0; i<20; i++)
            {
				
				 temp_nh3_ADC2=readADC(2); //muxpos input   13 11
				 temp_co_ADC2=readADC(3);
				 temp_no2_ADC2=readADC(4);
				
                if((abs(temp_nh3_ADC-temp_nh3_ADC2) > MICS6814_CALIBRATION_MAX_VARIANCE ))cnt++;
                if((abs(temp_co_ADC -temp_co_ADC2) > MICS6814_CALIBRATION_MAX_VARIANCE ))cnt++;
                if((abs(temp_no2_ADC-temp_no2_ADC2) > MICS6814_CALIBRATION_MAX_VARIANCE ))cnt++;
				
				if( temp_nh3_ADC2 < temp_nh3_ADC_min )
				{
					temp_nh3_ADC_min=temp_nh3_ADC2;
				}
				 if (temp_nh3_ADC2 > temp_nh3_ADC_max)
				{
					temp_nh3_ADC_max=temp_nh3_ADC2;	
				}
				
				if( temp_co_ADC2 < temp_co_ADC_min )
				{
					temp_co_ADC_min=temp_co_ADC2;
				}
				 if (temp_co_ADC2 > temp_co_ADC_max)
				{
					temp_co_ADC_max=temp_co_ADC2;
				}
				
				if( temp_no2_ADC2 < temp_no2_ADC_min )
				{
					temp_no2_ADC_min=temp_no2_ADC2;
				}
				 if (temp_no2_ADC2 > temp_no2_ADC_max)
				{
					temp_no2_ADC_max=temp_no2_ADC2;
				}  
				
                if(cnt>5)
                {
                    break;
                }
				 while(gde027w3.update_inprogress);
				 delay_ms(1000);
				
            }
			
			
			temp_nh3_ADC_min=abs(temp_nh3_ADC_max-temp_nh3_ADC_min);
			temp_co_ADC_min=abs( temp_co_ADC_max-temp_co_ADC_min);
			temp_no2_ADC_min=abs(temp_no2_ADC_max-temp_no2_ADC_min);
			
			intToStr(temp_nh3_ADC_min,nh3_min_string,2);
			intToStr(temp_co_ADC_min,co_min_string,2);
			intToStr(temp_no2_ADC_min,no2_min_string,2);
			
			intToStr(temp_nh3_ADC_max,nh3_max_string,2);
			intToStr(temp_co_ADC_max,co_max_string,2);
			intToStr(temp_no2_ADC_max,no2_max_string,2);
			
			gde027w3.ClearFrame();
			gde027w3.drawString(200, 10, nh3_min_string, GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
			gde027w3.drawString(200, 20, co_min_string, GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
			gde027w3.drawString(200, 30, no2_min_string, GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
			
			gde027w3.drawString(200, 40, nh3_max_string, GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
			gde027w3.drawString(200, 50, co_max_string, GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
			gde027w3.drawString(200, 60, no2_max_string, GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
			
			gde027w3.RefreshPartial(200, 10, 50, 70);//x,y,w,h
			//gde027w3.DisplayFrame(); //update whole display
			
            if(cnt <= 5)break; //success
            delay_ms(200);
        }
	
	while(gde027w3.update_inprogress);
	// delay_ms(1000);4
	
	
	//after done show values on display
	//have to save values in program flash manually, since EEPROM will get deleted during erase cycle
	
	
char nh3_string[10]={0,0,0,0,0,0,0,0,0,0};
char co_string[10]={0,0,0,0,0,0,0,0,0,0};
char no2_string[10]={0,0,0,0,0,0,0,0,0,0};

char calib_string[22]="Calibration Complete!";

char nh3_string_mark[10]={'N','H','3',':',0,0,0,0,0,0};
char co_string_mark[10]={'C','O',':',0,0,0,0,0,0,0};
char no2_string_mark[10]={'N','O','2',':',0,0,0,0,0,0};
	
	temp_nh3_ADC_min=abs(temp_nh3_ADC_max-temp_nh3_ADC_min);
	temp_co_ADC_min=abs( temp_co_ADC_max-temp_co_ADC_min);
	temp_no2_ADC_min=abs(temp_no2_ADC_max-temp_no2_ADC_min);
	
	intToStr(temp_nh3_ADC,nh3_string,2);
	intToStr(temp_co_ADC,co_string,2);
	intToStr(temp_no2_ADC,no2_string,2);
	
	intToStr(temp_nh3_ADC_min,nh3_min_string,2);
	intToStr(temp_co_ADC_min,co_min_string,2);
	intToStr(temp_no2_ADC_min,no2_min_string,2);
	
	
	//intToStr(temp_nh3_ADC_max,nh3_max_string,2);
	//intToStr(temp_co_ADC_max,co_max_string,2);
	//intToStr(temp_no2_ADC_max,no2_max_string,2);
	
		
	gde027w3.ClearFrame();
	
	//gde027w3.drawString(160, 10, nh3_min_string, GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
	//gde027w3.drawString(160, 20, co_min_string, GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
	//gde027w3.drawString(160, 30, no2_min_string, GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
	
	//gde027w3.drawString(200, 40, nh3_max_string, GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
	//gde027w3.drawString(200, 50, co_max_string, GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
	//gde027w3.drawString(200, 60, no2_max_string, GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
	
	
	gde027w3.drawString(180, 40, calib_string , GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
	
	gde027w3.drawString(100, 60, nh3_string_mark, GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
	gde027w3.drawString(100, 70, co_string_mark, GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
	gde027w3.drawString(100, 80, no2_string_mark, GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
	
	gde027w3.drawString(150, 60, nh3_string, GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
	gde027w3.drawString(150, 70, co_string, GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
	gde027w3.drawString(150, 80, no2_string, GDEW027W3_BLACK, GDEW027W3_WHITE, 1);
	
	gde027w3.DisplayFrame(); //update whole display
	while(gde027w3.update_inprogress);

}



