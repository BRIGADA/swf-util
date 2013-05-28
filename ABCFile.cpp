/*
 * ABCFile.cpp
 *
 *  Created on: 22.05.2013
 *      Author: brigada
 */

#include "ABCFile.h"
#include "ABCReader.h"
#include <stdio.h>

ABCFile::ABCFile() : versionMajor(0), versionMinor(0) {
    cpool.ints.resize(1);
    cpool.uints.resize(1);
    cpool.strings.resize(1);
    cpool.doubles.resize(1);
    cpool.namespaces.resize(1);
    cpool.namespaceSets.resize(1);
    cpool.multinames.resize(1);
}

ABCFile::~ABCFile() {
}

std::string ABCFile::getMultiname(uint32_t index) {
    if (index > cpool.multinames.size()) return "OUT OF RANGE";

    std::string result;

    switch (cpool.multinames[index].kind) {
        case CONSTANT_QName:
            result = "kind: QName, " + getNamespace(cpool.multinames[index].ns) + ", name: " + cpool.strings[cpool.multinames[index].name];
            break;
    }
    return result;
}

std::string ABCFile::getNamespace(uint32_t i) {
    return getNamespaceType(cpool.namespaces[i].kind) + "(" + cpool.strings[cpool.namespaces[i].name] + ")";
}

std::string ABCFile::getNamespaceType(uint8_t type) {
    switch (type) {
        case CONSTANT_Namespace: return "Namespace";
        case CONSTANT_PrivateNs: return "PrivateNs";
        case CONSTANT_PackageNamespace: return "PackageNamespace";
        case CONSTANT_PackageInternalNs: return "PackageInternalNs";
        case CONSTANT_ProtectedNamespace: return "ProtectedNamespace";
        case CONSTANT_ExplicitNamespace: return "ExplicitNamespace";
        case CONSTANT_StaticProtectedNs: return "StaticProtectedNs";
        default: throw "Unknown namespace type"; //return "UnknownNS";
    }
}

std::string ABCFile::getNameType(uint8_t type) {
    switch (type) {
        case CONSTANT_QName: return "QName";
        case CONSTANT_QNameA: return "QNameA";
        case CONSTANT_RTQName: return "RTQName";
        case CONSTANT_RTQNameA: return "RTQNameA";
        case CONSTANT_RTQNameL: return "RTQNameL";
        case CONSTANT_RTQNameLA: return "RTQNameLA";
        case CONSTANT_Multiname: return "Multiname";
        case CONSTANT_MultinameA: return "MultinameA";
        case CONSTANT_MultinameL: return "MultinameL";
        case CONSTANT_MultinameLA: return "MultinameLA";
        case CONSTANT_TypeName: return "TypeName";
        default: throw "Unknown name type";
    }
}
