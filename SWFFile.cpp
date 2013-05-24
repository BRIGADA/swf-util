/*
 * SWFFile.cpp
 *
 *  Created on: 21.05.2013
 *      Author: brigada
 */

#include "SWFFile.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <unistd.h>
#include <zlib.h>
#include <stdio.h>
#include "DataReader.h"
#define SWF_USE_ZLIB

SWFFile::SWFFile() : compression(swfUncompressed), version(10), frameRate(0), frameCount(0)
{
}

SWFFile::~SWFFile()
{
	while(tags.size())
	{
		delete tags.front();
		tags.erase(tags.begin());
	}
}

bool SWFFile::load(std::string filename)
{
	int fd = open(filename.data(), O_RDONLY);
	if(fd == -1) return false;

	struct stat st;
	if(fstat(fd, &st) == -1) {
		close(fd);
		return false;
	}

	SWFHeader_t header;
	if(read(fd, &header, sizeof(SWFHeader_t)) != sizeof(SWFHeader_t)) {
		close(fd);
		return false;
	}

	version = header.version;

	// check last 2 bytes of signature
	if(header.signature[1] != 'W' || header.signature[2] != 'S') {
		close(fd);
		return false;
	}

	bool result = true;

	char * buf = new char[header.filesize - sizeof(SWFHeader_t)];

	// check first byte of signature
	switch(header.signature[0])
	{
	case 'F':
		compression = swfUncompressed;
		if(st.st_size != header.filesize || read(fd, buf, header.filesize - sizeof(SWFHeader_t)) + sizeof(SWFHeader_t) != header.filesize ) {
			result = false;
		}
		break;

#ifdef SWF_USE_ZLIB
	case 'C':
	{
		compression = swfZLIB;
		unsigned char * buf_c = new unsigned char[st.st_size - sizeof(SWFHeader_t)];

		int r = read(fd, buf_c, st.st_size - sizeof(SWFHeader_t));

		if(r  + sizeof(SWFHeader_t) - st.st_size != 0) {
			result = false;
		}
		else {
			z_stream strm;

		    strm.zalloc = Z_NULL;
		    strm.zfree = Z_NULL;
		    strm.opaque = Z_NULL;
		    strm.next_in = buf_c;
		    strm.avail_in = st.st_size - sizeof(SWFHeader_t);
		    strm.next_out = reinterpret_cast<unsigned char *>(buf);
		    strm.avail_out = header.filesize - sizeof(SWFHeader_t);

		    if(inflateInit2(&strm, 15) == Z_OK) {
		    	if(inflate(&strm, Z_FINISH) != Z_STREAM_END) {
		    		result = false;
		    	}
			    inflateEnd(&strm);
		    }
		    else {
		    	result = false;
		    }
		}
		delete [] buf_c;
		break;
	}
#endif

#ifdef SWF_USE_LZMA
	case 'Z':
		compression = swfLZMA;
		break;
#endif

	default:
		result = false;
	}

	close(fd);

	if(result) {
		DataReader d(buf, header.filesize - sizeof(SWFHeader_t));

		frameSize = d.readRECT();
		frameRate = d.readUI16();
		frameCount = d.readUI16();

		while(!d.eof())
		{
			SWFTag * tag = new SWFTag();
			uint16_t TagCodeAndLength = d.readUI16();
			uint32_t TagLength = TagCodeAndLength & 0x003f;
			if(TagLength == 0x3f) {
				tag->isExtraLength = true;
				TagLength = d.readUI32();
			}
			tag->type = TagCodeAndLength >> 6;
			tag->content = d.read(TagLength);
			tags.push_back(tag);
		}
	}

	delete [] buf;
	return result;
}

bool SWFFile::save(std::string filename) {

	SWFHeader_t header;


	return false;
}
