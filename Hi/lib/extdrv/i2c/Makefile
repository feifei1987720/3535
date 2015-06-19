ifeq ($(PARAM_FILE), )
	PARAM_FILE:=../../Makefile.param
	include $(PARAM_FILE)
endif

obj-m := i2c.o
hi_i2c-y += i2c.o

default:	
	$(CC) -g -Wall -o i2c_read i2c_read.c    strfunc.c
	$(CC) -g -Wall -o i2c_write i2c_write.c  strfunc.c
	@$(MAKE) -C $(LINUX_ROOT) M=$(PWD) modules 
	#@cp i2c_read i2c_write $(REL_KO)
	@rm -f *.o modules.* *.symvers *.mod.c
clean:
	@rm -f i2c_read i2c_write
	@make -C $(LINUX_ROOT) M=$(PWD) clean 
	@rm -rf $(REL_KO)/i2c_read 
	@rm -rf $(REL_KO)/i2c_write 
