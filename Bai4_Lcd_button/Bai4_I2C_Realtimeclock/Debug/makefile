################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := Bai4_I2C_Realtimeclock
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
Bai4_I2C_Realtimeclock.elf \

MAP_FILES += \
Bai4_I2C_Realtimeclock.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
Bai4_I2C_Realtimeclock.list \

OBJCOPY_HEX += \
Bai4_I2C_Realtimeclock.hex \


# All Target
all: main-build

# Main-build Target
main-build: Bai4_I2C_Realtimeclock.elf secondary-outputs

# Tool invocations
Bai4_I2C_Realtimeclock.elf Bai4_I2C_Realtimeclock.map: $(OBJS) $(USER_OBJS) C:\Users\Admin\Downloads\LAB2\LAB2\Embedded_lab2\Bai4_I2C_Realtimeclock\STM32F407ZGTX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "Bai4_I2C_Realtimeclock.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\Users\Admin\Downloads\LAB2\LAB2\Embedded_lab2\Bai4_I2C_Realtimeclock\STM32F407ZGTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="Bai4_I2C_Realtimeclock.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

Bai4_I2C_Realtimeclock.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "Bai4_I2C_Realtimeclock.list"
	@echo 'Finished building: $@'
	@echo ' '

Bai4_I2C_Realtimeclock.hex: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O ihex $(EXECUTABLES) "Bai4_I2C_Realtimeclock.hex"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) Bai4_I2C_Realtimeclock.elf Bai4_I2C_Realtimeclock.hex Bai4_I2C_Realtimeclock.list Bai4_I2C_Realtimeclock.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_HEX)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
