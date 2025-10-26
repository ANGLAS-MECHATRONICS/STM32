################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../JPEG/jpeg_chan.c \
../JPEG/tjpgd.c 

OBJS += \
./JPEG/jpeg_chan.o \
./JPEG/tjpgd.o 

C_DEPS += \
./JPEG/jpeg_chan.d \
./JPEG/tjpgd.d 


# Each subdirectory must supply rules for building sources it contributes
JPEG/%.o JPEG/%.su JPEG/%.cyclo: ../JPEG/%.c JPEG/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F401xC -DUSE_FULL_LL_DRIVER -DUSE_HAL_DRIVER -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/Display" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/FATFS" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/PCM5102" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/MP3Helix/include" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/JPEG" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/AVI" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/FileManager" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/Keyboard" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/MicroGL2D" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/Encoder" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/MyString" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/INA226" -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-JPEG

clean-JPEG:
	-$(RM) ./JPEG/jpeg_chan.cyclo ./JPEG/jpeg_chan.d ./JPEG/jpeg_chan.o ./JPEG/jpeg_chan.su ./JPEG/tjpgd.cyclo ./JPEG/tjpgd.d ./JPEG/tjpgd.o ./JPEG/tjpgd.su

.PHONY: clean-JPEG

