/*
 * ABCFile.h
 *
 *  Created on: 22.05.2013
 *      Author: brigada
 */

#ifndef ABCFILE_H_
#define ABCFILE_H_
#include <inttypes.h>
#include "ABCTypes.h"
#include <string>

class ABCFile
{
public:
	ABCFile();
	virtual ~ABCFile();

	uint16_t versionMajor;
	uint16_t versionMinor;
        
        ABCConstantPool cpool;
        
	ABCMethodList methods;
	ABCMetadataList metadatas;
	ABCInstanceList instances;
	ABCClassList classes;
	ABCScriptList scripts;
	ABCBodyList bodies;

	std::string getMultiname(uint32_t i);
	std::string getNamespace(uint32_t i);

	static std::string getNamespaceType(uint8_t type);
	static std::string getNameType(uint8_t type);
};

#endif /* ABCFILE_H_ */
