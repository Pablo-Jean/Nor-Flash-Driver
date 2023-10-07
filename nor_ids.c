/*
 * nor_ids.c
 *
 *  Created on: Oct 6, 2023
 *      Author: pablo-jean
 */

#include "nor_ids.h"

nor_manuf_e NOR_IDS_Interpret_Manufacturer (uint32_t JedecID){
	return (nor_manuf_e)(JedecID & 0xFF);
}

nor_model_e NOR_IDS_Interpret_Model (uint32_t JedecID){
	nor_manuf_e Manuf = NOR_IDS_Interpret_Manufacturer(JedecID);

	switch (Manuf){
	case	MANUF_PUYA:
	case	MANUF_ADESTO:
	case	MANUF_MICROCHIP:
	default:
		return NOR_MODEL_UNKNOWN;
	case 	MANUF_MXIC:
		return ((JedecID >> 16) & 0xFF);
		break;
	case	MANUF_WINBOND:
		return ((JedecID >> 8) & 0xFFFF);
		break;
	}
}

uint32_t NOR_IDS_GetQtdBlocks(uint32_t JedecID){
	nor_manuf_e Manuf = NOR_IDS_Interpret_Manufacturer(JedecID);
	uint8_t density, i;
	uint32_t blocks;

	switch (Manuf){
	case	MANUF_ADESTO:
	case	MANUF_MICROCHIP:
	default:
		blocks = 0;
		break;
	case	MANUF_PUYA:
	case 	MANUF_MXIC:
	case	MANUF_WINBOND:
		density = ((JedecID >> 16) & 0xFF);
		density -= 0x11;
		blocks = 2;
		// yes, I'm doing this only to prevent the use of math lib
		for (i=0 ; i<density ; i++){
			blocks *= 2;
		}
		break;
	}

	return blocks;
}
