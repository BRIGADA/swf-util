/*
 * DataReader.h
 *
 *  Created on: 19.05.2013
 *      Author: brigada
 */

#ifndef DATAREADER_H_
#define DATAREADER_H_

#include <inttypes.h>
#include "SWFStructs.h"
#include <string>

class DataReader {
public:
	DataReader(const char * buf, uint32_t size);
	virtual ~DataReader();

	void align();

	uint32_t readUB(uint8_t bits);
	 int32_t readSB(uint8_t bits);

	SWFRect_t readRECT();
	     void readRECT(SWFRect_t &result);

	uint16_t readUI16();
	uint32_t readUI29();
	uint32_t readUI32();

	uint8_t readU8();
	uint32_t readU32();
	uint32_t readU30();

	std::string readString();

	double readD64();

	std::string read(uint32_t length);

	bool eof();


protected:
	const char * _cur;
	const char * _end;
	uint8_t _bit;
};

#endif /* DATAREADER_H_ */
