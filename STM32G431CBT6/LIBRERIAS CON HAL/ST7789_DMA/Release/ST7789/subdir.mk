################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ST7789/fonts.c \
../ST7789/st7789.c 

OBJS += \
./ST7789/fonts.o \
./ST7789/st7789.o 

C_DEPS += \
./ST7789/fonts.d \
./ST7789/st7789.d 


# Each subdirectory must supply rules for building sources it contributes
ST7789/%.o ST7789/%.su ST7789/%.cyclo: ../ST7789/%.c ST7789/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32G431xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32G431CBT6/LIBRERIAS CON HAL/ST7789_DMA/ST7789" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-ST7789

clean-ST7789:
	-$(RM) ./ST7789/fonts.cyclo ./ST7789/fonts.d ./ST7789/fonts.o ./ST7789/fonts.su ./ST7789/st7789.cyclo ./ST7789/st7789.d ./ST7789/st7789.o ./ST7789/st7789.su

.PHONY: clean-ST7789

