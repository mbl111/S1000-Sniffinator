#pragma once

#include <array>
#include "GPIO.h"

namespace Pacom
{
	namespace HAL
	{
		class HardwareVersion
		{
		  public:
			HardwareVersion();
			void initalize();
			uint8_t getHardwareVersion() { return _hardwareVersion; };

			bool hasBLESupport(){
				return _hardwareVersion == 0;
			};

		  private:
			static const uint8_t VersionPinCount = 3;
			static const uint8_t LowestVersionPinNumber = 19;
			std::array<GPIO *, VersionPinCount> _versionPins;
			uint8_t _hardwareVersion = 255;
		};
	} // namespace HAL
}