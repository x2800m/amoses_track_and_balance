/*
 * Copyright 2016-2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file    amoses_initial_husk.c
 * @brief   Application entry point.
 */
/* C headers */
#include <stdio.h>

/* chip drivers */
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "LPC55S16.h"
#include "fsl_debug_console.h"
#include "fsl_power.h"
#include "fsl_gint.h"

/* FreeRTOS headers */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

/* application specific files */
#include "amoses_track_and_balance.h"
#include "amoses_core_functions.h"
/* Task handles */
TaskHandle_t 	data_processing_task_handle,
				watchdog_kicker_task_handle,
				kb_input_task_handle;

//-------------------------------------------------------------------------------------------------
/*
 * 				MAIN
 */
//-------------------------------------------------------------------------------------------------
int main(void) {
	POWER_SetBodVbatLevel(kPOWER_BodVbatLevel1650mv, kPOWER_BodHystLevel50mv, false);
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
    BOARD_InitDebugConsole();

    init_gpio();
    init_tach_pinint(TACHOMETER_POLARITY);
    init_rpm_timer();
    init_adc();
    init_watchdog();

    PRINTF("Amoses Track and Balance Init v0.1\n\r");
    PRINTF("S/N: FK-HU-NE-WEL-01\n\r");

    if(xTaskCreate(data_processing_task, "data_processing_task", configMINIMAL_STACK_SIZE + 1000, NULL, processing_task_PRIORITY, &data_processing_task_handle) != pdPASS){
    	PRINTF("data processing task creation failed! \n\r");
    }

    if(xTaskCreate(watchdog_kicker_task, "watchdog_kicker", configMINIMAL_STACK_SIZE + 100, NULL, watchdog_task_PRIORITY, &watchdog_kicker_task_handle) != pdPASS){
    	PRINTF("watchdog kicker task creation failed! \n\r");
    }

    if(xTaskCreate(kb_input_task, "kb_input", configMINIMAL_STACK_SIZE + 100, NULL, watchdog_task_PRIORITY, &kb_input_task_handle) != pdPASS){
    	PRINTF("kb input task creation failed! \n\r");
    }

    vTaskStartScheduler();
   	//If you get here... something is wrong. Don't get here.

    return 0;
}
