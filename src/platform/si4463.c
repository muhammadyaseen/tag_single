///*
// * si4463.c
// *
// *  Created on: 2 Jun 2015
// *      Author: KFRL/SM
// */
//
//#include "si4463.h"
//
//const static uint8 config_table[] = RADIO_CONFIGURATION_DATA_ARRAY;
///*! ------------------------------------------------------------------------------------------------------------------
// * Function: SI4463_init
// *
// * Initialise the SI4463 device
// *
// */
//void SI4463_init(void)
//{
//	uint8 *app_command_buf;
//
//    uint8 i ,k ,hc[15];
//    uint16 j = 0;
//    // read the configuration array, break it in commands according to length defined in the array
//    // send each command separately
//    while( ( i = config_table[j] ) != 0 )
//    {
//        j += 1;
//        for(k = 0 ; k < i ; ++k){hc[k] = config_table[j + k];}
//        si_cmd(&hc[0] , i );
//        j += i;
//    }
//
//#if SI_PACKET_LENGTH > 0           //fixed packet length
//    si_setPreoperty1( PKT_FIELD_1_LENGTH_7_0, SI_PACKET_LENGTH );
//#else                           //variable packet length
//    SI446X_SET_PROPERTY_1( PKT_CONFIG1, 0x00 );
//    SI446X_SET_PROPERTY_1( PKT_CRC_CONFIG, 0x00 );
//
//    SI446X_SET_PROPERTY_1( PKT_LEN_FIELD_SOURCE, 0x01 );
//    for(k = 0 ; k < 255 ; ++k){;}
//    i = SI446X_GET_PROPERTY_1(PKT_LEN_FIELD_SOURCE);
//
//    SI446X_SET_PROPERTY_1( PKT_LEN, 0x2A );
//    SI446X_SET_PROPERTY_1( PKT_LEN_ADJUST, 0x00 );
//    SI446X_SET_PROPERTY_1( PKT_FIELD_1_LENGTH_12_8, 0x00 );
//    SI446X_SET_PROPERTY_1( PKT_FIELD_1_LENGTH_7_0, 0x01 );
//    for(k = 0 ; k < 255 ; ++k){;}
//    i = SI446X_GET_PROPERTY_1(PKT_FIELD_1_LENGTH_7_0);
//    SI446X_SET_PROPERTY_1( PKT_FIELD_1_CONFIG, 0x00 );
//    SI446X_SET_PROPERTY_1( PKT_FIELD_1_CRC_CONFIG, 0x00 );
//    SI446X_SET_PROPERTY_1( PKT_FIELD_2_LENGTH_12_8, 0x00 );
//    SI446X_SET_PROPERTY_1( PKT_FIELD_2_LENGTH_7_0, 0x10 );
//    for(k = 0 ; k < 255 ; ++k){;}
//    i = SI446X_GET_PROPERTY_1(PKT_FIELD_2_LENGTH_7_0);
//    SI446X_SET_PROPERTY_1( PKT_FIELD_2_CONFIG, 0x00 );
//    SI446X_SET_PROPERTY_1( PKT_FIELD_2_CRC_CONFIG, 0x00 );
//#endif //PACKET_LENGTH
//
//	*(app_command_buf+0) = RF_GPIO_PIN_CFG; // gpio setting
//	*(app_command_buf+1) = 0x01; // 0x21;    // gpio 0 ,Rx data
//	*(app_command_buf+2) = 0x01; // gpio1, output 0  high level when power on
//	*(app_command_buf+3) = 0x01; // gpio2, h = tx mode
//	*(app_command_buf+4) = 0x01; // gpio3
//	*(app_command_buf+5) = 0x27; // nIRQ
//	*(app_command_buf+6) = 0x0b; // sdo
//	si_spi_write(7, app_command_buf);
//}
//
///*! ------------------------------------------------------------------------------------------------------------------
// * Function: SI446X_NOP
// *
// * no operation command
// */
//uint8 SI446X_NOP( void )
//{
//    uint8 cts;
//    cts = si_readSPIbyte();
//	return cts;
//}
///*! ------------------------------------------------------------------------------------------------------------------
// * Function: wait_cts
// *
// * read spi until clear to send signal receive
// */
//void wait_cts(void)
//{
//	uint8 cts;
//    do
//    {
//    	si_writeSPIbyte( READ_CMD_BUFF );
//    	SI_NSEL_CLR();
//        cts = SPI_ExchangeByte( 0xFF );
//        SI_NSEL_SET();
//    }while( cts != 0xFF );
//}
//
///*! ------------------------------------------------------------------------------------------------------------------
// * Function: si_writeSPIbyte
// *
// * write one byte on si4463 spi port
// */
//void si_writeSPIbyte(uint8 cmd)
//{
//	CLR_SI_SCK();
//	SI_NSEL_CLR();
//	SI_SPI->DR = cmd;
//	while ((SI_SPI->SR & SPI_I2S_FLAG_RXNE) == (uint16_t)RESET);
//	SI_SPI->DR ;
//	SI_NSEL_SET();
//
//} // end si_readSPIbyte()
//
///*! ------------------------------------------------------------------------------------------------------------------
// * Function: si_readSPIbyte
// *
// * read one byte from si4463 spi port
// */
//uint8 si_readSPIbyte()
//{
//	uint8 i;
//
//	CLR_SI_SCK();
//	SI_NSEL_CLR();
//	SPIx->DR = 0;
//	while((SPIx->SR & SPI_I2S_FLAG_RXNE) == (uint16_t)RESET);
//	i = SPIx->DR;//port_SPIx_receive_data(); //this clears RXNE bit
//	SI_NSEL_SET();
//    return i;
//} // end si_readSPIbyte()
//
///*! ------------------------------------------------------------------------------------------------------------------
// * Function: si_spi_write
// *
// * write multiple bytes on si4463 spi port
// */
//void si_spi_write( uint16 length, uint8 *buffer)
//{
//	uint16 i=0;
//	CLR_SI_SCK();
//	SI_NSEL_CLR();
//    for(i=0; i<length; i++)
//    {
//    	SI_SPI->DR = buffer[i];
//    	while ((SI_SPI->SR & SPI_I2S_FLAG_RXNE) == (uint16_t)RESET);
//    	SI_SPI->DR ;
//    }
//    SI_NSEL_SET();
//}
//
///*! ------------------------------------------------------------------------------------------------------------------
// * Function: si_writetospi_packet
// *
// * write packet with header on si4463 spi port
// */
//int si_writetospi_packet
//(
//    uint16       headerLength,
//    const uint8 *headerBuffer,
//    uint32       bodylength,
//    const uint8 *bodyBuffer
//)
//{
//	int i=0;
//	CLR_SI_SCK();
//	SI_NSEL_CLR();
//	// write header
//    for(i=0; i<headerLength; i++)
//    {
//    	SI_SPI->DR = headerBuffer[i];
//    	while ((SI_SPI->SR & SPI_I2S_FLAG_RXNE) == (uint16_t)RESET);
//    	SI_SPI->DR ;
//    }
//    // write body
//    for(i=0; i<bodylength; i++)
//    {
//    	SI_SPI->DR = bodyBuffer[i];
//    	while((SI_SPI->SR & SPI_I2S_FLAG_RXNE) == (uint16_t)RESET);
//    	SI_SPI->DR ;
//	}
//    SI_NSEL_SET();
//    return 0;
//}
//
///*! ------------------------------------------------------------------------------------------------------------------
// * Function: si_cmd
// *
// * send si4463 command with other bytes (parameters and etc.)
// */
//void si_cmd( uint8 *cmd, uint8 cmdsize )
//{
//    wait_cts( );
//    SI_NSEL_CLR( );
//    Delay(5);
//    while( cmdsize -- )
//    {
//        si_writeSPIbyte( *cmd++ );
//    }
//    SI_NSEL_SET( );
//}
//
///*! ------------------------------------------------------------------------------------------------------------------
// * Function: SPI_ExchangeByte
// *
// * write and read one byte of spi at same time,
// * using software driver
// */
//uint8 SPI_ExchangeByte(uint8 byte)
//{
//	uint8 i ,zhi ,j;
//
//	CLR_SI_SCK();
//	zhi = byte;
//	Delay(5);
//	for(i = 0 ; i < 8 ; ++i)
//	{
//		Delay(5);
//		if((zhi & 0x80) == 0)
//			CLR_SI_SDO();
//		else
//			SET_SI_SDO();
//		zhi <<= 1;
//		Delay(5);
//		SET_SI_SCK();
//		Delay(5);
//		j = GET_SI_SDI();
//		if(j != 0)
//			zhi |= 0x01;
//		CLR_SI_SCK();
//	}
//	return(zhi);           		  // return read byte
//}
//
///*! ------------------------------------------------------------------------------------------------------------------
// * Function: si_readResponce
// *
// * read response after sending command
// */
//void si_readResponce( uint8 *buffer, uint8 size )
//{
//	wait_cts( );
//	si_writeSPIbyte( READ_CMD_BUFF );
//	SI_NSEL_CLR( );
//    while( size -- )
//    {
//        *buffer++ = SPI_ExchangeByte( 0xFF );
//    }
//    SI_NSEL_SET( );
//}
//
///*! ------------------------------------------------------------------------------------------------------------------
// * Function: si_setPreoperty
// *
// * modify the properties of the SI4463 device
// */
//void si_setPreoperty( SI446X_PROPERTY GROUP_NUM, uint8 NUM_PROPS, uint8 *PAR_BUFF )
//{
//    uint8 cmd[20], i = 0;
//    if( NUM_PROPS >= 16 )   { return; }
//    cmd[i++] = SET_PROPERTY;
//    cmd[i++] = GROUP_NUM>>8;	//	MSB of group number address
//    cmd[i++] = NUM_PROPS;		//	Number of bytes to be set
//    cmd[i++] = GROUP_NUM;		//	LSB of group number address
//    while( NUM_PROPS-- )
//    {
//        cmd[i++] = *PAR_BUFF++;
//    }
//    si_cmd( cmd, i );
//}
//
///*! ------------------------------------------------------------------------------------------------------------------
// * Function: si_setPreoperty1
// *
// * modify the properties of the SI4463 device with NUM_PROPS = 1
// */
//void si_setPreoperty1( SI446X_PROPERTY GROUP_NUM, uint8 proirity )
//{
//    uint8 cmd[5];
//
//    cmd[0] = SET_PROPERTY;
//    cmd[1] = GROUP_NUM>>8;	//	MSB of group number address
//    cmd[2] = 1;
//    cmd[3] = GROUP_NUM;		//	LSB of group number address
//    cmd[4] = proirity;
//    si_cmd( cmd, 5 );
//}
//
///*! ------------------------------------------------------------------------------------------------------------------
// * Function: si_getPreoperty
// *
// * read the properties of the SI4463 device
// */
//void si_getPreoperty( SI446X_PROPERTY GROUP_NUM, uint8 NUM_PROPS, uint8 *buffer  )
//{
//	uint8 cmd[4];
//
//    cmd[0] = GET_PROPERTY;
//    cmd[1] = GROUP_NUM>>8;	//	MSB of group number address
//    cmd[2] = NUM_PROPS;		//	Number of bytes to get
//    cmd[3] = GROUP_NUM;		//	LSB of group number address
//
//    si_cmd( cmd, 4 );
//    si_readResponce( buffer, NUM_PROPS + 1 );
//}
//
///*! ------------------------------------------------------------------------------------------------------------------
// * Function: si_getPreoperty1
// *
// * read the properties of the SI4463 device with NUM_PROPS = 1
// */
//uint8 si_getPreoperty1( SI446X_PROPERTY GROUP_NUM )
//{
//    uint8 cmd[4];
//
//    cmd[0] = GET_PROPERTY;
//    cmd[1] = GROUP_NUM>>8;	//	MSB of group number address
//    cmd[2] = 1;
//    cmd[3] = GROUP_NUM;		//	LSB of group number address
//    si_cmd( cmd, 4 );
//    si_readResponce( cmd, 2 );
//    return cmd[1];
//}
//
///*! ------------------------------------------------------------------------------------------------------------------
// * Function: si_powerUp
// *
// * power up the device and set the oscillator frequency
// * Parameters:
// * --PATCH - Select patch mode.
// * 0 = Copy selected functional image from OTP and boot device.
// * 1 = Indicates a patch has been applied. Validate patched image matches function selected and boot.
// * --FUNC[5:0] - Selects the boot function of the device
// * 0 = Stay in bootload
// * 1 = Boot main application image
// * --TCXO - Select if TCXO is in use.
// * 0 = TCXO not used, XTAL used.
// * 1 = TCXO used.
// * --XO_FREQ[31:0] - Frequency of TCXO
// */
//void si_powerUp( uint32 f_xtal )
//{
//    uint8 cmd[7];
//    cmd[0] = POWER_UP;
//    cmd[1] = 0x01;
//    cmd[2] = 0x00;
//    cmd[3] = f_xtal>>24;
//    cmd[4] = f_xtal>>16;
//    cmd[5] = f_xtal>>8;
//    cmd[6] = f_xtal;
//    si_cmd( cmd, 7 );
//}
//
///*! ------------------------------------------------------------------------------------------------------------------
// * Function: si_partInfo
// *
// * Returns Part Number, Part Version, ROM ID, etc
// */
//void si_partInfo( uint8 *buffer )
//{
//    uint8 cmd = PART_INFO;
//
//    si_cmd( &cmd, 1 );
//    si_readResponce( buffer, 8 );
//
//}
//
///*! ------------------------------------------------------------------------------------------------------------------
// * Function: si_funcInfo
// *
// * Return Function revision numbers for currently loaded functional mode firmware. Contrast with PART_INFO
// */
//void si_funcInfo( uint8 *buffer )
//{
//    uint8 cmd = FUNC_INFO;
//
//    si_cmd( &cmd, 1 );
//    si_readResponce( buffer, 7 );
//}
//
///*! ------------------------------------------------------------------------------------------------------------------
// * Function: si_intStatus
// *
// * Returns the current interrupt status byte.
// */
//void si_intStatus( uint8 *buffer )
//{
//    uint8 cmd[4];
//    cmd[0] = GET_INT_STATUS;
//    cmd[1] = 0;
//    cmd[2] = 0;
//    cmd[3] = 0;
//
//    si_cmd( cmd, 4 );
//    si_readResponce( buffer, 9 );
//
//}
//
///*! ------------------------------------------------------------------------------------------------------------------
// * Function: si_phStatus
// *
// * Returns current packet handler status bytes and possibly clears pending packet handler interrupts.
// */
//void si_phStatus( uint8 *buffer)
//{
//    uint8 cmd;
//    cmd = GET_PH_STATUS;
//    si_cmd( &cmd, 1 );
//    si_readResponce( buffer, 3 );
//
//}
//
///*! ------------------------------------------------------------------------------------------------------------------
// * Function: si_CRCError
// *
// * returns 1 if CRC-32 error is occured
// */
//uint8 si_CRCError(uint8 ph_status)
//{
//	uint8 CRC_error;
//	CRC_error = (0x08 & ph_status) >> 3;
//	return CRC_error;
//}
//
///*! ------------------------------------------------------------------------------------------------------------------
// * Function: si_fifoReset
// *
// * reset both RX and TX FIFO
// */
//void si_fifoReset(void)
//{
//	uint8 *p;
//
//	*(p) = RF_FIFO_INFO;
//	*(p) = 0x03;   // reset tx ,rx fifo
//	si_spi_write(2,p);
//}
//
///*! ------------------------------------------------------------------------------------------------------------------
// * reset both RX FIFO
// */
//void si_RXfifoReset( void )
//{
//    uint8 cmd[2];
//
//    cmd[0] = FIFO_INFO;
//    cmd[1] = 0x02;
//    si_cmd( cmd, 2 );
//}
//
///*! ------------------------------------------------------------------------------------------------------------------
// * reset both TX FIFO
// */
//void si_TXfifoReset( void )
//{
//    uint8 cmd[2];
//
//    cmd[0] = FIFO_INFO;
//    cmd[1] = 0x01;
//    si_cmd(cmd, 2 );
//}
//
///*! ------------------------------------------------------------------------------------------------------------------
// * Function: si_fifoReset
// *
// * write device TX FIFO
// */
//void si_W_TX_FIFO( uint8 *txbuffer, uint8 size )
//{
//    SI_NSEL_CLR( );
//    si_writeSPIbyte( WRITE_TX_FIFO );
//    while( size -- )    { si_writeSPIbyte( *txbuffer++ ); }
//    SI_NSEL_SET( );
//}
//
///*! ------------------------------------------------------------------------------------------------------------------
// * Function: si_sendPacket
// *
// * send message with defined packet size
// */
//void si_sendPacket( uint8 *txbuffer, uint8 size, uint8 channel, uint8 condition )
//{
//    uint8 cmd[5];
//    uint8 tx_len = size;
//    si_fifoReset( ); // clear FIFO
//    SI_NSEL_CLR( );
//    si_writeSPIbyte( WRITE_TX_FIFO );
//#if PACKET_LENGTH == 0
//    tx_len ++;
//    si_writeSPIbyte( size );
//#endif
//    while( size -- )    { si_writeSPIbyte( *txbuffer++ ); }
//    SI_NSEL_SET();
//    cmd[0] = START_TX; //
//    cmd[1] = channel;  //
//    cmd[2] = condition;
//    cmd[3] = 0;
//    cmd[4] = tx_len;
//    si_cmd( cmd, 5 );
//}
//
///*! ------------------------------------------------------------------------------------------------------------------
// * Function: si_readPacket
// *
// * read message with defined packet size
// */
//uint8 si_readPacket( uint8 *buffer )
//{
//    uint8 length, i;
//    wait_cts( );
//    si_writeSPIbyte( READ_RX_FIFO );
//#if SI_PACKET_LENGTH == 0
//    length = SPI_ExchangeByte( 0xFF );
//#else
//    length = SI_PACKET_LENGTH;
//#endif
//    i = length;
//    SI_NSEL_CLR( );
//    while( length -- )
//    {
//        *buffer++ = SPI_ExchangeByte( 0xFF );
//    }
//    SI_NSEL_SET( );
//    return i;
//}
//
///*! ------------------------------------------------------------------------------------------------------------------
// * Function: si_startTX
// *
// * read message with defined packet size
// */
//void si_startTX( uint8 channel, uint8 condition, uint16 tx_len )
//{
//	uint8 cmd[5];
//    cmd[0] = START_TX;
//    cmd[1] = channel;
//    cmd[2] = condition;
//    cmd[3] = tx_len>>8;
//    cmd[4] = tx_len;
//    si_cmd( cmd, 5 );
//    si_current_state = SI_STATE_TX;
//}
//
//void si_startRX( uint8 channel, uint8 condition, uint16 rx_len, uint8 n_state1, uint8 n_state2, uint8 n_state3 )
//{
//    uint8 cmd[8];
//    si_RXfifoReset( );
//    si_TXfifoReset( );
//    cmd[0] = START_RX;
//    cmd[1] = channel;
//    cmd[2] = condition;
//    cmd[3] = rx_len>>8;
//    cmd[4] = rx_len;
//    cmd[5] = n_state1;
//    cmd[6] = n_state2;
//    cmd[7] = n_state3;
//    si_cmd( cmd, 8 );
//    si_current_state = SI_STATE_RX;
//}
//
//
//void si_pktInfo( uint8 *buffer, uint8 FIELD, uint16 length, uint16 diff_len )
//{
//    uint8 cmd[6];
//    cmd[0] = PACKET_INFO;
//    cmd[1] = FIELD;
//    cmd[2] = length >> 8;
//    cmd[3] = length;
//    cmd[4] = diff_len >> 8;
//    cmd[5] = diff_len;
//
//    si_cmd( cmd, 6 );
//    si_readResponce( buffer, 3 );
//}
//
//void si_fifoInfo( uint8 *buffer )
//{
//    uint8 cmd[2];
//    cmd[0] = FIFO_INFO;
//    cmd[1] = 0x03;
//    si_cmd( cmd, 2 );
//    si_readResponce( buffer, 3);
//}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void si_changeState(SI446X_STATE NEW_STATE)
//{
//  uint8 cmd[2];
//  cmd[0] = CHANGE_STATE;
//  cmd[1] = NEW_STATE;
//  si_cmd( cmd, 2 );
//  si_current_state = NEW_STATE;
//}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void si_sleep(void)
//{
//  si_changeState(SI_STATE_SLEEP);
//}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void si_activeSPI(void)
//{
//  si_changeState(SI_STATE_SPI_ACTIVE);
//}
