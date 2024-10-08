#include "controller.h"
#include "pid.h"
#include "stdio.h"
#include <iostream>



_CONTROLLER_CNT controller_cnt = { 0 };
_THROTTLE_TYPE  throttle_type = { 0 };
_CONTROLLER_MODE _control = { 0 };

#define throttle_flying  10

_OUT_Motor Motor1 = { 0 };
_OUT_Motor Motor2 = { 0 };
_OUT_Motor Motor3 = { 0 };
_OUT_Motor Motor4 = { 0 };

//�⻷�Ƕȿ�����
void angle_controller(int rolrtos,int pitrtos,int yawrtos,int expecyaw)//��  mpu6050�� rol pit yaw  ����  һ��pid����һ��*5������
{
    static uint16_t yaw_init_cnt = 0;
    //Ԫ�������rol������
    all.rol_angle.feedback = rolrtos;// att.rol;MPU6050 ����              //��̬�����rol����Ϊ������
    pid_controller(&all.rol_angle);                 //�⻷pid������  ��Ҫ��������error last��ֵ���ˣ���������������->out��    
    all.pit_angle.feedback = pitrtos;//att.pit;   mpu6050            //��̬�����pit����Ϊ������
    pid_controller(&all.pit_angle);                 //�⻷pid������    

    if (yaw_init_cnt < 300)
        yaw_init_cnt++;
    else
    {
        if (0 == 0)                               //ƫ����λ����λʱ����˫������
        {
            if (all.yaw_angle.expect == 0)
            {
                all.yaw_angle.expect = expecyaw;//��Ԫ��ƫ����att.yaw; MPU6050    //���ʱ��ƫ�����֮�󣬵�ǰƫ������ΪĿ��Ƕ�       
            }
            all.yaw_angle.feedback = expecyaw;//att.yaw;MPU6050
            pid_controller(&all.yaw_angle);         //�⻷pid������

            all.yaw_gyro.expect = all.yaw_angle.out;
        }
        else                                        //ƫ�����д��ʱ��ң��ֱ����Ϊ�ٶȻ�����    
        {
            all.yaw_angle.expect = 0;
            all.yaw_gyro.expect = 5;//rc.yaw * 5; ����Ƕ�Ϊ��ǰ�������ٶ�����
        }
    }
}
//�ڻ����ٶȿ�����
void gyro_controller(int rtosdegx,int rtosdey,int rtosdegz)
{
    all.pit_gyro.expect = all.pit_angle.out;         //�⻷�����Ϊ�ڻ�����ֵ
    all.pit_gyro.feedback = rtosdegx;//Mpu.deg_s.x;             //���ٶ�ֵ��Ϊ������ 
    pid_controller(&all.pit_gyro);                   //�ڻ�pid������     

    all.rol_gyro.expect = all.rol_angle.out;         //�⻷�����Ϊ�ڻ�����ֵ
    all.rol_gyro.feedback = rtosdey;//Mpu.deg_s.y;             //���ٶ�ֵ��Ϊ������
    pid_controller(&all.rol_gyro);                   //�ڻ�pid������ 

    all.yaw_gyro.feedback = rtosdegz;//Mpu.deg_s.z;             //���ٶ�ֵ��Ϊ������   
    pid_controller(&all.yaw_gyro);                   //�ڻ�pid������    
}

#define thr_deadzone_min 350
#define thr_deadzone_max 650
#define vel_up     200
#define vel_down   200
#define throttle_max     1000
#define throttle_min     0

uint8_t high_mark_flag = 0;                     //���붨��ʱ���ż�¼��־λ
uint8_t fix_mark_flag = 0;




//����ģʽѡ��
int rtoshightvalue=0;
void _controller_detect(int mode_choice_high, int mode_choice_fix, int expecrol, int hightvalue, int expecpit, int expecthr,int expecfx,int expecfy)
{
    //����ģʽ
    if (mode_choice_high==0)//(rc.high_flag == 0)
    {
        _control.mode = 1;

        all.rol_angle.expect = expecrol;                  //ң����������Ϊrol��������
        all.pit_angle.expect = expecpit;                  //ң����������Ϊpit��������         
        throttle_type.final_out = expecthr;               //�����������������ң��������
    }
    //����ģʽ
    if (mode_choice_high == 1)
    {
        if (high_mark_flag == 0)                                                           //���߿�����������ʱ����һ�ε�ʱ�߶�ֵ                                   
        {
            throttle_type.mark_high = rtoshightvalue;//rc.thr;                                           //���浱ǰ�߶�����    
            all.pos_high.expect = hightvalue;//sins.pos.z;                                           //����һ�������߶�    
            high_mark_flag = 1;
        }
        if (high_mark_flag == 1)
        {
            _control.mode = 2;                                                          //����ģʽ   
            all.rol_angle.expect = expecrol; // rc.rol;                  //ң����������Ϊrol��������
            all.pit_angle.expect = expecpit;//rc.pit;                  //ң����������Ϊpit��������              
        }
    }
    //����ģʽ(�ڶ���ģʽ�����ϲ��ܿ�������)
    if (mode_choice_fix == 1 && mode_choice_high == 1)
    {
        if (fix_mark_flag == 0)
        {
            all.pos_fix_x.expect = expecfx;//flow.fix_pos.x;                                      //���浱ǰx�᷽��λ��
            all.pos_fix_y.expect = expecfy;//flow.fix_pos.y;                                      //���浱ǰy�᷽��λ��

            fix_mark_flag = 1;
        }
        if (fix_mark_flag == 1)
        {
            _control.mode = 3;
        }
    }
}
//������
void _controller_perform(void)
{
    switch (1)//_control.mode)
    {
    case 1:
        puts("����ģʽ");
        //����ģʽ    
      //  angle_controller();
       // gyro_controller();
        break;

    case 2:
        //����ģʽ
        puts("����ģʽ");
      //  high_controller();
        //angle_controller();
       // gyro_controller();
        break;

    case 3: //����ģʽ     
        //high_controller();
        puts("����ģʽ");
        //fix_controller();
       // angle_controller();
       // gyro_controller();
        break;

    default:
        break;
    }
}

//����ң����λʱ���ֵ�ǰ�߶ȣ����и߶��⻷����
void keep_alt(int expechigh,int rtoshigh)
{
    if (all.pos_high.expect == 0)
    {
        all.pos_high.expect = expechigh;// sins.pos.z;                                       //����һ�������߶�
    }
    controller_cnt.high++;
    if (controller_cnt.high >= 2)                                                  //�߶�10ms��������
    {
        controller_cnt.high = 0;
        all.pos_high.feedback = rtoshigh;//sins.pos.z;                                     //��ǰ�ߵ��߶���Ϊ������
        pid_controller(&all.pos_high);                                          //�߶ȿ���
        all.vel_high.expect = all.pos_high.out;                                //�߶��⻷�����Ϊ�ڻ��ٶ�����
    }
}

//�������ٶ����ϣ�����ң�г�����λ��ֻ���ٶȿ���

void rise_alt(int rcva)
{
    //�����ٶ�    ���2m/s
    all.vel_high.expect = vel_up * (/*rc.thr_zone*/rcva - thr_deadzone_max) / (throttle_max - thr_deadzone_max);
    all.pos_high.expect = 0;
}

//�������ٶ����£�����ң�е�����λ��ֻ���ٶȿ���
void drop_alt(int rcva)
{
    //�½��ٶ�        
    all.vel_high.expect = vel_down * (/*rc.thr_zone*/rcva - thr_deadzone_min) / (thr_deadzone_min - throttle_min);
    all.pos_high.expect = 0;
}

//�߶ȿ�����
void high_controller(int rcva,int mid,int expechigh ,int rtosz ,int rtoshigh)
{
    /** ��ֱ�߶ȿ��� **/


    //����������λ
    if(rcva== mid)// (rc.thr_zone >= thr_deadzone_min && rc.thr_zone <= thr_deadzone_max)
    {
        _control.high_mode = 2;
    }
    //���Ŵ�����λ
    else if (rcva < mid)//(rc.thr_zone > thr_deadzone_max)
    {
        _control.high_mode = 3;
    }
    //����С����λ
    else if (rcva >mid)//(rc.thr_zone < thr_deadzone_min)
    {
        _control.high_mode = 1;
    }

    switch (_control.high_mode)
    {
    case 1: //���¼���
        drop_alt(rcva);
        break;

    case 2: //���ֵ�ǰ�߶�
        keep_alt(expechigh, rtoshigh);
        break;

    case 3: //���ϼ���
        rise_alt(rcva);
        break;

    default:
        break;
    }
    
    /** ��ֱ�ٶȿ��� **/
    all.vel_high.feedback = rtosz;//sins.vel.z;                                             //�ߵ��ٶ���Ϊ�ٶȷ�����
    pid_controller(&all.vel_high);                                                  //�ٶ�5ms��������

    //����������� = ���붨��ʱ����(��ʱң��������) + �߶ȿ������ٶȻ����
    throttle_type.final_out = throttle_type.mark_high + all.vel_high.out;
}

uint8_t flow_fix_flag = 0;
//�������
void fix_controller(void)
{
    //�޷�����ƣ����淽�����λ
    if (0==0)//(rc.pit == 0 && rc.rol == 0)
    {
        //����˻���ʱ���浱ǰλ��Ϊ������λ
        if ((all.pos_fix_x.expect == 0 && all.pos_fix_y.expect == 0) || flow_fix_flag == 0)
        {
            flow_fix_flag = 1;
            all.pos_fix_x.expect = -4;//-flow.fix_pos.x;                  //���浱ǰx�᷽��λ��
            all.pos_fix_y.expect = -5;//-flow.fix_pos.y;                  //���浱ǰy�᷽��λ��			
        }
        //������ǰ��λ��Ϊ������λ
        all.pos_fix_x.feedback = -5;//-flow.fix_pos.x;
        all.pos_fix_y.feedback = -5;//-flow.fix_pos.y;
        //λ�ÿ�����
        pid_controller(&all.pos_fix_x);
        pid_controller(&all.pos_fix_y);

        //λ�������Ϊ�ٶ�����
        all.vel_fix_x.expect = all.pos_fix_x.out;
        all.vel_fix_y.expect = all.pos_fix_y.out;

        //�����ٶ���Ϊ�����ٶ�
        all.vel_fix_x.feedback = -1;//-flow.fix_vel.x;
        all.vel_fix_y.feedback = -1;//-flow.fix_vel.y;

        pid_controller(&all.vel_fix_x);
        pid_controller(&all.vel_fix_y);

        all.rol_angle.expect = all.vel_fix_x.out;
        all.pit_angle.expect = all.vel_fix_y.out;

        //		//λ�������Ϊ�ٶ�����
        //		all.rol_angle.expect = all.pos_fix_x.out;
        //		all.pit_angle.expect = all.pos_fix_y.out;        
    }
    //�з��������
    else
    {
        //�����ֱ�ӿ����ٶ�����
//		all.vel_fix_x.expect = 0;
//		all.vel_fix_y.expect = 0;		
        all.vel_fix_x.expect = 20;//rc.rol * 20;
        all.vel_fix_y.expect = 20;//rc.pit * 20;

        //�����ٶ���Ϊ�����ٶ�
        all.vel_fix_x.feedback = -1;//-flow.fix_vel.x;
        all.vel_fix_y.feedback = -1;//-flow.fix_vel.y;

        pid_controller(&all.vel_fix_x);
        pid_controller(&all.vel_fix_y);

        all.rol_angle.expect = all.vel_fix_x.out;
        all.pit_angle.expect = all.vel_fix_y.out;

        //�������ʱ��λ����������
        all.pos_fix_x.expect = 0;
        all.pos_fix_y.expect = 0;
    }
}

//������pwm���
void _controller_output(void)
{
    if (0==0)//fc.state == fc_unlock)                         //���������            
    {
        printf("unlock\n");

        if ( 30> throttle_flying)                  //�����������rc.thr
        {
            printf("fly\n");
            Motor1.out = throttle_type.final_out + all.pit_gyro.out - all.rol_gyro.out + all.yaw_gyro.out;
            Motor2.out = throttle_type.final_out + all.pit_gyro.out + all.rol_gyro.out - all.yaw_gyro.out;
            Motor3.out = throttle_type.final_out - all.pit_gyro.out + all.rol_gyro.out + all.yaw_gyro.out;
            Motor4.out = throttle_type.final_out - all.pit_gyro.out - all.rol_gyro.out - all.yaw_gyro.out;
            printf("Motor 1:%d,2:%d 3:%d 4:%d\n", Motor1.out, Motor2.out, Motor3.out, Motor4.out);
        }
        else                                        //С���������
        {
            Motor1.out = 0;//rc.thr;
            Motor2.out = 0;//rc.thr;
            Motor3.out = 0;//rc.thr;
            Motor4.out = 0;//rc.thr;

            clear_integral(&all.pit_angle);         //�������
            clear_integral(&all.pit_gyro);          //�������        
            clear_integral(&all.rol_angle);         //�������    
            clear_integral(&all.rol_gyro);          //�������
            clear_integral(&all.yaw_angle);         //�������
            clear_integral(&all.yaw_gyro);          //�������
        }
    }
    else                                        //δ����
    {
        Motor1.out = 0;
        Motor2.out = 0;
        Motor3.out = 0;
        Motor4.out = 0;

        clear_integral(&all.pit_angle);         //�������
        clear_integral(&all.pit_gyro);          //�������        
        clear_integral(&all.rol_angle);         //�������    
        clear_integral(&all.rol_gyro);          //�������
        clear_integral(&all.yaw_angle);         //�������
        clear_integral(&all.yaw_gyro);          //�������        
    }


}


