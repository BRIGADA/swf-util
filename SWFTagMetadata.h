/*
 * SWFTagMetadata.h
 *
 *  Created on: 22.05.2013
 *      Author: brigada
 */

#ifndef SWFTAGMETADATA_H_
#define SWFTAGMETADATA_H_
#include <string>
#include <inttypes.h>

class SWFTagMetadata {
public:
	std::string value;

	SWFTagMetadata();
	virtual ~SWFTagMetadata();

	uint32_t size();
	bool set(std::string val);
	std::string get();
};

#endif /* SWFTAGMETADATA_H_ */
