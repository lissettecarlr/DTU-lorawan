#ifndef __FIFO_H__
#define __FIFO_H__

//#include "type.h"
#include "stdint.h"

/*!
 * \brief Check if the fifo is empty
 */
#define IsFifoEmpty( fifo )                     ((fifo)->r == (fifo)->w)

/*!
 * \brief Check if the fifo is full
 */
#define IsFifoFull( fifo )                      ((((fifo)->w+1) % (fifo)->size) == (fifo)->r)

/*!
 * FIFO object description
 */
typedef struct
{
    char *buf;              //! Pointer to the data buffer
    uint16_t r;             //! Last reading position
    uint16_t w;             //! Last written position
    uint16_t size;          //! Total used bytes of the data buffer
} Fifo_t;

/*!
 * Initializes the FIFO object
 *
 * \param [IN] fifo     Pointer to the FIFO object
 * \param [IN] buf      Pointer to the buffer used as FIFO
 * \param [IN] size     Size of the buffer
 */
void FifoInit( Fifo_t *fifo, char *buf, uint16_t size );

/*!
 * Flushes the FIFO
 *
 * \param [IN] Pointer to the FIFO object
 */
void FifoFlush( Fifo_t *fifo );

/*!
 * Pushes data to the FIFO
 *
 * \param [IN] fifo     Pointer to the FIFO object
 * \param [IN] value    Data to be pushed into the FIFO
 *
 * \retval result [0:success, FIFO is not full, -1:failure, FIFO is full]
 */
int FifoPush( Fifo_t *fifo, char value );

/*!
 * Pops data from the FIFO
 *
 * \param [IN] fifo         Pointer to the FIFO object
 * \param [IN] value        Pointer to the data store 
 *
 * \retval result [0:success, obtain valid data, FIFO is not empty, -1:failure, FIFO is empty]
 */
int FifoPop( Fifo_t *fifo, char *value );

/*!
 * Gets the number of valid data in the FIFO
 *
 * \param [IN] fifo         Pointer to the FIFO object
 *
 * \retval length Valid data number
 */
uint16_t FifoDataLen( Fifo_t *fifo );

#endif
