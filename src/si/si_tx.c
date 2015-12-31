
#include "si_tx.h"

//const unsigned char RF_MODEM_DSA_CTRL1_5_data[] = 		{RF_MODEM_DSA_CTRL1_5};
 unsigned char RF_FREQ_CONTROL_INTE_8_data[] = 		{RF_FREQ_CONTROL_INTE_8};
 unsigned char RF_POWER_UP_data[] = 			   		{ RF_POWER_UP};

 unsigned char RF_FRR_CTL_A_MODE_4_data[] = 		   	{ RF_FRR_CTL_A_MODE_4};

 unsigned char RF_MODEM_FREQ_DEV_0_1_data[] = 		   	{ RF_MODEM_FREQ_DEV_0_1};
 unsigned char RF_MODEM_AGC_CONTROL_1_data[] = 		{ RF_MODEM_AGC_CONTROL_1};
 unsigned char RF_MODEM_MOD_TYPE_12_data[]=			{RF_MODEM_MOD_TYPE_12};
 unsigned char RF_MODEM_TX_RAMP_DELAY_12_data[]=				{RF_MODEM_TX_RAMP_DELAY_12};
 unsigned char BCR_NCO_OFFSET_2_12_data[]=					{RF_MODEM_BCR_NCO_OFFSET_2_12};
 unsigned char RF_MODEM_AFC_LIMITER_1_3_data[]=						{RF_MODEM_AFC_LIMITER_1_3};
 unsigned char AGC_WINDOW_SIZE_12_data[]=				{RF_MODEM_AGC_WINDOW_SIZE_12};
 unsigned char RF_MODEM_RAW_CONTROL_8_data[]=					{RF_MODEM_RAW_CONTROL_8};
 unsigned char COE13_7_0_12_data[]=	{RF_MODEM_CHFLT_RX1_CHFLT_COE13_7_0_12};
 unsigned char COE1_7_0_12_data[]=	{RF_MODEM_CHFLT_RX1_CHFLT_COE1_7_0_12};
 unsigned char COE7_7_0_12_data[]=	{RF_MODEM_CHFLT_RX2_CHFLT_COE7_7_0_12};
 unsigned char RF_SYNTH_PFDCP_CPFF_7_data[]=					{RF_SYNTH_PFDCP_CPFF_7};
 unsigned char RF_MODEM_RAW_SEARCH2_2_data[]={RF_MODEM_RAW_SEARCH2_2};


unsigned char tx_test_aa_data[14] = {0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa};
unsigned char tx_ph_data[14] = {'s','w','w','x',0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x6d};

void setup_si_radio()
{
	unsigned char  i, j, chksum;
	 i=10;

	//__disable_irq();

	sdn_reset();


	//SI4463_init();  // RF init

	//__enable_irq();
	//read part info

	si_partInfo();

	//while(1) si_partInfo();

	if ( spi_read_buf[2] == 0x44 && (spi_read_buf[3] & 0xF0) == 0x60 )
	{
		//while(1) {
							led_on(LED_PC6);
							Sleep(2000);
							led_on(LED_PC7);
							Sleep(2000);
							led_off(LED_PC6);
							Sleep(2000);
							led_off(LED_PC7);
							Sleep(2000);

			//			}


	}

	Sleep(20);

	SI4463_init();  // RF init

	Sleep(1000);

	while( 1 )		//send packet every three seconds
	{

		Sleep(3000);
		for( int j = 0; j < 3; j++)
		{

			 led_off(LED_PC7);
			 //led_on(LED_PC6);
			 Sleep(200);

			 //led_off(LED_PC6);
			 led_on(LED_PC7);
			 Sleep(200);

		}
		tx_data();

		//rf_standby();
	}

	/*count_1hz = 0;
	Flag.reach_1s = 0;

	while(1)
	{
		if(Flag.reach_1s)
		{
			Flag.reach_1s = 0;
			tx_data();			// send packet every 2s
  			rf_standby();		// send over, enter standy Mode
		}
	}*/
}

void SI4463_init(void)
{
	U8 app_command_buf[20];

	//spi_write(0x07, RF_GPIO_PIN_CFG_data);
	app_command_buf[0] = 0x13;			// SET GPIO PORT
	app_command_buf[1]  = 0x14; 		// gpio 0 ,Rx data
	app_command_buf[2]  = 0x02;    		// gpio1, output 0
	app_command_buf[3]  = 0x21;  		// gpio2, hign while in receive mode
	app_command_buf[4]  = 0x20; 		// gpio3, hign while in transmit mode
	app_command_buf[5]  = 0x27;   		// nIRQ
	app_command_buf[6]  = 0x0b;  		// sdo
	spi_write(7, app_command_buf);

	// spi_write(0x05, RF_GLOBAL_XO_TUNE_1_data);
    app_command_buf[0] = 0x11;
	app_command_buf[1]  = 0x00;
	app_command_buf[2]  = 0x01;
	app_command_buf[3]  = 0x00;
	app_command_buf[4]  = 98;  			// freq  adjustment
	spi_write(5, app_command_buf);

	// spi_write(0x05, RF_GLOBAL_CONFIG_1_data);
  	app_command_buf[0] = 0x11;
	app_command_buf[1]  = 0x00;
	app_command_buf[2]  = 0x01;
	app_command_buf[3]  = 0x03;
	app_command_buf[4]  = 0x40;  		// tx = rx = 64 byte,PH,high performance mode
	spi_write(5, app_command_buf);

    spi_write(0x08, RF_FRR_CTL_A_MODE_4_data);    // disable all fast response register

    // spi_write(0x0D, RF_PREAMBLE_TX_LENGTH_9_data); // set Preamble
 	app_command_buf[0] = 0x11;
	app_command_buf[1]  = 0x10;
	app_command_buf[2]  = 0x09;
	app_command_buf[3]  = 0x00;
	app_command_buf[4]  = 0x08;							//  8 bytes Preamble
	app_command_buf[5]  = 0x14;							//  detect 20 bits
	app_command_buf[6]  = 0x00;
	app_command_buf[7]  = 0x0f;
	app_command_buf[8]  = 0x31;  						//  no manchest.1010.¡£¡£
	app_command_buf[9]  = 0x00;
	app_command_buf[10]  = 0x00;
	app_command_buf[11]  = 0x00;
	app_command_buf[12]  = 0x00;
	spi_write(13, app_command_buf);

	//  RF_SYNC_CONFIG_5_data,							// set sync
    app_command_buf[0] = 0x11;
	app_command_buf[1]  = 0x11;
	app_command_buf[2]  = 0x05;
	app_command_buf[3]  = 0x00;
	app_command_buf[4]  = 0x01;   						// no manchest , 2 bytes
	app_command_buf[5]  = 0x2d;   						// sync byte3
	app_command_buf[6]  = 0xd4;							// sync byte2
	app_command_buf[7]  = 0x00;							// sync byte1
	app_command_buf[8]  = 0x00;							// sync byte0
    spi_write(9, app_command_buf);

	//  packet crc
    app_command_buf[0] = 0x11;
	app_command_buf[1]  = 0x12;
	app_command_buf[2]  = 0x01;
	app_command_buf[3]  = 0x00;
	app_command_buf[4]  = 0x81;							// CRC = itu-c, enable crc
    spi_write(5, app_command_buf);

	// packet   gernale configuration
    app_command_buf[0] = 0x11;
	app_command_buf[1]  = 0x12;
	app_command_buf[2]  = 0x01;
	app_command_buf[3]  = 0x06;
	app_command_buf[4]  = 0x02;							// CRC MSB£¬ data MSB
    spi_write(5, app_command_buf);

  	// spi_write(0x07, RF_PKT_LEN_3_data);
    app_command_buf[0] = 0x11;
	app_command_buf[1]  = 0x12;
	app_command_buf[2]  = 0x03;
	app_command_buf[3]  = 0x08;
	app_command_buf[4]  = 0x00;
	app_command_buf[5]  = 0x00;
	app_command_buf[6]  = 0x00;
    spi_write(7, app_command_buf);

	app_command_buf[0] = 0x11;
	app_command_buf[1]  = 0x12;
	app_command_buf[2]  = 0x0c;
	app_command_buf[3]  = 0x0d;
	app_command_buf[4]  = 0x00;
	app_command_buf[5]  = PAYLOAD_LENGTH;
	app_command_buf[6]  = 0x04;
	app_command_buf[7]  = 0xaa;
	app_command_buf[8]  = 0x00;
	app_command_buf[9]  = 0x00;
	app_command_buf[10]  = 0x00;
	app_command_buf[11]  = 0x00;
	app_command_buf[12]  = 0x00;
	app_command_buf[13]  = 0x00;
	app_command_buf[14]  = 0x00;
	app_command_buf[15]  = 0x00;
	spi_write(16, app_command_buf);					// set length of Field 1 -- 4

    // spi_write(0x0C, RF_PKT_FIELD_4_LENGTH_12_8_8_data);
    app_command_buf[0] = 0x11;
	app_command_buf[1]  = 0x12;
	app_command_buf[2]  = 0x08;
	app_command_buf[3]  = 0x19;
	app_command_buf[4]  = 0x00;
	app_command_buf[5]  = 0x00;
	app_command_buf[6]  = 0x00;
	app_command_buf[7]  = 0x00;
	app_command_buf[8]  = 0x00;
	app_command_buf[9]  = 0x00;
	app_command_buf[10]  = 0x00;
	app_command_buf[11]  = 0x00;
    spi_write(12, app_command_buf);

    spi_write(0x10, RF_MODEM_MOD_TYPE_12_data);
	spi_write(0x05, RF_MODEM_FREQ_DEV_0_1_data);

    spi_write(0x10, RF_MODEM_TX_RAMP_DELAY_12_data);
    spi_write(0x10, BCR_NCO_OFFSET_2_12_data);
	spi_write(0x10, RF_MODEM_TX_RAMP_DELAY_12_data);
    spi_write(0x07, RF_MODEM_AFC_LIMITER_1_3_data);
	//spi_write(0x10, BCR_NCO_OFFSET_2_12_data);

    spi_write(0x05, RF_MODEM_AGC_CONTROL_1_data);
    spi_write(0x10, AGC_WINDOW_SIZE_12_data);
    spi_write(0x0c, RF_MODEM_RAW_CONTROL_8_data);
//	spi_write(0x10, AGC_WINDOW_SIZE_12_data);

	// spi_write(0x05, RF_MODEM_RSSI_COMP_1_data);
	app_command_buf[0] = 0x11;
	app_command_buf[1] = 0x20;
	app_command_buf[2] = 0x01;
	app_command_buf[3] = 0x4e;
	app_command_buf[4]  = 0x40;
    spi_write(5, app_command_buf);

    spi_write(0x10, COE13_7_0_12_data);
    spi_write(0x10, COE1_7_0_12_data);
    spi_write(0x10, COE7_7_0_12_data);

	// RF_PA
	app_command_buf[0] = 0x11;
	app_command_buf[1]  = 0x22;
	app_command_buf[2]  = 0x04;
	app_command_buf[3]  = 0x00;
	app_command_buf[4]  = 0x08;
	app_command_buf[5]  = 127;							// set max power
	app_command_buf[6]  =0x00;
	app_command_buf[7]  = 0x3d;
    spi_write(8, app_command_buf);

	spi_write(0x0B, RF_SYNTH_PFDCP_CPFF_7_data);

   	// header match
   	app_command_buf[0] = 0x11;
	app_command_buf[1]  = 0x30;
	app_command_buf[2]  = 0x0c;
	app_command_buf[3]  = 0x00;
	app_command_buf[4]  = 's';
	app_command_buf[5]  = 0xff;
	app_command_buf[6]  = 0x40;
	app_command_buf[7]  = 'w';
	app_command_buf[8]  = 0xff;
	app_command_buf[9]  = 0x01;
	app_command_buf[10] = 'w';
	app_command_buf[11]  =0xff;
	app_command_buf[12]  =0x02;
	app_command_buf[13]  = 'x';
	app_command_buf[14]  = 0xff;
	app_command_buf[15]  =0x03;
    spi_write(16, app_command_buf);

	spi_write(6, RF_MODEM_RAW_SEARCH2_2_data);
    spi_write(12, RF_FREQ_CONTROL_INTE_8_data); 	    // set frequency
}

void fifo_reset(void)			// reset FIFO
{
	U8 p[2];

	p[0] = FIFO_INFO;
	p[1] = 0x03;   // reset tx ,rx fifo
	spi_write(2,p);
}

void clr_interrupt(void)
{
// since you are clearning ALL the interrupts before reading , doesnt this mean that every time the read response will be zero ??
//what is the purpose of spi_read(9,GET_INT_STATUS); here.. My understandng is that it will always read zero. because all the interrupts
//are cleared by spi_write(4,p); command
	U8 p[4];

	spi_read(9,GET_INT_STATUS);
	p[0] = GET_INT_STATUS;
	p[1] = 0;   // clr  PH pending
	p[2] = 0;   // clr modem_pending
	p[3] = 0;   // clr chip pending
	spi_write(4,p);
}


void enable_tx_interrupt(void)
{
	U8 p[6];

	p[0] = 0x11;  //SET_PROPERTY command
	p[1] = 0x01;  // 0x0100
	p[2] = 0x02;   // 2 properties to be set
	p[3] = 0x00;  // 0x0100		start property
	p[4] = 0x01;   // Interrupt enable property value,
	p[5] = 0x20; //  enable  packet sent interrupt
	spi_write(0x06, p);
}

void enable_rx_interrupt(void)
{
	U8 p[7];

	p[0] = 0x11;
	p[1] = 0x01;  // 0x0100
	p[2] = 0x03;  // 3 parameters
	p[3] = 0x00;   // 0100
	p[4] = 0x03;   // ph, modem int
	p[5] = 0x18; // 0x10;   // Pack received int
	p[6] = 0x00;   //preamble int, sync int setting
	spi_write(0x07, p);  // enable  packet receive interrupt
}


void rf_standby(void)
{
	U8 p[2];

	p[0] = CHANGE_STATE ; 	// CHANGE_STATE CMD
	p[1] = 0x01 ; 			// sleep mode
	spi_write(2, p);
}

void tx_start(void)
{
	U8 p[5];

	p[0] = START_TX ; // start CMD
	p[1] = FREQ_CHANNEL ; // channel 0
	p[2] = 0x30; // return Ready mode after TX
	p[3] = 0;
	p[4] = 0; //payload_length; 10 BYTES
	spi_write(5, p);
}

void tx_data(void)
{
	unsigned char i;

	//Flag.is_tx = 1;
	//LED_RED ^= 1;

	fifo_reset();  // clr fifo

	spi_write_fifo();  // load data to fifo

	enable_tx_interrupt();

	clr_interrupt();  // clr INT Factor

	tx_start();    // enter TX mode, sent data


	//TODO: wait for packet sent interrupt

	//clr_interrupt();

			//check for PACKET_SENT interrupt flag
	do
	{
		Sleep(10);
		//clr_interrupt();
		spi_read(9, GET_INT_STATUS);

	} while ( (spi_read_buf[4] & 0x20) != 0x20 );

	//clr_interrupt();

	//rf_timeout = 0;

	//Flag.rf_reach_timeout = 0;

	//while(nIRQ)		// wait INT
	//{
		//wdtr = 0;
		//wdtr = 0xff;
	//	if(Flag.rf_reach_timeout)
	//	{
	//		sdn_reset();
 	//		SI4463_init();  // RF timeout,need init RF
	//		break;
	//	}
	//}
}

void rx_init(void)
{
	//Flag.is_tx = 0;

	fifo_reset();  // clr fifo

	enable_rx_interrupt();

	clr_interrupt();  // clr INT Factor

	rx_start();
}

void rx_start(void)
{
	U8 p[8];

	p[0] = START_RX ;
	p[1] = FREQ_CHANNEL ; // channel 0
	p[2] = 0x00;
	p[3] = 0;
	p[4] = 0;
	p[5] = 0;  // unchanged after preamble timeout
	p[6] = 0x08;
	p[7] = 0x08;
	spi_write(8, p);
}


U8 spi_byte(U8 data)
{
	/*unsigned char i;

	for (i = 0; i < 8; i++)
	{
		if (data & 0x80)
			SDI = 1;
		else
			SDI = 0;

		data <<= 1;
		SCK = 1;

		if (SDO)
			data |= 0x01;
		else
			data &= 0xfe;

		SCK = 0;
	}*/

	//CLR_SI_SCK();

	//SI_NSEL_CLR();

	SI_SPI->DR = data;
	while ((SI_SPI->SR & SPI_I2S_FLAG_RXNE) == (uint16_t)RESET);
	data = SI_SPI->DR ;

//	if (data == 0x44)
//	{
//
//		while(1) {
//			led_on(LED_PC6);
//			Sleep(1000);
//			led_on(LED_PC7);
//			Sleep(1000);
//			led_off(LED_PC6);
//			Sleep(1000);
//			led_off(LED_PC7);
//			Sleep(1000);
//
//		}
//
//	}

	//SI_NSEL_SET();

	return (data);
}

void spi_write(U8 tx_length, U8 *p)
{
	U8 i,j;

	i = 0;
	while ( i != 0xFF )
		i = check_cts();

	//SCK = 0;

	//nSEL = 0;
	SI_NSEL_CLR();

	for (i = 0; i < tx_length; i++)
	{
		j = *(p+i);
		spi_byte(j);
	}

	//nSEL = 1;
	SI_NSEL_SET();
}

U8 check_cts(void)
{
	U8 i;

	//nSEL = 1;
	//SCK = 0;
	//nSEL = 0;
	SI_NSEL_CLR();

	spi_byte(0x44);

	i = spi_byte(0x00);

	//nSEL = 1;
	SI_NSEL_SET();

	return (i);
}

void spi_read(U8 data_length, U8 api_command )
{
	uint16 i;

	U8 p[1];
	p[0] = api_command;
	i = 0;

/*	for( int j = 0; j < 8; j++)
{

	 led_off(LED_PC7);
	 //led_on(LED_PC6);
	 Sleep(200);

	 //led_off(LED_PC6);
	 led_on(LED_PC7);
	 Sleep(200);

}*/
//led_off(LED_ALL);


	while(i != 0xFF )
		i = check_cts();

	spi_write(1, p);


	i = 0;
	while(i != 0xFF )
		i = check_cts();

	//nSEL = 1;
	//SCK = 0;
	//nSEL = 0;
	SI_NSEL_CLR();
	//toggle_to_rst_nSEL();

	spi_byte(0x44);

	for (i = 0; i< data_length; i++)
	{
		spi_read_buf[i] = spi_byte(0xFF);

//		if ( spi_read_buf[i] > 0 )
//		{
//		    while(1) {
//		   				led_on(LED_PC7);
//		    				Sleep(1000);
//
//		    				led_off(LED_PC7);
//		    				Sleep(1000);
//
//		    			}
//		}
	}

	//nSEL = 1;
	SI_NSEL_SET();
}

void spi_write_fifo(void)
{
	U8 i;

	i = 0;
	while(i!=0xff)
		i = check_cts();

	//nSEL = 1;
	//SCK = 0;
	//nSEL = 0;
	toggle_to_rst_nSEL();

	spi_byte(WRITE_TX_FIFO);

	//if(mode == tx_test_mode)
	//{
	//	for (i = 0; i< PAYLOAD_LENGTH; i++)
	//	{
	//		spi_byte(tx_test_aa_data[i]);
	//	}
	//}
	//else
	//{
		for (i = 0; i< PAYLOAD_LENGTH; i++)
		{
			spi_byte(tx_ph_data[i]);
		}
	//}

	//nSEL = 1;
	SI_NSEL_SET();
}

void spi_read_fifo(void)
{
	U8 i;

	i = 0;
	while(i!=0xff)
		i = check_cts();

	//nSEL = 1;
	//SCK = 0;
	//nSEL = 0;
	toggle_to_rst_nSEL();

	spi_byte(READ_RX_FIFO);

	for (i = 0; i< PAYLOAD_LENGTH; i++)
		rx_buf[i] = spi_byte(0xff);

	//nSEL = 1;
	SI_NSEL_SET();
}

void sdn_reset(void)
{
	int i;

 	SET_SI_SDN();
 	Sleep(10);
	for( i = 0; i < 4; i++)
{

	 led_off(LED_PC7);
	 led_on(LED_PC6);
	 Sleep(300);

	 led_off(LED_PC6);
	 led_on(LED_PC7);
	 Sleep(300);

}
led_off(LED_ALL);
 	//delay_1ms(2);
 	//SDN = 0;
 	//delay_1ms(10);

 	CLR_SI_SDN();

 	Sleep(500);
	for( i = 0; i < 4; i++)
{

	 led_off(LED_PC7);
	 led_on(LED_PC6);
	 Sleep(300);

	 led_off(LED_PC6);
	 led_on(LED_PC7);
	 Sleep(300);

}
led_off(LED_ALL);
 	//nSEL = 1;
	//SCK = 0;
	//nSEL = 0;
 	toggle_to_rst_nSEL();

 	for (i = 0; i < 7; i++)
		spi_byte(RF_POWER_UP_data[i]);

 	//nSEL = 1;
 	SI_NSEL_SET();

 	Sleep(500);
}

/*! ------------------------------------------------------------------------------------------------------------------
 * Function: si_partInfo
 *
 * Returns Part Number, Part Version, ROM ID, etc
 */
void si_partInfo()
{

    spi_read( 9, PART_INFO );

}

void toggle_to_rst_nSEL()
{
	SI_NSEL_SET();
	Sleep(1);
	SI_NSEL_CLR();
	Sleep(1);
}


