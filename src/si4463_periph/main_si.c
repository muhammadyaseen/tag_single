//ErrorStatus HSEStartUpStatus;
//
//uint8  buffer[64] = {'0','0','0','0','0',0};
///*******************************************************************************
//* Function Name  : main
//* Description    : Main program.
//* Input          : None
//* Output         : None
//* Return         : None
//*******************************************************************************/
//int main(void)
//{
//    uint8 i, length, error;
//    uint32 dly;
//    uint16 tx_conter = 0, itmp;
//
//#ifdef DEBUG
//  debug();
//#endif
//
////  for(i = 0 ; i < 0xffff ; ++i){;}
//  chushihua();
//
//  SI446X_RESET( );        //SI446X 模块复位
//  SI446X_CONFIG_INIT( );  //SI446X 模块初始化配置函数
//  SI446X_START_RX( 0, 0, 7,0,0,3 );
//  //SI446X_START_TX( 0, 0, 10);
//
////===================================发送==================================
//   while( 1 )
//    {
//        //发送数字0-9
//        for( i = 0; i < 10; i ++ )      { buffer[i] = i + 1; }
//        //变长或定长模式，在SI446X.H中配置
//#if PACKET_LENGTH == 0   //变长模式，最大发射15字节,此处发10字节
//        SI446X_SEND_PACKET( buffer, 10, 0, 0 );
//#else                    //定长模式，长度取决于PACKET_LENGTH,最大64
//        SI446X_SEND_PACKET( buffer, PACKET_LENGTH, 0, 0 );
//#endif
//
//        do{              //等待发射完成（中断产生）
//            SI446X_INT_STATUS( buffer );
//        }while( !( buffer[3] & ( 1<<5 ) ) );
//
//
//        SI446X_START_RX( 0, 0, 7,0,0,3 );
//        dly = 1000;
//        while( dly-- )
//        {
//            for( length = 0; length < 20; length ++ );
//            SI446X_INT_STATUS( buffer );
//            if( buffer[3] & ( 1<<4 ) )
//            {
//                length = SI446X_READ_PACKET( buffer );
//                for( i = 0, error = 0; i < 10; i ++ )
//                {
//                    if( buffer[i] != i)
//                    {
//                        error = 1;
//                    }
//                }
//            }
//        }
//        if(error == 0)
//        {
//          SPI_Initial();
//
//          for( dly = 0; dly < 15000; dly ++ );
//          LED_On( );
//          for( dly = 0;dly < 15000; dly ++ );
//          LED_Off( );
//        }
//    }
//}
