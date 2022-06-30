
/* Header --------------------------------------------------------------------*/

/*
 * @file thisFile.c
 * @brief This file provides code for...
 */

/* Includes ------------------------------------------------------------------*/

#include "Flash/flash.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/*
 * @brief No definition available
 */
struct flashManager_t _flashManager;

/* External variables --------------------------------------------------------*/

/* Function declarations -----------------------------------------------------*/

/*
 * @brief No definition available
 */
void flash_init()
{
	_flashManager.lastFlashOpResult = MCU_FLASH_OP_SUCCESSFULL;
	_flashManager.lockStatus = MCU_FLASH_LOCKED;

	_flashManager.readWriteIsRunning = false;
	_flashManager.nextReadWriteAddr = FLASH_BASE;
	_flashManager.readWriteByteCounter = 0;
}

/*
 * @brief No definition available
 */
mcuFlashOperationResults_t flash_erasePages(uint32_t startPageAddr, uint32_t pagesEraseCount)
{
#ifdef FLASH_TYPEERASE_PAGES

	uint32_t eraseErrReport;
	FLASH_EraseInitTypeDef eraseInit;

	HAL_FLASH_Unlock();

	eraseInit.NbPages = pagesEraseCount;
	eraseInit.PageAddress = startPageAddr;
	eraseInit.TypeErase = FLASH_TYPEERASE_PAGES;

	HAL_FLASHEx_Erase(&eraseInit, &eraseErrReport);

	if(eraseErrReport != 0xFFFFFFFF)
	{
		HAL_FLASH_Lock();
		return MCU_FLASH_OP_ERR;
	}

	HAL_FLASH_Lock();
	return MCU_FLASH_OP_SUCCESSFULL;

#endif
}

/*
 * @brief No definition available
 */
void flash_unlock()
{
	HAL_FLASH_Unlock();
}

/*
 * @brief No definition available
 */
void flash_lock()
{
	HAL_FLASH_Lock();
}

/*
 * @brief No definition available
 */
void flash_read(uint32_t flashReadStartAddr, uint32_t lengthToRead, uint8_t *pDst)
{
	if(!_flashManager.readWriteIsRunning)
	{
		_flashManager.readWriteIsRunning = true;
		_flashManager.nextReadWriteAddr = flashReadStartAddr;
		_flashManager.readWriteByteCounter = 0;
	}

	uint32_t lengthRemainingToRead = lengthToRead - _flashManager.readWriteByteCounter;

	if(lengthRemainingToRead >= 8)
	{
		uint64_t val = *(uint64_t*)_flashManager.nextReadWriteAddr;
		*(uint64_t*)pDst = val;
		_flashManager.nextReadWriteAddr += 8;
		_flashManager.readWriteByteCounter += 8;
	}
	else if(lengthRemainingToRead >= 4)
	{
		uint32_t val = *(uint32_t*)_flashManager.nextReadWriteAddr;
		*(uint32_t*)pDst = val;
		_flashManager.nextReadWriteAddr += 4;
		_flashManager.readWriteByteCounter += 4;
	}
	else if(lengthRemainingToRead >= 2)
	{
		uint16_t val = *(uint16_t*)_flashManager.nextReadWriteAddr;
		*(uint16_t*)pDst = val;
		_flashManager.nextReadWriteAddr += 2;
		_flashManager.readWriteByteCounter += 2;
	}
	else if(lengthRemainingToRead >= 1)
	{
		uint8_t val = *(uint8_t*)_flashManager.nextReadWriteAddr;
		*(uint8_t*)pDst = val;
		_flashManager.nextReadWriteAddr += 1;
		_flashManager.readWriteByteCounter += 1;
	}
	else
	{
		_flashManager.readWriteIsRunning = false;
		return;
	}

	flash_read(flashReadStartAddr, lengthToRead, pDst);
}

/*
 * @brief No definition available
 */
void flash_unlockAndPrepareForNextReadWriteOperations(uint32_t flashReadWriteStartAddr)
{
	_flashManager.readWriteByteCounter = 0;
	_flashManager.nextReadWriteAddr = flashReadWriteStartAddr;

	flash_unlock();
}

/*
 * @brief No definition available
 */
mcuFlashOperationResults_t flash_nextWrite(void *pData, uint8_t size)
{
	HAL_StatusTypeDef status = HAL_ERROR;

	_flashManager.readWriteIsRunning = true;

	switch(size)
	{
	case 2:

		status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, _flashManager.nextReadWriteAddr, *(uint64_t*)pData);
		break;

	case 4:

		status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, _flashManager.nextReadWriteAddr, *(uint64_t*)pData);
		break;

	case 8:

		status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, _flashManager.nextReadWriteAddr, *(uint64_t*)pData);
		break;
	}

	if(status == HAL_OK)
	{
		_flashManager.readWriteIsRunning = false;
		_flashManager.nextReadWriteAddr += size;
		_flashManager.readWriteByteCounter += size;
		return MCU_FLASH_OP_SUCCESSFULL;
	}

	_flashManager.readWriteIsRunning = false;

	return MCU_FLASH_OP_ERR;
}

/*
 * @brief No definition available
 */
void flash_jumpToAddr(uint32_t jumpAddr)
{
	uint32_t  JumpAddress = *(__IO uint32_t*)(jumpAddr + 4);

	typedef void (*pFunction)(void);
	pFunction Jump = (pFunction)JumpAddress;

	__set_MSP(*(__IO uint32_t*)jumpAddr);
	Jump();
}
