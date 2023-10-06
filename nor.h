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

#include "norDefs.h"

/**
 * Macros
 */

#define NOR_INITIALIZED_FLAG		0xCFFE

#define NOR_CMD_WRITE_EN			0x06
#define NOR_CMD_SR_WRITE_EN			0x50
#define NOR_CMD_WRITE_DIS			0x04

#define NOR_RELEASE_PD				0xAB
#define NOR_ENTER_PD				0xB9
#define NOR_DEVICE_ID				0x90
#define NOR_JEDEC_ID				0x9F
#define NOR_UNIQUE_ID				0x4B

#define NOR_READ_DATA				0x03
#define NOR_READ_FAST_DATA			0x0B

#define NOR_PAGE_PROGRAM			0x02

#define NOR_SECTOR_ERASE_4K			0x20
#define NOR_SECTOR_ERASE_32K		0x52
#define NOR_SECTOR_ERASE_64K		0xD8
#define NOR_CHIP_ERASE				0xC7

#define NOR_READ_SR1				0x05
#define NOR_WRITE_SR1				0x01
#define NOR_READ_SR2				0x35
#define NOR_WRITE_SR2				0x31
#define NOR_READ_SR3				0x15
#define NOR_WRITE_SR3				0x11

#define NOR_READ_SFDP_REG			0x5A
#define NOR_ERASE_SEC_REG			0x44
#define NOR_PROGRAM_SEC_REG			0x42
#define NOR_READ_SEC_REG			0x48

#define NOR_GLOBAL_BL_LOCK			0x7E
#define NOR_GLOBAL_BL_UNLOCK		0x98
#define NOR_READ_BL_LOCK			0x3D
#define NOR_IND_BL_LOCK				0x36
#define NOR_IND_BL_UNLOCK			0x39

#define NOR_ER_PROG_SUSPEND			0x75
#define NOR_ER_PROG_RESUME			0x7A

#define NOR_ENABLE_RESET			0x66
#define NOR_DEVICE_RESET			0x99

/**
 * Enumerates
 */

typedef enum{
	NOR_OK,
	NOR_FAIL,
	NOR_NO_MEMORY_FOUND,
	NOR_UNKNOWN_DEVICE,
	NOR_OUT_OF_RANGE,
	NOR_NOT_INITIALIZED,
	NOR_REGIONS_IS_NOT_EMPTY,
	NOR_IS_LOCKED,

	NOR_UNKNOWN = 0xFF
}nor_err_e;


/**
 * Function Typedefs
 */

typedef void (*SpiTx_fxn_t)(uint8_t* TxBuff, uint32_t len);
typedef void (*SpiRx_fxn_t)(uint8_t* RxBuff, uint32_t len);
typedef void (*CS_Assert_fnx_t)(void);
typedef void (*CS_Deassert_fxn_t)(void);
typedef void (*delay_us_fxn_t)(void);

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
	}info;
	struct{
		uint16_t u16Initialized;
		uint8_t u8StatusReg1;
		uint8_t u8StatusReg2;
		uint8_t u8StatusReg3;
	}_internal;
	nor_manuf_e Manufacturer;
	nor_model_e Model;
}nor_t;

/**
 * Publics
 */

nor_err_e NOR_Init(nor_t *nor);

nor_err_e NOR_EnterPowerDown(nor_t *nor);
nor_err_e NOR_ExitPortDown(nor_t *nor);

nor_err_e NOR_EraseChip(nor_t *nor);
nor_err_e NOR_EraseAddress(nor_t *nor, uint32_t Address);
nor_err_e NOR_EraseSector(nor_t *nor, uint32_t SectorAddr);
nor_err_e NOR_EraseBlock(nor_t *nor, uint32_t BlockAddr);

nor_err_e NOR_PageToSector(nor_t *nor, uint32_t PageAddr, uint32_t *SectorAddr);
nor_err_e NOR_PageToBlock(nor_t *nor, uint32_t PageAddr, uint32_t *PageAddr);
nor_err_e NOR_SectorToBlock(nor_t *nor, uint32_t SectorAddr, uint32_t *BlockAddr);
nor_err_e NOR_SectorToPage(nor_t *nor, uint32_t SectorAddr, uint32_t *PageAddr);
nor_err_e NOR_BlockToPage(nor_t *nor, uint32_t BlockAddr, uint32_t *PageAddr);

nor_err_e NOR_IsEmptyPage(nor_t *nor, uint32_t PageAddr, uint32_t Offset, uint32_t NumBytesToCheck);
nor_err_e NOR_IsEmptySector(nor_t *nor, uint32_t SectorAddr, uint32_t Offset, uint32_t NumBytesToCheck);
nor_err_e NOR_IsEmptyBlock(nor_t *nor, uint32_t BlockAddr, uint32_t Offset, uint32_t NumBytesToCheck);

nor_err_e NOR_WriteByte(nor_t *nor, uint8_t pBuffer, uint32_t BytesAddr);
nor_err_e NOR_WritePage(nor_t *nor, uint8_t *pBuffer, uint32_t PageAddr, uint32_t Offset, uint32_t NumBytesToWrite);
nor_err_e NOR_WriteSector(nor_t *nor, uint8_t *pBuffer, uint32_t Sector_Address, uint32_t Offset, uint32_t NumBytesToWrite);
nor_err_e NOR_WriteBlock(nor_t *nor, uint8_t *pBuffer, uint32_t Block_Address, uint32_t Offset, uint32_t NumBytesToWrite);

nor_err_e NOR_ReadByte(nor_t *nor, uint8_t *pBuffer, uint32_t ByteAddr);
nor_err_e NOR_ReadBytes(nor_t *nor, uint8_t *pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead);
nor_err_e NOR_ReadPage(nor_t *nor, uint8_t *pBuffer, uint32_t Page_Addr, uint32_t Offset, uint32_t NumByteToRead);
nor_err_e NOR_ReadSector(nor_t *nor, uint8_t *pBuffer, uint32_t Sector_Addr, uint32_t Offset, uint32_t NumByteToRead);
nor_err_e NOR_ReadBlock(nor_t *nor, uint8_t *pBuffer, uint32_t BlockAddr, uint32_t Offset, uint32_t NumByteToRead);

#endif /* FLASH_NOR_NOR_H_ */
