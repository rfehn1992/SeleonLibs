
#ifndef SRC_SELEONLIBS_FLASH_FLASH_H_
#define SRC_SELEONLIBS_FLASH_FLASH_H_

/* Header --------------------------------------------------------------------*/

/*
 * @file flash.h
 * @brief Header for flash.c file. <br>
 */

/* Includes ------------------------------------------------------------------*/

#include "../libConfig.h"
#include "stdbool.h"

/* Defines -------------------------------------------------------------------*/

/* Type and struct defines ---------------------------------------------------*/

/*
 * @brief No definition available
 */
typedef enum
{
	MCU_FLASH_OP_SUCCESSFULL							= 0,
	MCU_FLASH_OP_ERR

}mcuFlashOperationResults_t;

/*
 * @brief No definition available
 */
typedef enum
{
	MCU_FLASH_LOCKED									= 0,
	MCU_FLASH_UNLOCKED

}mcuFlashLockStatus_t;

/*
 * @brief No definition available
 */
struct flashManager_t
{
	mcuFlashOperationResults_t lastFlashOpResult;
	mcuFlashLockStatus_t lockStatus;

	bool readWriteIsRunning;
	uint32_t nextReadWriteAddr;
	uint32_t readWriteByteCounter;
};

/* Function declarations -----------------------------------------------------*/

void flash_init();

mcuFlashOperationResults_t flash_erasePages(uint32_t startPageAddr, uint32_t pagesEraseCount);

void flash_unlock();
void flash_lock();
void flash_read(uint32_t flashReadStartAddr, uint32_t lengthToRead, uint8_t *pDst);
void flash_unlockAndPrepareForNextReadWriteOperations(uint32_t flashReadWriteStartAddr);
mcuFlashOperationResults_t flash_nextWrite(void *pData, uint8_t size);

void flash_jumpToAddr(uint32_t jumpAddr);

#endif /* SRC_SELEONLIBS_FLASH_FLASH_H_ */
