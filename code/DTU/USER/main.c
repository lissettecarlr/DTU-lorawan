#include <stm32f10x.h>
#include <delay.h>
#include <usart.h>
#include "weichuan-m100d.h"
#include "led.h"
#include "wdg.h"
#include "string.h"
#include "sensor-mag.h"
#include "flash.h"

#define USE_DEBUG 1

#if USE_DEBUG
#define PRINTF(format, ...) printf (format, ##__VA_ARGS__)
#else
#define PRINTF(info)
#endif

#define DEFAULT_UPLOAD_CYCLE             60     //默认上传周期
#define QUERY_WIRELESS_MOD_STATE_CYCLE   60     //对模组状态定时查询
#define QUERY_CONFIG_LIST_CYCLE          60     //对平台请求配置单周期
#define WIRELESS_MOD_USART               2      //无线模组使用的串口号
#define WIRELESS_SEND_PORT               128    //模组上行端口
#define DEFAULT_SENSOR_BAUD              9600   //传感器串口默认波特率

//唯传模组复位引脚
void mod_rst_init()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 				
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 			
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;					
	GPIO_Init(GPIOA, &GPIO_InitStructure);		
}
void m100d_rst()
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_5);
	DelayMs(110);
	GPIO_SetBits(GPIOA,GPIO_Pin_5);
}

//下行DTU命令
enum{
  CMD_JUMP_SYSTEM_CONFIG=0XC1,
	CMD_JUMP_OPERATION=0XC2,
	CMD_REST=0XC3,
	CMD_REST_WIRELESS = 0XC4,
	CMD_REQUEST_LIST = 0XC5,//请求节点配置信息
};

//通信协议 包类型
enum
{
  SYSTEM_CONFIG_PCK =1,
	SENSOR_CONFIG_PCK =2,
	SENSOR_DATA_PCK   =3,
	SENSOR_CMD_PCK    =4,
	SYSTEM_CMD_PCK    =5,
	SYSTEM_STATE_CPK  =6,
};

//状态应答
enum{
ACK_NODE_CONFIG=1,
ACK_NODE_OPERATION=2,
ACk_NODE_LIST=3,
};
//系统运行状态
typedef enum
{
	STATE_NO_NETWORL=0,
	STATE_FLASH_READ,
	STATE_WAIT_SYSTEM_CONFIG_INIT,
  STATE_WAIT_SYSTEM_CONFIG,
	STATE_OPERATION_INIT,
	STATE_OPERATION,
}operation_state;


operation_state system_state = STATE_NO_NETWORL;
static double mod_state_record =0;//运行时，定时查询模块状态
static uint16_t sensor_baud = 0;//表单波特率
static double sensor_update_upload =0;//运行时，定时上报传感器数据
static double wait_system_config_led=0;//配置时，闪烁LED
static double request_system_config=0;//配置时,请求配置单

static uint16_t system_upload_time = DEFAULT_UPLOAD_CYCLE;//默认上传周期
static uint8_t sensor_cmd_flag=0;//是否有下行传感器命令，在发现是传感器命令包时被置1，发送后被置0
static uint8_t request_system_config_flag=0; //是否接收到下行配置信息，如果没有则会一直发送配置请求
static uint8_t request_sensor_config_flag=0; //是否接收到下行传感器配置信息
static uint8_t wireless_send=1; //允许无线发送标志位
static uint8_t rev_config_temp_flag=1;//接收到数据包开始计时标志

//function
void save_system_sensor_list(void);
void system_config(uint8_t *data);
void system_cmd(uint8_t *data);
void switch_download_data(uint8_t *data);
void send_system_state(uint8_t sw);
void deel_mod_rcv_joinpck(uint8_t data);
void deel_mod_rcv_send_state(uint8_t data);
void deel_mod_rcv_state(uint8_t data);
void w100d_com_callback(void);
void system_init(void);
void main_flash_read(void);
void main_system_config_init(void);
void main_system_config(void);
void main_operation_init(void);
void main_operation(void);

//将系统配置和传感器配置保存到flash
void save_system_sensor_list()
{
   uint16_t temp[512];
	 uint16_t p=0;
	 int i=0;
	 temp[0] = sensor_mag.number;    //54
//	 temp[1] = 
	 p+=1;
	 for(i=0;i<sensor_mag.number;i++)
	{
	   memcpy(temp+p,&sensor_mag.buff[i],sizeof(sensor_mag.buff[i]));
		 p+=(sizeof(sensor_mag.buff[i])/2);
	}
	memcpy(temp+p,&sensor_baud,2);
	p+=1;
	memcpy(temp+p,&system_upload_time,2);
	p+=1;
	flash_write(0,0,temp,p);
}

//系统配置包格式是固定
//上报周期-波特率-检验位-数据位-停止位
void system_config(uint8_t *data)
{
	 sensor_baud = ((data[2] <<8) | data[3] )*100;
   system_upload_time = (data[0]<<8) | data[1]; 
   sensor_mag_set_baud(sensor_baud);
	 PRINTF("DTU-DEBUG-->upload : %d, baud: %d\n",system_upload_time,sensor_baud);
}

//下行的系统命令
void system_cmd(uint8_t *data)
{
	 if(data[0] == 0xFA)
	 {	
		switch (data[1])
		{
			case CMD_JUMP_SYSTEM_CONFIG:
					system_state = STATE_WAIT_SYSTEM_CONFIG_INIT;
					PRINTF("DTU-DEBUG-->download cmd system config\n");
				break;
			case CMD_JUMP_OPERATION:
					system_state = STATE_OPERATION_INIT;
					save_system_sensor_list();
					PRINTF("DTU-DEBUG-->download cmd operattion\n");
				break;
			case CMD_REST:
				m100d_rst();
				*((u32 *)0xE000ED0C) = 0x05fa0004;
			PRINTF("DTU-DEBUG-->reset all\n");
				//复位唯传模组和主控
				break;
			case CMD_REST_WIRELESS:
				  m100d_rst();
			PRINTF("DTU-DEBUG-->reset weichuan\n");
				  break;
			case CMD_REQUEST_LIST:
					send_system_state(3);
			    PRINTF("DTU-DEBUG-->download cmd send list\n");
				break;
			}	
		}else if(data[0] == 0xfb)
	  {
		  system_upload_time = (u16)(data[1]<<8) + data[2];
			PRINTF("DTU-DEBUG-->upload_time set %d\n",system_upload_time);
		}
	 
}
//根据包头区分类型，并且去掉了第一个字节的包头 
//在配置状态才会处理配置包
//在任何状态都会处理下行命令
void switch_download_data(uint8_t *data)
{
   switch (data[0])
	 {
	   case SYSTEM_CONFIG_PCK://下行系统配置
			   if(system_state == STATE_WAIT_SYSTEM_CONFIG)
				 {
					 request_system_config_flag =1;
		       system_config(data+1);
				 }
			 break;
		 case SENSOR_CONFIG_PCK://下行传感器配置（多个传感器会多个包）
			   if(system_state == STATE_WAIT_SYSTEM_CONFIG)
				 {
					 request_sensor_config_flag=1;
			     sensor_config(data+1);
				 }
			 break;
		 case SYSTEM_CMD_PCK: //该帧格式是0x05 FA XX 
			  system_cmd(data+1);
			 break;
		 case SENSOR_CMD_PCK://下行给传感器的命令，该命令不能在更新周期处理
			  sensor_save_cmd(data[1],&data[2]);
		    sensor_cmd_flag=1;
			 break;
	 }
}

void send_system_state(uint8_t sw)
{
	 lora_send_pck SendModPck;
	 uint8_t data[3]={0x06,0x00,0x00};
	 uint8_t list_send[24];
	 uint8_t i=0,j=0;
	 switch (sw)
	 {
		 case ACK_NODE_CONFIG:
			 data[1] = ACK_NODE_CONFIG;
		   SendModPck=M100dFrameSend(data,3,0,WIRELESS_SEND_PORT);	 
			 usart_send_buffer(2,SendModPck.data,SendModPck.Lenth);
		   wireless_send=0;
			 break;
		 case ACK_NODE_OPERATION:
			 data[1] = ACK_NODE_OPERATION;
		   data[2] = sensor_mag.number;
		   SendModPck=M100dFrameSend(data,3,0,WIRELESS_SEND_PORT);	 
			 usart_send_buffer(2,SendModPck.data,SendModPck.Lenth);
		   wireless_send=0;
			 break;
		 case ACk_NODE_LIST: //返回模组配置清单
			 list_send[j++] = 0x06;
		   list_send[j++] = 0x03;
		   
		   list_send[j++] = (u8)((sensor_baud/100)>>8);
		   list_send[j++] = (u8)(sensor_baud/100);
		   list_send[j++] = (u8)(system_upload_time>>8);
		   list_send[j++] = (u8)system_upload_time;
		 
		  for(i=0;i<sensor_mag.number;i++)
		  {
			   list_send[j] = (u8)(sensor_mag.buff[i].key >>8);
				 list_send[j+1] = (u8)sensor_mag.buff[i].key; 
				 j+=2;
			}
		   SendModPck=M100dFrameSend(list_send,j,0,WIRELESS_SEND_PORT);	 
			 usart_send_buffer(2,SendModPck.data,SendModPck.Lenth);
		   wireless_send=0;
			 break;
	 }
}

void deel_mod_rcv_joinpck(uint8_t data)
{
	 static uint8_t joinpck_flag=1; //保证只执行一次，当出现主控主动复位模组的时候，不能再次进入配置状态
  	  switch(data)
		  {
			 case LORA_JOIN_OK:
				    if(joinpck_flag)//系统启动->入网->配置->运行
						{
				      system_state = STATE_FLASH_READ;
							joinpck_flag=0;
						}
						else//复位无线模组后自动重连
						{
						 	 LED_SET_ON(); 
						}
            PRINTF("DTU-DEBUG-->jion OK\n");
				 break;
			 case LORA_JOIN_FAIL:
				    LED_SET_OFF();
			      PRINTF("DTU-DEBUG-->jion fail\n");
				 break;
			 case LORA_JOIN_ING:
            PRINTF("DTU-DEBUG-->jion ing\n");
				 break;
		 }
}

void deel_mod_rcv_send_state(uint8_t data)
{
   	  switch(data)
		 {
			 case SEND_OK:
				   PRINTF("DTU-DEBUG-->send ok\n");	
				   break;
		   case SEND_FINISH: //发送完成
				   PRINTF("DTU-DEBUG-->send over\n");
			     wireless_send=1;
				   LED_SET_ON(); 
				 break;
			 case MSG_ERR_NOT_JOIN: //未入网
				   PRINTF("DTU-DEBUG-->send no network\n");
			     wireless_send=1;
				   LED_SET_OFF();
				 break;
			 case ERR_BUSY: //设备忙
				   PRINTF("DTU-DEBUG-->send busy\n");
			     wireless_send=1;
			     //m100d_rst();//解决唯传设备忙的BUG，直接复位无线模组
				   LED_SET_OFF();
				 break;
			 default:
			    {
						wireless_send=1;
						PRINTF("DTU-DEBUG-->send data error\n");
					  LED_SET_OFF();
					}
		 }
}

void deel_mod_rcv_state(uint8_t data)
{
  		 switch(data)
		 {
			 case 2: //已入网
			 {
				 PRINTF("DTU-DEBUG-->state network ok\n");
				 LED_SET_ON();
			 }break;
			 case 3: //入网失败
			 {
				 PRINTF("DTU-DEBUG-->state network fail\n");
				 LED_SET_OFF();
			 }break;
			  case 4://发送中
			 {
				 PRINTF("DTU-DEBUG-->state send while \n"); //建议发生问题将唯传模组复位
				 
			 }break;
				case 5://发送成功
			 {
				 PRINTF("DTU-DEBUG-->state send ok \n");
				 LED_SET_ON();
			 }break;
			 default:
			  {
				 PRINTF("DTU-DEBUG-->state send other \n");
				}
		 }
}

void w100d_com_callback()
{
  //通讯模组接收到数据后，拆帧发送给传感器
	//通信模组接收到通信模组返回，做出相应操作
	lora_send_pck RcvModPck;
	lora_send_pck SendAckPck;
	uint8_t data[200];
	uint8_t lenth= GetSerialReciveDataSize(WIRELESS_MOD_USART);
	if(lenth >=200)
	{
		ClearSerialBuffer(WIRELESS_MOD_USART);
		return;
	}
	GetSerialReciveData(WIRELESS_MOD_USART,(char *)data,lenth);
	RcvModPck=M100FrameReciveData(data,lenth);
	SendAckPck = M100dFrameACK(SEND_OK);
	if(RcvModPck.type == LORA_MOD_DATA) //如果接受到平台下发数据
  {
		 switch_download_data(RcvModPck.data+1);//第一个包是端口号
		 usart_send_buffer(WIRELESS_MOD_USART,SendAckPck.data,SendAckPck.Lenth);
	}
	else if(RcvModPck.type == LORA_MOD_STATE)//启动入网时的返回包
	{
		 deel_mod_rcv_joinpck(RcvModPck.data[0]);
		 usart_send_buffer(WIRELESS_MOD_USART,SendAckPck.data,SendAckPck.Lenth);
	}
	else if(RcvModPck.type == LORA_MOD_RST_OK)//复位后回包，不做处理
	{
	  usart_send_buffer(WIRELESS_MOD_USART,SendAckPck.data,SendAckPck.Lenth);
	}
	else if(RcvModPck.type == LORA_MOD_RESPONSE)//发送数据后的状态返回
	{
		deel_mod_rcv_send_state(RcvModPck.data[0]);
	}		
	else if(RcvModPck.type == LORA_MOD_RESPONSE_STATE) //请求命令得到的返回
	{
		deel_mod_rcv_state(RcvModPck.data[0]);
	}		
	ClearSerialBuffer(WIRELESS_MOD_USART);
}


void system_init()
{
	sysclk_init();
	m100d_rst();
	usart2_init(); //无线
	usart3_init(); //打印
	timer2_init();
	sensor_mag_init(DEFAULT_SENSOR_BAUD);
	set_usart2_irq_callback(w100d_com_callback);
	flash_init();
	LED_Init();
	LED_SET_OFF();
	IWDG_Init(4,625); //设定1s看门狗
}

//从flash中读取配置，如果没有配置则进入接收状态，如果有则直接跳转到运行态
//数据保存顺序 : 传感器数量-传感器表单-波特率-上传周期
void main_flash_read()
{
   uint16_t number=0xffff;
	 uint16_t data_p;//指向需要读取的开始位置
	 uint8_t i=0;
	 flash_read(0,0,&number,1);
	 if(number < SENSRO_MAX_NUMBER &&number >0) //是否flash里面有保存信息
	 {
	    //读取数据保存到传感器表中
		  Sensor temp;
      data_p +=2;
		  //读取传感器
		  for(i=0;i<number;i++)
		  {
			   flash_read(0,data_p,(u16 *) &temp,sizeof(temp)/2);
				 sensor_save(temp);
				 data_p += (sizeof(temp));
				 if(data_p >=0x400) //读取大于一页
					 while(1);//看门狗复位
		  }
			
			//读取系统配置
			flash_read(0,data_p,&sensor_baud,1);
			data_p+=2;
			flash_read(0,data_p,&system_upload_time,1);
			sensor_mag_set_baud(sensor_baud);
       PRINTF("DTU-DEBUG-->upload : %d, baud: %d\n",system_upload_time,sensor_baud);

			//跳过运行初始化，直接进入运行状态
			PRINTF("DTU-DEBUG-->flash read OK , system state operation\n");
		  send_system_state(ACK_NODE_OPERATION);//告诉平台已经开始正常运行
		  mod_state_record = GetNowTime();
		  sensor_update_upload = GetNowTime();
		  LED_SET_ON();
		  system_state =STATE_OPERATION;
	 }
	 else
	 {
		 PRINTF("DTU-DEBUG-->read flash fail jump system-config\n");
	   system_state = STATE_WAIT_SYSTEM_CONFIG_INIT;
	 }
}

void main_system_config_init()
{			  
	sensor_rcv_buff_clear();//待发送队列清空
	sensor_list_clear();//传感器表清空
	request_system_config_flag=0;
	request_sensor_config_flag=0;
	sensor_baud = 0;
	request_system_config = GetNowTime();
	wait_system_config_led =GetNowTime();
	rev_config_temp_flag=1;
	send_system_state(ACK_NODE_CONFIG);//发送配置表请求
}

//系统配置态
void main_system_config()
{	
	  static double rev_config_timeout=0;

		if(ClockTool(&wait_system_config_led,1))//系统配置模式LED闪烁
		{
		   LED_REVERSE();
		} 
	 if(ClockTool(&request_system_config,QUERY_CONFIG_LIST_CYCLE) && (!(request_system_config_flag|request_sensor_config_flag)))
		{
		   send_system_state(ACK_NODE_CONFIG);//发送配置表请求
		}
		//如果在开始接收后一段时间没有数据包则重启请求接收
		if(request_system_config_flag|request_sensor_config_flag) 
		{
		    if(rev_config_temp_flag)//避免刚进入就超时
				{
					 rev_config_timeout = GetNowTime();
					 rev_config_temp_flag =0;
				}
        if(ClockTool(&rev_config_timeout,300) && rev_config_temp_flag ==0)
        {
					rev_config_temp_flag=1;
					send_system_state(3);//发送配置单信息
					//PRINTF("DTU-DEBUG-->rcv config timeout,jump config init\n");
				  //system_state = STATE_WAIT_SYSTEM_CONFIG_INIT;
				}
		}
}

void main_operation_init()
{
	  //此处应该基本判断一下系统配置是否完善，不完善则情况传感器清单，重新进入配置模式
	  if(  !(request_system_config_flag && request_sensor_config_flag)  )
		{
		   PRINTF("DTU-DEBUG-->The system configuration is incomplete\n");
			 system_state = STATE_WAIT_SYSTEM_CONFIG_INIT; //跳转到系统配置状态	 
		}
		else
		{
			 PRINTF("DTU-DEBUG-->system state operation\n");
		   send_system_state(ACK_NODE_OPERATION);//告诉平台已经开始正常运行
		   mod_state_record =GetNowTime();
		   sensor_update_upload =GetNowTime();
		   LED_SET_ON();
		   system_state = STATE_OPERATION;
		}
}

//系统运行态
void main_operation()
{
	  int i =0;//,j=0;
	  static  uint8_t send_data[50];
		static  uint8_t send_lenth=0;
	  static lora_send_pck SendModPck;
   	static double timeout_send=0;//等待模组发送完成
	  static uint8_t timeout_number=0; //超时次数
    if(ClockTool(&mod_state_record,QUERY_WIRELESS_MOD_STATE_CYCLE)) //定时查询通信模组状态
		{
	    lora_send_pck GetModStatePck;
    	GetModStatePck = lora_send_request_state();
		  usart_send_buffer(WIRELESS_MOD_USART,GetModStatePck.data,GetModStatePck.Lenth); 
		}
		if(ClockTool(&sensor_update_upload,system_upload_time)) 
		{
		   sensor_update(); //更新完成后，发送数据表
			for(i=0;i<sensor_data.number;i++)
			{
 				 //封发送包
			   send_data[0] = SENSOR_DATA_PCK;
				 send_data[1] = sensor_data.buff[i].pck_id;
				
				 send_data[2] = (uint8_t)((sensor_data.buff[i].key)>>8);
				 send_data[3] = (uint8_t)(sensor_data.buff[i].key);
				 send_data[4] = sensor_data.buff[i].lenth;
				 memcpy(&send_data[5],sensor_data.buff[i].data,send_data[4]);
				 send_lenth = 5 + send_data[4];
				 
				 timeout_send = GetNowTime();
				 while(!wireless_send)//等待允许发送标准
				 {
					  IWDG_Feed(); 
				    if(ClockTool(&timeout_send,10))
						{
							timeout_number++;
							PRINTF("DTU-DEBUG-->timeout_number:%d\n",timeout_number);
							PRINTF("DTU-DEBUG-->wait send timeout \n");
							break;
						}
				 }				 
				 //数据发送 				 
				 SendModPck=M100dFrameSend(send_data,send_lenth,0,WIRELESS_SEND_PORT);
				 usart_send_buffer(WIRELESS_MOD_USART,SendModPck.data,SendModPck.Lenth);
				 wireless_send=0;//该标志位在模组发送数据应答处理函数中被置位1
		    
				 //全部超时 那基本就是模组死掉了
				 if(timeout_number >= sensor_data.number * 3)
				 {
					 PRINTF("weichuan reset \n");
		
					 wireless_send = 1;
					  m100d_rst();
					  timeout_number=0;
				 }
//				#if USE_DEBUG
//				  PRINTF("DTU-DEBUG-->send data : ");
//          for(j=0;j<send_lenth;j++)
//          PRINTF(" %02X",send_data[j]);
//          PRINTF("\n");
//				#endif
			}
		}
			if(sensor_cmd_flag)//如有有下行传感器命令
			{
				 PRINTF("DTU-DEBUG-->send sensor cmd \n");
				 sensor_send_cmd();
				 sensor_cmd_flag=0;
			}
}

int main()
{
	system_init();
	mod_rst_init();
	m100d_rst();//启动是复位无线模组
	PRINTF("DTU-DEBUG-->system init ok\n");
	while(1) 
	{
		switch(system_state)
		{
		  case STATE_NO_NETWORL:
				break;
			case STATE_FLASH_READ:
				  main_flash_read();
				break;
			case STATE_WAIT_SYSTEM_CONFIG_INIT: //在配置前的准备工作
				  main_system_config_init();
				  system_state = STATE_WAIT_SYSTEM_CONFIG;
				break;
		  case STATE_WAIT_SYSTEM_CONFIG:
          main_system_config();	
			
				break;
			case STATE_OPERATION_INIT: //在正常运行前的准备工作
          main_operation_init();
				break;
			case STATE_OPERATION: 
				  main_operation();
				break;
		}
		IWDG_Feed(); 
	}
}

