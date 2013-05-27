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
	ABCOP(uint8_t op);
	virtual ~ABCOP();
	virtual std::string render() = 0;
	uint8_t op();
	static ABCOP* create(ABCReader& reader);
protected:
	uint8_t _op;


};

class ABCOP_add : public ABCOP {
public:
	ABCOP_add();

};

#endif /* ABCOP_H_ */
