################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/MY_CS43L22.c \
../Src/diskio.c \
../Src/display.c \
../Src/ff.c \
../Src/ffsystem.c \
../Src/ffunicode.c \
../Src/main.c \
../Src/music.c \
../Src/stm32f4xx_hal_msp.c \
../Src/stm32f4xx_it.c \
../Src/system_stm32f4xx.c 

OBJS += \
./Src/MY_CS43L22.o \
./Src/diskio.o \
./Src/display.o \
./Src/ff.o \
./Src/ffsystem.o \
./Src/ffunicode.o \
./Src/main.o \
./Src/music.o \
./Src/stm32f4xx_hal_msp.o \
./Src/stm32f4xx_it.o \
./Src/system_stm32f4xx.o 

C_DEPS += \
./Src/MY_CS43L22.d \
./Src/diskio.d \
./Src/display.d \
./Src/ff.d \
./Src/ffsystem.d \
./Src/ffunicode.d \
./Src/main.d \
./Src/music.d \
./Src/stm32f4xx_hal_msp.d \
./Src/stm32f4xx_it.d \
./Src/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F407xx -I"D:/Studia/Semestr 4/PTM/2019_MusicClock/MusicClock/Inc" -I"D:/Studia/Semestr 4/PTM/2019_MusicClock/MusicClock/Drivers/STM32F4xx_HAL_Driver/Inc" -I"D:/Studia/Semestr 4/PTM/2019_MusicClock/MusicClock/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"D:/Studia/Semestr 4/PTM/2019_MusicClock/MusicClock/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"D:/Studia/Semestr 4/PTM/2019_MusicClock/MusicClock/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


