#include "ABCTypes.h"
#include "utils.h"

std::string ABCConstantPool::getSTR(uint32_t index, bool quote) {
    if (index >= strings.size()) {
        throw "STRING OUT OF RANGE";
    }

    return quote ? "\"" + strings[index] + "\"" : strings[index];
}

std::string ABCConstantPool::getNS(uint32_t index) {
    if (index >= namespaces.size()) {
        throw "NAMESPACE OUT OF RANGE";
    }

    switch (namespaces[index].kind) {
        case CONSTANT_Namespace:
            return "Namespace(" + getSTR(namespaces[index].name, true) + ")";
        case CONSTANT_PrivateNs:
            return "PrivateNs(" + getSTR(namespaces[index].name, true) + ")";
        case CONSTANT_PackageNamespace:
            return "PackageNamespace(" + getSTR(namespaces[index].name, true) + ")";
        case CONSTANT_PackageInternalNs:
            return "PackageInternalNs(" + getSTR(namespaces[index].name, true) + ")";
        case CONSTANT_ProtectedNamespace:
            return "ProtectedNamespace(" + getSTR(namespaces[index].name, true) + ")";
        case CONSTANT_ExplicitNamespace:
            return "ExplicitNamespace(" + getSTR(namespaces[index].name, true) + ")";
        case CONSTANT_StaticProtectedNs:
            return "StaticProtectedNs(" + getSTR(namespaces[index].name, true) + ")";
        default:
            DEBUG("KIND=%u", namespaces[index].kind);
            throw "UNKNOWN NAMESPACE KIND";
    }
}

std::string ABCConstantPool::getNSS(uint32_t index) {
    if (index >= namespaceSets.size()) {
        throw "NAMESPACE SET OUT OF RANGE";
    }

    std::string result;
    for (ABCNamespaceSet::iterator it = namespaceSets[index].begin(); it != namespaceSets[index].end(); ++it) {
        if (!result.empty()) {
            result += ", ";
        }
        result += getNS(*it);
    }

    return "[" + result + "]";
}

std::string ABCConstantPool::getName(uint32_t index) {
    if (index >= multinames.size()) {
        throw "MULTINAME OUT OF RANGE";
    }

    switch (multinames[index].kind) {
        case CONSTANT_QName:
            return "QName(" + getNS(multinames[index].ns) + ", " + getSTR(multinames[index].name, true) + ")";
        case CONSTANT_QNameA:
            return "QNameA(" + getNS(multinames[index].ns) + ", " + getSTR(multinames[index].name, true) + ")";
        case CONSTANT_RTQName:
            return "RTQname(" + getSTR(multinames[index].name, true) + ")";
        case CONSTANT_RTQNameA:
            return "RTQnameA(" + getSTR(multinames[index].name, true) + ")";
        case CONSTANT_RTQNameL:
            return "RTQNameL()";
        case CONSTANT_RTQNameLA:
            return "RTQNameLA()";
        case CONSTANT_Multiname:
            return "Multiname(" + getSTR(multinames[index].name, true) + ", " + getNSS(multinames[index].nsset) + ")";
        case CONSTANT_MultinameA:
            return "MultinameA(" + getSTR(multinames[index].name, true) + ", " + getNSS(multinames[index].nsset) + ")";
        case CONSTANT_MultinameL:
            return "MultinameL(" + getNSS(multinames[index].nsset) + ")";
        case CONSTANT_MultinameLA:
            return "MultinameLA(" + getNSS(multinames[index].nsset) + ")";
        default:
            DEBUG("KIND=%u", multinames[index].kind);
            throw "UNKNOWN MULTINAME KIND";
    }
}
