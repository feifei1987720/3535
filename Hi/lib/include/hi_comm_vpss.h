/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_vpss.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 
  Last Modified :
  Description   : common struct definition for vpss
  Function List :
  History       :
  1.Date        : 20130508
    Author      : l00183122
    Modification: Create
  
 
******************************************************************************/

#ifndef __HI_COMM_VPSS_H__
#define __HI_COMM_VPSS_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "hi_type.h"
#include "hi_common.h"
#include "hi_errno.h"
#include "hi_comm_video.h"

#define HI_ERR_VPSS_NULL_PTR        HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define HI_ERR_VPSS_NOTREADY        HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define HI_ERR_VPSS_INVALID_DEVID   HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
#define HI_ERR_VPSS_INVALID_CHNID   HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
#define HI_ERR_VPSS_EXIST           HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
#define HI_ERR_VPSS_UNEXIST         HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
#define HI_ERR_VPSS_NOT_SUPPORT     HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define HI_ERR_VPSS_NOT_PERM        HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define HI_ERR_VPSS_NOMEM           HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define HI_ERR_VPSS_NOBUF           HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
#define HI_ERR_VPSS_ILLEGAL_PARAM   HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define HI_ERR_VPSS_BUSY            HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define HI_ERR_VPSS_BUF_EMPTY       HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)


typedef   HI_S32 VPSS_GRP;
typedef   HI_S32 VPSS_CHN;

/*Define 4 video frame*/
typedef enum hiVPSS_FRAME_WORK_E
{
    VPSS_FRAME_WORK_LEFT     =  0,
    VPSS_FRAME_WORK_RIGHT    =  1,
    VPSS_FRAME_WORK_BOTTOM   =  2,
    VPSS_FRAME_WORK_TOP      =  3,
    VPSS_FRAME_WORK_BUTT
}VPSS_FRAME_WORK_E;

/*Define de-interlace mode*/    
typedef enum  hiVPSS_DIE_MODE_E
{
    VPSS_DIE_MODE_AUTO      = 0,
    VPSS_DIE_MODE_NODIE     = 1,
    VPSS_DIE_MODE_DIE       = 2,
    VPSS_DIE_MODE_BUTT
}VPSS_DIE_MODE_E;

/*Define attributes of video frame*/
#if 0
typedef struct HI_VPSS_FRAME_S
{
    HI_U32  u32Width[VPSS_FRAME_WORK_BUTT]; /*Width of 4 frames,0:L,1:R,2:B,3:T*/
    HI_U32  u32Color; /*Color of 4 frames must be the same,R/G/B*/
}BORDER_S;
#endif

/*Define attributes of vpss channel*/
typedef struct hiVPSS_CHN_ATTR_S
{
    HI_BOOL bSpEn;    /*Sharpen enable*/         
    HI_BOOL bBorderEn; /*Frame enable*/  
    BORDER_S  stBorder;     
}VPSS_CHN_ATTR_S;

typedef struct hiVPSS_TFY_S 
{
    HI_S32 s32Range;
    HI_S32 s32Rate;
}VPSS_TFY_S;

typedef struct hiVPSS_SFY_S
{
  HI_S32 s32Sharp;
  HI_S32 s32DePepp;
  HI_S32 s32DeSalt;
  HI_S32 s32JMode;
  HI_S32 s32Profile;
  HI_S32 s32JSlope;
  HI_S32 s32ISlope;
}VPSS_SFY_S;

typedef struct hiVPSS_NRC_S 
{
    HI_S32 s32NRc;
    HI_S32 s32SFc;
    HI_S32 s32TFc;
    HI_S32 s32TFp;
}VPSS_NRC_S;

typedef struct  hiVPSS_NRY_S
{
  VPSS_SFY_S  astSFy[2];
  VPSS_TFY_S  astTFy[2];

  VPSS_NRC_S  astNRc;  
}VPSS_NRY_S;

/*Defined detailed image quality debug param*/
typedef struct hiVPSS_IMG_QUALITY_PARAM_S
{
    VPSS_NRY_S stNRY;
    HI_U32 u32Reserved[6];
}VPSS_IMG_QUALITY_PARAM_S;

/*Defined private image quality debug config struct*/
typedef struct hiVPSS_IMG_QUALITY_CFG_S
{
    HI_BOOL bEnable;
    VPSS_IMG_QUALITY_PARAM_S  stImageQualityParam;
}VPSS_IMG_QUALITY_CFG_S;

typedef struct hiVPSS_GRP_PARAM_S
{
    HI_U32 u32Contrast;     /*strength of dymanic contrast improve*/
    HI_U32 u32IeStrength;   /*strength of image enhance*/
    HI_U32 u32DieStrength;  /*strength of de-interlace*/
    HI_U32 u32SfStrength;   /*strength of space filter*/
    HI_U32 u32TfStrength;   /*strength of time filter*/
    HI_U32 u32CfStrength;   /*strength of chroma filter*/
    HI_U32 u32DeMotionBlurring;  /*strength of de motion blurring*/
}VPSS_GRP_PARAM_S;

/* Define image feild select mode */
typedef enum hiVPSS_CAPSEL_E
{
    VPSS_CAPSEL_BOTH = 0,             /* top and bottom field */
    VPSS_CAPSEL_TOP,                  /* top field */
    VPSS_CAPSEL_BOTTOM,               /* bottom field */
    
    VPSS_CAPSEL_BUTT
} VPSS_CAPSEL_E;
/*Define coordinate mode*/
typedef enum hiVPSS_CROP_COORDINATE_E   
{
    VPSS_CROP_RATIO_COOR = 0,   /*Ratio coordinate*/
    VPSS_CROP_ABS_COOR          /*Absolute coordinate*/
}VPSS_CROP_COORDINATE_E;

/*Define attributes of CLIP function*/
typedef struct hiVPSS_CROP_INFO_S
{
    HI_BOOL bEnable;        /*CROP enable*/
    VPSS_CROP_COORDINATE_E  enCropCoordinate;   /*Coordinate mode of the crop start point*/
    RECT_S  stCropRect;     /*CROP rectangular*/
}VPSS_CROP_INFO_S;

/*Define attributes of vpss GROUP*/
typedef struct hiVPSS_GRP_ATTR_S
{
    /*statistic attributes*/
    HI_U32  u32MaxW;  /*MAX width of the group*/                    
    HI_U32  u32MaxH;  /*MAX height of the group*/
    PIXEL_FORMAT_E enPixFmt; /*Pixel format*/
    
    HI_BOOL bIeEn;    /*Image enhance enable*/
    HI_BOOL bDciEn;   /*Dynamic contrast Improve enable*/
    HI_BOOL bNrEn;    /*Noise reduce enable*/
    HI_BOOL bHistEn;  /*Hist enable*/
    VPSS_DIE_MODE_E enDieMode; /*De-interlace enable*/
}VPSS_GRP_ATTR_S;

/*Define vpss channel's work mode*/
typedef enum hiVPSS_CHN_MODE_E   
{
    VPSS_CHN_MODE_AUTO = 0, /*Auto mode*/
    VPSS_CHN_MODE_USER  /*User mode*/
}VPSS_CHN_MODE_E;

/*Define attributes of vpss channel's work mode*/
typedef struct hiVPSS_CHN_MODE_S
{
    VPSS_CHN_MODE_E  enChnMode;   /*Vpss channel's work mode*/
    HI_U32 u32Width;              /*Width of target image*/
    HI_U32 u32Height;             /*Height of target image*/
    HI_BOOL bDouble;              /*Field-frame transferonly valid for VPSS_PRE0_CHN*/
    PIXEL_FORMAT_E  enPixelFormat;/*Pixel format of target image*/
    COMPRESS_MODE_E enCompressMode;   /*Compression mode of the output*/

}VPSS_CHN_MODE_S;

typedef struct hiVPSS_FRAME_TIMEOUT_S
{
    VIDEO_FRAME_INFO_S stVideoFrame;
    HI_S32 s32MilliSec;
}VPSS_FRAME_TIMEOUT_S;

typedef struct hiVPSS_GET_GRP_FRAME_S
{
    HI_U32 u32FrameIndex;   /*reserved*/
    VIDEO_FRAME_INFO_S *pstVideoFrame;
}VPSS_GET_GRP_FRAME_S;

/*Define detailed NR params for channel image process*/
typedef struct hiVPSS_NR_PARAM_S
{
    HI_U32 u32SfStrength;
    HI_U32 u32TfStrength;
    HI_U32 u32CfStrength;
    HI_U32 u32DeMotionBlurring;  
}VPSS_NR_PARAM_S;

/*Define detailed params for channel image process*/
typedef struct hiVPSS_CHN_PARAM_S
{
    HI_U32 u32SpStrength;
} VPSS_CHN_PARAM_S;

/*Define vpss prescale info*/
typedef struct hiVPSS_PRESCALE_INFO_S
{
    HI_BOOL bPreScale;       /*prescale enable*/
    SIZE_S  stDestSize;      /*destination size*/     
}VPSS_PRESCALE_INFO_S;

/*Define vpss filter info*/
typedef struct hiVPSS_SIZER_INFO_S
{
    HI_BOOL bSizer;
    SIZE_S  stSize;
}VPSS_SIZER_INFO_S;

/*Define vpss frame control info*/
typedef struct hiVPSS_FRAME_RATE_S
{
    HI_S32  s32SrcFrmRate;        /* Input frame rate of a  group*/
    HI_S32  s32DstFrmRate;        /* Output frame rate of a channel group */
} VPSS_FRAME_RATE_S;

/*Define attributes of vpss extend channel*/
typedef struct hiVPSS_EXT_CHN_ATTR_S
{
    VPSS_CHN        s32BindChn;             /*channel bind to*/
    HI_U32          u32Width;               /*Width of target image*/
    HI_U32          u32Height;              /*Height of target image*/
    HI_S32          s32SrcFrameRate;        /*Frame rate of source*/
    HI_S32          s32DstFrameRate;        /*Frame rate of extend chn input&output*/
    PIXEL_FORMAT_E  enPixelFormat;          /*Pixel format of target image*/
}VPSS_EXT_CHN_ATTR_S;

typedef struct hiVPSS_REGION_INFO_S
{
    RECT_S *pstRegion;    /*region attribute*/
    HI_U32 u32RegionNum;       /*count of the region*/
}VPSS_REGION_INFO_S;

typedef struct hiVPSS_GET_REGION_LUMA_S
{
    VPSS_REGION_INFO_S stRegionInfo;  /*Information of the region*/
    HI_U32* pu32LumaData;             /*Luma data of the region*/
    HI_S32 s32MilliSec;               /*time parameter.less than 0 means waiting until get the luma data,
                                             equal to 0 means get the luma data no matter whether it can or not,
                                             more than 0 means waiting how long the time parameter it is*/
}VPSS_GET_REGION_LUMA_S;

/*region attach  mode*/    
typedef enum  hiVPSS_REGION_ATTACH_MODE_E
{    
    REGION_ATTACH_TO_GROUP      = 0,        /**/   
    REGION_ATTACH_TO_CHN        = 1,    
    REGION_ATTACH_MODE_BUTT
}VPSS_REGION_ATTACH_MODE_E;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* __HI_COMM_VPSS_H__ */


