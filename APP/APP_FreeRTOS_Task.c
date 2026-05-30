#include "APP_FreeRTOS_Task.h"
#include "APP_process_data.h"



#if FreeRTOStest
void task1(void *pvParameters);
void task2(void *pvParameters);
#endif /* FreeRTOStest */

//电源管理任务
void power_task(void *pvParameters); //创建电源管理任务
#define POWER_TASK_STACK_SIZE 128 //电源管理任务的栈空间大小，单位为字（4字节为1字）
#define POWER_TASK_PRIORITY 4   //电源管理任务的优先级，数值越大优先级越高，范围从0到configMAX_PRIORITIES-1
TaskHandle_t power_task_Handle = NULL;  //电源管理任务的句柄，可以用来操作任务，如删除、挂起等
#define POWER_TASK_DELAY_MS 10000 // 电源管理任务的延时周期，单位为毫秒

//通讯任务
void com_task(void *pvParameters); //创建通讯任务
#define COM_TASK_STACK_SIZE 128 //通讯任务的栈空间大小，单位为字（4字节为1字）
#define COM_TASK_PRIORITY 3   //通讯任务的优先级，数值越大
//优先级越高，范围从0到configMAX_PRIORITIES-1
TaskHandle_t com_task_Handle = NULL;  //通讯任务的句柄，可以用来操作任务，如删除、挂起等    
//任务延时周期，单位为毫秒
#define COM_TASK_DELAY_MS 6

//按键任务
void key_task(void *pvParameters); //创建按键任务
#define KEY_TASK_STACK_SIZE 128 //按键任务的栈空间大小，单位为字（4字节为1字）
#define KEY_TASK_PRIORITY 2   //按键任务的优先级，数值越大优先级越高，范围从0到configMAX_PRIORITIES-1
TaskHandle_t key_task_Handle = NULL;  //按键任务的句柄，可以用来操作任务，如删除、挂起等
#define KEY_TASK_DELAY_MS 20 // 按键任务的延时周期，单位为毫秒


//摇杆任务和按键任务优先级同级
void joystick_task(void *pvParameters); //创建摇杆任务
#define JOYSTICK_TASK_STACK_SIZE 128 //摇杆任务的栈空间大小，单位为字（4字节为1字）
#define JOYSTICK_TASK_PRIORITY 2   //摇杆任务的优先级，数值越大优先级越高，范围从0到configMAX_PRIORITIES-1
TaskHandle_t joystick_task_Handle = NULL;  //摇杆任务的句柄，可以用来操作任务，如删除、挂起等
#define JOYSTICK_TASK_DELAY_MS 20 // 摇杆任务的延时周期，单位为毫秒

//屏幕任务
void screen_task(void *pvParameters); //创建屏幕任务
#define SCREEN_TASK_STACK_SIZE 128 //屏幕任务的栈空间大小，单位为字（4字节为1字）
#define SCREEN_TASK_PRIORITY 1   //屏幕任务的优先级，数值越大优先级越高，范围从0到configMAX_PRIORITIES-1
TaskHandle_t screen_task_Handle = NULL;  //屏幕任务的句柄，可以用来操作任务，如删除、挂起等
#define SCREEN_TASK_DELAY_MS 100 // 屏幕任务的延时周期，单位为毫秒

//void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName);


#if FreeRTOStest
//移植freertos测试代码，创建两个任务，每个任务每秒打印一次自己的信息。
//任务栈空间大小，最小推荐128，等于128*4=512字节
#define TASK1_STACK_SIZE 128
//任务优先级，数值越大优先级越高，范围从0到configMAX_PRIORITIES-1
#define TASK1_PRIORITY 1
//任务句柄，可以用来操作任务，如删除、挂起等
TaskHandle_t task1Handle = NULL;

//任务栈空间大小，最小推荐128，等于128*4=512字节
#define TASK2_STACK_SIZE 128
//任务优先级，数值越大优先级越高，范围从0到configMAX_PRIORITIES-1
#define TASK2_PRIORITY 1
//任务句柄，可以用来操作任务，如删除、挂起等
TaskHandle_t task2Handle = NULL;
#endif

/*
启动FreeRTOS操作系统
*/
void APP_FreeRTOS_Task_Start(void)
{
    //创建任务
    //1、创建电源管理任务
    xTaskCreate(power_task, "power_task", POWER_TASK_STACK_SIZE, NULL, POWER_TASK_PRIORITY, &power_task_Handle);
    //2、创建通讯任务
    xTaskCreate(com_task, "com_task", COM_TASK_STACK_SIZE, NULL, COM_TASK_PRIORITY, &com_task_Handle);
    //3、创建按键任务
    xTaskCreate(key_task, "key_task", KEY_TASK_STACK_SIZE, NULL, KEY_TASK_PRIORITY, &key_task_Handle);
    //4、创建摇杆任务
    xTaskCreate(joystick_task, "joystick_task", JOYSTICK_TASK_STACK_SIZE, NULL, JOYSTICK_TASK_PRIORITY, &joystick_task_Handle);
    //5、创建屏幕任务
    xTaskCreate(screen_task, "screen_task", SCREEN_TASK_STACK_SIZE, NULL, SCREEN_TASK_PRIORITY, &screen_task_Handle);

    #if FreeRTOStest
    //移植freertos测试代码，创建两个任务，每个任务每秒打印一次自己的信息。
    xTaskCreate(task1, "Task1", TASK1_STACK_SIZE, NULL, TASK1_PRIORITY, &task1Handle);
    xTaskCreate(task2, "Task2", TASK2_STACK_SIZE, NULL, TASK2_PRIORITY, &task2Handle);
    #endif /* FreeRTOStest */
    //2、启动调度器
    vTaskStartScheduler();
}


void power_task(void *pvParameters)
{//创建电源管理任务
    //获取基准时间
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while(1)
    {
        //避免开机时真实按键和电源任务短时间内同时触发造成关机，先延时10S
        vTaskDelayUntil(&xLastWakeTime, POWER_TASK_DELAY_MS);
        //执行电源管理任务的功能
        IP5305T_Init();
    }
}

/*
通讯任务
*/


void com_task(void *pvParameters)
{
    //获取基准时间
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while(1)
    {
        //执行通讯任务的功能
        //调用发送函数，发送数据，数据来源于全局发送缓冲区
        APP_transmit_data();

        vTaskDelayUntil(&xLastWakeTime, COM_TASK_DELAY_MS);
    }
}


/*
按键任务
*/
void key_task(void *pvParameters)
{
    //获取基准时间
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while(1)
    {
        APP_process_key_data();
        //延时，避免频繁查询按键状态
        vTaskDelayUntil(&xLastWakeTime, KEY_TASK_DELAY_MS);
    }
}

void joystick_task(void *pvParameters)
{
    //获取基准时间
    TickType_t xLastWakeTime = xTaskGetTickCount();
    //初始化摇杆ADC
    Int_joystick_init();
    while(1)
    {
        APP_process_joystick_data();
        vTaskDelayUntil(&xLastWakeTime, JOYSTICK_TASK_DELAY_MS);
    }
}

void screen_task(void *pvParameters)
{
    APP_display_init(); // 初始化显示
    //获取基准时间
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while(1)
    {
        APP_display_show();
        //执行屏幕任务的功能
        vTaskDelayUntil(&xLastWakeTime, SCREEN_TASK_DELAY_MS);
    }
}

#if FreeRTOStest
//移植freertos测试代码，创建两个任务，每个任务每秒打印一次自己的信息。

void task1(void *pvParameters)
{
    while (1)
    {
        debug_printf("This is Task 1\r\n");
        vTaskDelay(pdMS_TO_TICKS(1000)); // 延时 1000ms
    }
}

void task2(void *pvParameters)
{
    while (1)
    {
        debug_printf("This is Task 2\r\n");
        vTaskDelay(pdMS_TO_TICKS(1000)); // 延时 1000ms
    }
}
#endif /* FreeRTOStest */



/* FreeRTOS 任务栈溢出钩子函数，当任务发生栈溢出时会调用此函数 
需要在 FreeRTOSConfig.h 中将 configCHECK_FOR_STACK_OVERFLOW 设置为 1 或 2 来启用栈溢出检查，并提供此钩子函数的实现。
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)xTask;
    (void)pcTaskName;

    taskDISABLE_INTERRUPTS();
    for (;;)
    {
    }
}
*/
