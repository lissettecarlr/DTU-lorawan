#ifndef  __FLASH_H
#define  __FLASH_H


#include "stm32f10x.h"
#include "stm32f10x_flash.h"


#define MEMORY_PAGE_SIZE  (uint16_t)0x400
/* Page status definitions */
#define MEMORY_STATUS_ERASED                  ((uint16_t)0xFFFF)     /* PAGE is empty */
#define MEMORY_STATUS_RECEIVE_DATA            ((uint16_t)0xEEEE)     /* PAGE is marked to receive data */
#define MEMORY_STATUS_VALID_PAGE              ((uint16_t)0x0000)     /* PAGE containing valid data */

static uint32_t startAddress = (0x08000000+63*MEMORY_PAGE_SIZE);

void flash_init(void);
void flash_read(u16 pageNumber,u16 position,u16* data,u16 length);
uint8_t flash_write(uint16_t pageNumber,u16 position,uint16_t* data,u16 length);
uint8_t flash_write_string(uint16_t pageNumber,u16 position,char* str);
uint8_t flash_read_string(uint16_t pageNumber,u16 position,char *str);
uint8_t flash_clear(uint16_t pageNumber);

#endif

