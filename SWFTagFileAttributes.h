/*
 * SWFTagFileAttributes.h
 *
 *  Created on: 22.05.2013
 *      Author: brigada
 */

#ifndef SWFTAGFILEATTRIBUTES_H_
#define SWFTAGFILEATTRIBUTES_H_
#include <string>

class SWFTagFileAttributes {
public:

	bool useDirectBlit;
	bool useGPU;
	bool hasMetadata;
	bool actionScript3;
	bool useNetwork;

	SWFTagFileAttributes();
	virtual ~SWFTagFileAttributes();

	bool set(std::string data, bool strict = true);
};

#endif /* SWFTAGFILEATTRIBUTES_H_ */
