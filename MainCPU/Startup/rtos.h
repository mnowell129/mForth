/*
Public Domain Open Source
Written by Charles M Nowell, Jr.
For muliple uses.
Free and open to all for use. 
 */
#ifndef RTOS_H
#define RTOS_H

///*************************************************************
///*************************************************************
   #include "freertos.h"
   #include "semphr.h"
   #include "task.h"
   #include "queue.h"


   #define RTOS_TICKS_PER_SECOND configTICK_RATE_HZ

   // user needs to define name_STACK_SIZE
   // and name
   #define TASK_STACK_DATA(a)    StackType_t  a##StackData[a##_STACK_SIZE]
   #define TASK_TCB_DATA(a)      StaticTask_t a##TaskTCB
   #define TASK_PTR(a)           TaskHandle_t a##TaskPtr
   #define TASK_DATA(a)          TaskHandle_t a##TaskPtr;\
                                 StaticTask_t a##TaskTCB;\
                                 StackType_t  a##StackData[a##_STACK_SIZE]

   #define EXTERN_TASK_DATA(a)   extern TaskHandle_t a##TaskPtr;\
                                 extern StaticTask_t a##TaskTCB;\
                                 extern StackType_t  a##StackData[a##_STACK_SIZE]

// note for FreeRTOS V.9x or later, with static task data
    #define CREATE_THREAD(name,params,function)  name##TaskPtr = \
    xTaskCreateStatic(function,\
    #name,\
    name##_STACK_SIZE,\
    (void *)(params),\
    name##_PRIORITY,\
    (StackType_t * const)(name##StackData),\
    (StaticTask_t * const)&(name##TaskTCB));\
    if(name##TaskPtr == NULL)while(1)


   #define CONTEXT               BaseType_t error
 //  #define CONTEXT                
   #define ERROR_VALUE
   #define ISRCONTEXT            BaseType_t    preemptYes = pdFALSE
   #define ENTER_CRITICAL()      taskENTER_CRITICAL()
   #define EXIT_CRITICAL()       taskEXIT_CRITICAL()

   #define ISR_CRITICAL_CONTEXT     uint32_t  basePriorityValue

   #define ENTER_CRITICAL_ISR()     basePriorityValue = taskENTER_CRITICAL_FROM_ISR()
   #define EXIT_CRITICAL_ISR()       taskEXIT_CRITICAL_FROM_ISR( basePriorityValue )

   #define RTOS_SEMA             void
   #define RTOS_SEMA_PTR         SemaphoreHandle_t
   #define RTOS_SEMA_OBJECT(a)   SemaphoreHandle_t a;StaticSemaphore_t a##Data
   #define RTOS_COUNTING_SEMA_OBJECT(a,b,c)   SemaphoreHandle_t a;StaticSemaphore_t a##Data;\
   const uint32_t a##_INITIAL = b;const uint32_t a##_MAX = c
   #define EXTERN_RTOS_SEMA_OBJECT(a)   extern SemaphoreHandle_t a;extern StaticSemaphore_t a##Data
   #define RTOS_SEMA_DATA(a)     StaticSemaphore_t  a##Data


//   #define SEMA_CREATE(a)                        a=xSemaphoreCreateBinary()
//   #define SEMA_CREATE_COUNTING(a,max,initial)   a = xSemaphoreCreateCounting(max,initial)
//   #define SEMA_CREATE_BINARY(a)                 a=xSemaphoreCreateBinary();xSemaphoreGive(a)

//    #define SEMA_CREATE(a)                        a=xSemaphoreCreateBinary()
   #define SEMA_CREATE(a)               a = xSemaphoreCreateCountingStatic(a##_MAX,a##_INITIAL,&a##Data)
   #define SEMA_CREATE_STATIC(a)               a = xSemaphoreCreateCountingStatic(a##_MAX,a##_INITIAL,&a##Data)
   #define SEMA_CREATE_COUNTING(ptr,max,initial)               ptr = xSemaphoreCreateCounting(max,initial)
   #define SEMA_CREATE_BINARY(a)                 a=xSemaphoreCreateBinary();xSemaphoreGive(a)
//   #define SEMA_CREATE_BINARY_STATIC(a)                 a=xSemaphoreCreateBinaryStatic();xSemaphoreGive(a)
   #define SEMA_CREATE_BINARY_STATIC(a)                 a=xSemaphoreCreateBinaryStatic(&a##Data);xSemaphoreGive(a)
   #define SEMA_CREATE_BINARY_STATIC_NO_GIVE(a)         a=xSemaphoreCreateBinaryStatic(&a##Data)
   #define SEMA_GET(a,t,e)       e = xSemaphoreTake(a,t)
   #define SEMA_GET_WAIT_FOREVER(a)       xSemaphoreTake(a,portMAX_DELAY)
   #define SEMA_PUT(a)           xSemaphoreGive(a)
   #define ISR_SEMA_GET(a,t,e)   e = xSemaphoreTakeFromISR(a,t)
   #define ISR_SEMA_PUT(a)       xSemaphoreGiveFromISR(a,((BaseType_t * const)(&preemptYes)))
   #define ISR_EXIT()            portEND_SWITCHING_ISR(preemptYes)


   #define RTOS_QUEUE_PTR        QueueHandle_t
   #define RTOS_QUEUE            void
   #define RTOS_QUEUE_OBJECT(a)  QueueHandle_t a;StaticQueue_t a##Data;uint32_t a##Array[a##_DEPTH]
   #define EXTERN_RTOS_QUEUE_OBJECT(a)  extern QueueHandle_t a;extern StaticQueue_t a##Data; extern uint32_t a##Array[a##_DEPTH]


   #define COMMON_QUEUE_SIZE 4

//   #define QUEUE_CREATE(array,queueLength,itemSize,handle)          handle = xQueueCreate(queueLength,itemSize)
//   #define QUEUE_CREATE(array,queueLength,itemSize,handle)          handle = xQueueCreateStatic(queueLength,itemSize)
   #define QUEUE_CREATE(symbol)         symbol = xQueueCreateStatic(symbol##_DEPTH,4,(uint8_t *)(symbol##Array),&symbol##Data);\
   vQueueAddToRegistry(symbol,#symbol)
   #define QUEUE_GET(a,pData,t,e)       e = xQueueReceive(a,(void *)&(pData),t)
   #define QUEUE_QUERY(a,pData,t,e)     e = xQueuePeek(a,(void *)&(pData),t)
   #define QUEUE_PUT(a,pData,t,e)       e = xQueueSend(a,(void*)&(pData),t)
   #define ISR_QUEUE_PUT(a,pdata)       xQueueSendFromISR(a,(void *)&(pdata),(BaseType_t * const)&preemptYes)


   #define WAIT_FOREVER                 portMAX_DELAY
   #define WAIT_NOT_AT_ALL       0
   #define OS_ERROR_NONE         pdPASS
   #define OS_ERROR_TIMEOUT      errQUEUE_EMPTY
   #define ISR_EXIT()            portEND_SWITCHING_ISR(preemptYes)

   #define FAILTEST(e)           ((e) == pdFALSE)
   #define PASSTEST(e)           ((e) != pdFALSE)
   #define OSTIME()                 ((Int32)(xTaskGetTickCount()))

   #define RTOS_DELAY(a)            vTaskDelay(a)
   #define RTOS_TICKS_PER_SECOND    configTICK_RATE_HZ
   #define MSECS_TO_TICKS(a)        (a == WAIT_FOREVER ? WAIT_FOREVER : ((a)*1000 / (1000000u/RTOS_TICKS_PER_SECOND)))
   #define RTOS_MSEC_DELAY(a)       vTaskDelay(MSECS_TO_TICKS(a))


#endif

///*************************************************************
///*************************************************************
#define SECONDS_FROM_MSEC(v)            ((v)/1000)
#define MINUTES_FROM_SECONDS(v)         ((v)/60)
#define HOURS_FROM_MINUTES(v)           ((v)/60)
#define SECONDS_FROM_MINUTES(v)         ((v)*60)
#define MINUTES_FROM_HOURS(v)           ((V)*60)
#define SECONDS_PER_MINUTE              (60)
#define MINUTES_PER_HOUR                (60)
#define HOURS_PER_DAY                   (24)
#define MINUTES_PER_DAY                 (MINUTES_PER_HOUR * HOURS_PER_DAY)
#define SECONDS_PER_HOUR                (SECONDS_PER_MINUTE * MINUTES_PER_HOUR)
#define MSEC_PER_SEC                    (1000)
#define MSEC_PER_MINUTE                 (MSEC_PER_SEC * SECONDS_PER_MINUTE)
#define MSEC_PER_HOUR                   (MSEC_PER_MINUTE * MINUTES_PER_HOUR)
#define MSEC_PER_DAY                    (HOURS_PER_DAY*MSEC_PER_HOUR)
#define TICKS_PER_MINUTE                (RTOS_TICKS_PER_SECOND * SECONDS_PER_MINUTE)

#define MINUTES_TO_TICKS(v)             ((v) * TICKS_PER_MINUTE)
#define SECONDS_TO_TICKS(v)             ((v) * RTOS_TICKS_PER_SECOND)
#define HOURS_TO_TICKS(v)               ((v) * MINUTES_PER_HOUR * RTOS_TICKS_PER_SECOND * SECONDS_PER_MINUTE)

#define TICKS_TO_MINUTES(v)             ((v)/ TICKS_PER_MINUTE)
#define TICKS_TO_SECONDS(v)             ((v)/ RTOS_TICKS_PER_SECOND)

///*************************************************************
///*************************************************************


