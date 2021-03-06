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

void saveXML(ABCFile *abc);

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

int main(int argc, char * argv[]) {


    if (argc == 1) return -1;
    SWFFile swf;
    if (swf.load(argv[1])) {
        // unsigned int abccount = 0;
        for (SWFTagList::iterator tag = swf.tags.begin(); tag != swf.tags.end(); ++tag) {
            switch ((*tag)->type) {
                case 69:
                {
                    DEBUG("FileAttributes");
                    SWFTagFileAttributes t;
                    if (t.set((*tag)->content)) {
                        DEBUG("\tUseDirectBlit: %d", t.useDirectBlit);
                        DEBUG("\t       UseGPU: %d", t.useGPU);
                        DEBUG("\t  HasMetadata: %d", t.hasMetadata);
                        DEBUG("\tActionScript3: %d", t.actionScript3);
                        DEBUG("\t   UseNetwork: %d", t.useNetwork);
                    }
                    break;
                }
                case 82:
                {
                    DEBUG("DoABC2");

                    int p = (*tag)->content.find_first_of('\0', 4);
                    ABCFile * abc = ABCReader((*tag)->content.data() + p + 1, (*tag)->content.length() - p - 1).read();

                    if (abc) {
                        DEBUG("version: %u.%u", abc->versionMajor, abc->versionMinor);
                        DEBUG("   ints: %u", abc->cpool.ints.size());
                        DEBUG("  uints: %u", abc->cpool.uints.size());
                        DEBUG("doubles: %u", abc->cpool.doubles.size());
                        DEBUG("strings: %u", abc->cpool.strings.size());
                        DEBUG("     ns: %u", abc->cpool.namespaces.size());
                        DEBUG("    nss: %u", abc->cpool.namespaceSets.size());
                        DEBUG("  names: %u", abc->cpool.multinames.size());
                        DEBUG("methods: %u", abc->methods.size());
                        DEBUG("  metas: %u", abc->metadatas.size());
                        DEBUG("  insts: %u", abc->instances.size());
                        DEBUG("classes: %u", abc->classes.size());
                        DEBUG("scripts: %u", abc->scripts.size());
                        DEBUG(" bodies: %u", abc->bodies.size());

                        DEBUG("INTS");
                        for (uint32_t i = 0; i < abc->cpool.ints.size(); ++i) {
                            DEBUG("%8u: %d", i, abc->cpool.ints[i]);
                        }

                        DEBUG("UINTS");
                        for (uint32_t i = 0; i < abc->cpool.uints.size(); ++i) {
                            DEBUG("%8u: %u", i, abc->cpool.uints[i]);
                        }

                        DEBUG("DOUBLES");
                        for (uint32_t i = 0; i < abc->cpool.doubles.size(); ++i) {
                            DEBUG("%8u: %f", i, abc->cpool.doubles[i]);
                        }

                        DEBUG("STRINGS");
                        for (uint32_t i = 0; i < abc->cpool.strings.size(); ++i) {
                            DEBUG("%8u: '%s'", i, string_escape(abc->cpool.strings[i]).data());
                        }

                        //                        DEBUG("NAMESPACES");
                        //                        for(uint32_t i = 0; i < abc->cpool.namespaces.size(); ++i) {
                        //                            DEBUG("%8u: %s", i, abc->cpool.getNS(i).data());
                        //                        }
                        //                        
                        //                        DEBUG("NAMESPACE SETS");
                        //                        for(uint32_t i = 0; i < abc->cpool.namespaceSets.size(); ++i) {
                        //                            DEBUG("%8u: %s", i, abc->cpool.getNSS(i).data());
                        //                        }

                        DEBUG("METHODS");
                        for (uint32_t i = 0; i < abc->methods.size(); ++i) {
                            DEBUG("%6u: %s", i, abc->methods[i].name ? abc->cpool.getString(abc->methods[i].name).data() : "<NONAME>");
                            for (uint32_t j = 0; j < abc->methods[i].params.size(); ++j) {
                                if ((abc->methods[i].flags & METHOD_FLAG_HAS_OPTIONAL) && (j >= abc->methods[i].params.size() - abc->methods[i].options.size())) {
                                    DEBUG("\t%s:%s = %s",
                                            abc->methods[i].params[j].name ? abc->cpool.getMultinameHRF(abc->methods[i].params[j].name).data() : stringf("PARAM_%u", j).data(),
                                            abc->cpool.getMultinameHRF(abc->methods[i].params[j].type).data(), 
                                            abc->cpool.getMethodOptionHRF(abc->methods[i].options[j - (abc->methods[i].params.size() - abc->methods[i].options.size())]).data());
                                } else {
                                    DEBUG("\t%s:%s",
                                            abc->methods[i].params[j].name ? abc->cpool.getMultinameHRF(abc->methods[i].params[j].name).data() : stringf("PARAM_%u", j).data(),
                                            abc->cpool.getMultinameHRF(abc->methods[i].params[j].type).data());
                                }
                            }
                            DEBUG("\treturn %s", abc->cpool.getMultinameHRF(abc->methods[i].returnType).data());
                        }

                        //                        for(uint i = 0; i < abc->cpool.multinames.size(); ++i) {
                        //                            DEBUG("MULTINAME (%u)", i);
                        //                            DEBUG("\tkind: %u", abc->cpool.multinames[i].kind);
                        //                        }

                        if (!abc->metadatas.empty()) {
                            DEBUG("METADATAS");
                            for (ABCMetadataList::iterator it = abc->metadatas.begin(); it != abc->metadatas.end(); ++it) {
                                DEBUG("\tname: %u, items: %u", it->name, it->items.size());
                            }
                        }

                        if (!abc->instances.empty()) {
                            DEBUG("INSTANCES:");
                            uint32_t i = 0;
                            for (ABCInstanceList::iterator instance = abc->instances.begin(); instance != abc->instances.end(); ++instance, ++i) {
                                if (abc->cpool.multinames[instance->name].kind != 7) throw "wrong kind";
                                DEBUG("\tname:  %s", abc->cpool.getMultiname(instance->name).data());
                                if (instance->super) {
                                    DEBUG("\tsuper: %s", abc->cpool.getMultiname(instance->super).data());
                                }

                                ABCStringList flags;
                                if (instance->flags & INSTANCE_FLAG_FINAL) flags.push_back("FINAL");
                                if (instance->flags & INSTANCE_FLAG_INTERFACE) flags.push_back("INTERFACE");
                                if (instance->flags & INSTANCE_FLAG_PROTECTED_NS) flags.push_back("PROTECTED_NS");
                                if (instance->flags & INSTANCE_FLAG_SEALED) flags.push_back("SEALED");

                                DEBUG("\tflags: %s", implode(", ", flags).data());
                                if (instance->flags & INSTANCE_FLAG_PROTECTED_NS) {
                                    DEBUG("\tprotected_ns: %s", abc->cpool.getNamespace(instance->protectedNS).data());
                                }

                                if (!instance->interfaces.empty()) {
                                    DEBUG("\tinterfaces:");
                                    for (uint32_t i = 0; i < instance->interfaces.size(); ++i) {
                                        DEBUG("\t\t%u: %s", i, abc->cpool.getMultiname(instance->interfaces[i]).data());
                                    }
                                }

                                DEBUG("\tiinit: %u", instance->iinit);

                                if (!instance->traits.empty()) {
                                    DEBUG("\ttraits:");
                                    for (uint32_t i = 0; i < instance->traits.size(); ++i) {
                                        DEBUG("\t\t%u:", i);
                                        DEBUG("\t\t\tname: %s", abc->cpool.getMultiname(instance->traits[i].name).data());
                                        DEBUG("\t\t\tkind: %s", instance->traits[i].kindString().data());
                                        if (!instance->traits[i].getAttrs().empty()) {
                                            DEBUG("\t\t\tattrs: %s", implode(", ", instance->traits[i].getAttrs()).data());
                                        }
                                        continue;
                                        if ((instance->traits[i].kind & 0x0f) == TRAIT_KIND_SLOT) {
                                            DEBUG("\t\t\t\tslot_id: %u", instance->traits[i].Slot.slotID);
                                            if (instance->traits[i].Slot.typeName) {
                                                DEBUG("\t\t\t\ttype_name: %s", abc->cpool.getMultiname(instance->traits[i].Slot.typeName).data());
                                            }
                                            DEBUG("\t\t\t\tvindex: %u", instance->traits[i].Slot.vindex);
                                            DEBUG("\t\t\t\tvkind: %u", instance->traits[i].Slot.vkind);
                                        }
                                    }
                                }

                                DEBUG("");
                            }
                        }
                        continue;


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


                            if (!(*it).exceptions.empty()) {
                                DEBUG("EXCEPTION COUNT: %u", (*it).exceptions.size());
                                for (ABCExceptionList::iterator e = (*it).exceptions.begin(); e != (*it).exceptions.end(); ++e) {
                                    DEBUG("[%u ... %u]->%u, type=%s, name=%s", (*e).from, (*e).to, (*e).target, (*e).type ? abc->cpool.getMultiname((*e).type).data() : "*", (*e).name ? abc->cpool.getMultiname((*e).name).data() : "*");
                                    jumps.push_back(e->from);
                                    jumps.push_back(e->to);
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
                                    DEBUG("L%u:\t%s", (*li).first, li->second.data());
                                } else {
                                    DEBUG("\t%s", (*li).second.data());
                                }
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
                    DEBUG("%s (%u bytes)", TagTypeAsString((*tag)->type).data(), (*tag)->content.size());
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

uint32_t level = 0;

class XMLAttrList : public ABCStringList {
public:

    XMLAttrList& set(std::string name, std::string value) {
        this->push_back(stringf("%s=\"%s\"", name.data(), value.data()));
        return *this;
    }

    XMLAttrList& set(std::string name, uint32_t value) {
        this->push_back(stringf("%s=\"%u\"", name.data(), value));
        return *this;
    }

    XMLAttrList& set(std::string name, int32_t value) {
        this->push_back(stringf("%s=\"%d\"", name.data(), value));
        return *this;
    }
};

void CONTENTTAG(std::string name, std::string content, XMLAttrList attrs = XMLAttrList());
void TAG(std::string name, XMLAttrList attrs = XMLAttrList());
void OPENTAG(std::string name, XMLAttrList attrs = XMLAttrList());
void CLOSETAG(std::string name);

void saveXML(ABCFile *abc) {
    DEBUG("<?xml version\"1.0\" ?>");
    OPENTAG("ABC", XMLAttrList().set("versionMajor", abc->versionMajor).set("versionMinor", abc->versionMinor));
    OPENTAG("CPool");
    OPENTAG("integers");
    for (uint32_t i = 0; i < abc->cpool.ints.size(); ++i) {
        CONTENTTAG("item", stringf("%d", abc->cpool.ints[i]), XMLAttrList().set("id", i));
    }
    CLOSETAG("integers");
    CLOSETAG("CPool");
    CLOSETAG("ABC");
}

void CONTENTTAG(std::string name, std::string content, XMLAttrList attrs) {
    if (attrs.empty()) {
        DEBUG("%s<%s>%s</%s>", std::string(level, '\t').data(), name.data(), content.data(), name.data());
    } else {
        DEBUG("%s<%s %s>%s</%s>", std::string(level, '\t').data(), name.data(), implode(" ", attrs).data(), content.data(), name.data());
    }
}

void TAG(std::string name, XMLAttrList attrs) {
    if (attrs.empty()) {
        DEBUG("%s<%s/>", std::string(level, '\t').data(), name.data());
    } else {
        DEBUG("%s<%s %s/>", std::string(level, '\t').data(), name.data(), implode(" ", attrs).data());
    }
}

void OPENTAG(std::string name, XMLAttrList attrs) {
    if (attrs.empty()) {
        DEBUG("%s<%s>", std::string(level, '\t').data(), name.data());
    } else {
        DEBUG("%s<%s %s>", std::string(level, '\t').data(), name.data(), implode(" ", attrs).data());
    }
    level++;
}

void CLOSETAG(std::string name) {
    level--;
    DEBUG("%s</%s>", std::string(level, '\t').data(), name.data());
}