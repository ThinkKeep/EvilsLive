/************************************************************************
#      Copyright (C) 2010-2020 VIAZIJING TECHNOLOGY CO., LTD.
#      All rights reserved.
#***********************************************************************/

#ifndef __OS_LINUX_H__
#define __OS_LINUX_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <pthread.h>
#include <semaphore.h>
#include <ctype.h>
#include <fcntl.h>
#include <signal.h>
#include <mntent.h>
#include <dirent.h>
//#include <ifaddrs.h>
#include <netdb.h>
#include <arpa/inet.h>
//#include <uuid/uuid.h>
#include <linux/magic.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
//#include <sys/timeb.h>
#include <sys/vfs.h>
#include <sys/syscall.h>
#include <sys/un.h>
#include <sys/prctl.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <assert.h>

#ifdef __arm__
#ifndef CLOCK_MONOTONIC_RAW
#define CLOCK_MONOTONIC_RAW		CLOCK_MONOTONIC
#endif
#endif

/* add by lixl for android ndk-build */
/* start */
#ifndef S_IREAD
#define S_IREAD S_IRUSR
#endif

#ifndef S_IEXEC
#define S_IEXEC S_IXUSR
#endif

#ifndef S_IWRITE
#define S_IWRITE S_IWUSR
#endif
/* end */
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

typedef pthread_t				thread_t;
typedef pthread_mutex_t			mutex_t;
typedef sem_t					sem_t;


#define O_BINARY				0

#define closesocket				close

#define zjv_open				open
#define zjv_read				read
#define zjv_write				write
#define zjv_lseek				lseek
#define zjv_length				flength
#define zjv_unlink				unlink
#define zjv_rename				rename
#define zjv_flush				fdatasync
#define zjv_close				close


inline int SleepUs(int nMicroseconds)
{
	if (nMicroseconds <= 0)
		return 0;

	struct timeval tv;

	tv.tv_sec  = nMicroseconds / 1000000;
	tv.tv_usec = nMicroseconds % 1000000;

	while (1)
	{
		select(0, NULL, NULL, NULL, &tv);
		if (0 == tv.tv_sec && 0 == tv.tv_usec) break;
	}

	return 0;
}

inline int64 flength(int fd)
{
	if (fd <= 0) return 0;

	int64 posCur = lseek(fd, 0, SEEK_CUR);
	int64 len = lseek(fd, 0, SEEK_END);
	lseek(fd, posCur, SEEK_SET);

	return len;
}

inline int _create_file(const char* path)
{
	int fd = open(path, O_CREAT | O_WRONLY | O_BINARY | O_TRUNC, S_IREAD | S_IWRITE);
	if (-1 == fd) return errno;

	close(fd);

	return 0;
}

inline int _create_path(const char* path)
{
	return mkdir(path, S_IRWXU);
}

inline int _move_path(const char* oldpath, const char* newpath)
{
	return rename(oldpath, newpath);
}

inline int _unlink_path(const char* path)
{
	char cmd[512];

	sprintf(cmd, "rm -rf '%s' >/dev/null 2>&1", path);
	return system(cmd);
}


/*
 * class CMutex
 */
class CMutex
{
public:
	CMutex()
	{
		pthread_mutex_init(&m_mutex, NULL); 
	}

	~CMutex()
	{
		pthread_mutex_destroy(&m_mutex);
	}

	inline void Lock()
	{
		pthread_mutex_lock(&m_mutex);
	}

	inline void Lock(const char*, int)
	{
		pthread_mutex_lock(&m_mutex);
	}

	inline void Unlock()
	{
		pthread_mutex_unlock(&m_mutex);
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
		sem_init(&m_sem, 0, 0);
	}

	~CSemaphore()
	{
		sem_destroy(&m_sem);
	}

	inline bool Wait(int timeout = -1)
	{
		if (timeout <= 0)
		{
			return (0 == sem_wait(&m_sem));
		}
		else
		{
			struct timespec ts;

			clock_gettime(CLOCK_REALTIME, &ts);

			ts.tv_sec  += timeout / 1000;
			ts.tv_nsec += 1000000 * (timeout % 1000);
			if (ts.tv_nsec >= 1000000000)
			{
				ts.tv_nsec -= 1000000000;
				ts.tv_sec  ++;
			}

			return (0 == sem_timedwait(&m_sem, &ts));
		}
	}

	inline void Post()
	{
		sem_post(&m_sem);
	}

	inline bool Try()
	{
		return (0 == sem_trywait(&m_sem));
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
		int val = 0;
		sem_getvalue(&m_sem, &val);
		return val;
	}

private:
	sem_t		m_sem;
};

#endif	//#ifndef __OS_LINUX_H__
