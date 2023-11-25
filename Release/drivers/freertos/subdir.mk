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
	arm-none-eabi-gcc -DCPU_LPC55S16JBD100 -DCPU_LPC55S16JBD100_cm33 -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DPRINTF_FLOAT_ENABLE=1 -DSDK_DEBUGCONSOLE_UART -DSDK_OS_FREE_RTOS -DFFR_INCLUDE='"fsl_iap_ffr.h"' -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DNDEBUG -D__REDLIB__ -I"C:\workspace\mcuxpresso\amoses_tab_2\board" -I"C:\workspace\mcuxpresso\amoses_tab_2\source" -I"C:\workspace\mcuxpresso\amoses_tab_2\freertos\freertos-kernel\include" -I"C:\workspace\mcuxpresso\amoses_tab_2\drivers" -I"C:\workspace\mcuxpresso\amoses_tab_2\LPC55S16\drivers" -I"C:\workspace\mcuxpresso\amoses_tab_2\drivers\freertos" -I"C:\workspace\mcuxpresso\amoses_tab_2\CMSIS_driver\Include" -I"C:\workspace\mcuxpresso\amoses_tab_2\device" -I"C:\workspace\mcuxpresso\amoses_tab_2\CMSIS" -I"C:\workspace\mcuxpresso\amoses_tab_2\component\serial_manager" -I"C:\workspace\mcuxpresso\amoses_tab_2\utilities" -I"C:\workspace\mcuxpresso\amoses_tab_2\component\uart" -I"C:\workspace\mcuxpresso\amoses_tab_2\freertos\freertos-kernel\portable\GCC\ARM_CM33_NTZ\non_secure" -I"C:\workspace\mcuxpresso\amoses_tab_2\component\lists" -Os -fno-common -g -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-drivers-2f-freertos

clean-drivers-2f-freertos:
	-$(RM) ./drivers/freertos/fsl_i2c_freertos.d ./drivers/freertos/fsl_i2c_freertos.o ./drivers/freertos/fsl_spi_freertos.d ./drivers/freertos/fsl_spi_freertos.o ./drivers/freertos/fsl_usart_freertos.d ./drivers/freertos/fsl_usart_freertos.o

.PHONY: clean-drivers-2f-freertos

