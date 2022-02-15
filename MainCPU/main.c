#include "FreeRTOS.h"
/* Freescale includes. */
#include "fsl_device_registers.h"
#include "pin_mux.h"
#include <stdbool.h>
#include "fsl_iocon.h"

#include "fsl_usart_freertos.h"
#include "fsl_usart.h"

#include "pin_mux.h"
#include "clock_config.h"
#include "type.h"
#include "conio.h"
#include "rtos.h"
#include "forth.h"
#include "board.h"

// comment

#define forthRootTask_PRIORITY                       1
#define forthRootTask                                forthRootTask
#define forthRootTask_STACK_SIZE                     512        



#define uart_task_PRIORITY 1
#define led_task_PRIORITY  0
#define USART_NVIC_PRIO 5

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

#define RTOS_DELAY(a)            vTaskDelay(a)
#define BOARD_LED_PORT BOARD_LED_BLUE_GPIO_PORT
#define BOARD_LED_PIN BOARD_LED_BLUE_GPIO_PIN
//#define BOARD_LED_PORT BOARD_LED_GREEN_GPIO_PORT
//#define BOARD_LED_PIN BOARD_LED_GREEN_GPIO_PIN
#include "fsl_gpio.h"

int32_t rate = 1000;

void forthBlinkRate(FCONTEXT)
{
   rate = UPOP();
}
void setRate(int32_t value)
{
   rate = value;
}

/******************************************************************************************/
/******************************************************************************************/
/*
Sample code from NXP
*/
/******************************************************************************************/
/******************************************************************************************/
#define IOCON_PIO_FUNC0 0x00u         /*!<@brief Selects pin function 0 */

void assignPinToGPIO(void)
{
   CLOCK_EnableClock(kCLOCK_InputMux);
   /* Enables the clock for the I/O controller.: Enable Clock. */
   CLOCK_EnableClock(kCLOCK_Iocon);
   CLOCK_EnableClock(kCLOCK_Gpio0);


   const uint32_t port0_pin29_config = ( /* Pin is configured as PIO1_7 */
                                       IOCON_PIO_FUNC1 |
                                       /* No addition pin function */
                                       IOCON_PIO_MODE_INACT |
                                       /* Standard mode, output slew rate control is enabled */
                                       IOCON_PIO_SLEW_STANDARD |
                                       /* Input function is not inverted */
                                       IOCON_PIO_INV_DI |
                                       /* Enables digital function */
                                       IOCON_PIO_DIGITAL_EN |
                                       /* Open drain is disabled */
                                       IOCON_PIO_OPENDRAIN_DI);
   /* PORT1 PIN7 (coords: 9) is configured as PIO1_7 */
   IOCON_PinMuxSet(IOCON,0U,29U,port0_pin29_config);
  const uint32_t port0_pin30_config = ( /* Pin is configured as PIO1_7 */
                                       IOCON_PIO_FUNC1 |
                                       /* No addition pin function */
                                       IOCON_PIO_MODE_INACT |
                                       /* Standard mode, output slew rate control is enabled */
                                       IOCON_PIO_SLEW_STANDARD |
                                       /* Input function is not inverted */
                                       IOCON_PIO_INV_DI |
                                       /* Enables digital function */
                                       IOCON_PIO_DIGITAL_EN |
                                       /* Open drain is disabled */
                                       IOCON_PIO_OPENDRAIN_DI);
   /* PORT1 PIN7 (coords: 9) is configured as PIO1_7 */
   IOCON_PinMuxSet(IOCON,0U,30U,port0_pin30_config);


   const uint32_t port1_pin7_config = ( /* Pin is configured as PIO1_7 */
                                       IOCON_PIO_FUNC0 |
                                       /* No addition pin function */
                                       IOCON_PIO_MODE_INACT |
                                       /* Standard mode, output slew rate control is enabled */
                                       IOCON_PIO_SLEW_STANDARD |
                                       /* Input function is not inverted */
                                       IOCON_PIO_INV_DI |
                                       /* Enables digital function */
                                       IOCON_PIO_DIGITAL_EN |
                                       /* Open drain is disabled */
                                       IOCON_PIO_OPENDRAIN_DI);
   /* PORT1 PIN7 (coords: 9) is configured as PIO1_7 */
   IOCON_PinMuxSet(IOCON,1U,7U,port1_pin7_config);

   const uint32_t port1_pin4_config = ( /* Pin is configured as PIO1_7 */
                                       IOCON_PIO_FUNC0 |
                                       /* No addition pin function */
                                       IOCON_PIO_MODE_INACT |
                                       /* Standard mode, output slew rate control is enabled */
                                       IOCON_PIO_SLEW_STANDARD |
                                       /* Input function is not inverted */
                                       IOCON_PIO_INV_DI |
                                       /* Enables digital function */
                                       IOCON_PIO_DIGITAL_EN |
                                       /* Open drain is disabled */
                                       IOCON_PIO_OPENDRAIN_DI);
   /* PORT1 PIN7 (coords: 9) is configured as PIO1_7 */
   IOCON_PinMuxSet(IOCON,1U,4U,port1_pin4_config);

}

#define APP_BOARD_TEST_LED_PORT 1U
#define APP_BOARD_TEST_LED_PIN 7U
#define APP_SW_PORT BOARD_SW1_GPIO_PORT
#define APP_SW_PIN BOARD_SW1_GPIO_PIN

void setUpGreen(void)
{

   gpio_pin_config_t led_config =
   {
      kGPIO_DigitalOutput,
      0,
   };
   assignPinToGPIO();
   /* Init output LED GPIO. */
   GPIO_PortInit(GPIO,APP_BOARD_TEST_LED_PORT);
//   GPIO_PortInit(GPIO,APP_SW_PORT);
   GPIO_PinInit(GPIO,APP_BOARD_TEST_LED_PORT,APP_BOARD_TEST_LED_PIN,&led_config);
   GPIO_PinWrite(GPIO,APP_BOARD_TEST_LED_PORT,APP_BOARD_TEST_LED_PIN,0);
}


#if 0
TASK_DATA(ledTask);
#define ledInputQueue_DEPTH    8
RTOS_QUEUE_OBJECT(ledInputQueue);

void ledTaskFunction(void *pvParameters)
{
   gpio_pin_config_t led_config =
   {
      kGPIO_DigitalOutput,
      0,
   };

//   setUpGreen();
   assignPinToGPIO();

   /* Init output LED GPIO. */
   GPIO_PortInit(GPIO,BOARD_LED_PORT);
   GPIO_PortInit(GPIO,APP_BOARD_TEST_LED_PORT);
   GPIO_PinInit(GPIO,APP_BOARD_TEST_LED_PORT,APP_BOARD_TEST_LED_PIN,&led_config);
   GPIO_PinInit(GPIO,BOARD_LED_PORT,BOARD_LED_PIN,&led_config);

   while(1)
   {
      /* Delay 1000 ms */
      RTOS_DELAY(rate);
      GPIO_PortToggle(GPIO,BOARD_LED_PORT,1u << BOARD_LED_PIN);
      GPIO_PortToggle(GPIO,APP_BOARD_TEST_LED_PORT,1 << APP_BOARD_TEST_LED_PIN);
   }

}
#endif


const char startString1[] =
                            "0x1000 200 wordlist stuff\n"\
   "stuff add-to-order definitions\n"\
   "\x04";

void startOtherThreads(FCONTEXT)
{

}


void addExternalWords(FCONTEXT)
{
} 


/**
 * Memory allocation for the user Terminal Input Buffer (TIB) 
 * and the PAD where formatting of output strings takes place 
 * for the root level forth interpreter. 
 */
///*************************************************************
///*************************************************************
#define USER_TIB_SIZE     512
#define USER_PAD_SIZE     128
char      tibBuffer1[USER_TIB_SIZE];
uint32_t  padBuffer1[USER_PAD_SIZE];


void forthTask(void *pvParameters)
{
   uart0Init(115200);
   #define ESC  "\x1B"

   printf(ESC "[2J" ESC "[H");
   printf(ESC "[31m" ESC "[40m");
   printf(ESC "[0m");
   printf("mForth>\r\n");

   initCoreForth(startString1);

   user1->userVariables.namedVariables.setEcho = NULL;
   user1->tibBuffer = tibBuffer1;
   user1->padBuffer = padBuffer1;
   user1->userVariables.namedVariables.tibSize = USER_TIB_SIZE;
   user1->userVariables.namedVariables.padSize = USER_PAD_SIZE;
   user1->userVariables.namedVariables.user_id = ROOT_USER_ID;

   runForth(user1);

}




int main(void)
{

   CLOCK_EnableClock(kCLOCK_InputMux);
   /* attach 12 MHz clock to FLEXCOMM0 (debug console) */
   CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

   /* attach main clock to SDIF */
   //CLOCK_AttachClk(BOARD_SDIF_CLK_ATTACH);

   BOARD_InitPins();
   /* This function is used to cover the IP bug which the DATA4-7 pin should be configured, otherwise the SDIF will not
    * work */
   // BOARD_BootClockPLL144M();
   BOARD_BootClockFROHF96M();
   /* need call this function to clear the halt bit in clock divider register */
   // CLOCK_SetClkDiv(kCLOCK_DivSdioClk,(uint32_t)(SystemCoreClock / FSL_FEATURE_SDIF_MAX_SOURCE_CLOCK + 1U),true);

   // BOARD_InitDebugConsole(); 

   CLOCK_EnableClock(kCLOCK_Gpio0);
   CLOCK_EnableClock(kCLOCK_Gpio1);

   if(xTaskCreate(forthTask,"Forth_task",configMINIMAL_STACK_SIZE + 10,NULL,uart_task_PRIORITY,NULL) != pdPASS)
   {
      while(1)
      ;
   }
   vTaskStartScheduler();
   for(;;)
   ;
}


void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize)
{
   ///*************************************************************
   ///*************************************************************
   /// If the buffers to be provided to the Idle task are declared
   /// inside this function then they must be declared static -
   /// otherwise they will be allocated on the stack and so not
   /// exists after this function exits.
   ///*************************************************************
   ///*************************************************************

   static StaticTask_t xIdleTaskTCB;
   static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

   /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
   state will be stored. */
   *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

   /* Pass out the array that will be used as the Idle task's stack. */
   *ppxIdleTaskStackBuffer = uxIdleTaskStack;

   /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
   Note that, as the array is necessarily of type StackType_t,
   configMINIMAL_STACK_SIZE is specified in words, not bytes. */
   *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/* configUSE_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
 * application must provide an implementation of vApplicationGetTimerTaskMemory()
 * to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize)
{
   /* If the buffers to be provided to the Timer task are declared inside this
    * function then they must be declared static - otherwise they will be allocated on
    * the stack and so not exists after this function exits. */
   static StaticTask_t xTimerTaskTCB;
   static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

   /* Pass out a pointer to the StaticTask_t structure in which the Timer
    * task's state will be stored. */
   *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

   /* Pass out the array that will be used as the Timer task's stack. */
   *ppxTimerTaskStackBuffer = uxTimerTaskStack;

   /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
    * Note that, as the array is necessarily of type StackType_t,
    * configTIMER_TASK_STACK_DEPTH is specified in words, not bytes. */
   *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
/*-----------------------------------------------------------*/

/* Stack overflow hook. */
void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName)
{
   /* Silence warning about unused parameters. */
   (void)xTask;

   /*Force an assert. */
   configASSERT(pcTaskName == 0);
}
/*-----------------------------------------------------------*/

void vGetRegistersFromStack(uint32_t *pulFaultStackAddress)
{
   /* These are volatile to try and prevent the compiler/linker optimising them
    * away as the variables never actually get used.  If the debugger won't show the
    * values of the variables, make them global my moving their declaration outside
    * of this function. */
   volatile uint32_t r0;
   volatile uint32_t r1;
   volatile uint32_t r2;
   volatile uint32_t r3;
   volatile uint32_t r12;
   volatile uint32_t lr;  /* Link register. */
   volatile uint32_t pc;  /* Program counter. */
   volatile uint32_t psr; /* Program status register. */

   r0 = pulFaultStackAddress[0];
   r1 = pulFaultStackAddress[1];
   r2 = pulFaultStackAddress[2];
   r3 = pulFaultStackAddress[3];

   r12 = pulFaultStackAddress[4];
   lr  = pulFaultStackAddress[5];
   pc  = pulFaultStackAddress[6];
   psr = pulFaultStackAddress[7];

   /* Remove compiler warnings about the variables not being used. */
   (void)r0;
   (void)r1;
   (void)r2;
   (void)r3;
   (void)r12;
   (void)lr;  /* Link register. */
   (void)pc;  /* Program counter. */
   (void)psr; /* Program status register. */

   /* When the following line is hit, the variables contain the register values. */
   for(;;)
   {
   }
}

