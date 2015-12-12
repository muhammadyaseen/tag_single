/*! ----------------------------------------------------------------------------
 *  @file    instance.c
 *  @brief   DecaWave application level message exchange for ranging demo
 *
 * @attention
 *
 * Copyright 2013 (c) DecaWave Ltd, Dublin, Ireland.
 *
 * All rights reserved.
 *
 * @author DecaWave
 */
#include "compiler.h"
#include "port.h"
#include "deca_device_api.h"
#include "deca_spi.h"
#include "deca_regs.h"

#include "instance.h"

// -------------------------------------------------------------------------------------------------------------------

//application data message byte offsets
#define FCODE                               0               // Function code is 1st byte of messageData
#define PTXT                                1
#define RRXT                                6
#define FTXT                                11
#define TOFR                                1
#define RES_R1                              1               // Response option octet 0x02 (1),
#define RES_R2                              2               // Response option parameter 0x00 (1) - used to notify Tag that the report is coming
#define RES_R3                              3               // Response option parameter 0x00 (1),
#define RES_T1                              3               // Ranging request response delay low byte
#define RES_T2                              4               // Ranging request response delay high byte
#define POLL_TEMP                           1               // Poll message TEMP octet
#define POLL_VOLT                           2               // Poll message Voltage octet




// -------------------------------------------------------------------------------------------------------------------
//      Data Definitions
// -------------------------------------------------------------------------------------------------------------------

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// NOTE: the maximum RX timeout is ~ 65ms
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


// -------------------------------------------------------------------------------------------------------------------
// Functions
// -------------------------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------------------------
//
// function to construct the message/frame header bytes
//
// -------------------------------------------------------------------------------------------------------------------
//
void instanceconfigframeheader(instance_data_t *inst, int ackrequest)
{
    inst->msg.panID[0] = (inst->panid) & 0xff;
    inst->msg.panID[1] = inst->panid >> 8;

    //set frame type (0-2), SEC (3), Pending (4), ACK (5), PanIDcomp(6)
    inst->msg.frameCtrl[0] = 0x1 /*frame type 0x1 == data*/ | 0x40 /*PID comp*/;
    inst->msg.frameCtrl[0] |= (ackrequest ? 0x20 : 0x00);
#if (USING_64BIT_ADDR==1)
    //source/dest addressing modes and frame version
    inst->msg.frameCtrl[1] = 0xC /*dest extended address (64bits)*/ | 0xC0 /*src extended address (64bits)*/;
#else
    inst->msg.frameCtrl[1] = 0x8 /*dest short address (16bits)*/ | 0x80 /*src short address (16bits)*/;
#endif
    inst->msg.seqNum = inst->frame_sn++;

}



#if (DR_DISCOVERY == 0)
int destaddress(instance_data_t *inst)
{
    int getnext = 1;
    int tempanchorListIndex = 0;
    //set destination address (Tag will cycle though the list of anchor addresses)

    if(inst->payload.anchorPollMask == 0)
        return 1; //error - list is empty

    while(getnext)
    {
        if((0x1 << inst->anchorListIndex) & inst->payload.anchorPollMask)
        {
            memcpy(&inst->msg.destAddr[0], &inst->payload.anchorAddressList[inst->anchorListIndex], ADDR_BYTE_SIZE_L);
            getnext = 0;
        }

        inst->anchorListIndex++ ;
    }

    tempanchorListIndex = inst->anchorListIndex;

    while(tempanchorListIndex <= inst->payload.anchorListSize)
    {
        //check if there are any anchors left to poll
        if((0x1 << tempanchorListIndex) & inst->payload.anchorPollMask)
        {
            return 0;
        }
        tempanchorListIndex++;
    }

    //if we got this far means that we are just about to poll the last anchor in the list
    inst->instToSleep = 1; //we'll sleep after this poll
    inst->anchorListIndex = 0; //start from the first anchor in the list after sleep finishes

    return 0;
}
#endif


// -------------------------------------------------------------------------------------------------------------------
//
// function to configure the frame data, prior to writing the frame to the TX buffer
//
// -------------------------------------------------------------------------------------------------------------------
//
void setupmacframedata(instance_data_t *inst, int len, int framectrllen, int fcode, int ack)
{
    inst->msg.messageData[FCODE] = fcode; //message function code (specifies if message is a poll, response or other...)

    inst->psduLength = len + framectrllen;

	//inst->psduLength += adduserpayload(inst, len); //add any user data to the message payload

	instanceconfigframeheader(inst, ack);

    if(ack == ACK_REQUESTED)
        inst->wait4ack = DWT_RESPONSE_EXPECTED;

    inst->ackexpected = ack ; //used to ignore unexpected ACK frames
}

// -------------------------------------------------------------------------------------------------------------------
//
// Turn on the receiver with/without delay
//
void instancerxon(instance_data_t *inst, int delayed, uint64 delayedReceiveTime)
{
    if (delayed)
    {
        uint32 dtime;
        dtime =  (uint32) (delayedReceiveTime>>8);
        dwt_setdelayedtrxtime(dtime) ;
    }

    inst->lateRX -= dwt_rxenable(delayed) ;  //- as when fails -1 is returned             // turn receiver on, immediate/delayed

} // end instancerxon()


int instancesendpacket(instance_data_t *inst, int delayedTx)
{
    int result = 0;

    dwt_writetxfctrl(inst->psduLength, 0);
    if(delayedTx)
    {
        uint32 dtime;
        dtime = (uint32) (inst->delayedReplyTime>>8);
        dwt_setdelayedtrxtime(dtime) ;
    }

    //begin delayed TX of frame
    if (dwt_starttx(delayedTx | inst->wait4ack))  // delayed start was too late
    {
        result = 1; //late/error
        inst->lateTX++;
    }


    return result;                                              // state changes
    // after sending we should return to TX ON STATE ?
}

// -------------------------------------------------------------------------------------------------------------------
#if NUM_INST != 1
#error These functions assume one instance only
#else


// -------------------------------------------------------------------------------------------------------------------
// function to set the fixed blink reply delay time (in ms)
// NOTE: this is the delay between receiving the blink and sending the ranging init message
// The anchor ranging init response delay has to match the delay the tag expects
// thereafter the tag will use ranging response delay as specified in the ranging init message
void instancesetblinkreplydelay(int delayms) //delay in ms
{
#if (USING_64BIT_ADDR == 1)
	int msgdatalen = RANGINGINIT_MSG_LEN + FRAME_CRTL_AND_ADDRESS_L + FRAME_CRC;
#else
	int msgdatalen = RANGINGINIT_MSG_LEN + FRAME_CRTL_AND_ADDRESS_S + FRAME_CRC;
#endif

	//msgdatalen = BLINK_FRAME_CRTL_AND_ADDRESS + BLINK_FRAME_CRC;

    instancesetreplydelay(delayms, msgdatalen) ;
}

// -------------------------------------------------------------------------------------------------------------------
// function to set the fixed reply delay time (in ms)
//
extern uint8 dwnsSFDlen[];

void instancesetreplydelay(int delayms, int datalength) //delay in ms
{
    int instance = 0;

    int margin = 3000; //2000 symbols

	//configure the rx delay receive delay time, it is dependent on the message length
	float msgdatalen = 0;
	float preamblelen = 0;
	int sfdlen = 0;
	int x = 0;

	if(datalength == 0)
    {
#if (USING_64BIT_ADDR == 1)
		msgdatalen = TAG_FINAL_MSG_LEN + FRAME_CRTL_AND_ADDRESS_L + FRAME_CRC;
#else
		msgdatalen = TAG_FINAL_MSG_LEN + FRAME_CRTL_AND_ADDRESS_S + FRAME_CRC;
#endif

		//msgdatalen += instance_data[instance].payload.payloadLen ;
    }
	else //blink message
    {
		msgdatalen = datalength;
    }

	x = (int) ceil(msgdatalen*8/330.0f);

	msgdatalen = msgdatalen*8 + x*48;

	//assume PHR length is 172308us for 110k and 21539us for 850k/6.81M
	if(instance_data[instance].configData.dataRate == DWT_BR_110K)
    {
		msgdatalen *= 8205.13f;
		msgdatalen += 172308;

                }
	else if(instance_data[instance].configData.dataRate == DWT_BR_850K)
                {
		msgdatalen *= 1025.64f;
		msgdatalen += 21539;
        }
	else
        {
		msgdatalen *= 128.21f;
		msgdatalen += 21539;
	}

	//SFD length is 64 for 110k (always)
	//SFD length is 8 for 6.81M, and 16 for 850k, but can vary between 8 and 16 bytes
	sfdlen = dwnsSFDlen[instance_data[instance].configData.dataRate];

	switch (instance_data[instance].configData.txPreambLength)
    {
    case DWT_PLEN_4096 : preamblelen = 4096.0f; break;
    case DWT_PLEN_2048 : preamblelen = 2048.0f; break;
    case DWT_PLEN_1536 : preamblelen = 1536.0f; break;
    case DWT_PLEN_1024 : preamblelen = 1024.0f; break;
    case DWT_PLEN_512  : preamblelen = 512.0f; break;
    case DWT_PLEN_256  : preamblelen = 256.0f; break;
    case DWT_PLEN_128  : preamblelen = 128.0f; break;
    case DWT_PLEN_64   : preamblelen = 64.0f; break;
            }

	//preamble  = plen * (994 or 1018) depending on 16 or 64 PRF
	if(instance_data[instance].configData.prf == DWT_PRF_16M)
	{
		preamblelen = (sfdlen + preamblelen) * 0.99359f;
	}
	else
	{
		preamblelen = (sfdlen + preamblelen) * 1.01763f;
	}


	if(datalength != 0)
	{
		//set the frame wait timeout time - total time the frame takes in symbols
		instance_data[instance].fwtoTimeB_sy = 16 + (int)((preamblelen + (msgdatalen/1000.0))/ 1.0256) + margin;

		instance_data[instance].rnginitW4Rdelay_sy = (int) (delayms * 1000 / 1.0256) - 16 - (int)((preamblelen + (msgdatalen/1000.0))/ 1.0256);
	}
	else
	{
		//set the frame wait timeout time - total time the frame takes in symbols
		instance_data[instance].fwtoTime_sy = 16 + (int)((preamblelen + (msgdatalen/1000.0))/ 1.0256) + margin;

		//this is the delay used for the delayed transmit (when sending the ranging init, response, and final messages)
		instance_data[instance].fixedReplyDelay = convertmicrosectodevicetimeu (delayms * 1e3) ;

		instance_data[instance].fixedReplyDelay_ms = (int) delayms ;

		//this it the delay used for configuring the receiver on delay (wait for response delay),
		instance_data[instance].fixedReplyDelay_sy = (int) (delayms * 1000 / 1.0256) - 16 - (int)((preamblelen + (msgdatalen/1000.0))/ 1.0256); //subtract 16 symbols, as receiver has a 16 symbol start up time
	}
	//printf("preamble %4.3fus, Final msg %4.3fus\n", preamblelen, msgdatalen/1000);
	//printf("Set response delay time to %d ms, %d sym payload %d\n", (int) delayms, instance_data[instance].fixedReplyDelay_sy, instance_data[instance].payload.payloadLen);
}

// -------------------------------------------------------------------------------------------------------------------
// function to configure anchor instance whether to send TOF reports to Tag
//
void instancesetreporting(int anchorSendsTofReports)
{
    int instance = 0 ;
    instance_data[instance].sendTOFR2Tag = anchorSendsTofReports ;        // Set whether TOF reports are sent
}

#if (DR_DISCOVERY == 0)
// -------------------------------------------------------------------------------------------------------------------
//
// Set Payload parameters for the instance
//
// -------------------------------------------------------------------------------------------------------------------
void instancesetaddresses(instanceAddressConfig_t *plconfig)
    {
    int instance = 0 ;

    instance_data[instance].payload = *plconfig ;       // copy configurations

    instancesetreporting(instance_data[instance].payload.sendReport);
}
#endif

uint64 instance_get_addr(void) //get own address
{
    int instance = 0;
    uint64 x = (uint64) instance_data[instance].eui64[0];
    x |= (uint64) instance_data[instance].eui64[1] << 8;
    x |= (uint64) instance_data[instance].eui64[2] << 16;
    x |= (uint64) instance_data[instance].eui64[3] << 24;
    x |= (uint64) instance_data[instance].eui64[4] << 32;
    x |= (uint64) instance_data[instance].eui64[5] << 40;
    x |= (uint64) instance_data[instance].eui64[6] << 48;
    x |= (uint64) instance_data[instance].eui64[7] << 56;


    return (x);
}

uint64 instance_get_tagaddr(void) //get own address
{
    int instance = 0;
    uint64 x = (uint64) instance_data[instance].tagList[0][0];
    x |= (uint64) instance_data[instance].tagList[0][1] << 8;
    x |= (uint64) instance_data[instance].tagList[0][2] << 16;
    x |= (uint64) instance_data[instance].tagList[0][3] << 24;
    x |= (uint64) instance_data[instance].tagList[0][4] << 32;
    x |= (uint64) instance_data[instance].tagList[0][5] << 40;
    x |= (uint64) instance_data[instance].tagList[0][6] << 48;
    x |= (uint64) instance_data[instance].tagList[0][7] << 56;


    return (x);
}

uint64 instance_get_anchaddr(void) //get anchor address (that sent the ToF)
{
    int instance = 0;
    uint64 x = (uint64) instance_data[instance].relpyAddress[0];
    x |= (uint64) instance_data[instance].relpyAddress[1] << 8;
    x |= (uint64) instance_data[instance].relpyAddress[2] << 16;
    x |= (uint64) instance_data[instance].relpyAddress[3] << 24;
    x |= (uint64) instance_data[instance].relpyAddress[4] << 32;
    x |= (uint64) instance_data[instance].relpyAddress[5] << 40;
    x |= (uint64) instance_data[instance].relpyAddress[6] << 48;
    x |= (uint64) instance_data[instance].relpyAddress[7] << 56;
    return (x);
}

void instance_readaccumulatordata(void)
{
#if DECA_SUPPORT_SOUNDING==1
    int instance = 0;
    uint16 len = 992 ; //default (16M prf)

    if (instance_data[instance].configData.prf == DWT_PRF_64M)  // Figure out length to read
        len = 1016 ;

    instance_data[instance].buff.accumLength = len ;                                       // remember Length, then read the accumulator data

    len = len*4+1 ;   // extra 1 as first byte is dummy due to internal memory access delay

    dwt_readaccdata((uint8*)&(instance_data[instance].buff.accumData->dummy), len, 0);
#endif  // support_sounding
}

#endif


/* ==========================================================

Notes:

Previously code handled multiple instances in a single console application

Now have changed it to do a single instance only. With minimal code changes...(i.e. kept [instance] index but it is always 0.

Windows application should call instance_init() once and then in the "main loop" call instance_run().

*/
