/*
 * SWFTag.h
 *
 *  Created on: 21.05.2013
 *      Author: brigada
 */

#ifndef SWFTAG_H_
#define SWFTAG_H_
#include <string>
#include <inttypes.h>

class SWFTag
{
public:
	uint8_t type;
	std::string content;

	SWFTag();
	virtual ~SWFTag();
};

#endif /* SWFTAG_H_ */
