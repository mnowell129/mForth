#include "type.h"
#include "conio.h"
#include "snprintf99.h"

#define UART0_BUFFER_SIZE  256
static char buffer0[UART0_BUFFER_SIZE];
void _printf0(char *format,va_list list)
{
   int length;
   _rpl_vsnprintf(&length,buffer0,UART0_BUFFER_SIZE,format,list);
   //uart0Send((Byte *)buffer0,length);
}

void printf0(char *format,...)
{
   va_list list;
   va_start(list,format);
   _printf0(format,list);
   va_end(list);
}

void forthPrintf0(char *format,...)
{
   va_list list;

   va_start(list,format);
   _printf0(format,list);
   va_end(list);
}


