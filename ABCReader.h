/*
 * ABCReader.h
 *
 *  Created on: 25.05.2013
 *      Author: brigada
 */

#ifndef ABCREADER_H_
#define ABCREADER_H_

#include <inttypes.h>
#include <string>
#include "ABCTypes.h"
#include "ABCFile.h"

class ABCReader {
public:
    ABCReader(const char * data, uint32_t size);
    ABCReader(const std::string& data);
    virtual ~ABCReader();

    ABCFile * read();
    uint8_t readU8(); // 1 byte fixed length unsigned integer
    uint16_t readU16(); // 2 byte fixed length unsigned integer
    uint32_t readU30(); // 1...5 bytes variable length unsigned integer
    uint32_t readU32(); // 1...5 bytes variable length unsigned integer

    int32_t readS24(); // 3-byte fixed length signed integer
    int32_t readS32(); // 1...5 bytes variable length signed integer

    double readD64(); // 8-byte IEEE-754 floating point
    
    uint32_t offset();
    void offset(uint32_t value);
    bool eof();

private:
    void readString(std::string& res);

    void readTrait(ABCTrait& res);
    void readTraitList(ABCTraitList& res);

protected:
    const char * _start;
    const char * _cur;
    const char * _end;
};
#endif /* ABCREADER_H_ */
