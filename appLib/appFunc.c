/**********************************************************
 * appFunc.c
 * 
 * 
 *
 * 
 * 
 * 
 **********************************************************/

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>


#include "sdk_struct.h"
#include "main.h"
#include "param.h"
#include "networkLib.h"
#include "commonLib.h"
#include "databaseLib.h"
#include "decodeLib.h"
#include "appLib.h"
#include "diskLib.h"
#include "recordLib.h"
#include "hiLib.h"
#include "configLib.h"
#include "watchdogLib.h"
#include <signal.h>
#define  P2P_DANALE_RUN_CMD  "/app/bin/P2P_Danale &"
#define  P2P_FREE_IP_RUN_CMD  "/app/bin/P2P_FreeIp &"
#define  DB_BK_TABLE_NUM     7
//static pthread_mutex_t databaseMutex = PTHREAD_MUTEX_INITIALIZER;
static unsigned char formatDiskFlag;   //标志现在是不是在格式化硬盘的状态
extern pthread_mutex_t databaseMutex;
static sdk_preview_t g_previewInfo;

void appStartDecodeChannel(sdk_channel_t *pDevChannelInfo)
{
	int tmpChannelNo = 0;
	int tmpStreamNo = 0;
	int initMode = 0;
	network_info_t networkInfo;
	network_info_t networkInfo_old;

	memset(&networkInfo, 0, sizeof(network_info_t));

	networkInfo.channelNo = pDevChannelInfo->chann_no;
	networkInfo.streamNo = pDevChannelInfo->chann_win_mode;
      
	strncpy(networkInfo.rtspmainurl,pDevChannelInfo->rtsp_main_url,MAX_RTSP_MAINURL_LEN);
	strncpy(networkInfo.rtspsuburl,pDevChannelInfo->rtsp_sub_url,MAX_RTSP_MAINURL_LEN);
	
	networkInfo.protocol = pDevChannelInfo->manufacturer;
	strcpy((char *)networkInfo.onvifAddr, (char *)pDevChannelInfo->ip_addr);
	networkInfo.onvifPort = pDevChannelInfo->port;
	strcpy(networkInfo.userName, (char *)pDevChannelInfo->user.user_name);
	strcpy(networkInfo.userPwd, (char *)pDevChannelInfo->user.user_pwd);

	if(configGetDisplayNum() == 32)		initMode = 7;
	else if(configGetDisplayNum() == 25)	initMode = 6;
	else if(configGetDisplayNum() == 16)	initMode = 5;
	else if(configGetDisplayNum() == 9)	initMode = 4;
	else if(configGetDisplayNum() == 8)	initMode = 3;
	else if(configGetDisplayNum() == 6)	initMode = 2;
	else if(configGetDisplayNum() == 4)	initMode = 1;
	else if(configGetDisplayNum() == 1)	initMode = 0;
	else					initMode = 0;

	//Printf("channelNo %d, enable %d, chann_win_mode %d, networkInfo.streamNo == %d, viewPos = %d, userName (%s), pwd(%s)\r\n",
	//networkInfo.channelNo, pDevChannelInfo->enable, pDevChannelInfo->chann_win_mode, networkInfo.streamNo, g_previewInfo.windows[initMode][networkInfo.channelNo], networkInfo.userName, networkInfo.userPwd);

	networkInfo.enable = pDevChannelInfo->enable;
	networkGetNetworkInfo(networkInfo.channelNo,networkInfo.streamNo,&networkInfo_old);

	networkSetNetworkInfo(&networkInfo);

	if(pDevChannelInfo->enable == 1)
	{
		//decodeStopDecode(pDevChannelInfo->chann_no);
		if(decodeGetCurViewChannelNoAndStreamNo(g_previewInfo.windows[initMode][networkInfo.channelNo], &tmpChannelNo, &tmpStreamNo) != 0)
		{
			if((tmpChannelNo != networkInfo.channelNo) || (tmpStreamNo != networkInfo.streamNo)||(networkInfo_old.protocol != networkInfo.protocol))
			{
				decodeStopDecode(g_previewInfo.windows[initMode][networkInfo.channelNo]);
				if(networkInfo_old.protocol != networkInfo.protocol)
				{
					networkStopStream(networkInfo.channelNo, 0, NETWORK_INVITE_TYPE_RECORD);
					sleep(2);
				}
			}
		}

		networkRestartStream(networkInfo.channelNo, 1);//networkInfo.streamNo);
		networkRestartStream(networkInfo.channelNo, 0);//networkInfo.streamNo);
		decodeStartDecode(networkInfo.channelNo, networkInfo.streamNo, g_previewInfo.windows[initMode][networkInfo.channelNo], 1);
	}
	else
	{
		decodeStopDecode(g_previewInfo.windows[initMode][networkInfo.channelNo]);
		//networkStopStream(networkInfo.channelNo, 0, NETWORK_INVITE_TYPE_ALL);
		//networkStopStream(networkInfo.channelNo, 1, NETWORK_INVITE_TYPE_ALL);
		networkRestartStream(networkInfo.channelNo, 1);//networkInfo.streamNo);
		networkRestartStream(networkInfo.channelNo, 0);//networkInfo.streamNo);
	}
	decodeSetViewChannelNoAndStreamNo(g_previewInfo.windows[initMode][networkInfo.channelNo], networkInfo.channelNo, networkInfo.streamNo);
}

int appStartDecode()
{
	int i = 0;
	int len = 0;
	sdk_channel_t devChannelInfo[MAX_DISPLAY_NUM];

	int channel = CHANN_TYPE_ALL;

	len = databaseOperate(SDK_MAIN_MSG_PREVIEW_CFG, SDK_PARAM_GET, NULL, 0, &g_previewInfo, sizeof(g_previewInfo));
	if(len != sizeof(g_previewInfo))
	{
		Printf("get preview info error, len = %d\r\n", len);
	}

	len = databaseOperate(SDK_MAIN_MSG_CHANN_CFG, SDK_PARAM_GET, &channel, sizeof(channel), &devChannelInfo, sizeof(devChannelInfo));
	if(len > 0)
	{
		for(i = 0; i < len/sizeof(sdk_channel_t); i++)
		{
			if(i >= configGetDisplayNum())
			{
				break;
			}

			appStartDecodeChannel(&devChannelInfo[i]);
		}
	}

	return 0;
}

int appParamGetResolution(int *resolution, int *encodeMode)
{
	sdk_comm_cfg_t comm;

	*resolution = PIC_HD720;
	*encodeMode = VIDEO_ENCODING_MODE_NTSC;

	if(databaseOperate(SDK_MAIN_MSG_COMM_CFG, SDK_PARAM_GET, NULL, 0, &comm, sizeof(sdk_comm_cfg_t)) == sizeof(sdk_comm_cfg_t))
	{
		if(comm.resolution == 0)
		{
			*resolution = PIC_XGA;
		}
		else if(comm.resolution == 1)
		{
			*resolution = PIC_HD720;
		}
		else if(comm.resolution == 2)
		{
			*resolution = PIC_SXGA;
		}
		else if(comm.resolution == 3)
		{
			*resolution = PIC_HD1080;
		}
		else
		{
			*resolution = PIC_HD720;
		}

		if(comm.video_mode == 0)
		{
			*encodeMode = VIDEO_ENCODING_MODE_NTSC;
		}
		else
		{
			*encodeMode = VIDEO_ENCODING_MODE_NTSC;
		}
	}
	return 0;
}

void appParamSetNetwork(sdk_eth_cfg_t *p_eth_cfg)
{
	if(p_eth_cfg->ip_info.enable_dhcp == 1)
	{
		dhcpNetFun();
	}
	else
	{
		netSetIp("eth0", inet_addr((char *)p_eth_cfg->ip_info.ip_addr));
		netSetMask("eth0", inet_addr((char *)p_eth_cfg->ip_info.mask));
		netSetGw("eth0", inet_addr((char *)p_eth_cfg->ip_info.gateway));
		netSetRoute("eth0", inet_addr((char *)p_eth_cfg->ip_info.gateway));	
		//SetDns((char *)p_eth_cfg->ip_info.dns1, (char *)p_eth_cfg->ip_info.dns2);
		netSetDns(inet_addr((char *)p_eth_cfg->ip_info.dns1), inet_addr((char *)p_eth_cfg->ip_info.dns2));
	}
}

void appParamInitNetwork()
{
	int no = 0;
	int len = 0;
	sdk_eth_cfg_t net;

	memset(&net, 0, sizeof(sdk_eth_cfg_t));
	len = databaseOperate(SDK_MAIN_MSG_NET_LINK_CFG, SDK_PARAM_GET, &no, sizeof(no), &net, sizeof(sdk_eth_cfg_t));
	if(len == sizeof(sdk_eth_cfg_t))
	{
		appParamSetNetwork(&net);
	}
}

void appParamInit()
{
	appParamInitNetwork();
}

#define UPGRADE_FILEPATH_LEN 64

static int get_updatefile(char *filepath,int platform,char *usbpath)
{
	int i = 0;
	char tmpfile[128];
	//char usbpath[128];
	DIR *usb_dir=NULL;
	struct dirent *usb_item = NULL;
	//memset(usbpath,0,128);
	//for(i=0;i<8;i++)
	{
		//sprintf(usbpath, "/usb/usb%04d/", i);
		//fprintf(stderr,"[%s:%s:%d]:usbpath=%s \n",__FILE__,__FUNCTION__,__LINE__,usbpath);
		usb_dir = opendir(usbpath);
		if(usb_dir == NULL)
			return -1;
		while ((usb_item = readdir(usb_dir)) != NULL)
		{
			if (strcmp(usb_item->d_name, ".") == 0|| strcmp(usb_item->d_name, "..") == 0)
			{
				continue;
			}
			if(strstr(usb_item->d_name,"_update") != NULL)
			{
				int fdsrc = 0;
				MYUpdate_Info g_update_info;

				memset(tmpfile,0,sizeof(tmpfile));
				sprintf(tmpfile,"%s%s",usbpath,usb_item->d_name);
				fdsrc = open(tmpfile,O_RDONLY);

				//fprintf(stderr,"[%s:%s:%d]:tmpfile=%s \n",__FILE__,__FUNCTION__,__LINE__,tmpfile);
				if(fdsrc == -1)
					continue;
				lseek(fdsrc,0,SEEK_SET);

				if(read(fdsrc,&g_update_info,sizeof(g_update_info)) != sizeof(g_update_info))
				{		
					continue;
				}
				//fprintf(stderr,"[%s:%s:%d]:platform=%d \n",__FILE__,__FUNCTION__,__LINE__,platform);
				switch(platform)
				{
					case CPU_3520D:
						if(strcmp(g_update_info.platform_name,"HI3520D") == 0)
						{
							strcpy(filepath,usb_item->d_name);
							close(fdsrc);
							close(usb_dir);
							return 0;
						}
						break;
					case CPU_3535:
						if(strcmp(g_update_info.platform_name,"HI3535") == 0)
						{
							//fprintf(stderr,"[%s:%s:%d]:debug \n",__FILE__,__FUNCTION__,__LINE__);
							strcpy(filepath,usb_item->d_name);
							close(fdsrc);
							close(usb_dir);
							return 0;
						}
						break;
					default:
						break;
				}

			}

		}
	}

	return -1;
}



int extract_update_file(char *filepath)
{
	int i = 0;
	char buf[1024]={0};
	char *p = buf;
	DIR *dir=NULL;	
	struct dirent *item = NULL;
	char srcPath[UPGRADE_FILEPATH_LEN];	
	char itemPath[UPGRADE_FILEPATH_LEN];
	int fdsrc = 0;
	MYUpdate_Info g_update_info;

	unsigned int totalFileSize = 0;
	unsigned int readed_file_size = 0;

	strcpy(srcPath,"/tmp/update/");
	struct stat st;

	system("mkdir /tmp/update/");
	fprintf(stderr,"[%s:%s:%d]:filepath=%s \n",__FILE__,__FUNCTION__,__LINE__,filepath);

	if(stat(filepath, &st) < 0)
	{  
		fprintf(stderr,"[%s:%s:%d]:get file %s size failed \n",__FILE__,__FUNCTION__,__LINE__,filepath);
		return -1;  
	}
	else
	{  
		totalFileSize = st.st_size;  
	}  


	fdsrc = open(filepath,O_RDONLY);
	if(fdsrc == -1)
		return -1;	
	lseek(fdsrc,0,SEEK_SET);
	if(read(fdsrc,&g_update_info,sizeof(g_update_info)) != sizeof(g_update_info))
	{		
		printf("read head err!\n");
		return -1;
	}
	int fddst=-1;
	unsigned int offset,file_len;
	//unsigned int rlength=0,Rlength = 0,wlength=0;  	
	//unsigned int left;
	unsigned int rlen = 0;
	unsigned int ret = 0;
	for(i=0;i<FILE_NUM;i++)
	{		
		
		lseek(fdsrc,0,SEEK_SET);		
		offset = g_update_info.files[i].offset;
		lseek(fdsrc,offset,SEEK_SET);
		memset(itemPath,0,sizeof(itemPath));
		strcpy(itemPath,srcPath);
		strcat(itemPath,g_update_info.files[i].filename);
		fprintf(stderr,"[%s:%s:%d]:itemPath=%s \n",__FILE__,__FUNCTION__,__LINE__,itemPath);

		fddst = open(itemPath,O_CREAT|O_RDWR|O_APPEND,0666);

		//rlength=0;
		//Rlength=0;
		//wlength=0;                                                         
		file_len = g_update_info.files[i].size;

#if 0
		while(length > 0)
		{
			left = length;
			if(left > 1024)
			{
				rlength = read(fdsrc,p,1024);
			}
			else
			{
				rlength = read(fdsrc,p,left);

			}		
			Rlength = rlength;
			while(rlength > 0)
			{
				wlength = write(fddst,p,rlength);
				rlength -= wlength;
				p += wlength;
			}
			p = buf;
			length -= Rlength;
		}
#else
		if(file_len < 1024)
			rlen = file_len;
		else
			rlen = 1024;
		while(file_len >0)
		{
			ret = read(fdsrc,p,rlen);
			if(ret != rlen)
			{
				fprintf(stderr,"[%s:%s:%d]:read file %s failed, \n",__FILE__,__FUNCTION__,__LINE__,g_update_info.files[i].filename);
				return -1;
			}
			write(fddst,p,rlen);
			file_len -= rlen;
			if(file_len < 1024)
				rlen = file_len;
			else
				rlen = 1024;
		}
#endif
		close(fddst);
		readed_file_size+=g_update_info.files[i].size;
	}
	
	if(readed_file_size < totalFileSize)
	{
		unsigned int uboot_len = totalFileSize-readed_file_size-sizeof(MYUpdate_Info);
		memset(itemPath,0,sizeof(itemPath));
		strcpy(itemPath,srcPath);

		#ifdef  HI3520D
			strcat(itemPath,"HI3520DUBOOT.bin");	
		#elif   HI3535					
			strcat(itemPath,"HI3535UBOOT.bin");
		#elif   HI3521
			strcat(itemPath,"HI3521UBOOT.bin");
		#elif   HI3531
			strcat(itemPath,"HI3531UBOOT.bin"); 
		#endif
		
		Printf("itemPath=%s,ubootlen=%u,totalFileSize=%u,readed_file_size=%u\n",itemPath,uboot_len,totalFileSize,readed_file_size);
		fddst = open(itemPath,O_CREAT|O_RDWR|O_APPEND,0666);
		if(uboot_len < 1024)
			rlen = uboot_len;
		else
			rlen = 1024;
		while(uboot_len >0)
		{
			ret = read(fdsrc,p,rlen);
			if(ret != rlen)
			{
				fprintf(stderr,"[%s:%s:%d]:read file %s failed,ret=%d,rlen=%d \n",__FILE__,__FUNCTION__,__LINE__,itemPath,ret,rlen);
				close(fddst);
				//exit(0);
				return -1;
			}
			write(fddst,p,rlen);
			uboot_len -= rlen;
			if(uboot_len < 1024)
				rlen = uboot_len;
			else
				rlen = 1024;
		}
		close(fddst);
	}
	close(fdsrc);
	return 0;
}

int cmdOperate(int type, int subType, void *data, int len, void *pRecvData, int recvDataLen)
{
	int i = 0;
	int ret = 0;
	static int update = 0;
	switch ( type )
	{
		case SDK_MAIN_MSG_DISK:
			{
				switch ( subType )
				{
					case SDK_DISK_FORMAT: 
						{
							int diskId = *(int*)data;
							Printf(" %s %s  SDK_DISK_FORMAT diskId %d\n", __FILE__, __FUNCTION__, diskId );

							//P2pCtrl(P2P_FREE_IP,0);
							//P2pCtrl(P2P_DANALE,0);
							recordStopAllDevChannel();
							//decodeStopAllMainDecode();
							for(i=0;i<configGetSataNum();i++)
							{
								recordStopUsedDiskNo(i);
								Printf("recordStopUsedDiskNo(%d)\n",i);
							}
							recordLibdestroy();
							decodeStopAllDecode();
							//decodePauseAllDecode();
							sleep(2);
							formatDiskFlag =1;
							ret = FormatDisk(diskId);
							break;
						}
					case SDK_DISK_PROGRESS:
						{
							int *p = (int*)data;

							FILE* pF = fopen("/tmp/process.txt", "r");

							int process = 20;
							if ( pF ) {
								process = fgetc(pF);
								fclose(pF);
								pF = NULL;
							}

							if ( 49 == process ) { // 1
								process = 20;

							} else if ( 50 == process ) { // 2
								process = 60;

							} else if ( 51 == process ) { // 3
								process = 90;

							} else if ( 52 == process ) { // 4
								process = 100;
							}	

							*p  = process;
							Printf(" %s %s SDK_DISK_PROGRESS chann %d\n", __FILE__, __FUNCTION__, *p );	

							break;
						}

					case SDK_DISK_UMOUNT:
						{


							break;
						}

					default:
						break;					
				}				
				break;
			}
		case SDK_MAIN_MSG_VERSION:
			{
				sdk_version_t *p_cfg = (sdk_version_t *)pRecvData;

				memset((char *)p_cfg->serial_no, 0, sizeof(p_cfg->serial_no));
				char buf[64];
				memset(&buf,0,sizeof(buf));
				FILE *fp = fopen(SN_FILE_DANALE, "r");
				if (!fp )
				{
					fp=NULL;
					break;
				}
				ret=fread(&buf, sizeof(buf), 1, fp);
				if(ret<0)
				{
					Printf("read  fail\n");
				}
				if(fp>0)
				{
					fclose(fp);
				}
				sprintf((char *)p_cfg->serial_no, "%s", buf);
				sprintf((char *)p_cfg->device_type, "%d-CHANNEL", configGetDisplayNum());
				strcpy((char *)p_cfg->software_ver, NVR_VERSION);
				strcpy((char *)p_cfg->software_date, __DATE__);

				ret = sizeof(sdk_version_t);
				break;
			}
		case SDK_MAIN_MSG_CLOSE_SYSTEM:
			{
				if(subType == SDK_CLOSE_SYS_SHUTDOWN)
				{
					Printf("SDK_CLOSE_SYS_SHUTDOWN\r\n");
					recordStopAllDevChannel();
					recordLibdestroy();
					for(i=0;i<configGetSataNum() + configGetEsataNum();i++)
					{
						recordStopUsedDiskNo(i);
						Printf("recordStopUsedDiskNo(%d)\n",i);
					}
					sleep(3);
					system("/app/bin/init");
					system("halt");
				}
				else if(subType == SDK_CLOSE_SYS_REBOOT)
				{
					Printf("SDK_CLOSE_SYS_REBOOT\r\n");
					if(formatDiskFlag ==1)
					{
						system("reboot");
					}
					else
					{
						recordStopAllDevChannel();
						for(i=0;i<configGetSataNum() + configGetEsataNum();i++)
						{
							recordStopUsedDiskNo(i);
							Printf("recordStopUsedDiskNo(%d)\n",i);
						}
					}
					sleep(2);
					ret=system("\nreboot\n");
					if(ret<0)
					{
						Printf("system reboot fail \n");
						perror(system("reboot\n"));
					}
					system("reboot\n");
				}
				else if(subType == SDK_CLOSE_SYS_REBOOT)
				{
					Printf("SDK_COLSE_SYS_LOGOUT\r\n");
				}
				break;
			}
		case SDK_MAIN_MSG_UPGRAD:
			{
				char filePath[1024] = {0};
				char fileDir[1024] = {0};
				static char mountPath[1024] = {0};
				struct stat st;
				char cmd[512] = {0};
				int ret_update =0;
				if(subType == SDK_UPGRAD_REQUEST)
				{
				}
				else if(subType == SDK_UPGRAD_DISK_FILE)
				{
					update = 0;
					recordStopAllDevChannel();
					recordLibdestroy();
					decodeStopAllDecode();
					sleep(3);

					upgrad_pkg_file_t *pfile = (upgrad_pkg_file_t *)pRecvData;
					memset(mountPath, 0, sizeof(mountPath));
					strcpy(mountPath, (char *)pfile->file_path);
					if(strstr((char*)pfile->file_path,"OCXUpdate") == NULL)
					{
						//sprintf(cmd, "mkdir /tmp/update/ ; tar zxf %s/%s -C /tmp/update/", pfile->file_path, pfile->name);
						//system(cmd);
						memset(filePath,0,sizeof(filePath));
						sprintf(filePath,"%s%s",pfile->file_path,pfile->name);
						ret_update = extract_update_file(filePath);
						if(ret_update == -1)
							break;
					}


					stopWtdthread();
					sprintf(cmd, "cp /app/bin/update.sh /tmp/update/ -fa && chmod 777 /tmp/update/update.sh && /tmp/update/update.sh /tmp/update &");
					//				Printf("cmd === (%s)\r\n", cmd);
					system(cmd);
				}
				else if(subType == SDK_UPGRAD_MEM_FILE)
				{
				}
				else if(subType == SDK_UPGRAD_PROGRESS)
				{
					int *p = (int*)data;

					FILE* pF = fopen("/tmp/update.txt", "r");

					int process = 0;
					if(pF)
					{
						process = fgetc(pF);
						fclose(pF);
						pF = NULL;
					}

					if(49 == process)
					{
						if(update < 30)
						{
							update = 30;
						}
					}
					else if(50 == process)
					{
						if(update < 60)
						{
							update = 60;
						}
					}
					else if(51 == process)
					{
						if(update < 99)
						{
							update = 99;
						}
					}
					else if(52 == process)
					{
						update = 100;
					}

					if(((update >= 0) && (update < 29))
							|| ((update >= 30) && (update < 59))
							|| ((update >= 60) && (update < 99))
					  )
					{
						*p = update++;
					}
					else if(update >= 100)
					{
						sprintf(cmd, "umount %s && sleep 2 && reboot", mountPath);
						system(cmd);
						*p = 100;
					}
					else
					{
						*p = update;
					}

					ret = sizeof(int);
					//ret = 0;
					//Printf("###################   update=%d\n",update);
				}
				else if(subType == SDK_UPGRAD_FIND_FILE)
				{
					update = 0;
					upgrad_pkg_file_t *pUpgrad = (upgrad_pkg_file_t *)pRecvData;
					char filename[128];

					for(i = 0; i < 8; i++)
					{

						memset(filePath,0,sizeof(filePath));
						memset(fileDir,0,sizeof(fileDir));
						memset(filename,0,sizeof(filename));
						memset(&st, 0, sizeof(st));

						//sprintf(filePath, "/usb/usb%04d/update_nvr3531.tgz", i);
						sprintf(fileDir, "/usb/usb%04d/", i);
						if(get_updatefile(filename,configGetCpuType(), fileDir) == 0)
						{
							sprintf(filePath, "%s%s", fileDir,filename);
							if(stat(filePath, &st) == 0)
							{
								struct tm * pTm = localtime(&st.st_mtime);
								sprintf((char *)pUpgrad->name, filename);
								sprintf((char *)pUpgrad->file_path, "%s", fileDir);
								pUpgrad->size = st.st_size;
								sprintf((char *)pUpgrad->date, "%04d-%02d-%02d", pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday);
								ret += sizeof(upgrad_pkg_file_t);
								pUpgrad++;
							}
						}
#if 0
						if(configGetCpuType()==CPU_3531)
						{

							if(stat(filePath, &st) == 0)
							{
								struct tm * pTm = localtime(&st.st_mtime);
								sprintf((char *)pUpgrad->name, "update_nvr3531.tgz");
								sprintf((char *)pUpgrad->file_path, "%s", fileDir);
								pUpgrad->size = st.st_size;
								sprintf((char *)pUpgrad->date, "%04d-%02d-%02d", pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday);
								ret += sizeof(upgrad_pkg_file_t);
								pUpgrad++;
							}
						}
						else if(configGetCpuType()==CPU_3521)
						{
							sprintf(filePath, "/usb/usb%04d/update_nvr3521.tgz", i);
							sprintf(fileDir, "/usb/usb%04d", i);
							if(stat(filePath, &st) == 0)
							{
								struct tm * pTm = localtime(&st.st_mtime);
								sprintf((char *)pUpgrad->name, "update_nvr3521.tgz");
								sprintf((char *)pUpgrad->file_path, "%s", fileDir);
								pUpgrad->size = st.st_size;
								sprintf((char *)pUpgrad->date, "%04d-%02d-%02d", pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday);
								ret += sizeof(upgrad_pkg_file_t);
								pUpgrad++;
							}
						}
						else if(configGetCpuType()==CPU_3520D)
						{
							sprintf(filePath, "/usb/usb%04d/update_nvr3520D.tgz", i);
							sprintf(fileDir, "/usb/usb%04d", i);
							if(stat(filePath, &st) == 0)
							{
								struct tm * pTm = localtime(&st.st_mtime);
								sprintf((char *)pUpgrad->name, "update_nvr3520D.tgz");
								sprintf((char *)pUpgrad->file_path, "%s", fileDir);
								pUpgrad->size = st.st_size;
								sprintf((char *)pUpgrad->date, "%04d-%02d-%02d", pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday);
								ret += sizeof(upgrad_pkg_file_t);
								pUpgrad++;
							}
						}
						else if(configGetCpuType() == CPU_3535)
						{
							sprintf(filePath, "/usb/usb%04d/update_nvr3535.tgz", i);
							sprintf(fileDir, "/usb/usb%04d", i);
							if(stat(filePath, &st) == 0)
							{
								struct tm * pTm = localtime(&st.st_mtime);
								sprintf((char *)pUpgrad->name, "update_nvr3535.tgz");
								sprintf((char *)pUpgrad->file_path, "%s", fileDir);
								pUpgrad->size = st.st_size;
								sprintf((char *)pUpgrad->date, "%04d-%02d-%02d", pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday);
								ret += sizeof(upgrad_pkg_file_t);
								pUpgrad++;
							}
						}


						sprintf(filePath, "/usb/usb%04d/update_nvr.tgz", i);
						sprintf(fileDir, "/usb/usb%04d", i);
						if(stat(filePath, &st) == 0)
						{
							struct tm * pTm = localtime(&st.st_mtime);
							sprintf((char *)pUpgrad->name, "update_nvr.tgz");
							sprintf((char *)pUpgrad->file_path, "%s", fileDir);
							pUpgrad->size = st.st_size;
							sprintf((char *)pUpgrad->date, "%04d-%02d-%02d", pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday);
							ret += sizeof(upgrad_pkg_file_t);
							pUpgrad++;
						}
#endif
					}
				}
				else
				{
					Printf("can not support upgrad cmd %d\r\n", subType);
				}
				break;
			}
		default:
			{
				break;
			}
	}

	return ret;
}
int upnpStart(int type, sdk_upnp_t *pUpnp)
{
	int len = 0;
	//	int ret = 0;
	char upnp_cmd[1024] = {0};
	sdk_upnp_t upnp;

	if(type == 0)
	{
		memset(&upnp, 0, sizeof(sdk_upnp_t));
		len = databaseOperate(SDK_MAIN_MSG_UPNP_CFG, SDK_PARAM_GET, NULL, 0, &upnp, sizeof(sdk_upnp_t));
	}
	else if(pUpnp != NULL)
	{
		memcpy(&upnp, pUpnp, sizeof(sdk_upnp_t));
	}

	system("killall -9 upnp");

	if(upnp.enable)
	{
		sprintf(upnp_cmd, "%s -IS %d -ES %d -WI %d -WE %d -start &\n", "uPnP", upnp.cmd_port, upnp.data_port, upnp.web_port, upnp.res_port);
		system(upnp_cmd);
	}
	else
	{
		system("killall -9 upnp");
	}

	return 0;
}
int GetP2pSn(char *SnFile,char *Sn)
{
	FILE *fp = NULL;
	fp = fopen(SnFile, "r");
	if (fp == NULL) 
	{
		return -1;
	}
	if(fread(Sn, SN_LEN-1, 1, fp)<0)
	{
		fprintf(stderr,"read fail  %s:%d\n",__FILE__,__LINE__);
		fclose(fp);
		return -1;
	}
	fclose(fp);
	return 0;
}
// 系统启动时，将Danale 和FreeIp的P2P序列号更新到数据库
int  P2pSnUpdate(void)
{
	int ret=0;
	char buf[2][SN_LEN] = {{0}};
	sdk_p2p_conf_t p2p_info[2];
	memset(&buf,0,sizeof(buf));
	GetP2pSn(SN_FILE_FREEIP,buf[0]);
	GetP2pSn(SN_FILE_DANALE,buf[1]);
	memset(p2p_info,0,sizeof(p2p_info));
	ret=databaseOperate(SDK_MAIN_MSG_P2P, SDK_PARAM_GET, NULL, 0, p2p_info, sizeof(sdk_p2p_conf_t));
	if(ret<0)
	{
		fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_DDNS_CFG  read  fail %s:%d\n",__FILE__,__LINE__);
		return -1;
	}
	strcpy(p2p_info[0].SerialNum,buf[0]);
	strcpy(p2p_info[1].SerialNum,buf[1]);
	ret=databaseOperate(SDK_MAIN_MSG_P2P, SDK_PARAM_SET, NULL, 0, p2p_info, sizeof(sdk_p2p_conf_t));
	if(ret<0)
	{
		fprintf(stderr,"databaseOperate  SDK_MAIN_MSG_DDNS_CFG  write  fail %s:%d\n",__FILE__,__LINE__);
		return -1;
	}	
	return 0;
}
//系统启动时，根据数据库的P2P使能信息启动(关闭)对应的P2P程序
int P2pInitStart(void)
{
	sdk_p2p_conf_t p2p_info[2];
	int ret = 0;
	Printf("P2pInitStart\n\n");
	memset(p2p_info,0,sizeof(p2p_info));
	ret=databaseOperate(SDK_MAIN_MSG_P2P, SDK_PARAM_GET, NULL, 0, p2p_info, sizeof(sdk_p2p_conf_t));
	if(ret<0)
	{
		Printf("database Error!\n");
		return -1;
	}
	system("killall -9 P2P_FreeIp");
	system("killall -9 P2P_Danale");
	#if  0
	if(p2p_info[0].P2pEn)// FreeIP P2P
	{
		system(P2P_FREE_IP_RUN_CMD);
	}
	else if(p2p_info[1].P2pEn)// Danale P2P
	{
		system(P2P_DANALE_RUN_CMD);
	}
	#endif
	return 0;
}
void sigAlarm3SFreeIp(int sig)
{
	system(P2P_FREE_IP_RUN_CMD);
	signal(SIGALRM,sigAlarm3SFreeIp);
}
void sigAlarm3SDanale(int sig)
{
	system(P2P_DANALE_RUN_CMD);
	signal(SIGALRM,sigAlarm3SDanale);
}

int P2pCtrl(int P2pType,int P2pEn)
{
	#if  1
	switch(P2pType)
	{
		case P2P_FREE_IP:
			{
				if(P2pEn)
				{
					p2pTHRStart();
				}
				else
				{
					P2P_end();
				
				}
			}
			break;
	#if 0
		case P2P_DANALE:
			{
				if(P2pEn)
				{
					// 开启Danale的P2P进程
					system("killall -9 P2P_Danale");
					system("killall -9 P2P_FreeIp");
					signal(SIGALRM,sigAlarm3SDanale);
					alarm(3); 
				}
				else
				{
					// 关闭Danale的P2P进程
					system("killall -9 P2P_Danale");
					system("killall -9 P2P_FreeIp");
				}
			}
			break;
    #endif
		default:
			break;
	}
	#endif
	return 0;
}
#if  1
// 获取码流速率
int GetNetworkFlow(char * interface ,double * recv_rate, double * send_rate)
{ 
	static double  time_pre = 0;
	static long long  recv_pre= 0;
	static long long  send_pre= 0;
	static int g_recv_rate = 0;
	struct timeval tv_now;
	int nDevLen = strlen(interface);
	if (nDevLen < 1 || nDevLen > 100)
	{
		printf("dev length too long\n");
		return -1;
	}
	FILE* fd = fopen("/proc/net/dev","r+");
	if (NULL == fd)
	{
		printf("/proc/net/dev not exists!\n");
		return -1;
	}
	char buf[1024];
	fseek(fd, 0, SEEK_SET);
	int nBytes = fread(buf,1, sizeof(buf)-1,fd);
	if (-1 == nBytes)
	{
		perror("read error");
		fclose(fd);
		return -1;
	}
	buf[nBytes] = '\0';
	//返回第一次指向interface位置的指针
	char* pDev = strstr(buf, interface);
	if (NULL == pDev)
	{
		printf("don't find dev %s\n", interface);
		fclose(fd);
		return -1;
	}
	char *p;
	int i = 0;
	long long recv = 0 ;
	long long send = 0;
	/*去除空格，制表符，换行符等不需要的字段*/
	for (p = strtok(pDev, " \t\r\n"); p; p = strtok(NULL, " \t\r\n"))
	{
		char ifconfig_value[32]; 
		i++;
		strcpy(ifconfig_value, p);
		/*得到的字符串中的第二个字段是接收流量*/
		if(i == 2)
		{
			gettimeofday(&tv_now,NULL);
			recv = atoll(ifconfig_value);
			*recv_rate= (recv - recv_pre)/(tv_now.tv_sec+tv_now.tv_usec*0.000001-time_pre);
			if(*recv_rate <0)
			{
				*recv_rate = 0;
			} 
			//break;
		}
#if 1
		/*得到的字符串中的第十个字段是发送流量*/
		if(i == 10)
		{
			send = atoll(ifconfig_value);
			*send_rate= (send - send_pre)/(tv_now.tv_sec+tv_now.tv_usec*0.000001-time_pre);
			break;
		}
#endif

	}
	time_pre= tv_now.tv_sec+tv_now.tv_usec*0.000001;
	recv_pre = recv;
	send_pre = send;
	fclose(fd);
	return 0;
}
#endif

// 查询重要的表单数据
int DbTblChk(sqlite3 *db)
{
	unsigned char i = 0;
	int result = 0;
	char **dbResult = NULL;
	char *err_msg = NULL;
   	int nRow = 0, nColumn = 0;
	// 打开system.db中的一些比较重要的表单    
	// 如果修改了这些表单名称，请及时修改此处对应的表单名称。
	char *sqlcmd[DB_BK_TABLE_NUM] = {
	"select * from userInfo", 
	"select * from SystemInfo",
	"select * from networkInfo",
	"select * from netManageInfo",
	"select * from CommCfgInfo",
	"select * from RecordChannelTime",
	"select * from RecordChannelInfo"
	};
	char sqlcmd1[256] = {0};
	pthread_mutex_lock(&databaseMutex);
  	// 读数据库
	for(i = 0; i < DB_BK_TABLE_NUM; i++)
	{
		result = sqlite3_get_table(db, sqlcmd[i], &dbResult, &nRow, &nColumn, &err_msg);
		if(SQLITE_OK != result)//查询某个表单失败，恢复数据库
		{
			sqlite3_free_table(dbResult);
			pthread_mutex_unlock(&databaseMutex);
			Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
			return DB_OPR_ERROR;
		}
		sqlite3_free_table(dbResult);
	}
   	// 写数据库
	memset(sqlcmd1,0,sizeof(sqlcmd1));
	sprintf(sqlcmd1,"update DbBkTestTbl set  BackupTest='%s' where idx=0","Backup Test!");
	if(SQLITE_OK != sqlite3_exec(db, sqlcmd1, NULL, NULL, &err_msg))
	{
		Printf("err_msg = %s %s %d\n",err_msg,__FILE__,__LINE__);
		pthread_mutex_unlock(&databaseMutex);
		return DB_OPR_ERROR;
	}
	pthread_mutex_unlock(&databaseMutex);
	return 0;
}

// 数据库备份
int DbBackup(void)
{
	sqlite3 *db;
	int ret = 0;
	if(SQLITE_OK != sqlite3_open(DATABASE_FILE, &db))//数据库打开失败,恢复数据库。
	{
		sqlite3_close(db);
		pthread_mutex_lock(&databaseMutex);
		system("cp /config/system.db /data -f");
		pthread_mutex_unlock(&databaseMutex);
		Printf("Database is damaged, and have recovered! %s  %d \n",__FILE__,__LINE__);
	}
	else
	{
		ret = DbTblChk(db);
		sqlite3_close(db);
		if(ret == DB_OPR_ERROR) //数据库损坏，恢复数据库
		{	
			pthread_mutex_lock(&databaseMutex);
			system("cp /config/system.db /data -f");
			pthread_mutex_unlock(&databaseMutex);
			Printf("Database is damaged, and have recovered!  %s  %d \n",__FILE__,__LINE__);
		}
		else
		{
			pthread_mutex_lock(&databaseMutex);
			system("cp /data/system.db /config -f");//数据库正常,备份当前的数据库。
			pthread_mutex_unlock(&databaseMutex);
			Printf("Database is OK, backup!! %s  %d \n",__FILE__,__LINE__);
		}
		
	}
	return 0;
}

int  DiskQuery(void *pRecvData)
{
	int ret = 0;
	sdk_disk_t *pDisk = (sdk_disk_t *)pRecvData;
	sata_info_t pSataInfo = { 0 };
	int i = 0;
	findSataDisk( &pSataInfo );
	for ( i = 0; i < pSataInfo.diskNum; i++)
	{
		printf("sataPort %d devName %s size %llu G \n", 
				pSataInfo.sataInfo[i].sataPort, pSataInfo.sataInfo[i].devName, pSataInfo.sataInfo[i].totalSize / 1000 / 1000 / 1000);
		pDisk->disk_no = pSataInfo.sataInfo[i].sataPort;
		pDisk->disk_type = (pDisk->disk_no >= 10 ) ? 1 : 0;
		pDisk->status = SDK_DISK_STATUS_DISK_NOMAL;
		pDisk->total_size = (int)(pSataInfo.sataInfo[i].totalSize / 1000 / 1000 );
		if(pDisk->disk_no >= 10)
		{						
			pDisk->free_size = GetDiskFreeSize(pSataInfo.sataInfo[i].devName);
		}
		else
		{
			pDisk->status = recordGetDiskUsedStatus(pDisk->disk_no - 1);
			pDisk->free_size = recordGetFreePercent(pDisk->disk_no - 1) * pDisk->total_size / 1000;
		}
		pDisk->is_backup = 0;
		pDisk->is_raid = 0;	
		pDisk++;
	}

	ret = pSataInfo.diskNum * sizeof(sdk_disk_t); 
	return ret;
}


