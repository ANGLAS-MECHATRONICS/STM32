################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include PCM5102/subdir.mk
-include MyString/subdir.mk
-include MicroGL2D/subdir.mk
-include MP3Helix/src/subdir.mk
-include MAX17048/subdir.mk
-include Keyboard/subdir.mk
-include JPEG/subdir.mk
-include INA226/subdir.mk
-include FileManager/subdir.mk
-include FATFS/subdir.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Display/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include AVI/subdir.mk
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


BUILD_ARTIFACT_NAME := SPK_BT_MP4_V5
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
SPK_BT_MP4_V5.elf \

MAP_FILES += \
SPK_BT_MP4_V5.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
SPK_BT_MP4_V5.list \

OBJCOPY_HEX += \
SPK_BT_MP4_V5.hex \

OBJCOPY_BIN += \
SPK_BT_MP4_V5.bin \


# All Target
all:
	+@$(MAKE) --no-print-directory pre-build && $(MAKE) --no-print-directory main-build

# Main-build Target
main-build: SPK_BT_MP4_V5.elf secondary-outputs

# Tool invocations
SPK_BT_MP4_V5.elf SPK_BT_MP4_V5.map: $(OBJS) $(USER_OBJS) E:\GIOVANNY_TODO\SKETCHS\STM32\STM32F401CCU6\MIS\ PROYECTOS\SPK_BT_MP4_V5\STM32F401CCUX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "SPK_BT_MP4_V5.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"E:\GIOVANNY_TODO\SKETCHS\STM32\STM32F401CCU6\MIS PROYECTOS\SPK_BT_MP4_V5\STM32F401CCUX_FLASH.ld" --specs=nosys.specs -Wl,-Map="SPK_BT_MP4_V5.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -u _printf_float -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

SPK_BT_MP4_V5.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "SPK_BT_MP4_V5.list"
	@echo 'Finished building: $@'
	@echo ' '

SPK_BT_MP4_V5.hex: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O ihex $(EXECUTABLES) "SPK_BT_MP4_V5.hex"
	@echo 'Finished building: $@'
	@echo ' '

SPK_BT_MP4_V5.bin: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "SPK_BT_MP4_V5.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) SPK_BT_MP4_V5.bin SPK_BT_MP4_V5.elf SPK_BT_MP4_V5.hex SPK_BT_MP4_V5.list SPK_BT_MP4_V5.map default.size.stdout
	-@echo ' '

pre-build:
	make -f ../Display/makefile
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_HEX) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified pre-build

-include ../makefile.targets
