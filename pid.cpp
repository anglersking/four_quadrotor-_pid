#include "pid.h"
#include"stdio.h"

_ALL_PID all;

//�洢pid����������
const float  controller_parameter[15][5] =
{
    //0.kp 1.ki 2.kd 3.�����޷�  4.pid����޷�ֵ  

    //��̬�⻷����  
    {2.55,  0,   0,  300 , 800 },          //pit_angle ������
    {2.55,  0,   0,  300 , 800 },          //rol_angle ������
    {3.2,  0,   0,  300 , 800 },          //yaw_angle ƫ����

    //��̬�ڻ�����
    {0.45,  0.0015,   0.15,  300 , 800 },          //pit_gyro
    {0.45,  0.0015,   0.15,  300 , 800 },          //rol_gyro
    {1.25,  0.0025,   0.15,  300 , 800 },          //yaw_gyro

    //��ֱ���߲���
    {0  ,0  ,0  ,200,800},                  //acc_high
    {3.1  ,0  ,0  ,200,800},                //vel_high
    {1.05  ,0  ,0  ,200,800},              //pos_high

    //x������� 
    {0  ,0  ,0  ,200,800},              //acc_x       
    {0.08  ,0.0  ,0.0  ,200,800},              //vel_x
    {2.2  ,0  ,0  ,200,800},              //pos_x

    //y�������
    {0  ,0  ,0  ,200,800},              //acc_y       
    {0.08  ,0.0  ,0.0  ,200,800},              //vel_y
    {2.2  ,0  ,0  ,200,800},              //pos_y    
};

//pid������ʼ������
void pid_init(_PID* controller, uint8_t label)
{
    controller->kp = controller_parameter[label][0];
    controller->ki = controller_parameter[label][1];
    controller->kd = controller_parameter[label][2];
    controller->integral_max = controller_parameter[label][3];
    controller->out_max = controller_parameter[label][4];
}
//pid������ʼ��
void all_pid_init(void)
{
    pid_init(&all.pit_angle, 0);
    pid_init(&all.rol_angle, 1);
    pid_init(&all.yaw_angle, 2);

    pid_init(&all.pit_gyro, 3);
    pid_init(&all.rol_gyro, 4);
    pid_init(&all.yaw_gyro, 5);

    pid_init(&all.acc_high, 6);
    pid_init(&all.vel_high, 7);
    pid_init(&all.pos_high, 8);

    pid_init(&all.acc_fix_x, 9);
    pid_init(&all.vel_fix_x, 10);
    pid_init(&all.pos_fix_x, 11);

    pid_init(&all.acc_fix_y, 12);
    pid_init(&all.vel_fix_y, 13);
    pid_init(&all.pos_fix_y, 14);
}
//pid������
float pid_controller(_PID* controller)
{
    controller->err = controller->err_last;                                                 //�����ϴ�ƫ��
    controller->err = controller->expect - controller->feedback;                            //ƫ�����

    controller->integral += controller->ki * controller->err;                               //����  

    //�����޷�
    if (controller->integral > controller->integral_max)     controller->integral = controller->integral_max;
    if (controller->integral < -controller->integral_max)     controller->integral = -controller->integral_max;

    //pid����
    controller->out = controller->kp * controller->err
        + controller->integral
        + controller->kd * (controller->err - controller->err_last);
    //����޷�
    if (controller->out > controller->out_max)   controller->out = controller->out_max;
    if (controller->out < -controller->out_max)   controller->out = -controller->out_max;

   // printf("controller->out:%f\n", controller->out);

    return  controller->out;
}
//�������
void clear_integral(_PID* controller)
{
    controller->integral = 0.0f;
}
