#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>

#include "type.h"
#include "rtos.h"
#include "fsl_device_registers.h"
#include "pin_mux.h"
#include "fsl_iocon.h"
#include "fsl_gpio.h"
#include "fsl_dma.h"
#include "fsl_inputmux.h"
#include "fsl_power.h"

#include "fsl_usart.h"
#include "fsl_device_registers.h"
#include "fsl_flexcomm.h"
#include "usart.h"
#include "USART0.h"
#include "ASCII.h"
// Local Data
RTOS_SEMA_PTR      TX_SEMA;
RTOS_SEMA_PTR      RX_SEMA;

Byte RX_BUFFER[UART_RX_SIZE];
Byte TX_BUFFER[UART_TX_SIZE];

UART_INDICES(UART_NUMBER);

#define uartKbhit             UART_FUNCTION(Kbhit)
#define uartIOInit            UART_FUNCTION(IOInit)
#define uartInit              UART_FUNCTION(Init)
#define uartSetXOn            UART_FUNCTION(SetXOn)
#define uartSetXOff           UART_FUNCTION(SetXOff)
#define uartPutch             UART_FUNCTION(Putch)
#define uartGetch             UART_FUNCTION(Getch)
#define uartSend              UART_FUNCTION(Send)
#define uartGets              UART_FUNCTION(Gets)
#define uartPuts              UART_FUNCTION(Puts)


#define interruptHandler      FLEXCOMM_ISR(UART_NUMBER)
#define interruptHandler      FLEXCOMM_ISR(UART_NUMBER)


bool uartKbhit(void)
{
   return((RX_COUNT > 0) ? 1 : 0);
}


void uartIOInit(void)
{
//   INITGPIOAFDIGITAL(UART_PIN(TX));
//   INITGPIOAFDIGITAL(UART_PIN(RX));

}


//void FLEXCOMM0_IRQHandler(void)
void interruptHandler(void)
{
   ISRCONTEXT;
   USART_Type *usartHW = THE_UART(UART_NUMBER);
   volatile Byte receivedByte;

   /* If RX overrun. */
   if(usartHW->FIFOSTAT & USART_FIFOSTAT_RXERR_MASK)
   {
      /* Clear rx error state. */
      usartHW->FIFOSTAT |= USART_FIFOSTAT_RXERR_MASK;
      /* clear rxFIFO */
      usartHW->FIFOCFG |= USART_FIFOCFG_EMPTYRX_MASK;
   }
   while(((usartHW->FIFOSTAT & USART_FIFOSTAT_RXNOTEMPTY_MASK)) ||
         (isTransmitterEnabledAndReadyForAnotherByte(usartHW)))
   {
      /* Receive data */
      if(usartHW->FIFOSTAT & USART_FIFOSTAT_RXNOTEMPTY_MASK)
      {
         receivedByte = usartHW->FIFORD;
         // if the buffer is full, then drop the new bytes
         if(RX_COUNT >= UART_RX_SIZE - 1)
         {
            // don't put receivedByte in the buffer
         }
         else // won't overflow
         {
            RX_BUFFER[RX_WRITE_INDEX] = receivedByte;
            RX_WRITE_INDEX++;
            if(RX_WRITE_INDEX >= UART_RX_SIZE)
            {
               RX_WRITE_INDEX = 0;
            }
            RX_COUNT++;
            if(RX_COUNT == 0)
            {
               // then it was -1, someone was waiting
               ISR_SEMA_PUT(RX_SEMA);
            } //if  someone waiting
         } //if  buffer full
      }
      if(isTransmitterEnabledAndReadyForAnotherByte(usartHW))
      {
         // USART_ClearFlag((USART_TypeDef *)THE_UART_BASE,USART_FLAG_TXE);
         // not needed on this part
         TX_COUNT--;
         TX_READ_INDEX++;
         if(TX_READ_INDEX >= UART_TX_SIZE)
         {
            TX_READ_INDEX = 0;
         }
         if(TX_COUNT > 0)
         {
            usartHW->FIFOWR = TX_BUFFER[TX_READ_INDEX];
         }
         else  // no more bytes to send
         {
            // disable the interrupt so it won't keep occurring due to empty buffer
            // now some tricky bits when we are using a FIFO
            // when there are no more bytes to send
            // but the fifo is full
            // we need to get an interupt when the FIFO empties
            // so we can disable any transmit interrupts.
            // to be simple for startup purposes if we set the fifo to 0
            // then it works like a single register uart
            usartHW->FIFOINTENCLR = USART_FIFOINTENCLR_TXLVL(1);
         }
         // this logic works because
         // the waiting task only waits on this semaphore
         // after it has incremented the count to
         // > UART_TX_SIZE.
         // Since we just decremented it, and it is now = UART_TX_SIZE
         // the task, through a critical region is already waiting
         if(TX_COUNT == UART_TX_SIZE)
         { // let a waiting task know there is now room
           // let a waiting task know there is now room
            ISR_SEMA_PUT(TX_SEMA);
         }
      } // transmitter interrupt
   } // while interrupts pending
}


void uartPutch(Byte value)
{
   volatile    CONTEXT;
   USART_Type *usartHW = THE_UART(UART_NUMBER);

   // Byte error;
   ENTER_CRITICAL();
   // if buffer is empty then we need to prime it with the first character
   if(TX_COUNT == 0)
   {
      // transmitter is idle, buffer is empty
      // show one character being output
      TX_COUNT++;
      TX_BUFFER[TX_WRITE_INDEX] = value;
      // reenable the tx interrupt now we have more data
      usartHW->FIFOINTENSET = USART_FIFOINTENSET_TXLVL(1);
      // write into tx register
      usartHW->FIFOWR = TX_BUFFER[TX_WRITE_INDEX];
   } //if
   else if(TX_COUNT >= UART_TX_SIZE)
   {
      // uart full, wait for a space to open up

      TX_COUNT++;
      EXIT_CRITICAL();
      // wait for a char to tx so there'll be more room
      SEMA_GET(TX_SEMA,WAIT_FOREVER,error);
      // now there's room
      ENTER_CRITICAL();
      TX_BUFFER[TX_WRITE_INDEX] = value;

   } // else if
   else
   {
      // finally there's room and the count is > 0
      TX_COUNT++;
      TX_BUFFER[TX_WRITE_INDEX] = value;
   }

   TX_WRITE_INDEX++;
   if(TX_WRITE_INDEX >= UART_TX_SIZE)
   {
      TX_WRITE_INDEX = 0;
   }
   EXIT_CRITICAL();
} //void uartNPutch(Byte value)


void uartSend(Byte *bufferPtr,uint32_t length)
{
   while(length != 0)
   {
      uartPutch(*bufferPtr++);
      length--;
   }
}

void uartPuts(char *string)
{
   uartSend((Byte *)string,strlen(string) + 1);
} 

uint8_t uartGetch(void)
{
   uint8_t value;
   volatile CONTEXT;
   ENTER_CRITICAL();  
   RX_COUNT--; 
   if(RX_COUNT < 0)
   {  
      EXIT_CRITICAL();
      SEMA_GET(RX_SEMA,WAIT_FOREVER,error);
   }  
   else  
   {
      EXIT_CRITICAL(); 
   }  
   value = RX_BUFFER[RX_READ_INDEX++];
   if(RX_READ_INDEX >= UART_RX_SIZE)
   {
      RX_READ_INDEX = 0;
   }
   return(value);
} 


void uartGets(Byte *buffer)
{
   Int16 value;
   *buffer = 0;
   int count = 0;
   while(1)
   {
      value = uartGetch();
      if(value == UART_GETCH_ERROR)
      {
         continue;
      }

      switch(value)
      {
         case ASCII_BS:
            if(count > 0)
            {
               count--;
               buffer--;
               *buffer = 0;
               uartSend(BACKSPACE" "BACKSPACE,3);
            }
            break;
         case '\r':
            return;
         case '\n':
            return;
            break;
         case ASCII_NUL:
            *buffer++ = (Byte)(0xff & value);
            count++;
            *buffer = 0;
            return;

         default:
            if(isprint(value))
            {
               uartPutch(value & 0xff);
               *buffer++ = (Byte)(0xff & value);
               count++;
               *buffer = 0;
            } //if
            break;
      } //switch
   } //while
} //uartNGets


uint32_t uartInit(uint32_t baudRate)
{


   USART_Type *base = THE_UART(UART_NUMBER);
   uint32_t priority;



   RX_SEMA = xSemaphoreCreateCounting(1024,0); // OSSemCreate(0);
   TX_SEMA = xSemaphoreCreateCounting(1024,0); // OSSemCreate(0);


   RX_WRITE_INDEX = 0;
   RX_READ_INDEX = 0;
   TX_WRITE_INDEX = 0;
   TX_READ_INDEX = 0;
   RX_COUNT = 0;
   TX_COUNT = 0;

   uartIOInit();

   /* attach 12 MHz clock to FLEXCOMMX (debug console) */
   CLOCK_AttachClk(UART_CLOCK_ATTACH);
   CLOCK_EnableClock(CLOCK_CHANNEL(UART_NUMBER));

   usartDeInitHardware(base);
   usartInitHardware(base,baudRate,12000000U);

   priority = NVIC_EncodePriority(NVIC_GetPriorityGrouping(),5,2); // magic
   NVIC_SetPriority(FLEXCOMM_NUMBER(UART_NUMBER),priority);
   NVIC_EnableIRQ(FLEXCOMM_NUMBER(UART_NUMBER));

   // Don't enable until ready to transmit
   // base->FIFOINTENSET = USART_FIFOINTENSET_TXLVL(1);
   base->FIFOINTENSET = USART_FIFOINTENSET_RXLVL(1);
   // UART_Cmd(USARTy,ENABLE);
   return(1);
}


