//
//  endian.h
//  MkXTBWikiplexus
//
//  Created by Kawada Tomoaki on 7/3/11.
//  Copyright 2011 Nexhawks. All rights reserved.
//

#pragma once

#include <stdint.h>

#define XTBBigEndian	4321
#define XTBLittleEndian	1234
#define XTBEndian XTBLittleEndian

static inline uint32_t XTBSwap32(uint32_t value){
	union{
		uint32_t vvvv;
		uint8_t v[4];
	} u1, u2;
	u1.vvvv=value;
	u2.v[0]=u1.v[3];
	u2.v[1]=u1.v[2];
	u2.v[2]=u1.v[1];
	u2.v[3]=u1.v[0];
	return u2.vvvv;
}

static inline uint16_t XTBSwap16(uint16_t value){
	union{
		uint16_t vvvv;
		uint8_t v[2];
	} u1, u2;
	u1.vvvv=value;
	u2.v[0]=u1.v[1];
	u2.v[1]=u1.v[0];
	return u2.vvvv;
}

#if XTBEndian==XTBLittleEndian

#define XTBSysToLE32(v)	(v)
#define XTBLEToSys32(v)	(v)
#define XTBSysToBE32(v)	XTBSwap32(v)
#define XTBBEToSys32(v)	XTBSwap32(v)
#define XTBSysToLE16(v)	(v)
#define XTBLEToSys16(v)	(v)
#define XTBSysToBE16(v)	XTBSwap16(v)
#define XTBBEToSys16(v)	XTBSwap16(v)


#elif XTBEndian==XTBBigEndian

#define XTBSysToLE32(v)	XTBSwap32(v)
#define XTBLEToSys32(v)	XTBSwap32(v)
#define XTBSysToBE32(v)	(v)
#define XTBBEToSys32(v)	(v)
#define XTBSysToLE16(v)	XTBSwap16(v)
#define XTBLEToSys16(v)	XTBSwap16(v)
#define XTBSysToBE16(v)	(v)
#define XTBBEToSys16(v)	(v)

#endif

