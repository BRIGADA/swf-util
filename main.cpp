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
#include <map>


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
                            
                            ABCReader reader((*it).code);

                            std::map<uint32_t, std::string> ops;
                            
                            std::vector<bool> used;
                            used.resize((*it).code.size());
                            
                            ABCUI32List ep; // entryPoints
                            ep.push_back(0);
                            
                            try {
                            
                                while(!ep.empty()) {
                                    uint32_t ip = ep.back();
                                    ep.pop_back();
//                                    DEBUG("IP=%u", ip);

                                    if(ops.find(ip) != ops.end()) continue;                                
                                    reader.pos(ip);                                
                                    uint8_t op = reader.readU8();
                                    switch(op) {
                                        case 0x01: // bkpt
                                        {
                                            ops[ip] = "bkpt";
                                            break;
                                        }
                                        case 0x02: // nop
                                        {
                                            ops[ip] = "nop";
                                            break;
                                        }
                                        case 0x03: // throw
                                        {
                                            ops[ip] = "throw";
                                            break;
                                        }
                                        case 0x04: // getsuper
                                        {
                                            uint32_t index = reader.readU30();
                                            ops[ip] = stringf("getsuper (index=%u)", index);
                                            break;
                                        }
                                        case 0x05: // setsuper
                                        {
                                            uint32_t index = reader.readU30();
                                            ops[ip] = stringf("setsuper (index=%u)", index);
                                            break;
                                        }
                                        case 0x06: // dxns
                                        {
                                            uint32_t index = reader.readU30();
                                            ops[ip] = stringf("dxns (index=%u)", index);
                                            break;
                                        }
                                        case 0x07: // dxnslate
                                        {
                                            ops[ip] = "dxnslate";
                                            break;
                                        }
                                        case 0x08: // kill
                                            {
                                            uint32_t index = reader.readU30();
                                            ops[ip] = stringf("kill (index=%u)", index);
                                            break;
                                        }
                                        case 0x09: // label
                                        {
                                            ops[ip] = "label";
                                            break;
                                        }
                                        case 0x0c: // ifnlt
                                        {
                                            int32_t offset = reader.readS24();
                                            ep.push_back(reader.pos() + offset);
                                            ops[ip] = stringf("ifnlt (offset=%d)", offset);
                                            break;
                                        }
                                        case 0x0d: // ifnle
                                        {
                                            int32_t offset = reader.readS24();
                                            ep.push_back(reader.pos() + offset);
                                            ops[ip] = stringf("ifnle (offset=%d)", offset);
                                            break;
                                        }
                                        case 0x0e: // ifngt
                                        {
                                            int32_t offset = reader.readS24();
                                            ep.push_back(reader.pos() + offset);
                                            ops[ip] = stringf("ifngt (offset=%d)", offset);
                                            break;
                                        }
                                        case 0x0f: // ifnge
                                        {
                                            int32_t offset = reader.readS24();
                                            ep.push_back(reader.pos() + offset);
                                            ops[ip] = stringf("ifnge (offset=%d)", offset);
                                            break;
                                        }
                                        case 0x10: // jump
                                        {
                                            int32_t offset = reader.readS24();
                                            ep.push_back(reader.pos() + offset);
                                            ops[ip] = stringf("jump (offset=%d)", offset);
                                            used[ip + 0] = true;
                                            used[ip + 1] = true;
                                            used[ip + 2] = true;
                                            used[ip + 3] = true;
                                            continue;
                                        }
                                        case 0x11: // iftrue
                                        {
                                            int32_t offset = reader.readS24();
                                            ep.push_back(reader.pos() + offset);
                                            ops[ip] = stringf("iftrue (offset=%d)", offset);
                                            break;
                                        }
                                        case 0x12: // iffalse
                                        {
                                            int32_t offset = reader.readS24();
                                            ep.push_back(reader.pos() + offset);
                                            ops[ip] = stringf("iffalse (offset=%d)", offset);
                                            break;
                                        }
                                        case 0x13: // ifeq
                                        {
                                            int32_t offset = reader.readS24();
                                            ep.push_back(reader.pos() + offset);
                                            ops[ip] = stringf("ifeq (offset=%d)", offset);
                                            break;
                                        }
                                        case 0x14: // ifne
                                        {
                                            int32_t offset = reader.readS24();
                                            ep.push_back(reader.pos() + offset);
                                            ops[ip] = stringf("ifne (offset=%d)", offset);
                                            break;
                                        }
                                        case 0x15: // iflt
                                        {
                                            int32_t offset = reader.readS24();
                                            ep.push_back(reader.pos() + offset);
                                            ops[ip] = stringf("iflt (offset=%d)", offset);
                                            break;
                                        }
                                        case 0x16: // ifle
                                        {
                                            int32_t offset = reader.readS24();
                                            ep.push_back(reader.pos() + offset);
                                            ops[ip] = stringf("ifle (offset=%d)", offset);
                                            break;
                                        }
                                        case 0x17: // ifgt
                                        {
                                            int32_t offset = reader.readS24();
                                            ep.push_back(reader.pos() + offset);
                                            ops[ip] = stringf("ifgt (offset=%d)", offset);
                                            break;
                                        }
                                        case 0x18: // ifge
                                        {
                                            int32_t offset = reader.readS24();
                                            ep.push_back(reader.pos() + offset);
                                            ops[ip] = stringf("ifge (offset=%d)", offset);
                                            break;
                                        }
                                        case 0x19: // ifstricteq
                                        {
                                            int32_t offset = reader.readS24();
                                            ep.push_back(reader.pos() + offset);
                                            ops[ip] = stringf("ifstricteq (offset=%d)", offset);
                                            break;
                                        }
                                        case 0x1a: // ifstrictne
                                        {
                                            int32_t offset = reader.readS24();
                                            ep.push_back(reader.pos() + offset);
                                            ops[ip] = stringf("ifstrictne (offset=%d)", offset);
                                            break;
                                        }
                                        case 0x1b: // lookupswitch
                                        {
                                            int32_t default_offset = reader.readS24();
                                            ep.push_back(ip + default_offset);

                                            int32_t case_count = reader.readU30();
                                            case_count++;
                                            
                                            std::string cases;
                                            
                                            do {
                                                uint32_t case_offset = reader.readS24();
                                                ep.push_back(ip + case_offset);
                                                if(!cases.empty()) cases += ", ";
                                                cases += stringf("%d", case_offset);
                                            } while (case_count--);

                                            ops[ip] = stringf("lookupswitch (default=%d, cases=[%s])", default_offset, cases.data());
                                            for(uint32_t i = ip; i < reader.pos(); ++i) used[i] = true;
                                            
                                            continue;
                                        }
                                        case 0x1c: // pushwith
                                        {
                                            ops[ip] = "pushwith";
                                            break;
                                        }
                                        case 0x1d: // popscope
                                        {
                                            ops[ip] = "popscope";
                                            break;
                                        }
                                        case 0x1e: // nextname
                                        {
                                            ops[ip] = "nextname";
                                            break;
                                        }
                                        case 0x1f: // hasnext
                                        {
                                            ops[ip] = "hasnext";
                                            break;
                                        }
                                        case 0x20: // pushnull
                                        {
                                            ops[ip] = "pushnull";
                                            break;
                                        }
                                        case 0x21: // pushundefined
                                        {
                                            ops[ip] = "pusundefined";
                                            break;
                                        }
                                        case 0x23: // nextvalue
                                        {
                                            ops[ip] = "nextvalue";
                                            break;
                                        }
                                        case 0x24: // pushbyte
                                        {
                                            uint8_t value = reader.readU8();
                                            ops[ip] = stringf("pushbyte (value=%u)", value);
                                            break;
                                        }
                                        case 0x25: // pushshort
                                        {
                                            uint32_t value = reader.readU30();
                                            ops[ip] = stringf("pushshort (value=%u)", value);
                                            break;
                                        }
                                        case 0x26: // pushtrue
                                        {
                                            ops[ip] = "pushtrue";
                                            break;
                                        }
                                        case 0x27: // pushfalse
                                        {
                                            ops[ip] = "pushfalse";
                                            break;
                                        }
                                        case 0x28: // pushnan
                                        {
                                            ops[ip] = "pushnan";
                                            break;
                                        }
                                        case 0x29: // pop
                                        {
                                            ops[ip] = "pop";
                                            break;
                                        }
                                        case 0x2a: // dup
                                        {
                                            ops[ip] = "dup";
                                            break;
                                        }
                                        case 0x2b: // swap
                                        {
                                            ops[ip] = "swap";
                                            break;
                                        }
                                        case 0x2c: // pushstring
                                        {
                                            uint32_t index = reader.readU30();
                                            
                                            ops[ip] = stringf("pushstring \"%s\" (index=%d)", abc->cpool.strings[index].data(), index);
                                            break;
                                        }
                                        case 0x2d: // pushint
                                        {
                                            uint32_t index = reader.readU30();
                                            ops[ip] = stringf("pushint (index=%u)", index);
                                            break;
                                        }
                                        case 0x2e: // pushuint
                                        {
                                            uint32_t index = reader.readU30();
                                            ops[ip] = stringf("pushuint (index=%u)", index);
                                            break;
                                        }
                                        case 0x2f: // pushdouble
                                        {
                                            uint32_t index = reader.readU30();
                                            ops[ip] = stringf("pushdouble (index=%u)", index);
                                            break;
                                        }
                                        case 0x30: // pushscope
                                        {
                                            ops[ip] = "pushscope";
                                            break;
                                        }
                                        case 0x31: // pushnamespace
                                        {
                                            uint32_t index = reader.readU30();
                                            ops[ip] = stringf("pushnamespace (index=%u)", index);
                                            break;
                                        }
                                        case 0x32: // hasnext2
                                        {
                                            uint32_t object_reg = reader.readU30();
                                            uint32_t index_reg = reader.readU30();
                                            ops[ip] = stringf("hasnext2 (object_reg=%u, index_reg=%u)", object_reg, index_reg);
                                            break;
                                        }
                                        case 0x40: // newfunction
                                        {
                                            uint32_t index = reader.readU30();
                                            ops[ip] = stringf("newfunction (index=%u)", index);
                                            break;
                                        }
                                        case 0x41: // call
                                        {
                                            uint32_t argcount = reader.readU30();
                                            ops[ip] = stringf("call (argcount=%u)", argcount);
                                            break;                                            
                                        }
                                        case 0x42: // construct
                                        {
                                            uint32_t argcount = reader.readU30();
                                            ops[ip] = stringf("construct (argcount=%u)", argcount);
                                            break;                                            
                                        }
                                        case 0x43: // callmethod
                                        {
                                            uint32_t index = reader.readU30();
                                            uint32_t argcount = reader.readU30();
                                            ops[ip] = stringf("callmethod (index=%u, argcount=%u)", index, argcount);
                                            break;
                                        }
                                        case 0x44: // callstatic
                                        {
                                            uint32_t index = reader.readU30();
                                            uint32_t argcount = reader.readU30();
                                            ops[ip] = stringf("callstatic (index=%u, argcount=%u)", index, argcount);
                                            break;
                                        }
                                        case 0x45: // callsuper
                                        {
                                            uint32_t index = reader.readU30();
                                            uint32_t argcount = reader.readU30();
                                            ops[ip] = stringf("callsuper (index=%u, argcount=%u)", index, argcount);
                                            break;
                                        }
                                        case 0x46: // callproperty
                                        {
                                            uint32_t index = reader.readU30();
                                            uint32_t argcount = reader.readU30();
                                            ops[ip] = stringf("callproperty (index=%u, argcount=%u)", index, argcount);
                                            break;
                                        }
                                        case 0x47: // returnvoid
                                        {
                                            ops[ip] = "returnvoid";
                                            used[ip] = true;
                                            continue;
                                        }
                                        case 0x48: // returnvalue
                                        {
                                            ops[ip] = "returnvalue";
                                            used[ip] = true;
                                            continue;
                                        }
                                        case 0x49: // constructsuper
                                        {
                                            uint32_t argcount = reader.readU30();
                                            ops[ip] = stringf("constructsuper (argcount=%u)", argcount);
                                            break;                                            
                                        }
                                        case 0x4a: // constructprop
                                        {
                                            uint32_t index = reader.readU30();
                                            uint32_t argcount = reader.readU30();
                                            ops[ip] = stringf("constructprop (index=%u, argcount=%u)", index, argcount);
                                            break;
                                        }
                                        case 0x4c: // callproplex
                                        {
                                            uint32_t index = reader.readU30();
                                            uint32_t argcount = reader.readU30();
                                            ops[ip] = stringf("callproplex (index=%u, argcount=%u)", index, argcount);
                                            break;
                                        }
                                        case 0x4e: // callsupervoid
                                        {
                                            uint32_t index = reader.readU30();
                                            uint32_t argcount = reader.readU30();
                                            ops[ip] = stringf("callsupervoid (index=%u, argcount=%u)", index, argcount);
                                            break;
                                        }
                                        case 0x4f: // callpropvoid
                                        {
                                            uint32_t index = reader.readU30();
                                            uint32_t argcount = reader.readU30();
                                            ops[ip] = stringf("callpropvoid (index=%u, argcount=%u)", index, argcount);
                                            break;
                                        }
                                        case 0x53: // applytype
                                        {
                                            uint32_t argcount = reader.readU30();
                                            ops[ip] = stringf("applytype (argcount=%u)", argcount);
                                            break;
                                        }
                                        case 0x55: // newobject
                                        {
                                            uint32_t argcount = reader.readU30();
                                            ops[ip] = stringf("newobject (argcount=%u)", argcount);
                                            break;
                                        }
                                        case 0x56: // newarray
                                        {
                                            uint32_t argcount = reader.readU30();
                                            ops[ip] = stringf("newarray (argcount=%u)", argcount);
                                            break;
                                        }
                                        case 0x57: // newactivation
                                        {
                                            ops[ip] = "newactivation";
                                            break;
                                        }
                                        case 0x58: // newclass
                                        {
                                            uint32_t index = reader.readU30();
                                            ops[ip] = stringf("newclass (index=%u)", index);
                                            break;
                                        }
                                        case 0x59: // getdescendants
                                        {
                                            uint32_t index = reader.readU30();
                                            ops[ip] = stringf("getdescendants (index=%u)", index);
                                            break;
                                        }
                                        case 0x5a: // newcatch
                                        {
                                            uint32_t index = reader.readU30();
                                            ops[ip] = stringf("newcatch (index=%u)", index);
                                            break;
                                        }
                                        case 0x5d: // findpropstrict
                                        {
                                            uint32_t index = reader.readU30();
                                            ops[ip] = stringf("findpropstrict (index=%u)", index);
                                            break;                                            
                                        }
                                        case 0x5e: // findproperty
                                        {
                                            uint32_t index = reader.readU30();
                                            ops[ip] = stringf("findproperty (index=%u)", index);
                                            break;                                            
                                        }
                                        case 0x5f: // finddef
                                        {
                                            // not in spec
                                            uint32_t index = reader.readU30();
                                            ops[ip] = stringf("finddef (index=%u)", index);
                                            break;                                            
                                        }
                                        case 0x60: // getlex
                                        {
                                            uint32_t index = reader.readU30();
                                            ops[ip] = stringf("getlex (index=%u)", index);
                                            break;
                                        }
                                        case 0x61: // setproperty
                                        {
                                            uint32_t index = reader.readU30();
                                            ops[ip] = stringf("setproperty (index=%u)", index);
                                            break;
                                        }
                                        case 0x62: // getlocal
                                        {
                                            uint32_t index = reader.readU30();
                                            ops[ip] = stringf("getlocal (index=%u)", index);
                                            break;
                                        }
                                        case 0x63: // setlocal
                                        {
                                            uint32_t index = reader.readU30();
                                            ops[ip] = stringf("setlocal (index=%u)", index);
                                            break;
                                        }
                                        case 0x64: // getglobalscope
                                        {
                                            ops[ip] = "getglobalscope";
                                            break;
                                        }
                                        case 0x65: // getscopeobject
                                        {
                                            uint8_t index = reader.readU8();
                                            ops[ip] = stringf("getscopeobject (index=%u)", index);
                                            break;
                                        }
                                        case 0x66: // getproperty
                                        {
                                            uint32_t index = reader.readU30();
                                            ops[ip] = stringf("getproperty (index=%u)", index);
                                            break;
                                        }
                                        case 0x68: // initproperty
                                        {
                                            uint32_t index = reader.readU30();
                                            ops[ip] = stringf("initproperty (index=%u)", index);
                                            break;
                                        }
                                        case 0x6a: // deleteproperty
                                        {
                                            uint32_t index = reader.readU30();
                                            ops[ip] = stringf("deleteproperty (index=%u)", index);
                                            break;
                                        }
                                        case 0x6c: // getslot
                                        {
                                            uint32_t index = reader.readU30();
                                            ops[ip] = stringf("getslot (index=%u)", index);
                                            break;
                                        }
                                        case 0x6d: // setslot
                                        {
                                            uint32_t index = reader.readU30();
                                            ops[ip] = stringf("setslot (index=%u)", index);
                                            break;
                                        }
                                        case 0x6e: // getglobalslot
                                        {
                                            uint32_t index = reader.readU30();
                                            ops[ip] = stringf("getglobalslot (index=%u)", index);
                                            break;
                                        }
                                        case 0x6f: // setglobalslot
                                        {
                                            uint32_t index = reader.readU30();
                                            ops[ip] = stringf("setglobalslot (index=%u)", index);
                                            break;
                                        }
                                        case 0x70: // convert_s
                                        {
                                            ops[ip] = "convert_s";
                                            break;
                                        }
                                        case 0x71: // esc_xelem
                                        {
                                            ops[ip] = "esc_xelem";
                                            break;
                                        }
                                        case 0x72: // esc_xattr
                                        {
                                            ops[ip] = "esc_xattr";
                                            break;
                                        }
                                        case 0x73: // convert_i
                                        {
                                            ops[ip] = "convert_i";
                                            break;
                                        }
                                        case 0x74: // convert_u
                                        {
                                            ops[ip] = "convert_u";
                                            break;
                                        }
                                        case 0x75: // convert_d
                                        {
                                            ops[ip] = "convert_d";
                                            break;
                                        }
                                        case 0x76: // convert_b
                                        {
                                            ops[ip] = "convert_b";
                                            break;
                                        }
                                        case 0x77: // convert_o
                                        {
                                            ops[ip] = "convert_o";
                                            break;
                                        }
                                        case 0x78: // checkfilter
                                        {
                                            ops[ip] = "checkfilter";
                                            break;
                                        }
                                        case 0x80: // coerce
                                        {
                                            uint32_t index = reader.readU30();
                                            ops[ip] = stringf("coerce (index=%u)", index);
                                            break;
                                        }
                                        case 0x81: // coerce_b
                                        {
                                            // not in spec, deprecated
                                            ops[ip] = "coerce_b";
                                            break;
                                        }
                                        case 0x82: // coerce_a
                                        {
                                            ops[ip] = "coerce_a";
                                            break;
                                        }
                                        case 0x83: // coerce_i
                                        {
                                            // not in spec, deprecated
                                            ops[ip] = "coerce_i";
                                            break;
                                        }
                                        case 0x84: // coerce_d
                                        {
                                            // not in spec, deprecated
                                            ops[ip] = "coerce_d";
                                            break;
                                        }
                                        case 0x85: // coerce_s
                                        {
                                            ops[ip] = "coerce_s";
                                            break;
                                        }
                                        case 0x86: // astype
                                        {
                                            uint32_t index = reader.readU30();
                                            ops[ip] = stringf("astype (index=%u)", index);
                                            break;
                                        }
                                        case 0x87: // astypelate
                                        {
                                            ops[ip] = "astypelate";
                                            break;
                                        }
                                        case 0x88: // coerce_u
                                        {
                                            // not in spec, deprecated
                                            ops[ip] = "coerce_u";
                                            break;
                                        }
                                        case 0x89: // coerce_o
                                        {
                                            // not in spec, deprecated
                                            ops[ip] = "coerce_o";
                                            break;
                                        }
                                        case 0x90: // negate
                                        {
                                            ops[ip] = "negate";
                                            break;
                                        }
                                        case 0x91: // increment
                                        {
                                            ops[ip] = "increment";
                                            break;
                                        }
                                        case 0x92: // inclocal
                                        {
                                            uint32_t index = reader.readU30();
                                            ops[ip] = stringf("inclocal (index=%u)", index);
                                            break;
                                        }
                                        case 0x93: // decrement
                                        {
                                            ops[ip] = "decrement";
                                            break;
                                        }
                                        case 0x94: // declocal
                                        {
                                            uint32_t index = reader.readU30();
                                            ops[ip] = stringf("declocal (index=%u)", index);
                                            break;
                                        }
                                        case 0x95: // typeof
                                        {
                                            ops[ip] = "typeof";
                                            break;
                                        }
                                        case 0x96: // not
                                        {
                                            ops[ip] = "not";
                                            break;
                                        }
                                        case 0x97: // bitnot
                                        {
                                            ops[ip] = "bitnot";
                                            break;
                                        }
                                        case 0xa0: // add
                                        {
                                            ops[ip] = "add";
                                            break;
                                        }
                                        case 0xa1: // subtract
                                        {
                                            ops[ip] = "subtract";
                                            break;
                                        }
                                        case 0xa2: // multiply
                                        {
                                            ops[ip] = "multiply";
                                            break;
                                        }
                                        case 0xa3: // divide
                                        {
                                            ops[ip] = "divide";
                                            break;
                                        }
                                        case 0xa4: // modulo
                                        {
                                            ops[ip] = "modulo";
                                            break;
                                        }
                                        case 0xa5: // lshift
                                        {
                                            ops[ip] = "lshift";
                                            break;
                                        }
                                        case 0xa6: // rshift
                                        {
                                            ops[ip] = "rshift";
                                            break;
                                        }
                                        case 0xa7: // urshift
                                        {
                                            ops[ip] = "urshift";
                                            break;
                                        }
                                        case 0xa8: // bitand
                                        {
                                            ops[ip] = "bitand";
                                            break;
                                        }
                                        case 0xa9: // bitor
                                        {
                                            ops[ip] = "bitor";
                                            break;
                                        }
                                        case 0xaa: // bitxor
                                        {
                                            ops[ip] = "bitxor";
                                            break;
                                        }
                                        case 0xab: // equals
                                        {
                                            ops[ip] = "equals";
                                            break;
                                        }
                                        case 0xac: // strictequals
                                        {
                                            ops[ip] = "strictequals";
                                            break;
                                        }
                                        case 0xad: // lessthan
                                        {
                                            ops[ip] = "lessthan";
                                            break;
                                        }
                                        case 0xae: // lessequals
                                        {
                                            ops[ip] = "lessequals";
                                            break;
                                        }
                                        case 0xaf: // greaterthan
                                        {
                                            ops[ip] = "greaterthan";
                                            break;
                                        }
                                        case 0xb0: // greaterequals
                                        {
                                            ops[ip] = "greaterequals";
                                            break;
                                        }
                                        case 0xb1: // instanceof
                                        {
                                            ops[ip] = "instanceof";
                                            break;
                                        }
                                        case 0xb2: // istype
                                        {
                                            uint32_t index = reader.readU30();
                                            ops[ip] = stringf("istype (index=%u)", index);
                                            break;
                                        }
                                        case 0xb3: // istypelate
                                        {
                                            ops[ip] = "istypelate";
                                            break;
                                        }
                                        case 0xb4: // in
                                        {
                                            ops[ip] = "in";
                                            break;
                                        }
                                        case 0xc0: // increment_i
                                        {
                                            ops[ip] = "increment_i";
                                            break;
                                        }
                                        case 0xc1: // decrement_i
                                        {
                                            ops[ip] = "decrement_i";
                                            break;
                                        }
                                        case 0xc2: // inclocal_i
                                        {
                                            uint32_t index = reader.readU30();
                                            ops[ip] = stringf("inclocal_i (index=%u)", index);
                                            break;
                                        }
                                        case 0xc3: // declocal_i
                                        {
                                            uint32_t index = reader.readU30();
                                            ops[ip] = stringf("declocal_i (index=%u)", index);
                                            break;
                                        }
                                        case 0xc4: // negate_i
                                        {
                                            ops[ip] = "negate_i";
                                            break;
                                        }
                                        case 0xc5: // add_i
                                        {
                                            ops[ip] = "add_i";
                                            break;
                                        }
                                        case 0xc6: // subtract_i
                                        {
                                            ops[ip] = "subtract_i";
                                            break;
                                        }
                                        case 0xc7: // multiply_i
                                        {
                                            ops[ip] = "multiply_i";
                                            break;
                                        }
                                        case 0xd0: // getlocal_0 
                                        {
                                            ops[ip] = "getlocal_0";
                                            break;
                                        }
                                        case 0xd1: // getlocal_1                                        
                                        {
                                            ops[ip] = "getlocal_1";                                            
                                            break;
                                        }
                                        case 0xd2: // getlocal_2                                        
                                        {
                                            ops[ip] = "getlocal_2";
                                            break;
                                        }
                                        case 0xd3: // getlocal_3                                        
                                        {
                                            ops[ip] = "getlocal_3";
                                            break;
                                        }
                                        case 0xd4: // setlocal_0
                                        {
                                            ops[ip] = "setlocal_0";
                                            break;
                                        }
                                        case 0xd5: // setlocal_1
                                        {
                                            ops[ip] = "setlocal_1";
                                            break;
                                        }
                                        case 0xd6: // setlocal_2
                                        {
                                            ops[ip] = "setlocal_2";
                                            break;
                                        }
                                        case 0xd7: // setlocal_3
                                        {
                                            ops[ip] = "setlocal_3";
                                            break;
                                        }
                                        case 0xef: // debug
                                        {
                                            uint8_t debug_type = reader.readU8();
                                            uint32_t index = reader.readU30();
                                            uint8_t reg = reader.readU8();
                                            uint32_t extra = reader.readU30();
                                            ops[ip] = stringf("debug \"%s\" (type=%u, index=%u, reg=%u, extra=%u)", abc->cpool.strings[index].data(), debug_type, index, reg, extra);
                                            break;                                            
                                        }
                                        case 0xf0: // debugline
                                        {
                                            uint32_t linenum = reader.readU30();
                                            ops[ip] = stringf("debugline (linenum=%u)", linenum);
                                            break;
                                        }
                                        case 0xf1: // debugfile
                                        {
                                            uint32_t index = reader.readU30();
                                            ops[ip] = stringf("debugfile \"%s\" (index=%u)", abc->cpool.strings[index].data(), index);
                                            break;
                                        }
                                        case 0xf2: // bkptline
                                        {
                                            // not in spec
                                            uint32_t linenum = reader.readU30();
                                            ops[ip] = stringf("bkptline (linenum=%u)", linenum);
                                            break;
                                        }
                                        case 0xf3: // timestamp
                                        {
                                            // not in spec
                                            ops[ip] = "timestamp";
                                            break;
                                        }
                                        default:
                                            DEBUG("UNKNOWN OP: %#x", op);
                                            exit(-1);
                                    }
                                    // normal flow - next operation
                                    ep.push_back(reader.pos());
                                    for(uint32_t i = ip; i < reader.pos(); ++i) used[i] = true;
                                }
                                DEBUG("LISTING (%u bytes):", (*it).code.size());
                                for(std::map<uint32_t, std::string>::iterator li = ops.begin(); li != ops.end(); ++li) {
                                    DEBUG("%4u\t%s", (*li).first, (*li).second.data());                                    
                                }
                            }
                            catch(const char * e) {
                                DEBUG("EXCEPTION: %s", e);
                            }
                            catch(...) {
                                DEBUG("UNKNOWN EXCEPTION");
                            }
                            DEBUG("USED:");
                            uint32_t i = 0;
                            while(i < used.size()) {
                                printf("%c", used[i] ? '#' : '%');
                                ++i;
                                if(!(i % 32)) printf("\n");
                            }
                            printf("\n");
                            
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

