CROSS=arm-hisiv100nptl-linux-
CC=$(CROSS)gcc
AR=$(CROSS)ar
STRIP=$(CROSS)strip

INC += -I$(shell pwd)/include
SRC += $(wildcard $(shell pwd)/*.c)
#CFLAGS += -Wall -g $(INC) -D$(PLATFORM)
CFLAGS += -Wall -g $(INC)

TARGET=libptz.a
PTZDEMO=demo
all:$(TARGET)
OBJS = $(patsubst %.c,%.o,$(SRC))
$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<
	

$(TARGET):$(OBJS)
	$(CC) -o $(PTZDEMO) $(OBJS)
	$(AR) -rc $(TARGET) $(OBJS)
	chmod 777 $(TARGET)
#	cp -f $(TARGET) ../common/lib/$(TARGET)

clean:
	rm -f *.o; rm -f *.a;
