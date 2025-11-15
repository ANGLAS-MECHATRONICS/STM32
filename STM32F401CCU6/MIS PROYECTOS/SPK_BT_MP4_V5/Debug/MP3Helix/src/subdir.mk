################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../MP3Helix/src/asmpoly_thumb2.s 

C_SRCS += \
../MP3Helix/src/bitstream.c \
../MP3Helix/src/buffers.c \
../MP3Helix/src/dct32.c \
../MP3Helix/src/dequant.c \
../MP3Helix/src/dqchan.c \
../MP3Helix/src/huffman.c \
../MP3Helix/src/hufftabs.c \
../MP3Helix/src/imdct.c \
../MP3Helix/src/mp3dec.c \
../MP3Helix/src/mp3tabs.c \
../MP3Helix/src/scalfact.c \
../MP3Helix/src/stproc.c \
../MP3Helix/src/subband.c \
../MP3Helix/src/trigtabs.c 

OBJS += \
./MP3Helix/src/asmpoly_thumb2.o \
./MP3Helix/src/bitstream.o \
./MP3Helix/src/buffers.o \
./MP3Helix/src/dct32.o \
./MP3Helix/src/dequant.o \
./MP3Helix/src/dqchan.o \
./MP3Helix/src/huffman.o \
./MP3Helix/src/hufftabs.o \
./MP3Helix/src/imdct.o \
./MP3Helix/src/mp3dec.o \
./MP3Helix/src/mp3tabs.o \
./MP3Helix/src/scalfact.o \
./MP3Helix/src/stproc.o \
./MP3Helix/src/subband.o \
./MP3Helix/src/trigtabs.o 

S_DEPS += \
./MP3Helix/src/asmpoly_thumb2.d 

C_DEPS += \
./MP3Helix/src/bitstream.d \
./MP3Helix/src/buffers.d \
./MP3Helix/src/dct32.d \
./MP3Helix/src/dequant.d \
./MP3Helix/src/dqchan.d \
./MP3Helix/src/huffman.d \
./MP3Helix/src/hufftabs.d \
./MP3Helix/src/imdct.d \
./MP3Helix/src/mp3dec.d \
./MP3Helix/src/mp3tabs.d \
./MP3Helix/src/scalfact.d \
./MP3Helix/src/stproc.d \
./MP3Helix/src/subband.d \
./MP3Helix/src/trigtabs.d 


# Each subdirectory must supply rules for building sources it contributes
MP3Helix/src/%.o: ../MP3Helix/src/%.s MP3Helix/src/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/Display" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/FATFS" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/PCM5102" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/MP3Helix/include" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/JPEG" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/AVI" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/FileManager" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/Keyboard" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/MicroGL2D" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/MyString" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/INA226" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/MAX17048" -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"
MP3Helix/src/%.o MP3Helix/src/%.su MP3Helix/src/%.cyclo: ../MP3Helix/src/%.c MP3Helix/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F401xC -DUSE_FULL_LL_DRIVER -DUSE_HAL_DRIVER -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/Display" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/FATFS" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/PCM5102" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/MP3Helix/include" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/JPEG" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/AVI" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/FileManager" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/Keyboard" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/MicroGL2D" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/MyString" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/INA226" -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/MAX17048" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-MP3Helix-2f-src

clean-MP3Helix-2f-src:
	-$(RM) ./MP3Helix/src/asmpoly_thumb2.d ./MP3Helix/src/asmpoly_thumb2.o ./MP3Helix/src/bitstream.cyclo ./MP3Helix/src/bitstream.d ./MP3Helix/src/bitstream.o ./MP3Helix/src/bitstream.su ./MP3Helix/src/buffers.cyclo ./MP3Helix/src/buffers.d ./MP3Helix/src/buffers.o ./MP3Helix/src/buffers.su ./MP3Helix/src/dct32.cyclo ./MP3Helix/src/dct32.d ./MP3Helix/src/dct32.o ./MP3Helix/src/dct32.su ./MP3Helix/src/dequant.cyclo ./MP3Helix/src/dequant.d ./MP3Helix/src/dequant.o ./MP3Helix/src/dequant.su ./MP3Helix/src/dqchan.cyclo ./MP3Helix/src/dqchan.d ./MP3Helix/src/dqchan.o ./MP3Helix/src/dqchan.su ./MP3Helix/src/huffman.cyclo ./MP3Helix/src/huffman.d ./MP3Helix/src/huffman.o ./MP3Helix/src/huffman.su ./MP3Helix/src/hufftabs.cyclo ./MP3Helix/src/hufftabs.d ./MP3Helix/src/hufftabs.o ./MP3Helix/src/hufftabs.su ./MP3Helix/src/imdct.cyclo ./MP3Helix/src/imdct.d ./MP3Helix/src/imdct.o ./MP3Helix/src/imdct.su ./MP3Helix/src/mp3dec.cyclo ./MP3Helix/src/mp3dec.d ./MP3Helix/src/mp3dec.o ./MP3Helix/src/mp3dec.su ./MP3Helix/src/mp3tabs.cyclo ./MP3Helix/src/mp3tabs.d ./MP3Helix/src/mp3tabs.o ./MP3Helix/src/mp3tabs.su ./MP3Helix/src/scalfact.cyclo ./MP3Helix/src/scalfact.d ./MP3Helix/src/scalfact.o ./MP3Helix/src/scalfact.su ./MP3Helix/src/stproc.cyclo ./MP3Helix/src/stproc.d ./MP3Helix/src/stproc.o ./MP3Helix/src/stproc.su ./MP3Helix/src/subband.cyclo ./MP3Helix/src/subband.d ./MP3Helix/src/subband.o ./MP3Helix/src/subband.su ./MP3Helix/src/trigtabs.cyclo ./MP3Helix/src/trigtabs.d ./MP3Helix/src/trigtabs.o ./MP3Helix/src/trigtabs.su

.PHONY: clean-MP3Helix-2f-src

