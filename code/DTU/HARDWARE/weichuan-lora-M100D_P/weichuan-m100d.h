#ifndef _WEICHUAN_M100D_H_
#define _WEICHUAN_M100D_H_


/*

测试用命令:

FROM 模组 ->  事件和响应
FE 03 01 01 41 90 EF    （重启完成）       STARTID/事件/长度/命令字/数据/校验/ENDID
FE 03 02 02 00 A0 C0 EF （入网中）
FE 03 02 02 01 61 00 EF  （入网完成）

FE 02 01 00 D1 90 EF  (发送成功)
FE 02 01 01 10 50 EF （发送完成）
FE 02 01 02 50 51 EF
FE 02 01 06 51 92 EF (帧错误)

TO 模组 ->    命令和响应
FE 02 01 00 D1 90 EF                      STARTID/响应/长度/状态/参数/CRC/ENDID

FE 01 06 00 00 DE 01 02 03 CD 4E EF  （上传） STARTID/指令/长度/命令字/重传次数/端口号/数据/CRC/ENDIF
FE 03 05 00 08 31 32 33 2E 8F EF      (下行)

FE 01 01 73 60 75 EF    (状态确认)
FE 02 02 00 05 60 5F EF (状态返回)    STARTID/相应/长度/状态/结果（状态出错则结果为空）/校验
FE 02 02 00 02 21 9D EF

FE 01 10 00 00 03 FF 92 FF 92 FF 92 FF FF 92 FF 92 FF 92 EA FE EF //全部传感器丢失状态
FE 01 10 00 00 03 0F 01 2F 03 01 00 01 03 01 00 10 00 1E 0D A7 EF //模拟数据
FE 01 0B 00 00 03 0F 01 2F 03 01 00 01 1E 63 3A EF//模拟数据

FE 01 02 99 02 1A 49 EF class C 
*/

#include "stm32f10x.h"

typedef struct lora_send_pck{
   uint8_t type;
	 uint16_t Lenth;
	 uint8_t data[100];  // 数据位最大负荷255+6
}lora_send_pck;



enum{
	 MSGTYPE_ORDER = 0x01,
	 MSGTYPE_ACK   = 0x02,
   MSGTYPE_EVENT = 0X03, 
};

enum {
	StartID = 0xfe,
  EndID =  0xef,
};

//data type
enum{
	LORA_DATA_ERROR        = 0,
	LORA_MOD_RST_OK        = 1,
	LORA_MOD_STATE         = 2,  //这个是开机时事件发送的状态
	LORA_MOD_DATA          = 3,
	LORA_MOD_RESPONSE      = 4,
	LORA_MOD_RESPONSE_STATE= 5, //这个是查询命令后返回的状态
};

//event type
enum{
	LORA_JOIN_OK               =0X1, 
	LORA_JOIN_FAIL             =0X2, 
	LORA_JOIN_ING              =0X0, 
};

//response state type
enum{
	SEND_OK               =0X0,  
	SEND_FINISH           =0X1,  
	MSG_ERR_NOT_JOIN      =0X2,  
	SEND_FAIL_PAYLOAD_OVER=0X3,  
	SEND_FAIL_TX_TIMEOUT  =0X4,  
	MSG_SEND_FAIL_RX_ACK  =0X5,  
	ERR_FRM               =0X6,  
	ERR_CMD               =0X7,  
	ERR_PARA              =0X8,  
	ERR_BUSY              =0X9,  
	JOIN_OK               =0X21, 
	JOIN_FAIL             =0X22, 
	JOIN_ING              =0X20, 
};

//查询状态 后 返回的数据的含义
//enum{
//  
//};
//1 ：入网中
//2 ：已入网
//3 ：入网失败
//4 ：发送数据中
//5 ：发送完成
//6 ：发送失败，Payload 数据过多
//7 ：发送失败，发送超时
//8 ：发送失败，未收到确认

//数据上传
lora_send_pck  M100dFrameSend(uint8_t *PayLoad, uint8_t Length,uint8_t retransmission,uint8_t port);

//接收应答
lora_send_pck  M100dFrameACK(uint8_t State);

//旧版接收
uint8_t M100FrameRecive(lora_send_pck Pck);

//新版接收
lora_send_pck M100FrameReciveData(uint8_t *data,uint16_t len);

//主控发送命令等待接收  没写
lora_send_pck M100FrameWaitReciveOrderAck(lora_send_pck Pck,double Timeout);

lora_send_pck lora_send_request_state(void);

#endif
