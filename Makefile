FREERTOS_DIR=FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel
COREHTTP_DIR=FreeRTOS-LTS/FreeRTOS/coreHTTP/source
HEADER=-u _printf_float\
 -specs=nano.specs \
 -specs=nosys.specs\
  -mfpu=fpv4-sp-d16 \
  -mthumb -mfloat-abi=hard \
  -mcpu=cortex-m4 -std=gnu11 \
  -ICustom_Driver/Inc/ \
  -I$(FREERTOS_DIR)/include/ \
  -I.\
  -I$(FREERTOS_DIR)/portable/GCC/ARM_CM4F/ \
  -I$(COREHTTP_DIR)/include/  \
  -I$(COREHTTP_DIR)/interface/  \
  -I$(COREHTTP_DIR)/dependency/3rdparty/llhttp/include/ 


All:
# build freeRTOS kernel
	arm-none-eabi-gcc -c $(FREERTOS_DIR)/croutine.c $(HEADER) -o build/croutine.o
	arm-none-eabi-gcc -c $(FREERTOS_DIR)/event_groups.c $(HEADER) -o build/event_groups.o
	arm-none-eabi-gcc -c $(FREERTOS_DIR)/list.c $(HEADER) -o build/list.o
	arm-none-eabi-gcc -c $(FREERTOS_DIR)/queue.c $(HEADER) -o build/queue.o
	arm-none-eabi-gcc -c $(FREERTOS_DIR)/stream_buffer.c $(HEADER) -o build/stream_buffer.o
	arm-none-eabi-gcc -c $(FREERTOS_DIR)/tasks.c $(HEADER) -o build/tasks.o
	arm-none-eabi-gcc -c $(FREERTOS_DIR)/timers.c $(HEADER) -o build/timers.o
	arm-none-eabi-gcc -c $(FREERTOS_DIR)/portable/GCC/ARM_CM4F/port.c $(HEADER) -o build/port.o
	arm-none-eabi-gcc -c $(FREERTOS_DIR)/portable/MemMang/heap_4.c $(HEADER) -o build/heap_4.o

# build coreHTTP lib
	arm-none-eabi-gcc -c $(COREHTTP_DIR)/core_http_client.c $(HEADER) -o build/core_http_client.o
	arm-none-eabi-gcc -c $(COREHTTP_DIR)/dependency/3rdparty/llhttp/src/api.c $(HEADER) -o build/api.o
	arm-none-eabi-gcc -c $(COREHTTP_DIR)/dependency/3rdparty/llhttp/src/http.c $(HEADER) -o build/http.o
	arm-none-eabi-gcc -c $(COREHTTP_DIR)/dependency/3rdparty/llhttp/src/llhttp.c $(HEADER) -o build/llhttp.o
# build application
	arm-none-eabi-gcc -c main.c $(HEADER) -o build/main.o
	arm-none-eabi-gcc -c Custom_Driver/Src/uart.c $(HEADER) -o build/uart.o
	arm-none-eabi-gcc -c Custom_Driver/Src/adc.c $(HEADER) -o build/adc.o
	arm-none-eabi-gcc -c Custom_Driver/Src/led.c $(HEADER) -o build/led.o
	arm-none-eabi-gcc -c Custom_Driver/Src/pwm.c $(HEADER) -o build/pwm.o
	arm-none-eabi-gcc -c Custom_Driver/Src/delay.c $(HEADER) -o build/delay.o
	arm-none-eabi-gcc -c Custom_Driver/Src/clock.c $(HEADER) -o build/clock.o
	arm-none-eabi-gcc -c Custom_Driver/Src/capture.c $(HEADER) -o build/capture.o
	arm-none-eabi-gcc -c Custom_Driver/Src/low_power.c $(HEADER) -o build/low_power.o
	arm-none-eabi-gcc -c -x assembler-with-cpp startup_stm32f411vetx.s -mcpu=cortex-m4 -std=gnu11 -o build/startup.o
	arm-none-eabi-gcc build/*.o $(HEADER) -T"STM32F411VETX_FLASH.ld" -Wl,-Map="build/file.map" -Wl,--gc-sections -static -o build/blink_led.elf
	arm-none-eabi-objcopy -O ihex build/blink_led.elf build/blink_led.hex
	arm-none-eabi-objcopy -O binary build/blink_led.elf build/blink_led.bin

Clean:
	rm build/*

Flash:
	ST-LINK_CLI -c SWD  -P build/blink_led.hex -V -Run