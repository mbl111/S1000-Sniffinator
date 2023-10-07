#include "Applications/Console.h"
#include "tusb.h"
#include "pico/stdio/driver.h"
#include "pico/unique_id.h"
#include "hardware/watchdog.h"
#include "ApplicationManager.h"
#include "S1000.h"


#define configSTATUSLED_ERRORDISPLAYPERIOD pdMS_TO_TICKS(1000)
#define configSTATUSLED_COMMANDDISPLAYPERIOD pdMS_TO_TICKS(1000)
#define PICO_STDIO_USB_STDOUT_TIMEOUT_US 500000

stdio_driver_t usbConsoleStdioDriver = {
	.out_chars = Pacom::stdioDriverOut,
	.out_flush = Pacom::stdioDriverFlush,
	.in_chars = Pacom::stdioDriverIn,
};

void Pacom::Console::Run()
{
	//Uncomment to install the driver so that printf goes to our console!
#ifndef SIMPLE_COM_PORTS	
	stdio_set_driver_enabled(&usbConsoleStdioDriver, true);
#endif
	uint8_t consoleUartNumber = static_cast<uint8_t>(_usbSerialIndex);


	char consoleBuffer[64] = {};
	int len = 0;
	bool connected = false;
	bool cursorPromptDisplayed = false;

	while (true)
	{
		if (tud_cdc_n_connected(consoleUartNumber))
		{
			if (!connected)
			{
				connected = true;
				printf("|-----------------------------------------------------|\n");
				printf("|    S1000 Sniffinator. %u.%u.%u | Hardware Revision %u   |\n", 
					   FIRMWARE_VERSION_MAJOR, 
					   FIRMWARE_VERSION_MINOR, 
					   FIRMWARE_VERSION_BUILD, 
					   ApplicationManager::Instance().getHardwareVersion().getHardwareVersion());
				printf("|                 Press 'H' for help!                 |\n");
				printf("|-----------------------------------------------------|\n\n");
			}
		

			while (tud_cdc_n_available(consoleUartNumber) > 0)
			{
				cursorPromptDisplayed = false;
				char inputChar = tud_cdc_n_read_char(consoleUartNumber);
				tud_cdc_n_write_char(consoleUartNumber, inputChar);
				tud_cdc_n_write_char(consoleUartNumber, '\n');
				tud_cdc_n_write_flush(consoleUartNumber);
				ApplicationManager::Instance().getStatusLEDApplication().SetTemporaryStatus(Pacom::StatusLED::COMMAND, configSTATUSLED_COMMANDDISPLAYPERIOD);
				switch (inputChar)
				{
				case 'r':
					printf("[ CONSOLE] Restarting the S1000!\n");
					HAL::S1000::Instance().restart();
					break;
				case 'b':
					printf("[ CONSOLE] Restarting the S1000 into Boot0 mode!\n");
					HAL::S1000::Instance().enterBootloader();
					break;
				case 's':
					char picoUUID[32];
					pico_get_unique_board_id_string(picoUUID, 31);
					printf("[ CONSOLE] Sniffinator UUID: %s\n", picoUUID);
					HAL::S1000::Instance().enterBootloader();
					break;
				case 'l':
					printf("[ CONSOLE] Resetting BLE Chip\n");
					HAL::S1000::Instance().resetBle();
					break;
				case 'x':
					printf("[ CONSOLE] Restarting the Sniffinator!\n\n\n");
					tud_cdc_n_write(consoleUartNumber, consoleBuffer, len);
					tud_cdc_n_write_flush(consoleUartNumber);
					Delay(pdMS_TO_TICKS(100));
					//Force a restart of the pico!
					watchdog_enable(1, 1);
					while (1)
					{
					}
					break;
				case 'h':
					printHelpMessage();
					break;
				default:
					ApplicationManager::Instance().getStatusLEDApplication().SetTemporaryStatus(Pacom::StatusLED::CONSOLE_ERROR, configSTATUSLED_ERRORDISPLAYPERIOD);
					break;
				}
			}

			if (!cursorPromptDisplayed)
			{
				cursorPromptDisplayed = true;
				printf("COMMAND>");
			}
			
			tud_cdc_n_write_flush(consoleUartNumber);
		}
		else
		{
			connected = false;
		}
		
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}


void Pacom::Console::printHelpMessage()
{
	printf("h = Help\n");
	printf("r = Restart S1000\n");
	printf("b = Restart S1000 in Boot0\n");
	printf("x = Restart Sniffinator | h = Help\n");
	printf("l = Reset BLE Chip\n");
	printf("s = Display Sniffinator UUID\n");
}

int Pacom::Console::stdioIn(char *buf, int len) {
	if (!_logLock.Lock(100))
	{
		return 0;
	}
	cpp_freertos::LockGuard guard(_logLock);

	uint8_t usbSerialIndex = static_cast<uint8_t>(_usbSerialIndex);
	
	if (tud_cdc_n_connected(static_cast<uint8_t>(_usbSerialIndex)))
	{
		int rc = PICO_ERROR_NO_DATA;
		if (tud_cdc_n_connected(usbSerialIndex) && tud_cdc_n_available(usbSerialIndex))
		{
			int count = (int)tud_cdc_read(buf, (uint32_t)len);
			rc = count ? count : PICO_ERROR_NO_DATA;
		}
		_logLock.Unlock();

		return rc;
	}
	_logLock.Unlock();
	return 0;
}

void Pacom::Console::stdioOut(const char *buf, int len) {
	//cpp_freertos::LockGuard guard(_logLock);
	if (!_logLock.Lock(100))
	{
		return;
	}
	uint8_t usbSerialIndex = static_cast<uint8_t>(_usbSerialIndex);
	static uint64_t last_avail_time;

	if (tud_cdc_n_connected(usbSerialIndex))
	{
		for (int i = 0; i < len;)
		{
			int n = len - i;
			int avail = (int)tud_cdc_n_write_available(usbSerialIndex);
			if (n > avail)
				n = avail;
			if (n)
			{
				int n2 = (int)tud_cdc_n_write(usbSerialIndex, buf + i, (uint32_t)n);
				tud_task();
				tud_cdc_write_flush();
				i += n2;
				last_avail_time = time_us_64();
			}
			else
			{
				tud_task();
				tud_cdc_write_flush();
				if (!tud_cdc_n_connected(usbSerialIndex) ||
					(!tud_cdc_n_write_available(usbSerialIndex) && time_us_64() > last_avail_time + PICO_STDIO_USB_STDOUT_TIMEOUT_US))
				{
					break;
				}
			}
		}
	}
	_logLock.Unlock();
}

void Pacom::Console::stdioFlush(void) {
	//cpp_freertos::LockGuard guard(_logLock);

	if (!_logLock.Lock(100))
	{
		return;
	}
	
	if (tud_cdc_n_connected(static_cast<uint8_t>(_usbSerialIndex)))
	{
		tud_cdc_n_write_flush(static_cast<uint8_t>(_usbSerialIndex));
	}

	_logLock.Unlock();
}

//Exposing the console to the C driver



void Pacom::stdioDriverOut(const char *buf, int len)
{
	ApplicationManager::Instance().getConsoleApplication().stdioOut(buf, len);
}

void Pacom::stdioDriverFlush()
{
	ApplicationManager::Instance().getConsoleApplication().stdioFlush();
}

int Pacom::stdioDriverIn(char *buf, int len) {

	return ApplicationManager::Instance().getConsoleApplication().stdioIn(buf, len);
	
}
