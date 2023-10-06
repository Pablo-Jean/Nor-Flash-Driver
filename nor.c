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

void _nor_spi_tx(nor_t *nor, uint8_t *txBuf, uint32_t size)
{
	nor->config.SpiTxFxn(txBuf, size);
}

void _nor_spi_rx(nor_t *nor, uint8_t *rxBuf, uint32_t size){
	nor->config.SpiRxFxn(rxBuf, size);
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
	_nor_spi_tx(nor, &WriteDisCmd, sizeof(WriteEnCmd));
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
	_nor_spi_tx(nor, WriteSR[0], sizeof(WriteSR[0]));
	_nor_cs_deassert(nor);
}

nor_err_e W25qxx_WaitForWriteEnd(void)
{
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_RESET);
	W25qxx_Spi(0x05);
	do
	{
		w25qxx.StatusRegister1 = W25qxx_Spi(W25QXX_DUMMY_BYTE);
		W25qxx_Delay(1);
	} while ((w25qxx.StatusRegister1 & 0x01) == 0x01);
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_SET);
}

/*
 * Publics
 */

nor_err_e NOR_Init(nor_t *nor){

}

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
