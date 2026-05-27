#include "APP_process_data.h"
#include "int_joystick.h"
#include "semphr.h"



Joystick_Struct joystick_data = {0}; // 定义一个全局变量来存储摇杆数据，方便在任务中使用
Remote_Data remote_data = {0}; // 定义一个全局变量来存储遥控数据，方便在任务中使用

Key_Adjustment key_adjustment = {0}; // 定义一个全局变量来存储按键微调数据，方便在任务中使用

Joystick_Struct zero_offset = {0}; // 定义一个结构体来零偏值
//创建一个互斥锁句柄
static SemaphoreHandle_t app_data_mutex = NULL;
//创建互斥锁初始化函数，确保在使用互斥锁之前已经创建好互斥锁
static void APP_Data_MutexInit(void)
{
    if (app_data_mutex == NULL)
    {
        taskENTER_CRITICAL();
        if (app_data_mutex == NULL)
        {
            app_data_mutex = xSemaphoreCreateMutex();
        }
        taskEXIT_CRITICAL();
    }
}
//数据访问锁定和解锁函数，确保在访问共享数据时进行互斥保护，避免数据竞争和不一致
void APP_Data_Lock(void)
{
    APP_Data_MutexInit();
    if (app_data_mutex != NULL)
    {
        (void)xSemaphoreTake(app_data_mutex, portMAX_DELAY);
    }
}
//数据访问解锁函数，释放互斥锁，允许其他任务访问共享数据
void APP_Data_Unlock(void)
{
    if (app_data_mutex != NULL)
    {
        (void)xSemaphoreGive(app_data_mutex);
    }
}
//校准摇杆函数
void APP_process_calibrate_joystick(void)
{
    //清空按键微调值，避免校准时按键微调对零偏值的计算造成干扰
    APP_Data_Lock();
    key_adjustment.pitch_adj = 0;
    key_adjustment.roll_adj = 0;
    APP_Data_Unlock();
    //逻辑减去零偏的值测量多次求平均值
    const uint8_t calibrate_samples = 10; // 定义一个常量来指定校准时采样的次数
    int32_t sum_throttle = 0;
    int32_t sum_yaw = 0;
    int32_t sum_pitch = 0;
    int32_t sum_roll = 0;
    for(uint8_t i = 0; i < calibrate_samples; i++)
    {
        Joystick_Struct sample;
        Int_joystick_get(&sample);
        sum_throttle += sample.throttle-THROTTLE_ZERO_OFFSET;   //油门的零点在0，根据实际情况调整
        sum_yaw += sample.yaw-YAW_ZERO_OFFSET; // 偏航的零点在500，根据实际情况调整
        sum_pitch += sample.pitch-PITCH_ZERO_OFFSET; // 俯仰的零点在500，根据实际情况调整
        sum_roll += sample.roll-ROLL_ZERO_OFFSET;
        vTaskDelay(10); // 每次采样之间延时10ms，避免过快采样导致数据不稳定
    }
    //计算平均零偏值
    APP_Data_Lock();
    zero_offset.throttle = (int16_t)(sum_throttle / calibrate_samples);
    zero_offset.yaw = (int16_t)(sum_yaw / calibrate_samples);
    zero_offset.pitch = (int16_t)(sum_pitch / calibrate_samples);
    zero_offset.roll = (int16_t)(sum_roll / calibrate_samples);
    APP_Data_Unlock();
}

//处理按键数据，如果有按键按下，需要对应记录,FreeRTOS任务中调用这个函数获取按键状态并处理
void APP_process_key_data(void)
{
    //调用按键接口获取按键状态
    Key_type key_status = Int_key_get();
    uint8_t need_calibrate = 0;
    //根据按键状态执行相应操作
    APP_Data_Lock();
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
            need_calibrate = 1;
            break;
        default:
            //无按键按下
            break;
    }
    APP_Data_Unlock();

    if (need_calibrate != 0)
    {
        APP_process_calibrate_joystick();
    }
}

//处理摇杆数据，获取最新的摇杆状态，摇杆输出的值在一个稳定范围，修正极性相位标准值等，方便后续使用
void APP_process_joystick_data(void)
{
    //调用摇杆接口获取摇杆状态，FreeRTOS任务中调用这个函数获取摇杆状态并处理
    Joystick_Struct local_joystick;
    Joystick_Struct local_zero;
    Key_Adjustment local_adj;

    Int_joystick_get(&local_joystick);
    APP_Data_Lock();
    local_zero = zero_offset;
    local_adj = key_adjustment;
    APP_Data_Unlock();
    //处理范围和极性，假设摇杆输出的原始值范围是0-4095，我们需要将其转换为0到1000的范围，并修正极性
    local_joystick.throttle = 1000 - (local_joystick.throttle * 1000) / 4095; // 油门值转换为0-1000
    local_joystick.yaw = 1000 - (local_joystick.yaw * 1000) / 4095;
    local_joystick.pitch = 1000 - (local_joystick.pitch * 1000) / 4095; // 俯仰值转换为-1000到1000，修正极性
    local_joystick.roll = 1000 - (local_joystick.roll * 1000) / 4095; // 滚转值转换为-1000到1000，修正极性、

    //处理零偏校准
    local_joystick.throttle -= local_zero.throttle; // 油门值减去零偏值
    local_joystick.yaw -= local_zero.yaw; // 偏航值减去零偏值
    local_joystick.pitch -= local_zero.pitch; // 俯仰值减去零偏值
    local_joystick.roll -= local_zero.roll; // 滚转值减去零偏值   

    //使用按键微调，FREERTOS任务中调用这个函数获取摇杆状态并处理
    local_joystick.pitch += local_adj.pitch_adj; // 俯仰值加上按键微调值
    local_joystick.roll += local_adj.roll_adj; // 滚转值加上按键微调值

    //范围限制
    local_joystick.throttle = Com_limit_value(local_joystick.throttle, 0, 1000);
    local_joystick.yaw = Com_limit_value(local_joystick.yaw, 0, 1000);
    local_joystick.pitch = Com_limit_value(local_joystick.pitch, 0, 1000);
    local_joystick.roll = Com_limit_value(local_joystick.roll, 0, 1000);

    APP_Data_Lock();
    joystick_data = local_joystick;
    APP_Data_Unlock();
    //将摇杆数据赋值到remote_data结构体中，方便后续发送
    APP_Data_Lock();
    remote_data.throttle = joystick_data.throttle; 
    remote_data.yaw = joystick_data.yaw;
    remote_data.pitch = joystick_data.pitch;
    remote_data.roll = joystick_data.roll;
    APP_Data_Unlock();
    //用VOFA检测摇杆数据，debug_printf输出格式要符合VOFA要求，方便VOFA解析显示
    //debug_printf(":%d,%d,%d,%d\n", local_joystick.throttle, local_joystick.yaw, local_joystick.pitch, local_joystick.roll);
    //根据摇杆状态执行相应操作
    //延时，避免频繁查询摇杆状态
}