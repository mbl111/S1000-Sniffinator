#pragma once
#ifndef H_PACOM_HARDWAREUART
#define H_PACOM_HARDWAREUART


#include "pico/stdio.h"
#include "hardware/uart.h"
#include "PIOUartRx.h"
#include "FreeRTOS-Addons/include/queue.hpp"
#include "FreeRTOS-Addons/include/mutex.hpp"

namespace Pacom
{
	namespace HAL
	{
		class HardwareUart
		{
		  public:
			HardwareUart() : _uartQueue(1024, 1){};
			~HardwareUart();
			void init(uint8_t uartIndex, uint baudrate);
			bool getChar(char &c);
			uint read(uint8_t *buffer, uint length);
			bool configureUart(uint32_t baudrate, uint8_t dataLength, UartParity parity);
			uint32_t available();
			void writeChar(char c);
			void write(uint8_t *buffer, size_t len);
			void pushReadQueue(char c);
			inline void setDataLost() {
				_dataLost = true;
			}
			inline bool dataLost() {
				bool wasLost = _dataLost;
				_dataLost = false;
				return wasLost;
			}

			static Pacom::HAL::HardwareUart *hwUart0;
			static Pacom::HAL::HardwareUart *hwUart1;
			
		  private:
			uart_inst_t *_hwUart = NULL;
			uint _baudrate;
			uint8_t _dataLength;
			UartParity _parity;
			uint8_t _uartIndex;
			bool _dataLost;
			cpp_freertos::MutexStandard _uartMutex;
			cpp_freertos::Queue _uartQueue;
		};


		void handler_uartRX0();
		void handler_uartRX1();
	} // namespace HAL
}// namespace Pacom
#endif // !H_PACOM_HARDWAREUART

