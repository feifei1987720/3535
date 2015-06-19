/*-------------------------------------------------------
      Data Encryption Standard  56位密钥加密64位数据 
--------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include "desTables.h"
/*-------------------------------
 把DatIn开始的长度位Len位的二进制
 复制到DatOut后
--------------------------------*/
void BitsCopy(DesBool *DatOut,DesBool *DatIn,int Len)     // 数组复制 OK 
{
	int i=0;
	for(i=0;i<Len;i++)
	{
		DatOut[i]=DatIn[i];
	}
}

/*-------------------------------
 字节转换成位函数 
 每8次换一个字节 每次向右移一位
 和1与取最后一位 共64位 
--------------------------------*/
void ByteToBit(DesBool *DatOut,char *DatIn,int Num)       // OK
{
	int i=0;
	for(i=0;i<Num;i++)
	{
		DatOut[i]=(DatIn[i/8]>>(i%8))&0x01;   
	}                                       
}

/*-------------------------------
 位转换成字节函数
 字节数组每8次移一位
 位每次向左移 与上一次或   
---------------------------------*/
void BitToByte(char *DatOut,DesBool *DatIn,int Num)        // OK
{
	int i=0;
	for(i=0;i<(Num/8);i++)
	{
		DatOut[i]=0;
	} 
	for(i=0;i<Num;i++)
	{
		DatOut[i/8]|=DatIn[i]<<(i%8);	
	}		
}


/*----------------------------------
 二进制密文转换为十六进制
 需要16个字符表示
-----------------------------------*/
void BitToHex(char *DatOut,DesBool *DatIn,int Num)
{
	int i=0;
	for(i=0;i<Num/4;i++)
	{
		DatOut[i]=0;
	}
	for(i=0;i<Num/4;i++)
	{
		DatOut[i] = DatIn[i*4]+(DatIn[i*4+1]<<1)
					+(DatIn[i*4+2]<<2)+(DatIn[i*4+3]<<3);
		if((DatOut[i]%16)>9)
		{
			DatOut[i]=DatOut[i]%16+'7';       //  余数大于9时处理 10-15 to A-F
		}                                     //  输出字符 
		else
		{
			DatOut[i]=DatOut[i]%16+'0';       //  输出字符	   
		}
	}
	
}

/*---------------------------------------------
 十六进制字符转二进制
----------------------------------------------*/
void HexToBit(DesBool *DatOut,char *DatIn,int Num)
{
	int i=0;                        // 字符型输入 
	for(i=0;i<Num;i++)
	{
		if((DatIn[i/4])>'9')         //  大于9 
		{
			DatOut[i]=((DatIn[i/4]-'7')>>(i%4))&0x01;   			
		}
		else
		{
			DatOut[i]=((DatIn[i/4]-'0')>>(i%4))&0x01; 	
		} 
	}	
}

// 表置换函数  OK
void TablePermute(DesBool *DatOut,DesBool *DatIn,const char *Table,int Num)  
{
	int i=0;
	static DesBool Temp[256]={0};
	for(i=0;i<Num;i++)                // Num为置换的长度 
	{
		Temp[i]=DatIn[Table[i]-1];  // 原来的数据按对应的表上的位置排列 
	}
	BitsCopy(DatOut,Temp,Num);       // 把缓存Temp的值输出 
} 

// 子密钥的移位
void LoopMove(DesBool *DatIn,int Len,int Num) // 循环左移 Len数据长度 Num移动位数
{
	static DesBool Temp[256]={0};    // 缓存   OK
	BitsCopy(Temp,DatIn,Num);       // 将数据最左边的Num位(被移出去的)存入Temp 
	BitsCopy(DatIn,DatIn+Num,Len-Num); // 将数据左边开始的第Num移入原来的空间
	BitsCopy(DatIn+Len-Num,Temp,Num);  // 将缓存中移出去的数据加到最右边 
} 

// 按位异或
void Xor(DesBool *DatA,DesBool *DatB,int Num)           // 异或函数
{
	int i=0;
	for(i=0;i<Num;i++)
	{
		DatA[i]=DatA[i]^DatB[i];                  // 异或 
	}
} 

// 输入48位 输出32位 与Ri异或
void S_Change(DesBool DatOut[32],DesBool DatIn[48])     // S盒变换
{
	int i,X,Y;                                    // i为8个S盒 
	for(i=0,Y=0,X=0;i<8;i++,DatIn+=6,DatOut+=4)   // 每执行一次,输入数据偏移6位 
	{    										  // 每执行一次,输出数据偏移4位
		Y=(DatIn[0]<<1)+DatIn[5];                          // af代表第几行
		X=(DatIn[1]<<3)+(DatIn[2]<<2)+(DatIn[3]<<1)+DatIn[4]; // bcde代表第几列
		ByteToBit(DatOut,&S_Box[i][Y][X],4);      // 把找到的点数据换为二进制	
	}
}

// F函数
void F_Change(DesBool DatIn[32],DesBool DatKi[48])       // F函数
{
	static DesBool MiR[48]={0};             // 输入32位通过E选位变为48位
	TablePermute(MiR,DatIn,E_Table,48); 
	Xor(MiR,DatKi,48);                   // 和子密钥异或
	S_Change(DatIn,MiR);                 // S盒变换
	TablePermute(DatIn,DatIn,P_Table,32);   // P置换后输出
}



void SetKey(char *KeyIn)               // 设置密钥 获取子密钥Ki 
{
	int i=0;
	static DesBool KeyBit[64]={0};                // 密钥二进制存储空间 
	static DesBool *KiL=&KeyBit[0],*KiR=&KeyBit[28];  // 前28,后28共56
	ByteToBit(KeyBit,KeyIn,64);                    // 把密钥转为二进制存入KeyBit 
	TablePermute(KeyBit,KeyBit,PC1_Table,56);      // PC1表置换 56次
	for(i=0;i<16;i++)
	{
		LoopMove(KiL,28,Move_Table[i]);       // 前28位左移 
		LoopMove(KiR,28,Move_Table[i]);	      // 后28位左移 
	 	TablePermute(SubKey[i],KeyBit,PC2_Table,48); 
	 	// 二维数组 SubKey[i]为每一行起始地址 
	 	// 每移一次位进行PC2置换得 Ki 48位 
	}		
}

void DesEncode(char *MesOut,char *MesIn,char *MyKey)  // 执行DES加密
{                                           // 字节输入 Bin运算 Hex输出 
	int i=0;
	static DesBool MesBit[64]={0};        // 明文二进制存储空间 64位
	static DesBool Temp[32]={0};
	static DesBool *MiL=&MesBit[0],*MiR=&MesBit[32]; // 前32位 后32位 
	SetKey(MyKey);               // 设置密钥 得到子密钥Ki
	ByteToBit(MesBit,MesIn,64);                 // 把明文换成二进制存入MesBit
	TablePermute(MesBit,MesBit,IP_Table,64);    // IP置换 
	for(i=0;i<16;i++)                       // 迭代16次 
	{
		BitsCopy(Temp,MiR,32);            // 临时存储
		F_Change(MiR,SubKey[i]);           // F函数变换
		Xor(MiR,MiL,32);                  // 得到Ri 
		BitsCopy(MiL,Temp,32);            // 得到Li 
	}					
	TablePermute(MesBit,MesBit,IPR_Table,64);
	BitToHex(MesOut,MesBit,64);
}

void DesDecode(char *MesOut,char *MesIn,char *YourKey)       // 执行DES解密
{												// Hex输入 Bin运算 字节输出 
	int i=0;
	static DesBool MesBit[64]={0};        // 密文二进制存储空间 64位
	static DesBool Temp[32]={0};
	static DesBool *MiL=&MesBit[0],*MiR=&MesBit[32]; // 前32位 后32位
	SetKey(YourKey);                                       // 设置密钥
	HexToBit(MesBit,MesIn,64);                 // 把密文换成二进制存入MesBit
	TablePermute(MesBit,MesBit,IP_Table,64);    // IP置换 
	for(i=15;i>=0;i--)
	{
		BitsCopy(Temp,MiL,32);
		F_Change(MiL,SubKey[i]);
		Xor(MiL,MiR,32);
		BitsCopy(MiR,Temp,32);
	}	
	TablePermute(MesBit,MesBit,IPR_Table,64);
	BitToByte(MesOut,MesBit,64);		
} 