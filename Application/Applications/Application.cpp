/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "Application.h"
#include "FreeRTOS.h"
#include "task.h"
#include "StatusLED.h"
#include "pico/stdlib.h"
#include "tusb.h"
#include "hardware/clocks.h"
#include "Hal/PIOUartRx.h"
#include "Hal/HardwareUart.h"
#include "ApplicationManager.h"
#include "Hal/S1000.h"


using namespace Pacom;

Application::Application() : Thread("Application", 512, 20)
{

}

void Application::Run()
{

	printf("[APPLICAT] Initializing Application Manager\n");
	ApplicationManager::Instance().Initialize();

	printf("[APPLICAT] Initializing USB Connection\n");
	tusb_init();

	printf("[APPLICAT] Starting onboard applications\n");
	ApplicationManager::Instance().getStatusLEDApplication().Start();
	ApplicationManager::Instance().getConsoleApplication().Start();
	vTaskDelay(pdMS_TO_TICKS(100));

	printf("[APPLICAT] Starting Hardware Uart Applications\n");
	ApplicationManager::Instance().getLogUartOutApplication().Start();
	ApplicationManager::Instance().getLogUartInApplication().Start();
	
	ApplicationManager::Instance().getBleUartOutApplication().Start();
	ApplicationManager::Instance().getBleUartInApplication().Start();
	ApplicationManager::Instance().getBleUartInApplication().enableFlowControl(7, 6);

	printf("[APPLICAT] Starting PIO Uart Applications\n");
	ApplicationManager::Instance().getNonIso485RXApplication().Start();
	ApplicationManager::Instance().getNonIso485TXApplication().Start();
	ApplicationManager::Instance().getIso485RXApplication().Start();
	ApplicationManager::Instance().getIso485TXApplication().Start();

	printf("[APPLICAT] System Clockrate: %u\n", clock_get_hz(clk_sys));

	ApplicationManager::Instance().getStatusLEDApplication().SetStatus(StatusLED::CONNECTED);

	//We've finished booting.

	for (;;)
	{
		tud_task();
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}


int AppMain()
{
	printf("\n\n\n-------------------------------\n\n");
	printf("[MAINBOOT] Starting Application\n");
	Application application;
	application.Start();
	
	vTaskStartScheduler();

	for (;;)
	{
		
		vTaskDelay(pdMS_TO_TICKS(1000));
	}

	return 0;
}

TU_ATTR_WEAK void __not_in_flash_func(tud_cdc_line_state_cb)(uint8_t itf, bool dtr, bool rts)
{
	printf("[USBCALBK] Changed State on ITF %i. DTR: %i, RTS: %i\n", itf, dtr ? 1 : 0, rts ? 1 : 0);
	switch (itf)
	{
	
		case static_cast<int>(USBUART::DEBUGLOG):
			if (rts && !dtr)
			{
				//Only if we are very specifically configured to talk to the STM bootloader
				if (ApplicationManager::Instance().getLogUartOutApplication().getBaudRate() == 115200 &&
					ApplicationManager::Instance().getLogUartOutApplication().getParity() == HAL::UartParity::EVEN &&
					ApplicationManager::Instance().getLogUartOutApplication().getDataLength() == 8)
				{
					printf("[USBCALBK] Entering S1000 Bootloader due to RTS signal.\n");
					HAL::S1000::Instance().enterBootloader();
				}
			}
			break;
		case static_cast<int>(USBUART::BLEMODULE):
			break;
			if (rts)
				ApplicationManager::Instance().getBleUartInApplication().signalRTS();
			else
				ApplicationManager::Instance().getBleUartInApplication().clearRTS();
			
			break;
	}
}

TU_ATTR_WEAK void __not_in_flash_func(tud_cdc_line_coding_cb)(uint8_t itf, cdc_line_coding_t const *p_line_coding) {
	//tud_task();

	printf("[USBCALBK] Line Coding Changed. ITF: %u, Baud: %lu, Data Length: %u, Parity: %u, Stop Bits: %u\n", itf, p_line_coding->bit_rate, p_line_coding->data_bits, p_line_coding->parity, p_line_coding->stop_bits);
	HAL::UartParity uartParity = static_cast<Pacom::HAL::UartParity>(p_line_coding->parity);

	switch (itf)
		{
			case static_cast<int>(USBUART::ISO_RX):
				ApplicationManager::Instance().getIso485RXApplication().configureUart(p_line_coding->bit_rate, p_line_coding->data_bits, uartParity);
				break;
			case static_cast<int>(USBUART::ISO_TX):
				ApplicationManager::Instance().getIso485TXApplication().configureUart(p_line_coding->bit_rate, p_line_coding->data_bits, uartParity);
				break;
			case static_cast<int>(USBUART::NONISO_RX):
				ApplicationManager::Instance().getNonIso485RXApplication().configureUart(p_line_coding->bit_rate, p_line_coding->data_bits, uartParity);
				break;
			case static_cast<int>(USBUART::NONISO_TX):
				ApplicationManager::Instance().getNonIso485TXApplication().configureUart(p_line_coding->bit_rate, p_line_coding->data_bits, uartParity);
				break;
			case static_cast<int>(USBUART::DEBUGLOG):
				ApplicationManager::Instance().getLogUartOutApplication().configureUart(p_line_coding->bit_rate, p_line_coding->data_bits, uartParity);
				break;
			case static_cast<int>(USBUART::BLEMODULE):
				ApplicationManager::Instance().getBleUartOutApplication().configureUart(p_line_coding->bit_rate, p_line_coding->data_bits, uartParity);
				HAL::S1000::Instance().resetBle();
				break;
		}
	}
