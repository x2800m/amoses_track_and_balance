/*
 * amoses_track_and_balance.c
 *
 *  Created on: Oct 11, 2023
 *      Author: amoses
 */

/* FreeRTOS headers */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

/* Driver headers*/
#include "pin_mux.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_gpio.h"
#include <stdbool.h>
#include "fsl_power.h"
#include "fsl_gint.h"
#include "fsl_ctimer.h"
#include "fsl_lpadc.h"
#include "fsl_anactrl.h"
#include "fsl_iocon.h"

/* Track and Balance specific headers */
#include "amoses_track_and_balance.h"
#include "amoses_core_functions.h"
//-------------------------------------------------------------------------------------------------
//ADC variables.
volatile bool g_LpadcConversionCompletedFlag = false;
lpadc_conv_result_t adc_result_struct;
uint8_t  	raw_adc_buffer_selector = 0;
uint16_t 	raw_adc_buffer[2][ACCEL_ADC_BUFFER_LENGTH];
uint16_t	raw_adc_collection_index = 0;
uint16_t 	raw_adc_meaningful_buffer_length[2];
uint32_t	current_adc_match_value_increment = ADC_MATCH_VALUE_DEFAULT_INCREMENT;

//Timer match information and callbacks
ctimer_match_config_t strobe_matchconfig;
ctimer_match_config_t adc_matchconfig;
ctimer_match_config_t anti_strobe_matchconfig;
ctimer_callback_t ctimer_matches[] = {
		NULL, strobe_callback, adc_trigger_callback, anti_strobe_callback, NULL, NULL, NULL, NULL};

//Rotor speed
float 		main_rotor_rpm = 0.0;
float		main_rotor_rotational_period = 0.0;
uint32_t	rpm_timer_ticks = 0xFFFFFFFF;
uint8_t 	tach_cnt = 0;

//The filter variables and coefficients
float 		f1_a0 = 0.98441445;
float 		f1_b0 = 0.00779278;
float 		f1_b1 = 0.00779278;
uint16_t 	old_filtered_value = 0;
uint16_t 	current_value = 0;
uint16_t 	old_value = 0;
uint8_t 	filter_enabled = 1;

uint32_t 	button_iocon = 0;
//-------------------------------------------------------------------------------------------------
extern uint8_t flash_mode_var;
extern TaskHandle_t tach_led_task_handle, data_processing_task_handle, strobe_task_handle;
//-------------------------------------------------------------------------------------------------
void init_adc(void){
    lpadc_config_t 					mLpadcConfigStruct;
    lpadc_conv_trigger_config_t 	mLpadcTriggerConfigStruct;
    lpadc_conv_command_config_t 	mLpadcCommandConfigStruct;

    CLOCK_SetClkDiv(kCLOCK_DivAdcAsyncClk, 2U, true);
    CLOCK_AttachClk(kFRO_HF_to_ADC_CLK);

    POWER_DisablePD(kPDRUNCFG_PD_LDOGPADC);

    ANACTRL_Init(ANACTRL);
    ANACTRL_EnableVref1V(ANACTRL, true);

    LPADC_GetDefaultConfig(&mLpadcConfigStruct);
    mLpadcConfigStruct.enableAnalogPreliminary = true;
    mLpadcConfigStruct.referenceVoltageSource = ADC_VREF_SOURCE;
    mLpadcConfigStruct.conversionAverageMode = kLPADC_ConversionAverage1;
    LPADC_Init(ADC_BASE, &mLpadcConfigStruct);

    LPADC_DoOffsetCalibration(ADC_BASE);
    LPADC_DoAutoCalibration(ADC_BASE);

    LPADC_GetDefaultConvCommandConfig(&mLpadcCommandConfigStruct);
    mLpadcCommandConfigStruct.channelNumber = ADC_USER_CHANNEL;
    mLpadcCommandConfigStruct.conversionResolutionMode = kLPADC_ConversionResolutionHigh;
    LPADC_SetConvCommandConfig(ADC_BASE, ADC_USER_CMDID, &mLpadcCommandConfigStruct);
    LPADC_GetDefaultConvTriggerConfig(&mLpadcTriggerConfigStruct);
    mLpadcTriggerConfigStruct.targetCommandId       = ADC_USER_CMDID;
    mLpadcTriggerConfigStruct.enableHardwareTrigger = false;
    LPADC_SetConvTriggerConfig(ADC_BASE, 0U, &mLpadcTriggerConfigStruct);

    const uint32_t adc_iocon = (
									 IOCON_PIO_FUNC0 |
									 IOCON_PIO_MODE_INACT |
									 IOCON_PIO_SLEW_STANDARD |
									 IOCON_PIO_INV_DI |
									 IOCON_ANALOG_EN |
									 IOCON_ASW_EN |
									 IOCON_PIO_OPENDRAIN_DI);

    gpio_pin_config_t gpio0_pin20_config = {
    	kGPIO_DigitalInput,
        0,
    };

    GPIO_PinInit(GPIO, ADC_PORT, ADC_PIN, &gpio0_pin20_config);
    IOCON_PinMuxSet(IOCON, ADC_PORT, ADC_PIN, adc_iocon);
}
//-------------------------------------------------------------------------------------------------
void init_gpio(unsigned char tach_polarity){

	CLOCK_EnableClock(kCLOCK_Gpio0);
    CLOCK_EnableClock(kCLOCK_Gpio1);
    CLOCK_EnableClock(kCLOCK_Iocon);

    gpio_pin_config_t rgb_led_config = {
        kGPIO_DigitalOutput,
        0,
    };

    gpio_pin_config_t button_config = {
    	kGPIO_DigitalInput,
        0,
    };

    gpio_pin_config_t tach_pin_config = {
        kGPIO_DigitalInput,
        0,
    };

    if(tach_polarity == 1){
    	button_iocon  = (IOCON_PIO_FUNC0 |
						 IOCON_PIO_MODE_PULLUP |
						 IOCON_PIO_SLEW_STANDARD |
						 IOCON_PIO_INV_DI |
						 IOCON_PIO_DIGITAL_EN |
						 IOCON_PIO_OPENDRAIN_DI);
    }else{
    	button_iocon = ( IOCON_PIO_FUNC0 |
						 IOCON_PIO_MODE_PULLDOWN |
						 IOCON_PIO_SLEW_STANDARD |
						 IOCON_PIO_INV_DI |
						 IOCON_PIO_DIGITAL_EN |
						 IOCON_PIO_OPENDRAIN_DI);
    }

    //Configure the outputs and turn off the LEDs
    GPIO_PinInit(GPIO, RED_LED_PORT, RED_LED_PIN, &rgb_led_config);
    GPIO_PinInit(GPIO, GREEN_LED_PORT, GREEN_LED_PIN, &rgb_led_config);
    GPIO_PinInit(GPIO, BLUE_LED_PORT, BLUE_LED_PIN, &rgb_led_config);
    GPIO_PinWrite(GPIO, RED_LED_PORT, RED_LED_PIN, 0);
    GPIO_PinWrite(GPIO, GREEN_LED_PORT, GREEN_LED_PIN, 0);
    GPIO_PinWrite(GPIO, BLUE_LED_PORT, BLUE_LED_PIN, 0);

    //Configure the inputs
    GPIO_PinInit(GPIO, TACH_PORT, TACH_PIN, &tach_pin_config);
    GPIO_PinInit(GPIO, BUTTON_PORT, BUTTON_PIN, &button_config);

    IOCON_PinMuxSet(IOCON, 1U, 9U, button_iocon);
    IOCON_PinMuxSet(IOCON, 1U, 18U, button_iocon);
}
//-------------------------------------------------------------------------------------------------
void init_tach_pinint(unsigned char polarity){
	GINT_Init(TACHOMETER_GINT);
	GINT_SetCtrl(TACHOMETER_GINT, kGINT_CombineOr, kGINT_TrigEdge, tach_callback);
	if(polarity == 0){
		GINT_ConfigPins(TACHOMETER_GINT, BUTTON_PORT, (1U << BUTTON_PIN), (1U << BUTTON_PIN)); //Configure pin and polarity
	}else{
		GINT_ConfigPins(TACHOMETER_GINT, BUTTON_PORT, ~(1U << BUTTON_PIN), (1U << BUTTON_PIN)); //Configure pin and polarity
	}
	GINT_EnableCallback(TACHOMETER_GINT);
}
//-------------------------------------------------------------------------------------------------
void init_rpm_timer(void){
	ctimer_config_t config = {
		kCTIMER_TimerMode,
		kCTIMER_Capture_0,
		0,
	};

	CLOCK_AttachClk(kFRO_HF_to_CTIMER2);
	CTIMER_Init(RPM_CTIMER, &config);

	strobe_matchconfig.enableCounterReset = false;
	strobe_matchconfig.enableCounterStop  = false;
	strobe_matchconfig.matchValue         = 0xFFFFFFFF;
	strobe_matchconfig.outControl         = kCTIMER_Output_NoAction;
	strobe_matchconfig.outPinInitState    = false;
	strobe_matchconfig.enableInterrupt    = true;

	adc_matchconfig.enableCounterReset = false;
	adc_matchconfig.enableCounterStop  = false;
	adc_matchconfig.matchValue         = current_adc_match_value_increment;
	adc_matchconfig.outControl         = kCTIMER_Output_NoAction;
	adc_matchconfig.outPinInitState    = false;
	adc_matchconfig.enableInterrupt    = true;

	anti_strobe_matchconfig.enableCounterReset = false;
	anti_strobe_matchconfig.enableCounterStop  = false;
	anti_strobe_matchconfig.matchValue         = 0xFFFFFFFF;
	anti_strobe_matchconfig.outControl         = kCTIMER_Output_NoAction;
	anti_strobe_matchconfig.outPinInitState    = false;
	anti_strobe_matchconfig.enableInterrupt    = true;

	CTIMER_RegisterCallBack(RPM_CTIMER, &ctimer_matches[0], kCTIMER_MultipleCallback);

	CTIMER_SetupMatch(RPM_CTIMER, CTIMER_MAT_STROBE, &strobe_matchconfig);
	CTIMER_SetupMatch(RPM_CTIMER, CTIMER_MAT_ADC, &adc_matchconfig);
	CTIMER_SetupMatch(RPM_CTIMER, CTIMER_MAT_ANTI_STROBE, &anti_strobe_matchconfig);

	CTIMER_StartTimer(RPM_CTIMER);

	RPM_CTIMER->TC = 0;
}
//-------------------------------------------------------------------------------------------------
void adc_trigger_callback(void){
	uint16_t new_measurement = 0;

	LPADC_DoSoftwareTrigger(ADC_BASE, 1U); 		// Trigger the ADC measurement (1U is trigger0 mask)
	while (!LPADC_GetConvResult(ADC_BASE, &adc_result_struct, 0U));
	new_measurement = adc_result_struct.convValue;

	if(filter_enabled == 1){
		current_value = new_measurement;
		raw_adc_buffer[raw_adc_buffer_selector][raw_adc_collection_index] = (uint16_t)( (f1_a0 * old_filtered_value) +
																						(f1_b0 * current_value) +
																						(f1_b1 * old_value));

		old_filtered_value = raw_adc_buffer[raw_adc_buffer_selector][raw_adc_collection_index];
		old_value = current_value;
	}else{
		raw_adc_buffer[raw_adc_buffer_selector][raw_adc_collection_index] = new_measurement;
	}

	//Turn on the red LED if we have an ADC sample overrun.
	if(raw_adc_collection_index > ACCEL_ADC_BUFFER_LENGTH){
		GPIO_PinWrite(GPIO, RED_LED_PORT, RED_LED_PIN, 1);
	}else{
		GPIO_PinWrite(GPIO, RED_LED_PORT, RED_LED_PIN, 0);
		raw_adc_collection_index ++;
	}

	//Set the new match register value to trigger this routine at some point in the future.
	if(adc_matchconfig.matchValue + current_adc_match_value_increment  < 0xFFFFFFFF){
		adc_matchconfig.matchValue += current_adc_match_value_increment;
		CTIMER_SetupMatch(RPM_CTIMER, CTIMER_MAT_ADC, &adc_matchconfig);
	}else{
		adc_matchconfig.matchValue = current_adc_match_value_increment;
	}
}
//-------------------------------------------------------------------------------------------------
void strobe_callback(void){
	strobe_flash();
}
//-------------------------------------------------------------------------------------------------
void strobe_flash(void){
	uint32_t strobe_future_time = 0;
	GPIO_PinWrite(GPIO, GREEN_LED_PORT, GREEN_LED_PIN, 1);
	strobe_future_time = RPM_CTIMER->TC;
	anti_strobe_matchconfig.matchValue = strobe_future_time + ((TIMER_TICS_PER_SEC / 1000000) * STROBE_FLASH_DURATION_US);
	CTIMER_SetupMatch(RPM_CTIMER, CTIMER_MAT_ANTI_STROBE, &anti_strobe_matchconfig);
}
//-------------------------------------------------------------------------------------------------
void anti_strobe_callback(void){
	GPIO_PinWrite(GPIO, GREEN_LED_PORT, GREEN_LED_PIN, 0);
}
//-------------------------------------------------------------------------------------------------
void tach_callback(void){
	BaseType_t xHigherPriorityTaskWoken;
	uint32_t ulStatusRegister;

	//Record then reset the ticks used for the rpm counter
	rpm_timer_ticks = RPM_CTIMER->TC;
	RPM_CTIMER->TC = 0;

	//Recalculate the ADC trigger rate to scale according to the target rpm
	current_adc_match_value_increment = rpm_timer_ticks / ACCEL_ADC_SAMPLES_PER_REV;

	//Swap buffer location and record the number of samples taken.
	raw_adc_meaningful_buffer_length[raw_adc_buffer_selector] = raw_adc_collection_index - 1;
	raw_adc_buffer_selector = (~raw_adc_buffer_selector) & 0x00000001;	//Toggle buffer selection
	raw_adc_collection_index = 0;										//Reset ADC buffer index selection to zero.

	strobe_matchconfig.matchValue = rpm_timer_ticks/2;
	CTIMER_SetupMatch(RPM_CTIMER, CTIMER_MAT_STROBE, &strobe_matchconfig);

	adc_matchconfig.matchValue = current_adc_match_value_increment;
	CTIMER_SetupMatch(RPM_CTIMER, CTIMER_MAT_ADC, &adc_matchconfig);

	strobe_flash();
	tach_cnt ++;

	//Trigger the data_handling task
	xHigherPriorityTaskWoken = pdFALSE;
	xTaskNotifyIndexedFromISR(	data_processing_task_handle,
								0,
								ulStatusRegister,
								eNoAction,
								&xHigherPriorityTaskWoken );

}
//-------------------------------------------------------------------------------------------------
