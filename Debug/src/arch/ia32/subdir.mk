################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../src/arch/ia32/ia32_crt1.S \
../src/arch/ia32/ia32_crtbegin.S \
../src/arch/ia32/ia32_crtend.S \
../src/arch/ia32/ia32_crti.S \
../src/arch/ia32/ia32_crtn.S \
../src/arch/ia32/memchr.S \
../src/arch/ia32/memcmp.S \
../src/arch/ia32/memcpy.S \
../src/arch/ia32/memset.S \
../src/arch/ia32/strchr.S \
../src/arch/ia32/strlen.S 

CC_SRCS += \
../src/arch/ia32/cpu.cc \
../src/arch/ia32/cpu_init.cc \
../src/arch/ia32/cpu_test.cc \
../src/arch/ia32/mmu.cc \
../src/arch/ia32/mmu_init.cc \
../src/arch/ia32/mmu_test.cc \
../src/arch/ia32/tsc_test.cc 

OBJS += \
./src/arch/ia32/cpu.o \
./src/arch/ia32/cpu_init.o \
./src/arch/ia32/cpu_test.o \
./src/arch/ia32/ia32_crt1.o \
./src/arch/ia32/ia32_crtbegin.o \
./src/arch/ia32/ia32_crtend.o \
./src/arch/ia32/ia32_crti.o \
./src/arch/ia32/ia32_crtn.o \
./src/arch/ia32/memchr.o \
./src/arch/ia32/memcmp.o \
./src/arch/ia32/memcpy.o \
./src/arch/ia32/memset.o \
./src/arch/ia32/mmu.o \
./src/arch/ia32/mmu_init.o \
./src/arch/ia32/mmu_test.o \
./src/arch/ia32/strchr.o \
./src/arch/ia32/strlen.o \
./src/arch/ia32/tsc_test.o 

CC_DEPS += \
./src/arch/ia32/cpu.d \
./src/arch/ia32/cpu_init.d \
./src/arch/ia32/cpu_test.d \
./src/arch/ia32/mmu.d \
./src/arch/ia32/mmu_init.d \
./src/arch/ia32/mmu_test.d \
./src/arch/ia32/tsc_test.d 


# Each subdirectory must supply rules for building sources it contributes
src/arch/ia32/%.o: ../src/arch/ia32/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/arch/ia32/%.o: ../src/arch/ia32/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GCC Assembler'
	as  -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


