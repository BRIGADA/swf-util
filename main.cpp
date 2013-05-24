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
#include "SWFFile.h"
#include "SWFTagFileAttributes.h"
#include "ABCFile.h"
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
	SWFFile swf;
	if(swf.load(argv[1]))
	{
		unsigned int abccount = 0;
		for(SWFTagList::iterator it = swf.tags.begin(); it != swf.tags.end(); ++it)
		{
			switch((*it)->type)
			{
			case 69:
			{
				printf("FileAttributes\n");
				SWFTagFileAttributes t;
				if(t.set((*it)->content))
				{
					printf("\tUseDirectBlit: %d\n", t.useDirectBlit);
					printf("\t       UseGPU: %d\n", t.useGPU);
					printf("\t  HasMetadata: %d\n", t.hasMetadata);
					printf("\tActionScript3: %d\n", t.actionScript3);
					printf("\t   UseNetwork: %d\n", t.useNetwork);
				}
				break;
			}
			case 82:
			{
				printf("DoABC2\n");
//				char oname[1024];
				int p = (*it)->content.find_first_of('\0',4);
//				sprintf(oname, "%s-%d.abc", argv[1], abccount++);
//				int ofd = creat(oname, 00666);
//				write(ofd, (*it)->content.data() + p + 1, (*it)->content.size() - p - 1);
//				close(ofd);

				ABCFile abc;
				abc.parse((*it)->content.substr(p + 1));
				break;
			}
			default:
				printf("%d - %u\n", (*it)->type, (*it)->content.size());
			}
		}

	}

//	switch(TagType)
//	{
//	case   0:
//		printf("End");
//		break;
//
//	case   1:
//		printf("ShowFrame");
//		break;
//
//	case   2:
//		printf("DefineShape");
//		break;
//
//	case   3:
//		printf("FreeCharacter");
//		break;
//
//	case   4:
//		printf("PlaceObject");
//		break;
//
//	case   5:
//		printf("RemoveObject");
//		break;
//
//	case   6:
//		printf("DefineBits");
//		break;
//
//	case   7:
//		printf("DefineButton");
//		break;
//
//	case   8:
//		printf("JPEGTables");
//		break;
//
//	case   9:
//		printf("SetBackgroundColor");
//		break;
//
//	case  10:
//		printf("DefineFont");
//		break;
//
//	case  11:
//		printf("DefineText");
//		break;
//
//	case  12:
//		printf("DoAction");
//		break;
//
//	case  13:
//		printf("DefineFontInfo");
//		break;
//
//	case  14:
//		printf("DefineSound");
//		break;
//
//	case  15:
//		printf("StartSound");
//		break;
//
//	case  17:
//		printf("DefineButtonSound");
//		break;
//
//	case  18:
//		printf("SoundStreamHead");
//		break;
//
//	case  19:
//		printf("SoundStreamBlock");
//		break;
//
//	case  20:
//		printf("DefineBitsLossless");
//		break;
//
//	case  21:
//		printf("DefineBitsJPEG2");
//		break;
//
//	case  22:
//		printf("DefineShape2");
//		break;
//
//	case  23:
//		printf("DefineButtonCxform");
//		break;
//
//	case  24:
//		printf("Protect");
//		break;
//
//	case  25:
//		printf("PathsArePostScript");
//		break;
//
//	case  26:
//		printf("PlaceObject2");
//		break;
//
//	case  28:
//		printf("RemoveObject2");
//		break;
//
//	case  32:
//		printf("DefineShape3");
//		break;
//
//	case  33:
//		printf("DefineText2");
//		break;
//
//	case  34:
//		printf("DefineButton2");
//		break;
//
//	case  35:
//		printf("DefineBitsJPEG3");
//		break;
//
//	case  36:
//		printf("DefineBitsLossless2");
//		break;
//
//	case  39:
//		printf("DefineSprite");
//		break;
//
//	case  41:
//		printf("ProductInfo");
//		break;
//
//	case  43:
//		printf("FrameLabel");
//		break;
//
//	case  45:
//		printf("SoundStreamHead2");
//		break;
//
//	case  46:
//		printf("DefineMorphShape");
//		break;
//
//	case  48:
//		printf("DefineFont2");
//		break;
//
//	case  37:
//		printf("DefineEditText");
//		break;
//
//	case  56:
//		printf("ExportAssets");
//		break;
//
//	case  57:
//		printf("ImportAssets");
//		break;
//
//	case  58:
//		printf("EnableDebugger");
//		break;
//
//	case  59:
//		printf("DoInitAction");
//		break;
//
//	case  60:
//		printf("DefineVideoStream");
//		break;
//
//	case  61:
//		printf("VideoFrame");
//		break;
//
//	case  62:
//		printf("DefineFontInfo2");
//		break;
//
//	case  63:
//		printf("DebugID");
//		break;
//
//	case  64:
//		printf("EnableDebugger2");
//		break;
//
//	case  65:
//		printf("ScriptLimits");
//		break;
//
//	case  66:
//		printf("SetTabIndex");
//		break;
//
//	case  69:
//		printf("FileAttributes");
//		break;
//
//	case  70:
//		printf("PlaceObject3");
//		break;
//
//	case  71:
//		printf("ImportAssets2");
//		break;
//
//	case  72:
//		printf("DoABC");
//		break;
//
//	case  73:
//		printf("DefineFontAlignZones");
//		break;
//
//	case  74:
//		printf("CSMTextSettings");
//		break;
//
//	case  75:
//		printf("DefineFont3");
//		break;
//
//	case  76:
//		printf("SymbolClass");
//		break;
//
//	case  77:
//		printf("Metadata");
//		break;
//
//	case  78:
//		printf("DefineScalingGrid");
//		break;
//
//	case  82:
//		printf("DoABC2");
//		break;
//
//	case  83:
//		printf("DefineShape4");
//		break;
//
//	case  84:
//		printf("DefineMorphShape2");
//		break;
//
//	case  86:
//		printf("DefineSceneAndFrameLabelData");
//		break;
//
//	case  87:
//		printf("DefineBinaryData");
//		break;
//
//	case  88:
//		printf("DefineFontName");
//		break;
//
//	case 91:
//		printf("DefineFont4");
//		break;
//	default:
//		printf("UNKNOWN");
//	}

	return 0;
}

