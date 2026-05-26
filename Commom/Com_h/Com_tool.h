#ifndef __COM_TOOL_H
#define __COM_TOOL_H

#include "main.h"

//限幅函数，传输入值和限幅值，返回限幅后的值
int16_t Com_limit_value(int16_t value, int16_t min, int16_t max);


#endif // __COM_TOOL_H