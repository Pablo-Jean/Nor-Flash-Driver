/*
 * nor_defines.h
 *
 *  Created on: 6 de out de 2023
 *      Author: pablo-jean
 */

#ifndef FLASH_NOR_NOR_DEFINES_H_
#define FLASH_NOR_NOR_DEFINES_H_

#include <stdint.h>

// Flash NOR Commands
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

#define SR1_BUSY_BIT				(1)
#define SR1_WEL_BIT					(1<<1)
#define SR1_BP0_BIT					(1<<2)
#define SR1_BP1_BIT					(1<<3)
#define SR1_BP2_BIT					(1<<4)
#define SR1_TB_BIT					(1<<5)
#define SR1_SEC_BIT					(1<<6)
#define SR1_SRP_BIT					(1<<7)

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

// Flash Memory Global parameters
#define NOR_PAGE_SIZE				0x100
#define NOR_SECTOR_SIZE				0x1000
#define NOR_BLOCK_SIZE				0x10000

#define NOR_EXPECT_4K_ERASE_TIME	10000
#define NOR_EXPECT_32K_ERASE_TIME	16000
#define NOR_EXPECT_64K_ERASE_TIME	25000
#define NOR_EXPECT_ERASE_CHIP		160000
#define NOR_EXPECT_PAGE_PROG_TIME	500


#endif /* FLASH_NOR_NOR_DEFINES_H_ */
