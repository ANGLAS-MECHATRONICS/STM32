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
PRINTF\ SPRINTF\ USART1\ BOOL\ BIEN.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
PRINTF\ SPRINTF\ USART1\ BOOL\ BIEN.list \

OBJCOPY_BIN += \
PRINTF\ SPRINTF\ USART1\ BOOL\ BIEN.bin \


# All Target
all: PRINTF\ SPRINTF\ USART1\ BOOL\ BIEN.elf secondary-outputs

# Tool invocations
PRINTF\ SPRINTF\ USART1\ BOOL\ BIEN.elf: $(OBJS) $(USER_OBJS) E:\GIOVANNY\ TODO\SKETCHS\STM32\PRINTF\ SPRINTF\ USART1\ BOOL\ BIEN\STM32F103C8TX_FLASH.ld
	arm-none-eabi-gcc -o "PRINTF SPRINTF USART1 BOOL BIEN.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m3 -T"E:\GIOVANNY TODO\SKETCHS\STM32\PRINTF SPRINTF USART1 BOOL BIEN\STM32F103C8TX_FLASH.ld" --specs=nosys.specs -Wl,-Map="PRINTF SPRINTF USART1 BOOL BIEN.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

PRINTF\ SPRINTF\ USART1\ BOOL\ BIEN.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "PRINTF SPRINTF USART1 BOOL BIEN.list"
	@echo 'Finished building: $@'
	@echo ' '

PRINTF\ SPRINTF\ USART1\ BOOL\ BIEN.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "PRINTF SPRINTF USART1 BOOL BIEN.bin"
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
