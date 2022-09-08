#pragma once

#include "FreeRTOS-Addons/include/thread.hpp"
#include "PIOUartRx.h"
#include "HardwareUart.h"
#include "Hal/GPIO.h"

namespace Pacom
{

	//Maps the uart usage to a USB Uart number
	enum class USBUART
	{
		DEBUGLOG = 0,
		BLEMODULE,
		ISO_TX,
		ISO_RX,
		NONISO_TX,
		NONISO_RX,
		CONSOLE,
	};

	enum class ForwarderState
	{
		UNINITIALIZED = 0,
		FORWARDING,
		CONFIGURE,
	};

	class UartForwarder : public cpp_freertos::Thread
	{
	  public:
		UartForwarder(std::string name, uint16_t stackSize, uint8_t priority) : Thread(name, stackSize, priority), _state(ForwarderState::UNINITIALIZED){};
		void configureUart(uint32_t baudrate, uint8_t dataLength, HAL::UartParity parity) {
			if (_dataLength == dataLength && _baudrate == baudrate && _parity == parity)
				return;
			_dataLength = dataLength;
			_baudrate = baudrate;
			_parity = parity;
			setState(Pacom::ForwarderState::CONFIGURE);
		}

		inline uint8_t getDataLength() {
			return _dataLength;
		}

		inline uint32_t getBaudRate() {
			return _baudrate;
		}

		inline HAL::UartParity getParity() {
			return _parity;
		}

	  protected:
		
		void setState(ForwarderState newState) { 
			printf("[%s] State Change: %u -> %u\n", GetName().c_str(),  _state, newState);
			_state = newState;
		};
		
		ForwarderState _state;
		uint8_t _dataLength = 0;
		uint32_t _baudrate = 0;
		HAL::UartParity _parity = HAL::UartParity::SPACE;
	};

	class UartToUSBForwarder : public UartForwarder
	{

	  public:
		UartToUSBForwarder(HAL::PIOUart *uart, USBUART usbSerialIndex) : UartForwarder("PIOUART", 2048, 10), _uart(uart), _usbSerialIndex(usbSerialIndex){};
		~UartToUSBForwarder(){};
		void Run();

	  private:
		HAL::PIOUart *_uart;
		USBUART _usbSerialIndex;
	};

	class HWUartToUSBForwarder : public UartForwarder
	{
	  public:
		HWUartToUSBForwarder(HAL::HardwareUart *hwUart, USBUART usbSerialIndex) : UartForwarder("HWUART", 2048, 10), _hwUart(hwUart), _usbSerialIndex(usbSerialIndex){};
		~HWUartToUSBForwarder(){};
		void Run();

	  private:
		HAL::HardwareUart *_hwUart;
		USBUART _usbSerialIndex;
	};

	class USBToUartForwarder : public UartForwarder
	{

	  public:
		USBToUartForwarder(HAL::HardwareUart *hwUart, USBUART usbSerialIndex) : UartForwarder("USBUART", 2048, 10), _hwUart(hwUart), _usbSerialIndex(usbSerialIndex){};
		~USBToUartForwarder(){};
		void Run();
		void enableFlowControl(uint8_t rtsPin, uint8_t ctsPin);
		void signalRTS();
		void clearRTS();

	  private:
		HAL::HardwareUart* _hwUart;
		USBUART _usbSerialIndex;
		HAL::GPIO *_rtsPin = nullptr;
		HAL::GPIO *_ctsPin = nullptr;
	};
	
}