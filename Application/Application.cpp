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

using namespace FlashProgrammer;

Application::Application() : Task("Application", 512, 1)
{
	Application::Status = new StatusLED();
}

void Application::Run()
{
	Application::Status->Start();
	
	//We've finished booting
	printf("Waiting for Connection\n");
	Application::Status->SetStatus(StatusLED::WAITING_FOR_CONNECTION);

	
	uart_inst_t* terminalUart = uart_get_instance(0);
	char lastChar;
	
	tusb_init();
	

	printf(">");

	
	char dataBuffer[512] = {};
	uint16_t crc = 0x00;
	char dataLength = 0x00;
	char commandType = 0x00;
	//4 Byte Start code [RPFP]
	//1 Byte Command Type
	//1 Byte Data Length
	//Up to 255 Bytes Data
	//2 Bytes CRC
	char state = 0x00;
	const char PREAMBLE_0 = 0x00;
	const char PREAMBLE_1 = 0x01;
	const char PREAMBLE_2 = 0x02;
	const char PREAMBLE_3 = 0x03;
	const char COMMAND = 0x03;
	const char DATALENGTH = 0x03;
	const char DATA = 0x03;
	const char CRC = 0x03;
	const char ERROR = 0x03;

	for (;;)
	{
		tud_task();
		for (int i = 0; i < 5; i++)
			tud_cdc_n_write_flush(i);

		//Read UART 1 and write to UART 2
		while (tud_cdc_n_available(1))
		{
			char buffer[64];
			uint8_t rec = tud_cdc_n_read(1, buffer, 64);
			tud_cdc_n_write(2, buffer, rec < 64 ? rec : 64);
		}

		while (tud_cdc_available() > 0)
		{
			switch (tud_cdc_n_read_char(0))
			{
			case '0':
				tud_cdc_n_write_str(0, "UART 0");
				break;
			case '1':
				tud_cdc_n_write_str(1, "UART 1");
				break;
			case '2':
				tud_cdc_n_write_str(2, "UART 2");
				break;
			case '3':
				tud_cdc_n_write_str(3, "UART 3");
				break;
			case '4':
				tud_cdc_n_write_str(4, "UART 4");
				break;
			default:
				break;
			}

			tud_cdc_read(&dataBuffer, dataLength);
			//tud_cdc_write_char();
			tud_cdc_write_flush();
		}

		vTaskDelay(10);
	}


}

StatusLED* Application::Status = NULL;

int AppMain()
{
	printf("Starting Application\n");
	Application application;
	application.Start();

	vTaskStartScheduler();

	for (;;);

	return 0;
}