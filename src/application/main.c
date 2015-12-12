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
#include "port.h"

#include "instance.h"

#include "deca_spi.h"

/*
#ifdef USB_SUPPORT
	extern void usb_run(void);
	extern int usb_init(void);
	extern void usb_printconfig(void);
	extern void send_usbmessage(uint8*, int);
#endif
*/
//rangeStart = 0;

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
//        	i=15;
//        	while(i--)
//        	{
//        		if (i & 1) {
//        			//led_off(LED_PC6);
//        			led_on(LED_PC6);
//        			led_off(LED_PC7);
//        		}
//        		else{
//        			//led_on(LED_PC6);
//        			led_off(LED_PC6);
//        			led_on(LED_PC7);
//        		}
//
//        		Sleep(300);
//        	}
//
//        		i = 0;
//
//        		Sleep(300);
//        		led_on(LED_ALL);
//        		Sleep(1400);
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

//    	i=15;
//    	        	while(i--)
//    	        	{
//    	        		if (i & 1) {
//    	        			//led_off(LED_PC6);
//    	        			led_on(LED_PC7);
//    	        			led_off(LED_PC6);
//    	        		}
//    	        		else{
//    	        			//led_on(LED_PC6);
//    	        			led_off(LED_PC7);
//    	        			led_on(LED_PC6);
//    	        		}
//
//    	        		Sleep(300);
//    	        	}
//
//    	        		i = 0;
//

//    	        		Sleep(300);
//    	        		led_on(LED_ALL);
//    	        		Sleep(1400);

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

void initLCD(void)
{
    uint8 initseq[9] = { 0x39, 0x14, 0x55, 0x6D, 0x78, 0x38 /*0x3C*/, 0x0C, 0x01, 0x06 };
    uint8 command = 0x0;
    int j = 100000;

    //writetoLCD( 9, 0,  initseq); //init seq
    while(j--);

    command = 0x2 ;  //return cursor home
    //writetoLCD( 1, 0,  &command);
    command = 0x1 ;  //clear screen
    //writetoLCD( 1, 0,  &command);
}

/*
 * @brief switch_mask  - bitmask of testing switches (currently 7 switches)
 *        switchbuff[] - switch name to test
 *        *switch_fn[]() - corresponded to switch test function
**/
#define switch_mask   (0x7F)

const uint8 switchbuf[]={0, TA_SW1_3 , TA_SW1_4 , TA_SW1_8 };
const int (* switch_fn[])(uint16)={ &is_button_low, \
                                &is_switch_on, &is_switch_on, &is_switch_on,\
                                &is_switch_on, &is_switch_on, &is_switch_on };

/*
 * @fn test_application_run
 * @brief   test application for production pre-test procedure
**/
void test_application_run(void)
{
    char  dataseq[2][40];
    uint8 j, switchStateOn, switchStateOff;

    switchStateOn=0;
    switchStateOff=0;

    led_on(LED_ALL);    // show all LED OK
    Sleep(1000);

    dataseq[0][0] = 0x1 ;  //clear screen
    //writetoLCD( 1, 0, (const uint8 *) &dataseq);
    dataseq[0][0] = 0x2 ;  //return cursor home
    //writetoLCD( 1, 0, (const uint8 *) &dataseq);

/* testing SPI to DW1000*/
    //writetoLCD( 40, 1, (const uint8 *) "TESTING         ");
    //writetoLCD( 40, 1, (const uint8 *) "SPI, U2, S2, S3 ");
    Sleep(1000);

    if(inittestapplication() == (uint32)-1)
    {
        //writetoLCD( 40, 1, (const uint8 *) "SPI, U2, S2, S3 ");
        //writetoLCD( 40, 1, (const uint8 *) "-- TEST FAILS --");
        while(1); //stop
    }

    //writetoLCD( 40, 1, (const uint8 *) "SPI, U2, S2, S3 ");
    //writetoLCD( 40, 1, (const uint8 *) "    TEST OK     ");
    Sleep(1000);

/* testing of switch S2 */
    dataseq[0][0] = 0x1 ;  //clear screen
    //writetoLCD( 1, 0, (const uint8 *) &dataseq);

    while( (switchStateOn & switchStateOff) != switch_mask )
        {
        memset(&dataseq, ' ', sizeof(dataseq));
        strcpy(&dataseq[0][0], (const char *)"SWITCH");
        strcpy(&dataseq[1][0], (const char *)"toggle");
//switch 7-1
        for (j=0;j<sizeof(switchbuf);j++)
        {
            if( switch_fn[j](switchbuf[j]) ) //execute current switch switch_fn
            {
                dataseq[0][8+j]='O';
                switchStateOn |= 0x01<<j;
                switchStateOff &= ~(0x01<<j);//all switches finaly should be in off state
            }else{
                dataseq[1][8+j]='O';
                switchStateOff |=0x01<<j;
        }
        }

        ////writetoLCD(40, 1, (const uint8 *) &dataseq[0][0]);
        //writetoLCD(40, 1, (const uint8 *) &dataseq[1][0]);
        Sleep(100);
        }

    led_off(LED_ALL);

    //writetoLCD( 40, 1, (const uint8 *) "  Preliminary   ");
    //writetoLCD, (const uint8 *) "   TEST OKAY    ");

    while(1);
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

    //led_off(LED_ALL); //turn off all the LEDs

    peripherals_init();

    spi_peripheral_init();

    //Sleep(1000); //wait for LCD to power on

    //initLCD();

    //init_dw();

    role_btn_set(DISABLE);//emergency button settings


    //memset(dataseq, 40, 0);
    //memcpy(dataseq, (const uint8 *) "      KFRL      ", 16);
    ////writetoLCD( 40, 1, dataseq); //send some data
    //memcpy(dataseq, (const uint8 *) "Worker's Device ", 16); // Also set at line #26 (Should make this from single value !!!)
    //writetoLCD( 16, 1, dataseq); //send some data

    //Sleep(1000);

//    i=15;
//                                    while(i--)
//                                    {
//                                        if (i & 1) {
//                                        	led_off(LED_PC6);
//                                        	led_on(LED_PC8);
//                                        	led_off(LED_PC7);
//                                        }
//                                        else{
//                                        	led_on(LED_PC6);
//                                        	led_off(LED_PC8);
//                                        	led_on(LED_PC7);
//                                        }
//                                        Sleep(400);
//                                    }
//
//                                    i = 0;


//#ifdef USB_SUPPORT
//    // enable the USB functionality
//    usb_init();
//    Sleep(1000);
//#endif

    /*i=30;
                        while(i--)
                        {
                            if (i & 1) led_off(LED_ALL);
                            else    led_on(LED_ALL);
                            Sleep(200);
                        }

                        i = 0;
*/
    port_DisableEXT_IRQ(); //disable ScenSor IRQ until we configure the device

    //test EVB1000 - used in EVK1000 production
    //NOTE: when using non-Discovery mode switch 8 is part of address config/anchor poll mask config
/*#if (DR_DISCOVERY == 1)
    if((is_button_low(0) == S1_SWITCH_ON) && (is_switch_on(TA_SW1_8) == S1_SWITCH_ON)) //using BOOT1 switch for test
    {
        test_application_run(); //does not return....
    }
    else
#endif
*/
    //this 'if' is for pc software, i am commenting this bcz we dont need to run deca ranging on PC
    if(false/*is_switch_on(TA_SW1_3) == S1_SWITCH_OFF*/)
    {
    /*    int j = 1000000;
        uint8 command;

        memset(dataseq, 0, 40);

        while(j--);
        //command = 0x1 ;  //clear screen
        //////writetoLCD( 1, 0,  &command);
        command = 0x2 ;  //return cursor home
        //writetoLCD( 1, 0,  &command);

        memcpy(dataseq, (const uint8 *) "KFRL       ", 12);
        writetoLCD( 40, 1, dataseq); //send some data
#ifdef USB_SUPPORT //this is set in the port.h file
        memcpy(dataseq, (const uint8 *) "USB to SPI ", 12);
#else
#endif
        writetoLCD( 16, 1, dataseq); //send some data

        j = 1000000;

        while(j--);

        command = 0x2 ;  //return cursor home
        //writetoLCD( 1, 0,  &command);
#ifdef USB_SUPPORT //this is set in the port.h file
        // enable the USB functionality
        //usb_init();

        NVIC_DisableDECAIRQ();

        // Do nothing in foreground -- allow USB application to run, I guess on the basis of USB interrupts?
        while (1)       // loop forever
        {
            usb_run();
        }
#endif

        return 1;*/
    }
    else //run DecaRanging application on ARM board
    {
        uint8 dataseq[40];
        uint8 command = 0x0;

        command = 0x2 ;  //return cursor home
        ////writetoLCD( 1, 0,  &command);
        //memset(dataseq, ' ', 40);
        //memcpy(dataseq, (const uint8 *) "      Yaseen   ", 15);
        //writetoLCD( 15, 1, dataseq); //send some data

        //led_off(LED_ALL);

        //Sleep(1000);

        //led_on(LED_ALL);

        //Sleep(1000);

        //led_off(LED_ALL);

        /*i=30;
                            while(i--)
                            {
                                if (i & 1) {
                                	led_off(LED_PC6);
                                	led_on(LED_PC8);
                                	led_off(LED_PC7);
                                }
                                else{
                                	led_on(LED_PC6);
                                	led_off(LED_PC8);
                                	led_on(LED_PC7);
                                }
                                Sleep(200);
                            }

                            i = 0;
*/
                         //   led_off(LED_ALL);

        if(inittestapplication() == (uint32)-1) //test SPI and dw1000 device is working or not
        {
            led_off(LED_ALL); //to display error....

            dataseq[0] = 0x2 ;  //return cursor home
            //writetoLCD( 1, 0,  &dataseq[0]);
            //memset(dataseq, ' ', 40);
            //memcpy(dataseq, (const uint8 *) "ERROR   ", 12);
            //writetoLCD( 40, 1, dataseq); //send some data
            //memcpy(dataseq, (const uint8 *) "  INIT FAIL ", 12);
            //writetoLCD( 40, 1, dataseq); //send some data

//            i=30;
//                    while(i--)
//                    {
//                        if (i & 1) led_off(LED_ALL);
//                        else    led_on(LED_ALL);
//                        Sleep(200);
//                    }
//
//                    i = 0;

//            i=15;
//                                 while(i--)
//                                 {
//                                     if (i & 1) {
//                                     	//led_off(LED_PC6);
//                                     	led_on(LED_PC8);
//                                     	//led_off(LED_PC7);
//                                     }
//                                     else{
//                                     	//led_on(LED_PC6);
//                                     	led_off(LED_PC8);
//                                     	//led_on(LED_PC7);
//                                     }
//                                     Sleep(300);
//                                 }
//
//                                 i = 0;

            return 0; //error
        }
        else
        {
        	 i=10;
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
        	                                             Sleep(50);
        	                                         }

        	                                         i = 0;
        	                                 led_off(LED_ALL);
        }
//        i=15;
//                                         while(i--)
//                                         {
//                                             if (i & 1) {
//                                             	//led_off(LED_PC6);
//                                             	led_on(LED_PC6);
//                                             	//led_off(LED_PC7);
//                                             }
//                                             else{
//                                             	//led_on(LED_PC6);
//                                             	led_off(LED_PC6);
//                                             	//led_on(LED_PC7);
//                                             }
//                                             Sleep(300);
//                                         }
//
//                                         i = 0;

        //sleep for 5 seconds displaying "Decawave"
        /*i=30;
        while(i--)
        {
            if (i & 1) led_off(LED_ALL);
            else    led_on(LED_ALL);
            Sleep(200);
        }

        i = 0;
*/
  //      led_off(LED_ALL);
        //command = 0x2 ;  //return cursor home
        ////writetoLCD( 1, 0,  &command);

        //memset(dataseq, ' ', 40);

		//led_on(LED_PC7);

		//if TA_SW1_2 is ON use fast ranging (fast 2wr)
		//EVK1000 User manual pg 22
		if(is_button_low(0) == S1_SWITCH_ON)
		{
			//memcpy(&dataseq[2], (const uint8 *) " Fast Tag   ", 12);
			//writetoLCD( 40, 1, dataseq); //send some data
			//memcpy(&dataseq[2], (const uint8 *) "   Ranging  ", 12);
			//writetoLCD( 16, 1, dataseq); //send some data
			//Sleep(1000);
		}
		else
		{   //as of 26/08/15 I am seeing this when I execute code, means Fast ranging is disabled
			//memcpy(&dataseq[2], (const uint8 *) " TAG BLINK  ", 12);
			////writetoLCD( 40, 1, dataseq); //send some data
			//sprintf((char*)&dataseq[0], "%llX", instance_get_addr());
			////writetoLCD( 16, 1, dataseq); //send some data
			//Sleep(1000);
		}

        //command = 0x2 ;  //return cursor home
        //writetoLCD( 1, 0,  &command);
    }

    port_EnableEXT_IRQ(); //enable ScenSor IRQ before starting

    //memset(dataseq, ' ', 40);
    //memset(dataseq1, ' ', 40);

    // main loop

	role_btn_set(ENABLE); //configures interrupt

	bool dorange = true;
	//bool test = true;

	//TODO: Start Watchdog timer

	    	//IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	    	//IWDG_SetPrescaler(8);
	    	//IWDG_Enable();


    while(1)
    {
    	//IWDG_ReloadCounter();

    	//if( /*roleBtnOn() == */ true)
    	//{
    		//dorange = false;

    		/*if(dorange)
    		{
    			dataseq[0] = 0x2 ;  //return cursor home
    			////writetoLCD( 1, 0,  dataseq);
    			memcpy(dataseq, (const uint8 *) "   inside  if   ", 16);
    			//writetoLCD( 40, 1, dataseq); //send some data
    			dorange = false;
    		}*/


			role_btn_set(DISABLE);

			instance_run();							//ranging routine


			/*uint8 i=10;
			    		        	                                         while(i--)
			    		        	                                         {
			    		        	                                             if (i & 1) {
			    		        	                                             	led_off(LED_PC6);
			    		        	                                             	//led_on(LED_PC7);
			    		        	                                             	//led_off(LED_PC7);
			    		        	                                             }
			    		        	                                             else{
			    		        	                                             	led_on(LED_PC6);
			    		        	                                             	//led_off(LED_PC7);
			    		        	                                             	//led_on(LED_PC7);
			    		        	                                             }
			    		        	                                             Sleep(70);
			    		        	                                         }

			    		        	                                         i = 0;
			    		        	                                 led_off(LED_ALL);
*/



			//Sleep(500);

//#ifdef SAFETY_VEST
			//si_sendAlertMSG();
//#endif
			role_btn_set(ENABLE);

			/*if(instancenewrange())
			{
				int l = 0, txl = 0, rxl = 0, aaddr, taddr;
				ranging = 1;
				//send the new range information to LCD and/or USB
				range_result = instance_get_idist();
				avg_result = instance_get_adist();
				//set_rangeresult(range_result);
				dataseq[0] = 0x2 ;  //return cursor home
				//writetoLCD( 1, 0,  dataseq);

				memset(dataseq, ' ', 40);
				memset(dataseq1, ' ', 40);
				sprintf((char*)&dataseq[1], "LAST: %4.2f m", range_result);
				//writetoLCD( 40, 1, dataseq); //send some data

				sprintf((char*)&dataseq1[1], "AVG8: %4.2f m", avg_result);
				//writetoLCD( 16, 1, dataseq1); //send some data

				//while(1);

			}*/

			/*if(ranging == 0)
			{
				if(instancesleeping())
				{
					dataseq[0] = 0x2 ;  //return cursor home
					//writetoLCD( 1, 0,  dataseq);
					if(toggle)
					{
						toggle = 0;
						memcpy(&dataseq[0], (const uint8 *) "    AWAITING    ", 16);
						//writetoLCD( 40, 1, dataseq); //send some data
						memcpy(&dataseq[0], (const uint8 *) "    RESPONSE    ", 16);
						//writetoLCD( 16, 1, dataseq); //send some data
					}
					else
					{
						toggle = 1;
						memcpy(&dataseq[2], (const uint8 *) "   TAG BLINK    ", 16);
						//writetoLCD( 40, 1, dataseq); //send some data
						sprintf((char*)&dataseq[0], "%llX", instance_get_addr());
						//writetoLCD( 16, 1, dataseq); //send some data
					}
	//#ifdef USB_SUPPORT //this is set in the port.h file
					//send_usbmessage(&dataseq[0], 35);
	//#endif
				}

				if(instanceanchorwaiting() == 2)
				{
					ranging = 1;
					dataseq[0] = 0x2 ;  //return cursor home
					//writetoLCD( 1, 0,  dataseq);
					memcpy(&dataseq[0], (const uint8 *) "    RANGING WITH", 16);
					//writetoLCD( 40, 1, dataseq); //send some data
					sprintf((char*)&dataseq[0], "%016llX", instance_get_anchaddr());
					//writetoLCD( 16, 1, dataseq); //send some data
				}
			}*/

    	//}
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



