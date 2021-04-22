#ifndef CONIO_H
#define CONIO_H

//#include "usart0.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>

///*************************************************************
///*************************************************************

#define putchar putch

#define putch     uart0Putch
#define send      uart0Send
#define puts      uart0Puts
#define getchTO   uart0GetchTO
#define getch     uart0Getch
#define gets      uart0Gets
#define isTxDone  uart0IsTransmitComplete
#define kbhit     uart0Kbhit
#define printf    printf0
#define forthPrintf    forthPrintf0

void _printf0(char *format,va_list list);
void printf0(char *format,...);
void forthPrintf0(char *format,...);

#endif
int vsprintf(char *buffer,const char *format,va_list list);
int sprintf(char *buffer,const char *format,...);
int snprintf(char *, size_t, const char *, ...);
int vsnprintf(char *, size_t, const char *, va_list);
