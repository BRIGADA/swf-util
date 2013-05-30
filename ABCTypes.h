/*
 * ABCTypes.h
 *
 *  Created on: 25.05.2013
 *      Author: brigada
 */

#ifndef ABCTYPES_H_
#define ABCTYPES_H_

#include <vector>
#include <string>
#include <inttypes.h>

typedef std::vector<uint32_t> ABCUI32List;

typedef std::vector<int32_t> ABCSI32List;

typedef std::vector<double> ABCDoubleList;

#define CONSTANT_PrivateNs 		0x05
#define CONSTANT_Namespace 		0x08
#define CONSTANT_PackageNamespace 	0x16
#define CONSTANT_PackageInternalNs 	0x17
#define CONSTANT_ProtectedNamespace     0x18
#define CONSTANT_ExplicitNamespace 	0x19
#define CONSTANT_StaticProtectedNs 	0x1A

struct ABCNamespace {
    uint8_t kind;
    uint32_t name;
};

typedef std::vector<ABCNamespace> ABCNamespaceList;

typedef ABCUI32List ABCNamespaceSet;

typedef std::vector<ABCNamespaceSet> ABCNamespaceSetList;

#define CONSTANT_QName 			0x07
#define CONSTANT_QNameA 		0x0D
#define CONSTANT_RTQName 		0x0F
#define CONSTANT_RTQNameA 		0x10
#define CONSTANT_RTQNameL 		0x11
#define CONSTANT_RTQNameLA 		0x12
#define CONSTANT_Multiname 		0x09
#define CONSTANT_MultinameA 		0x0E
#define CONSTANT_MultinameL 		0x1B
#define CONSTANT_MultinameLA 		0x1C
#define CONSTANT_TypeName 		0x1D

struct ABCMultiname {
    uint8_t kind;
    uint32_t ns;
    uint32_t name;
    uint32_t nsset;
    ABCUI32List params;
};

typedef std::vector<ABCMultiname> ABCMultinameList;

typedef std::vector<std::string> ABCStringList;

struct ABCMetadataItem {
    uint32_t key;
    uint32_t val;
};

typedef std::vector<ABCMetadataItem> ABCMetadataItemList;

struct ABCMetadata {
    uint32_t name;
    ABCMetadataItemList items;
};

typedef std::vector<ABCMetadata> ABCMetadataList;

struct ABCMethodOption {
    uint32_t val;
    uint8_t kind;
};

typedef std::vector<ABCMethodOption> ABCMethodOptionList;

struct ABCMethodParam {
    uint32_t type;
    uint32_t name;
};

typedef std::vector<ABCMethodParam> ABCMethodParamList;

/* Suggests to the run-time that an “arguments” object (as specified by the 
 * ActionScript 3.0 Language Reference) be created. Must not be used together
 * with NEED_REST. See Chapter 3.*/
#define METHOD_FLAG_NEED_ARGUMENTS	0x01

/* Must be set if this method uses the newactivation opcode. */
#define METHOD_FLAG_NEED_ACTIVATION     0x02

/* This flag creates an ActionScript 3.0 rest arguments array. Must not be used
 * with NEED_ARGUMENTS. See Chapter 3. */
#define METHOD_FLAG_NEED_REST 		0x04

/* Must be set if this method has optional parameters and the options field is
 * present in this method_info structure. */
#define METHOD_FLAG_HAS_OPTIONAL 	0x08

/* Must be set if this method uses the dxns or dxnslate opcodes. */
#define METHOD_FLAG_SET_DXNS            0x40

/* Must be set when the param_names field is present in this method_info
 * structure. */
#define METHOD_FLAG_HAS_PARAM_NAMES     0x80

struct ABCMethod {
    uint32_t name;
    uint8_t flags;
    ABCMethodOptionList options;
    ABCMethodParamList params;
    uint32_t returnType;
};

typedef std::vector<ABCMethod> ABCMethodList;

#define TRAIT_KIND_SLOT		0
#define TRAIT_KIND_METHOD	1
#define TRAIT_KIND_GETTER	2
#define TRAIT_KIND_SETTER	3
#define TRAIT_KIND_CLASS	4
#define TRAIT_KIND_FUNCTION	5
#define TRAIT_KIND_CONST	6

/* Is used with Trait_Method, Trait_Getter and Trait_Setter. It marks a method
 * that cannot be overridden by a sub-class */
#define TRAIT_ATTR_FINAL 	0x10   

/*Is used with Trait_Method, Trait_Getter and Trait_Setter. It marks a method
 * that has been overridden in this class */
#define TRAIT_ATTR_OVERRIDE	0x20

/* Is used to signal that the fields metadata_count and metadata follow the
 * data field in the traits_info entry */
#define TRAIT_ATTR_METADATA	0x40

struct ABCTrait {
    uint32_t name;
    uint8_t kind;

    union {

        struct {
            uint32_t slotID;
            uint32_t typeName;
            uint32_t vindex;
            uint8_t vkind;
        } Slot;

        struct {
            uint32_t slotID;
            uint32_t classi;
        } Class;

        struct {
            uint32_t slotID;
            uint32_t functioni;
        } Function;

        struct {
            uint32_t dispID;
            uint32_t method;
        } Method;
    };

    ABCUI32List metadata;
};

typedef std::vector<ABCTrait> ABCTraitList;

/*
 * The class is sealed: properties can not be dynamically added to instances
 * of the class.
 */
#define INSTANCE_FLAG_SEALED            0x01

/*
 * The class is final: it cannot be a base class for any other class.
 */
#define INSTANCE_FLAG_FINAL 		0x02

/*
 * The class is an interface.
 */
#define INSTANCE_FLAG_INTERFACE 	0x04

/*
 * The class uses its protected namespace and the protectedNs field is present
 * in the interface_info structure.
 */
#define INSTANCE_FLAG_PROTECTED_NS 	0x08

struct ABCInstance {
    uint32_t name;
    uint32_t super;
    uint8_t flags;
    uint32_t protectedNS;
    ABCUI32List interfaces;
    uint32_t iinit;
    ABCTraitList traits;
};

typedef std::vector<ABCInstance> ABCInstanceList;

struct ABCClass {
    uint32_t cinit;
    ABCTraitList traits;
};

typedef std::vector<ABCClass> ABCClassList;

struct ABCScript {
    uint32_t init;
    ABCTraitList traits;
};

typedef std::vector<ABCScript> ABCScriptList;

/**
 * The exception_info entry is used to define the range of ActionScript 3.0
 * instructions over which a particular exception handler is engaged.
 */
struct ABCException {
    uint32_t from;      /* The starting position in the code field from which
                         * the exception is enabled. */

    uint32_t to;        /* The ending position in the code field after which
                         * the exception is disabled. */

    uint32_t target;    /* The position in the code field to which control
                         * should jump if an exception of type exc_type is
                         * encountered while executing instructions that lie
                         * within the region [from, to] of the code field. */

    uint32_t type;      /* An index into the string array of the constant pool
                         * that identifies the name of the type of exception
                         * that is to be monitored during the reign of this
                         * handler. A value of zero means the any type (“*”)
                         * and implies that this exception handler will catch
                         * any type of exception thrown. */

    uint32_t varName;   /* This index into the string array of the constant
                         * pool defines the name of the variable that is to
                         * receive the exception object when the exception is
                         * thrown and control is transferred to target location.
                         * If the value is zero then there is no name
                         * associated with the exception object. */
};

typedef std::vector<ABCException> ABCExceptionList;

struct ABCBody {
    uint32_t method;
    uint32_t maxStack;
    uint32_t localCount;
    uint32_t initScopeDepth;
    uint32_t maxScopeDepth;
    std::string code;
    ABCExceptionList exceptions;
    ABCTraitList traits;
};

typedef std::vector<ABCBody> ABCBodyList;

struct ABCConstantPool {
    ABCSI32List ints;
    ABCUI32List uints;
    ABCDoubleList doubles;
    ABCStringList strings;
    ABCNamespaceList namespaces;
    ABCNamespaceSetList namespaceSets;
    ABCMultinameList multinames;
};

#endif /* ABCTYPES_H_ */
