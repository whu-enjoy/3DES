// des.h : main header file for the DES application
//

#if !defined(AFX_DES_H__634A6A49_A036_4F5A_BBBF_3C6C01678244__INCLUDED_)
#define AFX_DES_H__634A6A49_A036_4F5A_BBBF_3C6C01678244__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDesApp:
// See des.cpp for the implementation of this class
//

class CDesApp : public CWinApp
{
public:
	CDesApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDesApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DES_H__634A6A49_A036_4F5A_BBBF_3C6C01678244__INCLUDED_)
