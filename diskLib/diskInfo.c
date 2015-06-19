#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/statfs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <fcntl.h>

#include "diskLib.h"
#include "global.h"
#include "configLib.h"
#include "gpioLib.h"
#include "commonLib.h"


/* 获取硬盘信息  uuid、卷名、上次挂载路径 */
int getDiskPartitionInfo(char *pDevicePath, unsigned char *pUuid, char *pVolumeName, char *pMountPath)
{
	int n = 0;
	int fd = 0;
	char buf[1024] = {0};
	char *pBuf = buf;

	if(NULL == pDevicePath)
	{
		return -1;
	}

	fd = open(pDevicePath, O_RDONLY);
	if(fd < 0)
	{
		perror("get disk partition info error");
		return -1;
	}

	lseek(fd, 1024, SEEK_SET);
	n = read(fd, buf, sizeof(buf));
	if(n !=  sizeof(buf))
	{
		perror("read super block");
	}
	else
	{

       // fprintf(stderr,"&&&&&&&&&&&&&&&&&%s:%d: buff == %s\r\n", __func__,__LINE__,buf);
		pBuf = &buf[0x68];
       // fprintf(stderr,"uuid %02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x\r\n", pBuf[0]&0xff, pBuf[1]&0xff,
	//			pBuf[2]&0xff, pBuf[3]&0xff, pBuf[4]&0xff, pBuf[5]&0xff, pBuf[6]&0xff, pBuf[7]&0xff, pBuf[8]&0xff, pBuf[9]&0xff, pBuf[10]&0xff, pBuf[11]&0xff, pBuf[12]&0xff, pBuf[13]&0xff, pBuf[14]&0xff, pBuf[15]&0xff);
		if(pUuid != NULL)
		{
			memcpy(pUuid, pBuf, 16);
		}

		pBuf = &buf[0x78];
		strncpy(pVolumeName, pBuf, 16);
       // fprintf(stderr,"@@@@@@@@@@@@@@@@%s:%d disk name %s\r\n", __func__,__LINE__,pBuf);

		pBuf = &buf[0x88];
	//	strncpy(pMountPath, pBuf, 16);
      //  fprintf(stderr,"disk last mount path %s\r\n", pBuf);
	}

	close(fd);
	fd = 0;

	return 0;
}

/* 设置硬盘卷标 */
int setDiskVolumeName(char *pDevicePath, char *pName)
{
	int fd = 0;

	if(NULL == pDevicePath)
	{
		return -1;
	}

	fd = open(pDevicePath, O_RDWR);
	if(fd < 0)
	{
		return -1;
	}

	lseek(fd, 1024 + 0x78, SEEK_SET);
	write(fd, pName, 8);

	close(fd);
	fd = 0;

	return 0;
}

/* 检测硬盘分区是否已经挂载 */
int checkDiskPartitionIsMount(char *pDevName, char *pMountPath)
{
	FILE *fp = 0;
	char buf[1024] = {0};
	int n = 0;
	char devName[64] = {0};//设备名
	char mountName[64] = {0};//挂载名
	char mountType[32] = {0};//挂载类型
	char power[256] = {0};//权限
	int tmp1 = 0;
	int tmp2 = 0;
	int findFlag = 0;

	fp = fopen("/proc/mounts", "r");
	if(fp == NULL)
	{
		printf("cannot open partitions\r\n");
		return -1;
	}

	while(1)
	{
		if(fgets(buf, sizeof(buf), fp) == NULL)
		{
			break;
		}

		n = sscanf(buf, "%s %s %s %s %d %d", devName, mountName, mountType, power, &tmp1, &tmp2);
		if(n != 6)
		{
			continue;
		}

		if(strstr(devName, pDevName) != NULL)
		{//分区已经挂载
			findFlag = 0x01;
		}

		if(pMountPath != NULL)
		{//挂载目录已经被使用
		    if(strstr(mountName, pMountPath) != NULL)
		    {
			findFlag = 0x02;
		    }
		}

		if((strstr(devName, pDevName) != NULL) && (pMountPath != NULL) && (strstr(mountName, pMountPath) != NULL))
		{//分区已经挂载到该目录
		    	findFlag = 0x03;
		}
	}

	fclose(fp);
	fp = NULL;

	return findFlag;
}

/* 获取已经挂载Usb路径 */
int getUsbMountPath(char *pPath)
{
	FILE *fp = 0;
	char buf[1024] = {0};
	int n = 0;
	char devName[64] = {0};//设备名
	char mountName[64] = {0};//挂载名
	char mountType[32] = {0};//挂载类型
	char power[256] = {0};//权限
	int tmp1 = 0;
	int tmp2 = 0;
	int findFlag = 0;

	fp = fopen("/proc/mounts", "r");
	if(fp == NULL)
	{
		fprintf(stderr,"cannot open partitions\r\n");
		return -1;
	}

	while(1)
	{
		if(fgets(buf, sizeof(buf), fp) == NULL)
		{
			break;
		}

		n = sscanf(buf, "%s %s %s %s %d %d", devName, mountName, mountType, power, &tmp1, &tmp2);
		if(n != 6)
		{
			continue;
		}

		if(strstr(mountName, "/usb/usb") != NULL)
		{//分区已经挂载到该目录
			//fprintf(stderr,"mountName ================= (%s)\r\n", mountName);
			strcpy(pPath, mountName);
			findFlag = 1;
			break;
		}
	}

	fclose(fp);
	fp = NULL;

	return findFlag;
}

int matchSataPort( const char *p )
{
	int sataPort = 0;

	if(configGetCpuType() == CPU_3531)
	{
		if(configGetSataNum() == 2)
		{//2盘位
			if (strncmp(p, "1:0:0:0",  7) == 0 )
			{    
				sataPort = 1;
				Printf("sata 6 \n");
			}
			else if (strncmp(p, "0:1:0:0",  7) == 0 )
			{    
				sataPort = 2;
				Printf("sata 9 \n");
			}
			else  if (strncmp(p, "0:0:0:0", 7) == 0 )
			{//esata
				sataPort = 3;
				Printf("sata 2 \n");
			}
		}
		else
		{
			if (strncmp(p, "0:2:0:0",  7) == 0 ) {
				sataPort = 1;
				Printf("sata 1 \n");
			} else  if (strncmp(p, "0:0:0:0", 7) == 0 ) {
				sataPort = 2;
				Printf("sata 2 \n");
			} else if (strncmp(p, "0:3:0:0",  7) == 0 ) {		
				sataPort = 3;	
				Printf("sata 3 \n");	
			} else if (strncmp(p, "0:4:0:0",  7) == 0 ) {
				sataPort = 4;
				Printf("sata 4 \n");
			} else  if (strncmp(p, "1:2:0:0", 7) == 0 ) {
				sataPort = 5;
				Printf("sata 5 \n");
			} else if (strncmp(p, "1:0:0:0",  7) == 0 ) {		
				sataPort = 6;	
				Printf("sata 6 \n");	
			} else  if (strncmp(p, "1:3:0:0", 7) == 0 ) {
				sataPort = 7;
				Printf("sata 7 \n");
			} else if (strncmp(p, "1:4:0:0",  7) == 0 ) {		
				sataPort = 8;	
				Printf("sata 8 \n");	
			} else if (strncmp(p, "0:1:0:0",  7) == 0 ) {		
				sataPort = 9;	
				Printf("sata 9 \n");	
			} 
		}
	}
	else if((configGetCpuType() == CPU_3520A) || (configGetCpuType() == CPU_3521)|| (configGetCpuType() == CPU_3520D))
	{
		if (strncmp(p, "0:0:0:0",  7) == 0 )
		{
			sataPort = 1;	
			Printf("sata 1 \n");	
		}
		else  if (strncmp(p, "1:0:0:0", 7) == 0 )
		{
			sataPort = 2;
			Printf("sata 2\n ");
		}
	}
	else if(configGetCpuType()==CPU_3535)
	{
		if(configGetSataNum() == 2)
		{//2
			if (strncmp(p, "1:0:0:0",  7) == 0 )
			{    
				sataPort = 1;
				Printf("sata 1 \n");
			}
			else if (strncmp(p, "0:1:0:0",  7) == 0 )
			{    
				sataPort = 3;
				Printf("sata 3 \n");
			}
			else  if (strncmp(p, "0:0:0:0", 7) == 0 )
			{//esata
				sataPort = 2;
				Printf("sata 2 \n");
			}
		}
		else
		{
			if (strncmp(p, "1:1:0:0",  7) == 0 ) {
				sataPort = 1;
				Printf("sata 1 \n");
			} else  if (strncmp(p, "1:2:0:0", 7) == 0 ) {
				sataPort = 2;
				Printf("sata 2 \n");
			} else if (strncmp(p, "1:0:0:0",  7) == 0 ) {		
				sataPort = 3;	
				Printf("sata 3 \n");	
			} else if (strncmp(p, "1:3:0:0",  7) == 0 ) {
				sataPort = 4;
				Printf("sata 4 \n");
			} else  if (strncmp(p, "0:2:0:0", 7) == 0 ) {
				sataPort = 5;
				Printf("sata 5 \n");
			} else if (strncmp(p, "0:0:0:0",  7) == 0 ) {		
				sataPort = 6;	
				Printf("sata 6 \n");	
			} else  if (strncmp(p, "0:1:0:0", 7) == 0 ) {
				sataPort = 7;
				Printf("sata 7 \n");
			} else if (strncmp(p, "0:3:0:0",  7) == 0 ) {		
				sataPort = 8;	
				Printf("sata 8 \n");	
			} else if (strncmp(p, "0:1:0:0",  7) == 0 ) {		
				//sataPort = 9;	
				Printf("sata 9 \n");	
			} 
		}
	}
	else
	{
		if (strncmp(p, "1:0:0:0",  7) == 0 ) {		
			sataPort = 1;	
			Printf("sata 1\n ");	
		} else  if (strncmp(p, "1:3:0:0", 7) == 0 ) {
			sataPort = 2;
			Printf("sata 2 \n");
		} else if (strncmp(p, "1:4:0:0",  7) == 0 ) {		
			sataPort = 3;	
			Printf("sata 3 \n");	
		} else if (strncmp(p, "0:1:0:0",  7) == 0 ) {		
			sataPort = 4;	
			Printf("sata 4 \n");	
		}
	}

	// usb port

	if (strncmp(p, "1-1:1.0",  3) == 0 ) {
		sataPort = 10;
		Printf("usb 10 \n");
	} else if ( strncmp(p, "1-2:1.0",  3) == 0 ) {
		sataPort = 11;
		Printf("usb 11 \n");
	} else if ( strncmp(p, "3-1:1.0",  3) == 0 ) {
		sataPort = 12;
		Printf("usb 12 \n");
	}

	return sataPort;

}

int GetDiskFreeSize( const char *pdevPath)
{
	int i = 0;

	char devPath[32] = { 0 };
	char mountPath[32] = { 0 };
	char cmd[128];

	for (i = 0; i < 8; i++)
	{
		sprintf(devPath, "%s", pdevPath);
		sprintf(mountPath, "/usb/usb%04d", i);

		Printf(" devPath %s \n", devPath);

		int ret = checkDiskPartitionIsMount(devPath, mountPath);
		if(ret == 0x00)
		{
			sprintf(cmd, "mount %s%d %s", devPath, i+1, mountPath);
			system(cmd);
			continue;
		}
		else if(ret == 0x03)
		{
			break;
		}
	}

	long long freespace = 0;     
	struct statfs disk_statfs; 

	//fprintf(stderr, "=========== mountPath %s \n", mountPath);

	if ( statfs(mountPath, &disk_statfs) >= 0 ) 
	{ 
		freespace = (((long long)disk_statfs.f_bsize  * (long long)disk_statfs.f_bfree)/(long long)(1024*1024)); 		
	}   

	//fprintf(stderr, "=========== mountPath %s freespace %lld\n", mountPath, freespace);

	return freespace;	

}

void sortSataByPort(sata_info_t *pSataInfo)
{

	int i = 0;
	int j = 0;	
	int exchanged = 1;
	sata_disk_info_t tmp;

	for ( i = pSataInfo->diskNum; i > 0 && exchanged; i-- )
	{
		exchanged = 0;

		for (j = 0; j < i - 1; j++) {
            if ( pSataInfo->sataInfo[j].sataPort > pSataInfo->sataInfo[j+1].sataPort )
            {
				memcpy(&tmp, &pSataInfo->sataInfo[j], sizeof(sata_disk_info_t));
				memcpy(&pSataInfo->sataInfo[j], &pSataInfo->sataInfo[j+1], sizeof(sata_disk_info_t));
				memcpy(&pSataInfo->sataInfo[j+1], &tmp, sizeof(sata_disk_info_t));				
				exchanged = 1;

				//fprintf(stderr, "========= j %d sataPort %d \n", j, pSataInfo->sataInfo[j].sataPort);
			}
		}
	}
}

int findSataDisk(sata_info_t *pSataInfo )
{


    pox_system("/app/bin/findsata.sh ");

	int n = 0;

	FILE *fp = 0;
	char buf[1024] = {0};
	char scsi[16] = { 0 };
	char Label[64] = { 0 };

	fp = fopen("/var/hdd.txt", "r");

	if (fp == NULL)	{
		fprintf(stderr, "%s", "cannot open /app/hdd.txt\r\n");
		return 0;
	}

	while ( pSataInfo->diskNum < MAX_SATA_DISK_NUM )
	{
		if (fgets(buf, sizeof(buf), fp) == NULL) {
			break;
		}

		n = sscanf(buf, "%s %s", scsi, Label);
		if (n != 2) {
			continue;
		}	

		sprintf(pSataInfo->sataInfo[pSataInfo->diskNum].devName, "/dev/%s", Label);
		pSataInfo->sataInfo[pSataInfo->diskNum].sataPort = matchSataPort(scsi);
		pSataInfo->diskNum++;
		
		fprintf(stderr, "disk %s \n", Label);
	}

	fclose(fp);
	fp = NULL;	

	// get disk size
	fp = fopen("/var/diskSize.txt", "r");
	if (fp == NULL)	{
		fprintf(stderr,"cannot open /app/diskSize.txt\r\n");
		return 0;
	}

	char devName[16] = { 0 };
	long long diskSize = 0;
	
	while ( 1 )
	{
		if (fgets(buf, sizeof(buf), fp) == NULL) {
			break;
		}
		
		n = sscanf(buf, "%s %llu", devName, &diskSize);
		if (n != 2) {
			continue;
		}		

		//Printf("devName %s diskSize %llu G \n", devName, diskSize / 1000 / 1000/ 1000);		
		
		int index = 0;
		
		for ( index = 0; index < pSataInfo->diskNum; index++ ) {
			if ( 0 == strcmp(pSataInfo->sataInfo[index].devName, devName ) ) {
				pSataInfo->sataInfo[index].totalSize = diskSize;
			}
		}
	}

	fclose(fp);
	fp = NULL;

	sortSataByPort(pSataInfo);

	return pSataInfo->diskNum;

}

int FormatDisk(unsigned int diskId )
{
	fprintf(stderr,"format disk %d \n", diskId);

	sata_info_t pSataInfo = { 0 };
	char cmd[256] = { 0 };
	
	char devPath[256] = { 0 };

	findSataDisk( &pSataInfo );
	int i = 0, j = 0;

	sync();
//	sleep(1);
	printf("55555555555555555555555555\n");
	for ( i = 0; i < pSataInfo.diskNum; i++) {

		printf("sataPort %d devName %s size %llu G \n",
			pSataInfo.sataInfo[i].sataPort, pSataInfo.sataInfo[i].devName, 
			pSataInfo.sataInfo[i].totalSize / 1000 / 1000 / 1000);

		if ( diskId == pSataInfo.sataInfo[i].sataPort && diskId < 10 )	{

			for ( j = 1; j < 8; j++ ) {	
				sprintf(devPath, "%s%d", pSataInfo.sataInfo[i].devName, j);		
				//umount(devPath);

				Printf("umount %s \n", devPath);
				//sprintf(cmd, "umount -f %s", devPath);
				sprintf(cmd, "umount -vl %s", devPath);
				system(cmd);
				system(cmd);
			}

			sprintf(cmd, "/app/bin/formatDisk.sh %s &", pSataInfo.sataInfo[i].devName);
			printf("sata %d format disk %s \n", diskId, pSataInfo.sataInfo[i].devName);
			return system(cmd);
		}
	}
	printf("6666666666666666666666666666666666\n");
	sprintf(cmd, "%s", "echo 4 > /tmp/process.txt &");
	system(cmd);

	return 0;
}

#if 0
int mountSataDisk(unsigned int diskId)
{
	fprintf(stderr, "mount disk %d \n", diskId);

	sata_info_t pSataInfo;
	char devPath[128] = { 0 };
	char mountPath[126] = { 0 };

	memset(&pSataInfo, 0, sizeof(pSataInfo));
	findSataDisk( &pSataInfo );

	fprintf(stderr, "=====mountSataDisk  diskNum %d \n", pSataInfo.diskNum);
		fprintf(stderr, "1ddddddddddddddddddddddddddddddddddddddddddddd\r\n");
	if(pSataInfo.diskNum > 0)
	{
		fprintf(stderr, "2ddddddddddddddddddddddddddddddddddddddddddddd\r\n");
		gpioSetDiskPower(0);
	}
	else
	{
		fprintf(stderr, "3ddddddddddddddddddddddddddddddddddddddddddddd\r\n");
		gpioSetBeep(1);
	}

	int i = 0;
	for ( i = 0; i < pSataInfo.diskNum; i++) {
		fprintf(stderr, "sataPort %d devName %s size %llu G \n", 
		pSataInfo.sataInfo[i].sataPort, pSataInfo.sataInfo[i].devName, 
		pSataInfo.sataInfo[i].totalSize / 1000 / 1000 / 1000);

		if ( diskId == pSataInfo.sataInfo[i].sataPort )	{

		
			sprintf(devPath, "%s1", pSataInfo.sataInfo[i].devName);
			sprintf(mountPath, "/disk/disk%04X", diskId - 1);
			mkdir(mountPath, S_IRWXU);
			
			if (mount(devPath, mountPath, "ext4", MS_NOATIME | MS_NODIRATIME, NULL) != 0) {
				return 0;
			}

			if (checkFallocateFile(mountPath, MAX_FILE_SIZE) != 0) {
			//	umount(devPath);
			//	umount(devPath);
				return 0;
			}			
			return 1;	
		}

	}

	return 0;

}
#endif

