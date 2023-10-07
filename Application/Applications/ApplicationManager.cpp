#include "ApplicationManager.h"

Pacom::ApplicationManager *Pacom::ApplicationManager::_instance = nullptr;

Pacom::ApplicationManager &Pacom::ApplicationManager::Instance() {
	if (_instance == nullptr)
	{
		_instance = new ApplicationManager();
	}
	return *_instance;
}

void Pacom::ApplicationManager::Initialize()
{

	_hardwareVersion = new HAL::HardwareVersion();
	_hardwareVersion->initalize();

	_statusLED = new StatusLED();
	
	HAL::HardwareUart::hwUart0->init(0, 115200);
	_logUart = new HWUartToUSBForwarder(HAL::HardwareUart::hwUart0, USBUART::DEBUGLOG);
	_logUartIn = new USBToUartForwarder(HAL::HardwareUart::hwUart0, USBUART::DEBUGLOG);
	
#ifndef SIMPLE_COM_PORTS

	if (_hardwareVersion->hasBLESupport())
		HAL::HardwareUart::hwUart1->init(1, 115200);

	HAL::PIOUart::pioUart00 = new HAL::PIOUart((PIO)pio0, 0, HAL::PIOUartPin::ISO_TX);
	HAL::PIOUart::pioUart01 = new HAL::PIOUart((PIO)pio0, 1, HAL::PIOUartPin::ISO_RX);

	HAL::PIOUart::pioUart10 = new HAL::PIOUart((PIO)pio1, 0, HAL::PIOUartPin::NONISO_TX);
	HAL::PIOUart::pioUart11 = new HAL::PIOUart((PIO)pio1, 1, HAL::PIOUartPin::NONISO_RX);

	//Application::logUart = new UartToUSBForwarder(new HAL::PIOUart((PIO)pio0, 0, 115200, HAL::PIOUartPin::DEBUGLOG), USBUART::DEBUGLOG);
	_Iso485TX = new UartToUSBForwarder(HAL::PIOUart::pioUart00, USBUART::ISO_TX);
	_Iso485RX = new UartToUSBForwarder(HAL::PIOUart::pioUart01, USBUART::ISO_RX);
	_NonIso485TX = new UartToUSBForwarder(HAL::PIOUart::pioUart10, USBUART::NONISO_TX);
	_NonIso485RX = new UartToUSBForwarder(HAL::PIOUart::pioUart11, USBUART::NONISO_RX);
	_console = new Console(USBUART::CONSOLE);

	if (_hardwareVersion->hasBLESupport())
	{
		_bleUart = new HWUartToUSBForwarder(HAL::HardwareUart::hwUart1, USBUART::BLEMODULE);
		_bleUartIn = new USBToUartForwarder(HAL::HardwareUart::hwUart1, USBUART::BLEMODULE);
	}
	
#endif
}
