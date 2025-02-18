/*
 * nor_ids.h
 *
 *  Created on: 4 de out de 2023
 *      Author: Pablo Jean
 *
 *      youtube:
 *      Github:
 *      Udemy:
 */

#ifndef NOR_IDS_H_
#define NOR_IDS_H_

#include <stdint.h>

typedef enum{
	MANUF_MXIC = 0xC2,
	MANUF_ADESTO = 0x1F,
	MANUF_WINBOND = 0xEF,
	MANUF_PUYA = 0x85,
	MANUF_MICROCHIP = 0xBF,
	MANUF_XMC = 0x20
}nor_manuf_e;

typedef enum{
	// TODO register more commom memory chips
	/* MXIC Codes */
	MX25x10xx = 0x11,
	MX25x20xx = 0x12,
	MX25x40xx = 0x13,
	MX25x80xx = 0x14,
	MX25x16xx = 0x15,
	MX25x32xx = 0x16,
	MX25x64xx = 0x17,
	MX25x128xx = 0x18,
	MX25x256xx = 0x19,
	/* Adesto Codes */
	// TODO : Adesto is complicated :(

	/* Winbond Codes */
	W25x10xx = 0x1140,
	W25x20xx = 0x1240,
	W25x40xx = 0x1340,
	W25x80xx = 0x1440,
	W25x16xx = 0x1540,
	W25x32xx = 0x1640,
	W25x64xx = 0x1740,
	W25x128xx = 0x1840,
	W25x256xx = 0x1940,


	/* Puya Codes */
	// TODO write the Puya Codes

	/* Microchip Codes */
	// TODO write the Microchip codes


	NOR_MODEL_UNKNOWN = 0xFFFF
}nor_model_e;

nor_manuf_e NOR_IDS_Interpret_Manufacturer (uint32_t JedecID);

nor_model_e NOR_IDS_Interpret_Model (uint32_t JedecID);

uint32_t NOR_IDS_GetQtdBlocks(uint32_t JedecID);

#endif /* NOR_IDS_H_ */
