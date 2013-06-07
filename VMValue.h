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
    uint32_t line;
    VMValueType type;
    
    union {
        int32_t ival;
        const char *  sval;
        double  fval;        
    };
    
    VMValue(uint32_t _line, VMValueType _type);
    VMValue(uint32_t _line, VMValueType _type, const char* v);
    VMValue(uint32_t _line, VMValueType _type, int32_t v);
    VMValue(uint32_t _line, VMValueType _type, double v);
    
    virtual ~VMValue();
    
protected:
    
};

#endif	/* VMVALUE_H */

