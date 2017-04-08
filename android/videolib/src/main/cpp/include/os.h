/************************************************************************
#      Copyright (C) 2010-2020 VIAZIJING TECHNOLOGY CO., LTD.
#      All rights reserved.
#***********************************************************************/

#ifndef __OS_H__
#define __OS_H__


#ifdef WIN32
#include "os-win32.h"
#else	//#ifdef WIN32
#include "os-linux.h"
#endif	//#ifdef WIN32

#endif	//#ifndef __OS_H__
