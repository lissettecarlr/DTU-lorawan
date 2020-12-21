#include "fifo.h"

void FifoInit( Fifo_t *fifo, char *buf, uint16_t size )
{
    fifo->buf = buf;
    fifo->r = 0;
    fifo->w = 0;
    fifo->size = size;
}

void FifoFlush( Fifo_t *fifo )
{
    fifo->r = 0;
    fifo->w = 0;
}

int FifoPush( Fifo_t *fifo, char value )
{
    if( IsFifoFull(fifo) == 0 )
    {
        fifo->buf[fifo->w] = value;
        fifo->w = (fifo->w + 1) % fifo->size;
        return 0;
    }
    
    return -1;
}

int FifoPop( Fifo_t *fifo, char *value )
{
    if( IsFifoEmpty(fifo) == 0 )
    {
        *value = fifo->buf[fifo->r];
        fifo->r = (fifo->r + 1) % fifo->size;
        return 0;
    }
    
    return -1;
}

uint16_t FifoDataLen( Fifo_t *fifo )
{
    uint16_t len;
    
    len = (fifo->w + fifo->size - fifo->r) % fifo->size;
    
    return len;
}
