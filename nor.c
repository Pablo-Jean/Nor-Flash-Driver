/*
 * nor.c
 *
 *  Created on: 5 de out de 2023
 *      Author: pablo-jean
 */

#include "nor.h"

/*
 * Privates
 */

#if defined (NOR_DEBUG)
#define NOR_PRINTF(...)		// TODO The printf :)
#else
#define NOR_PRINTF(...)
#endif

/* If you are in a RTOS Environment, and has more devices into the */
/* Spi Bus, please, implement a threadsafe method (semaphores) */
#if defined (NOR_THREADSAFE)
#define _NOR_LOCK()		// TODO Lock function
#define _NOR_UNLOCK()	// TODO Unlock function
#else
#define _NOR_LOCK()
#define _NOR_UNLOCK()
#endif

#ifndef NOR_EMPTY_CHECK_BUFFER_LEN
#define NOR_EMPTY_CHECK_BUFFER_LEN		64
#endif

#define _SANITY_CHECK(n)			if (n == NULL)	return NOR_INVALID_PARAMS;					\
									if (n->_internal.u16Initialized != NOR_INITIALIZED_FLAG)	\
										return NOR_NOT_INITIALIZED;

/* Enumerates */

enum _nor_sr_select_e{
	_SELECT_SR1,
	_SELECT_SR2,
	_SELECT_SR3,
};

/* Functions */

void _nor_cs_assert(nor_t *nor){
	nor->config.CsAssert();
}

void _nor_cs_deassert(nor_t *nor){
	nor->config.CsDeassert();
}

void _nor_spi_tx(nor_t *nor, uint8_t *txBuf, uint32_t size){
	nor->config.SpiTxFxn(txBuf, size);
}

void _nor_spi_rx(nor_t *nor, uint8_t *rxBuf, uint32_t size){
	nor->config.SpiRxFxn(rxBuf, size);
}

void _nor_delay_us(nor_t *nor, uint32_t us){
	nor->config.DelayUs(us);
}
//###################################################################################################################
uint32_t _nor_ReadID(nor_t *nor)
{
	uint8_t JedecIdCmd = NOR_JEDEC_ID;
	uint32_t ID = 0;

	_nor_cs_assert(nor);
	_nor_spi_tx(nor, &JedecIdCmd, sizeof(JedecIdCmd));
	_nor_spi_rx(nor, (uint8_t*)&ID, 3);
	_nor_cs_deassert(nor);

	return ID;
}
//###################################################################################################################
uint64_t _nor_ReadUniqID(nor_t *nor)
{
	uint8_t UniqueIdCmd = NOR_UNIQUE_ID;
	uint64_t UniqueId = 0;
	uint32_t DummyU32 = (~0UL);

	_nor_cs_assert(nor);
	_nor_spi_tx(nor, &UniqueIdCmd, sizeof(UniqueIdCmd));
	// this if the 4 dummy byte
	_nor_spi_tx(nor, (uint8_t*)&DummyU32, 4);
	_nor_spi_rx(nor, (uint8_t*)&UniqueId, sizeof(UniqueId));
	_nor_cs_deassert(nor);

	return UniqueId;
}
//###################################################################################################################
void _nor_WriteEnable(nor_t *nor)
{
	uint8_t WriteEnCmd = NOR_CMD_WRITE_EN;

	_nor_cs_assert(nor);
	_nor_spi_tx(nor, &WriteEnCmd, sizeof(WriteEnCmd));
	_nor_cs_deassert(nor);
	// TODO Check if a delay was needed here
}
//###################################################################################################################
void _nor_WriteDisable(nor_t *nor)
{
	uint8_t WriteDisCmd = NOR_CMD_WRITE_DIS;

	_nor_cs_assert(nor);
	_nor_spi_tx(nor, &WriteDisCmd, sizeof(WriteDisCmd));
	_nor_cs_deassert(nor);
}
//###################################################################################################################
uint8_t _nor_ReadStatusRegister(nor_t *nor, enum _nor_sr_select_e SelectSR)
{
	uint8_t status = 0, ReadSRCmd;
	uint8_t *SrUpdateHandler;

	switch (SelectSR){
	case _SELECT_SR1:
		ReadSRCmd = NOR_READ_SR1;
		SrUpdateHandler = &nor->_internal.u8StatusReg1;
		break;
	case _SELECT_SR2:
		ReadSRCmd = NOR_READ_SR2;
		SrUpdateHandler = &nor->_internal.u8StatusReg1;
		break;
	case _SELECT_SR3:
		ReadSRCmd = NOR_READ_SR3;
		SrUpdateHandler = &nor->_internal.u8StatusReg1;
		break;
	default:
		return 0xFF;
	}
	_nor_cs_assert(nor);
	_nor_spi_tx(nor, &ReadSRCmd, sizeof(ReadSRCmd));
	_nor_spi_rx(nor, &status, sizeof(status));
	_nor_cs_deassert(nor);

	*SrUpdateHandler = status;

	return status;
}
//###################################################################################################################
void _nor_WriteStatusRegister(nor_t *nor, enum _nor_sr_select_e SelectSR, uint8_t data)
{
	uint8_t WriteSR[2];

	switch (SelectSR){
	case _SELECT_SR1:
		WriteSR[0] = NOR_WRITE_SR1;
		nor->_internal.u8StatusReg1 = data;
		break;
	case _SELECT_SR2:
		WriteSR[0] = NOR_WRITE_SR2;
		nor->_internal.u8StatusReg1 = data;
		break;
	case _SELECT_SR3:
		WriteSR[0] = NOR_WRITE_SR3;
		nor->_internal.u8StatusReg1 = data;
		break;
	default:
		return ;
	}
	WriteSR[1] = data;
	_nor_cs_assert(nor);
	_nor_spi_tx(nor, WriteSR, sizeof(WriteSR));
	_nor_cs_deassert(nor);
}

nor_err_e _nor_WaitForWriteEnd(nor_t *nor, uint32_t msTimeout)
{
	uint8_t ReadSr1Cmd = NOR_READ_SR1;

	// multply timeout for 10, we must run a delay of 100us on each iteraction
	msTimeout *= 10;
	_nor_cs_assert(nor);
	_nor_spi_tx(nor, (uint8_t*)&ReadSr1Cmd, sizeof(ReadSr1Cmd));
	do{
		_nor_spi_rx(nor, &nor->_internal.u8StatusReg1, sizeof(uint8_t));
		_nor_delay_us(nor, 100);
		msTimeout -= 100;
	}while ((nor->_internal.u8StatusReg1 & SR1_BUSY_BIT) && (msTimeout > 0));
	_nor_cs_deassert(nor);

	if (msTimeout == 0){
		return NOR_FAIL;
	}
	return NOR_OK;
}

nor_err_e _nor_check_buff_is_empty(uint8_t *pBuffer, uint32_t len){
	uint32_t i;

	for (i=0 ; i<len ; i++){
		if (pBuffer[i] != 0xFF){
			return NOR_REGIONS_IS_NOT_EMPTY;
		}
	}

	return NOR_OK;
}

/*
 * Publics
 */

nor_err_e NOR_Init(nor_t *nor){
	if (nor == NULL || nor->config.CsAssert == NULL ||
			nor->config.CsDeassert == NULL || nor->config.DelayUs == NULL ||
			nor->config.SpiRxFxn == NULL || nor->config.SpiTxFxn == NULL){
		return NOR_INVALID_PARAMS;
	}
	if (nor->_internal.u16Initialized == NOR_INITIALIZED_FLAG){
		// the flash instance is already initialized
		return NOR_OK;
	}
	// we must have sure that the NOR has your CS pin deasserted
	_nor_cs_deassert(nor);
	_nor_delay_us(nor, 100);

	// we are assuming, on startup, that the Flash is on Power Down State
	nor->_internal.u8PdCount = 1;
	nor->pdState = NOR_DEEP_POWER_DOWN;
	NOR_ExitPowerDown(nor);

	nor->info.u32JedecID = _nor_ReadID(nor);
	// TODO: fnx to determine

	if (nor->info.u32JedecID == 0x000000 || nor->info.u32JedecID == 0xFFFFFF){
		// invalid Id, I thing we don't has any flash on SPI
		return NOR_NO_MEMORY_FOUND;
	}

	nor->info.u64UniqueId = _nor_ReadUniqID(nor);
	// TODO implement this function
//	nor->info.u32BlockCount = nor_get_blocks_from_id(nor);
	nor->info.u32BlockCount = 1024;

	nor->info.u16PageSize = NOR_PAGE_SIZE;
	nor->info.u16SectorSize = NOR_SECTOR_SIZE;
	nor->info.u32BlockSize = NOR_BLOCK_SIZE;
	nor->info.u32SectorCount = nor->info.u32BlockCount * (NOR_BLOCK_SIZE / NOR_SECTOR_SIZE);
	nor->info.u32PageCount = (nor->info.u32SectorCount * nor->info.u16SectorSize) / nor->info.u16PageSize;
	nor->info.u32Size = (nor->info.u32SectorCount * nor->info.u16SectorSize);

	_nor_ReadStatusRegister(nor, _SELECT_SR1);
	_nor_ReadStatusRegister(nor, _SELECT_SR2);
	_nor_ReadStatusRegister(nor, _SELECT_SR3);
#if (_W25QXX_DEBUG == 1)
	printf("w25qxx Page Size: %d Bytes\r\n", w25qxx.PageSize);
	printf("w25qxx Page Count: %d\r\n", w25qxx.PageCount);
	printf("w25qxx Sector Size: %d Bytes\r\n", w25qxx.SectorSize);
	printf("w25qxx Sector Count: %d\r\n", w25qxx.SectorCount);
	printf("w25qxx Block Size: %d Bytes\r\n", w25qxx.BlockSize);
	printf("w25qxx Block Count: %d\r\n", w25qxx.BlockCount);
	printf("w25qxx Capacity: %d KiloBytes\r\n", w25qxx.CapacityInKiloByte);
	printf("w25qxx Init Done\r\n");
#endif

	return NOR_OK;
}

nor_err_e NOR_Init_wo_ID(nor_t *nor){
	if (nor == NULL || nor->config.CsAssert == NULL ||
			nor->config.CsDeassert == NULL || nor->config.DelayUs == NULL ||
			nor->config.SpiRxFxn == NULL || nor->config.SpiTxFxn == NULL){
		return NOR_INVALID_PARAMS;
	}
	if (nor->_internal.u16Initialized == NOR_INITIALIZED_FLAG){
		// the flash instance is already initialized
		return NOR_OK;
	}
	// we must have sure that the NOR has your CS pin deasserted
	_nor_cs_deassert(nor);
	_nor_delay_us(nor, 100);

	// we are assuming, on startup, that the Flash is on Power Down State
	nor->_internal.u8PdCount = 1;
	nor->pdState = NOR_DEEP_POWER_DOWN;
	NOR_ExitPowerDown(nor);

	nor->info.u32JedecID = 0x0;
	nor->info.u64UniqueId = _nor_ReadUniqID(nor);

	nor->info.u16PageSize = NOR_PAGE_SIZE;
	nor->info.u16SectorSize = NOR_SECTOR_SIZE;
	nor->info.u32BlockSize = NOR_BLOCK_SIZE;

	_nor_ReadStatusRegister(nor, _SELECT_SR1);
	_nor_ReadStatusRegister(nor, _SELECT_SR2);
	_nor_ReadStatusRegister(nor, _SELECT_SR3);

	return NOR_OK;
}

nor_err_e NOR_EnterPowerDown(nor_t *nor){
	uint8_t DeepPDCmd = NOR_ENTER_PD;

	_SANITY_CHECK(nor);

	if (nor->_internal.u8PdCount > 0){
		nor->_internal.u8PdCount--;
		if (nor->_internal.u8PdCount == 0){
			_nor_spi_tx(nor, &DeepPDCmd, sizeof(DeepPDCmd));
			nor->pdState = NOR_DEEP_POWER_DOWN;
		}
	}

	return NOR_OK;
}
nor_err_e NOR_ExitPortDown(nor_t *nor){
	uint8_t ExitPDCmd = NOR_RELEASE_PD;

	_SANITY_CHECK(nor);

	if (nor->_internal.u8PdCount == 0){
		_nor_spi_tx(nor, &ExitPDCmd, sizeof(ExitPDCmd));
		nor->pdState = NOR_IN_IDLE;
	}
	nor->_internal.u8PdCount++;

	return NOR_OK;
}

nor_err_e NOR_EraseChip(nor_t *nor){
	uint8_t EraseChipCmd = NOR_CHIP_ERASE;

	_SANITY_CHECK(nor);

	_nor_WriteEnable(nor);
	_nor_cs_assert(nor);
	_nor_spi_tx(nor, &EraseChipCmd, sizeof(EraseChipCmd));
	_nor_cs_deassert(nor);
	return _nor_WaitForWriteEnd(nor, 80000000);
}

nor_err_e NOR_EraseAddress(nor_t *nor, uint32_t Address, nor_erase_method_e method){
	uint8_t EraseChipCmd[4];
	uint32_t expectedTimeoutUs;

	_SANITY_CHECK(nor);

	switch (method){
	case NOR_ERASE_4K:
		EraseChipCmd[0] = NOR_SECTOR_ERASE_4K;
		expectedTimeoutUs = NOR_EXPECT_4K_ERASE_TIME;
		break;
	case NOR_ERASE_32K:
		EraseChipCmd[0] = NOR_SECTOR_ERASE_32K;
		expectedTimeoutUs = NOR_EXPECT_32K_ERASE_TIME;
		break;
	case NOR_ERASE_64K:
		EraseChipCmd[0] = NOR_SECTOR_ERASE_64K;
		expectedTimeoutUs = NOR_EXPECT_64K_ERASE_TIME;
		break;
	}
	EraseChipCmd[1] = ((Address >> 16) & 0xFF);
	EraseChipCmd[2] = ((Address >> 8) & 0xFF);
	EraseChipCmd[3] = ((Address) & 0xFF);

	_nor_WriteEnable(nor);
	_nor_cs_assert(nor);
	_nor_spi_tx(nor, EraseChipCmd, sizeof(EraseChipCmd));
	_nor_cs_deassert(nor);
	return _nor_WaitForWriteEnd(nor, expectedTimeoutUs);
}

nor_err_e NOR_EraseSector(nor_t *nor, uint32_t SectorAddr){
	uint32_t Address;

	_SANITY_CHECK(nor);

	Address = SectorAddr * nor->info.u16SectorSize;
	return NOR_EraseAddress(nor, Address, NOR_ERASE_4K);
}

nor_err_e NOR_EraseBlock(nor_t *nor, uint32_t BlockAddr){
	uint32_t Address;

	_SANITY_CHECK(nor);

	Address = BlockAddr * nor->info.u32BlockSize;
	return NOR_EraseAddress(nor, Address, NOR_ERASE_64K);
}

nor_err_e NOR_PageToSector(nor_t *nor, uint32_t PageAddr, uint32_t *SectorAddr){
	_SANITY_CHECK(nor);
	*SectorAddr = PageAddr * nor->info.u16PageSize / nor->info.u16SectorSize;

	return NOR_OK;
}

nor_err_e NOR_PageToBlock(nor_t *nor, uint32_t PageAddr, uint32_t *BlockAddr){
	_SANITY_CHECK(nor);
	*BlockAddr = PageAddr * nor->info.u16PageSize / nor->info.u32BlockSize;

	return NOR_OK;
}

nor_err_e NOR_SectorToBlock(nor_t *nor, uint32_t SectorAddr, uint32_t *BlockAddr){
	_SANITY_CHECK(nor);
	*BlockAddr = SectorAddr * nor->info.u16SectorSize / nor->info.u32BlockSize;

	return NOR_OK;
}

nor_err_e NOR_SectorToPage(nor_t *nor, uint32_t SectorAddr, uint32_t *PageAddr){
	_SANITY_CHECK(nor);
	*PageAddr = SectorAddr * nor->info.u16SectorSize / nor->info.u16PageSize;

	return NOR_OK;
}

nor_err_e NOR_BlockToPage(nor_t *nor, uint32_t BlockAddr, uint32_t *PageAddr){
	_SANITY_CHECK(nor);
	*PageAddr = BlockAddr * nor->info.u32BlockSize / nor->info.u16PageSize;

	return NOR_OK;
}

nor_err_e NOR_IsEmptyPage(nor_t *nor, uint32_t PageAddr, uint32_t Offset, uint32_t NumBytesToCheck){
	uint8_t pBuffer[NOR_EMPTY_CHECK_BUFFER_LEN];
	uint32_t ActAddress;

	_SANITY_CHECK(nor);

	ActAddress = (nor->info.u16PageSize * PageAddr) + Offset;
	if (NumBytesToCheck == 0 || (NumBytesToCheck + Offset) > nor->info.u16PageSize){
		NumBytesToCheck = nor->info.u16PageSize - Offset;
	}
	while (NumBytesToCheck > 0){
		NOR_ReadBytes(nor, pBuffer, ActAddress, NOR_EMPTY_CHECK_BUFFER_LEN);
		NumBytesToCheck -= NOR_EMPTY_CHECK_BUFFER_LEN;
		if (_nor_check_buff_is_empty(pBuffer, NOR_EMPTY_CHECK_BUFFER_LEN) == NOR_REGIONS_IS_NOT_EMPTY){
			return NOR_REGIONS_IS_NOT_EMPTY;
		}
	}

	return NOR_OK;
}

nor_err_e NOR_IsEmptySector(nor_t *nor, uint32_t SectorAddr, uint32_t Offset, uint32_t NumBytesToCheck){
	uint8_t pBuffer[NOR_EMPTY_CHECK_BUFFER_LEN];
	uint32_t ActAddress;

	_SANITY_CHECK(nor);

	ActAddress = (nor->info.u16SectorSize * SectorAddr) + Offset;
	if (NumBytesToCheck == 0 || (NumBytesToCheck + Offset) > nor->info.u16SectorSize){
		NumBytesToCheck = nor->info.u16SectorSize - Offset;
	}
	while (NumBytesToCheck > 0){
		NOR_ReadBytes(nor, pBuffer, ActAddress, NOR_EMPTY_CHECK_BUFFER_LEN);
		NumBytesToCheck -= NOR_EMPTY_CHECK_BUFFER_LEN;
		if (_nor_check_buff_is_empty(pBuffer, NOR_EMPTY_CHECK_BUFFER_LEN) == NOR_REGIONS_IS_NOT_EMPTY){
			return NOR_REGIONS_IS_NOT_EMPTY;
		}
	}

	return NOR_OK;
}

nor_err_e NOR_IsEmptyBlock(nor_t *nor, uint32_t BlockAddr, uint32_t Offset, uint32_t NumBytesToCheck){
	uint8_t pBuffer[NOR_EMPTY_CHECK_BUFFER_LEN];
	uint32_t ActAddress;

	_SANITY_CHECK(nor);

	ActAddress = (nor->info.u32BlockSize * BlockAddr) + Offset;
	if (NumBytesToCheck == 0 || (NumBytesToCheck + Offset) > nor->info.u32BlockSize){
		NumBytesToCheck = nor->info.u32BlockSize - Offset;
	}
	while (NumBytesToCheck > 0){
		NOR_ReadBytes(nor, pBuffer, ActAddress, NOR_EMPTY_CHECK_BUFFER_LEN);
		NumBytesToCheck -= NOR_EMPTY_CHECK_BUFFER_LEN;
		if (_nor_check_buff_is_empty(pBuffer, NOR_EMPTY_CHECK_BUFFER_LEN) == NOR_REGIONS_IS_NOT_EMPTY){
			return NOR_REGIONS_IS_NOT_EMPTY;
		}
	}

	return NOR_OK;
}

nor_err_e NOR_WriteBytes(nor_t *nor, uint8_t *pBuffer, uint32_t WriteAddr, uint32_t NumBytesToWrite){
	uint8_t WriteCmd[4];

	_SANITY_CHECK(nor);

	// TODO check if Address is not grater than the Flash size

	// TODO check if Number of bytes to read will not overlap the page
	// if happens, iterate over the pages
	_nor_WriteEnable(nor);
	WriteCmd[0] = NOR_PAGE_PROGRAM;
	WriteCmd[1] = ((WriteAddr >> 16) & 0xFF);
	WriteCmd[2] = ((WriteAddr >> 8) & 0xFF);
	WriteCmd[3] = ((WriteAddr) & 0xFF);
	_nor_cs_assert(nor);
	_nor_spi_tx(nor, WriteCmd, sizeof(WriteCmd));
	_nor_spi_tx(nor, pBuffer, NumBytesToWrite);
	_nor_cs_deassert(nor);
	_nor_WaitForWriteEnd(nor, NOR_EXPECT_PAGE_PROG_TIME);

	return NOR_OK;
}


nor_err_e NOR_WritePage(nor_t *nor, uint8_t *pBuffer, uint32_t PageAddr, uint32_t Offset, uint32_t NumBytesToWrite){
	uint32_t Address;

	_SANITY_CHECK(nor);

	while (Offset >= nor->info.u16PageSize){
		PageAddr++;
		Offset -= nor->info.u16PageSize;
	}

	Address = (PageAddr * nor->info.u16PageSize) + Offset;
	return NOR_WriteBytes(nor, pBuffer, Address, NumBytesToWrite);
}

nor_err_e NOR_WriteSector(nor_t *nor, uint8_t *pBuffer, uint32_t SectorAddr, uint32_t Offset, uint32_t NumBytesToWrite){
	uint32_t Address;

	_SANITY_CHECK(nor);

	while (Offset >= nor->info.u16SectorSize){
		SectorAddr++;
		Offset -= nor->info.u16SectorSize;
	}

	Address = (SectorAddr * nor->info.u16SectorSize) + Offset;
	return NOR_WriteBytes(nor, pBuffer, Address, NumBytesToWrite);
}

nor_err_e NOR_WriteBlock(nor_t *nor, uint8_t *pBuffer, uint32_t BlockAddr, uint32_t Offset, uint32_t NumBytesToWrite){
	uint32_t Address;

	_SANITY_CHECK(nor);

	while (Offset >= nor->info.u32BlockSize){
		BlockAddr++;
		Offset -= nor->info.u32BlockSize;
	}

	Address = (BlockAddr * nor->info.u32BlockSize) + Offset;
	return NOR_WriteBytes(nor, pBuffer, Address, NumBytesToWrite);
}

nor_err_e NOR_ReadBytes(nor_t *nor, uint8_t *pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead){
	uint8_t ReadCmd[5];

	_SANITY_CHECK(nor);

	// TODO check if Address is not grater than the Flash size

	// TODO check if Number of bytes to read will not overlap the page
	// if happens, iterate over the pages
	ReadCmd[0] = NOR_READ_FAST_DATA;
	ReadCmd[1] = ((ReadAddr >> 16) & 0xFF);
	ReadCmd[2] = ((ReadAddr >> 8) & 0xFF);
	ReadCmd[3] = ((ReadAddr) & 0xFF);
	ReadCmd[4] = 0x00;
	_nor_cs_assert(nor);
	_nor_spi_tx(nor, ReadCmd, sizeof(ReadCmd));
	_nor_spi_rx(nor, pBuffer, NumByteToRead);
	_nor_cs_deassert(nor);

	return NOR_OK;
}

nor_err_e NOR_ReadPage(nor_t *nor, uint8_t *pBuffer, uint32_t PageAddr, uint32_t Offset, uint32_t NumByteToRead){
	uint32_t Address;

	_SANITY_CHECK(nor);

	while (Offset >= nor->info.u16PageSize){
		PageAddr++;
		Offset -= nor->info.u16PageSize;
	}

	Address = (PageAddr * nor->info.u16PageSize) + Offset;
	return NOR_ReadBytes(nor, pBuffer, Address, NumByteToRead);
}

nor_err_e NOR_ReadSector(nor_t *nor, uint8_t *pBuffer, uint32_t SectorAddr, uint32_t Offset, uint32_t NumByteToRead){
	uint32_t Address;

	_SANITY_CHECK(nor);

	while (Offset >= nor->info.u16SectorSize){
		SectorAddr++;
		Offset -= nor->info.u16SectorSize;
	}

	Address = (SectorAddr * nor->info.u16SectorSize) + Offset;
	return NOR_ReadBytes(nor, pBuffer, Address, NumByteToRead);
}

nor_err_e NOR_ReadBlock(nor_t *nor, uint8_t *pBuffer, uint32_t BlockAddr, uint32_t Offset, uint32_t NumByteToRead){
	uint32_t Address;

	_SANITY_CHECK(nor);

	while (Offset >= nor->info.u32BlockSize){
		BlockAddr++;
		Offset -= nor->info.u32BlockSize;
	}

	Address = (BlockAddr * nor->info.u32BlockSize) + Offset;
	return NOR_ReadBytes(nor, pBuffer, Address, NumByteToRead);
}

