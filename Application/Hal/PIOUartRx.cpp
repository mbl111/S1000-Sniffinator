#include "PIOUartRx.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/uart.h"
#include <memory>
#include "UartRx.pio.h"
#include <cstring>

uint Pacom::HAL::PIOUart::read(uint8_t *buffer, uint length)
{
	cpp_freertos::LockGuard guard(_uartMutex);
	int readLen = _uartQueue.NumItems() < length ? _uartQueue.NumItems() : length;

	for (int i = 0; i < readLen; i++)
	{
		_uartQueue.Dequeue(buffer + i, portMAX_DELAY);
	}

	return readLen;
}

uint32_t Pacom::HAL::PIOUart::available()
{
	cpp_freertos::LockGuard guard(_uartMutex);
	return _uartQueue.NumItems();
}

bool Pacom::HAL::PIOUart::init(uint32_t baudrate, uint8_t dataLength, UartParity parity )
{

	if (parity == Pacom::HAL::UartParity::MARK || parity == Pacom::HAL::UartParity::SPACE)
		return false;

	if (baudrate == _baudrate && dataLength == _dataLength && parity == _parity)
		return false;

	cpp_freertos::LockGuard guard(_uartMutex);

	pio_sm_set_enabled(_pio, _stateMachine, false);
	
	if (_pioProgram != NULL)
		pio_remove_program(_pio, _pioProgram, _pioProgramOffset);

	uint8_t pioIndex = pio_get_index(_pio);
	
	if (_stateMachine == 0)
	{
		pio_set_irq0_source_enabled(_pio, pis_sm0_rx_fifo_not_empty, false);
	}
	else
	{
		pio_set_irq1_source_enabled(_pio, pis_sm1_rx_fifo_not_empty, false);
	}

	_baudrate = baudrate;
	_parity = parity;
	_dataLength = dataLength;

	int inputBitCount = dataLength + (parity == Pacom::HAL::UartParity::NONE ? 0 : 1);
	
	//printf("Total Input bits: %u\n", inputBitCount);

	if (inputBitCount > 16)
	{
		printf("[PIOUART] Unable to Initialize PIOUart. Bit Length %u is longer than 16 (MAX)\n", inputBitCount);
		return false;
	}
	
	

	_pioProgram = getProgram(inputBitCount);
	
	if (_pioProgram == NULL)
	{
		printf("[PIOUART] Unable to get a program!!");
		return false;
	}

	if (!pio_can_add_program(_pio, _pioProgram))
	{
		printf("[PIOUART] Unable to add PIO program!");
		return false;
	}

	_pioProgramOffset = pio_add_program(_pio, _pioProgram);

	printf("[PIOUART] Initializing Program\n");

	uart_rx_program_init(_pio, _stateMachine, _pioProgramOffset, static_cast<int>(_rxPin), _baudrate, inputBitCount);

	pio_sm_clear_fifos(_pio, _stateMachine);

	if (pioIndex == 0)
	{
		if (_stateMachine == 0)
		{
			irq_set_enabled(PIO0_IRQ_0, false);
			irq_remove_handler(PIO0_IRQ_0, _pio0_sm0_datairqh);
			irq_set_exclusive_handler(PIO0_IRQ_0, _pio0_sm0_datairqh);
			irq_set_enabled(PIO0_IRQ_0, true);
		}
		else
		{
			irq_set_enabled(PIO0_IRQ_1, false);
			irq_remove_handler(PIO0_IRQ_1, _pio0_sm1_datairqh);
			irq_set_exclusive_handler(PIO0_IRQ_1, _pio0_sm1_datairqh);
			irq_set_enabled(PIO0_IRQ_1, true);
		}
	}
	else
	{
		if (_stateMachine == 0)
		{
			irq_set_enabled(PIO1_IRQ_0, false);
			irq_remove_handler(PIO1_IRQ_0, _pio1_sm0_datairqh);
			irq_set_exclusive_handler(PIO1_IRQ_0, _pio1_sm0_datairqh);
			irq_set_enabled(PIO1_IRQ_0, true);
		}
		else
		{
			irq_set_enabled(PIO1_IRQ_1, false);
			irq_remove_handler(PIO1_IRQ_1, _pio1_sm1_datairqh);
			irq_set_exclusive_handler(PIO1_IRQ_1, _pio1_sm1_datairqh);
			irq_set_enabled(PIO1_IRQ_1, true);
		}
	}

	printf("[PIOUART] Configured on Pin %u. PIO: %u SM: %u - Baud %lu, Bits %u, Parity %u\n", static_cast<int>(_rxPin), pio_get_index(_pio), _stateMachine, baudrate, dataLength, _parity);

	pio_sm_set_enabled(_pio, _stateMachine, true);

	if (_stateMachine == 0)
	{
		pio_set_irq0_source_enabled(_pio, pis_sm0_rx_fifo_not_empty, true);
	}
	else if
		(_stateMachine == 1)
	{
		pio_set_irq1_source_enabled(_pio, pis_sm1_rx_fifo_not_empty, true);
	}

	return true;
}

//Copied and Modified from the Arduino Library
pio_program_t *Pacom::HAL::PIOUart::getProgram(int bitLength)
{

	auto f = _rxMap.find(bitLength);

	if (f == _rxMap.end())
	{
		pio_program_t *p = new pio_program_t;
		p->length = uart_rx_program.length;
		p->origin = uart_rx_program.origin;
		uint16_t *insn = (uint16_t *)malloc(p->length * 2);
		if (!insn)
		{
			delete p;
			return nullptr;
		}
		std::memcpy(insn, uart_rx_program.instructions, p->length * 2);
		insn[1] = pio_encode_set(pio_x, bitLength - 1);
		p->instructions = insn;
		
		_rxMap.insert({bitLength, p});
		f = _rxMap.find(bitLength);
	}
	else
	{
		printf("Found PIO program for Uart Bitlength %u\n", bitLength);
	}
		return f->second;
}

static bool _calculateParity(int bits, int data)
{
	int p = 0;
	for (int b = 0; b < bits; b++)
	{
		p ^= (data & 1) ? 1 : 0;
		data >>= 1;
	}
	return p;
}

void Pacom::HAL::PIOUart::pullFromPIOFifo()
{
	if (((_pio->ctrl >> _stateMachine) & 0x1) == 0)
		return;

	//Shifting the whole 32 bits across, minus the length ofthe data we want to keep
	uint8_t shiftDistance = (32 - _dataLength);

	//We keep one more bit for the parity calculation
	if (_parity != Pacom::HAL::UartParity::NONE)
		shiftDistance--;

	while (pio_sm_get_rx_fifo_level(_pio, _stateMachine) > 0)
	{
		uint16_t rawInput = pio_sm_get(_pio, _stateMachine) >> shiftDistance;
		uint8_t data = rawInput & 0xFF;
		bool parityValid = _parity == Pacom::HAL::UartParity::NONE;

		if (!parityValid)
		{
			uint8_t dataParity = _calculateParity(_dataLength + 1, rawInput);

			if (_parity == Pacom::HAL::UartParity::ODD)
			{
				parityValid = dataParity == 0x01;
			}
			else if (_parity == Pacom::HAL::UartParity::EVEN)
			{
				parityValid = dataParity == 0x00;
			}
		}

		BaseType_t xHigherPriorityTaskWoken;


		if (parityValid)
		{
			_dataLost = !_uartQueue.EnqueueFromISR(&data, &xHigherPriorityTaskWoken);
		}

		if (((_pio->ctrl >> _stateMachine) & 0x1) == 0)
			return;
	}
}

void __not_in_flash_func(Pacom::HAL::_pio0_sm0_datairqh)(void)
{
	Pacom::HAL::PIOUart::pioUart00->pullFromPIOFifo();
}

void __not_in_flash_func(Pacom::HAL::_pio0_sm1_datairqh)(void) {
	Pacom::HAL::PIOUart::pioUart01->pullFromPIOFifo();
}

void __not_in_flash_func(Pacom::HAL::_pio1_sm0_datairqh)(void) {
	Pacom::HAL::PIOUart::pioUart10->pullFromPIOFifo();
}

void __not_in_flash_func(Pacom::HAL::_pio1_sm1_datairqh)(void) {
	Pacom::HAL::PIOUart::pioUart11->pullFromPIOFifo();
}

std::map<int, pio_program_t *> Pacom::HAL::PIOUart::_rxMap = std::map<int, pio_program_t *>();
Pacom::HAL::PIOUart *Pacom::HAL::PIOUart::pioUart00 = NULL;
Pacom::HAL::PIOUart *Pacom::HAL::PIOUart::pioUart01 = NULL;
Pacom::HAL::PIOUart *Pacom::HAL::PIOUart::pioUart10 = NULL;
Pacom::HAL::PIOUart *Pacom::HAL::PIOUart::pioUart11 = NULL;








