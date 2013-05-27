/*
 * ABCOP.cpp
 *
 *  Created on: 26.05.2013
 *      Author: brigada
 */

#include "ABCOP.h"

ABCOP::ABCOP(uint8_t op) : _op(op) {
}

ABCOP::~ABCOP() {
}

uint8_t ABCOP::op() {
    return _op;
}
