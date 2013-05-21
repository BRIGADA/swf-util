/*
 * main.cpp
 *
 *  Created on: 14.05.2013
 *      Author: brigada
 */


#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <zlib.h>
#include"SWFStructs.h"
#include"DataReader.h"

void SYSERR(const char * sysfunc)
{
	printf("SYSERR: %s: %s (%d)\n", sysfunc, strerror(errno), errno);
	exit(-1);
}


void PRINTBITS(uint8_t * ptr, uint8_t count)
{
	while(count) {
		for(uint8_t i=0; i<8; i++) {
			printf("%d", 1 & (*ptr >> (7 - i)));
		}
		printf("\n");
		ptr++;
		count--;
	}
}
int main(int argc, char * argv[])
{
	int fd = open(argv[1], O_RDONLY);

	if(fd == -1) SYSERR("open");

	struct stat st;

	if(fstat(fd, &st) == -1) SYSERR("fstat");

	SWFHeader_t header;

	read(fd, &header, sizeof(header));

	if(header.signature[2] != 'S' || header.signature[1] != 'W')
	{
		printf("Wrong signature: %3s\n", header.signature);
		exit(-1);
	}

	unsigned char * buf = new unsigned char[header.filesize - sizeof(SWFHeader_t)];;

	switch(header.signature[0])
	{
	case 'F': // uncompressed
		printf("Unkonmpessed\n");

		read(fd, buf, header.filesize - sizeof(SWFHeader_t));
		close(fd);

		break;
	case 'C': // ZLIB
	{
		printf("ZLIB compression\n");
		unsigned char * buf_c = new unsigned char[st.st_size - sizeof(SWFHeader_t)];
		read(fd, buf_c, st.st_size - sizeof(SWFHeader_t));
		close(fd);

		z_stream strm;

	    strm.zalloc = Z_NULL;
	    strm.zfree = Z_NULL;
	    strm.opaque = Z_NULL;
	    strm.next_in = buf_c;
	    strm.avail_in = st.st_size - sizeof(SWFHeader_t);
	    strm.next_out = buf;
	    strm.avail_out = header.filesize - sizeof(SWFHeader_t);

	    int zerr;

	    zerr = inflateInit2(&strm, 15);
	    if(zerr != Z_OK) {
	    	printf("ZLIB: inflateInit2: %d\n", zerr);
	    	delete [] buf_c;
	    	delete [] buf;
	    	exit(-1);
	    }

	    zerr = inflate(&strm, Z_NO_FLUSH);

	    if(zerr != Z_STREAM_END) {
	    	printf("ZLIB: inflate: %d\n", zerr);
	    	inflateEnd(&strm);
	    	delete [] buf_c;
	    	delete [] buf;
	    	exit(-1);
	    }

	    inflateEnd(&strm);
		delete [] buf_c;

		int ofd = open("decompressed.swf", O_WRONLY | O_CREAT | O_TRUNC);
		write(ofd, buf, header.filesize - sizeof(SWFHeader_t));
		close(ofd);

		break;
	}
	default:
		printf("Unknown compression method: '%c'\n", header.signature[0]);

		exit(-1);
	}

	printf("done\n");


//	uint8_t bbb[] = { 0x5D, 0x04, 0x0f, 0xe0, 0x3d, 0x01, 0 };

	PRINTBITS(buf, 10);

	DataReader d(buf, header.filesize - sizeof(SWFHeader_t));

	SWFRect_t FrameSize = d.readRECT();

	printf("x: %d...%d\n", FrameSize.Xmin, FrameSize.Xmax);
	printf("y: %d...%d\n", FrameSize.Ymin, FrameSize.Ymax);

	uint16_t FrameRate = d.readUI16();

	printf("FrameRate = %#X\n", FrameRate);

	uint16_t FrameCount = d.readUI16();

	printf("FrameCount = %u\n", FrameCount);

	int index =0;
	while(!d.eof())
	{
		uint16_t TagCodeAndLength = d.readUI16();
//		printf("tag = %u\n", TagCodeAndLength);

		uint16_t TagType = TagCodeAndLength >> 6;
//		printf("tag.type = %u\n", TagType);

		uint32_t TagLength = TagCodeAndLength & 0x003f;

		if(TagLength == 0x3f) {
			TagLength = d.readUI32();
		}
//		printf("tag.length = %u\n", TagLength);

		std::string TagData = d.read(TagLength);

		printf("%#X - ", TagType);
		switch(TagType)
		{
		case   0:
			printf("End");
			break;

		case   1:
			printf("ShowFrame");
			break;

		case   2:
			printf("DefineShape");
			break;

		case   3:
			printf("FreeCharacter");
			break;

		case   4:
			printf("PlaceObject");
			break;

		case   5:
			printf("RemoveObject");
			break;

		case   6:
			printf("DefineBits");
			break;

		case   7:
			printf("DefineButton");
			break;

		case   8:
			printf("JPEGTables");
			break;

		case   9:
			printf("SetBackgroundColor");
			break;

		case  10:
			printf("DefineFont");
			break;

		case  11:
			printf("DefineText");
			break;

		case  12:
			printf("DoAction");
			break;

		case  13:
			printf("DefineFontInfo");
			break;

		case  14:
			printf("DefineSound");
			break;

		case  15:
			printf("StartSound");
			break;

		case  17:
			printf("DefineButtonSound");
			break;

		case  18:
			printf("SoundStreamHead");
			break;

		case  19:
			printf("SoundStreamBlock");
			break;

		case  20:
			printf("DefineBitsLossless");
			break;

		case  21:
			printf("DefineBitsJPEG2");
			break;

		case  22:
			printf("DefineShape2");
			break;

		case  23:
			printf("DefineButtonCxform");
			break;

		case  24:
			printf("Protect");
			break;

		case  25:
			printf("PathsArePostScript");
			break;

		case  26:
			printf("PlaceObject2");
			break;

		case  28:
			printf("RemoveObject2");
			break;

		case  32:
			printf("DefineShape3");
			break;

		case  33:
			printf("DefineText2");
			break;

		case  34:
			printf("DefineButton2");
			break;

		case  35:
			printf("DefineBitsJPEG3");
			break;

		case  36:
			printf("DefineBitsLossless2");
			break;

		case  39:
			printf("DefineSprite");
			break;

		case  41:
			printf("ProductInfo");
			break;

		case  43:
			printf("FrameLabel");
			break;

		case  45:
			printf("SoundStreamHead2");
			break;

		case  46:
			printf("DefineMorphShape");
			break;

		case  48:
			printf("DefineFont2");
			break;

		case  37:
			printf("DefineEditText");
			break;

		case  56:
			printf("ExportAssets");
			break;

		case  57:
			printf("ImportAssets");
			break;

		case  58:
			printf("EnableDebugger");
			break;

		case  59:
			printf("DoInitAction");
			break;

		case  60:
			printf("DefineVideoStream");
			break;

		case  61:
			printf("VideoFrame");
			break;

		case  62:
			printf("DefineFontInfo2");
			break;

		case  63:
			printf("DebugID");
			break;

		case  64:
			printf("EnableDebugger2");
			break;

		case  65:
			printf("ScriptLimits");
			break;

		case  66:
			printf("SetTabIndex");
			break;

		case  69:
			printf("FileAttributes");
			break;

		case  70:
			printf("PlaceObject3");
			break;

		case  71:
			printf("ImportAssets2");
			break;

		case  72:
			printf("DoABC");
			break;

		case  73:
			printf("DefineFontAlignZones");
			break;

		case  74:
			printf("CSMTextSettings");
			break;

		case  75:
			printf("DefineFont3");
			break;

		case  76:
			printf("SymbolClass");
			break;

		case  77:
			printf("Metadata");
			break;

		case  78:
			printf("DefineScalingGrid");
			break;

		case  82:
			printf("DoABC2");
			break;

		case  83:
			printf("DefineShape4");
			break;

		case  84:
			printf("DefineMorphShape2");
			break;

		case  86:
			printf("DefineSceneAndFrameLabelData");
			break;

		case  87:
			printf("DefineBinaryData");
			break;

		case  88:
			printf("DefineFontName");
			break;

		case 91:
			printf("DefineFont4");
			break;
		default:
			printf("UNKNOWN");
		}

		printf(" %u\n", TagLength);

		char o_name[7] = {0};
		sprintf(o_name, "%02x.dat", index);
		
		int ofd = open(o_name, O_WRONLY|O_CREAT|O_TRUNC);
		write(ofd, TagData.data(), TagData.size());
		close(ofd);
		index++;
	}



	if(buf) {
		delete [] buf;
	}

	return 0;
}

