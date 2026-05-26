#ifndef INT_JOYSTICK_H
#define INT_JOYSTICK_H

#include "main.h"
#include "stm32f1xx_hal_adc.h"
#include "adc.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>


#define JOYSTICK_CHANNEL_NUM 4 // 定义摇杆通道数量，根据实际情况修改
//摇杆数据存储
typedef struct
{
    int16_t throttle;  //油门值，范围
    int16_t yaw;       //偏航值，范围
    int16_t pitch;     //俯仰值，范围
    int16_t roll;      //滚转值，范围
} Joystick_Struct;



//初始化ADC摇杆
void Int_joystick_init(void);

//获取摇杆数据保存到结构体之中
void Int_joystick_get(Joystick_Struct *joystick_data);



#endif // INT_JOYSTICK_H