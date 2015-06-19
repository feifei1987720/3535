#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stddef.h>
#include <sys/mount.h>
#include <sys/wait.h>
#include <sys/types.h>


#include "diskLib.h"


static pthread_t g_hotPlugThreadId;


/* 获取热插拔事件 */
static int init_hotplug_sock(void)
{
    struct sockaddr_nl snl;
    const int buffersize = 8*1024;//16*1024;
    int retval = 0;

    memset(&snl,0x00,sizeof(struct sockaddr_nl));
    snl.nl_family = AF_NETLINK;
    snl.nl_pid = getpid();
    snl.nl_groups = 1;

    int hotplug_sock = socket(PF_NETLINK,SOCK_DGRAM,NETLINK_KOBJECT_UEVENT);
    if(hotplug_sock == 1)
    {
        printf("error get socket:%s",strerror(errno));
        return -1;
    }

    /* set receive buffersize */
    setsockopt(hotplug_sock, SOL_SOCKET, SO_RCVBUFFORCE, &buffersize, sizeof(buffersize));
    retval = bind(hotplug_sock,(struct sockaddr *)&snl,sizeof(struct sockaddr_nl));
    if(0 > retval)
    {
        printf("bind failed:%s",strerror(errno));
        close(hotplug_sock);
        hotplug_sock = -1;
        return -1;
    }
    return hotplug_sock;
}

/* 获取设备名称 */
static int getDevName(char *buf, char *devName)
{
    char tmpBuf[4096] = {0};
    char *p = NULL;

    strncpy(tmpBuf, buf, sizeof(tmpBuf));

    p = strtok(tmpBuf, "\n");

    while(p)
    {
        if(strstr(p, "DEVNAME=") != NULL)
        {
            sscanf(p, "DEVNAME=%s\n", devName);
        }

        p = strtok(NULL, "\n");
    }

    if(strlen(devName) == 0)
    {
        return -1;
    }

    return 0;
}


int iCheckSystemReturnValue(int p_iStatus)
{
    if (-1 == p_iStatus)
    {
        return -1;
    }
    else
    {
        printf("exit status value = [0x%x]\n", p_iStatus);
        if (WIFEXITED(p_iStatus))
        {
            if (0 == WEXITSTATUS(p_iStatus))
            {
                printf( "run shell script successfully!");
                return 0 ;
            }
            else
            {
                printf( "run shell script fail!");
                return -1;
            }
        }
        else
        {
            printf("exit status illegal= [%d]\n", WEXITSTATUS(p_iStatus));
            return -1;
        }
    }
    return 0 ;
}


typedef void (*sighandler_t)(int);

int pox_system(const char *cmd_line)
{
    int ret = 0;
    sighandler_t old_handler;
    old_handler = signal(SIGCHLD, SIG_DFL);
    ret = system(cmd_line);
    signal(SIGCHLD, old_handler);
    return ret;
}

void *hotPlugThread(void *param)
{
    int i = 0;
    int ret = 0;
    char buf[1024] = {0};
    char devName[128] = {0};
    char devPath[124] = {0};
    char mountPath[128] = {0};
    char cmd[128] = {0};

    int hotplug_sock;
    int sum,size;
    char *str;

#if 0
    unsigned char uuid[16] = {0};

    setDiskVolumeName("/dev/sda1", "abcd");

    getDiskPartitionInfo("/dev/sda1", uuid, NULL, NULL);

    checkFallocateFile("/disk/disk0000", 1024*1024*1024);
#endif

    hotplug_sock = init_hotplug_sock();

    while(1)
    {
        sum = 0;
        memset(buf, 0, sizeof(buf));
        size = recv(hotplug_sock, buf, sizeof(buf), 0);
        	//	fprintf(stderr,"size=%d, buf=[%s], strlen(buf)=%d\n", size, buf, strlen(buf));
        while(sum < size)
        {
            str = buf + sum;
            sum += strlen(str);
            buf[sum] = '\n';
        }
        buf[sum] = 0;

      //  fprintf(stderr,"buf=[%s]\n\n", buf);

        if(((strstr(buf, "ACTION=add") != NULL) && (strstr(buf, "DEVTYPE=disk") != NULL))||((strstr(buf, "ACTION=add") != NULL) && (strstr(buf, "DEVTYPE=partition") != NULL)))
        {
            if((strstr(buf, "NPARTS=0") != NULL) || (strstr(buf, "PARTN=") != NULL))
            {
                memset(devName, 0, sizeof(devName));
                if(getDevName(buf, devName) == 0)
                {
                    fprintf(stderr,"add devName = [%s]\r\n", devName);
                    if(strstr(buf, "DEVPATH=/devices/platform/hiusb-ehci") != NULL)
                    {//u盘
                        for(i = 0; i < 8; i++)
                        {
                            sprintf(devPath, "/dev/%s", devName);
                            sprintf(mountPath, "/usb/usb%04d", i);
                            ret = checkDiskPartitionIsMount(devPath, mountPath);
                            if(ret == 0x00)
                            {
                                sprintf(cmd, "mount %s %s", devPath, mountPath);
                                ret=pox_system(cmd);
                                ret=iCheckSystemReturnValue(ret);
                                if(0!=ret)
                                    continue;
                            }
                            else if(ret == 0x03)
                            {
                                break;
                            }
                        }
                    }
                }
            }
        }

        if(((strstr(buf,"ACTION=remove")!=NULL) && (strstr(buf, "DEVTYPE=disk") != NULL))||((strstr(buf, "ACTION=remove") != NULL) && (strstr(buf, "DEVTYPE=partition") != NULL)))
        {
            if((strstr(buf, "NPARTS=0") != NULL) || (strstr(buf, "PARTN=") != NULL))
            {
                if(getDevName(buf, devName) == 0)
                {
                    printf("remove devName = [%s]\r\n", devName);
                    if(strstr(buf, "DEVPATH=/devices/platform/hiusb-ehci") != NULL)
                    {//u
                        sprintf(devPath, "/dev/%s", devName);
                        sprintf(cmd, "umount %s", devPath);
                        int i ;
                        for(i=0;i!=4;++i)
                        {
                            ret=pox_system(cmd);
                            ret=iCheckSystemReturnValue(ret);
                            if(0==ret)
                            {
                                break;
                            }
                            fprintf(stderr,"remove devName = [%s] fail \r\n", devName);
                            sleep(1);
                        }
                        umount(devPath);
                        umount(devPath);
                    }

                    else if(strstr(buf, "DEVPATH=/devices/platform/ahci") != NULL)
                    {//sata硬盘

                        fprintf(stderr , "\n===============%s \n", buf);

                        sprintf(devPath, "/dev/%s", devName);
                        sprintf(cmd, "umount -f %s", devPath);

                        fprintf(stderr, "================cmd %s \n", cmd);
                        system(cmd);
                        system(cmd);
                    }
		else
		{
			umount("/usb/usb0000");
			umount("/usb/usb0001");
			umount("/usb/usb0002");
			umount("/usb/usb0003");
		}
                }
            }
        }
        usleep(400*1000);
    }

    return NULL;
}

int startHotPlugThread()
{
    if(pthread_create(&g_hotPlugThreadId, NULL, hotPlugThread, NULL) < 0)
    {
        printf("hot plug thread error\r\n");
        return -1;
    }

    return 0;
}

