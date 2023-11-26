################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/freertos/fsl_i2c_freertos.c \
../drivers/freertos/fsl_spi_freertos.c \
../drivers/freertos/fsl_usart_freertos.c 

C_DEPS += \
./drivers/freertos/fsl_i2c_freertos.d \
./drivers/freertos/fsl_spi_freertos.d \
./drivers/freertos/fsl_usart_freertos.d 

OBJS += \
./drivers/freertos/fsl_i2c_freertos.o \
./drivers/freertos/fsl_spi_freertos.o \
./drivers/freertos/fsl_usart_freertos.o 


# Each subdirectory must supply rules for building sources it contributes
drivers/freertos/%.o: ../drivers/freertos/%.c drivers/freertos/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_LPC55S16JBD100 -DCPU_LPC55S16JBD100_cm33 -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DPRINTF_FLOAT_ENABLE=1 -DSDK_DEBUGCONSOLE_UART -DSDK_OS_FREE_RTOS -DFFR_INCLUDE='"fsl_iap_ffr.h"' -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__NEWLIB__ -I"C:\workspace\amoses_track_and_balance\board" -I"C:\workspace\amoses_track_and_balance\source" -I"C:\workspace\amoses_track_and_balance\freertos\freertos-kernel\include" -I"C:\workspace\amoses_track_and_balance\drivers" -I"C:\workspace\amoses_track_and_balance\LPC55S16\drivers" -I"C:\workspace\amoses_track_and_balance\drivers\freertos" -I"C:\workspace\amoses_track_and_balance\CMSIS_driver\Include" -I"C:\workspace\amoses_track_and_balance\device" -I"C:\workspace\amoses_track_and_balance\CMSIS" -I"C:\workspace\amoses_track_and_balance\component\serial_manager" -I"C:\workspace\amoses_track_and_balance\utilities" -I"C:\workspace\amoses_track_and_balance\component\uart" -I"C:\workspace\amoses_track_and_balance\freertos\freertos-kernel\portable\GCC\ARM_CM33_NTZ\non_secure" -I"C:\workspace\amoses_track_and_balance\component\lists" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__NEWLIB__ -fstack-usage -specs=nano.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-drivers-2f-freertos

clean-drivers-2f-freertos:
	-$(RM) ./drivers/freertos/fsl_i2c_freertos.d ./drivers/freertos/fsl_i2c_freertos.o ./drivers/freertos/fsl_spi_freertos.d ./drivers/freertos/fsl_spi_freertos.o ./drivers/freertos/fsl_usart_freertos.d ./drivers/freertos/fsl_usart_freertos.o

.PHONY: clean-drivers-2f-freertos

