/*
 * DataReader.cpp
 *
 *  Created on: 19.05.2013
 *      Author: brigada
 */

#include "DataReader.h"

DataReader::DataReader(const char* buf, uint32_t size) : _cur(buf), _end(buf+size), _bit(0){
}

DataReader::~DataReader() {
}

void DataReader::align() {
	if(_bit) {
		_cur++;
		_bit = 0;
	}
}

uint32_t DataReader::readUB(uint8_t bits) {
	uint32_t result = 0;

	if(_bit + bits > 8) {
		result = (*_cur) & (0xff >> _bit);

		_cur++;
		bits -= (8 - _bit);
		_bit = 0;

		while(bits >= 8) {
			result = (result << 8) | (*_cur);
			_cur++;
			bits -= 8;
		}

		result = (result << bits) | (*_cur >> (8 - bits));
		_bit = bits;
	}
	else {
		result = ((*_cur) & (0xff >> _bit)) >> (8 - _bit - bits);
		_cur += (_bit + bits) / 8;
		_bit = (_bit + bits) % 8;
	}

	return result;
}

int32_t DataReader::readSB(uint8_t bits) {
	uint32_t v = readUB(bits);
	return (v >> (bits - 1)) ? ((-1 << bits) | v) : v;
}

SWFRect_t DataReader::readRECT() {
	SWFRect_t result;
	readRECT(result);
	return result;
}

void DataReader::readRECT(SWFRect_t& result) {
	uint32_t nbits = readUB(5);
	result.Xmin = readSB(nbits);
	result.Xmax = readSB(nbits);
	result.Ymin = readSB(nbits);
	result.Ymax = readSB(nbits);
}

uint16_t DataReader::readUI16() {
	align();
	uint16_t result = *((uint16_t *)_cur);
	_cur += 2;
	return result;
}

uint32_t DataReader::readUI29()
{
	uint32_t result = *_cur;
	_cur++;

	if (result >> 7)
	{
		result = (result & 0x0000007f) | ((uint32_t)*_cur) << 7;
		_cur++;

		if (result >> 14)
		{
			result = (result & 0x00003fff) | ((uint32_t)*_cur) << 14;
			_cur++;

			if (result >> 21)
			{
				result = (result & 0x001fffff) | ((uint32_t)*_cur) << 21;
				_cur++;

				if (result >> 28)
				{
					result = (result & 0x0fffffff) | ((uint32_t)*_cur) << 28;
					_cur++;
				}
			}
		}
	}

	return result;
}

uint32_t DataReader::readUI32() {
	align();
	uint32_t result = *((uint32_t *)_cur);
	_cur += 4;
	return result;
}

uint32_t DataReader::readU32() {
	uint32_t result = *_cur;
	_cur++;
	if(result & 0x00000080) {
		result = (result & 0x0000007f) | (*_cur << 7);
		_cur++;
		if(result & 0x00004000) {
			result = (result & 0x00003fff) | (*_cur << 14);
			_cur++;
			if(result & 0x00200000) {
				result = (result & 0x001fffff) | (*_cur << 21);
				_cur++;
				if(result & 0x10000000) {
					result = (result & 0x0fffffff) | (*_cur << 28);
					_cur++;
				}
			}
		}
	}
	return result;
}

uint32_t DataReader::readU30() {
	return readU32() & 0x3fffffff;
}

double DataReader::readD64() {
	double result = *((double *)_cur);
	_cur+=8;
	return result;
}

std::string DataReader::read(uint32_t length)
{
	std::string result((char *)_cur, length);
	_cur += length;
	return result;
}

std::string DataReader::readString() {
	return read(readU30());
}

uint8_t DataReader::readU8() {
	uint8_t result = *(uint8_t*)_cur;
	_cur++;
	return result;
}

bool DataReader::eof() {
	return _cur >= _end;
}
