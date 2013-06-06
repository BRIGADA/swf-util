/* 
 * File:   VMValue.h
 * Author: brigada
 *
 * Created on 5 Июнь 2013 г., 14:44
 */

#ifndef VMVALUE_H
#define	VMVALUE_H

#include <inttypes.h>
#include <string>

#include "utils.h"
enum VMValueType {
    vtUndefined,
    vtNULL,
    vtNAN,
    vtNumber,
    vtString,
    vtFloat,
    vtBoolean,
    vtObject,
    vtExpression,
    vtComplex,
};

class VMValue {
public:
    const VMValueType type;
    const uint32_t line;
    VMValue(VMValueType _type, uint32_t _line) : type(_type), line(_line) {        
    }
    virtual ~VMValue() {        
    }
};

class VMValueUndefined : public VMValue {
public:
    VMValueUndefined(uint32_t _line) : VMValue(vtUndefined, _line) {        
    }
};

class VMValueNULL : public VMValue {
public:
    VMValueNULL(uint32_t _line) : VMValue(vtNULL, _line) {
    }
};

class VMValueNAN : public VMValue {
public:
    VMValueNAN(uint32_t _line) : VMValue(vtNAN, _line) {
    }
};

template <VMValueType _type, typename T> class VMValueSimple : public VMValue {
public:
    const T value;
    VMValueSimple(uint32_t _line, int32_t _value) : VMValue(_type, _line), value(_value) {
        DEBUG("%s", __FUNCTION__);
    }
};

typedef VMValueSimple<vtNumber, int32_t> VMValueNumber;
typedef VMValueSimple<vtString, std::string> VMValueString;
typedef VMValueSimple<vtFloat, double> VMValueFloat;
typedef VMValueSimple<vtBoolean, bool> VMValueBoolean;
typedef VMValueSimple<vtObject, std::string> VMValueObject;
typedef VMValueSimple<vtExpression, std::string> VMValueExpression;
typedef VMValueSimple<vtComplex, std::string> VMValueComplex;

#endif	/* VMVALUE_H */

