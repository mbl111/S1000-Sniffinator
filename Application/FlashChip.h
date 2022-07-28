#pragma once

#include "pico/stdlib.h"
#include "Mutex.h"

using namespace FreeRTOSCPP;

namespace FlashProgrammer
{
	enum FlashType
	{
		SST39LF010,	//1Mbit Flash
		SST39LF020,	//2Mbit Flash
		SST39LF040,	//4Mbit Flash
	};
	
	class FlashChip
	{
		
		FlashChip();
		void Write(uint8_t* data, uint32_t length, uint32_t startAddress = 0x00);
		void WriteByte(uint32_t address, uint8_t data);
		void ReadByte(uint32_t address, uint8_t data);
		void ReadBytes(uint8_t* dataBuffer, uint32_t length, uint32_t startAddress = 0x00);
	private:
		void readFlashInfo();
		void enterProgrammingMode();
		void sectorErase();
		void chipErase();
		void enterSoftwareIDMode();
		void exitSoftwareIDMode();
		FlashType _flashType;
		uint8_t _manufacturer;
		Mutex _mutex;
	};
}