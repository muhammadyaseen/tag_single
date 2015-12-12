///*
// * si4463.h
// *
// *  Created on: 6 Apr 2015
// *      Author: KFRL/SM
// */
//
//#ifndef SI4463_H_
//#define SI4463_H_
//
//#include "port.h"
//#include "radio_config_Si4461.h"
//#include "SI446X_defs.h"
//
//#define  SI_PACKET_LENGTH      	4 //0-64, if = 0: variable mode, else: fixed mode
//#define  SI_ALERT_MESSAGE	{0x55, 0xAA, 0x99, 0x7E}
//#define  SI_CLEAR_MESSAGE	{0x24, 0x42, 0x24, 0x42}
//#define  SI_CHANNEL				0
//#define  SI_CONDITION			0
//
//#define RF_NOP 					0x00
//#define RF_PART_INFO            0x01 // 9
//#define RF_FUNC_INFO            0x10 // 7
//#define RF_SET_PROPERTY         0x11
//#define RF_GET_PROPERTY         0x12
////#define RF_GPIO_PIN_CFG         0x13    // 8
//#define RF_GET_ADC_READING      0x14
//#define RF_FIFO_INFO            0x15    // 3
//#define RF_PACKET_INFO          0x16    // 3
//#define RF_IRCAL                0x17
//#define RF_PROTOCOL_CFG         0x18
//#define RF_GET_INT_STATUS       0x20    // 9
//#define RF_GET_PH_STATUS        0x21    // 3
//#define RF_GET_MODEM_STATUS     0x22    // 9
//#define RF_GET_CHIP_STATUS      0x23     // 4
//#define RF_START_TX             0x31
//#define RF_START_RX             0x32
//#define RF_REQUEST_DEVICE_STAT  0x33   // 3
//#define RF_CHANGE_STATE         0x34
//#define RF_READ_CMD_BUFF        0x44
//#define RF_FRR_A_READ           0x50   // 4
//#define RF_FRR_B_READ           0x51
//#define RF_FRR_C_READ           0x53
//#define RF_FRR_D_READ           0x57
//#define RF_WRITE_TX_FIFO        0x66
//#define RF_READ_RX_FIFO         0x77
//#define RF_START_MFSK           0x35
//#define RF_RX_HOP               0x36
//
//#define CLR_SI_SCK() 		GPIO_ResetBits(SI_GPIO, SI_SCK)
//#define SET_SI_SCK()      	GPIO_SetBits(SI_GPIO, SI_SCK)
//#define DWZ_SI_SCK()      	GPIO_ReadInputDataBit(SI_GPIO, SI_SCK)
//
//#define CLR_SI_IRQ()      	GPIO_ResetBits(SI_GPIO, SI_IRQ)
//#define SET_SI_IRQ()      	GPIO_SetBits(SI_GPIO, SI_IRQ)
//#define RF4463_IRQ()       	GPIO_ReadInputDataBit(SI_GPIO, SI_IRQ)
//
//#define SI_NSEL_CLR()      	GPIO_ResetBits(SI_GPIO, SI_CS)
//#define SI_NSEL_SET()      	GPIO_SetBits(SI_GPIO, SI_CS)
//
//#define CLR_SI_SDO()		GPIO_ResetBits(SI_GPIO, SI_CS)
//#define SET_SI_SDO()		GPIO_ResetBits(SI_GPIO, SI_CS)
//
//#define GET_SI_SDI()		GPIO_ReadInputDataBit(SI_GPIO, SI_MISO)
//
//#define payload_length  	14      //payload length = 4 (header) + 10 (payload)
//#define step_500K_step1		0x88    // register setting for frequency 500KHz step
//#define step_500K_step0		0x89	// register setting for frequency 500KHz step
//
////#define FRR_CTL_A_MODE_4_LEN 	8
////uint8 FRR_CTL_A_MODE_4[FRR_CTL_A_MODE_4_LEN] = {0x11, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00};
//
//#define  MODEM_MOD_TYPE_12_LEN 		       16
//#define  MODEM_TX_RAMP_DELAY_8_LEN         12
//#define  MODEM_BCR_OSR_1_9_LEN		       13
//#define  MODEM_AFC_GEAR_7_LEN		       11
//#define  MODEM_AGC_WINDOW_SIZE_9_LEN       13
//#define  MODEM_OOK_CNT1_11_LEN		       15
//#define  MODEM_CHFLT_RX1_COE13_7_0_12_LEN  16
//#define  MODEM_CHFLT_RX1_COE1_7_0_12_LEN   16
//#define  MODEM_CHFLT_RX2_COE7_7_0_12_LEN   16
//#define  SYNTH_PFDCP_CPFF_7_LEN			   11
//#define  MODEM_AGC_CONTROL_1_LEN	       5
//
//// as used in RF6433Pro library
//
////uint8 MODEM_AGC_CONTROL_1 		  [MODEM_AGC_CONTROL_1_LEN		   ]={0x11,0x20,0x01,0x35,0xE2}                                                       ;
////uint8 MODEM_MOD_TYPE_12 		  [MODEM_MOD_TYPE_12_LEN		   ]={0x11,0x20,0x0C,0x00,0x03,0x00,0x07,0x00,0x12,0xC0,0x04,0x2D,0xC6,0xC0,0x00,0x00};
////uint8 MODEM_TX_RAMP_DELAY_8       [MODEM_TX_RAMP_DELAY_8_LEN	   ]={0x11,0x20,0x08,0x18,0x01,0x80,0x08,0x03,0x80,0x00,0x70,0x20}                    ;
////uint8 MODEM_BCR_OSR_1_9		      [MODEM_BCR_OSR_1_9_LEN		   ]={0x11,0x20,0x09,0x22,0x03,0x0D,0x00,0xA7,0xC6,0x00,0x54,0x02,0xC2}               ;
////uint8 MODEM_AFC_GEAR_7		      [MODEM_AFC_GEAR_7_LEN			   ]={0x11,0x20,0x07,0x2C,0x04,0x36,0x80,0x03,0x30,0xAF,0x80}                         ;
////uint8 MODEM_AGC_WINDOW_SIZE_9     [MODEM_AGC_WINDOW_SIZE_9_LEN     ]={0x11,0x20,0x09,0x38,0x11,0xAB,0xAB,0x00,0x1A,0x14,0x00,0x00,0x2B}               ;
////uint8 MODEM_OOK_CNT1_11		      [MODEM_OOK_CNT1_11_LEN		   ]={0x11,0x20,0x0B,0x42,0xA4,0x02,0xD6,0x83,0x00,0xAD,0x01,0x80,0xFF,0x0C,0x00}     ;
////uint8 MODEM_CHFLT_RX1_COE13_7_0_12[MODEM_CHFLT_RX1_COE13_7_0_12_LEN]={0x11,0x21,0x0C,0x00,0xFF,0xC4,0x30,0x7F,0xF5,0xB5,0xB8,0xDE,0x05,0x17,0x16,0x0C};
////uint8 MODEM_CHFLT_RX1_COE1_7_0_12 [MODEM_CHFLT_RX1_COE1_7_0_12_LEN ]={0x11,0x21,0x0C,0x0C,0x03,0x00,0x15,0xFF,0x00,0x00,0xFF,0xC4,0x30,0x7F,0xF5,0xB5};
////uint8 MODEM_CHFLT_RX2_COE7_7_0_12 [MODEM_CHFLT_RX2_COE7_7_0_12_LEN ]={0x11,0x21,0x0C,0x18,0xB8,0xDE,0x05,0x17,0x16,0x0C,0x03,0x00,0x15,0xFF,0x00,0x00};
////uint8 SYNTH_PFDCP_CPFF_7		  [SYNTH_PFDCP_CPFF_7_LEN		   ]={0x11,0x23,0x07,0x00,0x2C,0x0E,0x0B,0x04,0x0C,0x73,0x03}                         ;
//
//void SI4463_init(void);
//void si_spi_write (uint16 length,uint8 *buffer);
//
//int si_writetospi_packet(uint16 headerLength, const uint8 *headerBuffer, uint32 bodylength, const uint8 *bodyBuffer);
//uint8 SPI_ExchangeByte(uint8 byte);
//void si_writeSPIbyte(uint8 cmd);
//uint8 si_readSPIbyte();
//void si_readResponce( uint8 *buffer, uint8 size );
//
//void wait_cts(void);
//
//void si_cmd( uint8 *cmd, uint8 cmdsize );
//
//void si_powerUp( uint32 f_xtal );
//
//void si_setPreoperty( SI446X_PROPERTY GROUP_NUM, uint8 NUM_PROPS, uint8 *PAR_BUFF );
//void si_setPreoperty1( SI446X_PROPERTY GROUP_NUM, uint8 proirity );
//uint8 si_getPreoperty1( SI446X_PROPERTY GROUP_NUM );
//void si_getPreoperty( SI446X_PROPERTY GROUP_NUM, uint8 NUM_PROPS, uint8 *buffer  );
//// =================== INFO ========================
//void si_partInfo( uint8 *buffer );
//void si_funcInfo( uint8 *buffer );
//void si_intStatus( uint8 *buffer );
//void si_phStatus( uint8 *buffer);
//uint8 si_CRCError(uint8);
//// =================== FIFO ========================
//void si_fifoReset(void);
//void si_RXfifoReset( void );
//void si_TXfifoReset( void );
//void si_W_TX_FIFO( uint8 *txbuffer, uint8 size );
//// =================== TX/ RX ======================
//void si_sendPacket( uint8 *txbuffer, uint8 size, uint8 channel, uint8 condition );
//uint8 si_readPacket( uint8 *buffer );
//void si_startTX( uint8 channel, uint8 condition, uint16 tx_len );
//void si_startRX( uint8 channel, uint8 condition, uint16 rx_len, uint8 n_state1, uint8 n_state2, uint8 n_state3 );
//
//void si_pktInfo( uint8 *buffer, uint8 FIELD, uint16 length, uint16 diff_len );
//void si_fifoInfo( uint8 *buffer );
//
//// =============== STATE_MACHINE ===================
//SI446X_STATE si_current_state;
//void si_changeState(SI446X_STATE NEW_STATE);
//void si_sleep(void);
//void si_activeSPI(void);
//
//#endif
