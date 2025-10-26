################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Core/Startup/startup_stm32f401ccux.s 

OBJS += \
./Core/Startup/startup_stm32f401ccux.o 

S_DEPS += \
./Core/Startup/startup_stm32f401ccux.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Startup/%.o: ../Core/Startup/%.s Core/Startup/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -c -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/Display" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/FATFS" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/PCM5102" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/MP3Helix/include" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/JPEG" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/AVI" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/FileManager" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/Keyboard" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/MicroGL2D" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/Encoder" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/SPK_BT_MP4_V5/MyString" -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Core-2f-Startup

clean-Core-2f-Startup:
	-$(RM) ./Core/Startup/startup_stm32f401ccux.d ./Core/Startup/startup_stm32f401ccux.o

.PHONY: clean-Core-2f-Startup

