#include <stdlib.h>
#include <errno.h>
#include <sys/mount.h>

#include "diskLib.h"
#include "configLib.h"


#define DISK_VOLUME_NAME		"HEROSPEED"


static disk_info_t g_diskInfo;


int mountAllSataDisk()
{

	char devPath[64] = {0};
	char volumeName[64] = {0};
	char mountPath[64] = {0};
	unsigned char uuid[64] = {0};

	memset(devPath, 0, sizeof(devPath));
	memset(mountPath, 0, sizeof(mountPath));
	memset(uuid, 0, sizeof(uuid));
	
	sata_info_t pSataInfo;
	memset(&pSataInfo, 0, sizeof(pSataInfo));
	findSataDisk( &pSataInfo );

	//fprintf(stderr, "=====mountSataDisk  diskNum %d \n", pSataInfo.diskNum);
	if(pSataInfo.diskNum > 0)
	{
		gpioSetDiskPower(1);
		gpioSetHdd(1);
		gpioSetBeep(0);
	}
	else
	{
		gpioSetDiskPower(0);
		gpioSetHdd(0);
		gpioSetBeep(0);
	}

	int diskIndex = 0;

	int i = 0;
	for ( i = 0; i < pSataInfo.diskNum; i++) {
		fprintf(stderr, "sataPort %d devName %s size %llu G \n", 
		pSataInfo.sataInfo[i].sataPort, pSataInfo.sataInfo[i].devName, 
		pSataInfo.sataInfo[i].totalSize / 1000 / 1000 / 1000);

		int diskId = pSataInfo.sataInfo[i].sataPort ;
		
		sprintf(devPath, "%s1", pSataInfo.sataInfo[i].devName);
		sprintf(mountPath, "/disk/disk%04X", diskId - 1);
		mkdir(mountPath, S_IRWXU);
			
		if (mount(devPath, mountPath, "ext4", MS_NOATIME | MS_NODIRATIME, NULL) != 0) {
			//return 0;
			//fprintf(stderr, "======== devPath %s mount failed \n", devPath);

		//	continue;
		}

		if (getDiskPartitionInfo(devPath, uuid, volumeName, mountPath) == 0) {

		}

		if (checkFallocateFile(mountPath, MAX_FILE_SIZE) == 0) {
				g_diskInfo.mountInfo[diskId - 1].formatFlag = 1;
				g_diskInfo.mountInfo[diskId - 1].mountFlag = 1;
				memcpy(g_diskInfo.mountInfo[diskId - 1].diskUuid, uuid, 16);
				snprintf(g_diskInfo.mountInfo[diskId - 1].devName, 16,  "%s1", pSataInfo.sataInfo[i].devName+5);
				strncpy(g_diskInfo.mountInfo[diskId - 1].volumeName, volumeName, 16);	

				//fprintf(stderr, "==========use devName %s \n", pSataInfo.sataInfo[i].devName);

				diskIndex++;

				if ( diskId - 1 >= configGetSataNum() + configGetEsataNum()) {
					return 1;
				}
						
		} else {
			umount(devPath);
			umount(devPath);
		}
		usleep(100);
	}

	return 0;

}


int mountAllDiskPartition()
{
	return mountAllSataDisk();
}

/* 获取硬盘信息 */
disk_info_t * diskGetInfo()
{
	mountAllDiskPartition();

	return &g_diskInfo;
}

/* 硬盘库初始化 */
void diskLibInit()
{
	memset(&g_diskInfo, 0, sizeof(g_diskInfo));
	mountAllDiskPartition();
	sleep(1);
	startHotPlugThread();
}

/* 硬盘库销毁 */
void diskLibDestroy()
{

}

