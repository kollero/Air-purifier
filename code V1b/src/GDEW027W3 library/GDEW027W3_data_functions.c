
void GDEW027W3::TransactSPI(uint8_t data)
{	
	REG_SERCOM1_SPI_DATA=data;
	while(!SERCOM1->SPI.INTFLAG.bit.TXC);
	
}

void GDEW027W3::IL91874_sendCmd(uint8_t data)
{
	GDEW027W3_DC_COMMAND;
	GDEW027W3_CS_LOW;	
	TransactSPI(data);
	GDEW027W3_CS_HIGH;
}

void GDEW027W3::IL91874_sendData(uint8_t data)
{
	GDEW027W3_DC_DATA;
	GDEW027W3_CS_LOW;	
	TransactSPI(data);
	GDEW027W3_CS_HIGH;
}
