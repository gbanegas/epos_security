################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/mach/atmega1281/adc.cc \
../src/mach/atmega1281/adc_test.cc \
../src/mach/atmega1281/at86rf230.cc \
../src/mach/atmega1281/battery.cc \
../src/mach/atmega1281/battery_init.cc \
../src/mach/atmega1281/ic.cc \
../src/mach/atmega1281/ic_test.cc \
../src/mach/atmega1281/machine.cc \
../src/mach/atmega1281/machine_init.cc \
../src/mach/atmega1281/machine_test.cc \
../src/mach/atmega1281/nic_init.cc \
../src/mach/atmega1281/nic_test.cc \
../src/mach/atmega1281/radio.cc \
../src/mach/atmega1281/rtc_test.cc \
../src/mach/atmega1281/sensor_test.cc \
../src/mach/atmega1281/sht11.cc \
../src/mach/atmega1281/timer_test.cc \
../src/mach/atmega1281/uart_test.cc 

OBJS += \
./src/mach/atmega1281/adc.o \
./src/mach/atmega1281/adc_test.o \
./src/mach/atmega1281/at86rf230.o \
./src/mach/atmega1281/battery.o \
./src/mach/atmega1281/battery_init.o \
./src/mach/atmega1281/ic.o \
./src/mach/atmega1281/ic_test.o \
./src/mach/atmega1281/machine.o \
./src/mach/atmega1281/machine_init.o \
./src/mach/atmega1281/machine_test.o \
./src/mach/atmega1281/nic_init.o \
./src/mach/atmega1281/nic_test.o \
./src/mach/atmega1281/radio.o \
./src/mach/atmega1281/rtc_test.o \
./src/mach/atmega1281/sensor_test.o \
./src/mach/atmega1281/sht11.o \
./src/mach/atmega1281/timer_test.o \
./src/mach/atmega1281/uart_test.o 

CC_DEPS += \
./src/mach/atmega1281/adc.d \
./src/mach/atmega1281/adc_test.d \
./src/mach/atmega1281/at86rf230.d \
./src/mach/atmega1281/battery.d \
./src/mach/atmega1281/battery_init.d \
./src/mach/atmega1281/ic.d \
./src/mach/atmega1281/ic_test.d \
./src/mach/atmega1281/machine.d \
./src/mach/atmega1281/machine_init.d \
./src/mach/atmega1281/machine_test.d \
./src/mach/atmega1281/nic_init.d \
./src/mach/atmega1281/nic_test.d \
./src/mach/atmega1281/radio.d \
./src/mach/atmega1281/rtc_test.d \
./src/mach/atmega1281/sensor_test.d \
./src/mach/atmega1281/sht11.d \
./src/mach/atmega1281/timer_test.d \
./src/mach/atmega1281/uart_test.d 


# Each subdirectory must supply rules for building sources it contributes
src/mach/atmega1281/%.o: ../src/mach/atmega1281/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


