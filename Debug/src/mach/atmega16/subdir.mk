################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/mach/atmega16/adc.cc \
../src/mach/atmega16/adc_test.cc \
../src/mach/atmega16/ic.cc \
../src/mach/atmega16/ic_test.cc \
../src/mach/atmega16/machine.cc \
../src/mach/atmega16/machine_init.cc \
../src/mach/atmega16/machine_test.cc \
../src/mach/atmega16/nic_init.cc \
../src/mach/atmega16/nic_test.cc \
../src/mach/atmega16/radio.cc \
../src/mach/atmega16/radio_init.cc \
../src/mach/atmega16/rtc_test.cc \
../src/mach/atmega16/timer_test.cc \
../src/mach/atmega16/uart_test.cc 

OBJS += \
./src/mach/atmega16/adc.o \
./src/mach/atmega16/adc_test.o \
./src/mach/atmega16/ic.o \
./src/mach/atmega16/ic_test.o \
./src/mach/atmega16/machine.o \
./src/mach/atmega16/machine_init.o \
./src/mach/atmega16/machine_test.o \
./src/mach/atmega16/nic_init.o \
./src/mach/atmega16/nic_test.o \
./src/mach/atmega16/radio.o \
./src/mach/atmega16/radio_init.o \
./src/mach/atmega16/rtc_test.o \
./src/mach/atmega16/timer_test.o \
./src/mach/atmega16/uart_test.o 

CC_DEPS += \
./src/mach/atmega16/adc.d \
./src/mach/atmega16/adc_test.d \
./src/mach/atmega16/ic.d \
./src/mach/atmega16/ic_test.d \
./src/mach/atmega16/machine.d \
./src/mach/atmega16/machine_init.d \
./src/mach/atmega16/machine_test.d \
./src/mach/atmega16/nic_init.d \
./src/mach/atmega16/nic_test.d \
./src/mach/atmega16/radio.d \
./src/mach/atmega16/radio_init.d \
./src/mach/atmega16/rtc_test.d \
./src/mach/atmega16/timer_test.d \
./src/mach/atmega16/uart_test.d 


# Each subdirectory must supply rules for building sources it contributes
src/mach/atmega16/%.o: ../src/mach/atmega16/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


