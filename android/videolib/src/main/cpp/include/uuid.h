/************************************************************************
#      Copyright (C) 2010-2020 VIAZIJING TECHNOLOGY CO., LTD.
#      All rights reserved.
#***********************************************************************/

#ifndef __UUID_WIN32_H__
#define __UUID_WIN32_H__


#undef uuid_t
typedef unsigned char	uuid_t[16];

struct uuid
{
	uint32	time_low;
	uint16	time_mid;
	uint16	time_hi_and_version;
	uint16	clock_seq;
	uint8	node[6];
};

inline void uuid_clear(uuid_t uu)
{
	memset(uu, 0, 16);
}

inline void uuid_pack(const struct uuid *uu, uuid_t ptr)
{
	uint32	tmp;
	unsigned char	*out = ptr;

	tmp = uu->time_low;
	out[3] = (unsigned char) tmp;
	tmp >>= 8;
	out[2] = (unsigned char) tmp;
	tmp >>= 8;
	out[1] = (unsigned char) tmp;
	tmp >>= 8;
	out[0] = (unsigned char) tmp;

	tmp = uu->time_mid;
	out[5] = (unsigned char) tmp;
	tmp >>= 8;
	out[4] = (unsigned char) tmp;

	tmp = uu->time_hi_and_version;
	out[7] = (unsigned char) tmp;
	tmp >>= 8;
	out[6] = (unsigned char) tmp;

	tmp = uu->clock_seq;
	out[9] = (unsigned char) tmp;
	tmp >>= 8;
	out[8] = (unsigned char) tmp;

	memcpy(out+10, uu->node, 6);
}

inline void uuid_unpack(const uuid_t in, struct uuid *uu)
{
	const uint8		*ptr = in;
	uint32			tmp;

	tmp = *ptr++;
	tmp = (tmp << 8) | *ptr++;
	tmp = (tmp << 8) | *ptr++;
	tmp = (tmp << 8) | *ptr++;
	uu->time_low = tmp;

	tmp = *ptr++;
	tmp = (tmp << 8) | *ptr++;
	uu->time_mid = tmp;

	tmp = *ptr++;
	tmp = (tmp << 8) | *ptr++;
	uu->time_hi_and_version = tmp;

	tmp = *ptr++;
	tmp = (tmp << 8) | *ptr++;
	uu->clock_seq = tmp;

	memcpy(uu->node, ptr, 6);
}

inline int uuid_parse(const char* in, uuid_t uu)
{
	struct uuid	uuid;
	int			i;
	const char*	cp;
	char		buf[3];

	if (36 != strlen(in))
		return -1;

	for (i=0, cp=in; i<=36; i++, cp++)
	{
		if ((i == 8) || (i == 13) || (i == 18) || (i == 23))
		{
			if (*cp == '-')
				continue;
			else
				return -1;
		}

		if (i== 36)
			if (*cp == 0)
				continue;

		if (!isxdigit(*cp))
			return -1;
	}

	uuid.time_low = strtoul(in, NULL, 16);
	uuid.time_mid = (uint16)strtoul(in+9, NULL, 16);
	uuid.time_hi_and_version = (uint16)strtoul(in+14, NULL, 16);
	uuid.clock_seq = (uint16)strtoul(in+19, NULL, 16);

	cp = in+24;
	buf[2] = 0;

	for (i=0; i<6; i++)
	{
		buf[0] = *cp++;
		buf[1] = *cp++;
		uuid.node[i] = (uint8)strtoul(buf, NULL, 16);
	}

	uuid_pack(&uuid, uu);

	return 0;
}

inline void uuid_unparse_upper(const unsigned char* pBinary, char* m_szGUID)
{
	sprintf(
		m_szGUID,
		"%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
		pBinary[0],  pBinary[1],  pBinary[2],  pBinary[3],
		pBinary[4],  pBinary[5],  pBinary[6],  pBinary[7],
		pBinary[8],  pBinary[9],  pBinary[10], pBinary[11],
		pBinary[12], pBinary[13], pBinary[14], pBinary[15]
	);
}

inline void get_random_bytes(char* buf, int len)
{
	static int uuit_rand_init = 0;

	if (0 == uuit_rand_init)
	{
		uuit_rand_init = 1;
		srand((unsigned int)time(NULL));

		for (int k=0; k<len; k++)
			buf[k] = (char)(0xFF & (rand()));
	}

	for (int k=0; k<len; k++)
		buf[k] = (char)(0xFF & (rand()));
}

inline void uuid_generate_random(uuid_t out)
{
	uuid_t	buf;
	struct uuid uu;
	int i, n;

	n = 1;

	for (i = 0; i < n; i++) {
		get_random_bytes((char*)&buf, sizeof(buf));
		uuid_unpack(buf, &uu);

		uu.clock_seq = (uu.clock_seq & 0x3FFF) | 0x8000;
		uu.time_hi_and_version = (uu.time_hi_and_version & 0x0FFF) | 0x4000;
		uuid_pack(&uu, out);
		out += sizeof(uuid_t);
	}
}

inline void uuid_generate(uuid_t out)
{
	uuid_generate_random(out);
}


#endif	//#ifndef __UUID_WIN32_H__
