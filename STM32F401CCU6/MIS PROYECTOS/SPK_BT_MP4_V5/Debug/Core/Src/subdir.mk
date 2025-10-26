################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/main.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c 

S_UPPER_SRCS += \
../Core/Src/asm.S 

OBJS += \
./Core/Src/asm.o \
./Core/Src/main.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o 

S_UPPER_DEPS += \
./Core/Src/asm.d 

C_DEPS += \
./Core/Src/main.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o: ../Core/Src/%.S Core/Src/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/Display" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/FATFS" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/PCM5102" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/MP3Helix/include" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/JPEG" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/AVI" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/FileManager" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/Keyboard" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/MicroGL2D" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/Encoder" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/MyString" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/INA226" -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F401xC -DUSE_FULL_LL_DRIVER -DUSE_HAL_DRIVER -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/Display" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/FATFS" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/PCM5102" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/MP3Helix/include" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/JPEG" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/AVI" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/FileManager" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/Keyboard" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/MicroGL2D" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/Encoder" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/MyString" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/INA226" -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/asm.d ./Core/Src/asm.o ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/stm32f4xx_hal_msp.cyclo ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_msp.su ./Core/Src/stm32f4xx_it.cyclo ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/stm32f4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f4xx.cyclo ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su

.PHONY: clean-Core-2f-Src

