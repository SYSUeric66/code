#include "main.h"
uint32_t Upload_Speed = 1;   //�����ϴ��ٶ�  ��λ Hz
#define upload_time (1000000/Upload_Speed)  //�����ϴ���ʱ�䡣��λΪus
uint16_t  power1=0,power2=0;
uint32_t system_micrsecond;   //ϵͳʱ�� ��λms
int main(void)
{     			
	delay_ms(800); 
  //CAN1_Configuration();                    
	USART1_Configuration(100000);        
	//USART3_Configuration();     			
	while(1)
	{   
//	  char temp[50];
//		sprintf(temp,"%d",32);	
//   	UART3_PutStr (temp);//�����ַ���
  }
}
