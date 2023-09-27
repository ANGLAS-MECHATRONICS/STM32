################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32F1xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
PUERTO\ SERIE\ UART.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
PUERTO\ SERIE\ UART.list \

OBJCOPY_BIN += \
PUERTO\ SERIE\ UART.bin \


# All Target
all: PUERTO\ SERIE\ UART.elf secondary-outputs

# Tool invocations
PUERTO\ SERIE\ UART.elf: $(OBJS) $(USER_OBJS) E:\GIOVANNY\ TODO\SKETCHS\STM32\PUERTO\ SERIE\ UART\STM32F103C8TX_FLASH.ld
	arm-none-eabi-gcc -o "PUERTO SERIE UART.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m3 -T"E:\GIOVANNY TODO\SKETCHS\STM32\PUERTO SERIE UART\STM32F103C8TX_FLASH.ld" --specs=nosys.specs -Wl,-Map="PUERTO SERIE UART.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

PUERTO\ SERIE\ UART.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "PUERTO SERIE UART.list"
	@echo 'Finished building: $@'
	@echo ' '

PUERTO\ SERIE\ UART.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "PUERTO SERIE UART.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
