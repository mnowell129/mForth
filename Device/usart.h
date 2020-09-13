#ifndef USART_H
#define USART_H

#ifndef UART_GETCH_ERROR
#define UART_GETCH_ERROR  -1
#define BACKSPACE  "\x08"
#endif


// Usart commmon routines
bool isTransmitterEnabledAndReadyForAnotherByte(USART_Type *usartHW);
bool areInterruptsDone(USART_Type *usartHW);
void usartDeInitHardware(USART_Type *base);
bool usartInitHardware(USART_Type *base,uint32_t baudRate, uint32_t srcClock_Hz);


/*********************************************************************************************/
/*********************************************************************************************/
/* Generic to all uarts.*/
/*********************************************************************************************/
/*********************************************************************************************/

#define BUFFER_SIZE(x,y)   CONCAT3(UART,x,y)
#define UART_RX_SIZE       BUFFER_SIZE(UART_NUMBER,_RX_SIZE)
#define UART_TX_SIZE       BUFFER_SIZE(UART_NUMBER,_TX_SIZE)


#define TO_NUMBER(x)      Number##x
#define _MAKE(x,y)        uart##x##y

#define UART_INDICES(x) volatile uint32_t _MAKE(x,RxWriteIndex);\
volatile uint32_t _MAKE(x,RxWriteIndex);\
volatile uint32_t _MAKE(x,RxReadIndex);\
volatile uint32_t _MAKE(x,TxWriteIndex);\
volatile uint32_t _MAKE(x,TxReadIndex);\
volatile int32_t  _MAKE(x,TxCount);\
volatile int32_t  _MAKE(x,RxCount)

#define __MAKE(x,y)        _MAKE(x,y)
#define RX_WRITE_INDEX     __MAKE(UART_NUMBER,RxWriteIndex)
#define RX_READ_INDEX      __MAKE(UART_NUMBER,RxReadIndex)
#define TX_WRITE_INDEX     __MAKE(UART_NUMBER,TxWriteIndex)
#define TX_READ_INDEX      __MAKE(UART_NUMBER,TxReadIndex)

#define RX_COUNT           __MAKE(UART_NUMBER,RxCount)
#define TX_COUNT           __MAKE(UART_NUMBER,TxCount)


#define TX_BUFFER           __MAKE(UART_NUMBER,TxBuffer)
#define RX_BUFFER           __MAKE(UART_NUMBER,RxBuffer)
#define TX_SEMA             __MAKE(UART_NUMBER,TxSema)
#define RX_SEMA             __MAKE(UART_NUMBER,RxSema)



typedef void *UartPointerType;

#define UART_FUNCTION(x)      __MAKE(UART_NUMBER,x)
#define UART_VARIABLE(x)      __MAKE(UART_NUMBER,x)
#define __BOB(x,y)            UART##x##_##y##_PIN
#define _BOB(x,y)             __BOB(x,y)
#define UART_PIN(x)           _BOB(UART_NUMBER,x)
#define CONCAT(a,b)           a##b
#define CONCAT3(a,b,c)         a##b##c
#define CLOCK_CHANNEL(a)      CONCAT(kCLOCK_FlexComm,a)
#define FLEXCOMM_NUMBER(a)    CONCAT3(FLEXCOMM,a,_IRQn)
#define THE_UART(a)           CONCAT(USART,a)

// E.G. 
//void FLEXCOMM0_IRQHandler(void)
#define FLEXCOMM_ISR(a)    CONCAT3(FLEXCOMM,a,_IRQHandler)


#endif
