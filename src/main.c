/**
 ******************************************************************************
 * @file    main.c
 * @author  Ac6
 * @version V1.0
 * @date    06-December-2013
 * @brief   Default main function.
 ******************************************************************************
 */

#include "bspTP4/bsp.h"
#include "cmsis_os.h"
#include "stdio.h"

void vTask1 (void *Parameters);
void vTask2 (void *Parameters);
void vTask3(void *Parameters);
void vTask4 (void *Parameters);
void vTaskManager(void *Parameters);
void vTaskUART(void* pvParameters);

void function_aux (leds_Typedef leds_exp);
QueueHandle_t buttonPushesQueue = NULL;
QueueHandle_t xQueue1;
TaskHandle_t tasks[5],taskManager, UART;

TickType_t ms = 100;
static TickType_t delta= 10;


int main(void)
{
	BSP_Init();

	buttonPushesQueue = xQueueCreate(1, sizeof(int));
	xQueue1 = xQueueCreate(1, sizeof(int));

		xTaskCreate( vTask1, 		"Led UP",		configMINIMAL_STACK_SIZE, NULL, 2, &tasks[1]);
 		xTaskCreate( vTask2, 		"Led DOWN",  	configMINIMAL_STACK_SIZE, NULL, 2, &tasks[2]);
		xTaskCreate( vTask3, 		"Led JUMP", 	configMINIMAL_STACK_SIZE, NULL, 2, &tasks[3]);
		xTaskCreate( vTask4, 		"Led CENTER", 	configMINIMAL_STACK_SIZE, NULL, 2, &tasks[4]);
 		xTaskCreate( vTaskManager, "Task Manager",  configMINIMAL_STACK_SIZE, NULL, 3, &taskManager);
 		xTaskCreate( vTaskUART, 	"UART COM", 	configMINIMAL_STACK_SIZE, NULL, 4, &UART);

		vTaskSuspend(tasks[2]);
		vTaskSuspend(tasks[3]);
		vTaskSuspend(tasks[4]);
		vTaskSuspend(taskManager);


	osKernelStart();



	while(1){
		}
}

void vTaskUART(void* pvParameters)
	{
	(void) pvParameters;                    // Just to stop compiler warnings.
	char msg;
	char buffer[150];
	uint16_t size;

	size = sprintf(buffer, "\nIniciando....\r\n");
	TransmitData(buffer, size);

	for (;;)

		{
		if(xQueueReceive( xQueue1, &msg, portMAX_DELAY))
			{
			size = sprintf(buffer, "\nPatrón actual: %c\n", msg);
			TransmitData(buffer, size);
			size = sprintf(buffer, "Velocidad actual: %d mseg.\n", ms);
			TransmitData(buffer, size);
			}
		vTaskSuspend(UART);
		vTaskDelay(200);
		}
	}


void vTaskManager(void *Parameters)
	{
	uint16_t sig=-1;
	uint16_t state=1;
	char msg;

	while (1)
		{
		xQueueReceive(buttonPushesQueue,&sig,portMAX_DELAY);
		switch (sig)
			{
			case 0:
				{
				state ++;
				if(state <= 4)
					{
					for (uint16_t x= 1 ; x <= 4 ; x++)
						vTaskSuspend(tasks[x]);

					vTaskResume(tasks[state]);
					}
				if(state == 5)
					{
					state=1;
					for (uint16_t x= 1 ; x < 5 ; x++)
						vTaskSuspend(tasks[x]);

					vTaskResume(tasks[state]);
					}
			}
		 	 break;

			case 1:
				{
				state --;
				if(state >= 1)
					{
					for (uint16_t x= 1 ; x < 5 ; x++)
						vTaskSuspend(tasks[x]);

					vTaskResume(tasks[state]);

				}

				if(state == 0)
					{
					state = 4;
					for (uint16_t x= 1 ; x < 5 ; x++)
						vTaskSuspend(tasks[x]);

					vTaskResume(tasks[state]);
					}
				}
			break;

			case 2:
				{
				if (ms == delta)
					ms = ms - delta;

				else if (ms > delta)
					ms = ms - delta;

				else if (ms < delta)
					ms=0;
				}
			break;

			case 3:
				ms = ms + delta;
			break;
		}

	msg = state + '0';

	vTaskResume(UART);
	xQueueSend(xQueue1,&msg, portMAX_DELAY);
	vTaskSuspend(taskManager);
		}
	}


void vTask1 (void *Parameters)
	{
	for(;;)
		{
		for (leds_exp=LED1_EXP; leds_exp<= LED8_EXP; leds_exp++)
			function_aux(leds_exp);
		}
	}

void vTask2 (void *Parameters)
	{
	for(;;)
		{
		for (leds_exp=LED8_EXP; leds_exp >= LED1_EXP; leds_exp--)
			{
			function_aux(leds_exp);
			if(leds_exp==0)
				leds_exp=LED8_EXP;
			}
		}
	}

void vTask3 (void *Parameters)
	{
	for(;;)
		{
		for (leds_exp=LED1_EXP; leds_exp<= LED7_EXP; leds_exp+=2)
			function_aux(leds_exp);

		for (leds_exp=LED8_EXP; leds_exp>= LED2_EXP; leds_exp-=2)
			function_aux(leds_exp);
		}
	}

void vTask4 (void *Parameters)
	{
	for(;;)
		{
		for (uint8_t i=0 ; i<4 ; i++)
			{
			LedOn(top);
			LedOn(bottom);
			vTaskDelay(ms);
			LedOff(top);
			LedOff(bottom);
			top++;
			bottom--;
			}
		top=LED1_EXP;
		bottom=LED8_EXP;
		}
	}

void function_aux (leds_Typedef leds_exp)
	{
	LedOn(leds_exp);
	vTaskDelay(ms);
	LedOff(leds_exp);
	}

