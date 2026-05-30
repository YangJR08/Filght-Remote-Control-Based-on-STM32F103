#ifndef   __APP_DISPLAY_H
#define   __APP_DISPLAY_H

#include "main.h"
#include "Inf_OLED.h"
#include "Int_SI24R1.h"
#include "APP_process_data.h"

void APP_display_init(void); //显示初始化函数
void APP_display_show(void); //显示更新函数
#endif /* __APP_DISPLAY_H */