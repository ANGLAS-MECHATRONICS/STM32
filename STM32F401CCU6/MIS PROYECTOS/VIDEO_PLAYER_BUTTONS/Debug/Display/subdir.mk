################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Display/display.c \
../Display/display_offsets.c \
../Display/fonts.c \
../Display/ili9341.c \
../Display/st7789.c 

S_UPPER_SRCS += \
../Display/display_asm.S 

OBJS += \
./Display/display.o \
./Display/display_asm.o \
./Display/display_offsets.o \
./Display/fonts.o \
./Display/ili9341.o \
./Display/st7789.o 

S_UPPER_DEPS += \
./Display/display_asm.d 

C_DEPS += \
./Display/display.d \
./Display/display_offsets.d \
./Display/fonts.d \
./Display/ili9341.d \
./Display/st7789.d 


# Each subdirectory must supply rules for building sources it contributes
Display/%.o Display/%.su Display/%.cyclo: ../Display/%.c Display/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F401xC -DUSE_FULL_LL_DRIVER -DHSE_VALUE=25000000 -DHSE_STARTUP_TIMEOUT=100 -DLSE_STARTUP_TIMEOUT=5000 -DLSE_VALUE=32768 -DEXTERNAL_CLOCK_VALUE=12288000 -DHSI_VALUE=16000000 -DLSI_VALUE=32000 -DVDD_VALUE=3400 -DPREFETCH_ENABLE=1 -DINSTRUCTION_CACHE_ENABLE=1 -DDATA_CACHE_ENABLE=1 -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/Display" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/FATFS" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/PCM5102" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/MP3Helix/include" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/JPEG" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/AVI" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/FileManager" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/Keyboard" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/MicroGL2D" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/Encoder" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/MyString" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Display/%.o: ../Display/%.S Display/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/Display" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/FATFS" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/PCM5102" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/MP3Helix/include" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/JPEG" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/AVI" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/FileManager" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/Keyboard" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/MicroGL2D" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/Encoder" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/MyString" -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Display

clean-Display:
	-$(RM) ./Display/display.cyclo ./Display/display.d ./Display/display.o ./Display/display.su ./Display/display_asm.d ./Display/display_asm.o ./Display/display_offsets.cyclo ./Display/display_offsets.d ./Display/display_offsets.o ./Display/display_offsets.su ./Display/fonts.cyclo ./Display/fonts.d ./Display/fonts.o ./Display/fonts.su ./Display/ili9341.cyclo ./Display/ili9341.d ./Display/ili9341.o ./Display/ili9341.su ./Display/st7789.cyclo ./Display/st7789.d ./Display/st7789.o ./Display/st7789.su

.PHONY: clean-Display

