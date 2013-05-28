/*
 * ABCOP.h
 *
 *  Created on: 26.05.2013
 *      Author: brigada
 */

#ifndef ABCOP_H_
#define ABCOP_H_

#include <inttypes.h>
#include <string>
#include "ABCReader.h"

class ABCOP {
public:

    ABCOP(uint8_t opcode) : _op(opcode) {
    }

    virtual ~ABCOP() {
    }

    virtual std::string render() = 0;

    uint8_t op() {
        return _op;
    }
    
    static ABCOP* create(ABCReader& reader);
    
protected:
    uint8_t _op;
};

class ABCOP_add : public ABCOP {
public:

    ABCOP_add();
    virtual std::string render();

};

#endif /* ABCOP_H_ */
