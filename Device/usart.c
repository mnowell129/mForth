#include <stdbool.h>
#include <stdint.h>

#include "type.h"
#include "rtos.h"
/* Freescale includes. */
#include "fsl_device_registers.h"
#include "pin_mux.h"
#include "fsl_iocon.h"
#include "fsl_gpio.h"
#include "fsl_usart.h"
#include "fsl_device_registers.h"
#include "fsl_flexcomm.h"
#include "usart.h"

bool isTransmitterEnabledAndReadyForAnotherByte(USART_Type *usartHW)
{
   // are the interrupts enabled
   if(((usartHW->FIFOINTENSET) & USART_FIFOINTENCLR_TXLVL_MASK) == USART_FIFOINTENCLR_TXLVL_MASK)
   {
      // is there room in the fifo
      if((usartHW->FIFOSTAT & USART_FIFOSTAT_TXNOTFULL_MASK))
      {
         return(true);
      }
   }
   return(false);
}

void usartDeInitHardware(USART_Type *base)
{

   base->FIFOINTENCLR = USART_FIFOINTENCLR_TXERR_MASK | USART_FIFOINTENCLR_RXERR_MASK | USART_FIFOINTENCLR_TXLVL_MASK |
                        USART_FIFOINTENCLR_RXLVL_MASK;
   base->FIFOCFG &= ~(USART_FIFOCFG_DMATX_MASK | USART_FIFOCFG_DMARX_MASK);
   base->CFG &= ~(USART_CFG_ENABLE_MASK);
}
bool usartInitHardware(USART_Type *base,uint32_t baudRate, uint32_t srcClock_Hz)
{
   int result;
   /* initialize flexcomm to USART mode */
   result = FLEXCOMM_Init(base,FLEXCOMM_PERIPH_USART);
   if(kStatus_Success != result)
   {
      return(false);
   }
   base->FIFOCFG |= USART_FIFOCFG_EMPTYTX_MASK | USART_FIFOCFG_ENABLETX_MASK;
   /* setup trigger level */
   base->FIFOTRIG &= ~(USART_FIFOTRIG_TXLVL_MASK);
   base->FIFOTRIG |= USART_FIFOTRIG_TXLVL(0);
   /* enable trigger interrupt */
   // don't do this until we are ready to transmit
   base->FIFOTRIG |= USART_FIFOTRIG_TXLVLENA_MASK;

   base->FIFOCFG |= USART_FIFOCFG_EMPTYRX_MASK | USART_FIFOCFG_ENABLERX_MASK;
   /* setup trigger level */
   base->FIFOTRIG &= ~(USART_FIFOTRIG_RXLVL_MASK);
   base->FIFOTRIG |= USART_FIFOTRIG_RXLVL(0);
   /* enable trigger interrupt */
   base->FIFOTRIG |= USART_FIFOTRIG_RXLVLENA_MASK;

   base->CFG = USART_CFG_PARITYSEL(kUSART_ParityDisabled) | USART_CFG_STOPLEN(kUSART_OneStopBit) |
               USART_CFG_DATALEN(kUSART_8BitsPerChar) | USART_CFG_LOOP(false) |
               USART_CFG_SYNCEN(kUSART_SyncModeDisabled >> 1) | USART_CFG_SYNCMST(kUSART_SyncModeDisabled) |
               USART_CFG_CLKPOL(kUSART_RxSampleOnFallingEdge) | USART_CFG_ENABLE_MASK;

   /* Setup baudrate */
   return(USART_SetBaudRate(base,baudRate,srcClock_Hz));
}

/**
 * @brief   status register is passed because it is read once. 
 *          so we don't accidently clear something by reading the register twice
 * 
 * @author byk1 (6/7/2018)
 * 
 * @param statusRegister 
 * @param theUart 
 * 
 * @return bool 
 */
bool areInterruptsDone(USART_Type *base)
{
   return(false);
}


