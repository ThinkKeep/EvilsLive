/************************************************************************
#      Copyright (C) 2010-2020 VIAZIJING TECHNOLOGY CO., LTD.
#      All rights reserved.
#***********************************************************************/

#ifndef __OS_WIN32_H__
#define __OS_WIN32_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <io.h>
#include <conio.h>
#include <process.h>
#include <fcntl.h>
#include <direct.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <sys/stat.h>
#include <assert.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <windows.h>


#define PRINTF					printf


typedef char					int8;
typedef unsigned char			uint8;
typedef short					int16;
typedef unsigned short			uint16;
typedef int						int32;
typedef unsigned int			uint32;
typedef long long				int64;
typedef unsigned long long		uint64;
typedef float					float32;
typedef double					float64;

#include "uuid.h"


typedef HANDLE					thread_t;
typedef HANDLE					mutex_t;
typedef HANDLE					sem_t;


#define S_IRUSR					_S_IREAD
#define S_IWUSR					_S_IWRITE

#define SHUT_RDWR				SD_BOTH

#define MSG_DONTWAIT			MSG_PARTIAL
#define MSG_NOSIGNAL			0

#define zjv_open				_open
#define zjv_read				_read
#define zjv_write				_write
#define zjv_lseek				_lseeki64
#define zjv_length				_filelengthi64
#define zjv_unlink				_unlink
#define zjv_rename				rename
#define zjv_flush				_commit
#define zjv_close				_close


inline int SleepUs(int nMicroseconds)
{
	SleepEx(nMicroseconds / 1000, FALSE);
	return 0;
}

inline void gettimeofday (struct timeval *tv, void *dummy)
{
	FILETIME	ftime;
	uint64		n;

	GetSystemTimeAsFileTime(&ftime);
	n = (((uint64)ftime.dwHighDateTime << 32) + (uint64)ftime.dwLowDateTime);
	if (n)
	{
		n /= 10;
		n -= ((369 * 365 + 89) * (uint64) 86400) * 1000000;
	}

	tv->tv_sec  = (long)(n / 1000000);
	tv->tv_usec = (long)(n % 1000000);
}

inline int _create_file(const char* path)
{
	int fd = _open(path, _O_CREAT | _O_WRONLY | _O_BINARY | _O_TRUNC, _S_IREAD | _S_IWRITE);
	if (-1 == fd) return errno;

	_close(fd);

	return 0;
}

inline int _create_path(const char* path)
{
	return _mkdir(path);
}

inline int _move_path(const char* oldpath, const char* newpath)
{
	return rename(oldpath, newpath);
}

inline int _unlink_path(const char* path)
{
	int err = 0;

	struct _stat st;
	memset(&st, 0, sizeof(st));

	err = _stat(path, &st);
	if (-1 == err) return errno;

	if (S_IFREG & st.st_mode)
	{
		err = _unlink(path);
	}

	if (S_IFDIR & st.st_mode)
	{
		WIN32_FIND_DATA		FindFileData;
		HANDLE				hFind;
		char				szFindPath[MAX_PATH];

		sprintf(szFindPath, "%s/*.*", path);

		memset(&FindFileData, 0, sizeof(FindFileData));
		hFind = FindFirstFile(szFindPath, &FindFileData);

		if (INVALID_HANDLE_VALUE == hFind)
		{
			return ENOENT;
		} 
		else 
		{
			while (1)
			{
				if (0 != strcmp(".", FindFileData.cFileName) && 0 != strcmp("..", FindFileData.cFileName))
				{
					char szSubPath[MAX_PATH];

					sprintf(szSubPath, "%s/%s", path, FindFileData.cFileName);
					err = _unlink_path(szSubPath);

					if (0 != err) break;
				}

				BOOL b = FindNextFile(hFind, &FindFileData);
				if (!b) break;
			}

			FindClose(hFind);
		}

		if (0 != err) return err;

		err = _rmdir(path);
	}

	return err;
}


/*
 * class CMutex
 */
class CMutex
{
public:
	CMutex()
	{
		m_mutex = CreateMutex(NULL, FALSE, NULL);
	}

	~CMutex()
	{
		CloseHandle(m_mutex);
	}

	inline bool Lock(int timeout = -1)
	{
		int ret;

		if (-1 == timeout)
			ret = WaitForSingleObject(m_mutex, INFINITE);
		else
			ret = WaitForSingleObject(m_mutex, timeout);

		if (WAIT_OBJECT_0 == ret)
			return true;
		else
			return false;
	}

	inline void Lock(const char* szFile, int iLine)
	{
		PRINTF("Lock() enter, %s, %d\n", szFile, iLine);
		int ret = WaitForSingleObject(m_mutex, 10000);
		if (WAIT_OBJECT_0 != ret)
		{
			PRINTF("lock timeout, %s, %d\n", szFile, iLine);
			exit(1);
		}
		PRINTF("Lock() leave, %s, %d ---\n\n", szFile, iLine);
	}

	inline void Unlock()
	{
		ReleaseMutex(m_mutex);
	}
	
private:
	mutex_t		m_mutex;
};


/*
 * class CSemaphore
 */
class CSemaphore
{
public:
	CSemaphore()
	{
		m_sem = CreateSemaphore(NULL, 0, 3000, NULL);
		m_count = 0;
	}

	~CSemaphore()
	{
		CloseHandle(m_sem);
	}

	inline bool Wait(int timeout = -1)
	{
		int ret;

		if (-1 == timeout)
			ret = WaitForSingleObject(m_sem, INFINITE);
		else
			ret = WaitForSingleObject(m_sem, timeout);

		if (WAIT_OBJECT_0 == ret)
		{
			m_count--;
			return true;
		}
		else
		{
			return false;
		}
	}

	inline void Post()
	{
		ReleaseSemaphore(m_sem, 1, &m_count);
		m_count++;
	}

	inline bool Try()
	{
		return (0 != m_count);
	}

	inline void Clear()
	{
		while (1)
		{
			if (Try()) Wait();
		}
	}

	inline int GetValue()
	{
		return (int)m_count;
	}

private:
	sem_t		m_sem;
	LONG		m_count;
};


#endif	//#ifndef __OS_WIN32_H__
