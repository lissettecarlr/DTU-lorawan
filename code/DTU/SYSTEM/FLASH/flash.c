#include "flash.h"


void flash_init()
{
	if(startAddress%MEMORY_PAGE_SIZE !=0)//不是页的开始,将开始处设置为下一个页开始的地方
		startAddress+=(MEMORY_PAGE_SIZE-(startAddress%MEMORY_PAGE_SIZE));
}

///////////////////////
///读取储存器中特定位置的值
///@param -pageNumber 相对于开始地址的地址
///@param -Data 读出的数据存放的地址
///@retval -1 : 读取成功 -0：读取失败
///////////////////////
void flash_read(u16 pageNumber,u16 position,u16* data,u16 length)
{
	u16 dataLength=length;

		while(dataLength)
		{
			*data=(*(__IO uint16_t*)(startAddress+pageNumber*MEMORY_PAGE_SIZE+position  +(length-dataLength)*2));
			++data;
			--dataLength;
		}
}

///////////////////////
///向储存器中特定位置写值
///@param -pageNumber 相对于开始地址的地址
///@param -Data 将要写入的数据
///@retval -1 : 写入成功 -0：写入失败
////
uint8_t flash_write(uint16_t pageNumber,u16 position,uint16_t* data,u16 length)
{
	u16 dataLength=0;
	if(position+length>MEMORY_PAGE_SIZE)
		return 0;
	
	dataLength=length;
	FLASH_Unlock();
	
	if(position==0){
	FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|
					FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
	if(!FLASH_ErasePage(startAddress+pageNumber*MEMORY_PAGE_SIZE))//擦除页
		return 0;
	}
	
	while(dataLength)
	{
		if(FLASH_COMPLETE!=FLASH_ProgramHalfWord(startAddress+pageNumber*MEMORY_PAGE_SIZE+position  +(length-dataLength)*2,*data))
			return 0;
		++data;
		--dataLength;
	}
	FLASH_Lock();
	return 1;
}



//字符串的保存方式为（字长+字符串）
uint8_t flash_write_string(uint16_t pageNumber,u16 position,char* str)
{
	u16 number=0;
	u32 Lenth=0;
	u16 temp;
	FLASH_Unlock();
	if(position ==0){//只有对该页开始位置写入时查出这也,不允许直接从中间开始写
	FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|
					FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
	if(!FLASH_ErasePage(startAddress+pageNumber*MEMORY_PAGE_SIZE))//擦除页
				return 0;
	}
	
	//计算出字符串长度
	while( (*str)!= '\0')
		{number++;str++;}
	if(number+position<1024)
		FLASH_ProgramHalfWord((startAddress+pageNumber*MEMORY_PAGE_SIZE+position)  +Lenth,number); //保存字符串长度
	else
		return 0;
		Lenth+=2;
	str=str - number ;
	
		while((*str)!= '\0')
		{
			temp =( (*str)<<8 )+( *(str+1));			
			str+=2;
			FLASH_ProgramHalfWord(startAddress+pageNumber*MEMORY_PAGE_SIZE+position  +Lenth,temp);
			Lenth+=2;
			number-=2;
			
			if(number==1)
			{
				temp =*str;	
				FLASH_ProgramHalfWord(startAddress+pageNumber*MEMORY_PAGE_SIZE+position +Lenth,temp);
				str++;
			}
			
		}
	FLASH_Lock();
	return 1;
}


//字符串读取
uint8_t flash_read_string(uint16_t pageNumber,u16 position,char *str)
{
	u16 number=0;
	u32 Lenth=0;
	u16 temp;
	u16 temp_number=0;
	number=(*( uint16_t*)(startAddress+pageNumber*MEMORY_PAGE_SIZE+position  +Lenth));
	if(number + position>1024)
		return 0;
	Lenth+=2;
	temp_number = number;
	
		while(number>0)
		{
			temp = (*(__IO uint16_t*)(startAddress+pageNumber*MEMORY_PAGE_SIZE+position  +Lenth));
			*str = (u8)(temp>>8);
			*(str+1) = (u8)temp;
			Lenth +=2;
			number-=2;
			str+=2;
			
			if(number ==1)//如果是奇数
			{
				temp = (*(__IO uint16_t*)(startAddress+pageNumber*MEMORY_PAGE_SIZE+position  +Lenth));
				*(str) = (u8)temp;
				number--;
				str++;
			}
		}
		
		*(str) = '\0' ;
		str= str-temp_number;
	return 1;

}

//擦除
uint8_t flash_clear(uint16_t pageNumber)
{
	FLASH_Unlock();
		FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|
		FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
		if(!FLASH_ErasePage(startAddress+pageNumber*MEMORY_PAGE_SIZE))//擦除页
				return 0;
		FLASH_Lock();
	return 1;
}
