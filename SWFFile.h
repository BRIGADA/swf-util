/*
 * SWFFile.h
 *
 *  Created on: 21.05.2013
 *      Author: brigada
 */

#ifndef SWFFILE_H_
#define SWFFILE_H_

#include <inttypes.h>
#include <vector>
#include "SWFStructs.h"
#include "SWFTag.h"

class SWFFile
{
public:
	enum Compression {
		swfUncompressed = 'F',
		swfZLIB = 'C',
		swfLZMA = 'Z'
	};

	Compression compression;
	uint8_t 	version;

	SWFRect_t 	frameSize;
	uint16_t 	frameRate;
	uint16_t 	frameCount;

	std::vector<SWFTag *> tags;

	SWFFile();
	virtual ~SWFFile();
};

#endif /* SWFFILE_H_ */
