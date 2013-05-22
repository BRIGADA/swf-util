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

typedef std::vector<SWFTag *> SWFTagList;

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

	SWFTagList tags;

	SWFFile();
	virtual ~SWFFile();
	bool load(std::string filename);
	bool save(std::string filename);
private:
	bool _parse(char * buf, uint32_t size);
};

#endif /* SWFFILE_H_ */
