/* 
 * File:   ABCVM.h
 * Author: brigada
 *
 * Created on 31 Май 2013 г., 23:44
 */

#ifndef ABCVM_H
#define	ABCVM_H

#include "ABCTypes.h"
#include <list>
#include <map>

class ABCVM {
public:
    ABCVM(ABCConstantPool& cpool);
    virtual ~ABCVM();
    bool disassemble(std::string& code, uint32_t start, std::map<uint32_t, std::string>& output, std::vector<bool>& used, std::list<uint32_t>& jumps);
private:
    ABCConstantPool& _cpool;

};

#endif	/* ABCVM_H */

