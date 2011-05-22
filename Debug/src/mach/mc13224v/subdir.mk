################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/mach/mc13224v/battery.cc \
../src/mach/mc13224v/battery_init.cc \
../src/mach/mc13224v/battery_test.cc \
../src/mach/mc13224v/buck_regulator.cc \
../src/mach/mc13224v/ertj1vg103fa.cc \
../src/mach/mc13224v/flash.cc \
../src/mach/mc13224v/ic.cc \
../src/mach/mc13224v/machine.cc \
../src/mach/mc13224v/machine_init.cc \
../src/mach/mc13224v/nic_init.cc \
../src/mach/mc13224v/radio.cc \
../src/mach/mc13224v/timer.cc \
../src/mach/mc13224v/transceiver.cc \
../src/mach/mc13224v/uart.cc 

OBJS += \
./src/mach/mc13224v/battery.o \
./src/mach/mc13224v/battery_init.o \
./src/mach/mc13224v/battery_test.o \
./src/mach/mc13224v/buck_regulator.o \
./src/mach/mc13224v/ertj1vg103fa.o \
./src/mach/mc13224v/flash.o \
./src/mach/mc13224v/ic.o \
./src/mach/mc13224v/machine.o \
./src/mach/mc13224v/machine_init.o \
./src/mach/mc13224v/nic_init.o \
./src/mach/mc13224v/radio.o \
./src/mach/mc13224v/timer.o \
./src/mach/mc13224v/transceiver.o \
./src/mach/mc13224v/uart.o 

CC_DEPS += \
./src/mach/mc13224v/battery.d \
./src/mach/mc13224v/battery_init.d \
./src/mach/mc13224v/battery_test.d \
./src/mach/mc13224v/buck_regulator.d \
./src/mach/mc13224v/ertj1vg103fa.d \
./src/mach/mc13224v/flash.d \
./src/mach/mc13224v/ic.d \
./src/mach/mc13224v/machine.d \
./src/mach/mc13224v/machine_init.d \
./src/mach/mc13224v/nic_init.d \
./src/mach/mc13224v/radio.d \
./src/mach/mc13224v/timer.d \
./src/mach/mc13224v/transceiver.d \
./src/mach/mc13224v/uart.d 


# Each subdirectory must supply rules for building sources it contributes
src/mach/mc13224v/%.o: ../src/mach/mc13224v/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


