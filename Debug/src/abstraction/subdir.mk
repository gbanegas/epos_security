################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/abstraction/active_test.cc \
../src/abstraction/address_space_test.cc \
../src/abstraction/alarm.cc \
../src/abstraction/alarm_init.cc \
../src/abstraction/alarm_test.cc \
../src/abstraction/chronometer_test.cc \
../src/abstraction/clock_test.cc \
../src/abstraction/condition_test.cc \
../src/abstraction/cpu_affinity_scheduler.cc \
../src/abstraction/icmp.cc \
../src/abstraction/icmp_test.cc \
../src/abstraction/ieee1451_ncap.cc \
../src/abstraction/ieee1451_ncap_application.cc \
../src/abstraction/ieee1451_test.cc \
../src/abstraction/ieee1451_tim.cc \
../src/abstraction/ieee1451_tim_temperature_sensor.cc \
../src/abstraction/ip.cc \
../src/abstraction/mutex_test.cc \
../src/abstraction/network.cc \
../src/abstraction/network_test.cc \
../src/abstraction/periodic_thread_test.cc \
../src/abstraction/scheduler_test.cc \
../src/abstraction/segment_test.cc \
../src/abstraction/semaphore2_test.cc \
../src/abstraction/semaphore_test.cc \
../src/abstraction/sip_body.cc \
../src/abstraction/sip_defs.cc \
../src/abstraction/sip_header.cc \
../src/abstraction/sip_manager.cc \
../src/abstraction/sip_message.cc \
../src/abstraction/sip_test.cc \
../src/abstraction/sip_transactions.cc \
../src/abstraction/sip_user_agent.cc \
../src/abstraction/system_init.cc \
../src/abstraction/task_init.cc \
../src/abstraction/task_test.cc \
../src/abstraction/tcp.cc \
../src/abstraction/tcp_test.cc \
../src/abstraction/thread.cc \
../src/abstraction/thread_init.cc \
../src/abstraction/thread_test.cc \
../src/abstraction/udp.cc \
../src/abstraction/udp_test.cc 

OBJS += \
./src/abstraction/active_test.o \
./src/abstraction/address_space_test.o \
./src/abstraction/alarm.o \
./src/abstraction/alarm_init.o \
./src/abstraction/alarm_test.o \
./src/abstraction/chronometer_test.o \
./src/abstraction/clock_test.o \
./src/abstraction/condition_test.o \
./src/abstraction/cpu_affinity_scheduler.o \
./src/abstraction/icmp.o \
./src/abstraction/icmp_test.o \
./src/abstraction/ieee1451_ncap.o \
./src/abstraction/ieee1451_ncap_application.o \
./src/abstraction/ieee1451_test.o \
./src/abstraction/ieee1451_tim.o \
./src/abstraction/ieee1451_tim_temperature_sensor.o \
./src/abstraction/ip.o \
./src/abstraction/mutex_test.o \
./src/abstraction/network.o \
./src/abstraction/network_test.o \
./src/abstraction/periodic_thread_test.o \
./src/abstraction/scheduler_test.o \
./src/abstraction/segment_test.o \
./src/abstraction/semaphore2_test.o \
./src/abstraction/semaphore_test.o \
./src/abstraction/sip_body.o \
./src/abstraction/sip_defs.o \
./src/abstraction/sip_header.o \
./src/abstraction/sip_manager.o \
./src/abstraction/sip_message.o \
./src/abstraction/sip_test.o \
./src/abstraction/sip_transactions.o \
./src/abstraction/sip_user_agent.o \
./src/abstraction/system_init.o \
./src/abstraction/task_init.o \
./src/abstraction/task_test.o \
./src/abstraction/tcp.o \
./src/abstraction/tcp_test.o \
./src/abstraction/thread.o \
./src/abstraction/thread_init.o \
./src/abstraction/thread_test.o \
./src/abstraction/udp.o \
./src/abstraction/udp_test.o 

CC_DEPS += \
./src/abstraction/active_test.d \
./src/abstraction/address_space_test.d \
./src/abstraction/alarm.d \
./src/abstraction/alarm_init.d \
./src/abstraction/alarm_test.d \
./src/abstraction/chronometer_test.d \
./src/abstraction/clock_test.d \
./src/abstraction/condition_test.d \
./src/abstraction/cpu_affinity_scheduler.d \
./src/abstraction/icmp.d \
./src/abstraction/icmp_test.d \
./src/abstraction/ieee1451_ncap.d \
./src/abstraction/ieee1451_ncap_application.d \
./src/abstraction/ieee1451_test.d \
./src/abstraction/ieee1451_tim.d \
./src/abstraction/ieee1451_tim_temperature_sensor.d \
./src/abstraction/ip.d \
./src/abstraction/mutex_test.d \
./src/abstraction/network.d \
./src/abstraction/network_test.d \
./src/abstraction/periodic_thread_test.d \
./src/abstraction/scheduler_test.d \
./src/abstraction/segment_test.d \
./src/abstraction/semaphore2_test.d \
./src/abstraction/semaphore_test.d \
./src/abstraction/sip_body.d \
./src/abstraction/sip_defs.d \
./src/abstraction/sip_header.d \
./src/abstraction/sip_manager.d \
./src/abstraction/sip_message.d \
./src/abstraction/sip_test.d \
./src/abstraction/sip_transactions.d \
./src/abstraction/sip_user_agent.d \
./src/abstraction/system_init.d \
./src/abstraction/task_init.d \
./src/abstraction/task_test.d \
./src/abstraction/tcp.d \
./src/abstraction/tcp_test.d \
./src/abstraction/thread.d \
./src/abstraction/thread_init.d \
./src/abstraction/thread_test.d \
./src/abstraction/udp.d \
./src/abstraction/udp_test.d 


# Each subdirectory must supply rules for building sources it contributes
src/abstraction/%.o: ../src/abstraction/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


