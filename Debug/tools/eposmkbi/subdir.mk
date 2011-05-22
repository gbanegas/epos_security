################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../tools/eposmkbi/eposmkbi.cc 

OBJS += \
./tools/eposmkbi/eposmkbi.o 

CC_DEPS += \
./tools/eposmkbi/eposmkbi.d 


# Each subdirectory must supply rules for building sources it contributes
tools/eposmkbi/%.o: ../tools/eposmkbi/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


