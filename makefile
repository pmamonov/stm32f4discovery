PREFIX=arm-none-eabi
AS      = $(PREFIX)-as
CC      = $(PREFIX)-gcc
CXX      = $(PREFIX)-g++
LD      = $(PREFIX)-ld
OBJCOPY = $(PREFIX)-objcopy

LDSCRIPT = stm32_flash.ld

CFLAGS = -mcpu=cortex-m4 -mthumb -mfloat-abi=softfp -Os -g\
 -I./inc\
 -ICMSIS/Device/ST/STM32F4xx/Include\
 -ICMSIS/Include\
 -ISTM32F4xx_StdPeriph_Driver/inc\
 -ISTM32_USB_OTG_Driver/inc\
 -ISTM32_USB_Device_Library/Core/inc\
 -ISTM32_USB_Device_Library/Class/cdc/inc\
 -DUSE_STDPERIPH_DRIVER\
 -IFreeRTOS/Source/include\
 -IFreeRTOS/Source/portable/GCC/ARM_CM4F


OBJ = src/startup_stm32f4xx.o\
  src/stm32fxxx_it.o\
  src/system_stm32f4xx.o\
  src/app.o\
  src/usb_bsp.o\
  src/usbd_cdc_vcp.o\
  src/usbd_desc.o\
  src/usbd_usr.o\
  src/newlib_stubs.o\
  FreeRTOS/Source/tasks.o\
  FreeRTOS/Source/queue.o\
  FreeRTOS/Source/list.o\
  FreeRTOS/Source/portable/GCC/ARM_CM4F/port.o\
  FreeRTOS/Source/portable/MemMang/heap_3.o\
  STM32_USB_Device_Library/Class/cdc/src/usbd_cdc_core.o\
  STM32_USB_Device_Library/Core/src/usbd_ioreq.o\
  STM32_USB_Device_Library/Core/src/usbd_req.o\
  STM32_USB_Device_Library/Core/src/usbd_core.o\
  STM32_USB_OTG_Driver/src/usb_core.o\
  STM32_USB_OTG_Driver/src/usb_dcd.o\
  STM32_USB_OTG_Driver/src/usb_dcd_int.o\
  STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.o\
  STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.o\
  STM32F4xx_StdPeriph_Driver/src/misc.o\
	STM32F4xx_StdPeriph_Driver/src/stm32f4xx_exti.o


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

