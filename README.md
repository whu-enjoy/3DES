# 3DES
<pre>
/////////////////////////////////////////////////////////////////////////////
//  工程 : des
//  作者 : enjoy5512   修改者 : enjoy5512   最后优化注释者 : enjoy5512
//  个人技术博客 : blog.csdn.net/enjoy5512
//  个人GitHub   : github.com/whu-enjoy
//  描述 : 对文件进行3DES加密的对话框主要实现文件,可以对文件进行加密,加密结果
//         保存在同目录下,后缀名改为ept,也可以对ept文件进行解密
//  主要实现代码 : desDlg.cpp
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
</pre>