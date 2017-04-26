/*
 *Airdwing Nymph v1.1
 *by AnyCen
 */

#include "main.h"
#include "anyh.h"
#include "bmp.h"


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
		
char can_cache[255];
uint8_t mobile_data_send[100];	
uint16_t mobile_data_t[100];		
		
		
int	flag_error_getDroneVersion = 0;
int flag_testsend = 0;
		
int co;
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
 
uint32_t crc32(char *buf, uint32_t size)//CRC计算
{
	uint32_t i, crc;
	crc = 0xFFFFFFFF;
	for (i = 0; i < size; i++)
	crc = crc32tab[(crc ^ buf[i]) & 0xff] ^ (crc >> 8);
	return crc^0xFFFFFFFF;
}			
		
int nymth()//Nymph协议解析封装
{
	char stx[3],Dstx[3]="FE";
	char len[4];
	char sat[4],Dsat_1[4]="LOD",Dsat_2[4]="COR",Dsat_3[4]="SED",Dsat_4[4]="DIS",Dsat_5[4]="RES",Dsat_6[4]="RCO",Dsat_7[4]="CMD";
	char sid[3],Dsid[3]="00";
	char msg[255],crcmsg[255];
	char crs[11];
	char eof[3],Deof[3]="FD";
	char data[512];
	
	strncpy(stx, can_cache, 2);//判断协议头
	printf("协议头：%s\n",stx);
	if(strcmp(stx,Dstx)==0)
	{
		 strncpy(len, can_cache+2, 3);//提取数据长度
		 printf("数据长度（字符串）：%s\n",len);
		 long ss = atoi(len);//转换为整形变量用于接下来的字长计算
		 printf("数据长度（整型）：%d\n",ss);

		 strncpy(sat, can_cache+5, 3);//提取状态码
		 if(strcmp(sat,Dsat_1)==0)
		 {
				 printf("状态码：主机启动\n");
		 }
		 else if(strcmp(sat,Dsat_2)==0)
		 {
				 printf("状态码：申请控制\n");
		 }
		 else if(strcmp(sat,Dsat_3)==0)
		 {
				 printf("状态码：外设转发\n");
		 }
		 else if(strcmp(sat,Dsat_4)==0)
		 {
				 printf("状态码：外设显示\n");
		 }
		 else if(strcmp(sat,Dsat_5)==0)
		 {
				 printf("状态码：返回注册\n");
		 }
		 else if(strcmp(sat,Dsat_6)==0)
		 {
				 printf("状态码：返回控制\n");
		 }
		 else if(strcmp(sat,Dsat_7)==0)
		 {
				 printf("状态码：发送控制\n");
		 }
		 else
		 {
				 printf("状态码错误！\n");
		 }

		 strncpy(sid, can_cache+8, 2);//提取设备ID
		 if(strcmp(sid,Dsid)==0)
		 {
				printf("设备ID：主机发送\n");
		 }
		 else
		 {
				printf("设备ID：%s\n",sid);
		 }

		 strncpy(msg, can_cache+11, ss);//提取数据
		 printf("消息内容：%s\n",msg);
		 

		 strncpy(crs, can_cache+(12+ss), 10);//提取CRC校验码
		 printf("协议CRC校验码：%s\n",crs);
		 long dd = atoi(crs);//转换为整形变量用于接下来的CRC校验

		 int crcdouble = crc32(msg,strlen(msg));//消息内容CRC校验生成
		 printf("消息内容CRC校验码：%010d\n",crc32(msg,strlen(msg)));
		 if(dd == crcdouble)
		 {
				 printf("消息内容校验通过，协议正确！\n");
		 }
		 else
		 {
				 printf("消息内容校验失败，请重新传输！\n");
		 }
		 
		 strncpy(eof, can_cache+(22+ss), 2);//提取协议尾
		 printf("协议尾：%s\n",eof);
		 if(strcmp(eof,Deof)==0)
		 {
				printf("协议结束！\n");
		 }
		 else
		 {
				printf("协议尾错误！\n");
		 }
	}
	else
	{
			printf("错误！不是标准的 Nymth 协议！\n");
	}
	free(can_cache);
	return 0;
}		
		
int main()//主函数
{
  delay_init(168);
	OLED_Init(); 
	OLED_Clear();
	OLED_DrawBMP(0,0,128,8,BMP1);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,3,CAN_Mode_Normal); 
	u8 canbuf[8];
	u8 Can_Send_Flag;
	u8 can_i = 0;
	u8 can_a = 0;
	u8 can_key;
	
	
	u8 flag_new=0;
	char c1='F';
	char c2='D';
	
	
	BSPinit();
  delay_nms(30);
  printf("\rAirDwing Nymph v1.1 \r\n");
  printf("http://www.airdwing.com \r\n");
  delay_nms(3000);
	delay_nms(3000);
	delay_nms(3000);
	delay_nms(3000);
	delay_nms(3000);
	delay_nms(3000);
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
		
		
		if(flag_error_getDroneVersion)//错误信息
		{
			OLED_ShowString(2,0,"Error:");
			OLED_ShowString(2,2,"GetDroneVersion");
			OLED_ShowString(2,4,"Please ReLoad");
			flag_error_getDroneVersion = 0;
		}
		else
		{
			//OLED_Clear();
			//OLED_ShowString(12,4,"Air Info");
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
		
CANReserve:	
		can_key=CAN1_Receive_Msg(canbuf);
		if(can_key)//接收到有数据
		{			
 			for(can_i=0;can_i<can_key;can_i++)
			{									   				
				can_cache[can_a]=canbuf[can_i];			
				if(flag_new)
				{
					if(c2==canbuf[can_i])
					{
						can_a++;
						for(int w=-1;w<can_a;w++)
						{
							USART_SendData(USART2, can_cache[w]);//向串口1发送数据
							while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//等待发送结束
						}
						can_a=0;
						printf("\r\n");//插入换行
						nymth();
						goto CANReserve;
					}
					else
						flag_new=0;
				}
				if(c1==canbuf[can_i])flag_new=1;
				can_a++;
			}
		}
  }
}

