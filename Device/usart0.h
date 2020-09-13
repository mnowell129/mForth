#ifndef USART0_H
#define USART0_H

// Specific to USART 0
#define UART_NUMBER                0
#define UART_CLOCK_ATTACH          kFRO12M_to_FLEXCOMM0

#define UART_TX                    UART0_TX_PIN
#define UART_RX                    UART0_RX_PIN

#define UART0_TX_SIZE              1024
#define UART0_RX_SIZE              1024

/**
 * Prototypes
 */

void uart0SetCooked(bool flag);
void uart0SetEcho(bool flag);
bool uart0Kbhit(void);
void uart0EnableReceive(bool value);
void uart0IOInit(void);
uint32_t uart0Init(uint32_t baudRate);
void uart0Stop(void);
void uart0Putch(Byte value);
void uart0Send(Byte *bufferPtr, uint32_t length);
void uart0Puts(char *string);
int32_t uart0GetchTO(uint32_t timeout);
Byte uart0Getch(void);
void uart0Gets(Byte *buffer);
bool uart0IsTransmitComplete(void);
bool uart0Kbhit(void);
void uart0SetEnableReceive(bool value);

#endif
