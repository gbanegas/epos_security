################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../app/at90can128_app.cc \
../app/atmega1281_app.cc \
../app/atmega128_app.cc \
../app/atmega16_app.cc \
../app/integratorcp_app.cc \
../app/mc13224v_app.cc \
../app/ml310_app.cc \
../app/pc_app.cc \
../app/plasma_app.cc 

OBJS += \
./app/at90can128_app.o \
./app/atmega1281_app.o \
./app/atmega128_app.o \
./app/atmega16_app.o \
./app/integratorcp_app.o \
./app/mc13224v_app.o \
./app/ml310_app.o \
./app/pc_app.o \
./app/plasma_app.o 

CC_DEPS += \
./app/at90can128_app.d \
./app/atmega1281_app.d \
./app/atmega128_app.d \
./app/atmega16_app.d \
./app/integratorcp_app.d \
./app/mc13224v_app.d \
./app/ml310_app.d \
./app/pc_app.d \
./app/plasma_app.d 


# Each subdirectory must supply rules for building sources it contributes
app/%.o: ../app/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


