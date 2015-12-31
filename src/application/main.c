/*! ----------------------------------------------------------------------------
 *  @file    main.c
 *  @brief   main loop for the Smart Safety Vest Design for Warehouse Worker - Tag device in worker's jacket
 *
 *	Developed by: KFRL / Syed Mustafa Kazmi
 *	Start Date: 15/5/2015
 *
 */
/* Includes */
#include <data_types.h>
#include "compiler.h"
#include <stm32f10x_usart.h>
#include "port.h"

#include "instance.h"

#include "deca_spi.h"
#include "../si/si_tx.h"

int instance_anchaddr = 0; //0 = 0xDECA020000000001; 1 = 0xDECA020000000002; 2 = 0xDECA020000000003
//NOTE: switches TA_SW1_7 and TA_SW1_8 are used to set tag/anchor address
//if instance_mode = TAG_TDOA then the device cannot be selected as anchor
//int instance_mode = TAG;
//int instance_mode = TAG;
//int instance_mode = TAG_TDOA;
//int instance_mode = LISTENER;


uint32 inittestapplication(void);

// Restart and re-configure
void restartinstance(void)
{
    instance_close() ;                          //shut down instance, PHY, SPI close, etc.

    spi_peripheral_init();                      //re initialise SPI...

    inittestapplication() ;                     //re-initialise instance/device
} // end restartinstance()


uint32 inittestapplication(void)
{
    uint32 devID ;
    int i , result;
    led_off(LED_ALL);
    SPI_ConfigFastRate(SPI_BaudRatePrescaler_32);  //max SPI before PLLs configured is ~4M

    //i = 10;

    //this is called here to wake up the device (i.e. if it was in sleep mode before the restart)
    devID = instancereaddeviceid() ;
    if((DWT_DEVICE_ID)&(0xFFFF0000) != (devID&0xFFFF0000)) //if the read of devide ID fails, the DW1000 could be asleep
    {
        port_SPIx_clear_chip_select();  //CS low
        Sleep(1);   //200 us to wake up then waits 5ms for DW1000 XTAL to stabilise
        port_SPIx_set_chip_select();  //CS high
        Sleep(7);
        devID = instancereaddeviceid() ;
        // SPI not working or Unsupported Device ID
        if((DWT_DEVICE_ID)&(0xFFFF0000) != (devID&0xFFFF0000))
        {
        	led_on(LED_PC7);
        	Sleep(1400);
        	return (-1) ;
        }
            //clear the sleep bit - so that after the hard reset below the DW does not go into sleep
        dwt_softreset();
    }

    //reset the DW1000 by driving the RSTn line low
    reset_DW1000();

    result = instance_init() ;

    if (0 > result) {
    	i=15;
		while(i--)
		{
			if (i & 1) {
				//led_off(LED_PC6);
				led_on(LED_PC6);
				led_off(LED_PC7);
			}
			else{
				//led_on(LED_PC6);
				led_off(LED_PC6);
				led_on(LED_PC7);
			}

			Sleep(500);
		}

			i = 0;

			Sleep(500);
			led_on(LED_ALL);
			Sleep(1400);


    	return (-1) ; // Some failure has occurred
    }

    SPI_ConfigFastRate(SPI_BaudRatePrescaler_4); //increase SPI to max
    devID = instancereaddeviceid() ;

    if ((DWT_DEVICE_ID)&(0xFFFF0000) != (devID&0xFFFF0000))   // Means it is NOT MP device
    {
        // SPI not working or Unsupported Device ID

    	led_on(LED_PC6);
    	Sleep(1400);
        return (-1) ;
    }

	//led_on(LED_PC7);

    instance_data[0].mode =  TAG;                   // set the role

	instance_init_f(); //initialise Fast 2WR specific data
	//when using fast ranging the channel config is either mode 2 or mode 6
	//default is mode 2


    instance_config() ;                  // Set operating channel etc

#if (DR_DISCOVERY == 0)
    addressconfigure() ;                            // set up initial payload configuration
#endif
    instancesettagsleepdelay(POLL_SLEEP_DELAY, BLINK_SLEEP_DELAY); //set the Tag sleep time

    //if TA_SW1_2 is on use fast ranging (fast 2wr)
    //if(is_fastrng_on(0) == S1_SWITCH_ON)
    //{
        //Fast 2WR specific config
        //configure the delays/timeouts
        instance_config_f();
    //}
    /*else //use default ranging modes
    {
        // NOTE: this is the delay between receiving the blink and sending the ranging init message
        // The anchor ranging init response delay has to match the delay the tag expects
        // the tag will then use the ranging response delay as specified in the ranging init message
        // use this to set the long blink response delay (e.g. when ranging with a PC anchor that wants to use the long response times != 150ms)
    	if(is_switch_on(TA_SW1_8) == S1_SWITCH_ON)
        {
            instancesetblinkreplydelay(FIXED_LONG_BLINK_RESPONSE_DELAY);
        }
        else //this is for ARM to ARM tag/anchor (using normal response times 150ms)
        {
            instancesetblinkreplydelay(FIXED_REPLY_DELAY);
        }

#if (DR_DISCOVERY == 1)
        //set the default response delays
        instancesetreplydelay(FIXED_REPLY_DELAY, 0);
#else
        //set the default response delays
        instancesetreplydelay(FIXED_REPLY_DELAY_MULTI, 0);
#endif
    }*/

    return devID;
}
/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/
void process_dwRSTn_irq(void)
{
    instance_notify_DW1000_inIDLE(1);
}

void process_deca_irq(void)
{
    do{

        instance_process_irq(0);

    }while(port_CheckEXT_IRQ() == 1); //while IRS line active (ARM can only do edge sensitive interrupts)

}

void init_dw(void)
{

	GPIO_SetBits(DW_GPIO, DW_PWR);
	Sleep(1000);

}
/*
 * @fn      main()
 * @brief   main entry point
**/
int main(void)
{
    int i = 0;
    int toggle = 1;
    int ranging = 0;
    uint8 dataseq[40];
    double range_result = 0;
    double avg_result = 0;
    uint8 dataseq1[40];

    led_off(LED_ALL); //turn off all the LEDs

    peripherals_init();

    spi_peripheral_init();

    Sleep(1000); //wait for LCD to power on

    //initLCD();

    //init_dw();

    //UART Loopback
//    while(1)
//    {
//		led_on(LED_PC7);
//		Sleep(500);
//		led_off(LED_PC7);
//		Sleep(500);
//
//		while(1)
//		{
//		while (USART_GetFlagStatus(USART1 , USART_FLAG_TXE) == RESET);
//
//		USART_SendData(USART1, 0x41);
//		}

//		USART_SendData(USART1, 0x37);
//
//		while (USART_GetFlagStatus(USART1 , USART_FLAG_TXE) == RESET);
//
//		USART_SendData(USART1, 0x35);
//
//		while (USART_GetFlagStatus(USART1 , USART_FLAG_TXE) == RESET);
//
//		Sleep(500);

//		while (USART_GetFlagStatus(USART1 , USART_FLAG_RXNE) == RESET);
//
//		uint8 rxbyte = USART_ReceiveData(USART1);
//
//		if ( rxbyte == 0x15 )
//		{
//				led_on(LED_PC6);
//				Sleep(500);
//				led_off(LED_PC6);
//				Sleep(500);
//		}
//    }
    role_btn_set(DISABLE);//emergency button settings


    port_DisableEXT_IRQ(); //disable ScenSor IRQ until we configure the device



	 i=5;
	 while(i--)
	 {
		 if (i & 1) {
			led_off(LED_PC6);
			led_on(LED_PC7);
			//led_off(LED_PC7);
		 }
		 else{
			led_on(LED_PC6);
			led_off(LED_PC7);
			//led_on(LED_PC7);
		 }
		 Sleep(100);
	 }

	 led_off(LED_ALL);

	 //__disable_irq();

	//setup_si_radio();

    //__enable_irq();

    led_on(LED_ALL);

    Sleep(2000);

    //this 'if' is for pc software, i am commenting this bcz we dont need to run deca ranging on PC
    if(false)
    {

    }
    else //run DecaRanging application on ARM board
    {
        led_off(LED_ALL);

        if(inittestapplication() == (uint32)-1) //test SPI and dw1000 device is working or not
        {
            led_on(LED_ALL); //to display error....
            return 0; //error
        }
        else	//init test was successfull.. Blink LEDs to indicate
        {
        	 i=20;
			 while(i--)
			 {
				 if (i & 1) {
					led_off(LED_PC6);
					led_on(LED_PC7);
					//led_off(LED_PC7);
				 }
				 else{
					led_on(LED_PC6);
					led_off(LED_PC7);
					//led_on(LED_PC7);
				 }
				 Sleep(100);
			 }

			 i = 0;
        	 led_off(LED_ALL);
        }
    }

    led_off(LED_ALL);

    // main loop
    port_EnableEXT_IRQ(); //enable ScenSor IRQ before starting
	role_btn_set(ENABLE); //configures interrupt

	//IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    //IWDG_SetPrescaler(IWDG_Prescaler_16);
    //IWDG_Enable();

    while(1)
    {
    	//IWDG_ReloadCounter();

		role_btn_set(DISABLE);

		instance_run();							//ranging routine

		//Sleep(500);

#ifdef SAFETY_VEST
			//si_sendAlertMSG();
#endif

		role_btn_set(ENABLE);

		if(instancenewrange())
		{
			//while(1);

			int l = 0, txl = 0, rxl = 0, aaddr, taddr;
			ranging = 1;
			//send the new range information to LCD and/or USB
			range_result = instance_get_idist();
			avg_result = instance_get_adist();
			//set_rangeresult(range_result);

			//dataseq[0] = 0x2 ;  //return cursor home
			//writetoLCD( 1, 0,  dataseq);

			//memset(dataseq, ' ', 40);
			//memset(dataseq1, ' ', 40);
			//sprintf((char*)&dataseq[1], "LAST: %4.2f m", range_result);
			//writetoLCD( 40, 1, dataseq); //send some data

			//sprintf((char*)&dataseq1[1], "AVG8: %4.2f m", avg_result);
			//writetoLCD( 16, 1, dataseq1); //send some data

		}

		if(ranging == 0)
		{
			if(instancesleeping())
			{
				dataseq[0] = 0x2 ;  //return cursor home
				//writetoLCD( 1, 0,  dataseq);
				if(toggle)
				{
					toggle = 0;
					//memcpy(&dataseq[0], (const uint8 *) "    AWAITING    ", 16);
					//writetoLCD( 40, 1, dataseq); //send some data
					//memcpy(&dataseq[0], (const uint8 *) "    RESPONSE    ", 16);
					//writetoLCD( 16, 1, dataseq); //send some data
				}
				else
				{
					toggle = 1;
					//memcpy(&dataseq[2], (const uint8 *) "   TAG BLINK    ", 16);
					//writetoLCD( 40, 1, dataseq); //send some data
					//sprintf((char*)&dataseq[0], "%llX", instance_get_addr());
					//writetoLCD( 16, 1, dataseq); //send some data
				}

				//#ifdef USB_SUPPORT //this is set in the port.h file
				//send_usbmessage(&dataseq[0], 35);
				//#endif

			}

				if(instanceanchorwaiting() == 2)
				{
					ranging = 1;
					//dataseq[0] = 0x2 ;  //return cursor home
					//writetoLCD( 1, 0,  dataseq);
					//memcpy(&dataseq[0], (const uint8 *) "    RANGING WITH", 16);
					//writetoLCD( 40, 1, dataseq); //send some data
					//sprintf((char*)&dataseq[0], "%016llX", instance_get_anchaddr());
					//writetoLCD( 16, 1, dataseq); //send some data
				}
			}

    	/*else
    	{
    		//Sleep(1000);
			dataseq[0] = 0x2 ;  //return cursor home
			//writetoLCD( 1, 0,  dataseq);
			memcpy(dataseq, (const uint8 *) "   Waiting for  ", 16);
			//writetoLCD( 40, 1, dataseq); //send some data
			memcpy(dataseq, (const uint8 *) "    Emergency   ", 16); // Also set at line #26 (Should make this from single value !!!)
			//writetoLCD( 16, 1, dataseq); //send some data

			//Sleep(500);

#ifdef SAFETY_VEST
			//stopAlert();
#endif
			__WFI();

			//dorange = roleBtnOn();

			dataseq[0] = 0x2 ;  //return cursor home
			//writetoLCD( 1, 0,  dataseq);
			memcpy(dataseq, (const uint8 *) "   WaitingOver  ", 16);
			//writetoLCD( 40, 1, dataseq); //send some data

			//Sleep(500);
		}*/
    }

    return 0;
}



