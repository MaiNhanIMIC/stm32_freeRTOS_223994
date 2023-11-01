/*
 * main.c
 *
 *  Created on: Aug 7, 2023
 *      Author: maitr
 */

#include <stdint.h>
#include "led.h"
#include "delay.h"
#include "clock.h"
#include "uart.h"
#include "low_power.h"
#include "FreeRTOS.h"
#include "task.h"
#include "adc.h"
#include "queue.h"
#include "event_groups.h"
#include "semphr.h"
void vApplicationMallocFailedHook()
{

}

void vApplicationTickHook()
{

}

void vApplicationStackOverflowHook( TaskHandle_t xTask,
                                        char * pcTaskName )
{
	
}

void vApplicationIdleHook()
{

}

void SystemInit()
{
	leds_init();
}

void func_1(void* param)
{
	while(1)
	{
		led_control(BLUE_LED, LED_ON);
		vTaskDelay(1000);
		led_control(BLUE_LED, LED_OFF);
		vTaskDelay(1000);
	}
}

void func_2(void* param)
{
	while(1)
	{
		led_control(RED_LED, LED_ON);
		vTaskDelay(1500);
		led_control(RED_LED, LED_OFF);
		vTaskDelay(1000);
	}
}

float common_memory;
QueueHandle_t temp_queue;
EventGroupHandle_t temp_event;
SemaphoreHandle_t uart_lock;
void func_3(void* param)
{
	UART1_Init();
	for(;;)
	{
		//wait until temp_event is set
		xEventGroupSync(temp_event, 0, 1, 0xffffffff);
		// float temp = common_memory;//read data from common memory
		float temp = 0;
		xSemaphoreTake(uart_lock, 0xffffffff);
		printlog("\033[0;31m[task 3]\033[0m: temperature: [");
		while(uxQueueMessagesWaiting(temp_queue) > 0)
		{
			xQueueReceive(temp_queue, &temp, 0);
			printlog("%.2f, ", temp);
		}
		printlog("\b\b]\r\n");
		xSemaphoreGive(uart_lock);
	}
}

void func_4(void* param)
{
	adc_init();
	int measure_cnt = 0;
	while(1)
	{
		float temp = adc_get_temp_ss();
		// common_memory = temp; //write temp_ss data to common memory
		xQueueSend(temp_queue, &temp, 10000);
		vTaskDelay(500);
		if(++measure_cnt >= 5)
		{
			measure_cnt = 0;
			// set event 
			xEventGroupSetBits(temp_event, 1);
		}
	}
}

void func_5(void* param)
{
	
	while(1)
	{
		xSemaphoreTake(uart_lock, 0xffffffff);
		printlog("\033[0;32m[task 5]\033[0m: hello world\r\n");
		xSemaphoreGive(uart_lock);
		vTaskDelay(500);
	}
}

int main()
{
	TaskHandle_t task_1 = NULL;
	TaskHandle_t task_2 = NULL;
	TaskHandle_t task_3 = NULL;
	TaskHandle_t task_4 = NULL;
	TaskHandle_t task_5 = NULL;
	xTaskCreate(func_1, "task 1", 512, NULL, 0, &task_1);
	xTaskCreate(func_2, "task 2", 512, NULL, 0, &task_2);
	xTaskCreate(func_3, "task 3", 512, NULL, 0, &task_3);
	xTaskCreate(func_4, "task 4", 512, NULL, 0, &task_4);
	// xTaskCreate(func_5, "task 5", 512, NULL, 0, &task_5);
	temp_queue = xQueueCreate(20, sizeof(float));
	temp_event = xEventGroupCreate();
	uart_lock = xSemaphoreCreateMutex();
	vTaskStartScheduler();
	while(1)
	{
		
	}
}

void HardFault_Handler()
{
	led_control(RED_LED, LED_ON);
	while(1);
}
