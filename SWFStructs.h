/*
 * SWFStructs.h
 *
 *  Created on: 14.05.2013
 *      Author: brigada
 */

#ifndef SWFSTRUCTS_H_
#define SWFSTRUCTS_H_

#include <inttypes.h>

struct SWFRect_t {
	int32_t Xmin;
	int32_t Xmax;
	int32_t Ymin;
	int32_t Ymax;
};

typedef struct {
    uint8_t signature[3];
    uint8_t version;
    uint32_t filesize;
} SWFHeader_t;

#endif /* SWFSTRUCTS_H_ */
