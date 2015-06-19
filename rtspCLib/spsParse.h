#ifndef __SPSPARSE_H
#define __SPSPARSE_H

typedef unsigned int UINT;
typedef unsigned char  BYTE;

int procSPS(const char *pBase64Sps, unsigned int inLen, int *width, int *height);
int h264_sps(const char *pBase64Sps, int *width, int *height, int *framerate);


#endif
