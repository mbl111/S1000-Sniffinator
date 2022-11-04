#include "Hal/GPIO.h"

Pacom::HAL::GPIO::GPIO(uint8_t gpioNumber, bool output) : _gpioNumber(gpioNumber), _output(output){
	gpio_init(_gpioNumber);
	gpio_set_dir(_gpioNumber, _output);
}

void Pacom::HAL::GPIO::set(bool on) {
	gpio_put(_gpioNumber, on);
}

bool Pacom::HAL::GPIO::get() {
	return gpio_get(_gpioNumber);
}

void Pacom::HAL::GPIO::setPullup(bool pullup) {
	if (pullup)
	{
		gpio_pull_up(_gpioNumber);
	}
	else
	{
		gpio_pull_down(_gpioNumber);
	}
}