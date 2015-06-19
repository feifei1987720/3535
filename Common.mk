
#%.o:%.c
#	gcc -c -o $@ $<


#生成依赖 （暂时未用）
include $($(CSOURCE):.c=.d)
%.d:%.c
	set -e; rm -f $@; \
		$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
		sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
		rm -f $@.$$$$

#$(LIB_NAME): $(DEP) $(OBJECTS)

#生成静态库、动态库
all: $(LIB_NAME).so #$(LIB_NAME).a
#all: $(LIB_NAME).a
#	@cp -fr $(LIB_INCLUDE) ../include/

#生成动态库
$(LIB_NAME).so:$(OBJECTS) $(REL_LIB)/*.a
	$(CC) -shared -o $(LIB_NAME).so $(OBJECTS) $(REL_LIB)/*.a
	@cp -fr $(LIB_NAME).so ../lib/

#生成静态库
$(LIB_NAME).a:$(OBJECTS)
	@$(AR) -r $@ $(OBJECTS)
	@cp -fr $(LIB_NAME).a ../lib/

#清除
clean:
	rm *.o *.d $(LIB_NAME).a $(LIB_NAME).so -f


