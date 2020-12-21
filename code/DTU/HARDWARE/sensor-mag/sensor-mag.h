#ifndef __SENSOR_MAG_H
#define __SENSOR_MAG_H

#include "stm32f10x.h"

#define SENSRO_MAX_NUMBER 10

typedef struct
{
	 uint16_t   key; 
	 uint8_t    use_time;
	 uint8_t    pro_lenth;
	 uint8_t    pro[50];
}Sensor;

typedef struct 
{
   uint8_t number;
	 Sensor buff[SENSRO_MAX_NUMBER]; //最多存储10个传感器
}Sensor_mag;

extern Sensor_mag sensor_mag;

//传感器返回数据结构体
typedef struct 
{
  uint8_t pck_id;
	uint16_t key;
	uint8_t lenth;
	uint8_t data[50];
}Sensor_rcv;

typedef struct 
{
   uint8_t number;
	 Sensor_rcv buff[SENSRO_MAX_NUMBER];
}Sensor_data;
extern Sensor_data sensor_data;


//发送给传感器的命令
typedef struct
{
  uint8_t cmd_lenth;
	uint8_t cmd[50];
}Sensor_cmd;
extern Sensor_cmd sensor_cmd;

void sensor_mag_init(int baud); 
void sensor_mag_callback(void);
void sensor_config(uint8_t *data);
void sensor_save(Sensor sensor); //将传感器数保存的buff中去
void sensor_rcv_save(Sensor_rcv sensor_rcv);//传感器返回数据保存到buff中去
void sensor_update(void);
void sensor_rcv_buff_clear(void);//清空接收待上传数据
void sensor_list_clear(void);//清空传感器清单
void sensor_save_cmd(uint8_t len,uint8_t *cmd);
void sensor_send_cmd(void);
void sensor_mag_set_baud(int baud);
#endif
