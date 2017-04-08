#ifndef __LIB_H264_ENC_H__
#define __LIB_H264_ENC_H__


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

enum Preset{
    UltraFast, 
    SuperFast,
    VeryFast,
    Faster,
    Fast
};

enum 
{ 
    CSP_I420,
    CSP_NV12
};


#define H264VENC_Handle void*

typedef struct _H264VENC_Params
{
	signed int		ProfileId;				//! Profile Id, 66 - baseline, 100 - high
	signed int		LevelId;				//! Level Id
	signed int		Transform8x8Flag;		//! Transform 8x8 is enabled or not; 1: enabled, 0: disabled
	signed int		EncQuality;				//! 0: High Qualilty 1: High speed
	signed int		SlicePackMode;			//! Slice packetization mode 0: One slice for a frame; 1: Fixed bytes in a slice; 2: Number of Mbs in a slice
	signed int		OutputFormat;			//! Ouput Nal unit format 0: Output data in Nal stream format; 1: Output data in byte stream format
	signed int		FrameWidth;				//! Frame Width to encode
	signed int		FrameHeight;			//! Frame height to encode
	signed int		SarWidth;				//! X scale for Aspect Ratio. 0: do not use it.
	signed int		SarHeight;				//! Y scale for Aspect Ratio. 0: do not use it.
	signed int		IdrFrameInterval;		//! Idr frame interval
	signed int		InterPartition;			//! 0: only support P16x16, 1: fully support any small inter partition
	signed int		RateControlMode;		//! Rate control mode; 0: VBR, CQP; 1: CBR
	signed int		FrameRate;				//! hugo-2013-02-01 , initial framerate , however dynmaic framerate doesn't work !!!
	signed int		bitrate;				//! hugo-2013-09-09 , initial bitrate , however dynmaic bitrate doesn't work !!!
	signed int      preset;
	signed int      csp;                 ///! colorspace of input data
} H264VENC_Params;

typedef struct _H264VENC_DynamicParams
{
	signed int		FrameRate;				//! Frame rate fps
	signed int		ForceIFrame;			//! Fastupdate flag
	signed int		RcQMax;					//! Max qp [1, 51]
	signed int		RcQMin;					//! Min qp [1, 51]
	signed int		SliceSize;				//! Indicate slice size according to SlicePackMode          
	signed int		Deblock;				//! Whether to enable deblock; 0: disable, 1: enable
	signed int		TargetBitRate;			//! Target bitrate in kbps
} H264VENC_DynamicParams;

typedef struct _YUV_POINTERS
{
	unsigned char *pu8Y;
	unsigned char *pu8U;
	unsigned char *pu8V;
	unsigned int	strideY;
	unsigned int	strideU;
	unsigned int	strideV;
} YUV_POINTERS;


#ifndef MSVC
signed int __attribute__((force_align_arg_pointer)) H264_EncodeFrame
#else	//#ifndef WIN32
signed int H264_EncodeFrame
#endif	//#ifndef WIN32
(
	H264VENC_Handle					handle,
	YUV_POINTERS					*ptCaptureFrame,
	unsigned char					*pu8Bitstream
);

void H264_SetDynamicParams
(
	H264VENC_Handle					handle,
	const H264VENC_DynamicParams	*ptVencDynamicParams
);

H264VENC_Handle H264VENC_Create
(
	const H264VENC_Params			*ptVencParams
);

void H264VENC_Close
(
	H264VENC_Handle					handle
);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif	//#ifndef __LIB_H264_ENC_H__
