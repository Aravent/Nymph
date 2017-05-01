/*
 *Airdwing Nymph v1.1
 *by AnyCen
 */

#include "main.h"
#include "anyh.h"
#include "bmp.h"//引用图像文件


#undef USE_ENCRYPT
/*-----------------------DJI_LIB VARIABLE-----------------------------*/
using namespace DJI::onboardSDK;

HardDriver* driver = new STM32F4;
CoreAPI defaultAPI = CoreAPI(driver);
CoreAPI *coreApi = &defaultAPI;

Flight flight = Flight(coreApi);
FlightData flightData;

VirtualRC virtualrc = VirtualRC(coreApi);
VirtualRCData myVRCdata =
{ 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024,
    1024, 1024 };

extern TerminalCommand myTerminal;
extern LocalNavigationStatus droneState;
extern uint8_t myFreq[16];
		
/*----------------------AirDwing VARIABLE----------------------------*/
		
char can_cache[255];            //can数据缓冲
uint8_t mobile_data_send[21];	//数据透传 OSDK->MSDK
uint16_t mobile_data_t[100];		//数据透传 MSDK->OSDK
		
int	flag_error_getDroneVersion = 0; //无法获取飞行器版本标志位（一般用于未连接飞行器）
int flag_testsend = 0;              //触发发送标志位
int flag_start = 0;                 //设备启动标志位
char sta_start;                     //状态识别码
int flag_menu = 0;                  //菜单界面标志位
		
int co;    //传感器显示模块（暂存）
int so2;
int o3;
int no2;
int pm25;
int pm10;
		
//CRC码表
static const uint32_t crc32tab[] = {
	0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL,
	0x076dc419L, 0x706af48fL, 0xe963a535L, 0x9e6495a3L,
	0x0edb8832L, 0x79dcb8a4L, 0xe0d5e91eL, 0x97d2d988L,
	0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L, 0x90bf1d91L,
	0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
	0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L,
	0x136c9856L, 0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL,
	0x14015c4fL, 0x63066cd9L, 0xfa0f3d63L, 0x8d080df5L,
	0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L, 0xa2677172L,
	0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
	0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L,
	0x32d86ce3L, 0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L,
	0x26d930acL, 0x51de003aL, 0xc8d75180L, 0xbfd06116L,
	0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L, 0xb8bda50fL,
	0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
	0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL,
	0x76dc4190L, 0x01db7106L, 0x98d220bcL, 0xefd5102aL,
	0x71b18589L, 0x06b6b51fL, 0x9fbfe4a5L, 0xe8b8d433L,
	0x7807c9a2L, 0x0f00f934L, 0x9609a88eL, 0xe10e9818L,
	0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
	0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL,
	0x6c0695edL, 0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L,
	0x65b0d9c6L, 0x12b7e950L, 0x8bbeb8eaL, 0xfcb9887cL,
	0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L, 0xfbd44c65L,
	0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
	0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL,
	0x4369e96aL, 0x346ed9fcL, 0xad678846L, 0xda60b8d0L,
	0x44042d73L, 0x33031de5L, 0xaa0a4c5fL, 0xdd0d7cc9L,
	0x5005713cL, 0x270241aaL, 0xbe0b1010L, 0xc90c2086L,
	0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
	0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L,
	0x59b33d17L, 0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL,
	0xedb88320L, 0x9abfb3b6L, 0x03b6e20cL, 0x74b1d29aL,
	0xead54739L, 0x9dd277afL, 0x04db2615L, 0x73dc1683L,
	0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
	0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L,
	0xf00f9344L, 0x8708a3d2L, 0x1e01f268L, 0x6906c2feL,
	0xf762575dL, 0x806567cbL, 0x196c3671L, 0x6e6b06e7L,
	0xfed41b76L, 0x89d32be0L, 0x10da7a5aL, 0x67dd4accL,
	0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
	0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L,
	0xd1bb67f1L, 0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL,
	0xd80d2bdaL, 0xaf0a1b4cL, 0x36034af6L, 0x41047a60L,
	0xdf60efc3L, 0xa867df55L, 0x316e8eefL, 0x4669be79L,
	0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
	0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL,
	0xc5ba3bbeL, 0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L,
	0xc2d7ffa7L, 0xb5d0cf31L, 0x2cd99e8bL, 0x5bdeae1dL,
	0x9b64c2b0L, 0xec63f226L, 0x756aa39cL, 0x026d930aL,
	0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
	0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L,
	0x92d28e9bL, 0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L,
	0x86d3d2d4L, 0xf1d4e242L, 0x68ddb3f8L, 0x1fda836eL,
	0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L, 0x18b74777L,
	0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
	0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L,
	0xa00ae278L, 0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L,
	0xa7672661L, 0xd06016f7L, 0x4969474dL, 0x3e6e77dbL,
	0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L, 0x37d83bf0L,
	0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
	0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L,
	0xbad03605L, 0xcdd70693L, 0x54de5729L, 0x23d967bfL,
	0xb3667a2eL, 0xc4614ab8L, 0x5d681b02L, 0x2a6f2b94L,
	0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL, 0x2d02ef8dL 
};
 
uint32_t crc32(char *buf, uint32_t size)//CRC计算校验
{
	uint32_t i, crc;
	crc = 0xFFFFFFFF;
	for (i = 0; i < size; i++)
	crc = crc32tab[(crc ^ buf[i]) & 0xff] ^ (crc >> 8);
	return crc^0xFFFFFFFF;
}			
		
void nymth()//Nymph协议解析封装 
//char portal[32] = {0xFE,0x14,0xA2,0x00,0x00,0x00,0x12,0xFF,0x40,0x14,0x03,0xC8,0x00,0x22,0x00,0x78,0x00,0x37,0x00,0x18,0x00,0x20,0x02,0xA2,0x01,0x65,0xCE,0x11,0x11,0x11,0x11,0xFD};
{
	char stx = 0xFE;
	int len;
	char sat,Dsat_1=0xA0,Dsat_2=0xA1,Dsat_3=0xA2,Dsat_4=0xA3,Dsat_5=0xA4,Dsat_6=0xA5,Dsat_7=0xA6;
	char sid;
	char msg[255],crcmsg[4];
	u32 crs;
	char eof = 0xFD;
	char data[512];
	u32 crcdata32;
	
	if(can_cache[0] == stx)
	{
		len = can_cache[1];//提取数据长度
		printf("Data length(str):%d\n",len);
		
		sat = can_cache[2];//提取状态码
		sta_start = can_cache[2];
		if(sat == Dsat_1)
		{
		 printf("State:Host start\n");//主机启动
		}
		else if(sat == Dsat_2)
		{
			 printf("State:Application control\n");//申请控制
		}
		else if(sat == Dsat_3)
		{
			 printf("State:Peripheral forwarding\n");//外设转发
		}
		else if(sat == Dsat_4)
		{
			 printf("State:Peripheral showing\n");//外设显示
		}
		else if(sat == Dsat_5)
		{
			 printf("State:Return register\n");//返回注册
		}
		else if(sat == Dsat_6)
		{
			 printf("State:Return control\n");//返回控制
		}
		else if(sat == Dsat_7)
		{
			 printf("State:Transmit control\n");//发送控制
		}
		else
		{
			 printf("State Error!\n");//状态码错误
		}

    sid = can_cache[6];//提取设备ID
		if(sid == 0x00)
		{
			printf("Peripheral ID:Host send\n");//主机发送
		}
		else
		{
			printf("Peripheral ID:%02X\n",sid);//设备ID
		}

		printf("Message content:");//数据内容
		for(int ts=0;ts<len;ts++)
		{
			msg[ts] = can_cache[7+ts];
			mobile_data_send[ts] = can_cache[7+ts];
			printf("%02X ",can_cache[7+ts]);
		}
		printf("\n");
		
		printf("Protocol CRC check:");//提取CRC校验码
		for(int crctq=0;crctq<4;crctq++)
		{
			crcmsg[crctq] = can_cache[len+7+crctq];
			printf("%02X ",can_cache[len+7+crctq]);
		}
		printf("\n");
		
		crcdata32 = ((u32)crcmsg[0] << 24) | ((u32)crcmsg[1] << 16) | ((u32)crcmsg[2] << 8) | ((u32)crcmsg[3]);//合成CRC码
    printf("Message content CRC01:%X\n",crcdata32);
		
		printf("Message content CRC check:%X\n",crc32(msg,strlen(msg)));//计算CRC码
		if(crcdata32 == crc32(msg,strlen(msg)))//比对CRC码
		{
			printf("Check the message content, the protocol is correct!\n");
		}
		else
		{
			printf("Message content validation failed, please re - transfer!\n");
		}

		if(can_cache[len+11]==eof)//校验协议尾
		{
			printf("End of protocol!\n");
		}
		else
		{
			printf("Protocol error!\n");
		}
	}
	else
	{
		printf("Error ! Not standard Nymth protocol!\n");
	}
	//free(can_cache);
	//return 0;
}		

int tiger_sniffing()
{
	//coreApi->sendToMobile(mobile_data_send, 21);
	int mix  = mobile_data_send[1] * 256 + mobile_data_send[2];
	co   = mobile_data_send[3] * 256 + mobile_data_send[4];
	so2  = mobile_data_send[5] * 256 + mobile_data_send[6];
	o3   = mobile_data_send[7] * 256 + mobile_data_send[8];
	no2  = mobile_data_send[9] * 256 + mobile_data_send[10];
	pm25 = mobile_data_send[11] * 256 + mobile_data_send[12];
	pm10 = mobile_data_send[13] * 256 + mobile_data_send[14];
	float temprature = (mobile_data_send[15] * 256 + mobile_data_send[16] - 500) / 10;
	float humidity = (mobile_data_send[17] * 256 + mobile_data_send[18]) / 10;
	
	unsigned char parity = 255;
	for (int i = 0; i <= 18; i++)
	{
			parity -= mobile_data_send[i];
	}
	if (parity == mobile_data_send[19])
	{
		printf("Check OK\n");
		coreApi->sendToMobile(mobile_data_send, 21);
	}
	else
	{
		printf("Check File\n");
		coreApi->sendToMobile(mobile_data_send, 21);
	}
	OLED_Clear();
	OLED_ShowString(0,2,"CO:");
	OLED_ShowNum(24,2,co,4,16);
	OLED_ShowString(64,2,"PM25:");
	OLED_ShowNum(96,2,pm25,4,16);
	OLED_ShowString(0,4,"O3:");
	OLED_ShowNum(24,4,o3,4,16);
	OLED_ShowString(64,4,"PM10:");
	OLED_ShowNum(96,4,pm10,4,16);
	OLED_ShowString(0,6,"SO2:");
	OLED_ShowNum(32,6,so2,3,16);
	OLED_ShowString(64,6,"NO2:");
	OLED_ShowNum(88,6,no2,5,16);
}
		
int main()//主函数
{
  delay_init(168);
	OLED_Init(); 
	OLED_Clear();
	OLED_DrawBMP(0,0,128,8,BMP1);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,3,CAN_Mode_Normal); //can协议发送指令
	u8 canbuf[8];
	u8 Can_Send_Flag;
	u8 can_i = 0;
	u8 can_a = 0;
	u8 can_key;
	
	
	u8 flag_new=0;
	char c1=0xA2;
	char c2 = 0xFD;
	
	///////////////////////////////
	u8 s=0;
	u8 h=0;
	u8 len;
	///////////////////////////////
	
	
	BSPinit();
  delay_nms(30);
  printf("\rAirDwing Nymph v1.1 \r\n");
  printf("http://www.airdwing.com \r\n");
  delay_nms(3000);
	//delay_nms(3000);
	//delay_nms(3000);
	//delay_nms(3000);
	//delay_nms(3000);
	//delay_nms(3000);
	OLED_Clear();

  uint32_t runOnce = 1;
  uint32_t next500MilTick;
  while (1)
  {
    // One time automatic activation
    if (runOnce)
    {
      runOnce = 0;
      coreApi->setBroadcastFreq(myFreq);
      delay_nms(50);

      // The Local navigation example will run in broadcast call back function,
      // immediate after GPS position is updated
      coreApi->setBroadcastCallback(myRecvCallback,(DJI::UserData)(&droneState));

      //! Since OSDK 3.2.1, the new versioning system does not require you to set version.
      //! It automatically sets activation version through a call to getDroneVersion.
      coreApi->getDroneVersion();
      delay_nms(1000);

      User_Activate();      
      delay_nms(50);

      next500MilTick = driver->getTimeStamp() + 500;
    }

    if (driver->getTimeStamp() >= next500MilTick)
    {
      next500MilTick = driver->getTimeStamp() + 500;

      // Handle user commands from mobile device
      mobileCommandHandler(coreApi, &flight);

      // Handle user commands from serial (USART2)
      myTerminal.terminalCommandHandler(coreApi, &flight);
    }

    coreApi->sendPoll();
		
		//printf("1/n");
		
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		if(flag_start == 0)//判断是否刚刚开机
		{
			if(sta_start == c1)
			{
				flag_menu = 1;
				OLED_Clear();
				flag_start = 1;
			}
			else
			{
				flag_menu = 0;
			}
		}
		
		if(flag_testsend) //触发发送
		{
			coreApi->sendToMobile(mobile_data_send, 21);
			OLED_Clear();
			OLED_ShowString(0,2,"CO:");
			OLED_ShowNum(24,2,co,4,16);
			OLED_ShowString(64,2,"PM25:");
			OLED_ShowNum(96,2,pm25,4,16);
			OLED_ShowString(0,4,"O3:");
			OLED_ShowNum(24,4,o3,4,16);
			OLED_ShowString(64,4,"PM10:");
			OLED_ShowNum(96,4,pm10,4,16);
			OLED_ShowString(0,6,"SO2:");
			OLED_ShowNum(32,6,so2,3,16);
			OLED_ShowString(64,6,"NO2:");
			OLED_ShowNum(88,6,no2,5,16);
			
			flag_testsend=0;
		}
		
		
		if(flag_error_getDroneVersion)//判断是否与飞行器连接
		{
			OLED_ShowCHinese(16,2,19);//未连接飞行器
			OLED_ShowCHinese(32,2,20);
			OLED_ShowCHinese(48,2,21);
			OLED_ShowCHinese(64,2,22);
			OLED_ShowCHinese(80,2,23);
			OLED_ShowCHinese(96,2,24);
			
			OLED_ShowCHinese(32,5,25);//本地模式
			OLED_ShowCHinese(48,5,26);
			OLED_ShowCHinese(64,5,27);
			OLED_ShowCHinese(80,5,28);
		}
		else
		{
			if(flag_menu == 0)
			{
				OLED_ShowCHinese(0,0,27);//模式选择
				OLED_ShowCHinese(16,0,28);
				OLED_ShowCHinese(32,0,33);
				OLED_ShowCHinese(48,0,34);
				
				OLED_ShowCHinese(0,3,35);//▲触发拍照模式
				OLED_ShowCHinese(16,3,29);
				OLED_ShowCHinese(32,3,30);
				OLED_ShowCHinese(48,3,31);
				OLED_ShowCHinese(64,3,32);
				OLED_ShowCHinese(80,3,27);
				OLED_ShowCHinese(96,3,28);
				
				OLED_ShowCHinese(16,6,14);//传感器模式
				OLED_ShowCHinese(32,6,15);
				OLED_ShowCHinese(48,6,16);
				OLED_ShowCHinese(64,6,27);
				OLED_ShowCHinese(80,6,28);
			}
			else if(flag_menu == 1)
			{
				OLED_ShowCHinese(0,0,14);
				OLED_ShowCHinese(16,0,15);
				OLED_ShowCHinese(32,0,16);
				OLED_ShowCHinese(48,0,17);
				OLED_ShowCHinese(64,0,18);
				OLED_ShowString(0,2,"CO:");
				OLED_ShowString(64,2,"PM25:");
				OLED_ShowString(0,4,"O3:");
				OLED_ShowString(64,4,"PM10:");
				OLED_ShowString(0,6,"SO2:");
				OLED_ShowString(64,6,"NO2:");
			}
		}

		
CANReserve:	  //can接收信息
		can_key=CAN1_Receive_Msg(canbuf);
		if(can_key)//接收到有数据
		{			
 			for(can_i=0;can_i<can_key;can_i++)
			{									   				
				can_cache[can_a]=canbuf[can_i];			
				//if(flag_new)
				//{
					if(c2==canbuf[can_i])
					{
						can_a++;
//						for(int w=-1;w<can_a;w++)
//						{
//							USART_SendData(USART2, can_cache[w]);//向串口1发送数据
//							while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//等待发送结束
//						}
						can_a=0;
						printf("\r\n");//插入换行
						nymth();
						tiger_sniffing();
						//delay_nms(300);
						goto CANReserve;
					}
					//else
						//flag_new=0;
				//}
				//if(c1==canbuf[can_i])flag_new=1;
				can_a++;
			}
			//USART_RX_STA=0;
		}
		
		if(USART_RX_STA&0x8000)
		{					   
			len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
			printf("\r\n您发送的消息为:\r\n");
			for(h=0;h<len;h++)
			{
				USART_SendData(USART1,USART_RX_BUF[h]);//向串口1发送数据
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
			} 
			USART_RX_STA=0;
		}
		

  }
}

//int main(void)
//{        
// 	u32 total,free;
//	u8 t=0;	

//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
//	delay_init(168);  //初始化延时函数
//	//uart_init(115200);		//初始化串口波特率为115200
//	//usmart_dev.init(84);		//初始化USMART
//	my_mem_init(SRAMCCM);		//初始化CCM内存池
//	
//  printf("FATFS TEST\n");	
// 	while(SD_Init())//检测不到SD卡
//	{
//		printf("SD Card Error!\n");
//		delay_ms(500);					
//		printf("Please Check!\n");
//		delay_ms(500);
//	}
// 	exfuns_init();							//为fatfs相关变量申请内存				 
//  	f_mount(fs[0],"0:",1); 					//挂载SD卡 											    	  
//	//while(exf_getfree("0",&total,&free))	//得到SD卡的总容量和剩余容量
//	{
//		printf("SD Card Fatfs Error!\n");
//		delay_ms(200);
//		delay_ms(200);
//	}													  			         
//  printf("FATFS OK!\n");
//	printf("SD Total Size:%d MB\n",total>>10);
//  printf("SD Free Size:%d MB\n",free>>10);	
//	
//	//mf_open("0:/xzl.txt",0x08);
//	filelog();
//	while(1)
//	{
//		t++; 
//		delay_ms(200);		 			   
//	} 
//}


