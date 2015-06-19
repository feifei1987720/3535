#ifndef DISCOVERY_H
#define DISCOVERY_H

#ifdef __cplusplus
extern "C"
{
#endif

/*DSC 是 discovery 的缩写*/
/*加密秘钥*/
#define DSC_ENCODE_KEY                      "13141314"
/*广播端口*/
#define DSC_BROADCAST_PORT                  6789
/*校验信息*/
#define DSC_CHECKCODE                       "searching.."
#define DSC_CHECKCODE_LEN                   12
/*消息类型*/
#define DSC_OMT_SEARCH_RESP                 1
#define DSC_OMT_SEARCH                      2
#define DSC_OMT_MODIFY                      3
#define DSC_OMT_MODIFY_RESP                 4

#define DSC_MT_SEARCH                       10
#define DSC_MT_SEARCH_RESP                  11
#define DSC_MT_MODIFY                       12
#define DSC_MT_MODIFY_RESP                  14
/*设备类型*/
#define DSC_DEVTYPE_DVR                     "DVR"
#define DSC_DEVTYPE_IPCAM                   "IPCAMERA"
#define DSC_DEVTYPE_DECODER                 "DECODER"
#define DSC_DEVTYPE_NVR                     "NVR"
/*其他*/
#define DSC_MESTYPE_OFFSET                  DSC_CHECKCODE_LEN
#define DSC_MESSAGE_MAXLEN                  1024
#define DSC_DEVTYPE_MAXLEN                  16
#define DSC_DEVNAME_MAXLEN                  64
#define DSC_VERSION_MAXLEN                  64
#define DSC_IPV4ADDR_LEN                    16
#define DSC_MACADDR_LEN                     18
#define DSC_USERNAME_MAXLEN                 32
#define DSC_PASSWORD_MAXLEN                 32

#define DSC_SOFTWARE_VERSION_MAXLEN         64
#define DSC_FIREWARE_VERSION_MAXLEN         64

/*平台类型*/
typedef enum DSC_PLATFORM_TYPE
{
    DSC_PLATFORM_TIIPC_222	            = 0,
    DSC_PLATFORM_TIIPC_238,

    DSC_PLATFORM_HISIPC_3516C_IMX222	=100,
    DSC_PLATFORM_HISIPC_3518C_IMX238,
    DSC_PLATFORM_HISIPC_3518C_OV9712S,
    DSC_PLATFORM_HISIPC_3518E_OV9712S,

    DSC_PLATFORM_HISNVR_3535	        =200,
    DSC_PLATFORM_HISNVR_3520D,
    DSC_PLATFORM_HISNVR_3521,
    DSC_PLATFORM_HISNVR_3531,

    DSC_PLATFORM_ABIPC_A5s66_1080P      = 300,
    DSC_PLATFORM_ABIPC_A5s55_960P,
    DSC_PLATFORM_ABIPC_A5s66_1080P_BOX,
    DSC_PLATFORM_ABIPC_A5s66_1080P_PTZ,
}DSC_PLATFORM_TYPE;

/*错误码*/
typedef enum DSC_ERRORCODE_TYPE{
    DSC_ERRORCODE_SUCCESS               =  0x00,           /*成功*/
    DSC_ERRORCODE_VERSION_ERROR,                           /*版本错误*/

    DSC_ERRORCODE_USERPASSWD_ERROR      =  0x10,           /*错误的用户名密码*/
    DSC_ERRORCODE_ID_ERROR,                                /*用户未登录*/
    DSC_ERRORCODE_ID_LIMITED,                              /*用户权限不足*/

    DSC_ERRORCODE_CMD_NOSUPPORT         =  0x20,           /*命令不支持*/
    DSC_ERRORCODE_ARGE_ERROR,                              /*参数错误*/
    DSC_ERRORCODE_ARGE_NOSUPPORT,                          /*参数不支持*/
}DSC_ERRORCODE_TYPE;

/*目标地址*/
typedef struct DSC_TERMINAL_INFO{
    unsigned char   ucIpAddr[DSC_IPV4ADDR_LEN];            /*ip地址*/
    unsigned char   ucMacAddr[DSC_MACADDR_LEN];           /*mac 地址: 例如 A1B2C3D4E5F6 前12位有效.剩下4位填0*/
}DSC_TERMINAL_INFO;

/*网络配置成员有效性枚举*/
typedef enum DSC_NETWORK_ISVALID{
    DSC_NETWORK_DHCP_VALID              = (1<<1),          /*dhcp*/
    DSC_NETWORK_IP_VALID                = (1<<2),          /*ip*/
    DSC_NETWORK_NETMASK_VALID           = (1<<3),          /*子网掩码*/
    DSC_NETWORK_GATEWAY_VALID           = (1<<4),          /*网关*/
    DSC_NETWORK_DNS1_VALID              = (1<<5),          /*DNS1*/
    DSC_NETWORK_DNS2_VALID              = (1<<6),          /*DNS2*/
    DSC_NETWORK_MAC_VALID               = (1<<7),          /*MAC*/
}DSC_NETWORK_ISVALID;


/*网络配置信息*/
typedef struct DSC_NETWORK_INFO{
    unsigned long   ulMemberIsValid;                       /*成员有效性*/
    unsigned char   ucDhcpEnable;                          /*dhcp是否启动: 0关闭, 1启动*/
    unsigned char   ucRes1[3];                             /*保留1*/
      unsigned char  ucIpAddr[DSC_IPV4ADDR_LEN];            /*ip地址*/
    unsigned char   ucNetmaskAddr[DSC_IPV4ADDR_LEN];       /*子网掩码*/
    unsigned char   ucGatewayAddr[DSC_IPV4ADDR_LEN];       /*网关地址*/
    unsigned char   ucDnsAddr1[DSC_IPV4ADDR_LEN];          /*dns1地址*/
    unsigned char   ucDnsAddr2[DSC_IPV4ADDR_LEN];          /*dns2地址*/
    unsigned char   ucMacAddr[DSC_MACADDR_LEN];            /*mac 地址: 例如 A1B2C3D4E5F6 前12位有效.剩下4位填0*/
    unsigned char   ucRes2[16];                            /*保留2*/
}DSC_NETWORK_INFO;

/*设备信息成员有效性枚举*/
typedef enum DSC_DEV_ISVALID{
    DSC_DEV_NAME_VALID                  = (1<<1),          /*设备名称*/
    DSC_DEV_HTTPPORT_VALID              = (1<<8),          /*http 端口*/
    DSC_DEV_RTSPPORT_VALID              = (1<<9),          /*设备名称*/
    DSC_DEV_TCPPORT_VALID               = (1<<10),         /*设备名称*/
    DSC_DEV_PLAYBACKPORT_VALID          = (1<<11),         /*设备名称*/
    DSC_DEV_PRIVATEPORT_VALID           = (1<<12),         /*设备名称*/
}DSC_DEV_ISVALID;
/*设备信息*/
typedef struct DSC_DEV_INFO{
    unsigned long   ulMemberIsValid;                       /*成员有效性*/
    unsigned char   ucDevName[DSC_DEVNAME_MAXLEN];         /*设备名称*/

    unsigned char   ucRes[32];                             /*保留*/
    unsigned long   ulHttpPort;                            /*http 端口*/
    unsigned long   ulRtspPort;                            /*rtsp 端口*/
    unsigned long   ulTcpPort;                             /*tcp  端口*/
    unsigned long   ulPlaybackPort;                        /*回放 端口*/
    unsigned long   ulPrivatePort;                         /*私有协议端口*/
    unsigned char   ucRes1[32];                            /*保留*/
}DSC_DEV_INFO;

/*平台信息成员有效性枚举*/
typedef enum DSC_PLATFORM_ISVALID{
    DSC_PLATFORM_CHANNEL_VALID           = (1<<1),          /*通道信息*/
    DSC_PLATFORM_TYPE_VALID              = (1<<2),          /*平台类型*/
    DSC_PLATFORM_DEV_TYPE_VALID          = (1<<3),          /*设备类型*/
    DSC_PLATFORM_SOFTWARE_VERSION_VALID  = (1<<4),          /*软件版本*/
    DSC_PLATFORM_FIRMWARE_VERSION_VALID  = (1<<5),          /*固件版本*/
}DSC_PLATFORM_ISVALID;

/*平台信息*/
typedef struct DSC_PLATFORM_INFO{
    unsigned long   ulMemberIsValid;                                               /*成员有效性*/
    unsigned char	ucChNum;													   /*通道数*/
    unsigned char   ucRes1[3];													   /*保留*/
    unsigned long   ucPlatformType;                                                /*平台类型: 填写 DSC_PLATFORM_XXX*/
    unsigned char   ucDevType[DSC_DEVTYPE_MAXLEN];                                 /*设备类型: 填写 DSC_DEVTYPE_XXX*/
    unsigned char   ucSoftwareVersion[DSC_SOFTWARE_VERSION_MAXLEN];                /*软件版本*/
    unsigned char   ucFirmwareVersion[DSC_FIREWARE_VERSION_MAXLEN];                /*固件版本*/
    unsigned char   ucRes2[32];													   /*保留*/
}DSC_PLATFORM_INFO;


/*用户信息*/
typedef struct DSC_USER_INFO{
    unsigned char   ucUsername[DSC_USERNAME_MAXLEN];       /*用户名*/
    unsigned char   ucPassword[DSC_PASSWORD_MAXLEN];       /*密码*/
}DSC_USER_INFO;

/*搜索结构体*/
typedef struct DSC_SEARCH_INFO{
    unsigned char            ucCheckCode[DSC_CHECKCODE_LEN];     /*校验码: 填写 DSC_CHECKCODE*/
    unsigned char            ucMesType;                          /*消息类型: 填写 DSC_MT_SEARCH*/
    unsigned char            ucRes1[3];                          /*保留1*/
    DSC_TERMINAL_INFO        srcInfo;                            /*源地址*/
    unsigned char            ucRes2[64];                         /*保留2*/
}DSC_SEARCH_INFO;

/*搜索结构体回复*/
typedef struct DSC_SEARCH_RESPONSE_INFO{
    unsigned char            ucCheckCode[DSC_CHECKCODE_LEN];     /*校验码: 填写 DSC_CHECKCODE*/
    unsigned char            ucMesType;                          /*消息类型: 填写 DSC_MT_SEARCH*/
    DSC_NETWORK_INFO         networkInfo;                        /*网络配置信息*/
    DSC_DEV_INFO             devInfo;                            /*设备信息*/
    DSC_PLATFORM_INFO        playformInfo;                       /*平台信息*/
    DSC_USER_INFO            userInfo;                           /*用户信息*/
}DSC_SEARCH_RESPONSE_INFO;

/*修改结构体*/
typedef struct DSC_MODIFY_INFO{
    unsigned char            ucCheckCode[DSC_CHECKCODE_LEN];     /*校验码: 填写 DSC_CHECKCODE*/
    unsigned char            ucMesType;                          /*消息类型: 填写 DSC_MT_SEARCH*/
    unsigned char            ucRes1[3];                          /*保留1*/
    DSC_TERMINAL_INFO        srcInfo;                            /*终端信息*/
    DSC_TERMINAL_INFO        dstInfo;                            /*终端信息*/
    DSC_NETWORK_INFO         networkInfo;                        /*网络配置信息*/
    DSC_DEV_INFO             devInfo;                            /*设备信息*/
    DSC_USER_INFO            securityInfo;                       /*安全信息*/
}DSC_MODIFY_INFO;

/*修改网络回复结构体*/
typedef struct DSC_MODIFY_RESPONSE_INFO{
    unsigned char           ucCheckCode[DSC_CHECKCODE_LEN];      /*校验码: 填写 DSC_CHECKCODE*/
    unsigned char           ucMesType;                           /*消息类型: 填写 DSC_MESTYPE_MODIFY_RESPONSE*/
    unsigned char           ucErrorCode;                         /*填写错误码*/
    unsigned char           ucRes1[2];                           /*保留1*/ 
    DSC_TERMINAL_INFO       dstInfo;                             /*终端信息*/
    DSC_NETWORK_INFO        networkInfo;                         /*网络配置信息*/
    DSC_DEV_INFO            devInfo;                             /*设备信息*/
}DSC_MODIFY_RESPONSE_INFO;

/********************************************************************************************/
/*******************************          OLD        ***************************************/
/********************************************************************************************/
/*搜索回复结构体*/
typedef struct DSC_SEARCH_RESPONSE_OINFO{
    unsigned char   ucCheckCode[DSC_CHECKCODE_LEN];        /*校验码: 填写 DSC_CHECKCODE*/
    unsigned char   ucMesType;                             /*消息类型: 填写 DSC_MESTYPE_SEARCH_RESPONSE*/

    unsigned char   ucDevType[DSC_DEVTYPE_MAXLEN];         /*设备乐享: 填写 DSC_DEVTYPE_XXX*/
    unsigned char   ucMacAddr[7];                          /*mac地址: 填写数字,第0-5位有效,第6位填0*/
    unsigned char   ucIpAddr[DSC_IPV4ADDR_LEN];            /*ip地址*/
    unsigned char   ucNetmaskAddr[DSC_IPV4ADDR_LEN];       /*子网掩码*/
    unsigned char   ucGatewayAddr[DSC_IPV4ADDR_LEN];       /*网关地址*/
    unsigned char   ucDnsAddr[DSC_IPV4ADDR_LEN];           /*dns地址*/

    unsigned char   ucDevName[DSC_DEVNAME_MAXLEN];         /*设备名称*/
    unsigned char   ucVersion[DSC_VERSION_MAXLEN];         /*版本信息*/
    unsigned long   ulDevModel;
    unsigned long   ulHttpPort;                            /*网页端口*/
    unsigned long   ulRtspPort;                            /*rtsp 端口*/
    unsigned long   ulMobilePort;
    unsigned long   ulTcpPort;                             /*tcp 端口*/
    unsigned long   ulPlaybackPort;                        /*回放 端口*/

    unsigned char   ucDhcpEnable;                          /*dhcp是否使能: 0关闭 1使能*/
    unsigned char   ucChNum;                               /*用户通道数: DVRNVR 填具体通道数量,IPC填写 1*/
    unsigned char   ucRes1[2];                             /*保留*/
    DSC_USER_INFO   userInfo;                              /*用户信息*/
}DSC_SEARCH_RESPONSE_OINFO;

/*修改网络结构体*/
typedef struct DSC_MODIFY_OINFO{
    unsigned char   ucCheckCode[DSC_CHECKCODE_LEN];        /*校验码: 填写 DSC_CHECKCODE*/
    unsigned char   ucMesType;                             /*消息类型: 填写 DSC_MESTYPE_MODIFY*/

    unsigned char   ucIpAddr[DSC_IPV4ADDR_LEN];            /*ip地址*/
    unsigned char   ucIpAddrNew[DSC_IPV4ADDR_LEN];         /*新的ip地址*/
    unsigned char   ucNetmaskAddr[DSC_IPV4ADDR_LEN];       /*子网掩码*/
    unsigned char   ucGatewayAddr[DSC_IPV4ADDR_LEN];       /*网关地址*/
    unsigned char   ucDnsAddr[DSC_IPV4ADDR_LEN];           /*dns地址*/
    unsigned char   ucMacAddr[7];                          /*mac地址: 填写数字,第0-5位有效,第6位填0*/

    unsigned long   ulHttpPort;                             /*网页端口*/
    unsigned long   ulRtspPort;                            /*rtsp 端口*/
    unsigned long   ulMobilePort;
    unsigned char   ucDhcpEnable;                          /*dhcp是否使能: 0关闭 1使能*/
    unsigned char   ucRes1[2];                             /*保留*/

    unsigned char   ucResetVieoConfig;                     /*重置视频配置: 0不重置 1重置*/
    DSC_USER_INFO   userInfo;                              /*用户信息*/
}DSC_MODIFY_OINFO;


typedef struct 
{
      unsigned char  ucIpAddr[DSC_IPV4ADDR_LEN];    
      unsigned char ucErrorCode ;
	  int state ;
}NetConfigResponse;

#ifdef __cplusplus
}
#endif
#endif


/********************************************************************************************/
/*******************************       搜索规则        ***************************************/
/********************************************************************************************/
/* 通信规则:
 *    本搜索设备使用 UDP 广播包进行通信.
 *
 *    UDP包头前 12(DSC_CHECKCODE_LEN)个字节用于校验当前UDP包是否属于本搜索规则
 *    必须填写 "searching.." (DSC_CHECKCODE)
 *    服务器端:在 DSC_BROADCAST_PORT 建立UDP广播socket. 用于监听搜索或修改命令
 *
 * 搜索设备:
 *    客户端: 建立一个UDP广播socket. 端口可以为任意大于 1024 的数值
 *    往广播地址(255.255.255.255)的(DSC_BROADCAST_PORT)端口发送设备搜索包(DSC_SEARCH_INFO)
 *    DSC_SEARCH_INFO.ucMesType = DSC_MT_SEARCH;
 *    服务器端: 接收到设备搜索包校验包参数.
 *    往对端UDP端口广播一个设备搜索回复包 (DSC_SEARCH_RESPONSE_INFO)
 *    DSC_SEARCH_RESPONSE_INFO.ucMesType = DSC_MESTYPE_SEARCH_RESPONSE;
 *    DSC_SEARCH_RESPONSE_INFO 的userInfo.ucPassword 需要使用 des 加密.加密秘钥默认为 DSC_ENCODE_KEY
 *	  DSC_SEARCH_INFO.srcInfo 填写客户端的ip和mac地址. 用于过滤本设备自己发出的搜索包.
 *
 * 修改设备参数:
 *    客户端: 建立一个UDP广播socket. 端口可以为任意大于 1024 的数值
 *    往广播地址(255.255.255.255)的(DSC_BROADCAST_PORT)端口发送参数修改包(DSC_MODIFY_INFO)
 *    DSC_MODIFY_INFO.ucMesType = DSC_MESTYPE_MODIFY;
 *
 *    DSC_MODIFY_INFO.srcInfo 填写本机信息
 *    DSC_MODIFY_INFO.dstInfo 填写对端信息
 *    DSC_MODIFY_INFO.securityInfo 填写用户名与密码
 *    DSC_MODIFY_INFO.networkInfo 与 DSC_MODIFY_INFO.devInfo 为可以修改的选项
 *
 *    DSC_NETWORK_INFO.ulMemberIsValid 表示成员是否有效.
 *    如果你只想修改ip地址.可以 DSC_NETWORK_INFO.ulMemberIsValid |= DSC_NETWORK_IP_VALID;
 *    这个表示这个网络结构体里只有 ip 成员是有效的.
 *    如果 DSC_NETWORK_INFO.ulMemberIsValid = DSC_NETWORK_IP_VALID | DSC_NETWORK_DHCP_VALID;
 *    表示 IP 与 dhcp 同时有效.
 *    DSC_DEV_INFO 与 DSC_PLATFORM_INFO 有相似的用法.
 *
 *    服务器端: 接参数修改包, (对比目标ip地址与mac地址是否为本机ip与mac地址.如果不是忽略这个包.)
 *    往对端UDP端口广播一个设备搜索回复包 (DSC_MODIFY_RESPONSE_INFO)
 *    DSC_MODIFY_RESPONSE_INFO.ucMesType = DSC_MESTYPE_MODIFY_RESPONSE;
 *    校验包参数:
 *       成功时:             DSC_MODIFY_RESPONSE_INFO.ucErrorCode = DSC_ERRORCODE_SUCCESS.
 *                          根据包内容修改本机参数.
 *       失败时:             DSC_MODIFY_RESPONSE_INFO.ucErrorCode =  具体错误码.
 *
 *
 *
 *
 *
*/














