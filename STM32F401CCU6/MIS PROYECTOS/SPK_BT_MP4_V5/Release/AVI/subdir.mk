################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../AVI/avi.c \
../AVI/jpeg_data.c \
../AVI/mp3_callback.c \
../AVI/player.c \
../AVI/riff_read_header.c \
../AVI/textures.c 

OBJS += \
./AVI/avi.o \
./AVI/jpeg_data.o \
./AVI/mp3_callback.o \
./AVI/player.o \
./AVI/riff_read_header.o \
./AVI/textures.o 

C_DEPS += \
./AVI/avi.d \
./AVI/jpeg_data.d \
./AVI/mp3_callback.d \
./AVI/player.d \
./AVI/riff_read_header.d \
./AVI/textures.d 


# Each subdirectory must supply rules for building sources it contributes
AVI/%.o AVI/%.su AVI/%.cyclo: ../AVI/%.c AVI/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DSTM32F401xC -DUSE_FULL_LL_DRIVER -DUSE_HAL_DRIVER -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/Display" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/FATFS" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/PCM5102" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/MP3Helix/include" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/JPEG" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/AVI" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/FileManager" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/Keyboard" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/MicroGL2D" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/MyString" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/INA226" -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/MAX17048" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-AVI

clean-AVI:
	-$(RM) ./AVI/avi.cyclo ./AVI/avi.d ./AVI/avi.o ./AVI/avi.su ./AVI/jpeg_data.cyclo ./AVI/jpeg_data.d ./AVI/jpeg_data.o ./AVI/jpeg_data.su ./AVI/mp3_callback.cyclo ./AVI/mp3_callback.d ./AVI/mp3_callback.o ./AVI/mp3_callback.su ./AVI/player.cyclo ./AVI/player.d ./AVI/player.o ./AVI/player.su ./AVI/riff_read_header.cyclo ./AVI/riff_read_header.d ./AVI/riff_read_header.o ./AVI/riff_read_header.su ./AVI/textures.cyclo ./AVI/textures.d ./AVI/textures.o ./AVI/textures.su

.PHONY: clean-AVI

