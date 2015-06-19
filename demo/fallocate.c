#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/vfs.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mount.h>

#include "recordLib.h"
#include "indexFunc.h"
#include "diskLib.h"

//#define FILE_SIZE	(128*1024*1024)
static record_info_t *g_pRecordInfo = NULL;


/*  */
static int getDirFileCount(char *pPath)
{
	DIR *dir;
	struct dirent * ptr;
	int total = 0;

	dir = opendir(pPath); /* */
	if(dir == NULL)
	{
		perror("fail to open dir");
		return -1;
	}

	while((ptr = readdir(dir)) != NULL)
	{
		//
		//...
		if(strcmp(ptr->d_name,".") == 0 || strcmp(ptr->d_name,"..") == 0)
		{
			continue;
		}

		if(ptr->d_type == DT_REG)
		{
			total++;
//			printf("%s%s\r\n", pPath, ptr->d_name);
		}
	}

	closedir(dir);

	return total;
}

/*  */
int getFileTotalNum(char *pDevicePath, unsigned int fileSize)
{
	struct statfs st;
	int oneFileUsedBlock = 0;

	if(NULL == pDevicePath)
	{
		return -1;
	}

	memset(&st, 0, sizeof(st));

	if(-1 == statfs(pDevicePath, &st))
	{
		return -1;
	}

	if((fileSize%st.f_bsize) == 0)
	{
		oneFileUsedBlock = fileSize/st.f_bsize;
	}
	else
	{
		printf("file used size is not block Integer multiple\r\n");
		oneFileUsedBlock = fileSize/st.f_bsize + 1;
	}
	printf("one file used %d blocks\r\n", oneFileUsedBlock);

	return ((st.f_bfree/oneFileUsedBlock) - 1);
}

/*  */
int checkFallocateFile(char *pDiskPath, unsigned int fileSize)
{
	struct statfs st;
	int oneFileUsedBlock = 0;

	printf("enter checkFallocateFile\r\n");

	if(NULL == pDiskPath)
	{
		fprintf(stderr, "fadasdf  pDiskPath NULL\r\n");
		
		return -1;
	}

	memset(&st, 0, sizeof(st));

	if(-1 == statfs(pDiskPath, &st))
	{
        	fprintf(stderr, "%s:%d:fadasd pDiskPath %s  %s \r\n", __func__,__LINE__, pDiskPath,strerror(errno));
		return -1;
	}

	if((fileSize%st.f_bsize) == 0)
	{
		oneFileUsedBlock = fileSize/st.f_bsize;
	}
	else
	{
		fprintf(stderr, "file used size is not block Integer multiple\r\n");
		oneFileUsedBlock = fileSize/st.f_bsize + 1;
	}

	if(((st.f_blocks - st.f_bfree)/oneFileUsedBlock) != getDirFileCount(pDiskPath))
	{
        	fprintf(stderr, "%s:%d: fallocate file num is error, %lu.  %d\r\n", __func__,__LINE__,((st.f_blocks - st.f_bfree)/oneFileUsedBlock ), getDirFileCount(pDiskPath));
		return -1;
	}

	return 0;
}

/*  */
int createFallocateFile(char *pFileName, int fileSize)
{
	int fd = 0;
	int ret = 0;

	fd = open(pFileName, O_RDWR | O_CREAT, 0666);
	if(fd <= 0)
	{
		return -1;
	}

	if(fallocate(fd, 0, 0, fileSize) != 0)
	{
	        fprintf(stderr,"%s:%d :fallocate fail",__func__,__LINE__);
	        close(fd);
		ret = -1;
	}

	//
	close(fd);

	return ret;
}

int main(int argc, char *argv[])
{	
	int i = 0, j = 0;
	int totalNum = 0;
	char fileName[32] = {0};
	char diskPath[128] = {0};
	char devName[128] = {0};
	unsigned int indexHandle = 0;	
	if(argc != 2)
	{		
		return -1;
	}	
	mkdir("/tmp/disk", 0644);
	sprintf(diskPath, "/tmp/disk");
	if(mount(argv[1], diskPath, "ext4", MS_NOATIME | MS_NODIRATIME, NULL) != 0)
	{
		printf("mount %s error\r\n", argv[1]);	
		return -1;
	}
	/*  */
	totalNum = getFileTotalNum(diskPath, MAX_FILE_SIZE);
	printf("have recordNo Num = %d. 0x%04X\r\n", totalNum, totalNum);	/*  */
	for(i = 0; i < totalNum; i++)	
	{		
		sprintf(fileName, "%s/%04X", diskPath, i);	
		if(createFallocateFile(fileName, MAX_FILE_SIZE) != 0)	
		{			
			return -1;	
		}	
	}	
	/*  */	
	indexHandle = indexOpen(diskPath, INDEX_MAIN_NO);
	if(indexHandle > 0)
	{		
		indexInitRecordNoFlag(indexHandle, totalNum);
		#if 0		
		for(i = RECORD_START_NO; i < totalNum; i++)	
		{		
			printf("recordNo 0x%04X, indexHandle  = %d\r\n", i, indexHandle);	
			indexModifyRecordNoFlag(indexHandle, 1, i, 0, 0);		
		}
		#endif		
		indexClose(indexHandle);
	}	
	sync();
	umount(diskPath);
	return 0;
}

#if  0
int main(void)
{
	int i = 0, j = 0;
	int totalNum[MAX_SUPPORT_DISK_NUM] = {0};
	char fileName[32] = {0};
	char diskPath[128] = {0};

//	getDiskUuid("/dev/sda1", NULL);
	#if 0
	for(j = 0; j < MAX_SUPPORT_DISK_NUM; j++)
	{
		memset(diskPath, 0, sizeof(diskPath));
		sprintf(diskPath, "/disk/disk%04X", j);
		totalNum[j] = getFileTotalNum(diskPath, MAX_FILE_SIZE);
		printf("disk %d, have recordNo Num = %d\r\n", j, totalNum[j]);

		for(i = 0; i < totalNum[j]; i++)
		{
			sprintf(fileName, "/disk/disk%04X/%04X", j, i);
			createFallocateFile(fileName, MAX_FILE_SIZE);
			printf("disk 0x%04X, create file 0x%04X success, total 0x%04X\r\n", j, i, totalNum[j]);
		}
	}
	#endif
	memset(diskPath, 0, sizeof(diskPath));
	sprintf(diskPath, "/dev/sda1");
	printf("%s\n",diskPath);
	totalNum[0] = getFileTotalNum(diskPath, MAX_FILE_SIZE);
	printf("disk %d, have recordNo Num = %d\r\n", 0, totalNum[0]);

	for(i = 0; i < totalNum[0]; i++)
	{
		sprintf(fileName, "/disk/disk%04X/%04X", j, i);
		createFallocateFile(fileName, MAX_FILE_SIZE);
		printf("disk 0x%04X, create file 0x%04X success, total 0x%04X\r\n", j, i, totalNum[j]);
	}


	g_pRecordInfo = recordLibInit();

	for(j = 0; j < MAX_SUPPORT_DISK_NUM; j++)
	{
		for(i = RECORD_START_NO; i < totalNum[j]; i++)
		{
			printf("Flag disk 0x%04X, recordNo 0x%04X\r\n", j, i);
			indexModifyRecordNoFlag(g_pRecordInfo[j].indexHandle, 1, i, 0, 0);
		}
	}

	return 0;
}
#endif



