/*
 * ABCFile.cpp
 *
 *  Created on: 22.05.2013
 *      Author: brigada
 */

#include "ABCFile.h"
#include "ABCReader.h"
#include <stdio.h>
#include "utils.h"
#include <stdexcept>

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

