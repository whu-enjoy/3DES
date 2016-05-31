// desDlg.h : header file
//

#if !defined(AFX_DESDLG_H__70E4105E_95F2_47ED_8793_7474D107239E__INCLUDED_)
#define AFX_DESDLG_H__70E4105E_95F2_47ED_8793_7474D107239E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CDesDlg dialog

class CDesDlg : public CDialog
{
// Construction
public:
	CDesDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDesDlg)
	enum { IDD = IDD_DES_DIALOG };
	CButton	m_buttonDecrypt;         //解密按钮
	CButton	m_buttonEncrypt;         //加密按钮
	CString	m_editPath;              //文件路径
	CString	m_editPsw1;              //密钥1
	CString	m_editPsw2;              //密钥2
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	void cmCheck();                                       //检查文件后缀名,确定激活加密还是解密按钮
	// Generated message map functions
	//{{AFX_MSG(CDesDlg)
	virtual BOOL OnInitDialog();                          //对话框初始化函数
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam); 
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonOpen();                          //打开文件选择对话框响应函数
	afx_msg void OnDropFiles(HDROP hDropInfo);            //支持文件拖放功能响应函数
	afx_msg void OnButtonEncrypt();                       //加密按钮响应函数
	afx_msg void OnSetfocusEditPsw1();                    //密钥一EDIT框获取焦点响应函数
	afx_msg void OnSetfocusEditPsw2();                    //密钥二EDIT框获取焦点响应函数
	afx_msg void OnKillfocusEditPsw1();                   //密钥一EDIT框失去焦点响应函数
	afx_msg void OnKillfocusEditPsw2();                   //密钥一EDIT框失去焦点响应函数
	afx_msg void OnButtonDecrypt();                       //解密按钮响应函数
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESDLG_H__70E4105E_95F2_47ED_8793_7474D107239E__INCLUDED_)
