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

SRC_DIR = Src
BUILD_DIR = build

# Automatically find all .c files in Src directory (recursively)
SRCS = $(shell find $(SRC_DIR) -name '*.c')

# New version: preserves subpaths (e.g., Src/driver/i2c.c -> build/Src/driver/i2c.o)
OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o) # $(BUILD_DIR)/startup.o $(BUILD_DIR)/system.o

# Generate .su file names
SUS = $(addprefix $(BUILD_DIR)/,$(notdir $(SRCS:.c=.su))) $(BUILD_DIR)/system.su
TARGET = micromouse.elf

.PHONY: all clean flash

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)


# Pattern rules for compiling .c files from nested directories
$(BUILD_DIR)/$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
$(BUILD_DIR)/startup.o: $(STARTUP)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $(STARTUP) -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

flash: $(TARGET)
	openocd -f interface/stlink.cfg -f target/stm32g0x.cfg -c "program $(TARGET) verify reset exit"
