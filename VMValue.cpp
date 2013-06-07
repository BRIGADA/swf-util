/* 
 * File:   VMValue.h
 * Author: brigada
 *
 * Created on 5 Июнь 2013 г., 14:44
 */

#include "VMValue.h"

VMValue::VMValue(uint32_t _line, VMValueType _type) : line(_line), type(_type) {
    
}

VMValue::VMValue(uint32_t _line, VMValueType _type, const char* v) : line(_line), type(_type), sval(v) {

}

VMValue::VMValue(uint32_t _line, VMValueType _type, int32_t v) : line(_line), type(_type), ival(v) {

}

VMValue::VMValue(uint32_t _line, VMValueType _type, double v) : line(_line), type(_type), fval(v) {

}

VMValue::~VMValue() {
    
}