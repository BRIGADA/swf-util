/*
 * SWFTagMetadata.cpp
 *
 *  Created on: 22.05.2013
 *      Author: brigada
 */

#include "SWFTagMetadata.h"

SWFTagMetadata::SWFTagMetadata() {
	// TODO Auto-generated constructor stub

}

SWFTagMetadata::~SWFTagMetadata() {
	// TODO Auto-generated destructor stub
}

uint32_t SWFTagMetadata::size() {
	return value.size() + 1;
}

bool SWFTagMetadata::set(std::string val) {
	if(val[val.size()-1] != '\0') return false;
	value = val.substr(0, val.size() - 1);
	return true;
}

std::string SWFTagMetadata::get() {
	return value + std::string('\0');
}
