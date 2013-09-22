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
            return stringf("ProtectedNamespace(%s)", getSTR(namespaces[index].name, true).data());
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
            return stringf("QName(%s, %s)", getSTR(multinames[index].name, true).data(), getNS(multinames[index].ns).data());
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

std::string implode(const std::string &delim, const ABCStringList &items) {
    std::string result;
    for (ABCStringList::const_iterator it = items.begin(); it != items.end(); ++it) {
        if (!result.empty()) result += delim;
        result += *it;
    }
    return result;
}

std::string ABCTrait::getKind() {
    switch (this->kind & 0x0f) {
        case TRAIT_KIND_SLOT:
            return "SLOT";
        case TRAIT_KIND_METHOD:
            return "METHOD";
        case TRAIT_KIND_GETTER:
            return "GETTER";
        case TRAIT_KIND_SETTER:
            return "SETTER";
        case TRAIT_KIND_CLASS:
            return "CLASS";
        case TRAIT_KIND_FUNCTION:
            return "FUNCTION";
        case TRAIT_KIND_CONST:
            return "CONST";
        default:
            DEBUG("KIND=%u", this->kind);
            throw "UNKNOWN TRAIT KIND";
    }
}

ABCStringList ABCTrait::getAttrs()
{
    ABCStringList result;
    if(this->kind & TRAIT_ATTR_FINAL) result.push_back("FINAL");
    if(this->kind & TRAIT_ATTR_OVERRIDE) result.push_back("OVERRIDE");
    if(this->kind & TRAIT_ATTR_METADATA) result.push_back("METADATA");
    return result;
}