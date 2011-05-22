################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../src/arch/ppc32/ppc32_crt1.S \
../src/arch/ppc32/ppc32_crtbegin.S \
../src/arch/ppc32/ppc32_crtend.S \
../src/arch/ppc32/ppc32_crti.S \
../src/arch/ppc32/ppc32_crtn.S 

CC_SRCS += \
../src/arch/ppc32/cpu.cc \
../src/arch/ppc32/cpu_init.cc \
../src/arch/ppc32/cpu_test.cc \
../src/arch/ppc32/mmu.cc \
../src/arch/ppc32/mmu_init.cc \
../src/arch/ppc32/mmu_test.cc \
../src/arch/ppc32/tsc_test.cc 

OBJS += \
./src/arch/ppc32/cpu.o \
./src/arch/ppc32/cpu_init.o \
./src/arch/ppc32/cpu_test.o \
./src/arch/ppc32/mmu.o \
./src/arch/ppc32/mmu_init.o \
./src/arch/ppc32/mmu_test.o \
./src/arch/ppc32/ppc32_crt1.o \
./src/arch/ppc32/ppc32_crtbegin.o \
./src/arch/ppc32/ppc32_crtend.o \
./src/arch/ppc32/ppc32_crti.o \
./src/arch/ppc32/ppc32_crtn.o \
./src/arch/ppc32/tsc_test.o 

CC_DEPS += \
./src/arch/ppc32/cpu.d \
./src/arch/ppc32/cpu_init.d \
./src/arch/ppc32/cpu_test.d \
./src/arch/ppc32/mmu.d \
./src/arch/ppc32/mmu_init.d \
./src/arch/ppc32/mmu_test.d \
./src/arch/ppc32/tsc_test.d 


# Each subdirectory must supply rules for building sources it contributes
src/arch/ppc32/%.o: ../src/arch/ppc32/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/arch/ppc32/%.o: ../src/arch/ppc32/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GCC Assembler'
	as  -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


