#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

class CFrame;
class SUBPATH;

class CUITextureApp : public CWinApp
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
	CUITextureApp();
    virtual ~CUITextureApp();

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

extern CUITextureApp theApp;