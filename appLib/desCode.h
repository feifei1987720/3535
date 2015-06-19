/*
*******************************************************************************
*
**  : 
*******************************************************************************
*/

#ifndef __DES_CODE_H
#define __DES_CODE_H

#ifdef __cplusplus
extern "C" {
#endif
void DesEncode(char *MesOut,char *MesIn,char MyKey[8]);  // 执行DES加密
void DesDecode(char *MesOut,char *MesIn,char YourKey[8]);       // 执行DES解密
#ifdef __cplusplus
}
#endif


#endif//__DES_CODE_H
