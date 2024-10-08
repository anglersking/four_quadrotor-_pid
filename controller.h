#include <cstdint>
#ifndef _controller_h_
#define _controller_h_



typedef struct
{
    int out;

}_OUT_Motor;

typedef struct
{
    int mode : 4;
    int high_mode : 4;
}_CONTROLLER_MODE;

typedef struct
{
    int high;
    int longitude;
    int latitude;
}_CONTROLLER_CNT;

typedef struct
{
    int mark_high;
    int final_out;
}_THROTTLE_TYPE;


void angle_controller(int rolrtos, int pitrtos, int yawrtos, int expecyaw);
void gyro_controller(int rtosdegx, int rtosdey, int rtosdegz);
void _controller_output(void);
void high_controller(int rcva, int mid, int expechigh, int rtosz, int rtoshigh);
void fix_controller(void);
//void _controller_detect(void);
//void _controller_perform(void);

extern int high_mark_flag;
extern int fix_mark_flag;
extern int flow_fix_flag;

#endif

