#include "main.h"

//Timer 2 32-bit counter  
//Timer Clock is 168MHz / 4 * 2 = 84M
float aim,last,pre;
float initial = 0;
float last=0;
float pk=0.9,pi=0.1,pd=3;
int current;
#define MAXYAW 2000
#define MINYAW -2000
int flag=0;

void control(float angle){
	 flag=1;
	 aim = angle;
	 last=pre=0;
	 initial = GMYawEncoder.ecd_angle;
	 current = 0;
	 printf("a%f\n",initial);
}
	

float constrain(float t){
	if(t>MINYAW&&t<MAXYAW) return t;
	else if(t>=MAXYAW) return MAXYAW;
	else	return MINYAW; 
}
int mysum=0;
void pid()
{
	 if(!flag) return;
	 float diff = (GMYawEncoder.ecd_angle - initial)-aim;
   int out = diff*pk + last*pi;
	 //if(last> 20000||last<-20000) last=0;
	 last+=diff;
	 current=current+out;
	 out=constrain(out);
	 if(diff >=-1&&diff <= 1){
		    Set_Gimbal_Current(CAN2,-out,0);
		    delay_us(250);
				out=0,flag=0,printf("end%f\t%f\n",GMYawEncoder.ecd_angle-initial,GMYawEncoder.ecd_angle);
				Set_Gimbal_Current(CAN2,0,0);
	 } 
	 mysum++;
	 if(mysum==50) mysum=0,printf("%d\t%f\t\n",out,GMYawEncoder.ecd_angle-initial);
	 Set_Gimbal_Current(CAN2,out,0);
}

void control1(float angle){
	 flag=1;
	 aim = angle;
	 last=pre=0;
	 initial = GMPitchEncoder.ecd_angle;
	 current = 0;
	 printf("a%f\n",initial);
}
	

float constrain1(float t){
	if(t>MINYAW&&t<MAXYAW) return t;
	else if(t>=MAXYAW) return MAXYAW;
	else	return MINYAW; 
}
int mysum1=0;
void pid1()
{
	 if(!flag) return;
	 float diff = (GMPitchEncoder.ecd_angle - initial)-aim;
   int out = diff*pk + last*pi;
	 //if(last> 20000||last<-20000) last=0;
	 last+=diff;
	 current=current+out;
	 out=constrain(out);
	 if(diff >=-1&&diff <= 1){
		    Set_Gimbal_Current(CAN2,0,-out);
		    delay_us(250);
				out=0,flag=0,printf("end%f\t%f\n",GMPitchEncoder.ecd_angle-initial,GMPitchEncoder.ecd_angle);
				Set_Gimbal_Current(CAN2,0,0);
	 } 
	 mysum1++;
	 if(mysum==50) mysum1=0,printf("%d\t%f\t\n",out,GMPitchEncoder.ecd_angle-initial);
	 Set_Gimbal_Current(CAN2,0,out);
}
	
void TIM8_Configuration(void)
{
		TIM_TimeBaseInitTypeDef tim;	 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE);    
  	tim.TIM_Period = 0xFFFFFFFF;     
  	tim.TIM_Prescaler = 168-1;	 //1M ��ʱ��  
  	tim.TIM_ClockDivision = TIM_CKD_DIV1;	
  	tim.TIM_CounterMode = TIM_CounterMode_Up;  
		TIM_ARRPreloadConfig(TIM8, ENABLE);	
		//Ӧ�����õ�TIM8 
  	TIM_TimeBaseInit(TIM8, &tim);
		// ʹ��TIM2���ؼĴ���ARR
  	TIM_ARRPreloadConfig(TIM8, ENABLE);	
	  TIM_PrescalerConfig(TIM8, 0, TIM_PSCReloadMode_Update);
		
		/* Disable the TIM2 Update event */
		TIM_UpdateDisableConfig(TIM8, ENABLE);
	  TIM_Cmd(TIM8,ENABLE);	   
}



void TIM6_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  tim;
    NVIC_InitTypeDef         nvic;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
    nvic.NVIC_IRQChannel = TIM6_DAC_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 1;
    nvic.NVIC_IRQChannelSubPriority = 0;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);

    tim.TIM_Prescaler = 84-1;        //84M internal clock
    tim.TIM_CounterMode = TIM_CounterMode_Up;
    tim.TIM_ClockDivision = TIM_CKD_DIV1;
    tim.TIM_Period = 1000;  //1ms,1000Hz
    TIM_TimeBaseInit(TIM6,&tim);
}

void TIM6_Start(void)
{
	  flag=0;
    TIM_Cmd(TIM6, ENABLE);	 
    TIM_ITConfig(TIM6, TIM_IT_Update,ENABLE);
    TIM_ClearFlag(TIM6, TIM_FLAG_Update);	
}

void TIM4_Configuration(void)
{
    TIM_TimeBaseInitTypeDef tim;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);    
    tim.TIM_Period = 0xFFFFFFFF;     
    tim.TIM_Prescaler = 168-1;	 //1M ��ʱ��  
    tim.TIM_ClockDivision = TIM_CKD_DIV1;	
    tim.TIM_CounterMode = TIM_CounterMode_Up;  
    TIM_ARRPreloadConfig(TIM4, ENABLE);	
    //Ӧ�����õ�TIM2 
    TIM_TimeBaseInit(TIM4, &tim);
    // ʹ��TIM2���ؼĴ���ARR
    TIM_ARRPreloadConfig(TIM4, ENABLE);	
    TIM_PrescalerConfig(TIM4, 0, TIM_PSCReloadMode_Update);
    /* Disable the TIM2 Update event */
    TIM_UpdateDisableConfig(TIM4, ENABLE);
    TIM_Cmd(TIM4,ENABLE);	
}

void TIM2_Configuration(void)
{
    TIM_TimeBaseInitTypeDef tim;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
    tim.TIM_Period = 0xFFFFFFFF;
    tim.TIM_Prescaler = 84 - 1;	 //1M ��ʱ��  
    tim.TIM_ClockDivision = TIM_CKD_DIV1;	
    tim.TIM_CounterMode = TIM_CounterMode_Up;  
    TIM_ARRPreloadConfig(TIM2, ENABLE);	
    TIM_TimeBaseInit(TIM2, &tim);

    TIM_Cmd(TIM2,ENABLE);	
}
   
void TIM2_IRQHandler(void)
{
	  if (TIM_GetITStatus(TIM2,TIM_IT_Update)!= RESET) 
		{
			  TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
        TIM_ClearFlag(TIM2, TIM_FLAG_Update);
			  BOTH_LED_TOGGLE();
		}
} 

void TIM6_DAC_IRQHandler(void)  
{
	
    if (TIM_GetITStatus(TIM6,TIM_IT_Update)!= RESET) 
	  {
		TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
        TIM_ClearFlag(TIM6, TIM_FLAG_Update);
		   	  //pid();
			    pid1();
			//Control_Task();         //���̡���̨��������
    }
}

int32_t ms_count = 0;
uint32_t Get_Time_Micros(void)
{
	return TIM2->CNT;
}


