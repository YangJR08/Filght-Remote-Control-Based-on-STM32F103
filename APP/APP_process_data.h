#ifndef APP_PROCESS_DATA_H
#define APP_PROCESS_DATA_H

#include "int_joystick.h"
#include "int_key.h"
#include "Com_debug.h"

//遥控数据定义
typedef struct
{
    int16_t throttle;  //油门值，范围
    int16_t yaw;       //偏航值，范围
    int16_t pitch;     //俯仰值，范围
    int16_t roll;      //滚转值，范围
    int8_t altitude;  //定高值，范围
    int8_t shutdown;  //关机值，范围
} Remote_Data;


//按键微调结构体,只有前后左右四个按键的微调，摇杆按键不考虑微调
typedef struct
{
    int16_t pitch_adj;     //俯仰微调值，范围
    int16_t roll_adj;      //滚转微调值，范围
} Key_Adjustment;

void APP_process_calibrate_joystick(void); //校准摇杆函数
//处理按键数据，如果有按键按下，需要对应记录
void APP_process_key_data(void);
//处理摇杆数据，获取最新的摇杆状态，摇杆输出的值在一个稳定范围，修正极性相位标准值等，方便后续使用
void APP_process_joystick_data(void);

#endif // APP_PROCESS_DATA_H