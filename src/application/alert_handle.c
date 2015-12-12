///*
// * alert_handle.c
// *
// *  Created on: 16 Jun 2015
// *      Author: KFRL / Syed Mustafa Kazmi
// *
// *  Brief: contains functions to be used in emergency condition
// */
//#include "alert_handle.h"
//
//void alertInit()
//{
//	emergency_flag = 0;
//}
//
//uint8_t accidentOccur()
//{
//	return emergency_flag;
//}
//
//int roleBtnOn()
//{
//	return is_switch_on(ROLEBTN);
//}
//
/////////////////////////	alert generation and stop ////////////////////////////////////////////
//void generateAlertRec(void) // generate signal to indicate alert is received from other worker
//{
//
//	vibrate();
//	buzzer_on();
//	alert_led_on();
//}
//
//void stopAlert(void)
//{
//	vibrate_off();
//	buzzer_off();
//	alert_led_off();
//}
//
/////////////////////////////// Alert recieve and send for si4463 //////////////////////////////
//void si_sendAlertMSG()
//{
//	uint8 i;
//    uint8 cmd[SI_PACKET_LENGTH]= SI_ALERT_MESSAGE;
//
//    si_sendPacket( cmd, SI_PACKET_LENGTH, SI_CHANNEL, SI_CONDITION);
//}
//
//void si_sendClearMSG()
//{
//	uint8 i;
//    uint8 cmd[SI_PACKET_LENGTH]= SI_CLEAR_MESSAGE;
//    si_sendPacket( cmd, SI_PACKET_LENGTH, SI_CHANNEL, SI_CONDITION);
//}
//
