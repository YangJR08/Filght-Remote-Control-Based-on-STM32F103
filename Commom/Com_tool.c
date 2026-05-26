#include "Com_tool.h"


//限幅函数，传输入值和限幅值，返回限幅后的值
int16_t Com_limit_value(int16_t value, int16_t min, int16_t max)
{
    if (value < min)
    {
        return min;
    }
    else if (value > max)
    {
        return max;
    }
    else
    {
        return value;
    }

}