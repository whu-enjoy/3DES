/////////////////////////////////////////////////////////////////////////////
//  文件名 : desDlg.cpp
//  工程 : des
//  作者 : enjoy5512   修改者 : enjoy5512   最后优化注释者 : enjoy5512
//  个人技术博客 : blog.csdn.net/enjoy5512
//  个人GitHub   : github.com/whu-enjoy
//  描述 : 对文件进行3DES加密的对话框主要实现文件,可以对文件进行加密,加密结果
//         保存在同目录下,后缀名改为ept,也可以对ept文件进行解密
//  主要函数 :
//      对话框响应函数 :
//	void cmCheck();                                       //检查文件后缀名,确定激活加密还是解密按钮
//	virtual BOOL OnInitDialog();                          //对话框初始化函数
//	afx_msg void OnSysCommand(UINT nID, LPARAM lParam); 
//	afx_msg void OnPaint();
//	afx_msg HCURSOR OnQueryDragIcon();
//	afx_msg void OnButtonOpen();                          //打开文件选择对话框响应函数
//	afx_msg void OnDropFiles(HDROP hDropInfo);            //支持文件拖放功能响应函数
//	afx_msg void OnButtonEncrypt();                       //加密函数
//	afx_msg void OnSetfocusEditPsw1();                    //密钥一EDIT框获取焦点响应函数
//	afx_msg void OnSetfocusEditPsw2();                    //密钥二EDIT框获取焦点响应函数
//	afx_msg void OnKillfocusEditPsw1();                   //密钥一EDIT框失去焦点响应函数
//	afx_msg void OnKillfocusEditPsw2();                   //密钥一EDIT框失去焦点响应函数
//	afx_msg void OnButtonDecrypt();                       //解密函数
//
//       DES类成员函数 :
//  void ByteToBin(const unsigned char c_ucaByte[], int iaBin[], const int c_iCount);  //字节转换为二进制
//  void BinToByte(const int c_iaBin[], unsigned char ucaByte[], const int c_iCount);  //二进制转换为字节
//  void Replacement(const int c_iaSource[], const int c_iaReplaceTable[], int iaReplaced[], const int c_iCount);  //矩阵置换
//  void LeftMove(const int c_ia28Input[28], int ia28Output[28], const int c_iCount);  //数组左移
//  void SubKey(const int c_ia64Pwd[64], int ia16_48K[16][48]);                        //子密钥产生函数
//  void F(const int c_ia32A[32], const int c_ia48K[48], int ia32Output[32]);          //加密函数F
//  void EncryptBlock(unsigned char uc9Data[9], const int c_ia16_48K[16][48], const bool c_bFlag);  //对8字节数据加解密
//  //文件加密
//  bool FileEncrypt(const CString c_csOpenFilePath, const CString c_csSaveFilePath, const unsigned char c_uca9Pwd1[9], const unsigned char c_uca9Pwd2[9]);
//  //文件解密
//  bool FileDecrypt(const CString c_csOpenFilePath, CString csSaveFilePath, const unsigned char c_uca9Pwd1[9], const unsigned char c_uca9Pwd2[9]);
//
//  版本 : 最终确定版  完成日期 : 2016年5月30日 21:35:18
//  修改 :
//  参考文献 :
//       <<密码学引论>> 第二版 张焕国 王张宜编著
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "des.h"
#include "desDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//  DES class used for Encrypt/Decrypt
class DES : public CDialog{
public:
	//公共静态成员数组
	static int s_ia56PC_1[56];             //子密钥产生算法中的PC1矩阵
	static int s_ia16MoveTimes[16];        //子密钥产生算法中的左移次数表
	static int s_ia48PC_2[48];             //子密钥产生算法中的PC2矩阵
	static int s_ia64IP[64];               //加密算法中的初始置换IP矩阵
	static int s_ia48E[48];                //加密算法中的扩展置换E矩阵
	static int s_ia8_4_16S_Box[8][4][16];  //加密算法中的S盒
	static int s_ia32P[32];                //加密算法中的P矩阵
	static int s_ia64IP_1[64];             //加密算法中的逆初始置换IP^-1矩阵

	int ia2_16_48K[2][16][48];             //子密钥

public:
	//程序说明开始
	//==================================================================================
	//  功能 : 将输入的无符号字符数组转化为相应的二进制,转换c_iCount个字节
	//  参数 : const unsigned char c_ucaByte[], int iaBin[], const int c_iCount
	//  (入口)  c_ucaByte   : 需要转换的无符号字符数组
	//          c_iCount   : 需要转换的字节数
	//  (出口)  iaBin : 转换后的二进制流,用整型数组保存结果
	//  返回 : 无
	//  主要思路 : 对每个字节循环八次,第j次循环左移j次,再将最高位提取出来,就能获取这个
	//             字节的八位上的所有数据
	//  调用举例 : unsigned char uca5Pwd[5] = "test";
	//             int ia32Bin[32];
	//             ByteToBin(uca5Pwd, ia32Bin, 4);
	//  日期 : 2016年5月30日 19:17:10(注释日期)
	//==================================================================================
	//程序说明结束
	void ByteToBin(const unsigned char c_ucaByte[], int iaBin[], const int c_iCount)
	{
		int i = 0;
		int j = 0;

		//循环转化c_iCount个字节
		for ( i = 0; i < c_iCount; i++) 
		{
			for(j = 0; j < 8; j++)  
			{
				//第j次循环时,左移j次,再检查最高位是否是1,如果是,则赋值为1,否则赋值为0
				if (0x80 == ((c_ucaByte[i]<<j)&0x80))
				{
					iaBin[i*8+j] = 1;
				}
				else
				{
					iaBin[i*8+j] = 0;
				}
			}
		} 
	}

	//程序说明开始
	//==================================================================================
	//  功能 : 将输入的二进制转化为相应的无符号字符数组,转换c_iCount个字节
	//  参数 : const int c_iaBin[], unsigned char ucaByte[], const int c_iCount
	//  (入口)  c_iaBin : 需要转换的二进制流
	//          c_iCount     : 需要转换的字节数
	//  (出口)  ucaByte       : 转换后的无符号字符数组
	//  返回 : 无
	//  主要思路 : 对每个字节循环八次,第j次循环将原来的值左移一位,并将新的值加到最低位
	//  调用举例 : unsigned char uca5Pwd[5] = "";
	//             int ia32Bin[32] = {0,0,1,1,0,0,0,1,0,0,1,1,0,0,1,0,0,0,1,1,0,0,1,1,0,0,1,1,0,1,0,0};
	//             BinToByte(ia32Bin, uca5Pwd, 4);
	//  日期 : 2016年5月30日 19:24:25(注释日期)
	//==================================================================================
	//程序说明结束
	void BinToByte(const int c_iaBin[], unsigned char ucaByte[], const int c_iCount)
	{
		int i = 0;
		int j = 0;

		//转换c_iCount个字节
		for ( i = 0; i < c_iCount; i++) 
		{
			for(j = 0; j < 8; j++)  
			{
				//每次将原来的值左移一位,并加上新提取的
				ucaByte[i] = ucaByte[i] * 2 + c_iaBin[i*8+j];
			}
		} 
	}

	//程序说明开始
	//==================================================================================
	//  功能 : 根据输入的第二个矩阵将第一个矩阵进行转换,转换的结果保存在第三个矩阵里,
	//         转换c_iCount个数据
	//  参数 : const int c_iaSource[], const int c_iaReplaceTable[]
	//         int iaReplaced[], const int c_iCount
	//  (入口)  c_iaSource         : 需要转换的矩阵
	//          c_iaDisplaceTable  : 转换参考矩阵
	//          c_iCount           : 需要转换的数据个数
	//  (出口)  iaReplaced         : 转换后的矩阵
	//  返回 : 无
	//  主要思路 : iaReplaced矩阵的第i个位置上的数是c_iaSource矩阵中
	//             第c_iaReplaceTable[i]个位置的数据
	//  调用举例 : int ia64Source[64] = {xxxxx};
	//             int ia48Replace[48] = {xxxx};
	//             int ia48Replaced[48] = {0};
	//             Replacement(ia64Source, ia48Replace, ia48Replaced, 48);
	//  日期 : 2016年5月30日 19:39:24(注释日期)
	//==================================================================================
	//程序说明结束
	void Replacement(const int c_iaSource[], const int c_iaReplaceTable[], int iaReplaced[], const int c_iCount)
	{
		int i = 0;
		
		//循环c_iCount次
		for (i = 0; i < c_iCount; i++)
		{
			//根据c_iaReplaceTable[]置换原表
			iaReplaced[i] = c_iaSource[c_iaReplaceTable[i]-1];
		}
	}

	//程序说明开始
	//==================================================================================
	//  功能 : 将输入矩阵里面的数据左移c_iCount次,用ia28Output保存左移后的结果
	//  参数 : const int c_ia28Input[28], int ia28Output[28], const int c_iCount
	//  (入口)  c_ia28Input        : 需要左移的数组
	//          c_iCount           : 需要左移的数据个数
	//  (出口)  ia28Output         : 左移后的数组
	//  返回 : 无
	//  主要思路 : 先将原数组前c_iCount个数据保存在局部变量i2Temp中,然后输出数组的
	//             前28-c_iCount数据左移c_iCount位,再将i2Temp中的数据接到输出数组中
	//             这样做的时候,输入数组盒输出数组可以是同一个数组,减少内存空间使用量
	//  调用举例 : int ia28C[28] = {xxxxx};
	//             LeftMove(ia28C, ia28C, 2);
	//  日期 : 2016年5月30日 19:49:46(注释日期)
	//==================================================================================
	//程序说明结束
	void LeftMove(const int c_ia28Input[28], int ia28Output[28], const int c_iCount)
	{
		int i2Temp[2] = {0}; //用于保存要移动的输入数组前c_iCount个变量
		int i = 0;

		//保存前c_iCount个变量
		for (i = 0; i < c_iCount; i++)
		{
			i2Temp[i] = c_ia28Input[i];
		}

		//将原数组后28-c_iCount个数前移c_iCount个位置
		for (i = 0; i < 28-c_iCount; i++)
		{
			ia28Output[i] = c_ia28Input[i+c_iCount];
		}

		//将原数组前c_iCount个数接到输出数组后
		for (; i < 28; i++)
		{
			ia28Output[i] = i2Temp[c_iCount + i - 28];
		}
	}

	//程序说明开始
	//==================================================================================
	//  功能 : 根据输入的64位密钥,输出16组48位的子密钥
	//  参数 : const int c_ia64Pwd[64], int ia16_48K[16][48]
	//  (入口)  c_ia64Pwd          : 输入64位密钥
	//  (出口)  ia16_64K           : 16组48位子密钥
	//  返回 : 无
	//  主要思路 : 先将64位密钥通过PC1矩阵得到56位密钥,再将密钥分为左右两部分,根据左移表
	//             循环左移16轮,每轮循环后合并成56位中间数据,然后在通过PC2矩阵得到48位子密钥
	//  调用举例 : int ia64Pwd[64] = {xxxxx};
	//             int ia16_48K[16][48] = {0}
	//             SubKey(ia64Pwd, ia16_48K);
	//  日期 : 2016年5月30日 19:58:32(注释日期)
	//==================================================================================
	//程序说明结束
	void SubKey(const int c_ia64Pwd[64], int ia16_48K[16][48])
	{
		int ia56Key[56] = {0};     //保存56位的密钥
		int ia28C[28] = {0};       //保存28位的密钥左部分
		int ia28D[28] = {0};       //保存28位的密钥右部分

		int i = 0;
		int j = 0;

		//先从给定的64位密钥中通过PC_1矩阵获取56位的密钥
		Replacement(c_ia64Pwd, s_ia56PC_1, ia56Key, 56); 

		//得到密钥左右部分
		for (i = 0; i < 28; i++)
		{
			ia28C[i] = ia56Key[i];
			ia28D[i] = ia56Key[28+i];
		}

		//循环获取16轮密钥
		for (i = 0; i < 16; i++)
		{
			//分别左移左右部分的密钥
			LeftMove(ia28C,ia28C,s_ia16MoveTimes[i]);
			LeftMove(ia28D,ia28D,s_ia16MoveTimes[i]);
			
			//密钥合并
			for (j = 0; j < 28; j++)
			{
				ia56Key[j] = ia28C[j];
				ia56Key[28+j] = ia28D[j];
			}

			//通过置换选择2矩阵获取每一轮产生的密钥
			Replacement(ia56Key,s_ia48PC_2,ia16_48K[i],48);
		}
	}

	//程序说明开始
	//==================================================================================
	//  功能 : 加密函数,在第i次迭代中用子密钥Ki对Ri-1进行加密
	//  参数 : const int c_ia32A[32], const int c_ia48K[48], int ia32Output[32]
	//  (入口)  c_ia32A          : 输入32位数据
	//          c_ia48K          : 48位子密钥
	//  (出口)  ia32Output       : 32位加密结果
	//  返回 : 无
	//  主要思路 : 先用选择运算矩阵E对输入数据进行选择和排列,输出48位中间结果,这个结果和
	//             48位子密钥异或,然后送入S盒,得到32位输出结果,这个结果再经过置换运算P,
	//             将每一位打乱重排,得到加密结果
	//  调用举例 : int ia32Pwd[32] = {xxxxx};
	//             int ia48K[48] = {xxx}
	//             int ia32Output[32] = {0};
	//             F(ia32Pwd, ia48K, iaOutput);
	//  日期 : 2016年5月30日 20:07:24(注释日期)
	//==================================================================================
	//程序说明结束
	void F(const int c_ia32A[32], const int c_ia48K[48], int ia32Output[32])
	{
		int ia48Temp[48] = {0};  //48位的中间数据

		int iRow = 0;            //S盒的行
		int iCol = 0;            //S盒的列
		int i = 0;

		//先将32位输入通过选择运算E扩展成48位
		Replacement(c_ia32A, s_ia48E, ia48Temp, 48);
		
		//48位中间结果与子密钥异或
		for (i = 0; i < 48; i++)
		{
			ia48Temp[i] = ia48Temp[i] ^ c_ia48K[i];
		}

		//循环从S盒中获取32位的输出结果
		for (i = 0; i < 8; i++)
		{
			//获取行列
			iRow = ia48Temp[i*6]*2 + ia48Temp[i*6+5];
			iCol = ia48Temp[i*6+1]*8 + ia48Temp[i*6+2]*4 + ia48Temp[i*6+3]*2 + ia48Temp[i*6+4];

			//获取S盒中的数据,并转化成四位输出
			ia48Temp[i*4+0] = (s_ia8_4_16S_Box[i][iRow][iCol]&8)/8;
			ia48Temp[i*4+1] = (s_ia8_4_16S_Box[i][iRow][iCol]&4)/4;
			ia48Temp[i*4+2] = (s_ia8_4_16S_Box[i][iRow][iCol]&2)/2;
			ia48Temp[i*4+3] = (s_ia8_4_16S_Box[i][iRow][iCol]&1);
		}

		//将S盒的32位输出通过P矩阵获取最终的32位输出
		Replacement(ia48Temp, s_ia32P, ia32Output, 32);
	}

	//程序说明开始
	//==================================================================================
	//  功能 : 对8字节的数据进行加解密,c_bFlag为false代表加密操作,为true代表解密操作
	//  参数 : unsigned char uc9PlainText[9], const int c_ia16_48K[16][48], const bool c_bFlag
	//  (入口)  c_ia_16_48K      : 16轮子密钥
	//          c_bFlag          : 加解密标志
	//  (出口)  uc9Data          : 32位加密结果
	//  返回 : 无
	//  主要思路 : 先将输入的八字节数据转化为64位二进制,然后将64位二进制明文用初始置换
	//             矩阵IP打乱,获取左右半部分,然后进行16轮加密,每轮加密中,下一轮的左部分
	//             是上一轮的右部分,然后用F函数实现密钥K对上一轮右部分加密,得到的结果再
	//             与上一轮的左部分异或,得到下一轮的右部分数据,这样循环16次.第十六次迭代
	//             结束后,将结果的左部分接到右部分上,得到64位的输出,然后将64位数据经过逆
	//             初始化矩阵IP^-1重新排列,得到最终密文
	//  调用举例 : unsigned char uc9Str = "12346578";
	//             int ia16_48K[16][48] = {xxx}
	//             EncryptBlock(uc9Str, ia16_48K, false);  //加密
	//             EncryptBlock(uc9Str, ia16_48K, true);   //解密
	//  日期 : 2016年5月30日 20:20:41(注释日期)
	//==================================================================================
	void EncryptBlock(unsigned char uc9Data[9], const int c_ia16_48K[16][48], const bool c_bFlag)
	{
		int ia64Data[64] = {0};       //加密的数据的64位二进制
		int ia64Bin[64] = {0};        //临时保存64位二进制数据
		int ia32L_BK[32] = {0};       //每轮加密的左部分
		int ia32R_BK[32] = {0};       //每轮加密的右部分
		int ia32L[32] = {0};          //每轮加密结果的左部分
		int ia32R[32] = {0};          //每轮加密结果的右部分

		int i = 0;
		int j = 0;

		//将输入的8字节转换成64位二进制
		ByteToBin(uc9Data, ia64Data, 8);

		//将64位二进制明文通过初始置换矩阵IP打乱
		Replacement(ia64Data,s_ia64IP,ia64Bin,64);

		//获取每轮加密的64位数据的左右部分
		for (i = 0; i < 32; i++)
		{
			ia32L_BK[i] = ia64Bin[i];
			ia32R_BK[i] = ia64Bin[32+i];
		}

		//16轮加密
		for (i = 0; i < 16; i++)
		{
			//获取每轮加密后的左部分
			for (j = 0; j < 32; j++)
			{
				ia32L[j] = ia32R_BK[j];
			}

			//如果c_bFlag为false,则加密,为true则解密
			if (0 == c_bFlag)
			{
				F(ia32R_BK,c_ia16_48K[i],ia32R);     //加密
			}
			else
			{
				F(ia32R_BK,c_ia16_48K[15-i],ia32R);  //解密
			}

			//获取每轮加密后的右部分
			for (j = 0; j < 32; j++)
			{
				ia32R[j] = ia32R[j] ^ ia32L_BK[j];
			}

			//保存当前左右部分的数据
			for (j = 0; j < 32; j++)
			{
				ia32L_BK[j] = ia32L[j];
				ia32R_BK[j] = ia32R[j];
			}
		}

		//16轮加密后,把加密结果的左部分接到右部分后面
		for (i = 0; i < 32; i++)
		{
			ia64Bin[i] = ia32R[i];
			ia64Bin[i+32] = ia32L[i];
		}

		//通过逆初始化矩阵,获取加密后的64位数据
		Replacement(ia64Bin, s_ia64IP_1, ia64Data, 64);

		//将64位数据转化为8位输出
		BinToByte(ia64Data, uc9Data, 8);
	}

	//程序说明开始
	//==================================================================================
	//  功能 : 对文件进行加密
	//  参数 : const CString c_csOpenFilePath, const CString c_csSaveFilePath
	//         const unsigned char c_uca9Pwd1[9], const unsigned char c_uca9Pwd2[9]
	//  (入口)  c_csOpenFilePath      : 要加密的文件路径
	//          c_csSaveFilePath      : 加密后的文件保存路径
	//          c_uca9Pwd1            : 密钥一
	//          c_uca9Pwd2            : 密钥二
	//  (出口)  无
	//  返回 : true代表加密成功, false代表打开加密文件或创建保存加密结果的文件失败
	//  主要思路 : 1.先打开文件指针
	//             2.根据输入的两个密钥,获取两组16*48位的子密钥
	//             3.获取加密文件长度,写入保存文件的前八字节
	//             4.获取加密文件类型,写入保存文件的9-16字节
	//             5.循环读取加密文件,每次读取八字节,文件尾不够的用0填充到8字节.
	//               在每轮循环里,先用子密钥一对数据加密,然后用密钥二对数据解密
	//               然后再用密钥一对数据加密,之后将加密后的数据写入文件
	//  调用举例 : CString csOpenFilePath = L"c:\test.txt";
	//             CString csSaveFilePath = L"c:\test.ept":
	//             unsigned char uca9Pwd1[9] = "12345678";
	//             unsigned char uca9Pwd2[9] = "98756412";
	//             DES CcmDes;
	//             CcmDes.FileEncrypt(csOpenFilePath, csSaveFilePath, uca9Pwd1, uca9Pwd2);
	//  日期 : 2016年5月30日 20:36:32(注释日期)
	//==================================================================================
	bool FileEncrypt(const CString c_csOpenFilePath, const CString c_csSaveFilePath, const unsigned char c_uca9Pwd1[9], const unsigned char c_uca9Pwd2[9])
	{
		CString csFileExt = L"";               //保存加密后的文件的后缀名
		unsigned char uc9PlainText[9] = {0};   //保存8字节的数据
		int ia64Bin[64] = {0};                 //保存64位中间结果

		bool bStatus = true;                   //设置返回状态为true

		FILE *pOpenFile = NULL;                //加密文件指针
		FILE *pSaveFile = NULL;                //加密结果文件指针

		int iCharNum = 0;                      //读取的字节数
		int i = 0;     
		int j = 0;
		int iFileLen = 0;                      //加密文件长度

		//打开加密文件指针
		if (NULL == (pOpenFile = fopen(c_csOpenFilePath,"rb")))
		{
			MessageBox("打开解密文件失败！");
			return false;
		}

		//打开加密后的文件指针
		if (NULL == (pSaveFile = fopen(c_csSaveFilePath,"wb")))
		{
			MessageBox("创建文件失败！");
			return false;
		}

		//将8字节密钥一转化为64位二进制
		ByteToBin(c_uca9Pwd1, ia64Bin, 8);
		
		//获取16轮子密钥一
		SubKey(ia64Bin,ia2_16_48K[0]);
		
		//将8字节密钥二转化为64位二进制
		ByteToBin(c_uca9Pwd2, ia64Bin, 8);

		//获取16轮子密钥2
		SubKey(ia64Bin,ia2_16_48K[1]);

		//将文件指针移动到文件尾
		fseek(pOpenFile, 0, SEEK_END);

		//获取文件长度
		iFileLen = ftell(pOpenFile);

		//将文件指针设置回文件头
		fseek(pOpenFile, 0, SEEK_SET);

		//将文件长度保存,为了满足八字节,所以写了两次
		fwrite(&iFileLen,4,1,pSaveFile);
		fwrite(&iFileLen,4,1,pSaveFile);

		//获取加密文件的后缀名
		csFileExt = c_csOpenFilePath.Mid(c_csOpenFilePath.ReverseFind('.')+1);

		//将加密文件后缀名写入加密后的文件,以便于解密时恢复源文件
		fwrite(csFileExt,1,8,pSaveFile);

		//设置读取的字节数为0
		iCharNum = 0;

		//循环读取加密文件,直到文件尾
		while(0 != (iCharNum = fread(uc9PlainText,1,8,pOpenFile)))
		{
			//如果读取到的不是8个字节,说明到文件尾了,填充0到八字节长
			if (8 != iCharNum)
			{
				for (i = iCharNum; i < 8; i++)
				{
					uc9PlainText[i] = 0;
				}
			}

			//用密钥一加密
			EncryptBlock(uc9PlainText, ia2_16_48K[0], false);
			//用密钥二解密
			EncryptBlock(uc9PlainText, ia2_16_48K[1], true);
			//再用密钥一加密
			EncryptBlock(uc9PlainText, ia2_16_48K[0], false);

			//将加密结果写入文件
			fwrite(uc9PlainText, 1, 8, pSaveFile);
		}

		//关闭文件指针,返回
		fclose(pOpenFile);
		fclose(pSaveFile);
		return bStatus;
	}

	//程序说明开始
	//==================================================================================
	//  功能 : 对文件进行解密
	//  参数 : const CString c_csOpenFilePath, CString csSaveFilePath
	//         const unsigned char c_uca9Pwd1[9], const unsigned char c_uca9Pwd2[9]
	//  (入口)  c_csOpenFilePath      : 要解密的文件路径
	//          c_uca9Pwd1            : 密钥一
	//          c_uca9Pwd2            : 密钥二
	//  (出口)  csSaveFilePath        : 解密后的文件保存路径
	//  返回 : true代表解密成功, false代表打开解密文件或创建保存解密结果的文件失败
	//  主要思路 : 1.先打开解密文件指针
	//             2.读取解密文件前4字节获取解密后的文件大小
	//             3.读取解密文件8-16字节获取解密后的文件类型
	//             4.创建解密后的文件
	//             5.根据输入的两个密钥,获取两组16*48位的子密钥
	//             8.循环读取解密文件,每次读取八字节,已解密的数据长度加8
	//               在每轮循环里,先用子密钥一对数据解密,然后用密钥二对数据加密
	//               然后再用密钥一对数据解密
	//               如果已解密的数据长度小于文件长,则写入后继续解密
	//               否则,写入数据到原文件长度(原文件并不一定是8字节对齐
  //               但是加密后的文件却填充到8字节对齐状态),并退出解密
	//  调用举例 : CString csOpenFilePath = L"c:\test.ept";
	//             CString csSaveFilePath = L"c:\test.":
	//             unsigned char uca9Pwd1[9] = "12345678";
	//             unsigned char uca9Pwd2[9] = "98756412";
	//             DES CcmDes;
	//             CcmDes.FileDecrypt(csOpenFilePath, csSaveFilePath, uca9Pwd1, uca9Pwd2);
	//  日期 : 2016年5月30日 20:49:46(注释日期)
	//==================================================================================
	bool FileDecrypt(const CString c_csOpenFilePath, CString csSaveFilePath, const unsigned char c_uca9Pwd1[9], const unsigned char c_uca9Pwd2[9])
	{
		char ca8FileExt[8] = {0};             //保存解密后的文件的后缀名
		unsigned char uc9PlainText[9] = {0};  //保存8字节数据
		int ia64PlainText[64] = {0};          //保存64位数据
		int ia64Bin[64] = {0};                //保存64为中间数据

		bool bStatus = true;                  //设置返回状态为true

		FILE *pOpenFile = NULL;               //要解密的文件指针
		FILE *pSaveFile = NULL;               //解密后的文件指针

		int iFileLen = 0;                     //文件长度
		int iDecryptedCharNum = 0;            //已解密的数据长度
		int i = 0;
		int j = 0;

		//打开要解密的文件指针
		if (NULL == (pOpenFile = fopen(c_csOpenFilePath,"rb")))
		{
			MessageBox("打开解密文件失败！");
			return false;
		}

		//读取要解密的文件头四个字节,获取解密后的文件的长度
		//读取两遍是为了8字节对齐
		fread(&iFileLen,4,1,pOpenFile);
		fread(&iFileLen,4,1,pOpenFile);

		//读取要解密的文件的9-16字节数据,获取解密后的文件后缀名
		fread(ca8FileExt,1,8,pOpenFile);

		//将后缀名连接到解密后的文件路径中
		csSaveFilePath = csSaveFilePath + ca8FileExt;

		//打开解密后的文件指针
		if (NULL == (pSaveFile = fopen(csSaveFilePath,"wb")))
		{
			MessageBox("创建文件失败！");
			return false;
		}

		//将密钥一转化为64位二进制
		ByteToBin(c_uca9Pwd1, ia64Bin, 8);

		//获取16轮子密钥一
		SubKey(ia64Bin, ia2_16_48K[0]);

		//将密钥二转化为64位二进制
		ByteToBin(c_uca9Pwd2, ia64Bin, 8);

		//获取16轮子密钥二
		SubKey(ia64Bin, ia2_16_48K[1]);
		
		//设置已解密的数据长度为0
		iDecryptedCharNum = 0;

		//循环解密
		while(1)
		{
			//读取8字节密文
			fread(uc9PlainText,1,8,pOpenFile);

			//已解密数据长度加8
			iDecryptedCharNum += 8;

			//用子密钥一解密
			EncryptBlock(uc9PlainText, ia2_16_48K[0], true);

			//用子密钥二加密
			EncryptBlock(uc9PlainText, ia2_16_48K[1], false);

			//用子密钥一解密
			EncryptBlock(uc9PlainText, ia2_16_48K[0], true);
			
			//如果已解密的数据长度小于文件长,则写入后继续解密
			//否则,写入数据到原文件长度(原文件并不一定是8字节对齐
			//但是加密后的文件却填充到8字节对齐状态),并退出解密
			if (iDecryptedCharNum < iFileLen)
			{
				fwrite(uc9PlainText, 1, 8, pSaveFile);
			}
			else
			{
				fwrite(uc9PlainText, 1, 8 + iFileLen - iDecryptedCharNum, pSaveFile);
				break;
			}
		}

		//关闭文件指针,并退出
		fclose(pOpenFile);
		fclose(pSaveFile);
		return bStatus;
	}
};

//子密钥产生算法中的置换选择2矩阵
int DES::s_ia56PC_1[56] = 
{
	   57, 49, 41, 33, 25, 17,  9,
		1, 58, 50, 42, 34, 26, 18,
	   10,  2, 59, 51, 43, 35, 27,
	   19, 11,  3, 60, 52, 44, 36,

	   63, 55, 47, 39, 31, 23, 15,
		7, 62, 54, 46, 38, 30, 22,
	   14,  6, 61, 53, 45, 37, 29,
	   21, 13,  5, 28, 20, 12,  4
};

//子密钥产生算法中左移位数表
int DES::s_ia16MoveTimes[16] = {1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};

//子密钥产生算法中的置换选择2矩阵
int DES::s_ia48PC_2[48] =
{
	   14, 17, 11, 24,  1,  5,
		3, 28, 15,  6, 21, 10,
	   23, 19, 12,  4, 26,  8,
	   16,  7, 27, 20, 13,  2,
	   41, 52, 31, 37, 47, 55,
	   30, 40, 51, 45, 33, 48,
	   44, 49, 39, 56, 34, 53,
	   46, 42, 50, 36, 29, 32
};

//初始置换IP矩阵
int DES::s_ia64IP[64] = 
{
	   58, 50, 42, 34, 26, 18, 10,  2,
	   60, 52, 44, 36, 28, 20, 12,  4,
	   62, 54, 46, 38, 30, 22, 14,  6,
	   64, 56, 48, 40, 32, 24, 16,  8,
	   57, 49, 41, 33, 25, 17,  9,  1,
	   59, 51, 43, 35, 27, 19, 11,  3,
	   61, 53, 45, 37, 29, 21, 13,  5,
	   63, 55, 47, 39, 31, 23, 15,  7
};

//扩展置换E矩阵
int DES::s_ia48E[48] =
{
	   32,  1,  2,  3,  4,  5,
		4,  5,  6,  7,  8,  9,
		8,  9, 10, 11, 12, 13,
	   12, 13, 14, 15, 16, 17,
	   16, 17, 18, 19, 20, 21,
  	   20, 21, 22, 23, 24, 25,
	   24, 25, 26, 27, 28, 29,
	   28, 29, 30, 31, 32,  1
};

//S盒
int DES::s_ia8_4_16S_Box[8][4][16] = 
{  
	//S1 
	{{14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},   
	{0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},   
	{4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},  
	{15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}},  
    
	//S2  
    {{15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},  
	{3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5},  
	{0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},  
	{13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9}},  
    
	//S3  
    {{10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},  
	{13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},  
	{13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7},  
	{1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12}},  
             
	//S4  
    {{7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},  
    {13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},
	{10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},  
	{3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14}},  

	//S5  
	{{2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},  
	{14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6},  
	{4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},  
	{11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3}},  
     
	//S6  
	{{12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},  
	{10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},  
	{9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},  
	{4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13}},  
			  		  
	//S7  
	{{4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},  
	{13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},  
	{1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},  
	{6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12}},  
   
	//S8  
	{{13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},  
	{1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},  
	{7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},  
	{2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11}}
};

//置换运算P矩阵
int DES::s_ia32P[32] =
{
	   16,  7, 20, 21,
	   29, 12, 28, 17,
		1, 15, 23, 26,
		5, 18, 31, 10,
		2,  8, 24, 14,
	   32, 27,  3,  9,
	   19, 13, 30,  6,
	   22, 11,  4, 25
};

//逆初始置换IP^-1
int DES::s_ia64IP_1[64] =
{
	   40,  8, 48, 16, 56, 24, 64, 32,
	   39,  7, 47, 15, 55, 23, 63, 31,
	   38,  6, 46, 14, 54, 22, 62, 30,
	   37,  5, 45, 13, 53, 21, 61, 29,
	   36,  4, 44, 12, 52, 20, 60, 28,
	   35,  3, 43, 11, 51, 19, 59, 27,
	   34,  2, 42, 10, 50, 18, 58, 26,
	   33,  1, 41,  9, 49, 17, 57, 25
};

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDesDlg dialog

CDesDlg::CDesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDesDlg)
	m_editPath = _T("文件的完整路径");
	m_editPsw1 = _T("请输入八个字节的密钥");
	m_editPsw2 = _T("请输入八个字节的密钥");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDI_MAINFRAME);
}

void CDesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDesDlg)
	DDX_Control(pDX, IDC_BUTTON_DECRYPT, m_buttonDecrypt);
	DDX_Control(pDX, IDC_BUTTON_ENCRYPT, m_buttonEncrypt);
	DDX_Text(pDX, IDC_EDIT_PATH, m_editPath);
	DDX_Text(pDX, IDC_EDIT_PSW1, m_editPsw1);
	DDX_Text(pDX, IDC_EDIT_PSW2, m_editPsw2);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDesDlg, CDialog)
	//{{AFX_MSG_MAP(CDesDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, OnButtonOpen)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON_ENCRYPT, OnButtonEncrypt)
	ON_EN_SETFOCUS(IDC_EDIT_PSW1, OnSetfocusEditPsw1)
	ON_EN_SETFOCUS(IDC_EDIT_PSW2, OnSetfocusEditPsw2)
	ON_EN_KILLFOCUS(IDC_EDIT_PSW1, OnKillfocusEditPsw1)
	ON_EN_KILLFOCUS(IDC_EDIT_PSW2, OnKillfocusEditPsw2)
	ON_BN_CLICKED(IDC_BUTTON_DECRYPT, OnButtonDecrypt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDesDlg message handlers

BOOL CDesDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDesDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDesDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDesDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

	//程序说明开始
	//==================================================================================
	//  功能 : 响应打开文件按钮
	//  参数 : 无
	//  返回 : 无
	//  主要思路 : 创建一个CFileDialog类,然后用DoModal()方法打开文件选择对话框,如果点击了
	//             确定按钮,则获取文件路径,然后检查文件后缀名是否超过七个字节,如果超过了
	//             则提醒用户不支持这种类型,重新输入,否则更新文件路径框的值,然后检查两个
	//             密钥的长度是否为8,如果不是则禁用加解密按钮并退出,如果是的,则检查文件类型
	//  调用举例 : 无
	//  日期 : 2016年5月30日 20:59:18(注释日期)
	//==================================================================================
	//程序说明结束
void CDesDlg::OnButtonOpen() 
{
	// TODO: Add your control notification handler code here
	CString csFilePath = L"";                       //保存文件路径
	CString csPassword1 = L"";                      //保存密钥1
	CString csPassword2 = L"";                      //保存密钥2
	CString csFileExt = L"";                        //保存文件后缀名

	CFileDialog CcmFileOpenDlg(TRUE,NULL,NULL,NULL,NULL,NULL);  //获取打开文件对话框的类

	if (IDOK == CcmFileOpenDlg.DoModal())           //文件选择对话框
	{
		csFilePath = CcmFileOpenDlg.GetPathName();  //获取文件路径

		//获取文件后缀名
		csFileExt = csFilePath.Mid(csFilePath.ReverseFind('.')+1);

		//判断文件后缀名长度,如果超过七个字节,则提示出错并退出本函数
		if (7 < csFileExt.GetLength())
		{
			MessageBox("仅支持七个字节的后缀名的文件\n请检查后重新输入!","出错提示");
			return;
		}

		SetDlgItemText(IDC_EDIT_PATH,csFilePath);   //设置EDIT框的值

		//获取两个密钥
		GetDlgItem(IDC_EDIT_PSW1)->GetWindowText(csPassword1);
		GetDlgItem(IDC_EDIT_PSW2)->GetWindowText(csPassword2);

		//判断密钥长度,如果两个密钥长度都为8,则进行文件类型判断,否则将加解密按钮禁用
		if (8 == csPassword1.GetLength() && 8 == csPassword2.GetLength())
		{
			cmCheck();
		}
		else
		{
			m_buttonEncrypt.EnableWindow(false);
			m_buttonDecrypt.EnableWindow(false);
		}
	}
}

	//程序说明开始
	//==================================================================================
	//  功能 : 将文件拖到对话框上,获取文件路径
	//  参数 : 无
	//  返回 : 无
	//  主要思路 : 通过DragQueryFile函数获取所拖动的文件路径,然后转化为CString类.再获取
	//             两个密钥,检查长度是否为8,如果不是则禁用加解密按钮,如果是,则检查文件
	//             类型
	//  调用举例 : 无
	//  日期 : 2016年5月30日 21:01:00(注释日期)
	//==================================================================================
	//程序说明结束
void CDesDlg::OnDropFiles(HDROP hDropInfo) 
{
	// TODO: Add your message handler code here and/or call default
	char szFilePath[200] = {0};                               //保存文件路径
	CString csPassword1 = L"";				                  //保存密钥1
	CString csPassword2 = L"";                                //保存密钥2
	CString csFilePath = L"";                                 //保存文件路径
	CString csFileExt = L"";                                  //保存文件后缀名

	DragQueryFile(hDropInfo,0,szFilePath,sizeof(szFilePath)); //获取文件路径

	//将文件路径转化为CString类型
	csFilePath.Format("%s",szFilePath);

	//获取文件的后缀名
	csFileExt = csFilePath.Mid(csFilePath.ReverseFind('.')+1);

	//判断后缀名长度,如果大于七个字节则提示出错并让用户重新输入
	if (7 < csFileExt.GetLength())
	{
		MessageBox("仅支持七个字节的后缀名的文件\n请检查后重新输入!","出错提示");
		return;
	}


	SetDlgItemText(IDC_EDIT_PATH,csFilePath);                 //更新EDIT框
	DragFinish(hDropInfo);                                    //拖放结束后,释放内存

	//获取两个密钥
	GetDlgItem(IDC_EDIT_PSW1)->GetWindowText(csPassword1);
	GetDlgItem(IDC_EDIT_PSW2)->GetWindowText(csPassword2);

	//判断密钥长度,如果两个密钥长度都为8,则进行文件类型判断,否则将加解密按钮禁用
	if (8 == csPassword1.GetLength() && 8 == csPassword2.GetLength())
	{
		cmCheck();
	}
	else
	{
		m_buttonEncrypt.EnableWindow(false);
		m_buttonDecrypt.EnableWindow(false);
	}

	CDialog::OnDropFiles(hDropInfo);
}

	//程序说明开始
	//==================================================================================
	//  功能 : 焦点在第一个密钥编辑框时的响应函数
	//  参数 : 无
	//  返回 : 无
	//  主要思路 : 捕获EN_SETFOCUS消息,判断密钥是否是初始语句,如果是,则清空,否则退出
	//  调用举例 : 无
	//  日期 : 2016年5月30日 21:04:17(注释日期)
	//==================================================================================
	//程序说明结束
void CDesDlg::OnSetfocusEditPsw1() 
{
	// TODO: Add your control notification handler code here
	CString csStr = L"";     //保存第一个密钥

	//获取密钥
	GetDlgItem(IDC_EDIT_PSW1)->GetWindowText(csStr);

	//如果密钥是初始字符串,则清空
	if (0 == strcmp(csStr,"请输入八个字节的密钥"))
	{
		SetDlgItemText(IDC_EDIT_PSW1,"");
	}
}

	//程序说明开始
	//==================================================================================
	//  功能 : 焦点在第二个密钥编辑框时的响应函数
	//  参数 : 无
	//  返回 : 无
	//  主要思路 : 捕获EN_SETFOCUS消息,判断密钥是否是初始语句,如果是,则清空,否则退出
	//  调用举例 : 无
	//  日期 : 2016年5月30日 21:06:33(注释日期)
	//==================================================================================
	//程序说明结束
void CDesDlg::OnSetfocusEditPsw2() 
{
	// TODO: Add your control notification handler code here
	CString csStr = L"";     //保存第二个密钥

	//获取密钥
	GetDlgItem(IDC_EDIT_PSW2)->GetWindowText(csStr);

	//如果密钥是初始字符串,则清空
	if (0 == strcmp(csStr,"请输入八个字节的密钥"))
	{
		SetDlgItemText(IDC_EDIT_PSW2,"");
	}
}

	//程序说明开始
	//==================================================================================
	//  功能 : 焦点离开第一个密钥编辑框时的响应函数
	//  参数 : 无
	//  返回 : 无
	//  主要思路 : 捕获EN_KILLFOCUS消息,判断密钥长度是否为8,如果不是则设置密钥编辑框为
	//             初始化提醒语句,并禁用加解密按钮, 如果是,则检查密钥二的长度,如果密钥
	//             二长度也是8,则检查输入文件的类型,如果不是则退出
	//  调用举例 : 无
	//  日期 : 2016年5月30日 21:08:45(注释日期)
	//==================================================================================
	//程序说明结束
void CDesDlg::OnKillfocusEditPsw1() 
{
	// TODO: Add your control notification handler code here
	CString csPassword1 = L"";            //保存第一个密钥
	CString csPassword2 = L"";            //保存第二个密钥

	//获取两个密钥
	GetDlgItem(IDC_EDIT_PSW1)->GetWindowText(csPassword1);
	GetDlgItem(IDC_EDIT_PSW2)->GetWindowText(csPassword2);

	//如果输入的第一个密钥长度不为8,则提示输入八个字节的密钥
	//并将加解密按钮禁用,否则继续检查第二个密钥长度
	if (8 != csPassword1.GetLength())
	{
		SetDlgItemText(IDC_EDIT_PSW1,"请输入八个字节的密钥");
		m_buttonEncrypt.EnableWindow(false);
		m_buttonDecrypt.EnableWindow(false);
	}
	else
	{
		//如果第二个密钥的长度也是8,则进行文件类型判断
		if (8 == csPassword2.GetLength())
		{
			cmCheck();
		}
	}
}

	//程序说明开始
	//==================================================================================
	//  功能 : 焦点离开第二个密钥编辑框时的响应函数
	//  参数 : 无
	//  返回 : 无
	//  主要思路 : 捕获EN_KILLFOCUS消息,判断密钥长度是否为8,如果不是则设置密钥编辑框为
	//             初始化提醒语句,并禁用加解密按钮, 如果是,则检查密钥一的长度,如果密钥
	//             一长度也是8,则检查输入文件的类型,如果不是则退出
	//  调用举例 : 无
	//  日期 : 2016年5月30日 21:10:09(注释日期)
	//==================================================================================
	//程序说明结束
void CDesDlg::OnKillfocusEditPsw2() 
{
	// TODO: Add your control notification handler code here
	CString csPassword1 = L"";           //保存第一个密钥
	CString csPassword2 = L"";           //保存第二个密钥

	//获取两个密钥
	GetDlgItem(IDC_EDIT_PSW1)->GetWindowText(csPassword1);
	GetDlgItem(IDC_EDIT_PSW2)->GetWindowText(csPassword2);

	//如果输入的第二个密钥长度不为8,则提示输入八个字节的密钥
	//并将加解密按钮禁用,否则继续检查第一个密钥长度
	if (8 != csPassword2.GetLength())
	{
		SetDlgItemText(IDC_EDIT_PSW2,"请输入八个字节的密钥");
		m_buttonEncrypt.EnableWindow(false);
		m_buttonDecrypt.EnableWindow(false);
	}
	else
	{
		//如果第一个密钥的长度也是8,则进行文件类型判断
		if (8 == csPassword1.GetLength())
		{
			cmCheck();
		}
	}
}

	//程序说明开始
	//==================================================================================
	//  功能 : 检查文件类型,决定加解密按钮是否要被激活
	//  参数 : 无
	//  返回 : 无
	//  主要思路 : 获取文件路径,提取文件类型,如果不是ept类型,则只激活加密按钮,否则激活加解密按钮
	//  调用举例 : 无
	//  日期 : 2016年5月30日 21:13:25(注释日期)
	//==================================================================================
	//程序说明结束
void CDesDlg::cmCheck()
{
	CString csFilePath = L"";                 //保存文件路径
	CString csFileExt = L"";                  //保存文件扩展名

	//获取打开文件的路径,截取最后一个.后面的字符作为文件后缀名
	GetDlgItem(IDC_EDIT_PATH)->GetWindowText(csFilePath);
	csFileExt = csFilePath.Mid(csFilePath.ReverseFind('.')+1);

	//检查文件后缀名是否是ept,如果是,则可以加密和解密,如果不是,则只能加密
	if (0 != strcmp(csFileExt,"ept"))
	{
		m_buttonEncrypt.EnableWindow(true);
		m_buttonDecrypt.EnableWindow(false);
	}
	else
	{
		m_buttonEncrypt.EnableWindow(true);
		m_buttonDecrypt.EnableWindow(true);
	}
}

	//程序说明开始
	//==================================================================================
	//  功能 : 加密按钮响应函数
	//  参数 : 无
	//  返回 : 无
	//  主要思路 : 获取加密文件路径,然后将加密文件路径后缀名改为ept后设置为加密后的文件
	//             保存路径,获取两个密钥,加密之前保存当前系统时间,然后调用加密函数对文件
	//             加密,加密成功后,获取系统时间,计算时间差得到加密所消耗的时间,然后弹框
	//             提示加密成功和加密所需时间,如果加密失败,则提示加密失败
	//  调用举例 : 
	//  日期 : 2016年5月30日 21:17:04(注释日期)
	//==================================================================================
	//程序说明结束
void CDesDlg::OnButtonEncrypt() 
{
	// TODO: Add your control notification handler code here
	unsigned char uca9Pwd1[9] = {0};       //保存第一个密钥
	unsigned char uca9Pwd2[9] = {0};       //保存第二个密钥
	CString csOpenFilePath = L"";          //保存要解密的文件路径
	CString csSaveFilePath = L"";          //保存加密后的文件路径
	CString csPassword = L"";              //保存密钥
	CString csTip = L"";                   //保存提示字符
	CString csTemp = L"";                  //保存临时字符串
	long lEncryptStartTime = 0;            //加密起始时间
	long lEncryptEndTime = 0;              //加密结束后的时间
	long lTime = 0;                        //临时时间变量

	DES CcmDes;                            //DES类

	//获取要加密的文件的路径
	GetDlgItem(IDC_EDIT_PATH)->GetWindowText(csOpenFilePath);
	
	//提取加密文件路径,截取最后一个.之前的字符串,再连接上.ept作为加密后的文件保存路径
	csSaveFilePath = csOpenFilePath.Left(csOpenFilePath.ReverseFind('.')) + ".ept";

	//获取密钥一,unsigned char类型的变量已经不是字符串变量了,所以要进行转换
	GetDlgItem(IDC_EDIT_PSW1)->GetWindowText(csPassword);
	strncpy((char *)uca9Pwd1,csPassword,8);

	//获取密钥二
	GetDlgItem(IDC_EDIT_PSW2)->GetWindowText(csPassword);
	strncpy((char *)uca9Pwd2,csPassword,8);
	
	//获取加密前的系统时间
	lEncryptStartTime = GetTickCount();

	//调用加密函数,加密成功返回true,失败返回false
	if (CcmDes.FileEncrypt(csOpenFilePath, csSaveFilePath, uca9Pwd1, uca9Pwd2))
	{
		//加密成功,获取加密后的系统时间
		lEncryptEndTime = GetTickCount();

		//计算加密时间差
		lTime = lEncryptEndTime - lEncryptStartTime;

		//提取小时部分
		csTemp.Format("加密成功!!\n加密耗费 %d时",lTime/3600000);
		lTime = lTime%3600000;
		csTip = csTemp;

		//提取分钟部分
		csTemp.Format(" %d分",lTime/60000);
		lTime = lTime%60000;
		csTip += csTemp;

		//提取秒部分
		csTemp.Format(" %d秒",lTime/1000);
		lTime = lTime%1000;
		csTip += csTemp;

		//提取毫秒部分
		csTemp.Format(" %d",lTime);
		csTip += csTemp;

		//弹框提示
		MessageBox(csTip,"提示");
	}
	else
	{
		//加密失败提示
		MessageBox("加密失败!!","提示");
	}
}

	//程序说明开始
	//==================================================================================
	//  功能 : 解密按钮响应函数
	//  参数 : 无
	//  返回 : 无
	//  主要思路 : 获取解密文件路径,然后将解密文件路径去掉后缀名,加上(解密).后设置为加密
	//             后的文件保存路径,获取两个密钥,解密之前保存当前系统时间,然后调用解密
	//             函数对文件解密,解密成功后,获取系统时间,计算时间差得到解密所消耗的时间
	//             然后弹框提示解密成功和解密所需时间,如果解密失败,则提示解密失败
	//  调用举例 : 
	//  日期 : 2016年5月30日 21:19:18(注释日期)
	//==================================================================================
	//程序说明结束
void CDesDlg::OnButtonDecrypt() 
{
	// TODO: Add your control notification handler code here
	unsigned char uca9Pwd1[9] = {0};       //保存第一个密钥
	unsigned char uca9Pwd2[9] = {0};       //保存第二个密钥
	CString csOpenFilePath = L"";          //保存要解密的文件路径
	CString csSaveFilePath = L"";          //保存解密后的文件路径
	CString csPassword = L"";              //保存密钥
	CString csTip = L"";                   //保存提示字符
	CString csTemp = L"";                  //保存临时字符串
	long lEncryptStartTime = 0;            //解起始时间
	long lEncryptEndTime = 0;              //解密结束后的时间
	long lTime = 0;                        //临时时间变量

	DES CcmDes;                            //DES类

	//获取要解密的文件的路径
	GetDlgItem(IDC_EDIT_PATH)->GetWindowText(csOpenFilePath);
	
	//提取解密文件路径,截取最后一个.之前的字符串,再连接上(解密).作为解密后的文件保存路径
	//在解密函数里获取解密后保存文件的后缀名
	csSaveFilePath = csOpenFilePath.Left(csOpenFilePath.ReverseFind('.')) + "(解密).";

	//获取密钥一,unsigned char类型的变量已经不是字符串变量了,所以要进行转换
	GetDlgItem(IDC_EDIT_PSW1)->GetWindowText(csPassword);
	strncpy((char *)uca9Pwd1,csPassword,8);

	//获取密钥二
	GetDlgItem(IDC_EDIT_PSW2)->GetWindowText(csPassword);
	strncpy((char *)uca9Pwd2,csPassword,8);
	
	//获取解密前的系统时间
	lEncryptStartTime = GetTickCount();

	//调用解密函数,解密成功返回true,失败返回false
	if (CcmDes.FileDecrypt(csOpenFilePath, csSaveFilePath, uca9Pwd1, uca9Pwd2))
	{
		//解密成功,获取解密后的系统时间
		lEncryptEndTime = GetTickCount();

		//计算解密时间差
		lTime = lEncryptEndTime - lEncryptStartTime;

		//提取小时部分
		csTemp.Format("解密成功!!\n解密耗费 %d时",lTime/3600000);
		lTime = lTime%3600000;
		csTip = csTemp;

		//提取分钟部分
		csTemp.Format(" %d分",lTime/60000);
		lTime = lTime%60000;
		csTip += csTemp;

		//提取秒部分
		csTemp.Format(" %d秒",lTime/1000);
		lTime = lTime%1000;
		csTip += csTemp;

		//提取毫秒部分
		csTemp.Format(" %d",lTime);
		csTip += csTemp;

		//弹框提示
		MessageBox(csTip,"提示");
	}
	else
	{
		//解密失败提示
		MessageBox("解密失败!!","提示");
	}
}