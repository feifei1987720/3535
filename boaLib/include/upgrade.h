#define UCTRL_HEAD "MCTP/1.0"

#define RET_UPGRADE	8899

#define CORE_MTD	"/dev/mtd2"
#define APP_MTD		"/dev/mtd3"

typedef struct stHead
{
	char model[32];
	char version[16];
	int  core_len;
	int  app_len;
	unsigned short crc_core;
	unsigned short crc_app;
	char reserved[4];
}stHead;

int UpgradeHandle(int sock);
