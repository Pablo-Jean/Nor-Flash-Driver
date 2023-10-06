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

typedef enum{
	MANUF_MXIC,
	MANUF_ADESTO,
	MANUF_WINBOND,
	MANUF_PUYA,
	MANUF_MICROCHIP,

	MANUF_UNKNOWN = 0xFF
}nor_manuf_e;

typedef enum{
	// TODO register more commom memory chips
	/* MXIC Codes */

	/* Adesto Codes */

	/* Winbond Codes */
	W25QXX = 0x40,

	/* Puya Codes */

	/* Microchip Codes */

}nor_model_e;

#endif /* NOR_IDS_H_ */
