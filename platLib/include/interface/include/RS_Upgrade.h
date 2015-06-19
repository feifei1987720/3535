/********************************************************

  Remote Upgrade
  mai 20110330

********************************************************/



#ifndef __RS_UPGRADE_H__
#define __RS_UPGRADE_H__

#if (HIDVR_DEV_TYPE == 0x9617)
#define UPGRADE_PACKET_MAGIC 0X96170000
#elif (HIDVR_DEV_TYPE == 0x9624) 
#define UPGRADE_PACKET_MAGIC 0X96240000
#elif (HIDVR_DEV_TYPE == 0x9625)
#define UPGRADE_PACKET_MAGIC 0X96250001
#elif (HIDVR_DEV_TYPE == 0x9626) 
#define UPGRADE_PACKET_MAGIC 0X96260001
#elif (HIDVR_DEV_TYPE == 0x9627)
#define UPGRADE_PACKET_MAGIC 0X96270000
#elif (HIDVR_DEV_TYPE == 0x9629) 
#define UPGRADE_PACKET_MAGIC 0X96290000
#else
#define UPGRADE_PACKET_MAGIC 0X08122515
#endif

/**/
#define GEN_DVR16H 	"DVR16H"
#define GEN_DVR8D1  "DVR08L"
#define GEN_DVR16D1 "DVR16L"
#define GEN_DVR8BD1 "DVR08BD"
#define GEN_DVR16BD1 "DVR16BD"
#define GEN_DVR8HD1 "DVR08HD"
#define GEN_DVR16HD1 "DVR16HD"
//#define GEN_DVR8HD1 "DVR8D1"
#define GEN_DVR4D1 	"DVR4D1"
#define GEN_DVR16CIF "DVR16C"
#define GEN_DVR8CIF "DVR08C"
#define GEN_DVR4CIF	"DVR04C"
#define GEN_NVR4D1N "NVR04D"
#define GEN_NVR4D1E "NVR04E"
#define GEN_NVR4D1M "NVR04M"
#define GEN_MDVR4D1 "MDVR4D1"
#define GEN_DEC04D	"DEC04D"

/**/
#define UPGRADE_MEMORYMAP_MAGIC 0xaa5555aa
#define UMM_STRUCT_VERSION      0x00000001
/**/
#define UB_BOOT_BIT             (0x1<<0)
#define UB_KERNEL_BIT           (0x1<<1)
#define UB_ROOTFS_BIT           (0x1<<2)
#define UB_APP_BIT              (0x1<<3)
#define UB_BOOT_SLAVE_BIT       (0x1<<4)
#define UB_KERNEL_SLAVE_BIT     (0x1<<5)
#define UB_ROOTFS_SLAVE_BIT     (0x1<<6)
#define UB_LOGO_BIT             (0x1<<7)
#define UB_WWW_BIT              (0x1<<8)
#define UB_FPGA_BIT             (0x1<<9)

/**/
#define SOFTWARE_VERSION_LEN        7/**/
#define MAX_DEVICENAME_LEN 128
typedef struct _upgradefileheaderentry_{
    /*
      4*/
    char version[(SOFTWARE_VERSION_LEN + 1 + 3) / 4 * 4];
    unsigned long start;/**/
    unsigned long len;/**/
}upgradefileheaderentry_t;

typedef struct _upgradefileheader_{
    unsigned long magic;/**/
    unsigned long crc;/*CRCCRC*/
    unsigned long version;/**/
    upgradefileheaderentry_t boot;
    upgradefileheaderentry_t kernel;
    upgradefileheaderentry_t rootfs;
    upgradefileheaderentry_t app;
    upgradefileheaderentry_t slave_boot;
    upgradefileheaderentry_t slave_kernel;
    upgradefileheaderentry_t slave_rootfs;
    upgradefileheaderentry_t logo;
}upgradefileheader_t;

typedef struct _Newupgradefileheader_{
    unsigned long magic;/**/
    unsigned long crc;/*CRCCRC*/
	unsigned short version;/**/
	unsigned short Language_flag; /*1:CHINESE ONLY 0:MLANG */ 
    upgradefileheaderentry_t boot;
    upgradefileheaderentry_t kernel;
    upgradefileheaderentry_t rootfs;
    upgradefileheaderentry_t app;
    upgradefileheaderentry_t slave_boot;
    upgradefileheaderentry_t slave_kernel;
    upgradefileheaderentry_t slave_rootfs;
    upgradefileheaderentry_t logo;
    upgradefileheaderentry_t www;
}Newupgradefileheader_t;

//FPGA 
typedef struct _FpgaNewupgradefileheader_{
    unsigned long magic;/**/
    unsigned long crc;/*CRCCRC*/
	unsigned short version;/**/
	unsigned short Language_flag; /*1:CHINESE ONLY 0:MLANG */ 
    upgradefileheaderentry_t boot;
    upgradefileheaderentry_t kernel;
    upgradefileheaderentry_t rootfs;
    upgradefileheaderentry_t app;
    upgradefileheaderentry_t slave_boot;
    upgradefileheaderentry_t slave_kernel;
    upgradefileheaderentry_t slave_rootfs;
    upgradefileheaderentry_t logo;
    upgradefileheaderentry_t www;
    upgradefileheaderentry_t fpga; 
}FpgaNewupgradefileheader_t;

typedef struct {
	unsigned int StructSize;
	char DeviceType[MAX_DEVICENAME_LEN];
}RS_UpgradeExtInfo_t;

typedef struct {
	upgradefileheader_t UpGradeHead;
	RS_UpgradeExtInfo_t UpgradeExtInfo;
}RS_UpgradeFileHeader_t;

typedef struct {
	Newupgradefileheader_t NewUpGradeHead;
	RS_UpgradeExtInfo_t UpgradeExtInfo;
}RS_NewUpgradeFileHeader_t;

//FPGA ,
typedef struct {
	FpgaNewupgradefileheader_t FpgaNewUpGradeHead;
	RS_UpgradeExtInfo_t UpgradeExtInfo;
	char reserver[48];
}RS_FpgaNewUpgradeFileHeader_t;

typedef struct {
    unsigned char flag;                 /* */
    unsigned char charpercent;          /* */ 
    unsigned char charfinishflg;        /* */ 
}UG_WriteFlashProgress_s;

#endif
