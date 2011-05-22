################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../src/arch/arm7/arm7_crt1.S \
../src/arch/arm7/arm7_crtbegin.S \
../src/arch/arm7/arm7_crtend.S \
../src/arch/arm7/arm7_crti.S \
../src/arch/arm7/arm7_crtn.S 

CC_SRCS += \
../src/arch/arm7/arm7_setup.cc \
../src/arch/arm7/cpu.cc \
../src/arch/arm7/cpu_init.cc \
../src/arch/arm7/exception.cc \
../src/arch/arm7/mmu.cc \
../src/arch/arm7/mmu_init.cc \
../src/arch/arm7/tsc.cc 

OBJS += \
./src/arch/arm7/arm7_crt1.o \
./src/arch/arm7/arm7_crtbegin.o \
./src/arch/arm7/arm7_crtend.o \
./src/arch/arm7/arm7_crti.o \
./src/arch/arm7/arm7_crtn.o \
./src/arch/arm7/arm7_setup.o \
./src/arch/arm7/cpu.o \
./src/arch/arm7/cpu_init.o \
./src/arch/arm7/exception.o \
./src/arch/arm7/mmu.o \
./src/arch/arm7/mmu_init.o \
./src/arch/arm7/tsc.o 

CC_DEPS += \
./src/arch/arm7/arm7_setup.d \
./src/arch/arm7/cpu.d \
./src/arch/arm7/cpu_init.d \
./src/arch/arm7/exception.d \
./src/arch/arm7/mmu.d \
./src/arch/arm7/mmu_init.d \
./src/arch/arm7/tsc.d 


# Each subdirectory must supply rules for building sources it contributes
src/arch/arm7/%.o: ../src/arch/arm7/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GCC Assembler'
	as  -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/arch/arm7/%.o: ../src/arch/arm7/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


