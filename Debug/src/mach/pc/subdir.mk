################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/mach/pc/device.cc \
../src/mach/pc/display.cc \
../src/mach/pc/display_test.cc \
../src/mach/pc/e100.cc \
../src/mach/pc/e100_init.cc \
../src/mach/pc/ethernet_init.cc \
../src/mach/pc/ic.cc \
../src/mach/pc/ic_init.cc \
../src/mach/pc/ic_test.cc \
../src/mach/pc/machine.cc \
../src/mach/pc/machine_init.cc \
../src/mach/pc/machine_test.cc \
../src/mach/pc/nic_test.cc \
../src/mach/pc/pc_interrupt.cc \
../src/mach/pc/pci.cc \
../src/mach/pc/pci_init.cc \
../src/mach/pc/pci_test.cc \
../src/mach/pc/pcnet32.cc \
../src/mach/pc/pcnet32_init.cc \
../src/mach/pc/rtc.cc \
../src/mach/pc/rtc_test.cc \
../src/mach/pc/timer.cc \
../src/mach/pc/timer_init.cc \
../src/mach/pc/timer_test.cc \
../src/mach/pc/uart.cc \
../src/mach/pc/uart_test.cc 

OBJS += \
./src/mach/pc/device.o \
./src/mach/pc/display.o \
./src/mach/pc/display_test.o \
./src/mach/pc/e100.o \
./src/mach/pc/e100_init.o \
./src/mach/pc/ethernet_init.o \
./src/mach/pc/ic.o \
./src/mach/pc/ic_init.o \
./src/mach/pc/ic_test.o \
./src/mach/pc/machine.o \
./src/mach/pc/machine_init.o \
./src/mach/pc/machine_test.o \
./src/mach/pc/nic_test.o \
./src/mach/pc/pc_interrupt.o \
./src/mach/pc/pci.o \
./src/mach/pc/pci_init.o \
./src/mach/pc/pci_test.o \
./src/mach/pc/pcnet32.o \
./src/mach/pc/pcnet32_init.o \
./src/mach/pc/rtc.o \
./src/mach/pc/rtc_test.o \
./src/mach/pc/timer.o \
./src/mach/pc/timer_init.o \
./src/mach/pc/timer_test.o \
./src/mach/pc/uart.o \
./src/mach/pc/uart_test.o 

CC_DEPS += \
./src/mach/pc/device.d \
./src/mach/pc/display.d \
./src/mach/pc/display_test.d \
./src/mach/pc/e100.d \
./src/mach/pc/e100_init.d \
./src/mach/pc/ethernet_init.d \
./src/mach/pc/ic.d \
./src/mach/pc/ic_init.d \
./src/mach/pc/ic_test.d \
./src/mach/pc/machine.d \
./src/mach/pc/machine_init.d \
./src/mach/pc/machine_test.d \
./src/mach/pc/nic_test.d \
./src/mach/pc/pc_interrupt.d \
./src/mach/pc/pci.d \
./src/mach/pc/pci_init.d \
./src/mach/pc/pci_test.d \
./src/mach/pc/pcnet32.d \
./src/mach/pc/pcnet32_init.d \
./src/mach/pc/rtc.d \
./src/mach/pc/rtc_test.d \
./src/mach/pc/timer.d \
./src/mach/pc/timer_init.d \
./src/mach/pc/timer_test.d \
./src/mach/pc/uart.d \
./src/mach/pc/uart_test.d 


# Each subdirectory must supply rules for building sources it contributes
src/mach/pc/%.o: ../src/mach/pc/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


