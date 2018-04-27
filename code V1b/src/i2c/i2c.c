#define I2C_SERCOM SERCOM0

enum
{
  I2C_TRANSFER_WRITE = 0,
  I2C_TRANSFER_READ  = 1,
};


// Transmit buffer length
#define TXMAXBUFLEN 32
// Receive buffer length
#define RXMAXBUFLEN 32


uint8_t i2cTransmitBuffer[TXMAXBUFLEN];


volatile uint8_t i2cReceiveBuffer[RXMAXBUFLEN];


uint8_t dec2bcd(uint8_t d)
{
  return ((d/10 * 16) + (d % 10));
}

uint8_t bcd2dec(uint8_t b)
{
  return ((b/16 * 10) + (b % 16));
}



//-----------------------------------------------------------------------------
bool i2c_TransmitData(void *const TXdata, uint8_t dataLen)
{
	uint8_t *data = (uint8_t *)TXdata;
	
	for (int i = 0; i <= dataLen; i++)
	{
		i2cTransmitBuffer[i] = data[i];
	}
	
  I2C_SERCOM->I2CM.ADDR.reg = i2cTransmitBuffer[0] | I2C_TRANSFER_WRITE;

  while (0 == (I2C_SERCOM->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB));

  if (I2C_SERCOM->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_RXNACK)
  {
    I2C_SERCOM->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);
    //dbg_log("I2C: RXNACK during write (address)\r\n");
    return false;
  }

  for (int i = 1; i <= dataLen; i++)
  {
    I2C_SERCOM->I2CM.DATA.reg = i2cTransmitBuffer[i];

    while (0 == (I2C_SERCOM->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB));

    if (I2C_SERCOM->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_RXNACK)
    {
      I2C_SERCOM->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);
      //dbg_log("I2C: RXNACK during write (data)\r\n");
      return false;
    }
  }

  I2C_SERCOM->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);

  return true;
}

//-----------------------------------------------------------------------------
bool i2c_ReadData(void *const TXdata,uint8_t dataLen, uint8_t bytesToRead)
{
	
	uint8_t *data = (uint8_t *)TXdata;
	for (int i = 0; i <= dataLen; i++)
	{
		i2cTransmitBuffer[i] = data[i];
	}
	
	I2C_SERCOM->I2CM.ADDR.reg = i2cTransmitBuffer[0] | I2C_TRANSFER_WRITE;

  while (0 == (I2C_SERCOM->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB));

  if (I2C_SERCOM->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_RXNACK)
  {
    I2C_SERCOM->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);
    //dbg_log("I2C: RXNACK during write (address)\r\n");
    return false;
  }

  for (int i = 1; i <= dataLen; i++)
  {
	
    I2C_SERCOM->I2CM.DATA.reg = i2cTransmitBuffer[i];

    while (0 == (I2C_SERCOM->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB));

    if (I2C_SERCOM->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_RXNACK)
    {
      I2C_SERCOM->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);
      //dbg_log("I2C: RXNACK during write (data)\r\n");
      return false;
    }
  }
  //I2C_SERCOM->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(1); //added
  //no stop condition here
	
  I2C_SERCOM->I2CM.ADDR.reg = i2cTransmitBuffer[0] | I2C_TRANSFER_READ;

  while (0 == (I2C_SERCOM->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_SB));

  if (I2C_SERCOM->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_RXNACK)
  {
    I2C_SERCOM->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);
    //dbg_log("I2C: RXNACK during read (address)\r\n");
    return false;
  }
  
   if (bytesToRead >=2){
  I2C_SERCOM->I2CM.CTRLB.reg &= ~SERCOM_I2CM_CTRLB_ACKACT; //ack

  for (int i = 0; i < bytesToRead-1; i++)
  {
	 
    i2cReceiveBuffer[i] = I2C_SERCOM->I2CM.DATA.reg;
    while (0 == (I2C_SERCOM->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_SB));
  }
  }
    I2C_SERCOM->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_ACKACT; //send nack
    I2C_SERCOM->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);// stop condition
    i2cReceiveBuffer[bytesToRead-1] = I2C_SERCOM->I2CM.DATA.reg; //final data byte

  return true;
}
