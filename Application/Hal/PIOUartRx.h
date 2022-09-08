#pragma once

#include <stdio.h>
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "UartRx.pio.h"
#include "FreeRTOS-Addons/include/queue.hpp"
#include "FreeRTOS-Addons/include/mutex.hpp"
#include <map>
#include <memory>

#define UART_DMA_BUFFER_SIZE 1024

namespace Pacom {
	namespace HAL {

		enum class PIOUartPin
		{
			DEBUGLOG = 1,
			ISO_RX = 11,
			ISO_TX = 12,
			NONISO_TX = 13,
			NONISO_RX = 14,
		};

		enum class UartParity
		{
			NONE,
			ODD,
			EVEN,
			MARK,
			SPACE
			
		};

		class PIOUart
		{

		  public:
			PIOUart(PIO pio, uint8_t stateMachine, PIOUartPin rxPin) : _pio(pio), _stateMachine(stateMachine), _rxPin(rxPin), _uartQueue(UART_DMA_BUFFER_SIZE, 2), _pioProgram(NULL){};
			~PIOUart(){};

		 bool init(uint32_t baudRate = 115200, uint8_t dataLength = 8, UartParity parity = UartParity::NONE);
		 uint read(uint8_t *buffer, uint length);
		 uint32_t available();
		 void pullFromPIOFifo();

		 inline bool dataLost()
		 {
			 bool wasLost = _dataLost;
			 _dataLost = false;
			 return wasLost;
		 }
		 
		 static PIOUart *pioUart00; 
		 static PIOUart *pioUart01;
		 static PIOUart *pioUart10;
		 static PIOUart *pioUart11;

	   protected:
		 static std::map<int, pio_program_t*>  _rxMap;
		 static pio_program_t* getProgram(int bitLength);	//Bit Lenght = DATA BITS + STOP BIT(S) + PARITY bits.

		 cpp_freertos::Queue _uartQueue;
		 cpp_freertos::MutexStandard _uartMutex;
		 bool _dataLost;
		 uint _pioProgramOffset;
		 pio_program_t* _pioProgram;

		 inline void setDataLost()
		 {
			 _dataLost = true;
		 }

	   private:
		 uint32_t _baudrate;
		 UartParity _parity;
		 uint8_t _dataLength;
		 uint8_t _stateMachine;
		 PIOUartPin _rxPin;
		 PIO _pio;
		 
		};

		void _pio0_sm0_datairqh();
		void _pio0_sm1_datairqh();
		void _pio1_sm0_datairqh();
		void _pio1_sm1_datairqh();
	}//namespace HAL
}//namespace Pacom