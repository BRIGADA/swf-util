/*
 * ABCFile.cpp
 *
 *  Created on: 22.05.2013
 *      Author: brigada
 */

#include "ABCFile.h"
#include "DataReader.h"
#include <stdio.h>

ABCFile::ABCFile() : versionMajor(0), versionMinor(0)
{
	ints.resize(1);
	uints.resize(1);
	strings.resize(1);
	doubles.resize(1);
	namespaces.resize(1);
	namespaceSets.resize(1);
	multinames.resize(1);
}

ABCFile::~ABCFile()
{
}

bool ABCFile::parse(std::string data) {

	DataReader d(data.data(), data.size());
	this->versionMinor = d.readUI16();
	this->versionMajor = d.readUI16();

	printf("Version: %u.%u\n", versionMajor, versionMinor);

	ints.resize(d.readU30());
	if(ints.empty()) ints.resize(1);

	printf("          ints: %lu\n", ints.size());

	for(std::vector<int32_t>::iterator a = ints.begin() + 1; a != ints.end(); ++a) {
		(*a) = d.readU32();
	}

	uints.resize(d.readU30());
	if(uints.empty()) uints.resize(1);

	printf("         uints: %lu\n", uints.size());

	for(std::vector<uint32_t>::iterator a = uints.begin() + 1; a != uints.end(); ++a) {
		(*a) = d.readU32();
	}

	doubles.resize(d.readU30());
	if(doubles.empty()) doubles.resize(1);

	printf("       doubles: %lu\n", doubles.size());

	for(std::vector<double>::iterator a = doubles.begin() + 1; a != doubles.end(); ++a) {
		(*a) = d.readD64();
	}

	strings.resize(d.readU30());
	if(strings.empty()) strings.resize(1);

	printf("       strings: %lu\n", strings.size());

	for(std::vector<std::string>::iterator a = strings.begin() + 1; a != strings.end(); ++a) {
		(*a) = d.readString();
	}

	namespaces.resize(d.readU30());
	if(namespaces.empty()) namespaces.resize(1);

	printf("    namespaces: %lu\n", namespaces.size());

	for(ABCNamespaceList::iterator a = namespaces.begin() + 1; a != namespaces.end(); ++a) {
		(*a).kind = d.readU8();
		(*a).name = d.readU30();
	}

	namespaceSets.resize(d.readU30());
	if(namespaceSets.empty()) namespaceSets.resize(1);

	printf("namespaces_set: %lu\n", namespaceSets.size());

	for(ABCNamespaceSetList::iterator a = namespaceSets.begin() + 1; a != namespaceSets.end(); ++a) {
		(*a).resize(d.readU30());
		for(ABCNamespaceSet::iterator b = (*a).begin(); b != (*a).end(); ++b) {
			(*b) = d.readU30();
		}
	}

	multinames.resize(d.readU30());
	if(multinames.empty()) multinames.resize(1);

	printf("    multinames: %lu\n", multinames.size());

	for(ABCMultinameList::iterator a = multinames.begin() + 1; a != multinames.end(); ++a) {
		(*a).kind = d.readU8();
		switch((*a).kind) {
		case CONSTANT_QName:
		case CONSTANT_QNameA:
			(*a).ns = d.readU30();
			(*a).name = d.readU30();
			break;
		case CONSTANT_RTQName:
		case CONSTANT_RTQNameA:
			(*a).name = d.readU30();
			break;
		case CONSTANT_RTQNameL:
		case CONSTANT_RTQNameLA:
			break;
		case CONSTANT_Multiname:
		case CONSTANT_MultinameA:
			(*a).name = d.readU30();
			(*a).nsset = d.readU30();
			break;
		case CONSTANT_MultinameL:
		case CONSTANT_MultinameLA:
			(*a).nsset = d.readU30();
			break;
		case CONSTANT_TypeName:
			(*a).name = d.readU30();
			(*a).params.resize(d.readU30());
			for(std::vector<uint32_t>::iterator b = (*a).params.begin(); b != (*a).params.end(); ++b) {
				(*b) = d.readU30();
			}
			break;
		default:
			printf("Unknown multiname kind = %u", (*a).kind);
			break;
		}
	}

	methods.resize(d.readU30());
	printf("       methods: %lu\n", methods.size());

	for(ABCMethodInfoList::iterator a = methods.begin(); a != methods.end(); ++a)
	{
		(*a).params.resize(d.readU30());

		(*a).returnType = d.readU30();

		for(ABCMethodParamList::iterator b = (*a).params.begin(); b != (*a).params.end(); ++b) {
			(*b).type = d.readU30();
		}

		(*a).name = d.readU30();

		(*a).flags = d.readU8();

		if((*a).flags & METHOD_FLAG_HAS_OPTIONAL) {
			(*a).options.resize(d.readU30());
			for(ABCMethodOptionList::iterator b = (*a).options.begin(); b != (*a).options.end(); ++b) {
				(*b).val = d.readU30();
				(*b).kind = d.readU8();
			}
		}
		if((*a).flags & METHOD_FLAG_HAS_PARAM_NAMES) {
			for(ABCMethodParamList::iterator b = (*a).params.begin(); b != (*a).params.end(); ++b) {
				(*b).name = d.readU30();
			}
		}
	}

	metadatas.resize(d.readU30());
	printf("     metadatas: %lu\n", metadatas.size());

	for(ABCMetadataInfoList::iterator a = metadatas.begin(); a != metadatas.end(); ++a)
	{
		(*a).name = d.readU30();
		(*a).items.resize(d.readU30());
		for(ABCMetadataItemList::iterator b = (*a).items.begin(); b != (*a).items.end(); ++b) {
			(*b).key = d.readU30();
			(*b).val = d.readU30();
		}
	}

	instances.resize(d.readU30());
	printf("     instances: %lu", instances.size());

	for(ABCInstanceList::iterator a = instances.begin(); a != instances.end(); ++a) {
		(*a) = readInstance(d);
	}

	return true;
}

ABCInstance ABCFile::readInstance(DataReader& reader) {
	ABCInstance result;
	result.name = reader.readU30();
	result.supername = reader.readU30();
	result.flags = reader.readU8();
	if(result.flags & CONSTANT_ClassProtectedNs) {
		result.protectedNS = reader.readU30();
	}
	result.interfaces.resize(reader.readU30());
	for(UI32List::iterator a = result.interfaces.begin(); a != result.interfaces.end(); ++a) {
		(*a) = reader.readU30();
	}
	result.iinit = reader.readU30();
	result.traits.resize(reader.readU30());
	for(ABCTraitsInfoList::iterator a = result.traits.begin(); a != result.traits.end(); ++a) {
		(*a) = readTrait(reader);
	}
	return result;
}

ABCTraitsInfo ABCFile::readTrait(DataReader& reader) {
	ABCTraitsInfo res;
	res.name = reader.readU30();
	res.kindAttr = reader.readU8();
	switch(res.kindAttr) {

	}

	return res;
}
