/*
 * ABCOP.cpp
 *
 *  Created on: 26.05.2013
 *      Author: brigada
 */

#include "ABCOP.h"

ABCOP * ABCOP::create(ABCReader& reader) {
    uint8_t op = reader.readU8();
    switch(op) {
        case 0xa0:
            return new ABCOP_add();
        default:
            throw "UNKNOWN OPCODE";
    }
}

ABCOP_add::ABCOP_add() : ABCOP(0xa0) {
}

std::string ABCOP_add::render() {
    return "add";
}