#include "log.h"


u32 filelog(void)
{
	u32 total,free;
	u8 t=0;	
	
	//mf_open("0:/xzl.txt",0x08);
	
//	exfuns_init();							//Ϊfatfs��ر��������ڴ�				 
//  f_mount(fs[0],"0:",1); 					//����SD�� 	
										    	  
	while(exf_getfree("0",&total,&free))	//�õ�SD������������ʣ������
	{
		printf("SD Card Fatfs Error!\n");
		delay_ms(200);
		delay_ms(200);
	}													  			         
  printf("FATFS OK!\n");
	printf("SD Total Size:%d MB\n",total>>10);
  printf("SD Free Size:%d MB\n",free>>10);	
	
//	mf_open("0:/1231.txt",0x04);
//	mf_open("0:/1.txt",0x04);
//	mf_open("0:/13.txt",0x04);
//	printf("Buid 1231.txt\n");	
//	
//	while(1)
//	{
//		t++; 
//		delay_ms(200);		 			   
//	} 
}


