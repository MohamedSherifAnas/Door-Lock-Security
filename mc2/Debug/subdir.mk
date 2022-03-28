################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Application2.c \
../DC-motor.c \
../PWM.c \
../Timer0.c \
../buzzer.c \
../external_eeprom.c \
../gpio.c \
../twi.c \
../usart.c 

OBJS += \
./Application2.o \
./DC-motor.o \
./PWM.o \
./Timer0.o \
./buzzer.o \
./external_eeprom.o \
./gpio.o \
./twi.o \
./usart.o 

C_DEPS += \
./Application2.d \
./DC-motor.d \
./PWM.d \
./Timer0.d \
./buzzer.d \
./external_eeprom.d \
./gpio.d \
./twi.d \
./usart.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega16 -DF_CPU=1000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


