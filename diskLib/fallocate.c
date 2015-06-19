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

#include "recordLib.h"
#include "indexFunc.h"
#include "diskLib.h"

//#define FILE_SIZE	(128*1024*1024)
//static record_info_t *g_pRecordInfo = NULL;


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
		fprintf(stderr, "fadasdfddddddddddddpDiskPath NULL\r\n");
		
		return -1;
	}

	memset(&st, 0, sizeof(st));

	if(-1 == statfs(pDiskPath, &st))
	{
        fprintf(stderr, "@@@@@@@@@@@@@@@@@@@@ %s:%d:fadasdfdddddddddddddddddddd pDiskPath %s  %s \r\n", __func__,__LINE__, pDiskPath,strerror(errno));
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
        fprintf(stderr, "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ %s:%d: fallocate file num is error, %lu.  %d\r\n", __func__,__LINE__,((st.f_blocks - st.f_bfree)/oneFileUsedBlock ), getDirFileCount(pDiskPath));
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
        fprintf(stderr,"@@@@@@@@@@@@@@@@@ %s:%d :fallocate fail",__func__,__LINE__);
        close(fd);
		ret = -1;
	}

	//
	close(fd);
	return ret;
}


