#include "APP_transmit.h"
#include "Com_debug.h"



//数据传输数组，大小为TX_PLOAD_WIDTH字节，初始值为0
uint8_t transmit_buffer[TX_PLOAD_WIDTH] = {0};

//宏定义三个字节帧头
#define FRAME_HEADER_1 'Y'
#define FRAME_HEADER_2 'J'
#define FRAME_HEADER_3 'R'

//发送数据，调用api即可，不用传参数
void APP_transmit_data(void)
{
    //切换到发送模式
    Int_SI24R1_TX_Mode();
    
    //发送数据，数据来源于全局发送缓冲区
    //在任务系统中的保证任务的数据的一致性和完整性，避免数据竞争和不一致
    //帧头和校验，校验使用校验和->数据帧的每个字节相加取模256，结果放在数据帧的最后一个字节
    //Remote_Data结构体本身有10个字节，帧头校验和帧尾校验都需要加字节
    //帧头设置3字节，校验和帧尾设置为4字节，所以总共需要17字节的发送缓冲区
    //目前发送字节一共17字节，可以后续确定之后去int_SI24R1中修改#define TX_PLOAD_WIDTH 32 // 数据通道有效数据宽度
    //接收端也得同步修改，先修改成20字节吧，留一些余量
    Remote_Data local_remote_data; // 定义一个局部变量来存储遥控数据，避免在计算校验和时访问全局变量造成数据竞争
    //校验和计算，定义
    uint32_t checksum = 0;
    //加入互斥锁保护，确保在访问共享数据时进行互斥保护，避免数据竞争和不一致
    APP_Data_Lock();
    //计算校验和，累加数据帧的每个字节，除了最后一个字节（校验和帧）
    memcpy(&local_remote_data, &remote_data, sizeof(Remote_Data)); // 将Remote_Data结构体的数据复制到发送缓冲区的前面部分
    remote_data.altitude = 0; // 定高按键是瞬时的，发送完数据后需要清零
    remote_data.shutdown = 0; // 关机按键是瞬时的，发送完数据后需要清零
    APP_Data_Unlock();
    transmit_buffer[0] = FRAME_HEADER_1;
    transmit_buffer[1] = FRAME_HEADER_2;
    transmit_buffer[2] = FRAME_HEADER_3;
    //结构体数据高八位在前,油门处理
    transmit_buffer[3] = (local_remote_data.throttle >>8) & 0xFF;
    transmit_buffer[4] = local_remote_data.throttle & 0xFF;
    //偏航处理
    transmit_buffer[5] = (local_remote_data.yaw >>8) & 0xFF;
    transmit_buffer[6] = local_remote_data.yaw & 0xFF;
    //俯仰处理
    transmit_buffer[7] = (local_remote_data.pitch >>8) & 0xFF;
    transmit_buffer[8] = local_remote_data.pitch & 0xFF;
    //滚转处理
    transmit_buffer[9] = (local_remote_data.roll >>8) & 0xFF;
    transmit_buffer[10] = local_remote_data.roll & 0xFF;
    //定高处理
    transmit_buffer[11] = local_remote_data.altitude;
    //关机处理
    transmit_buffer[12] = local_remote_data.shutdown;
    //计算校验和，累加数据帧的每个字节，除了最后一个字节（校验和帧）
    for (uint8_t i = 0; i < 13; i++)
    {
        checksum += transmit_buffer[i];
    }
    //高位在前处理校验位
    transmit_buffer[13] = (checksum >>24) & 0xFF;
    transmit_buffer[14] = (checksum >>16) & 0xFF;
    transmit_buffer[15] = (checksum >>8) & 0xFF;
    transmit_buffer[16] = checksum & 0xFF;

    Int_SI24R1_TxPacket(transmit_buffer);
    //打印数据给VOFA解析
    debug_printf(":%d,%d,%d,%d,%d,%d\n", local_remote_data.throttle, local_remote_data.yaw, local_remote_data.pitch, local_remote_data.roll, local_remote_data.altitude, local_remote_data.shutdown);

    //切换回接收模式
    Int_SI24R1_RX_Mode();
}
