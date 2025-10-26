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
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/Display" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/FATFS" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/PCM5102" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/MP3Helix/include" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/JPEG" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/AVI" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/FileManager" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/Keyboard" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/MicroGL2D" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/Encoder" -I"E:/GIOVANNY_TODO/SKETCHS/STM32/STM32F401CCU6/MIS PROYECTOS/VIDEO_PLAYER_BUTTONS/MyString" -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Core-2f-Startup

clean-Core-2f-Startup:
	-$(RM) ./Core/Startup/startup_stm32f401ccux.d ./Core/Startup/startup_stm32f401ccux.o

.PHONY: clean-Core-2f-Startup

