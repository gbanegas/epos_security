################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/system/application_scaffold.cc \
../src/system/system_scaffold.cc 

OBJS += \
./src/system/application_scaffold.o \
./src/system/system_scaffold.o 

CC_DEPS += \
./src/system/application_scaffold.d \
./src/system/system_scaffold.d 


# Each subdirectory must supply rules for building sources it contributes
src/system/%.o: ../src/system/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


