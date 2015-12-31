/*
 * si_tx.h
 *
 *  Created on: 17 Dec 2015
 *      Author: rm
 */

#ifndef SRC_SI_SI_TX_H_
#define SRC_SI_SI_TX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "port.h"
#include "radio_config_si4463.h"

typedef 	uint8	U8;
typedef 	uint16	U16;
typedef 	unsigned long	U32;
typedef 	char			S8;
typedef 	int				S16;
typedef 	long			S32;

#define  	SDI        		SI_MOSI
#define  	nSEL			SI_CS
#define  	LED_GREEN		LED_PC6
#define  	LED_RED			LED_PC7
#define  	SCK				SI_SCK
//#define  	nIRQ       		SI_IRQ
#define  	SDN	        	SI_SDN
#define  	SDO	        	SI_MISO


#define NOP 							0x00
#define PART_INFO                       0x01
#define FUNC_INFO                       0x10
#define SET_PROPERTY                    0x11
#define GET_PROPERTY                    0x12
#define GPIO_PIN_CFG                    0x13
#define GET_ADC_READING                 0x14
#define FIFO_INFO                       0x15
#define PACKET_INFO                     0x16
#define IRCAL                           0x17
#define PROTOCOL_CFG                    0x18
#define GET_INT_STATUS                  0x20
#define GET_PH_STATUS                   0x21
#define GET_MODEM_STATUS                0x22
#define GET_CHIP_STATUS                 0x23
#define START_TX                        0x31
#define START_RX                        0x32
#define REQUEST_DEVICE_STAT             0x33
#define CHANGE_STATE                    0x34
#define READ_CMD_BUFF                   0x44
#define FRR_A_READ                      0x50
#define FRR_B_READ                      0x51
#define FRR_C_READ                      0x53
#define FRR_D_READ                      0x57
#define WRITE_TX_FIFO                   0x66
#define READ_RX_FIFO                    0x77
#define START_MFSK                      0x35
#define RX_HOP                          0x36

#define PAYLOAD_LENGTH  				14

#define FREQ_CHANNEL					0

#define step_500K_step1		0x88    // register setting for frequency 500KHz step
#define step_500K_step0		0x89	// register setting for frequency 500KHz step

typedef struct
{
	unsigned char reach_1s				: 1;
	unsigned char is_tx					: 1;
	unsigned char rf_reach_timeout		: 1;
}FlagType;



FlagType	Flag;

U16	count_1hz, rf_timeout;
U8 spi_read_buf[20];
U8 rx_buf[25];
U8 mode;

#define CLR_SI_SCK() 		GPIO_ResetBits(SI_GPIO, SI_SCK)
#define SET_SI_SCK()      	GPIO_SetBits(SI_GPIO, SI_SCK)
#define GET_SI_SCK()      	GPIO_ReadInputDataBit(SI_GPIO, SI_SCK)

//#define CLR_SI_IRQ()      	GPIO_ResetBits(SI_GPIO, SI_IRQ)
//#define SET_SI_IRQ()      	GPIO_SetBits(SI_GPIO, SI_IRQ)
//#define GET_SI_IRQ()       	GPIO_ReadInputDataBit(SI_GPIO, SI_IRQ)

#define SI_NSEL_CLR()      	{ GPIO_ResetBits(SI_GPIO, SI_CS); Sleep(5); }
#define SI_NSEL_SET()      	{ GPIO_SetBits(SI_GPIO, SI_CS); Sleep(5); }

#define CLR_SI_SDO()		GPIO_ResetBits(SI_GPIO, SI_MISO)
#define SET_SI_SDO()		GPIO_SetBits(SI_GPIO, SI_MISO)

#define GET_SI_SDI()		GPIO_ReadInputDataBit(SI_GPIO, SI_MOSI)

#define CLR_SI_SDN()		GPIO_ResetBits(SI_GPIO, SI_SDN)
#define SET_SI_SDN()      	GPIO_SetBits(SI_GPIO, SI_SDN)

void setup_si_radio(void);
void si_partInfo(void);

void spi_read(U8 data_length, U8 api_command );
void tx_data(void);
void SI4463_init(void);


unsigned char spi_byte(unsigned char data);
U8 check_cts(void);
void spi_write(unsigned char tx_length, unsigned char *p);
void spi_write_fifo(void);
void spi_read_fifo(void);
void sdn_reset(void);
void clr_interrupt(void);
void fifo_reset(void);
void enable_tx_interrupt(void);
void enable_rx_interrupt(void);
void tx_start(void);
void rx_start(void);
void rx_init(void);
void rf_standby(void);

void toggle_to_rst_nSEL(void);

#ifdef __cplusplus
}
#endif

#endif /* SRC_SI_SI_TX_H_ */
