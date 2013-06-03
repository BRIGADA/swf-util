/* 
 * File:   ABCVM.cpp
 * Author: brigada
 * 
 * Created on 31 Май 2013 г., 23:44
 */

#include "ABCVM.h"
#include "utils.h"
#include "ABCReader.h"

ABCVM::ABCVM(ABCConstantPool& cpool) : _cpool(cpool) {
}

bool ABCVM::disassemble(std::string& code, uint32_t start, std::map<uint32_t, std::string>& output, std::vector<bool>& used, std::list<uint32_t>& jumps) {
    ABCReader reader(code);
    
    uint32_t ip = start;
    ABCUI32List ep; // entryPoints
    ep.push_back(ip);

    try {

        while (!ep.empty()) {
            ip = ep.back();
            ep.pop_back();

            if (output.find(ip) != output.end()) continue;
            reader.pos(ip);
            uint8_t op = reader.readU8();
            switch (op) {
                case 0x01: // bkpt
                {
                    output[ip] = "bkpt";
                    break;
                }
                case 0x02: // nop
                {
                    output[ip] = "nop";
                    break;
                }
                case 0x03: // throw
                {
                    output[ip] = "throw";
                    used[ip] = true;
                    continue;
                }
                case 0x04: // getsuper
                {
                    uint32_t index = reader.readU30();
                    output[ip] = stringf("getsuper       %s", _cpool.getName(index).data());
                    break;
                }
                case 0x05: // setsuper
                {
                    uint32_t index = reader.readU30();
                    output[ip] = stringf("setsuper       %s", _cpool.getName(index).data());
                    break;
                }
                case 0x06: // dxns
                {
                    uint32_t index = reader.readU30();
                    output[ip] = stringf("dxns           %s", _cpool.getSTR(index, true).data());
                    break;
                }
                case 0x07: // dxnslate
                {
                    output[ip] = "dxnslate";
                    break;
                }
                case 0x08: // kill
                {
                    uint32_t index = reader.readU30();
                    output[ip] = stringf("kill           %u", index);
                    break;
                }
                case 0x09: // label
                {
                    output[ip] = "label";
                    break;
                }
                case 0x0c: // ifnlt
                {
                    int32_t offset = reader.readS24();
                    ep.push_back(reader.pos() + offset);
                    jumps.push_back(reader.pos() + offset);
                    output[ip] = stringf("ifnlt          L%u", reader.pos() + offset);
                    break;
                }
                case 0x0d: // ifnle
                {
                    int32_t offset = reader.readS24();
                    ep.push_back(reader.pos() + offset);
                    jumps.push_back(reader.pos() + offset);
                    output[ip] = stringf("ifnle          L%u", reader.pos() + offset);
                    break;
                }
                case 0x0e: // ifngt
                {
                    int32_t offset = reader.readS24();
                    ep.push_back(reader.pos() + offset);
                    jumps.push_back(reader.pos() + offset);
                    output[ip] = stringf("ifngt          L%u", reader.pos() + offset);
                    break;
                }
                case 0x0f: // ifnge
                {
                    int32_t offset = reader.readS24();
                    ep.push_back(reader.pos() + offset);
                    jumps.push_back(reader.pos() + offset);
                    output[ip] = stringf("ifnge          L%u", reader.pos() + offset);
                    break;
                }
                case 0x10: // jump
                {
                    int32_t offset = reader.readS24();
                    ep.push_back(reader.pos() + offset);
                    jumps.push_back(reader.pos() + offset);
                    output[ip] = stringf("jump           L%u", reader.pos() + offset);
//                    break;
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
                    jumps.push_back(reader.pos() + offset);
                    output[ip] = stringf("iftrue         L%u", reader.pos() + offset);
                    break;
                }
                case 0x12: // iffalse
                {
                    int32_t offset = reader.readS24();
                    ep.push_back(reader.pos() + offset);
                    jumps.push_back(reader.pos() + offset);
                    output[ip] = stringf("iffalse        L%u", reader.pos() + offset);
                    break;
                }
                case 0x13: // ifeq
                {
                    int32_t offset = reader.readS24();
                    ep.push_back(reader.pos() + offset);
                    jumps.push_back(reader.pos() + offset);
                    output[ip] = stringf("ifeq           L%u", reader.pos() + offset);
                    break;
                }
                case 0x14: // ifne
                {
                    int32_t offset = reader.readS24();
                    ep.push_back(reader.pos() + offset);
                    jumps.push_back(reader.pos() + offset);
                    output[ip] = stringf("ifne           L%u", reader.pos() + offset);
                    break;
                }
                case 0x15: // iflt
                {
                    int32_t offset = reader.readS24();
                    ep.push_back(reader.pos() + offset);
                    jumps.push_back(reader.pos() + offset);
                    output[ip] = stringf("iflt           L%u", reader.pos() + offset);
                    break;
                }
                case 0x16: // ifle
                {
                    int32_t offset = reader.readS24();
                    ep.push_back(reader.pos() + offset);
                    jumps.push_back(reader.pos() + offset);
                    output[ip] = stringf("ifle           L%u", reader.pos() + offset);
                    break;
                }
                case 0x17: // ifgt
                {
                    int32_t offset = reader.readS24();
                    ep.push_back(reader.pos() + offset);
                    jumps.push_back(reader.pos() + offset);
                    output[ip] = stringf("ifgt           L%u", reader.pos() + offset);
                    break;
                }
                case 0x18: // ifge
                {
                    int32_t offset = reader.readS24();
                    ep.push_back(reader.pos() + offset);
                    jumps.push_back(reader.pos() + offset);
                    output[ip] = stringf("ifge           L%u", reader.pos() + offset);
                    break;
                }
                case 0x19: // ifstricteq
                {
                    int32_t offset = reader.readS24();
                    ep.push_back(reader.pos() + offset);
                    jumps.push_back(reader.pos() + offset);
                    output[ip] = stringf("ifstricteq     L%u", reader.pos() + offset);
                    break;
                }
                case 0x1a: // ifstrictne
                {
                    int32_t offset = reader.readS24();
                    ep.push_back(reader.pos() + offset);
                    jumps.push_back(reader.pos() + offset);
                    output[ip] = stringf("ifstrictne     L%u", reader.pos() + offset);
                    break;
                }
                case 0x1b: // lookupswitch
                {
                    int32_t default_offset = reader.readS24();
                    ep.push_back(ip + default_offset);
                    jumps.push_back(ip + default_offset);
                    int32_t case_count = reader.readU30();

                    std::string cases;

                    do {
                        uint32_t case_offset = reader.readS24();
                        ep.push_back(ip + case_offset);
                        jumps.push_back(ip + case_offset);
                        if (!cases.empty()) cases += ", ";
                        cases += stringf("L%d", ip + case_offset);
                    } while (case_count--);

                    output[ip] = stringf("lookupswitch   L%u, [%s]", ip + default_offset, cases.data());

                    for (uint32_t i = ip; i < reader.pos(); ++i) used[i] = true;
                    continue;
                }
                case 0x1c: // pushwith
                {
                    output[ip] = "pushwith";
                    break;
                }
                case 0x1d: // popscope
                {
                    output[ip] = "popscope";
                    break;
                }
                case 0x1e: // nextname
                {
                    output[ip] = "nextname";
                    break;
                }
                case 0x1f: // hasnext
                {
                    output[ip] = "hasnext";
                    break;
                }
                case 0x20: // pushnull
                {
                    output[ip] = "pushnull";
                    break;
                }
                case 0x21: // pushundefined
                {
                    output[ip] = "pusundefined";
                    break;
                }
                case 0x23: // nextvalue
                {
                    output[ip] = "nextvalue";
                    break;
                }
                case 0x24: // pushbyte
                {
                    uint8_t value = reader.readU8();
                    output[ip] = stringf("pushbyte       %u", value);
                    break;
                }
                case 0x25: // pushshort
                {
                    uint32_t value = reader.readU30();
                    output[ip] = stringf("pushshort      %u", value);
                    break;
                }
                case 0x26: // pushtrue
                {
                    output[ip] = "pushtrue";
                    break;
                }
                case 0x27: // pushfalse
                {
                    output[ip] = "pushfalse";
                    break;
                }
                case 0x28: // pushnan
                {
                    output[ip] = "pushnan";
                    break;
                }
                case 0x29: // pop
                {
                    output[ip] = "pop";
                    break;
                }
                case 0x2a: // dup
                {
                    output[ip] = "dup";
                    break;
                }
                case 0x2b: // swap
                {
                    output[ip] = "swap";
                    break;
                }
                case 0x2c: // pushstring
                {
                    uint32_t index = reader.readU30();

                    output[ip] = stringf("pushstring     %s", _cpool.getSTR(index, true).data());
                    break;
                }
                case 0x2d: // pushint
                {
                    uint32_t index = reader.readU30();
                    output[ip] = stringf("pushint        %d", _cpool.ints[index]);
                    break;
                }
                case 0x2e: // pushuint
                {
                    uint32_t index = reader.readU30();
                    output[ip] = stringf("pushuint       %u", _cpool.ints[index]);
                    break;
                }
                case 0x2f: // pushdouble
                {
                    uint32_t index = reader.readU30();
                    output[ip] = stringf("pushdouble     %f", _cpool.doubles[index]);
                    break;
                }
                case 0x30: // pushscope
                {
                    output[ip] = "pushscope";
                    break;
                }
                case 0x31: // pushnamespace
                {
                    uint32_t index = reader.readU30();
                    output[ip] = stringf("pushnamespace  %s", _cpool.getNS(index).data());
                    break;
                }
                case 0x32: // hasnext2
                {
                    uint32_t object_reg = reader.readU30();
                    uint32_t index_reg = reader.readU30();
                    output[ip] = stringf("hasnext2       (object_reg=%u, index_reg=%u)", object_reg, index_reg);
                    break;
                }
                case 0x40: // newfunction
                {
                    uint32_t index = reader.readU30();
                    output[ip] = stringf("newfunction    (index=%u)", index);
                    break;
                }
                case 0x41: // call
                {
                    uint32_t argcount = reader.readU30();
                    output[ip] = stringf("call           (argcount=%u)", argcount);
                    break;
                }
                case 0x42: // construct
                {
                    uint32_t argcount = reader.readU30();
                    output[ip] = stringf("construct      (argcount=%u)", argcount);
                    break;
                }
                case 0x43: // callmethod
                {
                    uint32_t index = reader.readU30();
                    uint32_t argcount = reader.readU30();
                    output[ip] = stringf("callmethod     (index=%u, argcount=%u)", index, argcount);
                    break;
                }
                case 0x44: // callstatic
                {
                    uint32_t index = reader.readU30();
                    uint32_t argcount = reader.readU30();
                    output[ip] = stringf("callstatic     (index=%u, argcount=%u)", index, argcount);
                    break;
                }
                case 0x45: // callsuper
                {
                    uint32_t index = reader.readU30();
                    uint32_t argcount = reader.readU30();
                    output[ip] = stringf("callsuper      (index=%u, argcount=%u)", index, argcount);
                    break;
                }
                case 0x46: // callproperty
                {
                    uint32_t index = reader.readU30();
                    uint32_t argcount = reader.readU30();
                    output[ip] = stringf("callproperty   %s, %u", _cpool.getName(index).data(), argcount);
                    break;
                }
                case 0x47: // returnvoid
                {
                    output[ip] = "returnvoid";
                    used[ip] = true;
                    continue;
                }
                case 0x48: // returnvalue
                {
                    output[ip] = "returnvalue";
                    used[ip] = true;
                    continue;
                }
                case 0x49: // constructsuper
                {
                    uint32_t argcount = reader.readU30();
                    output[ip] = stringf("constructsuper (argcount=%u)", argcount);
                    break;
                }
                case 0x4a: // constructprop
                {
                    uint32_t index = reader.readU30();
                    uint32_t argcount = reader.readU30();
                    output[ip] = stringf("constructprop  %s, %u", _cpool.getName(index).data(), argcount);
                    break;
                }
                case 0x4c: // callproplex
                {
                    uint32_t index = reader.readU30();
                    uint32_t argcount = reader.readU30();
                    output[ip] = stringf("callproplex    %s, %u", _cpool.getName(index).data(), argcount);
                    break;
                }
                case 0x4e: // callsupervoid
                {
                    uint32_t index = reader.readU30();
                    uint32_t argcount = reader.readU30();
                    output[ip] = stringf("callsupervoid  %s, %u", _cpool.getName(index).data(), argcount);
                    break;
                }
                case 0x4f: // callpropvoid
                {
                    uint32_t index = reader.readU30();
                    uint32_t argcount = reader.readU30();
                    output[ip] = stringf("callpropvoid   %s, %u", _cpool.getName(index).data(), argcount);
                    break;
                }
                case 0x53: // applytype
                {
                    uint32_t argcount = reader.readU30();
                    output[ip] = stringf("applytype      (argcount=%u)", argcount);
                    break;
                }
                case 0x55: // newobject
                {
                    uint32_t argcount = reader.readU30();
                    output[ip] = stringf("newobject      (argcount=%u)", argcount);
                    break;
                }
                case 0x56: // newarray
                {
                    uint32_t argcount = reader.readU30();
                    output[ip] = stringf("newarray       (argcount=%u)", argcount);
                    break;
                }
                case 0x57: // newactivation
                {
                    output[ip] = "newactivation";
                    break;
                }
                case 0x58: // newclass
                {
                    uint32_t index = reader.readU30();
                    output[ip] = stringf("newclass       (index=%u)", index);
                    break;
                }
                case 0x59: // getdescendants
                {
                    uint32_t index = reader.readU30();
                    output[ip] = stringf("getdescendants (index=%u)", index);
                    break;
                }
                case 0x5a: // newcatch
                {
                    uint32_t index = reader.readU30();
                    output[ip] = stringf("newcatch       (index=%u)", index);
                    break;
                }
                case 0x5d: // findpropstrict
                {
                    uint32_t index = reader.readU30();
                    output[ip] = stringf("findpropstrict %s", _cpool.getName(index).data());
                    break;
                }
                case 0x5e: // findproperty
                {
                    uint32_t index = reader.readU30();
                    output[ip] = stringf("findproperty   %s", _cpool.getName(index).data());
                    break;
                }
                case 0x5f: // finddef
                {
                    // not in spec
                    uint32_t index = reader.readU30();
                    output[ip] = stringf("finddef        (index=%u)", index);
                    break;
                }
                case 0x60: // getlex
                {
                    uint32_t index = reader.readU30();
                    output[ip] = stringf("getlex         %s", _cpool.getName(index).data());
                    break;
                }
                case 0x61: // setproperty
                {
                    uint32_t index = reader.readU30();
                    output[ip] = stringf("setproperty    %s", _cpool.getName(index).data());
                    break;
                }
                case 0x62: // getlocal
                {
                    uint32_t index = reader.readU30();
                    output[ip] = stringf("getlocal       (index=%u)", index);
                    break;
                }
                case 0x63: // setlocal
                {
                    uint32_t index = reader.readU30();
                    output[ip] = stringf("setlocal       (index=%u)", index);
                    break;
                }
                case 0x64: // getglobalscope
                {
                    output[ip] = "getglobalscope";
                    break;
                }
                case 0x65: // getscopeobject
                {
                    uint8_t index = reader.readU8();
                    output[ip] = stringf("getscopeobject (index=%u)", index);
                    break;
                }
                case 0x66: // getproperty
                {
                    uint32_t index = reader.readU30();
                    output[ip] = stringf("getproperty    %s", _cpool.getName(index).data());
                    break;
                }
                case 0x68: // initproperty
                {
                    uint32_t index = reader.readU30();
                    output[ip] = stringf("initproperty   %s", _cpool.getName(index).data());
                    break;
                }
                case 0x6a: // deleteproperty
                {
                    uint32_t index = reader.readU30();
                    output[ip] = stringf("deleteproperty %s", _cpool.getName(index).data());
                    break;
                }
                case 0x6c: // getslot
                {
                    uint32_t index = reader.readU30();
                    output[ip] = stringf("getslot        (index=%u)", index);
                    break;
                }
                case 0x6d: // setslot
                {
                    uint32_t index = reader.readU30();
                    output[ip] = stringf("setslot        (index=%u)", index);
                    break;
                }
                case 0x6e: // getglobalslot
                {
                    uint32_t index = reader.readU30();
                    output[ip] = stringf("getglobalslot  (index=%u)", index);
                    break;
                }
                case 0x6f: // setglobalslot
                {
                    uint32_t index = reader.readU30();
                    output[ip] = stringf("setglobalslot  (index=%u)", index);
                    break;
                }
                case 0x70: // convert_s
                {
                    output[ip] = "convert_s";
                    break;
                }
                case 0x71: // esc_xelem
                {
                    output[ip] = "esc_xelem";
                    break;
                }
                case 0x72: // esc_xattr
                {
                    output[ip] = "esc_xattr";
                    break;
                }
                case 0x73: // convert_i
                {
                    output[ip] = "convert_i";
                    break;
                }
                case 0x74: // convert_u
                {
                    output[ip] = "convert_u";
                    break;
                }
                case 0x75: // convert_d
                {
                    output[ip] = "convert_d";
                    break;
                }
                case 0x76: // convert_b
                {
                    output[ip] = "convert_b";
                    break;
                }
                case 0x77: // convert_o
                {
                    output[ip] = "convert_o";
                    break;
                }
                case 0x78: // checkfilter
                {
                    output[ip] = "checkfilter";
                    break;
                }
                case 0x80: // coerce
                {
                    uint32_t index = reader.readU30();
                    output[ip] = stringf("coerce         (index=%u)", index);
                    break;
                }
                case 0x81: // coerce_b
                {
                    // not in spec, deprecated
                    output[ip] = "coerce_b";
                    break;
                }
                case 0x82: // coerce_a
                {
                    output[ip] = "coerce_a";
                    break;
                }
                case 0x83: // coerce_i
                {
                    // not in spec, deprecated
                    output[ip] = "coerce_i";
                    break;
                }
                case 0x84: // coerce_d
                {
                    // not in spec, deprecated
                    output[ip] = "coerce_d";
                    break;
                }
                case 0x85: // coerce_s
                {
                    output[ip] = "coerce_s";
                    break;
                }
                case 0x86: // astype
                {
                    uint32_t index = reader.readU30();
                    output[ip] = stringf("astype         (index=%u)", index);
                    break;
                }
                case 0x87: // astypelate
                {
                    output[ip] = "astypelate";
                    break;
                }
                case 0x88: // coerce_u
                {
                    // not in spec, deprecated
                    output[ip] = "coerce_u";
                    break;
                }
                case 0x89: // coerce_o
                {
                    // not in spec, deprecated
                    output[ip] = "coerce_o";
                    break;
                }
                case 0x90: // negate
                {
                    output[ip] = "negate";
                    break;
                }
                case 0x91: // increment
                {
                    output[ip] = "increment";
                    break;
                }
                case 0x92: // inclocal
                {
                    uint32_t index = reader.readU30();
                    output[ip] = stringf("inclocal       (index=%u)", index);
                    break;
                }
                case 0x93: // decrement
                {
                    output[ip] = "decrement";
                    break;
                }
                case 0x94: // declocal
                {
                    uint32_t index = reader.readU30();
                    output[ip] = stringf("declocal       (index=%u)", index);
                    break;
                }
                case 0x95: // typeof
                {
                    output[ip] = "typeof";
                    break;
                }
                case 0x96: // not
                {
                    output[ip] = "not";
                    break;
                }
                case 0x97: // bitnot
                {
                    output[ip] = "bitnot";
                    break;
                }
                case 0xa0: // add
                {
                    output[ip] = "add";
                    break;
                }
                case 0xa1: // subtract
                {
                    output[ip] = "subtract";
                    break;
                }
                case 0xa2: // multiply
                {
                    output[ip] = "multiply";
                    break;
                }
                case 0xa3: // divide
                {
                    output[ip] = "divide";
                    break;
                }
                case 0xa4: // modulo
                {
                    output[ip] = "modulo";
                    break;
                }
                case 0xa5: // lshift
                {
                    output[ip] = "lshift";
                    break;
                }
                case 0xa6: // rshift
                {
                    output[ip] = "rshift";
                    break;
                }
                case 0xa7: // urshift
                {
                    output[ip] = "urshift";
                    break;
                }
                case 0xa8: // bitand
                {
                    output[ip] = "bitand";
                    break;
                }
                case 0xa9: // bitor
                {
                    output[ip] = "bitor";
                    break;
                }
                case 0xaa: // bitxor
                {
                    output[ip] = "bitxor";
                    break;
                }
                case 0xab: // equals
                {
                    output[ip] = "equals";
                    break;
                }
                case 0xac: // strictequals
                {
                    output[ip] = "strictequals";
                    break;
                }
                case 0xad: // lessthan
                {
                    output[ip] = "lessthan";
                    break;
                }
                case 0xae: // lessequals
                {
                    output[ip] = "lessequals";
                    break;
                }
                case 0xaf: // greaterthan
                {
                    output[ip] = "greaterthan";
                    break;
                }
                case 0xb0: // greaterequals
                {
                    output[ip] = "greaterequals";
                    break;
                }
                case 0xb1: // instanceof
                {
                    output[ip] = "instanceof";
                    break;
                }
                case 0xb2: // istype
                {
                    uint32_t index = reader.readU30();
                    output[ip] = stringf("istype         (index=%u)", index);
                    break;
                }
                case 0xb3: // istypelate
                {
                    output[ip] = "istypelate";
                    break;
                }
                case 0xb4: // in
                {
                    output[ip] = "in";
                    break;
                }
                case 0xc0: // increment_i
                {
                    output[ip] = "increment_i";
                    break;
                }
                case 0xc1: // decrement_i
                {
                    output[ip] = "decrement_i";
                    break;
                }
                case 0xc2: // inclocal_i
                {
                    uint32_t index = reader.readU30();
                    output[ip] = stringf("inclocal_i     (index=%u)", index);
                    break;
                }
                case 0xc3: // declocal_i
                {
                    uint32_t index = reader.readU30();
                    output[ip] = stringf("declocal_i     (index=%u)", index);
                    break;
                }
                case 0xc4: // negate_i
                {
                    output[ip] = "negate_i";
                    break;
                }
                case 0xc5: // add_i
                {
                    output[ip] = "add_i";
                    break;
                }
                case 0xc6: // subtract_i
                {
                    output[ip] = "subtract_i";
                    break;
                }
                case 0xc7: // multiply_i
                {
                    output[ip] = "multiply_i";
                    break;
                }
                case 0xd0: // getlocal_0 
                {
                    output[ip] = "getlocal_0";
                    break;
                }
                case 0xd1: // getlocal_1                                        
                {
                    output[ip] = "getlocal_1";
                    break;
                }
                case 0xd2: // getlocal_2                                        
                {
                    output[ip] = "getlocal_2";
                    break;
                }
                case 0xd3: // getlocal_3                                        
                {
                    output[ip] = "getlocal_3";
                    break;
                }
                case 0xd4: // setlocal_0
                {
                    output[ip] = "setlocal_0";
                    break;
                }
                case 0xd5: // setlocal_1
                {
                    output[ip] = "setlocal_1";
                    break;
                }
                case 0xd6: // setlocal_2
                {
                    output[ip] = "setlocal_2";
                    break;
                }
                case 0xd7: // setlocal_3
                {
                    output[ip] = "setlocal_3";
                    break;
                }
                case 0xef: // debug
                {
                    uint8_t debug_type = reader.readU8();
                    uint32_t index = reader.readU30();
                    uint8_t reg = reader.readU8();
                    uint32_t extra = reader.readU30();
                    output[ip] = stringf("debug          \"%s\" (type=%u, index=%u, reg=%u, extra=%u)", _cpool.strings[index].data(), debug_type, index, reg, extra);
                    break;
                }
                case 0xf0: // debugline
                {
                    uint32_t linenum = reader.readU30();
                    output[ip] = stringf("debugline      %u", linenum);
                    break;
                }
                case 0xf1: // debugfile
                {
                    uint32_t index = reader.readU30();
                    output[ip] = stringf("debugfile      \"%s\"", _cpool.strings[index].data(), index);
                    break;
                }
                case 0xf2: // bkptline
                {
                    // not in spec
                    uint32_t linenum = reader.readU30();
                    output[ip] = stringf("bkptline       (linenum=%u)", linenum);
                    break;
                }
                case 0xf3: // timestamp
                {
                    // not in spec
                    output[ip] = "timestamp";
                    break;
                }
                default:
                    DEBUG("UNKNOWN OP: %#x", op);
                    return false;
            }
            // normal flow - next operation
            ep.push_back(reader.pos());
            for (uint32_t i = ip; i < reader.pos(); ++i) used[i] = true;
        }
        jumps.unique();
    } catch (const char * e) {
        output[ip] = stringf("EXCEPTION: %s", e);
        return false;
    } catch (...) {
        output[ip] = stringf("UNKNOWN EXCEPTION");
        return false;
    }
    return true;
}

ABCVM::~ABCVM() {
}

