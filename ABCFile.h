/*
 * ABCFile.h
 *
 *  Created on: 22.05.2013
 *      Author: brigada
 */

#ifndef ABCFILE_H_
#define ABCFILE_H_
#include <inttypes.h>
#include <string>
#include <vector>

class ABCFile
{
public:
	ABCFile();
	virtual ~ABCFile();

	uint16_t versionMajor;
	uint16_t versionMinor;

	std::vector<int32_t> ints;
	std::vector<uint32_t> uints;
	std::vector<double> doubles;
	std::vector<std::string> strings;






};

#endif /* ABCFILE_H_ */
