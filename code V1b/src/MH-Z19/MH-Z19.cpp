#include "MH-Z19.h"

MHZ19 mhz19=MHZ19(); 


MHZ19::MHZ19() 
{
	
}

void MHZ19::change_to_2000ppm()
{
	while(need_to_send_MHZ19);
	need_to_send_MHZ19=1;
	//MHZ19_tx_buffer=MHZ19_read_data;
	for(uint8_t i=0 ;i<MHZ19_read_buffer_length;i++)
	{
		MHZ19_tx_buffer[i]=MHZ19_change_to_2000ppm[i];
	}

	//memcpy(MHZ19_read_data, MHZ19_tx_buffer,9);
	SERCOM4->USART.DATA.reg=MHZ19_tx_buffer[tx_data_MHZ19_index++];
}

void MHZ19::change_to_5000ppm()
{
	while(need_to_send_MHZ19);
	need_to_send_MHZ19=1;
	//MHZ19_tx_buffer=MHZ19_read_data;
	for(uint8_t i=0 ;i<MHZ19_read_buffer_length;i++)
	{
		MHZ19_tx_buffer[i]=MHZ19_change_to_5000ppm[i];
	}

	//memcpy(MHZ19_read_data, MHZ19_tx_buffer,9);
	SERCOM4->USART.DATA.reg=MHZ19_tx_buffer[tx_data_MHZ19_index++];
}

void MHZ19::request_data()
{
	while(need_to_send_MHZ19);
	need_to_send_MHZ19=1;
	//MHZ19_tx_buffer=MHZ19_read_data;
	for(uint8_t i=0 ;i<MHZ19_read_buffer_length;i++)
	{
		MHZ19_tx_buffer[i]=MHZ19_read_data[i];
	}

	//memcpy(MHZ19_read_data, MHZ19_tx_buffer,9);
	SERCOM4->USART.DATA.reg=MHZ19_tx_buffer[tx_data_MHZ19_index++];
}



void MHZ19::wake()
{
	REG_PORT_OUTSET0=(1<<12); //for pin high
}
void MHZ19::sleep()
{
	REG_PORT_OUTCLR0=(1<<12); //for pin low	
}

void SERCOM4_Handler() //uart MHZ19
{
	//data receive complete
	//automatically cleared when data register is read
	
	//data register is empty, and all data has been read
	if(SERCOM4->USART.INTFLAG.bit.TXC==1) 
	{	
		if(need_to_send_MHZ19==1)
		{
			//MHZ19_tx_buffer[]=MHZ19_read_data[];
			SERCOM4->USART.DATA.reg=MHZ19_tx_buffer[tx_data_MHZ19_index++];
			if(tx_data_MHZ19_index>=9)
			{
				tx_data_MHZ19_index=0;
				need_to_send_MHZ19=0;
			}
		}
		else
		{
			SERCOM4->USART.INTFLAG.bit.TXC=1;
		}	
	}
	
	if(SERCOM4->USART.INTFLAG.bit.RXC==1) 
	{
		//SERCOM4->USART.INTFLAG.bit.DRE=1;
		MHZ19_rx_buffer[rx_data_MHZ19_index++]=SERCOM4->USART.DATA.reg;
		switch (rx_data_MHZ19_index) {
					case 1:
						if(MHZ19_rx_buffer[0]!=0xff  ) 
						{
							rx_data_MHZ19_index=0;
							MHZ19_rx_buffer[0]=0;
						}
						break;
					case 2:
						if(MHZ19_rx_buffer[1]!=0x86  ) 
						{
							rx_data_MHZ19_index=0;
							MHZ19_rx_buffer[0]=0;
							MHZ19_rx_buffer[1]=0;
						}
						break;
					case 9:
						rx_data_MHZ19_index=0;
						MHZ19_checksum=0;
						for(uint8_t i=1;i<=7;i++)
						{
							MHZ19_checksum+=MHZ19_rx_buffer[i];
						}
						MHZ19_rx_buffer[0]=0;
						MHZ19_rx_buffer[1]=0;
						
						MHZ19_checksum=0xff-MHZ19_checksum;
						MHZ19_checksum+=1; //add 1
						if(MHZ19_rx_buffer[8]==(uint8_t)(MHZ19_checksum)) //%256
						{
							co2_concentration=MHZ19_rx_buffer[3]+(MHZ19_rx_buffer[2]<<8);
						}	

						break;	
		}
		
	}
	
}
