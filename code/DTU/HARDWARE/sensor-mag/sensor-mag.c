#include "sensor-mag.h"
#include "string.h"
#include "stdio.h"
#include "usart.h"
#include "delay.h"
#include "wdg.h"

#define USE_SENSOR_DEBUG 1

#if USE_SENSOR_DEBUG
#define SENSOR_PRINTF(format, ...) printf (format, ##__VA_ARGS__)
#else
#define SENSOR_PRINTF(info)
#endif

Sensor_mag sensor_mag; //保存传感器信息的表单
Sensor_data sensor_data;//保存上传数据的表单
Sensor_cmd sensor_cmd;//保存单个发送给传感器的命令结构体

void sensor_mag_init(int baud)
{
   sensor_mag.number = 0;
	 sensor_data.number = 0;
	 usart_init(baud);
	 timer3_init();
	 set_usart1_irq_callback(sensor_mag_callback);//挂在串口回调
}

void sensor_mag_set_baud(int baud)
{
   usart_init(baud);
}
/*
void sensor_com_callback()
{
  //接收到传感器数据后 封装成帧，发送给通讯模组
	lora_send_pck SendModPck;
	uint8_t data[255];
	uint8_t lenth= GetSerialReciveDataSize(1);
	if(lenth >=255)
	{
		ClearSerialBuffer(1);
		return;
	}
	GetSerialReciveData(1,(char *)data,lenth);
	SendModPck=M100dFrameSend(data,lenth,0,128);
	usart_send_buffer(2,SendModPck.data,SendModPck.Lenth);
	PRINTF("recive sensor data and send to weichuan \n"); 
}
*/

static uint8_t data_flag=0;//串口数据接收标准
static uint8_t data[200];//串口缓存
static uint8_t lenth=0;
//串口接收程序
void sensor_mag_callback()
{
   if(data_flag)
	 {
		  lenth= GetSerialReciveDataSize(1);
	    if(lenth >=200)
	    {
		    ClearSerialBuffer(1);
		    return;
    	}
			GetSerialReciveData(1,(char *)data,lenth);
	    data_flag =0;
	 }
	 else//如果在非接收状态 则情况缓存
	 {
	   ClearSerialBuffer(1);
	 }
}

void sensor_update()
{
	 static uint8_t pck_id =0 ;//每给一组弄一样的ID号
	 static Sensor_rcv rcv_data;
	 double timeout=0;
	 uint8_t i;
	if(sensor_mag.number == 0) //没有传感器不进行更新，直接退出
		 return;
	 sensor_data.number=0;
   for(i=0;i<sensor_mag.number;i++)
	 {
		  ClearSerialBuffer(1);
		  data_flag =1;//开启串口接收
	    usart_send_buffer(1,sensor_mag.buff[i].pro,sensor_mag.buff[i].pro_lenth);
	    timeout = (int)GetNowTime();
		  while(1)
			{
				IWDG_Feed(); 
			  if(ClockTool(&timeout,sensor_mag.buff[i].use_time))
				{
					 //Sensor_rcv rcv_data={pck_id,sensor_mag.buff[i].key,2,{0xff,0x92}};
           rcv_data.pck_id = 	pck_id;
					 rcv_data.key = sensor_mag.buff[i].key;
					 rcv_data.lenth = 2;
					 rcv_data.data[0] = 0xff;
					 rcv_data.data[1] = 0x92;
					 sensor_rcv_save(rcv_data);
				   break;
				}
				if(data_flag ==0)//表示串口接收到数据了
				{
					 //Sensor_rcv rcv_data={pck_id,sensor_mag.buff[i].key,lenth,{0xff,0x92}};
					 rcv_data.pck_id = 	pck_id;
					 rcv_data.key = sensor_mag.buff[i].key;
					 rcv_data.lenth = lenth;
					 memcpy(rcv_data.data,data,lenth);
					 sensor_rcv_save(rcv_data);
					break;
				}
				//最后一次执行完成后，data_flag=0,表示串口不接收数据
			}
	 }
	 pck_id++;
}

void sensor_config(uint8_t *data)
{
	 u8 i=0;
   Sensor sensor;
	 sensor.key = data[0] *256 +data[1];
	 
	 sensor.use_time = data[2];
	 sensor.pro_lenth = data[3];
	 memcpy(sensor.pro,&data[4],data[3]);
	 //防止接收到重复的key
	 for(i=0;i<sensor_mag.number;i++)
	 {
	    if(sensor_mag.buff[i].key == sensor.key)
			{
				 SENSOR_PRINTF("sensor pck %d repetition\n",sensor.key);
			   return;
			}
	 }
	 sensor_save(sensor);
}

//保存到传感器信息表
void sensor_save(Sensor sensor)
{
	int i=0;
	if(sensor_mag.number < SENSRO_MAX_NUMBER)
	{
		sensor_mag.buff[sensor_mag.number] = sensor;
//	  sensor_mag.buff[sensor_mag.number].key = sensor.key;
//		sensor_mag.buff[sensor_mag.number].pro_lenth = sensor.pro_lenth;
//		sensor_mag.buff[sensor_mag.number].use_time = sensor.use_time;
//		memcpy(sensor_mag.buff[sensor_mag.number].pro,sensor.pro,sensor.pro_lenth);
		
		SENSOR_PRINTF("sensor-buffer-number : %d\n",sensor_mag.number+1);
		SENSOR_PRINTF("sensor-key: %d\t",sensor_mag.buff[sensor_mag.number].key);
		SENSOR_PRINTF("sensor-use-time: %d\t",sensor_mag.buff[sensor_mag.number].use_time);
		SENSOR_PRINTF("sensor-pro-len: %d\n",sensor_mag.buff[sensor_mag.number].pro_lenth);
		#ifdef USE_SENSOR_DEBUG
    SENSOR_PRINTF("PRO: ");
    for(i=0;i<sensor_mag.buff[sensor_mag.number].pro_lenth;i++)
      SENSOR_PRINTF(" %02X",sensor_mag.buff[sensor_mag.number].pro[i]);
      SENSOR_PRINTF("\n");
    #endif
		sensor_mag.number++;
	}
}

//保存到接收数据信息表
void sensor_rcv_save(Sensor_rcv sensor_rcv)
{
	 int i=0;
	if(sensor_data.number < SENSRO_MAX_NUMBER)
	{
	  sensor_data.buff[sensor_data.number] = sensor_rcv;
		
		SENSOR_PRINTF("save-send-pck : %d\n",sensor_data.number+1);
		SENSOR_PRINTF("save-send-key: %d\t",sensor_data.buff[sensor_data.number].key);
		SENSOR_PRINTF("save-send-pck_id: %d\t",sensor_data.buff[sensor_data.number].pck_id);
		SENSOR_PRINTF("save-send-len: %d\n",sensor_data.buff[sensor_data.number].lenth);
		#ifdef USE_SENSOR_DEBUG
    SENSOR_PRINTF("PRO: ");
    for(i=0;i<sensor_data.buff[sensor_data.number].lenth;i++)
      SENSOR_PRINTF(" %02X",sensor_data.buff[sensor_data.number].data[i]);
      SENSOR_PRINTF("\n");
    #endif
    sensor_data.number++;
	}
}

void sensor_rcv_buff_clear()
{
   sensor_data.number=0;
}

void sensor_list_clear()
{
  sensor_mag.number=0;
}

void sensor_save_cmd(uint8_t len,uint8_t *cmd)
{
   sensor_cmd.cmd_lenth = len;
	 memcpy(sensor_cmd.cmd,cmd,len);
}

void sensor_send_cmd()
{
   usart_send_buffer(1,sensor_cmd.cmd,sensor_cmd.cmd_lenth);
}
