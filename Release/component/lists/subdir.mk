################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../component/lists/fsl_component_generic_list.c 

C_DEPS += \
./component/lists/fsl_component_generic_list.d 

OBJS += \
./component/lists/fsl_component_generic_list.o 


# Each subdirectory must supply rules for building sources it contributes
component/lists/%.o: ../component/lists/%.c component/lists/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_LPC55S16JBD100 -DCPU_LPC55S16JBD100_cm33 -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DPRINTF_FLOAT_ENABLE=1 -DSDK_DEBUGCONSOLE_UART -DSDK_OS_FREE_RTOS -DFFR_INCLUDE='"fsl_iap_ffr.h"' -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DNDEBUG -D__REDLIB__ -I"C:\workspace\mcuxpresso\amoses_tab_2\board" -I"C:\workspace\mcuxpresso\amoses_tab_2\source" -I"C:\workspace\mcuxpresso\amoses_tab_2\freertos\freertos-kernel\include" -I"C:\workspace\mcuxpresso\amoses_tab_2\drivers" -I"C:\workspace\mcuxpresso\amoses_tab_2\LPC55S16\drivers" -I"C:\workspace\mcuxpresso\amoses_tab_2\drivers\freertos" -I"C:\workspace\mcuxpresso\amoses_tab_2\CMSIS_driver\Include" -I"C:\workspace\mcuxpresso\amoses_tab_2\device" -I"C:\workspace\mcuxpresso\amoses_tab_2\CMSIS" -I"C:\workspace\mcuxpresso\amoses_tab_2\component\serial_manager" -I"C:\workspace\mcuxpresso\amoses_tab_2\utilities" -I"C:\workspace\mcuxpresso\amoses_tab_2\component\uart" -I"C:\workspace\mcuxpresso\amoses_tab_2\freertos\freertos-kernel\portable\GCC\ARM_CM33_NTZ\non_secure" -I"C:\workspace\mcuxpresso\amoses_tab_2\component\lists" -Os -fno-common -g -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-component-2f-lists

clean-component-2f-lists:
	-$(RM) ./component/lists/fsl_component_generic_list.d ./component/lists/fsl_component_generic_list.o

.PHONY: clean-component-2f-lists

