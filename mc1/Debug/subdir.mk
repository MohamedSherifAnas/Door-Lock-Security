################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Application1.c \
../KeyPad.c \
../Timer0.c \
../gpio.c \
../lcd.c \
../usart.c 

OBJS += \
./Application1.o \
./KeyPad.o \
./Timer0.o \
./gpio.o \
./lcd.o \
./usart.o 

C_DEPS += \
./Application1.d \
./KeyPad.d \
./Timer0.d \
./gpio.d \
./lcd.d \
./usart.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega16 -DF_CPU=1000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


