/*
 * amoses_core_functions.h
 *
 *  Created on: Oct 11, 2023
 *      Author: amoses
 */

#ifndef AMOSES_CORE_FUNCTIONS_H_
#define AMOSES_CORE_FUNCTIONS_H_
//-------------------------------------------------------------------------------------------------
#define processing_task_PRIORITY	4
#define watchdog_task_PRIORITY		1
//-------------------------------------------------------------------------------------------------
#define APP_WDT_IRQn        	WDT_BOD_IRQn
#define APP_WDT_IRQ_HANDLER 	WDT_BOD_IRQHandler
#define WDT_CLK_FREQ 			CLOCK_GetWdtClkFreq()
#define WATCHDOG_KICK_DELAY_MS	100
#define FILTER_PHASE_SHIFT_DEG	-28.0	//17 degrees when testing on the lathe.
#define CHADWICK_ADJ_SCALAR		6.6
#define FILTER_AMP_GAIN_ADJ		1.2
#define MAXIMUM_NUMBER_OF_DISPLAY_FORMATS 6


#define TELEMETRY_USART          USART0
//#define TELEMETRY_USART_CLK_SRC  kCLOCK_Flexcomm0
#define TELEMETRY_USART_CLK_FREQ CLOCK_GetFlexCommClkFreq(0U)
//-------------------------------------------------------------------------------------------------

void tach_led_task(void *pvParameters);
void led_flash_task(void *pvParameters);
void data_processing_task(void *pvParameters);
void strobe_task(void *pvParameters);
void shift_and_generate_averages(void);
void watchdog_kicker_task(void *pvParameters);
void kb_input_task(void *pvParameters);
void init_watchdog(void);
unsigned int average_raw_adc_values(uint8_t buffer_selector, uint16_t search_length);
void data_processing_function(void);
void print_data(void);
void kb_input_function(void);
void data_telemetry_function(void);
float convert_deg_to_clock(float input_deg);
void init_uart0(void);
//-------------------------------------------------------------------------------------------------

#endif /* AMOSES_CORE_FUNCTIONS_H_ */
