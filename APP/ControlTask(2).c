#include "main.h"

PID_Regulator_t CMRotatePID = CHASSIS_MOTOR_ROTATE_PID_DEFAULT; 
PID_Regulator_t CM1SpeedPID = CHASSIS_MOTOR_SPEED_PID_DEFAULT;
PID_Regulator_t CM2SpeedPID = CHASSIS_MOTOR_SPEED_PID_DEFAULT;
PID_Regulator_t CM3SpeedPID = CHASSIS_MOTOR_SPEED_PID_DEFAULT;
PID_Regulator_t CM4SpeedPID = CHASSIS_MOTOR_SPEED_PID_DEFAULT;


/*--------------------------------------------CTRL Variables----------------------------------------*/
WorkState_e lastWorkState = PREPARE_STATE;
WorkState_e workState = PREPARE_STATE;
RampGen_t GMPitchRamp = RAMP_GEN_DAFAULT;
RampGen_t GMYawRamp = RAMP_GEN_DAFAULT;
	
float error_now=0,error_last=0;
float error_now_yaw=0,error_last_yaw=0;

/*
*********************************************************************************************************
*                                            FUNCTIONS 
*********************************************************************************************************
*/

static void SetWorkState(WorkState_e state)
{
    workState = state;
}

WorkState_e GetWorkState()
{
	return workState;
}
//���̿�������
void CMControlLoop(void)
{  
	//������ת������
	if(GetWorkState()==PREPARE_STATE) //�����׶Σ����̲���ת
	{
		ChassisSpeedRef.rotate_ref = 0;	 
	}
	else if(GimbalRef.yaw_angle_dynamic_ref == 0.0)
	{
		ChassisSpeedRef.rotate_ref = 0;
	}
	else
	{
		   ChassisSpeedRef.rotate_ref = 5*GimbalRef.yaw_angle_dynamic_ref;
	}
	
	CM1SpeedPID.ref =  -ChassisSpeedRef.forward_back_ref*0.075 + ChassisSpeedRef.left_right_ref*0.075 + 0.9*ChassisSpeedRef.rotate_ref;
	CM2SpeedPID.ref = ChassisSpeedRef.forward_back_ref*0.075 + ChassisSpeedRef.left_right_ref*0.075 + 0.9*ChassisSpeedRef.rotate_ref;
	CM3SpeedPID.ref = ChassisSpeedRef.forward_back_ref*0.075 - ChassisSpeedRef.left_right_ref*0.075 + 0.9*ChassisSpeedRef.rotate_ref;
	CM4SpeedPID.ref = -ChassisSpeedRef.forward_back_ref*0.075 - ChassisSpeedRef.left_right_ref*0.075 + 0.9*ChassisSpeedRef.rotate_ref;

	CM1SpeedPID.fdb = 0.04*CM1Encoder.filter_rate;
	CM2SpeedPID.fdb = 0.04*CM2Encoder.filter_rate;
	CM3SpeedPID.fdb = 0.04*CM3Encoder.filter_rate;
	CM4SpeedPID.fdb = 0.04*CM4Encoder.filter_rate;
	
	CM1SpeedPID.Calc(&CM1SpeedPID);
	CM2SpeedPID.Calc(&CM2SpeedPID);
	CM3SpeedPID.Calc(&CM3SpeedPID);
	CM4SpeedPID.Calc(&CM4SpeedPID);
	
	 if((GetWorkState() == STOP_STATE) || GetWorkState() == CALI_STATE || GetWorkState() == PREPARE_STATE)//if((GetWorkState() == STOP_STATE) ||Is_Serious_Error() || GetWorkState() == CALI_STATE || GetWorkState() == PREPARE_STATE)    //|| dead_lock_flag == 1����ͣ����������У׼���޿�������ʱ����ʹ���̿���ֹͣ
	 {
		 Set_CM_Speed(CAN2, 0,0,0,0);
	 }
	 else
	 {
		 Set_CM_Speed(CAN2, CHASSIS_SPEED_ATTENUATION * CM1SpeedPID.output, CHASSIS_SPEED_ATTENUATION * CM2SpeedPID.output, CHASSIS_SPEED_ATTENUATION * CM3SpeedPID.output, CHASSIS_SPEED_ATTENUATION * CM4SpeedPID.output);		 
	 } 
//	 char temp[50];
//	 sprintf(temp,"ref:%10.3f  ",CM1SpeedPID.ref);	
//   UART3_PutStr (temp);//�����ַ���
//	 sprintf(temp,"fdb:%10.3f  ",CM1SpeedPID.fdb);	
//   UART3_PutStr (temp);//�����ַ���
//	 sprintf(temp,"output:%10.3f  ",CM1SpeedPID.output);	
//   UART3_PutStr (temp);//�����ַ���
//	 USART_SendData(USART3, '\n');
}
//�����������������
int16_t pwm_ccr = 0;

static uint32_t time_tick_1ms = 0;
//�������񣬷���timer6 1ms��ʱ�ж���ִ��
void Control_Task(void)
{
	time_tick_1ms++;
	WorkStateFSM();
	WorkStateSwitchProcess();
		

	//chassis motor control
	if(time_tick_1ms%4 == 0)         //motor control frequency 4ms
	{
		CMControlLoop();			 
	}
	
}
/**********************************************************
*����״̬�л�״̬��,��1ms��ʱ�ж�ͬƵ��
**********************************************************/

void WorkStateFSM(void)
{
	lastWorkState = workState;
	switch(workState)
	{
		case PREPARE_STATE:
		{
			if(GetInputMode() == STOP) //|| Is_Serious_Error())
			{
				workState = STOP_STATE;
			}
			else if(time_tick_1ms > PREPARE_TIME_TICK_MS)
			{
				workState = NORMAL_STATE;
			}			
		}break;
		case NORMAL_STATE:     
		{
			if(GetInputMode() == STOP) //|| Is_Serious_Error())
			{
				workState = STOP_STATE;
			}	
		}break;
		case STANDBY_STATE:     
		{
			if(GetInputMode() == STOP) //|| Is_Serious_Error())
			{
				workState = STOP_STATE;
			}
			else if(IsRemoteBeingAction() || (GetShootState()==SHOOTING) || GetFrictionState() == FRICTION_WHEEL_START_TURNNING)
			{
				workState = NORMAL_STATE;
			}				
		}break;
		case STOP_STATE:   
		{
			if(GetInputMode() != STOP) //&& !Is_Serious_Error())
			{
				workState = PREPARE_STATE;   
			}
		}break;
		case CALI_STATE:      
		{
			if(GetInputMode() == STOP) //|| Is_Serious_Error())
			{
				workState = STOP_STATE;
			}
		}break;	    
		default:
		{
			
		}
	}	
}

static void WorkStateSwitchProcess(void)
{
	//���������ģʽ�л���prapareģʽ��Ҫ��һϵ�в�����ʼ��
	if((lastWorkState != workState) && (workState == PREPARE_STATE))  
	{
		ControtLoopTaskInit();
		RemoteTaskInit();
	}
}

/*
************************************************************************************************************************
*Name        : GimbalYawControlModeSwitch
* Description: This function process the yaw angle ref and fdb according to the WORKSTATE.
* Arguments  : void     
* Returns    : void
* Note(s)    : 1) from NORMAL to STANDBY it need to delay a few seconds to wait for the IMU to be steady.  
                  STATE_SWITCH_DELAY_TICK represents the delay time.
************************************************************************************************************************
*/

//���������ʼ������
void ControtLoopTaskInit(void)
{
	//������ʼ��
	time_tick_1ms = 0;   //�ж��еļ�������
	//���ù���ģʽ
	SetWorkState(PREPARE_STATE);
    
	//PID��ʼ��
	CMRotatePID.Reset(&CMRotatePID);
	CM1SpeedPID.Reset(&CM1SpeedPID);
	CM2SpeedPID.Reset(&CM2SpeedPID);
	CM3SpeedPID.Reset(&CM3SpeedPID);
	CM4SpeedPID.Reset(&CM4SpeedPID);
}
