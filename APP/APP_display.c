#include "APP_display.h"
#include "APP_process_data.h"
#include "Inf_OLED.h"
#include "int_SI24R1.h"
#include <stdint.h>

#define LIEN1_BEGIN 28
#define LIEN2_BEGIN 5
#define LIEN3_BEGIN 5
#define BAR1_BEGIN 35
#define BAR2_BEGIN 47 //35+12
#define LIEN2_BEGIN2 65
#define LIEN4_BEGIN2 65
#define BAR1_BEGIN2 95
#define BAR2_BEGIN2 107
#define Y0 0
#define Y1 14
#define Y2 26
#define Y3 38


void APP_display_init(void) //显示初始化函数
{
    OLED_Init();
}

//进度条展示
void APP_display_show_bar(uint8_t x,uint8_t y,uint8_t count)
{
    if(count < 13)
    {
        for (uint8_t i = 0; i < count; i++)
        {
            OLED_Show_CH(x, y, 12+count,12,1);
        }
    }
    
}

void APP_display_show(void) //显示更新函数
{
    //X,Y坐标，字符串，字体大小，显示模式
    //OLED_ShowString(0, 0, "Hello, World!", 16, 0);
    //展示2.4g通讯信道
    uint8_t buffer[4]= {0};
    sprintf((char *)buffer, "%03d", CHANNEL);
    OLED_ShowString(LIEN1_BEGIN, Y0, buffer, 12, 1);
    uint8_t count =0;
    //第二行展示遥控数据 油门和横滚角
    OLED_ShowString(LIEN2_BEGIN, Y1, (uint8_t *)"HTR:", 12, 1);
    if (remote_data.throttle > 500) // 油门大于500才显示，避免定高按键和关机按键的干扰
    {
       count = (remote_data.throttle-500) / 41; // 油门值除以100，显示在0-10之间，方便观察油门档位
       APP_display_show_bar(BAR1_BEGIN, Y1, 12);
       APP_display_show_bar(BAR1_BEGIN, Y1, count);
    }
    else
    {
        count = (remote_data.throttle) / 41; // 油门值除以100，显示在0-10之间，方便观察油门档位
        APP_display_show_bar(BAR1_BEGIN, Y1, count);
        APP_display_show_bar(BAR1_BEGIN, Y1, 0);
    }
    OLED_ShowString(LIEN2_BEGIN2, Y1, (uint8_t *)"ROL:", 12, 1);
    if (remote_data.roll > 500) // 横滚角大于500才显示，避免定高按键和关机按键的干扰
    {
       count = (remote_data.roll-500) / 41; // 横滚角值除以100，显示在0-10之间，方便观察横滚档位
       APP_display_show_bar(BAR1_BEGIN2, Y1, 12);
       APP_display_show_bar(BAR2_BEGIN2, Y1, count);
    }
    else
    {
        count = (remote_data.roll) / 41; // 横滚角值除以100，显示在0-10之间，方便观察横滚档位
        APP_display_show_bar(BAR1_BEGIN2, Y1, count);
        APP_display_show_bar(BAR2_BEGIN2, Y1, 0);
    }
        //第三行展示遥控数据 偏航角和俯仰角
    OLED_ShowString(LIEN3_BEGIN, Y2, (uint8_t *)"YAW:", 12, 1);
    if (remote_data.yaw > 500) // 偏航角大于500才显示，避免定高按键和关机按键的干扰
    {
       count = (remote_data.yaw-500) / 41; // 偏航角值除以100，显示在0-10之间，方便观察偏航档位
       APP_display_show_bar(BAR1_BEGIN, Y2, 12);
       APP_display_show_bar(BAR1_BEGIN, Y2, count);
    }
    else
    {
        count = (remote_data.yaw) / 41; // 偏航角值除以100，显示在0-10之间，方便观察偏航档位
        APP_display_show_bar(BAR1_BEGIN, Y2, count);
        APP_display_show_bar(BAR1_BEGIN, Y2, 0);
    }
    //第三行展示遥控数据 俯仰角和偏航角
    OLED_ShowString(LIEN4_BEGIN2, Y2, (uint8_t *)"PIT:", 12, 1);
    if (remote_data.pitch > 500) // 俯仰角大于500才显示，避免定高按键和关机按键的干扰
    {
       count = (remote_data.pitch-500) / 41; // 俯仰角值除以100，显示在0-10之间，方便观察俯仰档位
       APP_display_show_bar(BAR1_BEGIN2, Y2, 12);
       APP_display_show_bar(BAR2_BEGIN2, Y2, count);
    }
    else
    {
        count = (remote_data.pitch) / 41; // 俯仰角值除以100，显示在0-10之间，方便观察俯仰档位
        APP_display_show_bar(BAR1_BEGIN2, Y2, count);
        APP_display_show_bar(BAR2_BEGIN2, Y2, 0);
    }

    OLED_Refresh_Gram();
}