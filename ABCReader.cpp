/*
 * ABCReader.cpp
 *
 *  Created on: 25.05.2013
 *      Author: brigada
 */

#include "ABCReader.h"
#include <stdio.h>
#include <algorithm>

#define OOR_EXCEPTION_MESSAGE "data ptr out of range"

ABCReader::ABCReader(const char* data, uint32_t size) : _cur(data), _end(data + size){
}

ABCReader::ABCReader(const std::string& data) : _cur(data.data()), _end(data.data() + data.size()){
}

ABCReader::~ABCReader() {
}

uint8_t ABCReader::readU8() {
	if(_cur + 1 > _end) throw OOR_EXCEPTION_MESSAGE;
	uint8_t r = *_cur;
	_cur++;
	return r;
}

uint16_t ABCReader::readU16() {
	if(_cur + 2 > _end) throw OOR_EXCEPTION_MESSAGE;
	uint16_t r = *(uint16_t*)_cur;
	_cur+=2;
	return r;
}

uint32_t ABCReader::readU30() {
	return 0x3fffffff & readU32();
}

uint32_t ABCReader::readU32() {
	if(_cur + 1 > _end) throw OOR_EXCEPTION_MESSAGE;
	uint32_t r = *_cur;
	_cur++;
	if (r >> 7)
	{
		if(_cur + 1 > _end) throw OOR_EXCEPTION_MESSAGE;
		r = (r & 0x0000007f) | ((uint32_t)*_cur) << 7;
		_cur++;
		if (r >> 14)
		{
			if(_cur + 1 > _end) throw OOR_EXCEPTION_MESSAGE;
			r = (r & 0x00003fff) | ((uint32_t)*_cur) << 14;
			_cur++;
			if (r >> 21)
			{
				if(_cur + 1 > _end) throw OOR_EXCEPTION_MESSAGE;
				r = (r & 0x001fffff) | ((uint32_t)*_cur) << 21;
				_cur++;
				if (r >> 28)
				{
					if(_cur + 1 > _end) throw OOR_EXCEPTION_MESSAGE;
					r = (r & 0x0fffffff) | ((uint32_t)*_cur) << 28;
					_cur++;
				}
			}
		}
	}
	return r;
}

int32_t ABCReader::readS24() {
	if(_cur + 3 > _end) throw OOR_EXCEPTION_MESSAGE;
	int32_t r = (((int32_t)*(_cur + 2)) << 16) | *(int16_t*)_cur;
	_cur += 3;
	return r & 0x00800000 ? 0xff000000 | r : r;
}

int32_t ABCReader::readS32() {
	return readU32();
}

double ABCReader::readD64() {
	if(_cur + 8 > _end) throw OOR_EXCEPTION_MESSAGE;
	double r = *(double*)_cur;
	_cur += 8;
	return r;
}

void ABCReader::readString(std::string& res) {
	uint32_t l = readU30();
	if(_cur + l > _end) throw OOR_EXCEPTION_MESSAGE;
	res.assign(_cur, l);
	_cur += l;
}

void ABCReader::readTrait(ABCTrait& res) {
	res.name = readU30();
	res.kind = readU8();
	switch(res.kind & 0x0f) {
	case TRAIT_KIND_SLOT:
	case TRAIT_KIND_CONST:
		res.Slot.slotID = readU30();
		res.Slot.typeName = readU30();
		res.Slot.vindex = readU30();
		if(res.Slot.vindex) {
			res.Slot.vkind = readU8();
		}
		break;
	case TRAIT_KIND_METHOD:
	case TRAIT_KIND_GETTER:
	case TRAIT_KIND_SETTER:
		res.Method.dispID = readU30();
		res.Method.method = readU30();
		break;
	case TRAIT_KIND_CLASS:
		res.Class.slotID = readU30();
		res.Class.classi = readU30();
		break;
	case TRAIT_KIND_FUNCTION:
		res.Function.slotID = readU30();
		res.Function.functioni = readU30();
		break;
	default:
		printf("trait kind=%#0x\n", res.kind);
		throw "Unknown trait kind";
	}

	if(res.kind & TRAIT_ATTR_METADATA) {
		res.metadata.resize(readU30());
		for(ABCUI32List::iterator a = res.metadata.begin(); a != res.metadata.end(); ++a) {
			(*a) = readU30();
		}
	}
}

void ABCReader::readTraitList(ABCTraitList& res) {
	res.resize(readU30());
	for(ABCTraitList::iterator it = res.begin(); it != res.end(); ++it) {
		readTrait(*it);
	}
}

ABCFile* ABCReader::read() {
	ABCFile * result = new ABCFile();

	result->versionMinor = readU16();
	result->versionMajor = readU16();

	result->ints.resize(readU30());
	if(result->ints.empty()) result->ints.resize(1);
	for(ABCSI32List::iterator a = result->ints.begin() + 1; a != result->ints.end(); ++a) {
		(*a) = readU32();
	}

	result->uints.resize(readU30());
	if(result->uints.empty()) result->uints.resize(1);
	for(std::vector<uint32_t>::iterator a = result->uints.begin() + 1; a != result->uints.end(); ++a) {
		(*a) = readU32();
	}

	result->doubles.resize(readU30());
	if(result->doubles.empty()) result->doubles.resize(1);
	for(ABCDoubleList::iterator a = result->doubles.begin() + 1; a != result->doubles.end(); ++a) {
		(*a) = readD64();
	}

	result->strings.resize(readU30());
	if(result->strings.empty()) result->strings.resize(1);
	for(ABCStringList::iterator a = result->strings.begin() + 1; a != result->strings.end(); ++a) {
		readString(*a);
	}

	result->namespaces.resize(readU30());
	if(result->namespaces.empty()) result->namespaces.resize(1);
	for(ABCNamespaceList::iterator a = result->namespaces.begin() + 1; a != result->namespaces.end(); ++a) {
		(*a).kind = readU8();
		(*a).name = readU30();
	}

	result->namespaceSets.resize(readU30());
	if(result->namespaceSets.empty()) result->namespaceSets.resize(1);
	for(ABCNamespaceSetList::iterator a = result->namespaceSets.begin() + 1; a != result->namespaceSets.end(); ++a) {
		(*a).resize(readU30());
		for(ABCNamespaceSet::iterator b = (*a).begin(); b != (*a).end(); ++b) {
			(*b) = readU30();
		}
	}

	result->multinames.resize(readU30());
	if(result->multinames.empty()) result->multinames.resize(1);
	for(ABCMultinameList::iterator a = result->multinames.begin() + 1; a != result->multinames.end(); ++a) {
		(*a).kind = readU8();
		switch((*a).kind) {
		case CONSTANT_QName:
		case CONSTANT_QNameA:
			(*a).ns = readU30();
			(*a).name = readU30();
			break;
		case CONSTANT_RTQName:
		case CONSTANT_RTQNameA:
			(*a).name = readU30();
			break;
		case CONSTANT_RTQNameL:
		case CONSTANT_RTQNameLA:
			break;
		case CONSTANT_Multiname:
		case CONSTANT_MultinameA:
			(*a).name = readU30();
			(*a).nsset = readU30();
			break;
		case CONSTANT_MultinameL:
		case CONSTANT_MultinameLA:
			(*a).nsset = readU30();
			break;
		case CONSTANT_TypeName:
			(*a).name = readU30();
			(*a).params.resize(readU30());
			for(ABCUI32List::iterator it = (*a).params.begin(); it != (*a).params.end(); ++it) {
				(*it) = readU30();
			}
			break;
		default:
			printf("Unknown multiname kind = %u", (*a).kind);
			throw "Unknown multiname kind";
			break;
		}
	}

	result->methods.resize(readU30());
	for(ABCMethodList::iterator a = result->methods.begin(); a != result->methods.end(); ++a) {
		(*a).params.resize(readU30());
		(*a).returnType = readU30();
		for(ABCMethodParamList::iterator b = (*a).params.begin(); b != (*a).params.end(); ++b) {
			(*b).type = readU30();
		}
		(*a).name = readU30();
		(*a).flags = readU8();
		if((*a).flags & METHOD_FLAG_HAS_OPTIONAL) {
			(*a).options.resize(readU30());
			for(ABCMethodOptionList::iterator b = (*a).options.begin(); b != (*a).options.end(); ++b) {
				(*b).val = readU30();
				(*b).kind = readU8();
			}
		}
		if((*a).flags & METHOD_FLAG_HAS_PARAM_NAMES) {
			for(ABCMethodParamList::iterator b = (*a).params.begin(); b != (*a).params.end(); ++b) {
				(*b).name = readU30();
			}
		}
	}

	result->metadatas.resize(readU30());
	for(ABCMetadataList::iterator a = result->metadatas.begin(); a != result->metadatas.end(); ++a) {
		(*a).name = readU30();
		(*a).items.resize(readU30());
		for (ABCMetadataItemList::iterator b = (*a).items.begin(); b != (*a).items.end(); ++b) {
			(*b).key = readU30();
			(*b).val = readU30();
		}
	}

	uint32_t class_count = readU30();

	result->instances.resize(class_count);
	for(ABCInstanceList::iterator a = result->instances.begin(); a != result->instances.end(); ++a) {
		(*a).name = readU30();
		(*a).super = readU30();
		(*a).flags = readU8();
		if((*a).flags & INSTANCE_FLAG_PROTECTED_NS) {
			(*a).protectedNS = readU30();
		}
		(*a).interfaces.resize(readU30());
		for(ABCUI32List::iterator b = (*a).interfaces.begin(); b != (*a).interfaces.end(); ++b) {
			(*b) = readU30();
		}
		(*a).iinit = readU30();
		readTraitList((*a).traits);
	}

	result->classes.resize(class_count);
	for(ABCClassList::iterator a = result->classes.begin(); a != result->classes.end(); ++a) {
		(*a).cinit = readU30();
		readTraitList((*a).traits);
	}

	result->scripts.resize(readU30());
	for(ABCScriptList::iterator a = result->scripts.begin(); a != result->scripts.end(); ++a ) {
		(*a).init = readU30();
		readTraitList((*a).traits);
	}
	// reading method_body_info's...
	result->bodies.resize(readU30());
	for(ABCBodyList::iterator a = result->bodies.begin(); a != result->bodies.end(); ++a ) {
		(*a).method = readU30();
		(*a).maxStack = readU30();
		(*a).localCount = readU30();
		(*a).initScopeDepth = readU30();
		(*a).maxScopeDepth = readU30();
		readString((*a).code);
		(*a).exceptions.resize(readU30());
		for(ABCExceptionList::iterator b = (*a).exceptions.begin(); b != (*a).exceptions.end(); ++b) {
			(*b).from = readU30();
			(*b).to = readU30();
			(*b).target = readU30();
			(*b).type = readU30();
			(*b).varName = readU30();
		}
		readTraitList((*a).traits);
	}

	return result;
}
