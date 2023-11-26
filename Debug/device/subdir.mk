################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../device/system_LPC55S16.c 

C_DEPS += \
./device/system_LPC55S16.d 

OBJS += \
./device/system_LPC55S16.o 


# Each subdirectory must supply rules for building sources it contributes
device/%.o: ../device/%.c device/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_LPC55S16JBD100 -DCPU_LPC55S16JBD100_cm33 -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DPRINTF_FLOAT_ENABLE=1 -DSDK_DEBUGCONSOLE_UART -DSDK_OS_FREE_RTOS -DFFR_INCLUDE='"fsl_iap_ffr.h"' -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__NEWLIB__ -I"C:\workspace\amoses_track_and_balance\board" -I"C:\workspace\amoses_track_and_balance\source" -I"C:\workspace\amoses_track_and_balance\freertos\freertos-kernel\include" -I"C:\workspace\amoses_track_and_balance\drivers" -I"C:\workspace\amoses_track_and_balance\LPC55S16\drivers" -I"C:\workspace\amoses_track_and_balance\drivers\freertos" -I"C:\workspace\amoses_track_and_balance\CMSIS_driver\Include" -I"C:\workspace\amoses_track_and_balance\device" -I"C:\workspace\amoses_track_and_balance\CMSIS" -I"C:\workspace\amoses_track_and_balance\component\serial_manager" -I"C:\workspace\amoses_track_and_balance\utilities" -I"C:\workspace\amoses_track_and_balance\component\uart" -I"C:\workspace\amoses_track_and_balance\freertos\freertos-kernel\portable\GCC\ARM_CM33_NTZ\non_secure" -I"C:\workspace\amoses_track_and_balance\component\lists" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__NEWLIB__ -fstack-usage -specs=nano.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-device

clean-device:
	-$(RM) ./device/system_LPC55S16.d ./device/system_LPC55S16.o

.PHONY: clean-device

