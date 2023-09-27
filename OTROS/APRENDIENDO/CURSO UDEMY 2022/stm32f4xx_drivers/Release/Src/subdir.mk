################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/main.c 

OBJS += \
./Src/main.o 

C_DEPS += \
./Src/main.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DSTM32 -DSTM32F4 -DSTM32F446RETx -c -I../Inc -I"E:/GIOVANNY_TODO/SKETCHS/STM32/CURSO UDEMY 2022/stm32f4xx_drivers" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/CURSO UDEMY 2022/stm32f4xx_drivers/Drivers/Inc" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/CURSO UDEMY 2022/stm32f4xx_drivers/Drivers/Src" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/main.d ./Src/main.o ./Src/main.su

.PHONY: clean-Src

