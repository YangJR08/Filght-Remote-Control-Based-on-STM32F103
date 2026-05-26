#include "APP_process_data.h"
#include "int_joystick.h"



Joystick_Struct joystick_data = {0}; // 定义一个全局变量来存储摇杆数据，方便在任务中使用
Remote_Data remote_data = {0}; // 定义一个全局变量来存储遥控数据，方便在任务中使用

Key_Adjustment key_adjustment = {0}; // 定义一个全局变量来存储按键微调数据，方便在任务中使用

Joystick_Struct zero_offset = {0}; // 定义一个结构体来零偏值
//校准摇杆函数
void APP_process_calibrate_joystick(void)
{
    //清空按键微调值，避免校准时按键微调对零偏值的计算造成干扰
    key_adjustment.pitch_adj = 0;
    key_adjustment.roll_adj = 0;
    //逻辑减去零偏的值测量多次求平均值
    const uint8_t calibrate_samples = 10; // 定义一个常量来指定校准时采样的次数
    for(uint8_t i = 0; i < calibrate_samples; i++)
    {
        Joystick_Struct sample;
        Int_joystick_get(&sample);
        zero_offset.throttle += sample.throttle-THROTTLE_ZERO_OFFSET;   //油门的零点在0，根据实际情况调整
        zero_offset.yaw += sample.yaw-YAW_ZERO_OFFSET; // 偏航的零点在500，根据实际情况调整
        zero_offset.pitch += sample.pitch-PITCH_ZERO_OFFSET; // 俯仰的零点在500，根据实际情况调整
        zero_offset.roll += sample.roll-ROLL_ZERO_OFFSET;
        vTaskDelay(10); // 每次采样之间延时10ms，避免过快采样导致数据不稳定
    }
    //计算平均零偏值
    zero_offset.throttle = zero_offset.throttle / calibrate_samples;
    zero_offset.yaw = zero_offset.yaw / calibrate_samples;
    zero_offset.pitch = zero_offset.pitch / calibrate_samples;
    zero_offset.roll = zero_offset.roll / calibrate_samples;
}

//处理按键数据，如果有按键按下，需要对应记录,FreeRTOS任务中调用这个函数获取按键状态并处理
void APP_process_key_data(void)
{
    //调用按键接口获取按键状态
    Key_type key_status = Int_key_get();
    //根据按键状态执行相应操作
    switch(key_status)
    {
        case KEY_UP:
            //微调向前飞，俯仰角增加
            key_adjustment.pitch_adj += 10;
            break;
        case KEY_DOWN:
            //微调向后飞，俯仰角减小
            key_adjustment.pitch_adj -= 10;
            break;
        case KEY_LEFT:
            //微调向左转，滚转角减小
            key_adjustment.roll_adj -= 10;
            break;
        case KEY_RIGHT:
            //微调向右转，滚转角增大
            key_adjustment.roll_adj += 10;
            break;
        case KEY_LEFT_X:
            //左上键短按关机
            remote_data.shutdown = 1;
            break;
        case KEY_RIGHT_X:
            //右上键短按，定高
            remote_data.altitude = 1;
            break;
        case KEY_LEFT_X_LONG:
            //校准摇杆
            APP_process_calibrate_joystick();
            break;
        default:
            //无按键按下
            break;
    }
}

//处理摇杆数据，获取最新的摇杆状态，摇杆输出的值在一个稳定范围，修正极性相位标准值等，方便后续使用
void APP_process_joystick_data(void)
{
    //调用摇杆接口获取摇杆状态，FreeRTOS任务中调用这个函数获取摇杆状态并处理
    Int_joystick_get(&joystick_data);
    //处理范围和极性，假设摇杆输出的原始值范围是0-4095，我们需要将其转换为0到1000的范围，并修正极性
    joystick_data.throttle =1000- (joystick_data.throttle * 1000) / 4095; // 油门值转换为0-1000
    joystick_data.yaw = 1000-(joystick_data.yaw*1000)/ 4095;
    joystick_data.pitch = 1000-(joystick_data.pitch * 1000) / 4095; // 俯仰值转换为-1000到1000，修正极性
    joystick_data.roll = 1000-(joystick_data.roll * 1000) / 4095; // 滚转值转换为-1000到1000，修正极性、

    //处理零偏校准
    joystick_data.throttle -= zero_offset.throttle; // 油门值减去零偏值
    joystick_data.yaw -= zero_offset.yaw; // 偏航值减去零偏值
    joystick_data.pitch -= zero_offset.pitch; // 俯仰值减去零偏值
    joystick_data.roll -= zero_offset.roll; // 滚转值减去零偏值   

    //使用按键微调，FREERTOS任务中调用这个函数获取摇杆状态并处理
    joystick_data.pitch += key_adjustment.pitch_adj; // 俯仰值加上按键微调值
    joystick_data.roll += key_adjustment.roll_adj; // 滚转值加上按键微调值

    //范围限制
    joystick_data.throttle = Com_limit_value(joystick_data.throttle, 0, 1000);
    joystick_data.yaw = Com_limit_value(joystick_data.yaw, 0, 1000);
    joystick_data.pitch = Com_limit_value(joystick_data.pitch, 0, 1000);
    joystick_data.roll = Com_limit_value(joystick_data.roll, 0, 1000);

    //用VOFA检测摇杆数据，debug_printf输出格式要符合VOFA要求，方便VOFA解析显示
    debug_printf(":%d,%d,%d,%d\n", joystick_data.throttle, joystick_data.yaw, joystick_data.pitch, joystick_data.roll);
    //根据摇杆状态执行相应操作
    //延时，避免频繁查询摇杆状态
}