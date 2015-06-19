
#ifndef _LONGSE_DES_H_
#define _LONGSE_DES_H_

//定义部份类型
typedef unsigned char uint8;
typedef signed char int8;
typedef unsigned char BOOLEAN;

#define TRUE	1
#define FALSE	0

//为了提高程序效率，把这两个位操作功能定义在宏。

//读取缓冲区的指定位.
#define PP_GET_BIT(p_array, bit_index)  \
            ((p_array[(bit_index) >> 3] >> (7 - ((bit_index) & 0x07))) & 0x01)

//设置缓冲区的指定位.
#define PP_SET_BIT(p_array, bit_index, bit_val) \
            if (1 == (bit_val)) \
            {\
                p_array[(bit_index) >> 3] |= 0x01 << (7 - ((bit_index) & 0x07));\
            }\
            else\
            {\
                p_array[(bit_index) >> 3] &= ~(0x01 << (7 - ((bit_index) & 0x07)));\
            }

//加解密标识，这两个标识涉及到对表的读取位置,
//必须保证DES_ENCRYPT = 0 DES_DECRYPT = 1
typedef enum
{
    DES_ENCRYPT = 0,
    DES_DECRYPT = 1
}PP_DES_MODE;

///////////////////////////////////////////////////////////////
//	函 数 名 : des
//	函数功能 : DES加解密
//	处理过程 : 根据标准的DES加密算法用输入的64位密钥对64位密文进行加/解密
//				并将加/解密结果存储到p_output里
//	时    间 : 2006年9月2日
//	返 回 值 :
//	参数说明 :	const char * p_data		输入, 加密时输入明文, 解密时输入密文, 64位(8字节)
//				const char * p_key		输入, 密钥, 64位(8字节)
//				char * p_output			输出, 加密时输出密文, 解密时输入明文, 64位(8字节)
//				uint8 mode				0 加密  1 解密
///////////////////////////////////////////////////////////////
void PP_Des(const char * p_data, const char * p_key, const char * p_output, PP_DES_MODE mode);


void PP_DES_Encode(char *outData, char *inData, char *key, int len);
void PP_DES_Decode(char *outData, char *inData, char *key, int len);


#endif //#ifndef _DES_H_
