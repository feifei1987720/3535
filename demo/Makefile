
include ../Makefile.param
TARGET = demo

#CFLAGS = -Wall -g $(INC_FLAGS) -D$(HIARCH) -DHICHIP=$(HICHIP) -DSENSOR_TYPE=$(SENSOR_TYPE) -D$(HIDBG) -D$(HI_FPGA) -D$(CHIP_TYPE) -lpthread -lm

INC_FLAGS := -I$(REL_INC)\
	     -I$(SDK_PATH)/sample/common\
	     -I$(SDK_PATH)/component/hifb/include 
INC_FLAGS += -I$(REL_INC) -I$(SDK_PATH)/$(EXTDRV)/tw2865 -I$(SDK_PATH)/$(EXTDRV)/tw2960 -I$(SDK_PATH)/$(EXTDRV)/tlv320aic31 -I$(SDK_PATH)/$(EXTDRV)/tlv320aic23

CFLAGS := -Wall -g -O2 -fPIC #-static -pthread
#CFLAGS := -Wall -g  -fPIC #-static -pthread
#CFLAGS += $(INC_FLAGS) -D$(HIARCH) -DHICHIP=$(HICHIP) -DSENSOR_TYPE=$(SENSOR_TYPE) -D$(HIDBG) -D$(HI_FPGA) #-D$(CHIP_TYPE)
CFLAGS += $(INC_FLAGS) -D$(HIARCH) -D$(HIDBG) -D$(HI_FPGA) -D$(CPU) 

ifeq ($(HIARCH),hi3531)
CFLAGS += -march=armv7-a -mcpu=cortex-a9 -mfloat-abi=softfp -mfpu=vfpv3-d16
CXXFlAGS +=-march=armv7-a -mcpu=cortex-a9 -mfloat-abi=softfp -mfpu=vfpv3-d16
endif



LIB_PATH = -L../lib
LIB_PATH += -L../sqlite
LIB_PATH += -L../onvif
LIB_PATH += -L../libevent/lib

INCLUDE_PATH += -I../include/ -I./ -I../
INCLUDE_PATH += -I../libevent/include/
INCLUDE_PATH += -I../appLib   -I../appLib/ddnsLib  -I../appLib/smtpLib  -I../appLibntpLib   -I../app/ptzLib/include  -I../app/ftpLib
INCLUDE_PATH += -I../codeLib
INCLUDE_PATH += -I../privateLib
INCLUDE_PATH += -I../myThreadLib
INCLUDE_PATH += -I../networkLib
INCLUDE_PATH += -I../queueLib
INCLUDE_PATH += -I../recordLib
INCLUDE_PATH += -I../rtspCLib
INCLUDE_PATH += -I../rtspSLib
INCLUDE_PATH += -I../playbackLib
INCLUDE_PATH += -I../streamLib
INCLUDE_PATH += -I../sqlite
INCLUDE_PATH += -I../databaseLib
INCLUDE_PATH += -I../procMsgLib
INCLUDE_PATH += -I../commonLib
INCLUDE_PATH += -I../diskLib
INCLUDE_PATH += -I../myEvLib
INCLUDE_PATH += -I../guiServerLib
INCLUDE_PATH += -I../gpioLib
INCLUDE_PATH += -I../logLib
INCLUDE_PATH += -I../onvif
INCLUDE_PATH += -I../aviLib
INCLUDE_PATH += -I../alarmLib
INCLUDE_PATH += -I../configLib
INCLUDE_PATH += -I../boaLib
INCLUDE_PATH += -I../boaLib/include
INCLUDE_PATH += -I../mempoolLib
INCLUDE_PATH += -I../phoneLib
INCLUDE_PATH += -I../28181Lib
INCLUDE_PATH += -I../decodeLib
INCLUDE_PATH += -I../hiLib
INCLUDE_PATH += -I../p2pLib

#plat
INCLUDE_PATH += -I../platLib/include/common/include
INCLUDE_PATH += -I../platLib/include/common/lib/ftp/include
INCLUDE_PATH += -I../platLib/include/interface/include
INCLUDE_PATH += -I../platLib/include/huashida/include
INCLUDE_PATH += -I../platLib/include/media_serv/include
INCLUDE_PATH += -I../platLib/include/platform_serv/include
INCLUDE_PATH += -I../platLib/include/rtsp_serv/include
INCLUDE_PATH += -I../platLib/include/ftp_serv/include
INCLUDE_PATH += -I../platLib/include/p2p_mobile/include
INCLUDE_PATH += -I../platLib/include/bcast_serv/include
INCLUDE_PATH += -I../platLib/include/media_serv/mobile_msg_push/include



LIB_FILE += -lconfig
LIB_FILE += -lserversdk
LIB_FILE += -ldecode
LIB_FILE += -lboa
LIB_FILE += -lapp
LIB_FILE += -lprivate
LIB_FILE += -lrecord
LIB_FILE += -ldatabase
LIB_FILE += -lnetwork
LIB_FILE += -lrtspC
LIB_FILE += -lrtspS
LIB_FILE += -lplayback
LIB_FILE += -lcode
LIB_FILE += -lstream
LIB_FILE += -lprocMsg
LIB_FILE += -lphone
LIB_FILE += -ldisk
LIB_FILE += -lgpio
LIB_FILE += -lcommon
#LIB_FILE += -lmyEv -levent
LIB_FILE += -llog
LIB_FILE += -lavi
LIB_FILE += -lqueue
LIB_FILE += -lonvifClient
LIB_FILE += -lm -lssl -lcrypto
LIB_FILE += -lalarm
#LIB_FILE += -laudio
LIB_FILE += -ltemplate
LIB_FILE += -lguiServer
LIB_FILE += -lmyThread
LIB_FILE += -lmempool
LIB_FILE += -l28181
LIB_FILE += -lsqlite3
LIB_FILE += -lhi
LIB_FILE += -lp2p

HI_LIB_FILE += $(REL_LIB)/libmpi.a 
HI_LIB_FILE += $(REL_LIB)/libhdmi.a 
HI_LIB_FILE += $(REL_LIB)/libVoiceEngine.a 
#HI_LIB_FILE += $(REL_LIB)/libaec.a 
HI_LIB_FILE += $(REL_LIB)/libresampler.a 
#HI_LIB_FILE += $(REL_LIB)/libanr.a 
HI_LIB_FILE += $(REL_LIB)/libjpeg.a
HI_LIB_FILE += $(AUDIO_LIBA) $(JPEGD_LIBA)

ifeq ($(HIARCH),hi3535)
HI_LIB_FILE += $(REL_LIB)/libvqe.a
HI_LIB_FILE += $(REL_LIB)/libresampler.a
else
HI_LIB_FILE += $(REL_LIB)/libaec.a
HI_LIB_FILE += $(REL_LIB)/libanr.a
endif


#plat
#PLAT_LIB_PATH = /home/workspace/3531/nvr/src/platLib/lib
#LIB_PATH += -L../platLib/
#LIB_PATH += -L../platLib/lib/
#LIB_FILE += -lplat
#PLAT_LIB_FILE += $(PLAT_LIB_PATH)/libcommon.a  $(PLAT_LIB_PATH)/libcrypto.a  $(PLAT_LIB_PATH)/libftp.a  $(PLAT_LIB_PATH)/libinterface.a  $(PLAT_LIB_PATH)/lib_media_serv.a  $(PLAT_LIB_PATH)/libmxml.a  $(PLAT_LIB_PATH)/lib_platform_serv.a  $(PLAT_LIB_PATH)/libssl.a  $(PLAT_LIB_PATH)/libz.a


LIB_FILE += -lrt -lpthread -lm -ldl

all:
	$(CC) $(CFLAGS) -o $(TARGET) main.c $(INC_FLAGS) $(INCLUDE_PATH) $(LIB_PATH) $(LIB_FILE) $(HI_LIB_FILE) $(PLAT_LIB_FILE)
	$(CC) $(CFLAGS) -o startUsbMouse startUsbMouse.c $(INC_FLAGS) $(INCLUDE_PATH) $(LIB_PATH) -lcommon -lconfig  -llog -lgpio 
	$(CC) $(CFLAGS) -o test test.c $(INC_FLAGS) $(INCLUDE_PATH) $(LIB_PATH) -lcommon -llog -lgpio -lconfig
	$(CC) $(CFLAGS) -o myWatchDog myWatchDog.c ../appLib/watchdogLib.c $(INC_FLAGS) $(INCLUDE_PATH) $(LIB_PATH) -lpthread -lcommon  -lconfig -llog
	$(CC) $(CFLAGS) -o init init.c $(INC_FLAGS) $(INCLUDE_PATH) $(LIB_PATH) $(LIB_FILE) $(HI_LIB_FILE) $(PLAT_LIB_FILE)
	$(CC) $(CFLAGS) -o fallocate fallocate.c $(INC_FLAGS) $(INCLUDE_PATH) $(LIB_PATH) $(LIB_FILE) $(HI_LIB_FILE) $(PLAT_LIB_FILE)

clean:
	$(RM) $(TARGET) *.o 


