#ifndef __RSMedia_h__
#define __RSMedia_h__


#ifndef _WIN32
typedef struct _GUID {
	unsigned long  Data1;
	unsigned short Data2;
	unsigned short Data3;
	unsigned char  Data4[ 8 ];
} GUID;

typedef unsigned long long ULONGLONG;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int UINT;


#endif


//////////////////////////////////////////////////////////////////////////
//RSFrame

//RSVFrameHead.System
#define RSSystemPAL		'P'
#define RSSystemNTSC	'N'

typedef struct
{
	WORD Width;
	WORD Height;
	BYTE FrameRate;
	BYTE System;
	WORD Reserved;
}RSVideoHead;

typedef struct
{
	UINT SampleRate;
	BYTE Channels;
	BYTE BitsPerSample;
	WORD PackLen;
}RSAudioHead;

//RSFrameHead.FrameTag
#define RSFrameTag	*(UINT*)"RSFm"

//RSFrameHead.FrameType
#define RSIFrame	'I'
#define RSPFrame	'P'
#define RSAFrame	'A'
#define RSEndFrame   'E'
#define RSReposFrame 'R'

//RSFrameHead.EncodeType
//Video
#define RSVideoEncodeH264	0x01
//Audio
#define RSAudioEncodeG711A	0x01

//use ffmpeg to decode
#define RSVideoH264FFMPEG   0x02

typedef struct
{
	UINT FrameTag;

	BYTE FrameType;
	BYTE EncodeType;
	BYTE Channel:7;
	BYTE Encrypted:1; //Encrypt Flag. 0:no Encrypt; 1:Encrypted.
	BYTE Reserved;

	UINT FrameNo;
	UINT FrameSize;
	ULONGLONG Time; //(microseconds) Time/1000,000 = the number of seconds after January 1, 1970 UTC
	ULONGLONG Pts;

	union
	{
		RSVideoHead VideoHead;
		RSAudioHead AudioHead;
	};
}RSFrameHead;


//////////////////////////////////////////////////////////////////////////
//RSInfo
#define RSPuInfoTag *(unsigned int*)"RSI"
#define RSPuInfoTypeGPS 'G'
#define RSPuInfoTypeRepos 'R'
#define RSPuInfoTypeEnd 'E'

typedef struct __PuInfoHead_t
{
	unsigned int Tag : 24;
	unsigned int Type : 8;
	unsigned int InfoSize;
	unsigned long long Time; //(microseconds) Time/1000,000 = the number of seconds after January 1, 1970 UTC
}PuInfoHead_t;

//pu_info_head_t.type = RSPuInfoTypeGPS, pu_info_head_t.info_size = sizeof(gps_info_t)
typedef struct __GpsInfo_t
{
	double Longitude;
	double Latitude;
	float Speed;
	float SensorSpeed;          //
	float Azimuth;    			// 0 0~360
	int Signal3G;               //3G
	unsigned int TotalMileage;  //  
	int Temperature;
	char LatitudeDir;       	//  'N' or 'S'
	char LongitudeDir;      	//  'E' or 'W'
	char Rectified;             // 
	char Status;   				//, 'A' , 'V'3
	unsigned char IgnitionFlag; //1:0
	char Reserved[3];
	double LongitudeOffset;
	double LatitudeOffset;
}GpsInfo_t;


//GPS
typedef struct
{
	PuInfoHead_t Head;
	GpsInfo_t Body;
}RSGpsInfoPacket_t;


//////////////////////////////////////////////////////////////////////////
//RSFile

//File content outline
/*
	RSFileHead
	RSFileHeadInfo(extensible)

	Frame ... Frame

	//TailInfo

	RSFileListHead
	(optional list head info)
	RSFileFrameIndex(extensible) ... RSFileFrameIndex(extensible)

	(extensible)
*/

#define RSFileDataAlign	8 //File data alignment

#ifndef C_ASSERT
#define C_ASSERT(e) typedef char __C_ASSERT__[(e)?1:-1]
#endif

#define CheckFileDataAlign(type) C_ASSERT(sizeof(type) % RSFileDataAlign == 0)

//RSFileHead.Tag
#define RSFileTag	*(ULONGLONG*)"ALRSFile" //Identify ALRS media file

//RSFileHead.Status
#define RSFileFull			*(UINT*)"Full"
#define RSFileWritingData	*(UINT*)"Data"


typedef struct
{
	ULONGLONG Tag; //RSFileTag
	UINT HeadInfoSize; //sizeof(RSFileHeadInfo)
	UINT Status; //RSFileFull ...
	ULONGLONG TailInfoOffset; //Tail info offset from the beginning of the file
	GUID Guid; //Unique identifier for this file
}RSFileHead;
CheckFileDataAlign(RSFileHead);

typedef struct
{
	ULONGLONG BeginTime; //Equal to RSFrameHead.Time
	ULONGLONG EndTime; //Equal to RSFrameHead.Time
}RSFileHeadInfo;
CheckFileDataAlign(RSFileHeadInfo);

#define RSFileListTagPrefix			"List"
#define RSFileListTagPrefixValue	*(UINT*)RSFileListTagPrefix
#define RSFileIndexListTag		*(ULONGLONG*)RSFileListTagPrefix##"Idx"
#define RSFileIndexListTagEx	*(ULONGLONG*)"ListIdx"

typedef struct
{
	ULONGLONG Tag; //RSFileIndexListTag ...
	WORD ListHeadInfoSize; //Extra list head info size, 0 for RSFileIndexListTag
	WORD ItemSize; //One subsequent item size, sizeof(RSFileFrameIndex) for RSFileIndexListTag
	UINT ListSize; //Extra list head info size plus all subsequent item size
}RSFileListHead;
CheckFileDataAlign(RSFileListHead);

typedef struct
{
	ULONGLONG Time; //Equal to RSFrameHead.Time
	ULONGLONG Offset; //Frame offset from the beginning of the file
}RSFileFrameIndex;
CheckFileDataAlign(RSFileFrameIndex);

#endif
