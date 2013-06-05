/*
 * main.cpp
 *
 *  Created on: 14.05.2013
 *      Author: brigada
 */

#include <stdarg.h>
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
#include "ABCReader.h"
#include "utils.h"
#include "ABCOP.h"
#include "ABCVM.h"
#include <map>
#include <list>
#include <algorithm>

bool writefile(std::string filename, std::string& content) {
    int fd = open(filename.data(), O_WRONLY | O_CREAT | O_TRUNC, 00666);
    if (fd == -1) return false;
    write(fd, content.data(), content.size());
    close(fd);
    return true;
}

void SYSERR(const char * sysfunc) {
    printf("SYSERR: %s: %s (%d)\n", sysfunc, strerror(errno), errno);
    exit(-1);
}

void PRINTBITS(uint8_t * ptr, uint8_t count) {
    while (count) {
        for (uint8_t i = 0; i < 8; i++) {
            printf("%d", 1 & (*ptr >> (7 - i)));
        }
        printf("\n");
        ptr++;
        count--;
    }
}

class A 
{ 
public: 
    virtual void t(int a = 2) {std::cout << "A: "<<a;} 
}; 

class B : public A 
{ 
public: 
    void t(int b = 8) {std::cout << "B: "<<b;} 
}; 

int main(int argc, char * argv[]) {
        static_cast< B * >(new A())->t(); 
                return 0; 
 
    if (argc == 1) return -1;
    SWFFile swf;
    if (swf.load(argv[1])) {
        // unsigned int abccount = 0;
        for (SWFTagList::iterator it = swf.tags.begin(); it != swf.tags.end(); ++it) {
            switch ((*it)->type) {
                case 69:
                {
                    printf("FileAttributes\n");
                    SWFTagFileAttributes t;
                    if (t.set((*it)->content)) {
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
                    int p = (*it)->content.find_first_of('\0', 4);
                    ABCFile * abc = ABCReader((*it)->content.data() + p + 1, (*it)->content.length() - p - 1).read();

                    if (abc) {
                        printf("version: %u.%u\n", abc->versionMajor, abc->versionMinor);
                        printf("   ints: %lu\n", abc->cpool.ints.size());
                        printf("  uints: %lu\n", abc->cpool.uints.size());
                        printf("doubles: %lu\n", abc->cpool.doubles.size());
                        printf("strings: %lu\n", abc->cpool.strings.size());
                        printf("     ns: %lu\n", abc->cpool.namespaces.size());
                        printf("    sns: %lu\n", abc->cpool.namespaceSets.size());
                        printf("  names: %lu\n", abc->cpool.multinames.size());
                        printf("methods: %lu\n", abc->methods.size());
                        printf("  metas: %lu\n", abc->metadatas.size());
                        printf("  insts: %lu\n", abc->instances.size());
                        printf("classes: %lu\n", abc->classes.size());
                        printf("scripts: %lu\n", abc->scripts.size());
                        printf(" bodies: %lu\n", abc->bodies.size());

                        uint32_t i = 0;
                        for (ABCBodyList::iterator it = abc->bodies.begin(); it != abc->bodies.end(); ++it, ++i) {
                            std::string fn = stringf("%s.%04x.body", argv[1], i);
                            DEBUG("%s", fn.data());
                            writefile(fn, (*it).code);

                            bool stop = false;

                            std::map<uint32_t, std::string> ops;
                            std::vector<bool> used;
                            used.resize((*it).code.size());
                            std::list<uint32_t> jumps;

                            ABCVM VM(abc->cpool);

                            VM.disassemble((*it).code, 0, ops, used, jumps);

                            
                            if(!(*it).exceptions.empty()) {                            
                                DEBUG("EXCEPTION COUNT: %u", (*it).exceptions.size());
                                for(ABCExceptionList::iterator e = (*it).exceptions.begin(); e != (*it).exceptions.end(); ++e) {
                                    DEBUG("[%u ... %u]->%u, type=%s, name=%s", (*e).from, (*e).to, (*e).target, (*e).type ? abc->cpool.getName((*e).type).data() : "*", (*e).name ? abc->cpool.getName((*e).name).data() : "*");
                                    jumps.push_back(e->target);
                                    VM.disassemble((*it).code, e->target, ops, used, jumps);
//                                    DEBUG("from=%u, to=%u, target=%u, type=%s, name=%s", (*e).from, (*e).to, (*e).target, abc->cpool.getSTR((*e).type).data(), abc->cpool.getSTR((*e).name).data());
                                }
                            }
                            
                            DEBUG("USED:");
                            uint32_t i = 0;
                            while (i < used.size()) {
                                printf("%c", used[i] ? '#' : '%');
                                ++i;
                                if (!(i % 32)) printf("\n");
                            }
                            printf("\n");

                            DEBUG("LISTING (%u bytes):", (*it).code.size());
                            for (std::map<uint32_t, std::string>::iterator li = ops.begin(); li != ops.end(); ++li) {
                                if (std::find(jumps.begin(), jumps.end(), (*li).first) != jumps.end()) {
                                    DEBUG("L%u:", (*li).first);
                                }
                                DEBUG("\t%s", (*li).second.data());
                                //                                DEBUG("%4u\t%s", (*li).first, (*li).second.data());
                            }



                            if (stop) exit(-1);

                            DEBUG(std::string(40, '-'));
                        }

                        //                        for (ABCInstanceList::iterator it = abc->instances.begin(); it != abc->instances.end(); ++it) {
                        //                            if (abc->cpool.multinames[(*it).name].kind == CONSTANT_QName) {
                        //                                printf("package %s\n", abc->cpool.strings[abc->cpool.namespaces[abc->cpool.multinames[(*it).name].ns].name].data());
                        //                                printf("{\n");
                        //                                if ((*it).super) {
                        //                                    printf("  class %s extends %s\n", abc->cpool.strings[abc->cpool.multinames[(*it).name].name].data(), abc->cpool.strings[abc->cpool.multinames[(*it).super].name].data());
                        //                                } else {
                        //                                    printf("  class %s\n", abc->cpool.strings[abc->cpool.multinames[(*it).name].name].data());
                        //                                }
                        //                                printf("}\n\n");
                        //                            } else throw "Unknown class name type";
                        //                        }
                    }
                    break;
                }
                default:
                    printf("%d - %lu\n", (*it)->type, (*it)->content.size());
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

