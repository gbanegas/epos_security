################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/utility/elf.cc \
../src/utility/handler_test.cc \
../src/utility/hash_test.cc \
../src/utility/heap.cc \
../src/utility/list_test.cc \
../src/utility/malloc.cc \
../src/utility/malloc_test.cc \
../src/utility/observer.cc \
../src/utility/ostream.cc \
../src/utility/ostream_test.cc \
../src/utility/queue_test.cc \
../src/utility/random.cc \
../src/utility/srec.cc \
../src/utility/string.cc \
../src/utility/vector_test.cc 

OBJS += \
./src/utility/elf.o \
./src/utility/handler_test.o \
./src/utility/hash_test.o \
./src/utility/heap.o \
./src/utility/list_test.o \
./src/utility/malloc.o \
./src/utility/malloc_test.o \
./src/utility/observer.o \
./src/utility/ostream.o \
./src/utility/ostream_test.o \
./src/utility/queue_test.o \
./src/utility/random.o \
./src/utility/srec.o \
./src/utility/string.o \
./src/utility/vector_test.o 

CC_DEPS += \
./src/utility/elf.d \
./src/utility/handler_test.d \
./src/utility/hash_test.d \
./src/utility/heap.d \
./src/utility/list_test.d \
./src/utility/malloc.d \
./src/utility/malloc_test.d \
./src/utility/observer.d \
./src/utility/ostream.d \
./src/utility/ostream_test.d \
./src/utility/queue_test.d \
./src/utility/random.d \
./src/utility/srec.d \
./src/utility/string.d \
./src/utility/vector_test.d 


# Each subdirectory must supply rules for building sources it contributes
src/utility/%.o: ../src/utility/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


