################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/mach/common/cmac.cc \
../src/mach/common/cmac_init.cc \
../src/mach/common/ethernet.cc \
../src/mach/common/radio_common.cc \
../src/mach/common/rtc_common.cc \
../src/mach/common/serial_display.cc 

OBJS += \
./src/mach/common/cmac.o \
./src/mach/common/cmac_init.o \
./src/mach/common/ethernet.o \
./src/mach/common/radio_common.o \
./src/mach/common/rtc_common.o \
./src/mach/common/serial_display.o 

CC_DEPS += \
./src/mach/common/cmac.d \
./src/mach/common/cmac_init.d \
./src/mach/common/ethernet.d \
./src/mach/common/radio_common.d \
./src/mach/common/rtc_common.d \
./src/mach/common/serial_display.d 


# Each subdirectory must supply rules for building sources it contributes
src/mach/common/%.o: ../src/mach/common/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


