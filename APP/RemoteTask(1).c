#include <stm32f4xx.h>
#include "main.h"

RC_Ctl_t RC_CtrlData;   //remote control data
ChassisSpeed_Ref_t ChassisSpeedRef;
Gimbal_Ref_t GimbalRef;
FrictionWheelState_e friction_wheel_state = FRICTION_WHEEL_OFF;
static RemoteSwitch_t switch1;   //ң������ದ��
static volatile Shoot_State_e shootState = NOSHOOTING;
static InputMode_e inputmode = REMOTE_INPUT;   //����ģʽ�趨

RampGen_t frictionRamp = RAMP_GEN_DAFAULT;  //Ħ����б��
RampGen_t LRSpeedRamp = RAMP_GEN_DAFAULT;   //mouse�����ƶ�б��
RampGen_t FBSpeedRamp = RAMP_GEN_DAFAULT;   //mouseǰ���ƶ�б��
 
extern uint8_t receive_from_pi;
 
void GetRemoteSwitchAction(RemoteSwitch_t *sw, uint8_t val)
{
	static uint32_t switch_cnt = 0;

	/* ����״ֵ̬ */
	sw->switch_value_raw = val;
	sw->switch_value_buf[sw->buf_index] = sw->switch_value_raw;

	/* ȡ����ֵ����һ��ֵ */
	sw->switch_value1 = (sw->switch_value_buf[sw->buf_last_index] << 2)|
	(sw->switch_value_buf[sw->buf_index]);


	/* ���ϵ�״ֵ̬������ */
	sw->buf_end_index = (sw->buf_index + 1)%REMOTE_SWITCH_VALUE_BUF_DEEP;

	/* �ϲ�����ֵ */
	sw->switch_value2 = (sw->switch_value_buf[sw->buf_end_index]<<4)|sw->switch_value1;	

	/* �����ж� */
	if(sw->switch_value_buf[sw->buf_index] == sw->switch_value_buf[sw->buf_last_index])
	{
		switch_cnt++;	
	}
	else
	{
		switch_cnt = 0;
	}

	if(switch_cnt >= 40)
	{
		sw->switch_long_value = sw->switch_value_buf[sw->buf_index]; 	
	}

	//����ѭ��
	sw->buf_last_index = sw->buf_index;
	sw->buf_index++;		
	if(sw->buf_index == REMOTE_SWITCH_VALUE_BUF_DEEP)
	{
		sw->buf_index = 0;	
	}			
}
//return the state of the remote 0:no action 1:action 
uint8_t IsRemoteBeingAction(void)
{
	return (fabs(ChassisSpeedRef.forward_back_ref)>=10 || fabs(ChassisSpeedRef.left_right_ref)>=10 || fabs(GimbalRef.yaw_speed_ref)>=10 || fabs(GimbalRef.pitch_speed_ref)>=10);
}

//����ģʽ���� 
void SetInputMode(Remote *rc)
{
	if(rc->s2 == 1)
	{
		inputmode = REMOTE_INPUT;
	}
	else if(rc->s2 == 3)
	{
		inputmode = KEY_MOUSE_INPUT;
	}
	else if(rc->s2 == 2)
	{
		inputmode = STOP;
	}	
}

InputMode_e GetInputMode()
{
	return inputmode;
}

/*
input: RemoteSwitch_t *sw, include the switch info
*/
void RemoteShootControl(RemoteSwitch_t *sw, uint8_t val) 
{
	GetRemoteSwitchAction(sw, val);
	switch(friction_wheel_state)
	{
		case FRICTION_WHEEL_OFF:
		{
			if(sw->switch_value1 == REMOTE_SWITCH_CHANGE_3TO1)   //�ӹرյ�start turning
			{
			}				 		
		}break;
		case FRICTION_WHEEL_START_TURNNING:
		{
			if(sw->switch_value1 == REMOTE_SWITCH_CHANGE_3TO1)   //�������ͱ��ر�
			{

			}
			else
			{

			}
		}break;
		case FRICTION_WHEEL_ON:
		{
			if(sw->switch_value1 == REMOTE_SWITCH_CHANGE_3TO1)   //�ر�Ħ����
			{

			}
			else if(sw->switch_value_raw == 2)
			{

			}
			else
			{
			}					 
		} break;				
	}
}
	 //��ֱ���ˣ�����͵�ƽ�����ߡ�ˮƽ������͵�ƽ����
   //ˮƽ����D15,��ֱD14
void MouseShootControl(Mouse *mouse)
{
	if((mouse->press_l== 1)&&(mouse->press_r== 0)) //ˮƽ������ӽ�
	{
		 GPIO_SetBits(GPIOD, GPIO_Pin_15);
		 delay_us(10); 
     PWM1 = 100;             
	}
  else if((mouse->press_l== 0)&&(mouse->press_r== 1))
  {
		 GPIO_ResetBits(GPIOD, GPIO_Pin_15);  //��
		 delay_us(10); 
     PWM1 = 100;             
	}
	else
		 PWM1=0;
	
}

void RemoteDataPrcess(uint8_t *pData)
{
    if(pData == NULL)
    {
        return;
    }
    
    RC_CtrlData.rc.ch0 = ((int16_t)pData[0] | ((int16_t)pData[1] << 8)) & 0x07FF; 
    RC_CtrlData.rc.ch1 = (((int16_t)pData[1] >> 3) | ((int16_t)pData[2] << 5)) & 0x07FF;
    RC_CtrlData.rc.ch2 = (((int16_t)pData[2] >> 6) | ((int16_t)pData[3] << 2) |
                         ((int16_t)pData[4] << 10)) & 0x07FF;
    RC_CtrlData.rc.ch3 = (((int16_t)pData[4] >> 1) | ((int16_t)pData[5]<<7)) & 0x07FF;
    
    RC_CtrlData.rc.s1 = ((pData[5] >> 4) & 0x000C) >> 2;
    RC_CtrlData.rc.s2 = ((pData[5] >> 4) & 0x0003);

    RC_CtrlData.mouse.x = ((int16_t)pData[6]) | ((int16_t)pData[7] << 8);
    RC_CtrlData.mouse.y = ((int16_t)pData[8]) | ((int16_t)pData[9] << 8);
    RC_CtrlData.mouse.z = ((int16_t)pData[10]) | ((int16_t)pData[11] << 8);    

    RC_CtrlData.mouse.press_l = pData[12];
    RC_CtrlData.mouse.press_r = pData[13];
 
    RC_CtrlData.key.v = ((int16_t)pData[14]);// | ((int16_t)pData[15] << 8);
	
	SetInputMode(&RC_CtrlData.rc);
	
	//RemoteControlProcess(&(RC_CtrlData.rc));
//	char temp[50];
//		sprintf(temp,"%d",RC_CtrlData.rc.ch0);	
//   	UART3_PutStr (temp);//�����ַ���
	switch(GetInputMode())
	{
		case REMOTE_INPUT:
		{
			//ң��������ģʽ
			RemoteControlProcess(&(RC_CtrlData.rc));
		}break;
		case KEY_MOUSE_INPUT:
		{
			//ң��������ģʽ
			if(receive_from_pi == 0)
				MouseKeyControlProcess(&RC_CtrlData.mouse,&RC_CtrlData.key);
		}break;
		case STOP:
		{
			//����ͣ��
		}break;
	}
}
//ң��������ģʽ����
void RemoteControlProcess(Remote *rc)
{
    if(GetWorkState()!=PREPARE_STATE)
    {
        ChassisSpeedRef.forward_back_ref = 0.9*(RC_CtrlData.rc.ch1 - (int16_t)REMOTE_CONTROLLER_STICK_OFFSET) * STICK_TO_CHASSIS_SPEED_REF_FACT;
        ChassisSpeedRef.left_right_ref   = 1.4*
			(rc->ch0 - (int16_t)REMOTE_CONTROLLER_STICK_OFFSET) * STICK_TO_CHASSIS_SPEED_REF_FACT; 
    }

    if(GetWorkState() == NORMAL_STATE)
    {
        GimbalRef.pitch_angle_dynamic_ref += 0.3*(rc->ch3 - (int16_t)REMOTE_CONTROLLER_STICK_OFFSET) * STICK_TO_PITCH_ANGLE_INC_FACT;
        GimbalRef.yaw_angle_dynamic_ref   = 1.4*(rc->ch2 - (int16_t)REMOTE_CONTROLLER_STICK_OFFSET) * STICK_TO_YAW_ANGLE_INC_FACT;  
	  }
	
	/* not used to control, just as a flag */ 
    GimbalRef.pitch_speed_ref = rc->ch3 - (int16_t)REMOTE_CONTROLLER_STICK_OFFSET;    //speed_ref�����������ж���
    GimbalRef.yaw_speed_ref   = (rc->ch2 - (int16_t)REMOTE_CONTROLLER_STICK_OFFSET);
	GimbalAngleLimit();
	//���������˶������ϱպ�
	if((rc->ch3)>1236)
	{
		 GPIO_SetBits(GPIOD, GPIO_Pin_15);
		 delay_us(10); 
     PWM1 = 100;  
	}
	else if((rc->ch3)<888)
	{
		 GPIO_ResetBits(GPIOD, GPIO_Pin_15);  //��
		 delay_us(10); 
     PWM1 = 100;    
	}
	else
	{
		PWM1=0;
	}
	
	//ң�����������ݴ���	
	if(rc->s1==1)  //1��3��2��������
	{
			GPIO_ResetBits(GPIOD, GPIO_Pin_14);
			delay_us(10); 
			PWM2=100;
	}//����
	else if((rc->s1==2)&&(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15)))
	{
			GPIO_SetBits(GPIOD, GPIO_Pin_14);
			delay_us(10);
			PWM2=100;
	}
	else //ֹͣ
	{
		PWM2=0;
	}
	//RemoteShootControl(&switch1, rc->s1);
		
}
//����������ģʽ����
void MouseKeyControlProcess(Mouse *mouse, Key *key)
{
	static uint16_t forward_back_speed = 0;
	static uint16_t left_right_speed = 0;
    if(GetWorkState()!=PREPARE_STATE)
    {
		//speed mode: normal speed/high speed
	 //��ֱ���ˣ�����͵�ƽ�����ߡ�ˮƽ������͵�ƽ����
   //ˮƽ����D15,��ֱD14
		if(key->v & 0x40)  //shift���� ,����ͷƽ̨������
		{
			if(key->v & 0x10)//Q ����
			{
				 GPIO_ResetBits(GPIOD, GPIO_Pin_13);
         GPIO_SetBits(GPIOD, GPIO_Pin_12);
			}
			else if(key->v & 0x20)   //E �½�
			{
				 GPIO_ResetBits(GPIOD, GPIO_Pin_12);
         GPIO_SetBits(GPIOD, GPIO_Pin_13);
			}
			else
			{
				 GPIO_ResetBits(GPIOD, GPIO_Pin_12);
         GPIO_ResetBits(GPIOD, GPIO_Pin_13);
			}
		}
		else  
		{
				if(key->v & 0x10)//Q ����
				{
					 GPIO_ResetBits(GPIOD, GPIO_Pin_14);
					 delay_us(10); 
					 PWM2=100;
				}
				else if((key->v & 0x20)&&(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15)))   //E �½�
				{
					 GPIO_SetBits(GPIOD, GPIO_Pin_14);
					 delay_us(10);
					 PWM2=100;
				}
				else
					 PWM2=0;
		}
    
		if(key->v & 0x80)
		{
			forward_back_speed =  HIGH_FORWARD_BACK_SPEED;
			left_right_speed = HIGH_LEFT_RIGHT_SPEED;
		}
		else
		{
			forward_back_speed =  NORMAL_FORWARD_BACK_SPEED;
			left_right_speed = NORMAL_LEFT_RIGHT_SPEED;
		}
		//movement process
		if(key->v & 0x01)  // key: w
		{
			ChassisSpeedRef.forward_back_ref = 0.5*forward_back_speed* FBSpeedRamp.Calc(&FBSpeedRamp);
		}
		else if(key->v & 0x02) //key: s
		{
			ChassisSpeedRef.forward_back_ref = -0.5*forward_back_speed* FBSpeedRamp.Calc(&FBSpeedRamp);
		}
		else
		{
			ChassisSpeedRef.forward_back_ref = 0;
			FBSpeedRamp.ResetCounter(&FBSpeedRamp);
		}
		
		
		if(key->v & 0x04)  // key: d
		{
			ChassisSpeedRef.left_right_ref = -0.5*left_right_speed* LRSpeedRamp.Calc(&LRSpeedRamp);
		}
		else if(key->v & 0x08) //key: a
		{
			ChassisSpeedRef.left_right_ref = 0.5*left_right_speed* LRSpeedRamp.Calc(&LRSpeedRamp);
		}
		else
		{
			ChassisSpeedRef.left_right_ref = 0;
			LRSpeedRamp.ResetCounter(&LRSpeedRamp);
		}
	}
	//step2: gimbal ref calc
    if(GetWorkState() == NORMAL_STATE)
    {
				VAL_LIMIT(mouse->x, -150, 150); 
				VAL_LIMIT(mouse->y, -150, 150); 
			
		    if(receive_from_pi == 0)
				{
					GimbalRef.pitch_angle_dynamic_ref -= mouse->y* MOUSE_TO_PITCH_ANGLE_INC_FACT;  //(rc->ch3 - (int16_t)REMOTE_CONTROLLER_STICK_OFFSET) * STICK_TO_PITCH_ANGLE_INC_FACT;
					GimbalRef.yaw_angle_dynamic_ref   = 2*mouse->x* MOUSE_TO_YAW_ANGLE_INC_FACT;
				}
		}
	
	/* not used to control, just as a flag */ 
    GimbalRef.pitch_speed_ref = mouse->y;    //speed_ref�����������ж���
    GimbalRef.yaw_speed_ref   = mouse->x;
	GimbalAngleLimit();	
	MouseShootControl(mouse);
	
}

//
Shoot_State_e GetShootState()
{
	return shootState;
}

void SetShootState(Shoot_State_e v)
{
	shootState = v;
}

FrictionWheelState_e GetFrictionState()
{
	return friction_wheel_state;
}

void SetFrictionState(FrictionWheelState_e v)
{
	friction_wheel_state = v;
}
//ң��������ֵ���ã�
void GimbalAngleLimit()
{
	VAL_LIMIT(GimbalRef.pitch_angle_dynamic_ref, -PITCH_MAX, 2*PITCH_MAX); //PITCH_MAX);
	//VAL_LIMIT(GimbalRef.yaw_angle_dynamic_ref, GMYPositionPID.fdb - 60, GMYPositionPID.fdb + 60);
}

//ң�������ݳ�ʼ����б�º����ȵĳ�ʼ��
void RemoteTaskInit()
{
	//б�³�ʼ��
	frictionRamp.SetScale(&frictionRamp, FRICTION_RAMP_TICK_COUNT);
	LRSpeedRamp.SetScale(&LRSpeedRamp, MOUSE_LR_RAMP_TICK_COUNT);
	FBSpeedRamp.SetScale(&FBSpeedRamp, MOUSR_FB_RAMP_TICK_COUNT);
	frictionRamp.ResetCounter(&frictionRamp);
	LRSpeedRamp.ResetCounter(&LRSpeedRamp);
	FBSpeedRamp.ResetCounter(&FBSpeedRamp);
	//������̨����ֵ��ʼ��
	GimbalRef.pitch_angle_dynamic_ref = 0.0f;
	GimbalRef.yaw_angle_dynamic_ref = 0.0f;
	ChassisSpeedRef.forward_back_ref = 0.0f;
	ChassisSpeedRef.left_right_ref = 0.0f;
	ChassisSpeedRef.rotate_ref = 0.0f;
	//Ħ��������״̬��ʼ��
	SetFrictionState(FRICTION_WHEEL_OFF);
}
