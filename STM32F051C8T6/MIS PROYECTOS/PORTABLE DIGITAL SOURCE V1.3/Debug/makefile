################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32F0xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := PORTABLE DIGITAL SOURCE V1.3
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
PORTABLE\ DIGITAL\ SOURCE\ V1.3.elf \

MAP_FILES += \
PORTABLE\ DIGITAL\ SOURCE\ V1.3.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
PORTABLE\ DIGITAL\ SOURCE\ V1.3.list \


# All Target
all: main-build

# Main-build Target
main-build: PORTABLE\ DIGITAL\ SOURCE\ V1.3.elf secondary-outputs

# Tool invocations
PORTABLE\ DIGITAL\ SOURCE\ V1.3.elf PORTABLE\ DIGITAL\ SOURCE\ V1.3.map: $(OBJS) $(USER_OBJS) E:\GIOVANNY_TODO\SKETCHS\STM32\STM32F051C8T6\MIS\ PROYECTOS\PORTABLE\ DIGITAL\ SOURCE\ V1.3\STM32F051C8TX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "PORTABLE DIGITAL SOURCE V1.3.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m0 -T"E:\GIOVANNY_TODO\SKETCHS\STM32\STM32F051C8T6\MIS PROYECTOS\PORTABLE DIGITAL SOURCE V1.3\STM32F051C8TX_FLASH.ld" --specs=nosys.specs -Wl,-Map="PORTABLE DIGITAL SOURCE V1.3.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -u _printf_float -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

PORTABLE\ DIGITAL\ SOURCE\ V1.3.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "PORTABLE DIGITAL SOURCE V1.3.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) PORTABLE\ DIGITAL\ SOURCE\ V1.3.elf PORTABLE\ DIGITAL\ SOURCE\ V1.3.list PORTABLE\ DIGITAL\ SOURCE\ V1.3.map PORTABLE\\ DIGITAL\\ SOURCE\\ V1.3.elf default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
