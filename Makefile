
include Makefile.param

CP = cp -fr
RM = rm -f

SUBDIRS += appLib
SUBDIRS += procMsgLib
#SUBDIRS += diskLib
#SUBDIRS += codeLib
#SUBDIRS += commonLib
#SUBDIRS += myThreadLib
#SUBDIRS += networkLib
#SUBDIRS += queueLib
#SUBDIRS += recordLib
#SUBDIRS += rtspCLib
#SUBDIRS += rtspSLib
#SUBDIRS += statusLib
#SUBDIRS += streamLib
SUBDIRS += databaseLib
#SUBDIRS += playbackLib
#SUBDIRS += templateLib
ifneq ($(CC), gcc)
#SUBDIRS += decodeLib
#SUBDIRS += hiLib
endif

SUBDIRS += boaLib

all: inc $(SUBDIRS)
	@make -C demo
	cp demo/demo /opt/work/hisi_rootfiles/ -f

inc:
#	@$(foreach SUBDIR, $(SUBDIRS), cp $(SUBDIR)/$(SUBDIR).h ./include/ ;)

$(SUBDIRS):
	@make -C $@ > /dev/NULL

clean:
	@$(foreach SUBDIR, $(SUBDIRS), make clean -C $(SUBDIR) clean > /dev/NULL ;)
	@make -C demo clean > /dev/NULL

.PHONY: all clean $(SUBDIRS)

