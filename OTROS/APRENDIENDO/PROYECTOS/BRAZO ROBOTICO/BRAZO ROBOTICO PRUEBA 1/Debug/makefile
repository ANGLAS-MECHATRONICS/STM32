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
BRAZO\ ROBOTICO\ PRUEBA\ 1.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
BRAZO\ ROBOTICO\ PRUEBA\ 1.list \

OBJCOPY_BIN += \
BRAZO\ ROBOTICO\ PRUEBA\ 1.bin \


# All Target
all: BRAZO\ ROBOTICO\ PRUEBA\ 1.elf secondary-outputs

# Tool invocations
BRAZO\ ROBOTICO\ PRUEBA\ 1.elf: $(OBJS) $(USER_OBJS) E:\GIOVANNY\ TODO\SKETCHS\STM32\PROYECTOS\BRAZO\ ROBOTICO\BRAZO\ ROBOTICO\ PRUEBA\ 1\STM32F103C8TX_FLASH.ld
	arm-none-eabi-gcc -o "BRAZO ROBOTICO PRUEBA 1.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m3 -T"E:\GIOVANNY TODO\SKETCHS\STM32\PROYECTOS\BRAZO ROBOTICO\BRAZO ROBOTICO PRUEBA 1\STM32F103C8TX_FLASH.ld" --specs=nosys.specs -Wl,-Map="BRAZO ROBOTICO PRUEBA 1.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

BRAZO\ ROBOTICO\ PRUEBA\ 1.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "BRAZO ROBOTICO PRUEBA 1.list"
	@echo 'Finished building: $@'
	@echo ' '

BRAZO\ ROBOTICO\ PRUEBA\ 1.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "BRAZO ROBOTICO PRUEBA 1.bin"
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
