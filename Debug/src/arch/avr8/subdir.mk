################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../src/arch/avr8/avr8_crt1.S \
../src/arch/avr8/avr8_crtbegin.S \
../src/arch/avr8/avr8_crtend.S \
../src/arch/avr8/avr8_crti.S \
../src/arch/avr8/avr8_crtn.S 

CC_SRCS += \
../src/arch/avr8/cpu.cc \
../src/arch/avr8/cpu_init.cc \
../src/arch/avr8/cpu_test.cc \
../src/arch/avr8/mmu.cc \
../src/arch/avr8/mmu_init.cc \
../src/arch/avr8/mmu_test.cc \
../src/arch/avr8/tsc.cc \
../src/arch/avr8/tsc_init.cc \
../src/arch/avr8/tsc_test.cc 

OBJS += \
./src/arch/avr8/avr8_crt1.o \
./src/arch/avr8/avr8_crtbegin.o \
./src/arch/avr8/avr8_crtend.o \
./src/arch/avr8/avr8_crti.o \
./src/arch/avr8/avr8_crtn.o \
./src/arch/avr8/cpu.o \
./src/arch/avr8/cpu_init.o \
./src/arch/avr8/cpu_test.o \
./src/arch/avr8/mmu.o \
./src/arch/avr8/mmu_init.o \
./src/arch/avr8/mmu_test.o \
./src/arch/avr8/tsc.o \
./src/arch/avr8/tsc_init.o \
./src/arch/avr8/tsc_test.o 

CC_DEPS += \
./src/arch/avr8/cpu.d \
./src/arch/avr8/cpu_init.d \
./src/arch/avr8/cpu_test.d \
./src/arch/avr8/mmu.d \
./src/arch/avr8/mmu_init.d \
./src/arch/avr8/mmu_test.d \
./src/arch/avr8/tsc.d \
./src/arch/avr8/tsc_init.d \
./src/arch/avr8/tsc_test.d 


# Each subdirectory must supply rules for building sources it contributes
src/arch/avr8/%.o: ../src/arch/avr8/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GCC Assembler'
	as  -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/arch/avr8/%.o: ../src/arch/avr8/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


