/*
 * amoses_core_functions.c
 *
 *  Created on: Oct 11, 2023
 *      Author: amoses
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "LPC55S16.h"
#include "fsl_debug_console.h"
#include "fsl_ctimer.h"
#include "fsl_wwdt.h"
#include "fsl_debug_console.h"
#include "fsl_usart.h"
#include "fsl_flexcomm.h"

/* FreeRTOS headers */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

/* Additional includes */
#include "amoses_core_functions.h"
#include "amoses_track_and_balance.h"

//-------------------------------------------------------------------------------------------------
uint8_t display_format = 6;

// Floating point analog variables
float average_reading[2][MEASUREMENT_AVERAGE_SIZE];
float fundamental_ips = 0.0;
float ave_fundamentamental_ips = 0.0;
float ave_max_location_deg = 0.0;
float max_acceleration_g = 0.0;
double max_location_deg = 0.0;
double measurement_length = 0.0;
double pk_to_pk_v = 0.0;
double rms_v = 0.0;
double max_voltage_dc = 0.0;
double max_voltage_ac = 0.0;


// Fixed point Analog variables
uint8_t accel_inverted = 1;
uint8_t buffer_to_search = 0;
uint16_t min_value = 0xFFFF;
uint16_t min_location = 0;
uint16_t max_value = 0;
uint16_t max_location = 0;
uint16_t search_index = 0;
uint16_t pk_to_pk_raw = 0;
uint16_t print_comb = 0;
uint32_t average_adc_raw = 0;


// Rotor RPM and strobe variables
double rotor_rpm;
double rotor_ticks;
float fundamental_freq = 0.0;
uint32_t rotor_match_register_1 = 0;
uint8_t strobe_active = 1;
uint8_t current_number_of_blades = 4;
uint32_t strobe_increment = 0;
//-------------------------------------------------------------------------------------------------
extern uint8_t  	raw_adc_buffer_selector;
extern uint16_t 	raw_adc_buffer[2][ACCEL_ADC_BUFFER_LENGTH];
extern uint16_t 	raw_adc_meaningful_buffer_length[2];
extern uint32_t		rpm_timer_ticks;
extern ctimer_match_config_t strobe_matchconfig;
extern uint8_t 		filter_enabled;
extern uint8_t 		tach_cnt;
//-------------------------------------------------------------------------------------------------
void data_processing_task(void *pvParameters){
	uint32_t ulInterruptStatus = 0;

	while(1){
		xTaskNotifyWaitIndexed( 	0,
									0x00,
									0xFFFFFFFF,
									&ulInterruptStatus,
									portMAX_DELAY );

		data_processing_function();

    	if(tach_cnt == 10){
    		tach_cnt = 0;
    		print_data();
    	}
		taskYIELD();

	}
}
//-------------------------------------------------------------------------------------------------
float convert_deg_to_clock(float input_deg){
	//Generate clock direction from the degree format
	return (input_deg / 30.0);
}
//-------------------------------------------------------------------------------------------------
void print_data(void){
	switch(display_format){
		case 0:
			PRINTF("Page 0: (%.2f IPS @ %.2f) %.1f RPM (%.1fHz)\n\r", 	ave_fundamentamental_ips,
																		ave_max_location_deg,
																		rotor_rpm,
																		rotor_rpm / 60.0);
			break;

		case 1:

			PRINTF("Page 1: (%.2f IPS @ %.2f Deg) %.1f RPM (%.1fHz)\n\r", 	ave_fundamentamental_ips,
																			max_location_deg,
																			rotor_rpm,
																			rotor_rpm / 60.0);
			break;

		case 2:
			PRINTF("Page 2: Max G: %f, DC ave: %fv, AC amp: %fv %.1fHz\n\r", 	max_acceleration_g,
																				average_adc_raw * ADC_LSB_RESOLUTION,
																				max_voltage_ac,
																				rotor_rpm / 60.0);
			break;

		case 3:

			PRINTF("Page 3: %d,%d\n\r", raw_adc_meaningful_buffer_length[0], raw_adc_meaningful_buffer_length[1]);
			break;

		case 4:
			PRINTF("\n\r");
			PRINTF("Page 4: .1Hz, %d Samples, ", (rotor_rpm / 60.0), raw_adc_meaningful_buffer_length[buffer_to_search]);
			print_comb = 0;
			do{
				PRINTF("%d,", raw_adc_buffer[buffer_to_search][print_comb]);
				print_comb++;
			}while(print_comb < ACCEL_ADC_BUFFER_LENGTH);
			break;

		case 5:
			PRINTF("Page 5: %.3f Vpk-pk, %.3f VRMS, %.3f IPS, %.0fDeg, %.1f Hz, %.0fRPM\n\r", 	pk_to_pk_v,
																								rms_v,
																								fundamental_ips,
																								max_location_deg,
																								rotor_rpm / 60,
																								rotor_rpm);
			break;

		case 6:
			PRINTF("Page 6: %.0fRPM << %.3f IPS @ %.1f >>\n\r",		rotor_rpm,
																	ave_fundamentamental_ips,
																	convert_deg_to_clock(ave_max_location_deg));

			break;

		default:
			break;
	}
}
//-------------------------------------------------------------------------------------------------
void data_processing_function(void){
	//Reset the search values
	min_value = 0xFFFF;
	max_value = 0;
	search_index = 0;

	//Determine which buffer we're searching (the one opposite the active recording buffer)
	if(raw_adc_buffer_selector == 1){
		buffer_to_search = 0;
	}else{
		buffer_to_search = 1;
	}

	//Determine the rotor RPM & freq
	rotor_ticks = (double)rpm_timer_ticks;
	fundamental_freq = (TIMER_TICS_PER_SEC/rotor_ticks);
	rotor_rpm =  fundamental_freq * 60.0;

	//Determine the average value of the ADC capture array.
	average_adc_raw = average_raw_adc_values(buffer_to_search, raw_adc_meaningful_buffer_length[buffer_to_search]);

	//Search for the point of maximum and minimum displacement
	do{
		if(raw_adc_buffer[buffer_to_search][search_index] > max_value){
			max_location = search_index;
			max_value = raw_adc_buffer[buffer_to_search][search_index];
		}

		if(raw_adc_buffer[buffer_to_search][search_index] < min_value){
			min_location = search_index;
			min_value = raw_adc_buffer[buffer_to_search][search_index];
		}
		search_index++;
	}while(search_index <= raw_adc_meaningful_buffer_length[buffer_to_search]);

	//Convert the index to degrees.
	measurement_length = (double)raw_adc_meaningful_buffer_length[buffer_to_search];
	max_location_deg = 360.0 * (max_location / measurement_length);

	//Calculate the pk to pk voltage
	pk_to_pk_raw = max_value - min_value;
	pk_to_pk_v = pk_to_pk_raw * ADC_LSB_RESOLUTION;

	//If the filter is enabled, take care of the phase shift and adjust the amplitude.
	if(filter_enabled == 1){
		max_location_deg += FILTER_PHASE_SHIFT_DEG;
		if(max_location_deg >= 360.0){
			max_location_deg -= 360.0;
		}
		if(max_location_deg < 0.0){
			max_location_deg += 360.0;
		}

		pk_to_pk_v *= FILTER_AMP_GAIN_ADJ;
	}

	//Calculate the RMS voltage.
	rms_v = 0.707 * (pk_to_pk_v / 2.0);

	//convert the maximum counts to voltage then to acceleration in G (imperial)
	max_voltage_dc = max_value * ADC_LSB_RESOLUTION;
	max_voltage_ac = max_voltage_dc - (average_adc_raw * ADC_LSB_RESOLUTION);
	max_acceleration_g = (rms_v / 2.0) * ACCEL_SENSITIVITY;

	//Determine the peak IPS
	fundamental_ips = (G_IN_S2 * max_acceleration_g) / (2.0 * PI * fundamental_freq);

	//This adjustment comes from the comparison with the Asti Chadwick
	fundamental_ips /= CHADWICK_ADJ_SCALAR;

	shift_and_generate_averages();
}
//-------------------------------------------------------------------------------------------------
void data_telemetry_function(void){
	print_data();
}
//-------------------------------------------------------------------------------------------------
void kb_input_task(void *pvParameters){
	while(1){
		kb_input_function();
	}
}
//-------------------------------------------------------------------------------------------------
void kb_input_function(void){
	char kb_input;

	kb_input = GETCHAR();
	switch(kb_input){

		case 'F':
		case 'f':
			if(filter_enabled == 1){
				filter_enabled = 0;
				PRINTF("FILTER DISABLED\n\r");
			}else{
				filter_enabled = 1;
				PRINTF("FILTER ENABLED\n\r");
			}
			break;

		case 'D':
		case 'd':
			if(display_format >= MAXIMUM_NUMBER_OF_DISPLAY_FORMATS){
				display_format = 0;
			}else{
				display_format++;
			}
			break;

		case 'S':
		case 's':
			if(strobe_active == 1){
				strobe_active = 0;
				PRINTF("STROBE DISABLED\n\r");
			}else{
				strobe_active = 1;
				PRINTF("STROBE ENABLED\n\r");
			}
			break;

		case 'A':
		case 'a':
			if(accel_inverted == 1){
				accel_inverted = 0;
				PRINTF("ACCELEROMETER NORMAL\n\r");
			}else{
				accel_inverted = 1;
				PRINTF("ACCELEROMETER INVERTED\n\r");
			}
			break;

		case 'B':
		case 'b':
			current_number_of_blades++;
			if(current_number_of_blades > MAX_NUMBER_OF_BLADES){
				current_number_of_blades = 2;
			}
			PRINTF("NUMBER OF BLADES: %d\n\r", current_number_of_blades);
			break;

		default:
			break;
	}
}
//-------------------------------------------------------------------------------------------------
unsigned int average_raw_adc_values(uint8_t buffer_selector, uint16_t search_length){
	unsigned int raw_adc_accumulator = 0;
	uint16_t data_comb = 0;
	do{
		raw_adc_accumulator += raw_adc_buffer[buffer_selector][data_comb];
		data_comb++;
	}while(data_comb < search_length);

	raw_adc_accumulator /= search_length;
	return raw_adc_accumulator;
}
//-------------------------------------------------------------------------------------------------
void shift_and_generate_averages(void){
	uint16_t i = MEASUREMENT_AVERAGE_SIZE - 1;
	//Shift all the values "up"
	do{
		average_reading[0][i] = average_reading[0][i - 1];
		average_reading[1][i] = average_reading[1][i - 1];
		i--;
	}while(i > 0);

	//Null the values at the 0 index
	average_reading[0][0] = fundamental_ips;
	average_reading[1][0] = (float)max_location_deg;

	//Generate the average value
	i = 0;
	ave_fundamentamental_ips = 0;
	ave_max_location_deg = 0;

	do{
		ave_fundamentamental_ips += average_reading[0][i];
		ave_max_location_deg += average_reading[1][i];
		i++;
	}while(i < MEASUREMENT_AVERAGE_SIZE - 1 );

	ave_fundamentamental_ips /= MEASUREMENT_AVERAGE_SIZE;
	ave_max_location_deg /= MEASUREMENT_AVERAGE_SIZE;

}
//-------------------------------------------------------------------------------------------------
void watchdog_kicker_task(void *pvParameters){
	const TickType_t watchdog_kick_delay = WATCHDOG_KICK_DELAY_MS / portTICK_PERIOD_MS;
	while(1){
		WWDT_Refresh(WWDT);
		vTaskDelay(watchdog_kick_delay);
	}
}
//-------------------------------------------------------------------------------------------------
void init_watchdog(void){
    wwdt_config_t config;
    uint32_t wdtFreq;

    if (WWDT_GetStatusFlags(WWDT) & kWWDT_TimeoutFlag){
    	PRINTF("Watchdog reset occurred\r\n");
    }

    /* Enable FRO 1M clock for WWDT module. */
    SYSCON->CLOCK_CTRL |= SYSCON_CLOCK_CTRL_FRO1MHZ_CLK_ENA_MASK;
    /* Set clock divider for WWDT clock source. */
    CLOCK_SetClkDiv(kCLOCK_DivWdtClk, 1U, true);
    wdtFreq = WDT_CLK_FREQ / 4;

    WWDT_ClearStatusFlags(WWDT, kWWDT_TimeoutFlag);
    WWDT_ClearStatusFlags(WWDT, kWWDT_WarningFlag);

    WWDT_GetDefaultConfig(&config);
    config.timeoutValue = wdtFreq * 2;
    config.warningValue = 512;
    config.windowValue  = wdtFreq * 2;
    config.enableWatchdogReset = true;
    config.clockFreq_Hz = WDT_CLK_FREQ;
    WWDT_Init(WWDT, &config);
}
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
