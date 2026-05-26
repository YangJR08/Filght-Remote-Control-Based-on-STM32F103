#ifndef INT_KEY_H
#define INT_KEY_H

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

//用枚举类型来判断按键
typedef enum
{
    KEY_NONE = 0,   // 无按键按下
    KEY_UP, // 上键按下
    KEY_DOWN,   // 下键按下
    KEY_LEFT,   // 左键按下
    KEY_RIGHT,  // 右键按下
    KEY_LEFT_X,  // 左上键短按
    KEY_RIGHT_X, // 右上键短按
    KEY_LEFT_X_LONG,    // 左上键长按
    KEY_RIGHT_X_LONG,   // 右上键长按
} Key_type;

/*
 * Function: Int_key_get
 * Description: Get the status of the keyboard
 * Parameters: None
 * Return: Key_type - KEY_NONE:没有按键按下，其他都是对应标记
 */
Key_type Int_key_get(void); //获取按键状态的函数声明

#endif // INT_KEY_H

