

int VideoMDGetStatus(int channel );
int VideoMDSetStatus(int channel,int status);
int VideoMDSetSensitivity(int channel,int value ) ;
int codeLib_VideoSetMotionBlockValue(unsigned char *pBlockValue, int dataLen) ;
HI_VOID *MotionResultThread(HI_VOID *pdata);
int motionSetUpdateFlag(int channel, HI_BOOL update);
int startMotion(void);
int stopMotion(void);


