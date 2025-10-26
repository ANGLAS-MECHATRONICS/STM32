################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../FileManager/filemanager.c \
../FileManager/icons.c 

OBJS += \
./FileManager/filemanager.o \
./FileManager/icons.o 

C_DEPS += \
./FileManager/filemanager.d \
./FileManager/icons.d 


# Each subdirectory must supply rules for building sources it contributes
FileManager/%.o FileManager/%.su FileManager/%.cyclo: ../FileManager/%.c FileManager/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F401xC -DUSE_FULL_LL_DRIVER -DHSE_VALUE=25000000 -DHSE_STARTUP_TIMEOUT=100 -DLSE_STARTUP_TIMEOUT=5000 -DLSE_VALUE=32768 -DEXTERNAL_CLOCK_VALUE=12288000 -DHSI_VALUE=16000000 -DLSI_VALUE=32000 -DVDD_VALUE=3400 -DPREFETCH_ENABLE=1 -DINSTRUCTION_CACHE_ENABLE=1 -DDATA_CACHE_ENABLE=1 -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/Display" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/FATFS" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/PCM5102" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/MP3Helix/include" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/JPEG" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/AVI" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/FileManager" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/Keyboard" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/MicroGL2D" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/Encoder" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/MyString" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-FileManager

clean-FileManager:
	-$(RM) ./FileManager/filemanager.cyclo ./FileManager/filemanager.d ./FileManager/filemanager.o ./FileManager/filemanager.su ./FileManager/icons.cyclo ./FileManager/icons.d ./FileManager/icons.o ./FileManager/icons.su

.PHONY: clean-FileManager

