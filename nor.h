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

/**
 * @brief
 *
 */
typedef enum{
	NOR_IN_IDLE,       /**< NOR_IN_IDLE */
	NOR_DEEP_POWER_DOWN/**< NOR_DEEP_POWER_DOWN */
}nor_pd_e;

/**
 * @brief
 *
 */
typedef enum{
	NOR_OK,                  /**< NOR_OK */
	NOR_FAIL,                /**< NOR_FAIL */
	NOR_INVALID_PARAMS,      /**< NOR_INVALID_PARAMS */
	NOR_NO_MEMORY_FOUND,     /**< NOR_NO_MEMORY_FOUND */
	NOR_UNKNOWN_DEVICE,      /**< NOR_UNKNOWN_DEVICE */
	NOR_OUT_OF_RANGE,        /**< NOR_OUT_OF_RANGE */
	NOR_NOT_INITIALIZED,     /**< NOR_NOT_INITIALIZED */
	NOR_REGIONS_IS_NOT_EMPTY,/**< NOR_REGIONS_IS_NOT_EMPTY */
	NOR_IS_LOCKED,           /**< NOR_IS_LOCKED */

	NOR_UNKNOWN = 0xFF       /**< NOR_UNKNOWN */
}nor_err_e;

/**
 * @brief
 *
 */
typedef enum{
	NOR_ERASE_4K, /**< NOR_ERASE_4K */
	NOR_ERASE_32K,/**< NOR_ERASE_32K */
	NOR_ERASE_64K /**< NOR_ERASE_64K */
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

/* **********************************
 * Initialize Functions
 * **********************************/

/**
 * @brief Initialize the Flash Nor Driver. Getting the device JEDEC ID
 * and your size.
 *
 * @param nor : pointer to the Nor Instance
 * @return NOR_OK if everything is fine
 * @return NOR_INVALID_PARAMS if any parameter was NULL
 * @return NOR_NO_MEMORY_FOUND if no device was found on SPI Bus, or a
 * mistake on SPI implementation
 * @return NOR_UNKNOWN_DEVICE if JEDEC Device ID was not registered on nor_ids.
 * Or can be a mistake on the SPI implementation.
 *
 * @note If you get NOR_UNKNOWN_DEVICE, but you known your memory device, you
 * can call the NOR_Init_wo_ID, providing the Block Count value into info of
 * nor_t struct, where, this function, will bypass the JEDEC ID identification
 */
nor_err_e NOR_Init(nor_t *nor);

/**
 * @brief Initialize the Flash Nor Driver. The difference here, is that the
 * routine will get the JEDEC ID, but will bypass the Block Count Configuration.
 * Please! Provide the Block Count before call this functions:
 * Example: "nor.info.u32BlockCount = 1024;"
 *
 * @param nor pointer to the Nor Instance
 * @return NOR_OK if everything is fine
 * @return NOR_INVALID_PARAMS if any parameter was NULL or Block Count was 0
 * @return NOR_NO_MEMORY_FOUND if no device was found on SPI Bus, or a
 * mistake on SPI implementation
 * Or can be a mistake on the SPI implementation.
 */
nor_err_e NOR_Init_wo_ID(nor_t *nor);

/* **********************************
 * Deep Power Down Functions
 * **********************************/

/**
 * @brief Issue the Deep Power Down instruction into the SPI. The nor instance has a
 * internal counter, to prevent issuing multiple commands and, preventing a
 * bug in environments where the Flash device was used by multiple threads.
 * Every call to this function will increment the counter. The instruction is issued
 * only if this count, at the start, is 0.
 *
 * @param nor pointer to the Nor Instance
 * @return NOR_OK everything was ok
 * @return NOR_NOT_INITIALIZED the Instance was not initialized, please call NOR_Init
 * or NOR_Init_wo_ID
 * @return NOR_INVALID_PARAMS nor was NULL
 */
nor_err_e NOR_EnterPowerDown(nor_t *nor);

/**
 * @brief Issue the Release Deep Power Down instruction into the SPI.
 * The nor instance has a internal counter, to prevent issuing multiple commands and,
 * preventing a bug in environments where the Flash device was used by multiple threads.
 * Every call to this function will decrement the counter. When decrementing, if counter
 * was 0, the command was sent to the device. If the counter is zero, this command is
 * ignored.
 *
 * @param nor pointer to the Nor Instance
 * @return NOR_OK everything was ok
 * @return NOR_NOT_INITIALIZED the Instance was not initialized, please call NOR_Init
 * or NOR_Init_wo_ID
 * @return NOR_INVALID_PARAMS nor was NULL
 */
nor_err_e NOR_ExitPowerDown(nor_t *nor);

/* **********************************
 * Memory Erase Functions
 * **********************************/

/**
 * @brief Erase the entire memory.
 *
 * @note This command can take a lot of time, sit down, open your soda
 * and just wait. So, take care with any watchdog.
 *
 * @param nor pointer to the Nor Instance
 * @return NOR_OK everything was ok
 * @return NOR_NOT_INITIALIZED the Instance was not initialized, please call NOR_Init
 * or NOR_Init_wo_ID
 * @return NOR_INVALID_PARAMS nor was NULL
 */
nor_err_e NOR_EraseChip(nor_t *nor);

/**
 * @brief Erase a specific address, provided by the developer.
 *
 * @note The minimum address size to erase are 4K (4096) of an entire sector.
 * Generally, the sectors are aligned in 0x1000. So, if you issue an address like
 * '0x5400', the command will consideer your address as '0x5000', for the command to
 * erase a Sector.
 *
 * @param nor pointer to the Nor Instance
 * @param Address The address that we want to erase
 * @param method Accept the following vaues: NOR_ERASE_4K, NOR_ERASE_32K and
 * NOR_ERASE_64K
 * @return NOR_OK everything was ok
 * @return NOR_NOT_INITIALIZED the Instance was not initialized, please call NOR_Init
 * or NOR_Init_wo_ID
 * @return NOR_INVALID_PARAMS nor was NULL or methos is an invalid address
 * @return NOR_OUT_OF_RANGE if Address is greater than the device memory
 */
nor_err_e NOR_EraseAddress(nor_t *nor, uint32_t Address, nor_erase_method_e method);
nor_err_e NOR_EraseSector(nor_t *nor, uint32_t SectorAddr);
nor_err_e NOR_EraseBlock(nor_t *nor, uint32_t BlockAddr);

/* **********************************
 * Page/Sector/Block Conversions
 * **********************************/

uint32_t NOR_PageToSector(nor_t *nor, uint32_t PageAddr);
uint32_t NOR_PageToBlock(nor_t *nor, uint32_t PageAddr);
uint32_t NOR_SectorToBlock(nor_t *nor, uint32_t SectorAddr);
uint32_t NOR_SectorToPage(nor_t *nor, uint32_t SectorAddr);
uint32_t NOR_BlockToPage(nor_t *nor, uint32_t BlockAddr);

/* **********************************
 * Empty regions check Functions
 * **********************************/

nor_err_e NOR_IsEmptyAddress(nor_t *nor, uint32_t Address, uint32_t NumBytesToCheck);
nor_err_e NOR_IsEmptyPage(nor_t *nor, uint32_t PageAddr, uint32_t Offset, uint32_t NumBytesToCheck);
nor_err_e NOR_IsEmptySector(nor_t *nor, uint32_t SectorAddr, uint32_t Offset, uint32_t NumBytesToCheck);
nor_err_e NOR_IsEmptyBlock(nor_t *nor, uint32_t BlockAddr, uint32_t Offset, uint32_t NumBytesToCheck);

/* **********************************
 * Memory programming functions
 * **********************************/

nor_err_e NOR_WriteBytes(nor_t *nor, uint8_t *pBuffer, uint32_t WriteAddr, uint32_t NumBytesToWrite);
nor_err_e NOR_WritePage(nor_t *nor, uint8_t *pBuffer, uint32_t PageAddr, uint32_t Offset, uint32_t NumBytesToWrite);
nor_err_e NOR_WriteSector(nor_t *nor, uint8_t *pBuffer, uint32_t SectorAddr, uint32_t Offset, uint32_t NumBytesToWrite);
nor_err_e NOR_WriteBlock(nor_t *nor, uint8_t *pBuffer, uint32_t BlockAddr, uint32_t Offset, uint32_t NumBytesToWrite);

/* **********************************
 * Memory read functions
 * **********************************/

nor_err_e NOR_ReadBytes(nor_t *nor, uint8_t *pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead);
nor_err_e NOR_ReadPage(nor_t *nor, uint8_t *pBuffer, uint32_t PageAddr, uint32_t Offset, uint32_t NumByteToRead);
nor_err_e NOR_ReadSector(nor_t *nor, uint8_t *pBuffer, uint32_t SectorAddr, uint32_t Offset, uint32_t NumByteToRead);
nor_err_e NOR_ReadBlock(nor_t *nor, uint8_t *pBuffer, uint32_t BlockAddr, uint32_t Offset, uint32_t NumByteToRead);

#endif /* FLASH_NOR_NOR_H_ */
