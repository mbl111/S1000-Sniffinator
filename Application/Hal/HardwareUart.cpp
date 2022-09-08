#include "HardwareUart.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "Applications/Application.h"

Pacom::HAL::HardwareUart *Pacom::HAL::HardwareUart::hwUart0 = new Pacom::HAL::HardwareUart();
Pacom::HAL::HardwareUart *Pacom::HAL::HardwareUart::hwUart1 = new Pacom::HAL::HardwareUart();

void Pacom::HAL::HardwareUart::init(uint8_t uartIndex, uint baudrate)
{ 
	if (_hwUart != NULL)
		return;

	cpp_freertos::LockGuard guard(_uartMutex);

	_uartIndex = uartIndex;
	_baudrate = baudrate;
	_dataLength = 0;
	_parity = Pacom::HAL::UartParity::NONE;
	

	_hwUart = uart_get_instance(_uartIndex);
	uart_init(_hwUart, _baudrate);
	

	if (_uartIndex == 0)
	{
		printf("[HWUART] Enabling HW Uart 0\n");

		gpio_set_function(0, GPIO_FUNC_UART);
		gpio_set_function(1, GPIO_FUNC_UART);
	
		irq_set_exclusive_handler(UART0_IRQ, handler_uartRX0);
		irq_set_enabled(UART0_IRQ, true);
	}
	else
	{
		printf("[HWUART] Enabling HW Uart 1\n");
		gpio_set_function(4, GPIO_FUNC_UART);
		gpio_set_function(5, GPIO_FUNC_UART);
		gpio_set_function(6, GPIO_FUNC_UART);
		gpio_set_function(7, GPIO_FUNC_UART);

		uart_set_hw_flow(uart1, true, true);

		uart_set_fifo_enabled(uart1, false);
		
		irq_set_exclusive_handler(UART1_IRQ, handler_uartRX1);
		irq_set_enabled(UART1_IRQ, true);
	}

	uart_set_irq_enables(_hwUart, true, false);
}

uint Pacom::HAL::HardwareUart::read(uint8_t *buffer, uint length) {

	if (_hwUart == NULL)
		return 0;

	int readLen = _uartQueue.NumItems() < length ? _uartQueue.NumItems() : length;

	for (int i = 0; i < readLen; i++)
	{
		_uartQueue.Dequeue(buffer + i, portMAX_DELAY);
	}

	return readLen;
}

bool Pacom::HAL::HardwareUart::getChar(char &c)
{
	if (_hwUart == NULL)
		return 0;

	return _uartQueue.Dequeue(&c, portMAX_DELAY); 
}


uint32_t Pacom::HAL::HardwareUart::available()
{
	if (_hwUart == NULL)
	{
		printf("[HWUART  ] HW UART NULL");
		return 0;
	}

	return _uartQueue.NumItems();
}


void Pacom::HAL::HardwareUart::writeChar(char c)
{
	if (_hwUart == NULL)
		return;

	cpp_freertos::LockGuard guard(_uartMutex);

	uart_putc(_hwUart, c);
}


void Pacom::HAL::HardwareUart::write(uint8_t* buffer, size_t len)
{
	if (_hwUart == NULL)
	{
		printf("HW UAR NULL!! \n");
		return;
	}

	cpp_freertos::LockGuard guard(_uartMutex);
	
	uart_write_blocking(_hwUart, buffer, len);
}


void Pacom::HAL::HardwareUart::pushReadQueue(char c)
{
	if (_hwUart == NULL)
		return;
	BaseType_t xHigherPriorityTaskWoken;
	_dataLost = !_uartQueue.EnqueueFromISR(&c, &xHigherPriorityTaskWoken);
}

bool Pacom::HAL::HardwareUart::configureUart(uint32_t baudrate, uint8_t dataLength, UartParity parity) {

	if (_hwUart == NULL)
		return false;
	
	if (parity == UartParity::SPACE || parity == UartParity::MARK){
		return false;
	}

	//Don't bother, no change!
	if (_baudrate == baudrate && _dataLength == dataLength && _parity == parity)
		return false;

	cpp_freertos::LockGuard guard(_uartMutex);
	printf("[HWUART  ]Applying Uart configuration. Baud: %lu, Data Length: %u, Parity: %u\n", baudrate, dataLength, parity);
	_baudrate = baudrate;
	_dataLength = dataLength;
	_parity = parity;

	uart_parity_t uartParity = UART_PARITY_NONE;
	
	//The mapping of the USB parity and the hw uart parity isn't 1-1
	if (parity == Pacom::HAL::UartParity::ODD)
		uartParity = UART_PARITY_ODD;

	else if (parity == Pacom::HAL::UartParity::EVEN)
		uartParity = UART_PARITY_EVEN;

	uart_set_format(_hwUart, (uint32_t)dataLength, 1, uartParity);
	uart_set_baudrate(_hwUart, baudrate);

	if (_hwUart == uart1)
	{
		printf("HW 1\n");
		uart_set_hw_flow(_hwUart, true, true);
	}

	return true;
}

void __not_in_flash_func(Pacom::HAL::handler_uartRX0)(void)
{
	if (HardwareUart::hwUart0 == NULL)
		return;

	while (uart_is_readable(uart0))
	{
		uint8_t ch = (uint8_t)uart_get_hw(uart0)->dr;
		HardwareUart::hwUart0->pushReadQueue(ch);
	}
}

void __not_in_flash_func(Pacom::HAL::handler_uartRX1)(void)
{
	if (HardwareUart::hwUart1 == NULL)
		return;

	//printf("UART1 RX IRQ\n");

	while (uart_is_readable(uart1))
	{
		uint16_t ch = (uint16_t)(uart_get_hw(uart1)->dr);
		//uint16_t cht = ch;
		//printf("0x%03x\n", ch & 0xFFF);
		HardwareUart::hwUart1->pushReadQueue((uint8_t)(ch & 0xFF));
	}
}


