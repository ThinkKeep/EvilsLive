/************************************************************************
#      Copyright (C) 2010-2020 VIAZIJING TECHNOLOGY CO., LTD.
#      All rights reserved.
#***********************************************************************/

#ifndef __LIBRTMP_PUBLISH_DEFS_H__
#define	__LIBRTMP_PUBLISH_DEFS_H__


enum
{
	RTMPP_CMD_CREATE = 0,
	RTMPP_CMD_SETURL,
	RTMPP_CMD_CONNECT,
	RTMPP_CMD_CONNECTSTREAM,
	RTMPP_CMD_CONNECTED,
	RTMPP_CMD_BANDWIDTH,
	RTMPP_CMD_CREATESTREAM,
	RTMPP_CMD_PUBLISH,
	RTMPP_CMD_UNPUBLISH,
	RTMPP_CMD_DELETESTREAM,

	RTMPP_CMD_AAC = 100,
	RTMPP_CMD_AVC,

	RTMPP_CMD_QUIT = 10000,
};

enum
{
	RTMPP_RET_OK = 0,
	RTMPP_RET_ERROR,
};

#endif	//#ifndef __LIBRTMP_PUBLISH_DEFS_H__
