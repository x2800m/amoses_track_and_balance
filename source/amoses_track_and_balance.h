/*
 * amoses_track_and_balance.h
 *
 *  Created on: Oct 11, 2023
 *      Author: amoses
 */

#ifndef AMOSES_TRACK_AND_BALANCE_H_
#define AMOSES_TRACK_AND_BALANCE_H_
/*
 * red 				- 1_4
 * green 			- 1_7
 * blue				- 1_6
 * usr button 		- 1_9
 * tachometer		- 0_15
 * accelerometer	- 0_23 (adc 0_0)
 */

//-------------------------------------------------------------------------------------------------
#define RED_LED_PORT			1
#define RED_LED_PIN				4
#define GREEN_LED_PORT 			1
#define GREEN_LED_PIN 			7
#define BLUE_LED_PORT			1
#define BLUE_LED_PIN			6
#define TACH_PORT				0
#define TACH_PIN				15
#define BUTTON_PORT				1
#define BUTTON_PIN				9
#define ADC_PORT				0U
#define ADC_PIN					23U
//-------------------------------------------------------------------------------------------------
#define ACCEL_ADC_BUFFER_LENGTH			ACCEL_ADC_SAMPLES_PER_REV + 50
#define ACCEL_ADC_SAMPLES_PER_REV		1028
#define TACHOMETER_POLARITY				1
#define TACHOMETER_GINT					GINT1
#define CTIMER_CLK_FREQ 				CLOCK_GetCTimerClkFreq(2U)
#define CTIMER_MAT_STROBE  				kCTIMER_Match_1
#define CTIMER_MAT_ADC					kCTIMER_Match_2
#define CTIMER_MAT_ANTI_STROBE			kCTIMER_Match_3
#define RPM_CTIMER						CTIMER2
#define TIMER_TICS_PER_SEC				96000000

#define NUMBER_OF_BLADES 				2
#define STROBE_FLASH_DURATION_US		500
//-------------------------------------------------------------------------------------------------
#define MEASUREMENT_AVERAGE_SIZE		60
#define VOLTAGE_AVERAGE_SIZE			1000
//-------------------------------------------------------------------------------------------------
#define ADC_BASE                  			ADC0
#define ADC_IRQn                  			ADC0_IRQn
#define ADC_IRQ_HANDLER_FUNC      			ADC0_IRQHandler
#define ADC_USER_CHANNEL          			0U
#define ADC_USER_CMDID            			1U
#define ADC_VREF_SOURCE           			kLPADC_ReferenceVoltageAlt2
#define ADC_DO_OFFSET_CALIBRATION 			true
#define ADC_USE_HIGH_RESOLUTION   			true
#define ADC_MATCH_VALUE_DEFAULT_INCREMENT 	11700

#define ADC_MAX_VALUE				65535
#define ADC_REF_VOLTAGE				3.3
#define ADC_LSB_RESOLUTION			ADC_REF_VOLTAGE / ADC_MAX_VALUE//#define ADC_DC_BIAS					1.57
#define ACCEL_SENSITIVITY			10.0		//G per volt
#define G_IN_S2						386.0885827
#define PI							3.14159265359
//-------------------------------------------------------------------------------------------------
void init_gpio(unsigned char tach_polarity);
void init_tach_pinint(unsigned char polarity);
void tach_callback(void);
void adc_trigger_callback(void);
void init_rpm_timer(void);
void strobe_callback(void);
void init_adc(void);
void strobe_flash(void);
void anti_strobe_callback(void);

#endif /* AMOSES_TRACK_AND_BALANCE_H_ */
