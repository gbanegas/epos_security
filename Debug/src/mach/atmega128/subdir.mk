################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/mach/atmega128/adc.cc \
../src/mach/atmega128/adc_test.cc \
../src/mach/atmega128/cc1000.cc \
../src/mach/atmega128/cmac128.cc \
../src/mach/atmega128/cmac128_init.cc \
../src/mach/atmega128/ic.cc \
../src/mach/atmega128/ic_test.cc \
../src/mach/atmega128/machine.cc \
../src/mach/atmega128/machine_init.cc \
../src/mach/atmega128/machine_test.cc \
../src/mach/atmega128/mts300.cc \
../src/mach/atmega128/nic_init.cc \
../src/mach/atmega128/nic_test.cc \
../src/mach/atmega128/radio.cc \
../src/mach/atmega128/radio_init.cc \
../src/mach/atmega128/rtc_test.cc \
../src/mach/atmega128/sensor_test.cc \
../src/mach/atmega128/timer_test.cc \
../src/mach/atmega128/uart_test.cc 

OBJS += \
./src/mach/atmega128/adc.o \
./src/mach/atmega128/adc_test.o \
./src/mach/atmega128/cc1000.o \
./src/mach/atmega128/cmac128.o \
./src/mach/atmega128/cmac128_init.o \
./src/mach/atmega128/ic.o \
./src/mach/atmega128/ic_test.o \
./src/mach/atmega128/machine.o \
./src/mach/atmega128/machine_init.o \
./src/mach/atmega128/machine_test.o \
./src/mach/atmega128/mts300.o \
./src/mach/atmega128/nic_init.o \
./src/mach/atmega128/nic_test.o \
./src/mach/atmega128/radio.o \
./src/mach/atmega128/radio_init.o \
./src/mach/atmega128/rtc_test.o \
./src/mach/atmega128/sensor_test.o \
./src/mach/atmega128/timer_test.o \
./src/mach/atmega128/uart_test.o 

CC_DEPS += \
./src/mach/atmega128/adc.d \
./src/mach/atmega128/adc_test.d \
./src/mach/atmega128/cc1000.d \
./src/mach/atmega128/cmac128.d \
./src/mach/atmega128/cmac128_init.d \
./src/mach/atmega128/ic.d \
./src/mach/atmega128/ic_test.d \
./src/mach/atmega128/machine.d \
./src/mach/atmega128/machine_init.d \
./src/mach/atmega128/machine_test.d \
./src/mach/atmega128/mts300.d \
./src/mach/atmega128/nic_init.d \
./src/mach/atmega128/nic_test.d \
./src/mach/atmega128/radio.d \
./src/mach/atmega128/radio_init.d \
./src/mach/atmega128/rtc_test.d \
./src/mach/atmega128/sensor_test.d \
./src/mach/atmega128/timer_test.d \
./src/mach/atmega128/uart_test.d 


# Each subdirectory must supply rules for building sources it contributes
src/mach/atmega128/%.o: ../src/mach/atmega128/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


