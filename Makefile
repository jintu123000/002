#
# Makefile for STM32F103C8T6 + FreeRTOS LED Blink
#
# 双平台支持:
#   - GCC (arm-none-eabi-gcc): make / make clean / make flash
#   - Keil MDK-5: 打开 Project/firmware.uvprojx
#

# Toolchain
CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SZ      = arm-none-eabi-size

# Project name
TARGET  = firmware

# Directories
SRCDIR   = Src
INCDIR   = Inc
FRTDIR   = FreeRTOS/include
BUILDDIR = build

# Source files
C_SOURCES  = $(SRCDIR)/main.c
C_SOURCES += $(SRCDIR)/system_stm32f1xx.c
C_SOURCES += $(SRCDIR)/FreeRTOS/tasks.c
C_SOURCES += $(SRCDIR)/FreeRTOS/list.c
C_SOURCES += $(SRCDIR)/FreeRTOS/queue.c
C_SOURCES += $(SRCDIR)/FreeRTOS/portable/GCC/ARM_CM3/port.c

ASM_SOURCES = $(SRCDIR)/startup_stm32f103c8tx.s

# Include directories
INCLUDES  = -I$(INCDIR)
INCLUDES += -I$(FRTDIR)

# Compiler flags
CFLAGS  = -mcpu=cortex-m3
CFLAGS += -mthumb
CFLAGS += -O2
CFLAGS += -g3
CFLAGS += -Wall -Wextra
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -fno-common
CFLAGS += -nostdlib
CFLAGS += --specs=nosys.specs
CFLAGS += -DUSE_STDPERIPH_DRIVER
CFLAGS += -DSTM32F103xB
CFLAGS += $(INCLUDES)

# Linker flags
LDFLAGS  = -mcpu=cortex-m3
LDFLAGS += -mthumb
LDFLAGS += --specs=nosys.specs
LDFLAGS += -T STM32F103C8Tx_FLASH.ld
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,-Map=$(BUILDDIR)/$(TARGET).map,--cref
LDFLAGS += -nostartfiles
LDFLAGS += -lm -lc -lgcc

# Object files
C_OBJS   = $(addprefix $(BUILDDIR)/,$(notdir $(C_SOURCES:.c=.o)))
ASM_OBJS = $(addprefix $(BUILDDIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
OBJS     = $(ASM_OBJS) $(C_OBJS)

# Ensure build directory exists
$(shell mkdir -p $(BUILDDIR))

# Build rules
.PHONY: all clean flash

all: $(BUILDDIR)/$(TARGET).elf $(BUILDDIR)/$(TARGET).hex $(BUILDDIR)/$(TARGET).bin

$(BUILDDIR)/$(TARGET).elf: $(OBJS)
	@echo "LD    $@"
	@$(CC) $(LDFLAGS) -o $@ $^
	@$(SZ) $@

$(BUILDDIR)/%.hex: $(BUILDDIR)/$(TARGET).elf
	@echo "HEX   $@"
	@$(OBJCOPY) -O ihex $< $@

$(BUILDDIR)/%.bin: $(BUILDDIR)/$(TARGET).elf
	@echo "BIN   $@"
	@$(OBJCOPY) -O binary $< $@

# C sources -> objects
$(BUILDDIR)/main.o: $(SRCDIR)/main.c
	@echo "CC    $<"
	@$(CC) $(CFLAGS) -c -o $@ $<

$(BUILDDIR)/system_stm32f1xx.o: $(SRCDIR)/system_stm32f1xx.c
	@echo "CC    $<"
	@$(CC) $(CFLAGS) -c -o $@ $<

$(BUILDDIR)/tasks.o: $(SRCDIR)/FreeRTOS/tasks.c
	@echo "CC    $<"
	@$(CC) $(CFLAGS) -c -o $@ $<

$(BUILDDIR)/list.o: $(SRCDIR)/FreeRTOS/list.c
	@echo "CC    $<"
	@$(CC) $(CFLAGS) -c -o $@ $<

$(BUILDDIR)/queue.o: $(SRCDIR)/FreeRTOS/queue.c
	@echo "CC    $<"
	@$(CC) $(CFLAGS) -c -o $@ $<

$(BUILDDIR)/port.o: $(SRCDIR)/FreeRTOS/portable/GCC/ARM_CM3/port.c
	@echo "CC    $<"
	@$(CC) $(CFLAGS) -c -o $@ $<

# Assembly sources
$(BUILDDIR)/startup_stm32f103c8tx.o: $(SRCDIR)/startup_stm32f103c8tx.s
	@echo "AS    $<"
	@$(CC) $(CFLAGS) -x assembler-with-cpp -c -o $@ $<

clean:
	@rm -rf $(BUILDDIR)

flash: $(BUILDDIR)/$(TARGET).bin
	@echo "Flashing... (connect your ST-Link / serial bootloader)"
	@echo "Use: st-flash write $(BUILDDIR)/$(TARGET).bin 0x08000000"
