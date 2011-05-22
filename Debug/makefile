################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include tools/eposmkbi/subdir.mk
-include tools/eposldr/subdir.mk
-include src/utility/subdir.mk
-include src/system/subdir.mk
-include src/setup/subdir.mk
-include src/security/subdir.mk
-include src/mach/plasma/subdir.mk
-include src/mach/pc/subdir.mk
-include src/mach/ml310/subdir.mk
-include src/mach/mc13224v/subdir.mk
-include src/mach/integratorcp/subdir.mk
-include src/mach/common/subdir.mk
-include src/mach/atmega16/subdir.mk
-include src/mach/atmega1281/subdir.mk
-include src/mach/atmega128/subdir.mk
-include src/mach/at90can128/subdir.mk
-include src/init/subdir.mk
-include src/boot/subdir.mk
-include src/arch/ppc32/subdir.mk
-include src/arch/mips32/subdir.mk
-include src/arch/ia32/subdir.mk
-include src/arch/common/subdir.mk
-include src/arch/avr8/subdir.mk
-include src/arch/arm7/subdir.mk
-include src/abstraction/subdir.mk
-include app/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: epos_security

# Tool invocations
epos_security: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++  -o"epos_security" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C++_DEPS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) epos_security
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
