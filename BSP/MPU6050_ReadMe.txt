MPU�ӿ�ʹ��˵��V1.0

1.���ߣ�MPU6050����USART2����stm32���İ�ͨ�ţ���Ӧ
TX-PD5
RX-PD6

2.���빤�̣�
��ѹ�������ĸ��ļ���ӵ�BSP�ļ����£���Keil������c�ļ���ӵ������У�
������main.h��������ļ���ͷ�ļ�include��ȥ�����£�

#include"usart2.h"
#include"MPU6050_driver.h"

(���������������MPU6050���ļ������Ȱ���Щ�ļ��Ƴ����̣��ٽ������ϲ���)

3.�ӿڽ���

int MPU6050_Init(void)
���ܣ���ʼ��MPU6050������main�����е���
����ֵ����

void MPU6050_getlastMotion(float* ax, float* ay, float* az,float* wax, float* way,float* waz,float* wx, float* wy, float* wz)
���ܣ���ȡMPU6050��ǰ����ֵ
����ֵ�������ò������أ�����Ϊfloat��
          ax,ay,az---x,y,z����ٶ�
          wax,way,waz---x,y,z����ٶ�
          wa,wy,wz---x,y,z��Ƕ�


void MPU6050_getlastSpeed(float* Spx,float* Spy)
���ܣ���ȡ��ǰ�ٶ�
����ֵ�������ò������أ�����Ϊfloat��
         Spx---x���ٶ�
         Spy---y���ٶ�


void MPU6050_getlastDisplacement(float* Disx,float* Disy)
���ܣ���ȡ��ǰ�ٶ�
����ֵ�������ò������أ�����Ϊfloat��
         Disx---x���ٶ�
         Disy---y���ٶ�
