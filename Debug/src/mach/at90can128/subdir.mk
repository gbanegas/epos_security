################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/mach/at90can128/adc.cc \
../src/mach/at90can128/can.cc \
../src/mach/at90can128/can_init.cc \
../src/mach/at90can128/can_test.cc \
../src/mach/at90can128/ic.cc \
../src/mach/at90can128/ic_test.cc \
../src/mach/at90can128/machine.cc \
../src/mach/at90can128/machine_init.cc \
../src/mach/at90can128/machine_test.cc \
../src/mach/at90can128/rtc.cc \
../src/mach/at90can128/rtc_test.cc \
../src/mach/at90can128/uart.cc \
../src/mach/at90can128/uart_test.cc 

OBJS += \
./src/mach/at90can128/adc.o \
./src/mach/at90can128/can.o \
./src/mach/at90can128/can_init.o \
./src/mach/at90can128/can_test.o \
./src/mach/at90can128/ic.o \
./src/mach/at90can128/ic_test.o \
./src/mach/at90can128/machine.o \
./src/mach/at90can128/machine_init.o \
./src/mach/at90can128/machine_test.o \
./src/mach/at90can128/rtc.o \
./src/mach/at90can128/rtc_test.o \
./src/mach/at90can128/uart.o \
./src/mach/at90can128/uart_test.o 

CC_DEPS += \
./src/mach/at90can128/adc.d \
./src/mach/at90can128/can.d \
./src/mach/at90can128/can_init.d \
./src/mach/at90can128/can_test.d \
./src/mach/at90can128/ic.d \
./src/mach/at90can128/ic_test.d \
./src/mach/at90can128/machine.d \
./src/mach/at90can128/machine_init.d \
./src/mach/at90can128/machine_test.d \
./src/mach/at90can128/rtc.d \
./src/mach/at90can128/rtc_test.d \
./src/mach/at90can128/uart.d \
./src/mach/at90can128/uart_test.d 


# Each subdirectory must supply rules for building sources it contributes
src/mach/at90can128/%.o: ../src/mach/at90can128/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


