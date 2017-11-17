#ifndef __MPU6050_DRIVER_H__
#define __MPU6050_DRIVER_H__
#include "main.h"

typedef struct __MPU6050_RAW_Data__
{
    short Accel_X;  //�Ĵ���ԭ��X����ٶȱ�ʾֵ
    short Accel_Y;  //�Ĵ���ԭ��Y����ٶȱ�ʾֵ
    short Accel_Z;  //�Ĵ���ԭ��Z����ٶȱ�ʾֵ
    short Temp;     //�Ĵ���ԭ���¶ȱ�ʾֵ
    short Gyro_X;   //�Ĵ���ԭ��X�������Ǳ�ʾֵ
    short Gyro_Y;   //�Ĵ���ԭ��Y�������Ǳ�ʾֵ
    short Gyro_Z;   //�Ĵ���ԭ��Z�������Ǳ�ʾֵ
	  short Mag_X;   //�Ĵ���ԭ��X�������Ǳ�ʾֵ
    short Mag_Y;   //�Ĵ���ԭ��Y�������Ǳ�ʾֵ
    short Mag_Z;   //�Ĵ���ԭ��Z�������Ǳ�ʾֵ
	
}MPU6050_RAW_DATA;

typedef struct __MPU6050_REAL_Data__
{
    float Accel_X;  //ת����ʵ�ʵ�X����ٶȣ�
    float Accel_Y;  //ת����ʵ�ʵ�Y����ٶȣ�
    float Accel_Z;  //ת����ʵ�ʵ�Z����ٶȣ�
    float Temp;     //ת����ʵ�ʵ��¶ȣ���λΪ���϶�
    float Gyro_X;   //ת����ʵ�ʵ�X��Ǽ��ٶȣ�
    float Gyro_Y;   //ת����ʵ�ʵ�Y��Ǽ��ٶȣ�
    float Gyro_Z;   //ת����ʵ�ʵ�Z��Ǽ��ٶ�
	  float Mag_X;   //ת����ʵ�ʵ�X��Ǽ��ٶȣ�
    float Mag_Y;   //ת����ʵ�ʵ�Y��Ǽ��ٶȣ�
    float Mag_Z;   //ת����ʵ�ʵ�Z��Ǽ��ٶ�
	
}MPU6050_REAL_DATA;

//define the eluer angle
typedef struct AHRS
{
	float pitch;
	float roll;
	float yaw;
	
}AHRS;
extern AHRS ahrs;

//the max and min data of the mag
typedef __packed struct
{
	int16_t MaxMagX;
	int16_t MaxMagY;
	int16_t MaxMagZ;
	int16_t MinMagX;
	int16_t MinMagY;
	int16_t MinMagZ;
}MagMaxMinData_t;
extern MagMaxMinData_t MagMaxMinData;

#define GYRO_CALI_FLAG 	  		(1<<0)
#define HMC5883_CALI_FLAG 		(1<<1)
#define ENCODER_CALI_FLAG 		(1<<2)
#define ALL_SENSOR_CALI_FLAG	(GYRO_CALI_FLAG|HMC5883_CALI_FLAG|ENCODER_CALI_FLAG)

extern volatile MPU6050_RAW_DATA    MPU6050_Raw_Data; 
extern volatile MPU6050_REAL_DATA   MPU6050_Real_Data;
extern uint8_t mpu_buf[20];
extern int16_t  HMC5883_maxx,HMC5883_maxy,HMC5883_maxz,HMC5883_minx,HMC5883_miny,HMC5883_minz;

int MPU6050_Init(void);
uint8_t HMC5883_Init(void);
void HMC58X3_getlastValues(int16_t *x,int16_t *y,int16_t *z);
void MPU6050_Initialize(void);
void MPU6050_InitGyro_Offset_Start(void);
void MPU6050_InitGyro_Offset_Save(void);
void Init_Encoder_Offset_Start(void);
void Init_Encoder_Offset_Save(void);
int MPU6050_ReadData(uint8_t Slave_Addr, uint8_t Reg_Addr, uint8_t * Data, uint8_t Num);
int MPU6050_EnableInt(void);
void HMC58X3_mgetValues(volatile float *arry);
void HMC58X3_ReadData(u8 *vbuff);
void MPU6050_DataSave(int16_t ax,int16_t ay,int16_t az,int16_t wax,int16_t way,int16_t waz,int16_t wx,int16_t wy,int16_t wz);
void  HMC58X3_newValues(int16_t x,int16_t y,int16_t z);
void HMC5883L_Start_Calib(void);
void HMC5883L_Save_Calib(void);
void MPU6050_getlastMotion(float* ax, float* ay, float* az,float* wax, float* way,float* waz,float* wx, float* wy, float* wz);
void MPU6050_getMotion(float* ax, float* ay, float* az, float* wax, float* way, float* waz,float* wx, float* wy, float* wz);
void Reset_MAG_MinMaxVal(void);
void MPU6050_getSpeed(float* Spx,float* Spy,float* Acc_x,float* Acc_y,float* last_Acc_x,float* last_Acc_y);
void MPU6050_getDisplacement(float* Disx,float* Disy,float* Spx,float* Spy);
void MPU6050_getlastDisplacement(float* Disx,float* Disy);
void MPU6050_getlastSpeed(float* Spx,float* Spy);
#endif


