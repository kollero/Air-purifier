#include "PMS7003.h"

PMS7003 pms7003 = PMS7003();

PMS7003::PMS7003() 
{
	
}

void PMS7003::init() //init values
{		
		pms7003_values_done=false;
		aqi_index=0;
		old_avg_PM1_0_CF1=0;
		old_avg_PM2_5_CF1=0;
		old_avg_PM10_CF1=0;
		
		avg_PM1_0_CF1=0;
		avg_PM2_5_CF1=0;
		avg_PM10_CF1=0;
		Iaq2_5=0;
		Iaq10=0;
		

		avg_PM1_0_CF1_short=0;
		avg_PM2_5_CF1_short=0;
		avg_PM10_CF1_short=0;
		aqi_index_short=0;
		
		old_avg_PM1_0_CF1_short=0;
		old_avg_PM2_5_CF1_short=0;
		old_avg_PM10_CF1_short=0;
				
		first_time=true;
	
}

//call after x seconds
//concentrations are in ug/m3
void PMS7003::AQI_calc_indexes()
{
	if(aqi_index >= PMS7003_sample_num_LONG) //after enought samples
	{
	char helper[14]={0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	//zero arrays
	strncpy(AQI_CATEGORY_PM2_5_CF1, helper, 14);
	strncpy(AQI_CATEGORY_PM10_CF1, helper, 14);

	old_max_PM2_5_CF1=max_PM2_5_CF1;
	old_min_PM2_5_CF1=min_PM2_5_CF1;
	old_avg_PM2_5_CF1=avg_PM2_5_CF1;
	max_PM2_5_CF1=0;
	min_PM2_5_CF1=1024;
	avg_PM2_5_CF1=0;
	
	old_max_PM10_CF1=max_PM10_CF1;
	old_min_PM10_CF1=min_PM10_CF1;
	old_avg_PM10_CF1=avg_PM10_CF1;
	max_PM10_CF1=0;
	min_PM10_CF1=1024;
	avg_PM10_CF1=0;
	
	old_avg_PM1_0_CF1=avg_PM1_0_CF1;
	avg_PM1_0_CF1=0;
	
	old_avg_PM1_0_CF1=round(old_avg_PM1_0_CF1/aqi_index);
	old_avg_PM2_5_CF1=round(old_avg_PM2_5_CF1/aqi_index);
	old_avg_PM10_CF1=round(old_avg_PM10_CF1/aqi_index);
	aqi_index=0;
	
	if(old_avg_PM1_0_CF1 > 1024){
		old_avg_PM1_0_CF1=0;
	}
	if(old_avg_PM2_5_CF1 > 1024){
		old_avg_PM2_5_CF1=0;
	}
	if(old_avg_PM10_CF1 > 1024){
		old_avg_PM10_CF1=0;
	}
	
	uint16_t Imax_PM2_5_CF1=0;
	uint16_t Ilow_PM2_5_CF1=0;
	
	uint16_t Imax_PM10_CF1=0;
	uint16_t Ilow_PM10_CF1=0;
	//min 2.5
	
	uint16_t i;
	for(i=1; i < 7;i++)
	{
		if((double)old_min_PM2_5_CF1 < AQI_INDEXES_PM2_5[i][0] )
		{
			Ilow_PM2_5_CF1=AQI_INDEXES_PM2_5[i-1][2];
			break;
		}
		if(i==6)
		{
			Ilow_PM2_5_CF1=AQI_INDEXES_PM2_5[6][2];
			break;
		}
	}
	
	//max2.5
	for( i=0; i < 7;i++)
	{
		if((double)old_max_PM2_5_CF1 <= AQI_INDEXES_PM2_5[i][1] )
		{
			Imax_PM2_5_CF1=AQI_INDEXES_PM2_5[i][3];
			break;
		}
		if(i==6)
		{
			Imax_PM2_5_CF1=AQI_INDEXES_PM2_5[6][3];
			break;
		}
	}
	
	//min 10
	for( i=1; i < 7;i++)
	{
		if(old_min_PM10_CF1 < AQI_INDEXES_PM10[i][0] )
		{
			Ilow_PM10_CF1=AQI_INDEXES_PM10[i-1][2];
			break;
		}
		if(i==6)
		{
			Ilow_PM10_CF1=AQI_INDEXES_PM10[6][2];
			break;
		}
	}
	//max10
	for( i=0; i < 7;i++)
	{
		if(old_max_PM10_CF1 <= AQI_INDEXES_PM10[i][1] )
		{
			Imax_PM10_CF1=AQI_INDEXES_PM10[i][3];
			break;
		}
		if(i==6)
		{
			Imax_PM10_CF1=AQI_INDEXES_PM10[6][3];
			break;
		}
	}
	
//AQI formula is: Iaq=((Ihigh-Ilow)/(Chigh-Clow))(C-Clow)+Ilow
//Ihigh=index breakpoint Chigh
//Ilow=index breakpoint Clow
//Clow=lowest value
//Chigh=highest value
//C=average

double Iaq2_5_temp=(((double)Imax_PM2_5_CF1-(double)Ilow_PM2_5_CF1)/((double)old_max_PM2_5_CF1-(double)old_min_PM2_5_CF1))* 
((double)old_avg_PM2_5_CF1-(double)old_min_PM2_5_CF1)+(double)Ilow_PM2_5_CF1;	

Iaq2_5=round(Iaq2_5_temp);
	
double Iaq10_temp=(((double)Imax_PM10_CF1-(double)Ilow_PM10_CF1)/((double)old_max_PM10_CF1-(double)old_min_PM10_CF1))* 
((double)old_avg_PM10_CF1-(double)old_min_PM10_CF1)+(double)Ilow_PM10_CF1;	

Iaq10=round(Iaq10_temp);

if(Iaq2_5 > 999 || Iaq2_5 < 0) Iaq2_5=0;
if(Iaq10 > 999 || Iaq10 < 0  )	Iaq10=0;

for( i=0; i < 7;i++)
	{
		if((double)Iaq2_5 <= AQI_INDEXES_PM2_5[i][3] )
		{
			strncpy(AQI_CATEGORY_PM2_5_CF1, AQI_CATEGORY[i], 14);
			
			break;
		}
		if(i==6)
		{
			strncpy(AQI_CATEGORY_PM2_5_CF1, AQI_CATEGORY[6], 14);
			break;
		}
	}

for( i=0; i < 7;i++)
	{
		if(Iaq10 <= AQI_INDEXES_PM10[i][3] )
		{
			strncpy(AQI_CATEGORY_PM10_CF1, AQI_CATEGORY[i], 14);
			break;
		}
		if(i==6)
		{
			strncpy(AQI_CATEGORY_PM10_CF1, AQI_CATEGORY[6], 14);
			break;
		}
	}
	
	pms7003_values_done=true;
	}
}


void PMS7003::AQ_calc_avg_short()
{
	if(aqi_index_short >= PMS7003_sample_num_SHORT ) //after some samples
	{
		
		old_avg_PM1_0_CF1_short=round(avg_PM1_0_CF1_short/aqi_index_short);
		old_avg_PM2_5_CF1_short=round(avg_PM2_5_CF1_short/aqi_index_short);
		old_avg_PM10_CF1_short=round(avg_PM10_CF1_short/aqi_index_short);
		
		avg_PM1_0_CF1_short=0;
		avg_PM2_5_CF1_short=0;
		avg_PM10_CF1_short=0;
		aqi_index_short=0;
		
		//bessel
		uint8_t i=0;
		for(i=0;i< LPF_FIR_BUFFER_LENGTH-1;i++)
		{
			LPF_FIR_BUFFER_PM1_0[i]=LPF_FIR_BUFFER_PM1_0[i+1];
			LPF_FIR_BUFFER_PM2_5[i]=LPF_FIR_BUFFER_PM1_0[i+1];
			LPF_FIR_BUFFER_PM10[i]=LPF_FIR_BUFFER_PM1_0[i+1];
			
		}
		LPF_FIR_BUFFER_PM1_0[LPF_FIR_BUFFER_LENGTH-1]=old_avg_PM1_0_CF1_short;
		LPF_FIR_BUFFER_PM2_5[LPF_FIR_BUFFER_LENGTH-1]=old_avg_PM2_5_CF1_short;
		LPF_FIR_BUFFER_PM10[LPF_FIR_BUFFER_LENGTH-1]=old_avg_PM10_CF1_short;
		
		if(first_time)
		{
			first_time=false;
			//fill 
			for(i=0;i< LPF_FIR_BUFFER_LENGTH-1;i++)
			{
				LPF_FIR_BUFFER_PM1_0[i]=old_avg_PM1_0_CF1_short;
				LPF_FIR_BUFFER_PM2_5[i]=old_avg_PM2_5_CF1_short;
				LPF_FIR_BUFFER_PM10[i]=old_avg_PM10_CF1_short;
				
			}

		}

		BesselLowpassSecondOrder004_PM1_0();
		BesselLowpassSecondOrder004_PM2_5();
		BesselLowpassSecondOrder004_PM10();
		
		old_avg_PM1_0_CF1_short=BesselLowpassSecondOrder004_buffer_PM1_0[0];
		old_avg_PM2_5_CF1_short=BesselLowpassSecondOrder004_buffer_PM2_5[0];
		old_avg_PM10_CF1_short=BesselLowpassSecondOrder004_buffer_PM10[0];
		
		for (i = 1; i < LPF_FIR_BUFFER_LENGTH; i++)
		{
			old_avg_PM1_0_CF1_short+=BesselLowpassSecondOrder004_buffer_PM1_0[i];
			old_avg_PM2_5_CF1_short+=BesselLowpassSecondOrder004_buffer_PM2_5[i];
			old_avg_PM10_CF1_short+=BesselLowpassSecondOrder004_buffer_PM10[i];
		}

	}
}


// cuts off at 0.04 * sampling freq
void PMS7003::BesselLowpassSecondOrder004_PM1_0()
{
    float xv[3] = {0.0,0.0,0.0};
	float yv[3] = {0.0,0.0,0.0};
 
    for (int i = 0; i < LPF_FIR_BUFFER_LENGTH; i++)
    { 
        xv[0] = xv[1]; 
		xv[1] = xv[2]; 
        xv[2] = (float)LPF_FIR_BUFFER_PM1_0[i] / float(50.5047);
        yv[0] = yv[1]; 
		yv[1] = yv[2]; 
        yv[2] = (xv[0] + xv[2]) + 2 * xv[1] + ( -0.5731643146 * yv[0]) + (  1.4939637515 * yv[1]);
        BesselLowpassSecondOrder004_buffer_PM1_0[i] = yv[2];
    } 

}

// cuts off at 0.04 * sampling freq
void PMS7003::BesselLowpassSecondOrder004_PM2_5()
{
    float xv[3] = {0.0,0.0,0.0};
	float yv[3] = {0.0,0.0,0.0};
 
    for (int i = 0; i < LPF_FIR_BUFFER_LENGTH; i++)
    { 
        xv[0] = xv[1]; 
		xv[1] = xv[2]; 
        xv[2] = (float)LPF_FIR_BUFFER_PM2_5[i] / float(50.5047);
        yv[0] = yv[1]; 
		yv[1] = yv[2]; 
        yv[2] = (xv[0] + xv[2]) + 2 * xv[1] + ( -0.5731643146 * yv[0]) + (  1.4939637515 * yv[1]);
        BesselLowpassSecondOrder004_buffer_PM2_5[i] = yv[2];
    } 

}

// cuts off at 0.04 * sampling freq
void PMS7003::BesselLowpassSecondOrder004_PM10()
{
    float xv[3] = {0.0,0.0,0.0};
	float yv[3] = {0.0,0.0,0.0};
 
    for (int i = 0; i < LPF_FIR_BUFFER_LENGTH; i++)
    { 
        xv[0] = xv[1]; 
		xv[1] = xv[2]; 
        xv[2] = (float)LPF_FIR_BUFFER_PM10[i] / float(50.5047);
        yv[0] = yv[1]; 
		yv[1] = yv[2]; 
        yv[2] = (xv[0] + xv[2]) + 2 * xv[1] + ( -0.5731643146 * yv[0]) + (  1.4939637515 * yv[1]);
        BesselLowpassSecondOrder004_buffer_PM10[i] = yv[2];
    } 

}




//called in sercom interrupt
void PMS7003::values_calc_helper()
{

//1.0
avg_PM1_0_CF1+=PMS7003_Values.concPM1_0_CF1;

//2.5
if(PMS7003_Values.concPM2_5_CF1 > max_PM2_5_CF1)
{
	max_PM2_5_CF1=PMS7003_Values.concPM2_5_CF1;
}
if(PMS7003_Values.concPM2_5_CF1 < min_PM2_5_CF1)
{
	min_PM2_5_CF1=PMS7003_Values.concPM2_5_CF1;
}
avg_PM2_5_CF1+=PMS7003_Values.concPM2_5_CF1;

//10.0
if(PMS7003_Values.concPM10_0_CF1 > max_PM10_CF1)
{
	max_PM10_CF1=PMS7003_Values.concPM10_0_CF1;
}
if(PMS7003_Values.concPM10_0_CF1 < min_PM10_CF1)
{
	min_PM10_CF1=PMS7003_Values.concPM10_0_CF1;
}
avg_PM10_CF1+=PMS7003_Values.concPM10_0_CF1;
aqi_index++;

avg_PM1_0_CF1_short+=PMS7003_Values.concPM1_0_CF1;
avg_PM2_5_CF1_short+=PMS7003_Values.concPM2_5_CF1;;
avg_PM10_CF1_short+=PMS7003_Values.concPM10_0_CF1;
aqi_index_short++;
}

void PMS7003_move_to_frame()
{
	PMS7003_values_ready=0;
    PMS7003_Values.concPM1_0_CF1 = PMS7003_buffer[4]<<8|PMS7003_buffer[5];                    
    PMS7003_Values.concPM2_5_CF1 = PMS7003_buffer[6]<<8|PMS7003_buffer[7];                      
    PMS7003_Values.concPM10_0_CF1 = PMS7003_buffer[8]<<8|PMS7003_buffer[9];                       
    PMS7003_Values.concPM1_0_amb = PMS7003_buffer[10]<<8|PMS7003_buffer[11];                        
    PMS7003_Values.concPM2_5_amb = PMS7003_buffer[12]<<8|PMS7003_buffer[13];                      
    PMS7003_Values.concPM10_0_amb = PMS7003_buffer[14]<<8|PMS7003_buffer[15];     
    PMS7003_Values.rawGt0_3um = PMS7003_buffer[16]<<8|PMS7003_buffer[17];     
    PMS7003_Values.rawGt0_5um = PMS7003_buffer[18]<<8|PMS7003_buffer[19];   
    PMS7003_Values.rawGt1_0um = PMS7003_buffer[20]<<8|PMS7003_buffer[21];    
    PMS7003_Values.rawGt2_5um = PMS7003_buffer[22]<<8|PMS7003_buffer[23];     
    PMS7003_Values.rawGt5_0um = PMS7003_buffer[24]<<8|PMS7003_buffer[25];     
    PMS7003_Values.rawGt10_0um = PMS7003_buffer[26]<<8|PMS7003_buffer[27];     
    PMS7003_Values.version = PMS7003_buffer[28];    
    PMS7003_Values.errorCode = PMS7003_buffer[29];
	PMS7003_values_ready=1;
	
	pms7003.values_calc_helper(); //values for averaging
}

void PMS7003::wake()
{
	REG_PORT_OUTSET0=(1<<7); //for pin high
}
void PMS7003::sleep()
{
	REG_PORT_OUTCLR0=(1<<7); //for pin low	
}

void SERCOM2_Handler() //uart pms7003
{
	//data receive complete
	//automatically cleared when data register is read
	if(SERCOM2->USART.INTFLAG.bit.RXC) 
	{
		PMS7003_buffer[read_data_PMS7003_index++]=SERCOM2->USART.DATA.reg;
		switch (read_data_PMS7003_index) {
					case 1:
						if(PMS7003_buffer[0]!=PMS7003_Values.PMS7003_frameHeader[0] ) //0x42, 66 in dec
						{
							read_data_PMS7003_index=0;
							PMS7003_buffer[0]=0;
						}
						break;
					case 2:
						if(PMS7003_buffer[1]!=PMS7003_Values.PMS7003_frameHeader[1] ) //0x4d,77 in dec
						{
							read_data_PMS7003_index=0;
							PMS7003_buffer[0]=0;
							PMS7003_buffer[1]=0;
						}
						break;
					case 3:
						if(PMS7003_buffer[2]!=0 ) 
						{
							read_data_PMS7003_index=0;
							PMS7003_buffer[0]=0;
							PMS7003_buffer[1]=0;
							PMS7003_buffer[2]=0;
						}
						break;
					case 4:
						if(PMS7003_buffer[3]!=PMS7003_Values.PMS7003_frameHeader[2]) //0x1c=28
						{
							read_data_PMS7003_index=0;
							PMS7003_buffer[0]=0;
							PMS7003_buffer[1]=0;
							PMS7003_buffer[2]=0;
							PMS7003_buffer[3]=0;
						}
						break;		
					case PMS7003_read_buffer_length-1:
                        PMS7003_Values.checksum = PMS7003_buffer[PMS7003_read_buffer_length-2]<<8;
                        break;	
                    case PMS7003_read_buffer_length:
						read_data_PMS7003_index=0;
                        PMS7003_Values.checksum += PMS7003_buffer[PMS7003_read_buffer_length-1];
						
						for(uint8_t i=0;i<=29;i++)
						{
							PMS7003_Values.checksum-=PMS7003_buffer[i];
						}
						PMS7003_buffer[0]=0;
						PMS7003_buffer[1]=0;
						PMS7003_Values.values_ok=0;
						if(PMS7003_Values.checksum==0)
						{
							PMS7003_Values.values_ok=1;
							PMS7003_move_to_frame(); //move buffer values to a struct
						}
                        break;
		}
	}
		
	
}
