#ifndef __DISK_LIB_H__
#define __DISK_LIB_H__


#include "recordLib.h"

#define  HDD_FILE   "/www/hdd.txt"
#define  HDDSIZE_FILE  "/www/diskSize.txt"


#ifdef __cplusplus
extern "C" {
#endif

#define MAX_SATA_DISK_NUM  16

typedef struct mount_info_t
{//挂载信息
	unsigned char formatFlag;//格式化标记
	unsigned char mountFlag;//挂载标记
	unsigned char unused[2];//
	unsigned char diskUuid[16];//硬盘uuid
	char devName[16];//设备名
	char volumeName[16];//硬盘卷名
	char mountPath[16];//挂在路径
}mount_info_t;

typedef struct disk_info_t
{//硬盘信息
	int diskNum;//硬盘数目
	mount_info_t mountInfo[MAX_SUPPORT_DISK_NUM];
}disk_info_t;

	typedef struct sata_disk_info_t
	{
		int  sataPort;
		char devName[16];
		unsigned char diskUuid[16];
		long long totalSize;

	}sata_disk_info_t;

	typedef struct sata_info_t
	{
		int diskNum;
		sata_disk_info_t sataInfo[MAX_SATA_DISK_NUM];
		
	}sata_info_t;


/* diskLib.c */
/* 获取硬盘信息 */
disk_info_t * diskGetInfo();
/* 硬盘库初始化 */
void diskLibInit();
/* 硬盘库销毁 */
void diskLibDestroy();




/* diskInfo.c */
/* 获取硬盘信息  uuid、卷名、上次挂载路径 */
int getDiskPartitionInfo(char *pDevicePath, unsigned char *pUuid, char *pVolumeName, char *pMountPath);
/* 设置硬盘卷标 */
int setDiskVolumeName(char *pDevicePath, char *pName);
/* 检测硬盘分区是否已经挂载 */
int checkDiskPartitionIsMount(char *pDevName, char *pMountPath);
/* 获取已经挂载Usb路径 */
int getUsbMountPath(char *pPath);
/*扫描硬盘 */
int findSataDisk(sata_info_t *pSataInfo );
/*挂载硬盘*/
int mountSataDisk(unsigned int diskId);
/*格式化硬盘*/
int FormatDisk( unsigned int diskId );

int mountAllSataDisk();

int matchSataPort( const char *p );

//       /dev/sda
int GetDiskFreeSize( const char *pdevPath);


/* fallocate.c */
/* 获取可预分配的文件最大数目 */
int getFileTotalNum(char *pDevicePath, unsigned int fileSize);
/* 检测预分配文件 */
int checkFallocateFile(char *pDiskPath, unsigned int fileSize);
/* 创建预分配文件 */
int createFallocateFile(char *pFileName, int fileSize);


/* checkUsb.c */
int startHotPlugThread();
int pox_system(const char *cmd_line);
int iCheckSystemReturnValue(int p_iStatus);
#ifdef __cplusplus
}
#endif

#endif//__DISK_LIB_H__


