/* 
 * File:   VMValue.h
 * Author: brigada
 *
 * Created on 5 Июнь 2013 г., 14:44
 */

#ifndef VMVALUE_H
#define	VMVALUE_H

class VMValue {
public:
    enum VMValueType {
        vtNumber,
        vtString,
        vtFloat,
        vtBoolean,
        vtObject,
        vtExpression,
        vtComplex,
    };
    VMValue();
    VMValue(const VMValue& orig);
    virtual ~VMValue();
private:

};

#endif	/* VMVALUE_H */

