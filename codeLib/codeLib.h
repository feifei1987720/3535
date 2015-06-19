/*
*******************************************************************************
**
**  : epoll,epoll
*******************************************************************************
*/

#ifndef __CODE_LIB_H__
#define __CODE_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif


/* codeLib.c */
/* G711A编码 */
int G711AEnc(short* pSrc, unsigned char* pDst, int len);
/* G711U编码 */
int G711UEnc(short* pSrc, unsigned char* pDst, int len);
/* G711A解码 */
int G711ADec(unsigned char* pSrc, short* pDst, int len);
/* G711U解码 */
int G711UDec(unsigned char* pSrc, short* pDst, int len);


/* base64.c */
/* base64编码 */
int codeBase64Enc(char *dest, const char *src, int count);
/* base64解码 */
int codeBase64Dec(char* dest,const char* src, int count);


/* md5.c */
#if 0
typedef struct
{
	unsigned long int i[2];/* number of _bits_ handled mod 2^64 */
	unsigned long int buf[4];/* scratch buffer */
	unsigned char in[64];/* input buffer */
	unsigned char digest[16];/* actual digest after MD5Final call */
}MD5_CTX;
void MD5Init ();
void MD5Update ();
void MD5Final ();
#else
/***********************************************/
/*               MD5 Base Define               */
/***********************************************/
/* modified for oSIP: GCC supports this feature */
#define PROTOTYPES 1

#ifndef PROTOTYPES
#define PROTOTYPES 0
#endif

/* POINTER defines a generic pointer type */
typedef unsigned char *POINTER;

/* UINT2 defines a two byte word */
typedef unsigned short int UINT2;

/* UINT4 defines a four byte word */
typedef unsigned int UINT4;

/* PROTO_LIST is defined depending on how PROTOTYPES is defined above.
If using PROTOTYPES, then PROTO_LIST returns the list, otherwise it
  returns an empty list.
 */
#if PROTOTYPES
#define PROTO_LIST(list) list
#else
#define PROTO_LIST(list) ()
#endif


/**
 * Structure for holding MD5 context.
 * @var MD5_CTX
 */
typedef struct
{
    UINT4 state[4];             /* state (ABCD) */
    UINT4 count[2];             /* number of bits, modulo 2^64 (lsb first) */
    unsigned char buffer[64];   /* input buffer */
}MD5_CTX;

void MD5Init PROTO_LIST ((MD5_CTX *));
void MD5Update PROTO_LIST ((MD5_CTX *, unsigned char *, unsigned int));
void MD5Final PROTO_LIST ((unsigned char[16], MD5_CTX *));

/***********************************************/
/*               MD5 Application               */
/***********************************************/
#define HASHLEN         16
#define HASHHEXLEN      32

typedef char HASH[HASHLEN];
typedef char HASHHEX[HASHHEXLEN + 1];

/* md5 auth algo type */
typedef enum
{
    EN_MD5AUTH_ALGO_MD5,
    EN_MD5AUTH_ALGO_SESS,
    EN_MD5AUTH_ALGO_INVALID
}EN_MD5AUTH_ALGO;

void MD5Auth_CvtHex( HASH bin, HASHHEX hex );
void MD5Auth_DigestCalcHA1(  const char* authId, const char* password,
                             unsigned char alg, const char* realm, const char* nonce,
                             const char* cnonce, HASHHEX sessionKey );
void MD5Auth_DigestCalcResponse( HASHHEX ha1, const char* nonce, const char* nc,
                                 const char* cnonce, const char* qop, const char* method,
                                 const char* uri, HASHHEX entity, HASHHEX response );
char* MD5Auth_BuildAuthResonseWithMD5( char* resp, unsigned short maxlen,
                                     unsigned char alg, const char* auth, const char* realm,
                                     const char* pass, const char* nonce, const char* nc,
                                     const char* cnonce, const char* qop,
                                     const char* method,const char*  uri);


#endif

/* changeLib.c */
/* 将Ip（XXX.XXX.XXX.XXX）转换为int型 */
int changeIpCharToInt(char *pIp);
/* 将int型IP, 转换为（XXX.XXX.XXX.XXX）*/
int changeIpIntToChar(int ip, char *pIp);




/* charCodeChannge.c */
/* gb2312转utf8 */
void changeGb2312ToUtf8(char *pOut,char *pText, int len);
/* utf8转gb2312 */
void changeUtf8ToGb2312(char *pOut, char *pText, int len);




#ifdef __cplusplus
}
#endif


#endif//__CODE_LIB_H__

