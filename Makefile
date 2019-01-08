TARGET = Output


######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization Can be: -Og -Os -Ofast  -On (n can be: 0~3)
OPT = -Og

#######################################
# paths
#######################################
# source path
SOURCES_DIR =  \
CORE \
FWLIB/src \
HARDWARE/SDIO \
HARDWARE/PWM \
SYSTEM/delay \
SYSTEM/sys \
SYSTEM/usart \
FATFS \
USER \

# firmware library path
PERIFLIB_PATH =

# Build path
BUILD_DIR = OBJ

######################################
# source
######################################
# C sources
C_SOURCES =  \
$(foreach n,$(SOURCES_DIR) , $(wildcard $(n)/*.c))

# ASM sources
ASM_SOURCES =  \
USER/startup_stm32f407xx.s


######################################
# firmware library
######################################
PERIFLIB_SOURCES =


#######################################
# binaries
#######################################
BINPATH = 
PREFIX = arm-none-eabi-
CC = $(BINPATH)/$(PREFIX)gcc
AS = $(BINPATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(BINPATH)/$(PREFIX)objcopy
AR = $(BINPATH)/$(PREFIX)ar
SZ = $(BINPATH)/$(PREFIX)size
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m4

# fpu
FPU = -mfpu=fpv4-sp-d16

# float-abi
FLOAT-ABI = -mfloat-abi=hard

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS =

# C defines
C_DEFS =  \
-DUSE_STDPERIPH_DRIVER \
-DSTM32F40_41xxx \
-D__FPU_USED \
-D__FPU_PRESENT

# AS includes
AS_INCLUDES =

# C includes
C_INCLUDES =  \
-ICORE \
-IFWLIB/inc \
-IHARDWARE/PWM \
-IHARDWARE/SDIO \
-ISYSTEM/delay \
-ISYSTEM/sys \
-ISYSTEM/usart \
-IFATFS \
-IUSER \

# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif


# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"


#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = Link/STM32F407VETx_FLASH.ld

# libraries PS.if wanna support printf float, you will add "-u _printf_float" after "nano.specs"
LIBS = -lc -lm -lnosys
LIBDIR =
LDFLAGS = $(MCU) -specs=nano.specs -u _printf_float -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@

$(BUILD_DIR):
	mkdir $@

#######################################
# Updata flash
#######################################
erase:
	@echo "正在格式化flash......"
	st-flash erase

updata:
	st-flash write $(BUILD_DIR)/$(TARGET).bin 0x08000000
	@echo "刷入成功!!!"

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)/*.d
	-rm -fR $(BUILD_DIR)/*.lst
	-rm -fR $(BUILD_DIR)/*.o

delete:
	-rm -fR $(BUILD_DIR)/*

#######################################
# dependencies
#######################################
# -include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

# *** EOF ***
