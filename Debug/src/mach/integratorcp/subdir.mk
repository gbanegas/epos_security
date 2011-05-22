################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/mach/integratorcp/ic.cc \
../src/mach/integratorcp/machine_init.cc \
../src/mach/integratorcp/timer.cc 

OBJS += \
./src/mach/integratorcp/ic.o \
./src/mach/integratorcp/machine_init.o \
./src/mach/integratorcp/timer.o 

CC_DEPS += \
./src/mach/integratorcp/ic.d \
./src/mach/integratorcp/machine_init.d \
./src/mach/integratorcp/timer.d 


# Each subdirectory must supply rules for building sources it contributes
src/mach/integratorcp/%.o: ../src/mach/integratorcp/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


