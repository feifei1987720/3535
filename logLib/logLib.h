#ifndef __LOGLIB_H__
#define __LOGLIB_H__

//#define printf(args...) dump_print(stderr,__FILE__,__LINE__,__FUNCTION__,##args)
#define st_printf(fmt,args...) dump_printf(__FILE__,__LINE__,fmt,##args)

int logLibInit(void);
void dump_printf(char *file,int line,char *fmt,...);
//void st_printf(char *fmt,...);
//void dump_print(FILE *fp,const char *file_name,const int line,const char *function_name,char *fmt,...);

#endif//__LOGLIB_H__

