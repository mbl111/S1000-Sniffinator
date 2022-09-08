#include "UARTForwarder.h"
#include "pico/stdlib.h"
#include "tusb.h"
#include <stdio.h>

void Pacom::UartToUSBForwarder::Run()
{

	uint8_t buffer[64];
	uint8_t bufferIndex = 0;
	uint8_t readLength = 64;
	uint8_t writeAvailable = 0;

	printf("[PIOU2USB] Starting PIO Uart forwarder. USBUART: %u\n", static_cast<uint8_t>(_usbSerialIndex));

	for (;;)
	{
		switch (_state)
		{
		case ForwarderState::UNINITIALIZED:
			//Extra delay until setup!
			vTaskDelay(pdMS_TO_TICKS(100));
			break;

		case ForwarderState::CONFIGURE:
			printf("[PIOU2USB] Reconfiguring UART\n");
			_uart->init(_baudrate, _dataLength, _parity);
			setState(Pacom::ForwarderState::FORWARDING);
			break;

		case ForwarderState::FORWARDING:

			uint8_t attempts = 0;

			while (_uart->available() > 0 && attempts < 10)
			{
				attempts++;
				writeAvailable = tud_cdc_n_write_available(static_cast<uint8_t>(_usbSerialIndex));

				readLength = writeAvailable < 64 ? writeAvailable : 64;
				bufferIndex = _uart->read(buffer, readLength);

				tud_cdc_n_write(static_cast<uint8_t>(_usbSerialIndex), buffer, bufferIndex);
				tud_task();
			}

			tud_task();
			tud_cdc_n_write_flush(static_cast<uint8_t>(_usbSerialIndex));
			break;
		}

		vTaskDelay(pdMS_TO_TICKS(10));
	}
}

void Pacom::HWUartToUSBForwarder::Run()
{

	if (_hwUart == NULL)
		return;

	uint8_t buffer[64];
	uint8_t bufferIndex = 0;
	uint8_t readLength = 64;
	uint8_t writeAvailable = 0;

	printf("[HWURTUSB] Starting HW Uart forwarder. USBUART: %u\n", static_cast<uint8_t>(_usbSerialIndex));

	for (;;)
	{

		switch (_state)
		{
		case ForwarderState::UNINITIALIZED:
			vTaskDelay(pdMS_TO_TICKS(100));
			break;

		case ForwarderState::CONFIGURE:
			printf("[HWURTUSB] Reconfiguring UART\n");
			_hwUart->configureUart(_baudrate, _dataLength, _parity);
			setState(Pacom::ForwarderState::FORWARDING);
			break;

		case ForwarderState::FORWARDING:

			uint8_t attempts = 0;

			while (_hwUart->available() > 0 && attempts < 10)
			{
				attempts++;
				
				writeAvailable = tud_cdc_n_write_available(static_cast<uint8_t>(_usbSerialIndex));
				readLength = writeAvailable < 64 ? writeAvailable : 64;
				bufferIndex = _hwUart->read(buffer, readLength);
				tud_cdc_n_write(static_cast<uint8_t>(_usbSerialIndex), buffer, bufferIndex);

				tud_task();
			}

			tud_cdc_n_write_flush(static_cast<uint8_t>(_usbSerialIndex));
			break;
		}

		vTaskDelay(pdMS_TO_TICKS(10));
	}
}

void Pacom::USBToUartForwarder::enableFlowControl(uint8_t rtsPin, uint8_t ctsPin) {

	return;

	if (_rtsPin != nullptr)
		return;

	//uart_set_hw_flow(uart1, true, true);
	_rtsPin = new HAL::GPIO(rtsPin, true);
	_ctsPin = new HAL::GPIO(ctsPin, false);

	//Active Low
	_rtsPin->set(false);
}

void Pacom::USBToUartForwarder::signalRTS() {

	if (_rtsPin == nullptr)
		return;
	
	_rtsPin->set(false);
}

void Pacom::USBToUartForwarder::clearRTS()
{
	if (_rtsPin == nullptr)
		return;

	_rtsPin->set(true);
}
	
void Pacom::USBToUartForwarder::Run()
{

	if (_hwUart == NULL)
		return;

	uint8_t usbSerialIndex = static_cast<uint8_t>(_usbSerialIndex);
	uint8_t buffer[64] = {};

	printf("[USB2UART] Starting USB forwarder. USBUART: %u\n", usbSerialIndex);

	for (;;)
	{
		switch (_state)
		{
		case ForwarderState::UNINITIALIZED:
			setState(Pacom::ForwarderState::FORWARDING);
			vTaskDelay(pdMS_TO_TICKS(100));
			break;

		case ForwarderState::CONFIGURE:
			printf("[USB2UART] Reconfiguring UART\n");
			_hwUart->configureUart(_baudrate, _dataLength, _parity);
			setState(Pacom::ForwarderState::FORWARDING);
			break;

		case ForwarderState::FORWARDING:

			while (tud_cdc_n_available(usbSerialIndex) > 0)
			{
				uint32_t len = tud_cdc_n_read(usbSerialIndex, buffer, 64);

				_hwUart->write(buffer, len);
			}
			break;
		}

		vTaskDelay(pdMS_TO_TICKS(10));
	}
}