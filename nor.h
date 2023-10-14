/*
 * nor.h
 *
 *  Created on: 5 de out de 2023
 *      Author: Pablo Jean
 *
 *      youtube:
 *      Github:
 *      Udemy:
 */

#ifndef FLASH_NOR_NOR_H_
#define FLASH_NOR_NOR_H_

/**
 * Includes
 */

#include <stdint.h>
#include <stdint.h>
#include <stddef.h>

#include "nor_defines.h"
#include "nor_ids.h"

/**
 * Macros
 */

// Flag to tell to the library "Hey, I'm initialized"
#define NOR_INITIALIZED_FLAG		0xCFFE




/**
 * Enumerates
 */

typedef enum{
	NOR_IN_IDLE,
	NOR_DEEP_POWER_DOWN
}nor_pd_e;

typedef enum{
	NOR_OK,
	NOR_FAIL,
	NOR_INVALID_PARAMS,
	NOR_NO_MEMORY_FOUND,
	NOR_UNKNOWN_DEVICE,
	NOR_OUT_OF_RANGE,
	NOR_NOT_INITIALIZED,
	NOR_REGIONS_IS_NOT_EMPTY,
	NOR_IS_LOCKED,

	NOR_UNKNOWN = 0xFF
}nor_err_e;

typedef enum{
	NOR_ERASE_4K,
	NOR_ERASE_32K,
	NOR_ERASE_64K
}nor_erase_method_e;

/**
 * Function Typedefs
 */

typedef void (*SpiTx_fxn_t)(uint8_t* TxBuff, uint32_t len);
typedef void (*SpiRx_fxn_t)(uint8_t* RxBuff, uint32_t len);
typedef void (*CS_Assert_fnx_t)(void);
typedef void (*CS_Deassert_fxn_t)(void);
typedef void (*delay_us_fxn_t)(uint32_t us);

/**
 * Structs
 *
 */

typedef struct{
	struct{
		// TODO : Document and explain all these functions
		SpiTx_fxn_t SpiTxFxn;
		SpiRx_fxn_t SpiRxFxn;
		CS_Assert_fnx_t CsAssert;
		CS_Deassert_fxn_t CsDeassert;
		delay_us_fxn_t DelayUs;
	}config;
	struct{
		uint64_t u64UniqueId;
		uint32_t u32JedecID;
		uint32_t u32Size;
		uint16_t u16PageSize;
		uint32_t u32PageCount;
		uint16_t u16SectorSize;
		uint32_t u32SectorCount;
		uint32_t u32BlockSize;
		uint32_t u32BlockCount;
	}info;
	struct{
		uint16_t u16Initialized;
		uint8_t u8StatusReg1;
		uint8_t u8StatusReg2;
		uint8_t u8StatusReg3;
		uint8_t u8PdCount;
	}_internal;
	nor_manuf_e Manufacturer;
	nor_model_e Model;
	nor_pd_e pdState;
}nor_t;

/**
 * Publics
 */

nor_err_e NOR_Init(nor_t *nor);
nor_err_e NOR_Init_wo_ID(nor_t *nor);

nor_err_e NOR_EnterPowerDown(nor_t *nor);
nor_err_e NOR_ExitPowerDown(nor_t *nor);

nor_err_e NOR_EraseChip(nor_t *nor);
nor_err_e NOR_EraseAddress(nor_t *nor, uint32_t Address, nor_erase_method_e method);
nor_err_e NOR_EraseSector(nor_t *nor, uint32_t SectorAddr);
nor_err_e NOR_EraseBlock(nor_t *nor, uint32_t BlockAddr);

uint32_t NOR_PageToSector(nor_t *nor, uint32_t PageAddr);
uint32_t NOR_PageToBlock(nor_t *nor, uint32_t PageAddr);
uint32_t NOR_SectorToBlock(nor_t *nor, uint32_t SectorAddr);
uint32_t NOR_SectorToPage(nor_t *nor, uint32_t SectorAddr);
uint32_t NOR_BlockToPage(nor_t *nor, uint32_t BlockAddr);

nor_err_e NOR_IsEmptyAddress(nor_t *nor, uint32_t Address, uint32_t NumBytesToCheck);
nor_err_e NOR_IsEmptyPage(nor_t *nor, uint32_t PageAddr, uint32_t Offset, uint32_t NumBytesToCheck);
nor_err_e NOR_IsEmptySector(nor_t *nor, uint32_t SectorAddr, uint32_t Offset, uint32_t NumBytesToCheck);
nor_err_e NOR_IsEmptyBlock(nor_t *nor, uint32_t BlockAddr, uint32_t Offset, uint32_t NumBytesToCheck);

nor_err_e NOR_WriteBytes(nor_t *nor, uint8_t *pBuffer, uint32_t WriteAddr, uint32_t NumBytesToWrite);
nor_err_e NOR_WritePage(nor_t *nor, uint8_t *pBuffer, uint32_t PageAddr, uint32_t Offset, uint32_t NumBytesToWrite);
nor_err_e NOR_WriteSector(nor_t *nor, uint8_t *pBuffer, uint32_t SectorAddr, uint32_t Offset, uint32_t NumBytesToWrite);
nor_err_e NOR_WriteBlock(nor_t *nor, uint8_t *pBuffer, uint32_t BlockAddr, uint32_t Offset, uint32_t NumBytesToWrite);

nor_err_e NOR_ReadBytes(nor_t *nor, uint8_t *pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead);
nor_err_e NOR_ReadPage(nor_t *nor, uint8_t *pBuffer, uint32_t PageAddr, uint32_t Offset, uint32_t NumByteToRead);
nor_err_e NOR_ReadSector(nor_t *nor, uint8_t *pBuffer, uint32_t SectorAddr, uint32_t Offset, uint32_t NumByteToRead);
nor_err_e NOR_ReadBlock(nor_t *nor, uint8_t *pBuffer, uint32_t BlockAddr, uint32_t Offset, uint32_t NumByteToRead);

#endif /* FLASH_NOR_NOR_H_ */
