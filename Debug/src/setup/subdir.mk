################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/setup/ml310_setup.cc \
../src/setup/pc_setup.cc 

OBJS += \
./src/setup/ml310_setup.o \
./src/setup/pc_setup.o 

CC_DEPS += \
./src/setup/ml310_setup.d \
./src/setup/pc_setup.d 


# Each subdirectory must supply rules for building sources it contributes
src/setup/%.o: ../src/setup/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


