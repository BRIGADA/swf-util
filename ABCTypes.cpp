#include <stdexcept>

#include "ABCTypes.h"
#include "utils.h"

std::string ABCTrait::kindString() {
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
            throw std::invalid_argument(stringf("Unknown trait kind (%u)", this->kind));
    }
}

ABCStringList ABCTrait::getAttrs() {
    ABCStringList result;
    if (this->kind & TRAIT_ATTR_FINAL) result.push_back("FINAL");
    if (this->kind & TRAIT_ATTR_OVERRIDE) result.push_back("OVERRIDE");
    if (this->kind & TRAIT_ATTR_METADATA) result.push_back("METADATA");
    return result;
}

std::string ABCConstantPool::getString(uint32_t index) {
    if (index > strings.size()) {
        throw std::range_error(stringf("CPOOL: String index out of range (%u)", index));
    }
    return strings[index];
}

/**
 * Return multiname in human-redable form
 * @param index - index of multiname
 * @return string
 */
std::string ABCConstantPool::getMultiname(uint32_t index) {
    if (index >= multinames.size()) {
        throw std::out_of_range(stringf("CPOOL: Multiname index out of range (%u)", index));
    }

    if (!index) {
        return "*"; // специальное значение - "any name"
    }

    switch (multinames[index].kind) {
        case CONSTANT_QName:
            return stringf("QName('%s', %s)", getString(multinames[index].name).data(), getNamespace(multinames[index].ns).data());
        case CONSTANT_QNameA:
            return stringf("QNameA('%s', %s)", getString(multinames[index].name).data(), getNamespace(multinames[index].ns).data());
        case CONSTANT_RTQName:
            return stringf("RTQname('%s')", getString(multinames[index].name).data());
        case CONSTANT_RTQNameA:
            return stringf("RTQnameA('%s')", getString(multinames[index].name).data());
        case CONSTANT_RTQNameL:
            return "RTQNameL()";
        case CONSTANT_RTQNameLA:
            return "RTQNameLA()";
        case CONSTANT_Multiname:
            return stringf("Multiname('%s', %s)", getString(multinames[index].name).data(), getNamespaceSet(multinames[index].nsset).data());
        case CONSTANT_MultinameA:
            return stringf("MultinameA('%s', %s)", getString(multinames[index].name).data(), getNamespaceSet(multinames[index].nsset).data());
        case CONSTANT_MultinameL:
            return stringf("MultinameL(%s)", getNamespaceSet(multinames[index].nsset).data());
        case CONSTANT_MultinameLA:
            return stringf("MultinameLA(%s)", getNamespaceSet(multinames[index].nsset).data());
        case CONSTANT_TypeName:
        {
            ABCStringList params;
            for (ABCUI32List::iterator it = multinames[index].params.begin(); it != multinames[index].params.end(); ++it) {
                params.push_back(getMultiname(*it));
            }
            return stringf("Typename(%s, <%s>)", getMultiname(multinames[index].name).data(), implode(", ", params).data());
        }
        default:
            throw std::invalid_argument(stringf("CPOOL: Unknown multiname kind (%u)", multinames[index].kind));
    }
}

std::string ABCConstantPool::getMultinameHRF(uint32_t index) {
    if (index >= multinames.size()) {
        throw std::out_of_range(stringf("%s: Multiname index out of range (%u)", __FUNCTION__, index));
    }

    if (!index) {
        return "*"; // специальное значение - "any name"
    }
    
    switch(multinames[index].kind) {
        case CONSTANT_QName:
            return getString(multinames[index].name);
        case CONSTANT_TypeName: {
            std::vector<std::string> params;
            for (ABCUI32List::iterator it = multinames[index].params.begin(); it != multinames[index].params.end(); ++it) {
                params.push_back(getMultinameHRF(*it));
            }
            return stringf("%s<%s>", getMultinameHRF(multinames[index].name).data(), implode(", ", params).data());
        }
        default: 
            throw std::runtime_error(stringf("HRF for multiname not implemented (kind=%u)", multinames[index].kind));
    }
}

std::string ABCConstantPool::getNamespace(uint32_t index) {
    if (index >= namespaces.size()) {
        throw std::out_of_range(stringf("CPOOL: Namespace index out of range (%u)", index));
    }

    if (!index) {
        return "AnyNamespace(*)";
    }

    switch (namespaces[index].kind) {
        case CONSTANT_Namespace:
            return stringf("Namespace('%s')", getString(namespaces[index].name).data());
        case CONSTANT_PrivateNs:
            return stringf("PrivateNs('%s')", getString(namespaces[index].name).data());
        case CONSTANT_PackageNamespace:
            return stringf("PackageNamespace('%s')", getString(namespaces[index].name).data());
        case CONSTANT_PackageInternalNs:
            return stringf("PackageInternalNs('%s')", getString(namespaces[index].name).data());
        case CONSTANT_ProtectedNamespace:
            return stringf("ProtectedNamespace('%s')", getString(namespaces[index].name).data());
        case CONSTANT_ExplicitNamespace:
            return stringf("ExplicitNamespace('%s')", getString(namespaces[index].name).data());
        case CONSTANT_StaticProtectedNs:
            return stringf("StaticProtectedNs('%s')", getString(namespaces[index].name).data());
        default:
            throw std::invalid_argument(stringf("CPOOL: Unknown namespace[%u] kind (%u)", namespaces[index].kind));
    }
}

std::string ABCConstantPool::getNamespaceSet(uint32_t index) {
    if (index >= namespaceSets.size()) {
        throw std::out_of_range(stringf("CPOOL: Namespace set index out of range (%u)", index));
    }

    std::vector<std::string> result;
    for (ABCNamespaceSet::iterator it = namespaceSets[index].begin(); it != namespaceSets[index].begin(); ++it) {
        result.push_back(getNamespace(*it));
    }
    return "[" + implode(", ", result) + "]";
}

std::string ABCConstantPool::getMethodOptionHRF(ABCMethodOption option)
{
    switch (option.kind) {
        case CONSTANT_Undefined: return "undefined";
        case CONSTANT_Utf8: return stringf("\"%s\"", string_escape(getString(option.val)).data());
        case CONSTANT_Int: return stringf("%d", ints[option.val]);
        case CONSTANT_UInt: return stringf("%u", uints[option.val]);
        case CONSTANT_Double: return stringf("%f", doubles[option.val]);
        case CONSTANT_False: return "false";
        case CONSTANT_True: return "true";
        case CONSTANT_Null: return "null";
        default: throw std::invalid_argument(stringf("Unknown constant type", option.kind));
    }
}

std::string ABCConstantPool::getConstantType(uint8_t type) {
    switch (type) {
        case CONSTANT_Undefined: return "Undefined";
        case CONSTANT_Utf8: return "Utf8";
        case CONSTANT_Int: return "Int";
        case CONSTANT_UInt: return "UInt";
        case CONSTANT_Double: return "Double";
        case CONSTANT_False: return "False";
        case CONSTANT_True: return "True";
        case CONSTANT_Null: return "Null";
        case CONSTANT_Namespace: return "Namespace";
        case CONSTANT_PrivateNs: return "PrivateNs";
        case CONSTANT_PackageNamespace: return "PackageNamespace";
        case CONSTANT_PackageInternalNs: return "PackageInternalNs";
        case CONSTANT_ProtectedNamespace: return "ProtectedNamespace";
        case CONSTANT_ExplicitNamespace: return "ExplicitNamespace";
        case CONSTANT_StaticProtectedNs: return "StaticProtectedNs";
        default: throw std::invalid_argument(stringf("Unknown constant type", type));
    }
}

std::string ABCConstantPool::getMultinameType(uint8_t type) {
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
        default: throw std::invalid_argument(stringf("Unknown multiname type (%u)", type));
    }
}
