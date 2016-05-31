; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CDesDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "des.h"

ClassCount=3
Class1=CDesApp
Class2=CDesDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_DES_DIALOG

[CLS:CDesApp]
Type=0
HeaderFile=des.h
ImplementationFile=des.cpp
Filter=N

[CLS:CDesDlg]
Type=0
HeaderFile=desDlg.h
ImplementationFile=desDlg.cpp
Filter=W
BaseClass=CDialog
VirtualFilter=dWC
LastObject=CDesDlg

[CLS:CAboutDlg]
Type=0
HeaderFile=desDlg.h
ImplementationFile=desDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_DES_DIALOG]
Type=1
Class=CDesDlg
ControlCount=11
Control1=IDC_STATIC_PATH,static,1342308352
Control2=IDC_EDIT_PATH,edit,1350633600
Control3=IDC_BUTTON_OPEN,button,1342242816
Control4=IDC_STATIC_PSW1,static,1342308352
Control5=IDC_EDIT_PSW1,edit,1350631552
Control6=IDC_STATIC_PSW2,static,1342308352
Control7=IDC_EDIT_PSW2,edit,1350631552
Control8=IDC_BUTTON_ENCRYPT,button,1476460545
Control9=IDC_BUTTON_DECRYPT,button,1476460545
Control10=IDC_STATIC_AUTHOR,static,1342308352
Control11=IDC_STATIC,static,1342177287

