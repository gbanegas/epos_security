################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/arch/common/mmu_common.cc 

OBJS += \
./src/arch/common/mmu_common.o 

CC_DEPS += \
./src/arch/common/mmu_common.d 


# Each subdirectory must supply rules for building sources it contributes
src/arch/common/%.o: ../src/arch/common/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


