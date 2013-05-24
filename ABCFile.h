/*
 * ABCFile.h
 *
 *  Created on: 22.05.2013
 *      Author: brigada
 */

#ifndef ABCFILE_H_
#define ABCFILE_H_
#include <inttypes.h>
#include <string>
#include <vector>
#include "DataReader.h"

typedef std::vector<uint32_t> UI32List;

#define CONSTANT_QName 			0x07
#define CONSTANT_QNameA 		0x0D
#define CONSTANT_RTQName 		0x0F
#define CONSTANT_RTQNameA 		0x10
#define CONSTANT_RTQNameL 		0x11
#define CONSTANT_RTQNameLA 		0x12
#define CONSTANT_Multiname		0x09
#define CONSTANT_MultinameA		0x0E
#define CONSTANT_MultinameL		0x1B
#define CONSTANT_MultinameLA 	0x1C
#define CONSTANT_TypeName 		0x1D

#define CONSTANT_Namespace 			0x08
#define CONSTANT_PackageNamespace 	0x16
#define CONSTANT_PackageInternalNs 	0x17
#define CONSTANT_ProtectedNamespace 0x18
#define CONSTANT_ExplicitNamespace 	0x19
#define CONSTANT_StaticProtectedNs 	0x1A
#define CONSTANT_PrivateNs 			0x05


#define CONSTANT_ClassSealed        0x01	// The class is sealed: properties can not be dynamically added
											// to instances of the class.

#define CONSTANT_ClassFinal 		0x02 	// The class is final: it cannot be a base class for any other
											// class.

#define CONSTANT_ClassInterface 	0x04 	// The class is an interface.

#define CONSTANT_ClassProtectedNs 	0x08 	// The class uses its protected namespace and the
											// protectedNs field is present in the interface_info
											// structure.

#define METHOD_FLAG_NEED_ARGUMENTS	0x01 	// Suggests to the run-time that an “arguments” object (as specified by the
											// ActionScript 3.0 Language Reference) be created. Must not be used
											// together with NEED_REST. See Chapter 3.

#define METHOD_FLAG_NEED_ACTIVATION 0x02	// Must be set if this method uses the newactivation opcode.

#define METHOD_FLAG_NEED_REST 		0x04	// This flag creates an ActionScript 3.0 rest arguments array. Must not be
											// used with NEED_ARGUMENTS. See Chapter 3.

#define METHOD_FLAG_HAS_OPTIONAL 	0x08	// Must be set if this method has optional parameters and the options
											// field is present in this method_info structure.

#define METHOD_FLAG_SET_DXNS 		0x40	// Must be set if this method uses the dxns or dxnslate opcodes.

#define METHOD_FLAG_HAS_PARAM_NAMES 0x80 	// Must be set when the param_names field is present in this method_info structure.

#define TRAIT_TYPE_SLOT				0
#define TRAIT_TYPE_METHOD			1
#define TRAIT_TYPE_GETTER			2
#define TRAIT_TYPE_SETTER			3
#define TRAIT_TYPE_CLASS			4
#define TRAIT_TYPE_FUNCTION			5
#define TRAIT_TYPE_CONST			6

struct ABCNamespace {
	uint8_t kind;
	uint32_t name;
};

typedef std::vector<ABCNamespace> ABCNamespaceList;

typedef UI32List ABCNamespaceSet;

typedef std::vector<ABCNamespaceSet> ABCNamespaceSetList;

struct ABCMultiname {
	uint8_t kind;
	uint32_t ns;
	uint32_t name;
	uint32_t nsset;
	UI32List params;
};

typedef std::vector<ABCMultiname> ABCMultinameList;

struct ABCOptionDetail {
	uint32_t val;
	uint8_t kind;
};

struct ABCMethodParam {
	uint32_t type;
	uint32_t name;
};

struct ABCMetadataItem {
	uint32_t key;
	uint32_t val;
};

typedef std::vector<ABCMetadataItem> ABCMetadataItemList;

struct ABCMetadataInfo {
	uint32_t name;
	ABCMetadataItemList items;
};

typedef std::vector<ABCMetadataInfo> ABCMetadataInfoList;

typedef std::vector<ABCOptionDetail> ABCMethodOptionList;

typedef std::vector<ABCMethodParam> ABCMethodParamList;

struct ABCMethodInfo {
	uint32_t name;
	uint8_t flags;
	ABCMethodOptionList options;
	ABCMethodParamList params;
	uint32_t returnType;
};

typedef std::vector<ABCMethodInfo> ABCMethodInfoList;

struct ABCTraitsInfo {
	uint32_t name;
	uint8_t kindAttr;
	union {
		struct {
			uint32_t slotID;
			uint32_t typeName;
			uint32_t vindex;
			uint8_t  vkind;
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

	UI32List metadata;

	uint8_t kind() {
		return kindAttr & 0x0f;
	}

	void kind(uint8_t value) {
		kindAttr = (kindAttr & 0xF0) | value;
	}

	uint8_t attr() {
		return kindAttr >> 4;
	}

	void attr(uint8_t value) {
		kindAttr = (kindAttr & 0x0F) | (value << 4);
	}
};

typedef std::vector<ABCTraitsInfo> ABCTraitsInfoList;

struct ABCInstance {
	uint32_t name;
	uint32_t supername;
	uint8_t flags;
	uint32_t protectedNS;
	UI32List interfaces;
	uint32_t iinit;
	ABCTraitsInfoList traits;
};

typedef std::vector<ABCInstance> ABCInstanceList;

class ABCFile
{
public:
	ABCFile();
	virtual ~ABCFile();

	uint16_t versionMajor;
	uint16_t versionMinor;

	std::vector<uint32_t> metadata;


	std::vector<int32_t> ints;
	std::vector<uint32_t> uints;
	std::vector<double> doubles;
	std::vector<std::string> strings;
	ABCNamespaceList namespaces;
	ABCNamespaceSetList namespaceSets;
	ABCMultinameList multinames;

	ABCMethodInfoList methods;

	ABCMetadataInfoList metadatas;

	ABCInstanceList instances;

	bool parse(std::string data);
private:
	ABCInstance readInstance(DataReader &reader);
	ABCTraitsInfo readTrait(DataReader &reader);
};

#endif /* ABCFILE_H_ */
