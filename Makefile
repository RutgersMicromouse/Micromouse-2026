CC = arm-none-eabi-gcc

CFLAGS = -mcpu=cortex-m0plus -mthumb -O2 -Wall -Wextra -Werror -g -DSTM32G070xx \
		 -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage \
		 --specs=nano.specs -mfloat-abi=soft

LDFLAGS = -T chip_headers/Projects/NUCLEO-G070RB/Templates_LL/STM32CubeIDE/STM32G070RBTX_FLASH.ld -Wl,--gc-sections
INCLUDE = -I chip_headers/Drivers/CMSIS/Device/ST/STM32G0xx/Include \
		  -I chip_headers/Drivers/CMSIS/Include \
		  -I Inc

STARTUP = chip_headers/Drivers/CMSIS/Device/ST/STM32G0xx/Source/Templates/gcc/startup_stm32g070xx.s
SYSTEM = chip_headers/Drivers/CMSIS/Device/ST/STM32G0xx/Source/Templates/system_stm32g0xx.c

OBJS = main.o syscalls.o sysmem.o startup.o system.o i2c.o uart.o
SUS = syscalls.su sysmem.su system.su main.su i2c.su uart.su
TARGET = micromouse.elf

.PHONY: all clean flash

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

main.o: Src/main.c
	$(CC) $(CFLAGS) $(INCLUDE) -c Src/main.c -o main.o
syscalls.o: Src/syscalls.c
	$(CC) $(CFLAGS) $(INCLUDE) -c Src/syscalls.c -o syscalls.o
sysmem.o: Src/sysmem.c
	$(CC) $(CFLAGS) $(INCLUDE) -c Src/sysmem.c -o sysmem.o
i2c.o: Src/driver/i2c.c
	$(CC) $(CFLAGS) $(INCLUDE) -c Src/driver/i2c.c -o i2c.o
uart.o: Src/driver/uart.c
	$(CC) $(CFLAGS) $(INCLUDE) -c Src/driver/uart.c -o uart.o
startup.o: $(STARTUP)
	$(CC) $(CFLAGS) -c $(STARTUP) -o startup.o

system.o: $(SYSTEM)
	$(CC) $(CFLAGS) $(INCLUDE) -c $(SYSTEM) -o system.o

clean:
	rm -f $(OBJS) $(TARGET) $(SUS)

flash: $(TARGET)
	openocd -f interface/stlink.cfg -f target/stm32g0x.cfg -c "program $(TARGET) verify reset exit"
