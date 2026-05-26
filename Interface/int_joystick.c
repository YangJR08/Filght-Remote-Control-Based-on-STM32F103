#include "int_joystick.h"

static Joystick_Struct joystick_dma = {0}; // 直接作为DMA缓冲区，避免数组与结构体之间的拷贝

//初始化ADC摇杆
void Int_joystick_init(void)
{
    //直接使用HAL库开启ADC
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&joystick_dma, JOYSTICK_CHANNEL_NUM); // DMA直接写入结构体内的4个通道值
}

//获取摇杆数据保存到结构体之中
void Int_joystick_get(Joystick_Struct *out_joystick)
{
    //DMA不依赖CPU传输，实时更新joystick_dma结构体中的值
    out_joystick->throttle = joystick_dma.throttle;
    out_joystick->yaw = joystick_dma.yaw;
    out_joystick->pitch = joystick_dma.pitch;
    out_joystick->roll = joystick_dma.roll;
}