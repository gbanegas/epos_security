################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/mach/ml310/device.cc \
../src/mach/ml310/e100.cc \
../src/mach/ml310/e100_init.cc \
../src/mach/ml310/ic.cc \
../src/mach/ml310/ic_init.cc \
../src/mach/ml310/ic_test.cc \
../src/mach/ml310/machine.cc \
../src/mach/ml310/machine_init.cc \
../src/mach/ml310/machine_test.cc \
../src/mach/ml310/nic_init.cc \
../src/mach/ml310/nic_test.cc \
../src/mach/ml310/pci.cc \
../src/mach/ml310/pci_init.cc \
../src/mach/ml310/pci_test.cc \
../src/mach/ml310/rtc.cc \
../src/mach/ml310/rtc_test.cc \
../src/mach/ml310/timer.cc \
../src/mach/ml310/timer_init.cc \
../src/mach/ml310/timer_test.cc \
../src/mach/ml310/uart.cc \
../src/mach/ml310/uart_init.cc \
../src/mach/ml310/uart_test.cc 

OBJS += \
./src/mach/ml310/device.o \
./src/mach/ml310/e100.o \
./src/mach/ml310/e100_init.o \
./src/mach/ml310/ic.o \
./src/mach/ml310/ic_init.o \
./src/mach/ml310/ic_test.o \
./src/mach/ml310/machine.o \
./src/mach/ml310/machine_init.o \
./src/mach/ml310/machine_test.o \
./src/mach/ml310/nic_init.o \
./src/mach/ml310/nic_test.o \
./src/mach/ml310/pci.o \
./src/mach/ml310/pci_init.o \
./src/mach/ml310/pci_test.o \
./src/mach/ml310/rtc.o \
./src/mach/ml310/rtc_test.o \
./src/mach/ml310/timer.o \
./src/mach/ml310/timer_init.o \
./src/mach/ml310/timer_test.o \
./src/mach/ml310/uart.o \
./src/mach/ml310/uart_init.o \
./src/mach/ml310/uart_test.o 

CC_DEPS += \
./src/mach/ml310/device.d \
./src/mach/ml310/e100.d \
./src/mach/ml310/e100_init.d \
./src/mach/ml310/ic.d \
./src/mach/ml310/ic_init.d \
./src/mach/ml310/ic_test.d \
./src/mach/ml310/machine.d \
./src/mach/ml310/machine_init.d \
./src/mach/ml310/machine_test.d \
./src/mach/ml310/nic_init.d \
./src/mach/ml310/nic_test.d \
./src/mach/ml310/pci.d \
./src/mach/ml310/pci_init.d \
./src/mach/ml310/pci_test.d \
./src/mach/ml310/rtc.d \
./src/mach/ml310/rtc_test.d \
./src/mach/ml310/timer.d \
./src/mach/ml310/timer_init.d \
./src/mach/ml310/timer_test.d \
./src/mach/ml310/uart.d \
./src/mach/ml310/uart_init.d \
./src/mach/ml310/uart_test.d 


# Each subdirectory must supply rules for building sources it contributes
src/mach/ml310/%.o: ../src/mach/ml310/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


