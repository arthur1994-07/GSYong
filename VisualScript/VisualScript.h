/**
 * \date	2011/01/25
 * \author	Jun-Hyuk Choi
 */
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

class CFrame;
class SUBPATH;

class CVisualScriptApp : public CWinApp
{
protected:
	DECLARE_MESSAGE_MAP()

protected:
    CFrame* m_pFrame;
    HICON m_hIcon;
    SUBPATH* m_pPath;

public:
    CString m_strPath;

public:
	CVisualScriptApp();
    virtual ~CVisualScriptApp();

public:
    SUBPATH* GetSubPath() { return m_pPath; }

public:
	virtual BOOL InitInstance();
    virtual INT Run();

public:
    VOID OnCreateApp();
    VOID OnDestroyApp();

public:
    CFrame* GetFrame() { return m_pFrame; }
};

extern CVisualScriptApp theApp;