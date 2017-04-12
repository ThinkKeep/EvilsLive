/************************************************************************
#      Copyright (C) 2010-2020 VIAZIJING TECHNOLOGY CO., LTD.
#      All rights reserved.
#***********************************************************************/

#ifndef __AMF_BYTE_STREAM_H__
#define __AMF_BYTE_STREAM_H__


#include "os.h"


inline char* UI08ToBytes(char* buf, uint8 val)
{
	buf[0] = (char)(val) & 0xff;
	return buf+1;
}

inline char* UI16ToBytes(char* buf, uint16 val)
{
	buf[0] = (char)(val >> 8) & 0xff;
	buf[1] = (char)(val) & 0xff;
	return buf+2;
}

inline char* UI24ToBytes(char* buf, uint32 val)
{
	buf[0] = (char)(val >> 16) & 0xff;
	buf[1] = (char)(val >> 8) & 0xff;
	buf[2] = (char)(val) & 0xff;
	return buf+3;
}

inline char* UI32ToBytes(char* buf, uint32 val)
{
	buf[0] = (char)(val >> 24) & 0xff;
	buf[1] = (char)(val >> 16) & 0xff;
	buf[2] = (char)(val >> 8) & 0xff;
	buf[3] = (char)(val) & 0xff;
	return buf+4;
}

inline char* UI64ToBytes(char* buf, uint64 val)
{
	buf[0] = (char)(val >> 56) & 0xff;
	buf[1] = (char)(val >> 48) & 0xff;
	buf[2] = (char)(val >> 40) & 0xff;
	buf[3] = (char)(val >> 32) & 0xff;
	buf[4] = (char)(val >> 24) & 0xff;
	buf[5] = (char)(val >> 16) & 0xff;
	buf[6] = (char)(val >> 8) & 0xff;
	buf[7] = (char)(val) & 0xff;
	return buf+8;
}

inline char* DoubleToBytes(char* buf, double val) 
{
	union
	{
		unsigned char dc[8];
		double dd;
	} d;
	unsigned char b[8];

	d.dd = val;

	b[0] = d.dc[7];
	b[1] = d.dc[6];
	b[2] = d.dc[5];
	b[3] = d.dc[4];
	b[4] = d.dc[3];
	b[5] = d.dc[2];
	b[6] = d.dc[1];
	b[7] = d.dc[0];
	memcpy(buf, b, 8);
	return buf+8;
}

inline uint32 BytesToUI32(const char* buf)
{
	return
		  ((((unsigned int)buf[0]) << 24)	& 0xff000000)
		| ((((unsigned int)buf[1]) << 16)	& 0xff0000)
		| ((((unsigned int)buf[2]) << 8)	& 0xff00)
		| ((((unsigned int)buf[3]))			& 0xff)
	;
}

enum
{
	AMF_DATA_TYPE_NUMBER      = 0x00,
	AMF_DATA_TYPE_BOOL        = 0x01,
	AMF_DATA_TYPE_STRING      = 0x02,
	AMF_DATA_TYPE_OBJECT      = 0x03,
	AMF_DATA_TYPE_NULL        = 0x05,
	AMF_DATA_TYPE_UNDEFINED   = 0x06,
	AMF_DATA_TYPE_REFERENCE   = 0x07,
	AMF_DATA_TYPE_MIXEDARRAY  = 0x08,
	AMF_DATA_TYPE_OBJECT_END  = 0x09,
	AMF_DATA_TYPE_ARRAY       = 0x0a,
	AMF_DATA_TYPE_DATE        = 0x0b,
	AMF_DATA_TYPE_LONG_STRING = 0x0c,
	AMF_DATA_TYPE_UNSUPPORTED = 0x0d,
};

enum
{
	FLV_TAG_TYPE_AUDIO = 0x08,
	FLV_TAG_TYPE_VIDEO = 0x09,
	FLV_TAG_TYPE_META  = 0x12,
};

inline char* AmfStringToBytes(char* buf, const char *str)
{
	char* pbuf = buf;
	size_t len = strlen(str);
	pbuf = UI16ToBytes(pbuf, len);
	memcpy(pbuf, str, len);
	pbuf += len;
	return pbuf;
}

inline char* AmfDoubleToBytes(char* buf, double d)
{
	char* pbuf = buf;
	pbuf = UI08ToBytes(pbuf, AMF_DATA_TYPE_NUMBER);
	pbuf = DoubleToBytes(pbuf, d);
	return pbuf;
}

inline char* AmfBoolToBytes(char* buf, int b)
{
	char* pbuf = buf;
	pbuf = UI08ToBytes(pbuf, AMF_DATA_TYPE_BOOL);
	pbuf = UI08ToBytes(pbuf, !!b);
	return pbuf;
}

#endif // __AMF_BYTE_STREAM_H__
