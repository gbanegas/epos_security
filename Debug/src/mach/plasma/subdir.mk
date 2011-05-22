################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/mach/plasma/ic.cc \
../src/mach/plasma/ic_init.cc \
../src/mach/plasma/machine.cc \
../src/mach/plasma/machine_init.cc \
../src/mach/plasma/nic.cc \
../src/mach/plasma/nic_init.cc \
../src/mach/plasma/plasma_interrupt.cc \
../src/mach/plasma/timer.cc \
../src/mach/plasma/timer_init.cc \
../src/mach/plasma/timer_test.cc \
../src/mach/plasma/uart.cc \
../src/mach/plasma/uart_init.cc 

OBJS += \
./src/mach/plasma/ic.o \
./src/mach/plasma/ic_init.o \
./src/mach/plasma/machine.o \
./src/mach/plasma/machine_init.o \
./src/mach/plasma/nic.o \
./src/mach/plasma/nic_init.o \
./src/mach/plasma/plasma_interrupt.o \
./src/mach/plasma/timer.o \
./src/mach/plasma/timer_init.o \
./src/mach/plasma/timer_test.o \
./src/mach/plasma/uart.o \
./src/mach/plasma/uart_init.o 

CC_DEPS += \
./src/mach/plasma/ic.d \
./src/mach/plasma/ic_init.d \
./src/mach/plasma/machine.d \
./src/mach/plasma/machine_init.d \
./src/mach/plasma/nic.d \
./src/mach/plasma/nic_init.d \
./src/mach/plasma/plasma_interrupt.d \
./src/mach/plasma/timer.d \
./src/mach/plasma/timer_init.d \
./src/mach/plasma/timer_test.d \
./src/mach/plasma/uart.d \
./src/mach/plasma/uart_init.d 


# Each subdirectory must supply rules for building sources it contributes
src/mach/plasma/%.o: ../src/mach/plasma/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


