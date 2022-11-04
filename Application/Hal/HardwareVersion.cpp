#include "HardwareVersion.h"

Pacom::HAL::HardwareVersion::HardwareVersion() {
	_versionPins[0] = new GPIO(LowestVersionPinNumber + 2, false);
	_versionPins[1] = new GPIO(LowestVersionPinNumber + 1, false);
	_versionPins[2] = new GPIO(LowestVersionPinNumber + 0, false);
}

void Pacom::HAL::HardwareVersion::initalize() {
	for (GPIO* gpio : _versionPins)
	{
		gpio->setPullup(true);
	}

	_hardwareVersion = 0;

	_hardwareVersion |= _versionPins[0]->get() ? 0x00 : 0x01;
	_hardwareVersion |= _versionPins[1]->get() ? 0x00 : 0x02;
	_hardwareVersion |= _versionPins[2]->get() ? 0x00 : 0x04;

	_hardwareVersion++;
}
