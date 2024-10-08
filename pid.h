#ifndef _pid_h_
#define _pid_h_

#define uint8_t int

typedef struct
{
    float err;
    float err_last;

    float expect;
    float feedback;

    float kp;
    float ki;
    float kd;

    float integral;
    float integral_max;

    float out;
    float out_max;
}_PID;


typedef struct
{
    //��̬�⻷
    _PID pit_angle;
    _PID rol_angle;
    _PID yaw_angle;
    //��̬�ڻ�
    _PID pit_gyro;
    _PID rol_gyro;
    _PID yaw_gyro;
    //��ֱ����
    _PID acc_high;
    _PID vel_high;
    _PID pos_high;

    //����
    _PID acc_fix_x;
    _PID vel_fix_x;
    _PID pos_fix_x;

    //����
    _PID acc_fix_y;
    _PID vel_fix_y;
    _PID pos_fix_y;

}_ALL_PID;


extern _ALL_PID all;

float pid_controller(_PID* controller);
void all_pid_init(void);
void clear_integral(_PID* controller);


#endif
