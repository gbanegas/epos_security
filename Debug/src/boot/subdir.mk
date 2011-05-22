################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../src/boot/pc_boot.S 

CC_SRCS += \
../src/boot/ml310_boot.cc 

OBJS += \
./src/boot/ml310_boot.o \
./src/boot/pc_boot.o 

CC_DEPS += \
./src/boot/ml310_boot.d 


# Each subdirectory must supply rules for building sources it contributes
src/boot/%.o: ../src/boot/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/boot/%.o: ../src/boot/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GCC Assembler'
	as  -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


