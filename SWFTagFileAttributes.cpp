/*
 * SWFTagFileAttributes.cpp
 *
 *  Created on: 22.05.2013
 *      Author: brigada
 */

#include "SWFTagFileAttributes.h"

SWFTagFileAttributes::SWFTagFileAttributes() :
	useDirectBlit(false),
	useGPU(false),
	hasMetadata(false),
	actionScript3(false),
	useNetwork(false)
{
}

SWFTagFileAttributes::~SWFTagFileAttributes() {
}

bool SWFTagFileAttributes::set(std::string value, bool strict) {
	if(strict && ((value[0] & 0x86) || value[1] || value[2] || value[3]) ) return false;

	useDirectBlit = (value[0] >> 6) & 1;
	useGPU = (value[0] >> 5) & 1;
	hasMetadata = (value[0] >> 4) & 1;
	actionScript3 = (value[0] >> 3) & 1;
	useNetwork = value[0] & 1;
	return true;
}
