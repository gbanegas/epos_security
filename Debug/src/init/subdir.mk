################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/init/init_application.cc \
../src/init/init_first.cc \
../src/init/init_system.cc 

OBJS += \
./src/init/init_application.o \
./src/init/init_first.o \
./src/init/init_system.o 

CC_DEPS += \
./src/init/init_application.d \
./src/init/init_first.d \
./src/init/init_system.d 


# Each subdirectory must supply rules for building sources it contributes
src/init/%.o: ../src/init/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


