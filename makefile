ifeq ($(strip $(TARGET)),)
TARGET = F407
#TARGET = F091
endif

CROSS_COMPILE	= arm-none-eabi-
AS		= $(CROSS_COMPILE)as
CC		= $(CROSS_COMPILE)gcc
CXX		= $(CROSS_COMPILE)g++
LD		= $(CROSS_COMPILE)ld
OBJCOPY		= $(CROSS_COMPILE)objcopy

CFLAGS =	-Os -g -DTARGET_$(TARGET)				\
		-I./inc							\
		-IFreeRTOS/Source/include

OBJ =	inc/version.h							\
	src/app.o							\
	src/can.o							\
	src/newlib_stubs.o						\
	src/uqueue.o							\
	FreeRTOS/Source/tasks.o						\
	FreeRTOS/Source/queue.o						\
	FreeRTOS/Source/list.o						\
	FreeRTOS/Source/portable/MemMang/heap_3.o

ifeq ($(TARGET), F407)
CFLAGS +=	-mthumb -mcpu=cortex-m4					\
		-mfloat-abi=softfp -mfpu=fpv4-sp-d16			\
		-ICMSIS/Device/ST/STM32F4xx/Include			\
		-ICMSIS/Include						\
		-ISTM32F4xx_StdPeriph_Driver/inc			\
		-ISTM32_USB_OTG_Driver/inc				\
		-ISTM32_USB_Device_Library/Core/inc			\
		-ISTM32_USB_Device_Library/Class/cdc/inc		\
		-IFreeRTOS/Source/portable/GCC/ARM_CM4F			\
		-DUSE_STDPERIPH_DRIVER

LDSCRIPT 	= stm32_flash.ld

OBJ +=		src/startup_stm32f4xx.o					\
		src/stm32fxxx_it.o					\
		src/system_stm32f4xx.o					\
		src/usb_bsp.o						\
		src/usbd_cdc_vcp.o					\
		src/usbd_desc.o						\
		src/usbd_usr.o						\
		FreeRTOS/Source/portable/GCC/ARM_CM4F/port.o		\
		STM32F4xx_StdPeriph_Driver/src/stm32f4xx_can.o		\
		STM32_USB_Device_Library/Class/cdc/src/usbd_cdc_core.o	\
		STM32_USB_Device_Library/Core/src/usbd_ioreq.o		\
		STM32_USB_Device_Library/Core/src/usbd_req.o		\
		STM32_USB_Device_Library/Core/src/usbd_core.o		\
		STM32_USB_OTG_Driver/src/usb_core.o			\
		STM32_USB_OTG_Driver/src/usb_dcd.o			\
		STM32_USB_OTG_Driver/src/usb_dcd_int.o			\
		STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.o		\
		STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.o		\
		STM32F4xx_StdPeriph_Driver/src/misc.o			\
		STM32F4xx_StdPeriph_Driver/src/stm32f4xx_exti.o
endif

ifeq ($(TARGET), F091)
CFLAGS +=	-mcpu=cortex-m0 -mthumb					\
		-DSTM32F091						\
		-ICMSIS_F0/Device/ST/STM32F0xx/Include			\
		-ICMSIS_F0/Include					\
		-IFreeRTOS/Source/portable/GCC/ARM_CM0			\
		-ISTM32F0xx_StdPeriph_Driver/inc

LDSCRIPT = 	STM32F091CC_FLASH.ld

OBJ +=		src/startup_stm32f091.o					\
		src/stm32f0xx_it.o					\
		src/system_stm32f0xx.o					\
		FreeRTOS/Source/portable/GCC/ARM_CM0/port.o		\
		STM32F0xx_StdPeriph_Driver/src/stm32f0xx_can.o		\
		STM32F0xx_StdPeriph_Driver/src/stm32f0xx_usart.o	\
		STM32F0xx_StdPeriph_Driver/src/stm32f0xx_rcc.o		\
		STM32F0xx_StdPeriph_Driver/src/stm32f0xx_gpio.o		\
		STM32F0xx_StdPeriph_Driver/src/stm32f0xx_misc.o
endif

all: main.bin

main.bin: main.elf
	$(OBJCOPY) -O binary $< $@
	$(PREFIX)-size $<

main.elf: $(OBJ)
	$(CXX) $(CFLAGS) -o $@ -Wl,-T$(LDSCRIPT) -Wl,-Map=linker.map -Wl,-cref -Wl,--gc-sections $^

clean:
	rm -f $(OBJ) *.elf *.bin

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

src/app.o: inc/version.h src/app.c
	$(CC) $(CFLAGS) -c src/app.c -o $@

load: main.bin
	openocd -f fwload-$(TARGET).openocd

inc/version.h:
	sh -c 'echo "#define __VERSION \"$$(./setlocalversion)\""' > inc/version.h
