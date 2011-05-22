################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../src/arch/mips32/mips32_crt1.S \
../src/arch/mips32/mips32_crtbegin.S \
../src/arch/mips32/mips32_crtend.S \
../src/arch/mips32/mips32_crti.S \
../src/arch/mips32/mips32_crtn.S 

CC_SRCS += \
../src/arch/mips32/cpu.cc \
../src/arch/mips32/cpu_init.cc \
../src/arch/mips32/mmu.cc \
../src/arch/mips32/mmu_init.cc 

OBJS += \
./src/arch/mips32/cpu.o \
./src/arch/mips32/cpu_init.o \
./src/arch/mips32/mips32_crt1.o \
./src/arch/mips32/mips32_crtbegin.o \
./src/arch/mips32/mips32_crtend.o \
./src/arch/mips32/mips32_crti.o \
./src/arch/mips32/mips32_crtn.o \
./src/arch/mips32/mmu.o \
./src/arch/mips32/mmu_init.o 

CC_DEPS += \
./src/arch/mips32/cpu.d \
./src/arch/mips32/cpu_init.d \
./src/arch/mips32/mmu.d \
./src/arch/mips32/mmu_init.d 


# Each subdirectory must supply rules for building sources it contributes
src/arch/mips32/%.o: ../src/arch/mips32/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/arch/mips32/%.o: ../src/arch/mips32/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GCC Assembler'
	as  -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


