#include "int_key.h"

/*
 * Function: Int_key_get
 * Description: Get the status of the keyboard
 * Parameters: None
 * Return: Key_type - KEY_NONE:没有按键按下，其他都是对应标记
 */
Key_type Int_key_get(void) //获取按键状态的函数声明
{
    //基础四个按键操作
    if (HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin) == GPIO_PIN_RESET)
    {   //上键按下
        //1、电弧抖动处理，等待10ms后再次确认按键状态
        vTaskDelay(10);
        if (HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin) == GPIO_PIN_RESET)
        {
            //2、如果按下时间比较长，为了不被多次判断，等待按键释放后再返回按键状态
            while (HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin) == GPIO_PIN_RESET)
            {
                vTaskDelay(10);
            }
            return KEY_UP;
        }
    }
    else if (HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin) == GPIO_PIN_RESET)
    {   //下键按下
        //1、电弧抖动处理，等待10ms后再次确认按键状态
        vTaskDelay(10);
        if (HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin) == GPIO_PIN_RESET)
        {
            //2、如果按下时间比较长，为了不被多次判断，等待按键释放后再返回按键状态
            while (HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin) == GPIO_PIN_RESET)
            {
                vTaskDelay(10);
            }
            return KEY_DOWN;
        }
    }
    else if (HAL_GPIO_ReadPin(KEY_LEFT_GPIO_Port, KEY_LEFT_Pin) == GPIO_PIN_RESET)
    {   //左键按下
        //1、电弧抖动处理，等待10ms后再次确认按键状态
        vTaskDelay(10);
        if (HAL_GPIO_ReadPin(KEY_LEFT_GPIO_Port, KEY_LEFT_Pin) == GPIO_PIN_RESET)
        {
            //2、如果按下时间比较长，为了不被多次判断，等待按键释放后再返回按键状态
            while (HAL_GPIO_ReadPin(KEY_LEFT_GPIO_Port, KEY_LEFT_Pin) == GPIO_PIN_RESET)
            {
                vTaskDelay(10);
            }
            return KEY_LEFT;
        }
    }
    else if (HAL_GPIO_ReadPin(KEY_RIGHT_GPIO_Port, KEY_RIGHT_Pin) == GPIO_PIN_RESET)
    {   //右键按下
        //1、电弧抖动处理，等待10ms后再次确认按键状态
        vTaskDelay(10);
        if (HAL_GPIO_ReadPin(KEY_RIGHT_GPIO_Port, KEY_RIGHT_Pin) == GPIO_PIN_RESET)
        {
            //2、如果按下时间比较长，为了不被多次判断，等待按键释放后再返回按键状态
            while (HAL_GPIO_ReadPin(KEY_RIGHT_GPIO_Port, KEY_RIGHT_Pin) == GPIO_PIN_RESET)
            {
                vTaskDelay(10);
            }
            return KEY_RIGHT;
        }
    }
    else if (HAL_GPIO_ReadPin(KEY_LEFT_X_GPIO_Port, KEY_LEFT_X_Pin) == GPIO_PIN_RESET)
    {   //左上键按下
        //1、电弧抖动处理，等待10ms后再次确认按键状态
        vTaskDelay(10);
        if (HAL_GPIO_ReadPin(KEY_LEFT_X_GPIO_Port, KEY_LEFT_X_Pin) == GPIO_PIN_RESET)
        {
            //2、如果按下时间比较长，为了不被多次判断，等待按键释放后再返回按键状态
            while (HAL_GPIO_ReadPin(KEY_LEFT_X_GPIO_Port, KEY_LEFT_X_Pin) == GPIO_PIN_RESET)
            {
                vTaskDelay(10);
            }
            return KEY_LEFT_X;
        }
    }
    else if (HAL_GPIO_ReadPin(KEY_RIGHT_X_GPIO_Port, KEY_RIGHT_X_Pin) == GPIO_PIN_RESET)
    {   //右上键按下!!!需要判断长按和短按
        //1、电弧抖动处理，等待10ms后再次确认按键状态
        vTaskDelay(10);
        if (HAL_GPIO_ReadPin(KEY_RIGHT_X_GPIO_Port, KEY_RIGHT_X_Pin) == GPIO_PIN_RESET)
        {
            //2、开始计时，判断长按和短按
            TickType_t start_tick = xTaskGetTickCount();
            //3、等待按键释放
            while (HAL_GPIO_ReadPin(KEY_RIGHT_X_GPIO_Port, KEY_RIGHT_X_Pin) == GPIO_PIN_RESET)
            {
                vTaskDelay(10);
            }
            //4、计算按键按下的持续时间
            TickType_t duration = xTaskGetTickCount() - start_tick;
            //5、根据持续时间判断是长按还是短按，假设长按的阈值为1000ms
            if (duration >= pdMS_TO_TICKS(1000))    // 长按，pdMS_TO_TICKS(毫秒数) 会自动帮你计算：需要多少个Tick才能等于你输入的毫秒数。
            {
                return KEY_RIGHT_X_LONG;
            }
            else    //短按
            {
                return KEY_RIGHT_X;
            }
        }
    }


    //没有按键按下
    return KEY_NONE; // 没有按键按下
}