/*
Module : SPLASHER.H
Purpose: A splash screen component for MFC which uses a DIB bitmap instead of a resource. Palette management 
         code is also included so that the bitmap will display using its own optimized palette. Also most
         importantly, the splash screen is shown in its own thread, meaning that it will redraw correctly
         while you app is performing it's startup code and not necessarily pumping the message queue.
Created: PJN / 15-11-1996

Copyright (c) 1996 - 2009 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/
/*
    2012-02-06 란 툴에 맞게 소스가 수정되었습니다.
    더이상 비트맵을 사용하지 않습니다.
*/

///////////////////////// Macros / Defines ////////////////////////////////////

#pragma once

#ifndef __SPLASHER_H__
#define __SPLASHER_H__

#ifndef CSPLASHER_EXT_CLASS
#define CSPLASHER_EXT_CLASS
#endif

#ifndef CSPLASHER_EXT_API
#define CSPLASHER_EXT_API
#endif


///////////////////////// Includes ////////////////////////////////////////////

#ifndef __AFXMT_H__
#pragma message("To avoid this message, please put afxmt.h in your pre compiled header (normally stdafx.h)")
#include <afxmt.h>
#endif


///////////////////////// Classes /////////////////////////////////////////////

class CSPLASHER_EXT_CLASS CSplashWnd : public CWnd
{
public:
//Constructors / Destructors
  CSplashWnd();

  void DisplayMessage(const std::string Message);
  void DisplayProgress(const unsigned int _iPercentage);

protected:
  typedef BOOL (WINAPI *lpfnSetLayeredWindowAttributes)(HWND, COLORREF, BYTE, DWORD);

  DECLARE_DYNCREATE(CSplashWnd);

//Virtual methods
  virtual BOOL Create();
  //virtual BOOL LoadBitmap();

//Misc methods
  void SetBitmapToUse(const CString& sFilename);
  BOOL SetTransparent(COLORREF clrTransparent, DWORD dwFlags, BYTE bAlpha);
  void SetDropShadow(BOOL bDropShadow = TRUE);
  void SetBitmapToUse(UINT nResourceID);
  void SetBitmapToUse(LPCTSTR pszResourceName); 
  void SetOKToClose() { m_bOKToClose = TRUE; };  
  void SetIcon(HICON hIcon);
  BOOL SelRelPal(BOOL bForceBkgnd);
  //BOOL CreatePaletteFromBitmap();
  void SetDraggable(BOOL bDragable);
  BOOL TransparencyAvailable() const { return m_lpfnSetLayeredWindowAttributes != NULL; };

  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnPaint();
  afx_msg void OnClose();
  afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
  afx_msg BOOL OnQueryNewPalette(); 
 afx_msg LRESULT OnNcHitTest(CPoint point);

  DECLARE_MESSAGE_MAP()
  
//Member variables
  BOOL                           m_bOKToClose;                     //Ok to respond to WM_CLOSE messages
  CBitmap                        m_Bitmap;                         //The Bitmap we are displaying
  CPalette                       m_Palette;                        //The palette associated with the bitmap
  int                            m_nHeight;                        //The height in pixels of the bitmap
  int                            m_nWidth;                         //The width in pixels of the bitmap
  CWnd                           m_wndOwner;                       //Our hidden window parent (causes this window not to have a entry in the task bar)            
  BOOL                           m_bUseFile;                       //Should we use m_sFilename when it comes time to call LoadBitmap 
  LPCTSTR                        m_pszResourceName;                //The resource ID if m_bUseFile is FALSE
  CString                        m_sFilename;                      //The name of the file to load the bitmap from (used if m_bUseFile is TRUE)
  HICON                          m_hIcon;                          //The icon to use for this window
  BOOL                           m_bTransparent;                   //Should the image be drawn transparent
  COLORREF                       m_clrTransparent;                 //The transparent color to use
  BOOL                           m_bDraggable;                     //Should the splash window be draggable
  lpfnSetLayeredWindowAttributes m_lpfnSetLayeredWindowAttributes; //Pointer to the function "SetLayeredWindowAttributes"
  BOOL                           m_bDropShadow;                    //Should the window have a drop shadow effect
  DWORD                          m_dwSLWAFlags;                    //The flags passed to SetLayeredWindowAttributes
  BYTE                           m_bSLWAAlpha;                     //The transparency level passed to SetLayeredWindowAttributes
  CProgressCtrl*                 m_pProgress;
  CStatic*                       m_pStatic;
public:
    afx_msg void OnDestroy();
};

//GUI thread in which the splash window is run
class CSPLASHER_EXT_CLASS CSplashThread : public CWinThread
{
public:
//Member variables
  CRuntimeClass* m_pRuntimeClassSplashWnd; //The runtime class version of "m_pSplashScreen"
  CEvent         m_evtInitInstanceRun;     //Event using to synchronise to our InitInstance method
  volatile LONG  m_bInitOK;                //Was InitInstance successful
	CSplashWnd*    m_pSplashScreen;          //Pointer to the CWnd splash screen

protected:
//Constructors / Destructors
	CSplashThread();

	DECLARE_DYNCREATE(CSplashThread)

	virtual BOOL InitInstance();
    virtual int  ExitInstance();

	DECLARE_MESSAGE_MAP()
};

//Class which looks after creating the splash window in the separate thread
class CSPLASHER_EXT_CLASS CSplashFactory : public CWinThread
{
public:
//Constructors / Destructors
    CSplashFactory();
    ~CSplashFactory();

//Methods
    virtual BOOL Create(CRuntimeClass* pRuntimeClassSplashWnd, int nPriority = THREAD_PRIORITY_NORMAL);  
    void DisplayMessage( std::string Message_);
    void DisplayProgress(const unsigned int _iPercentage_);
    virtual BOOL Close(CWnd* pWndToGainFocus = NULL);

protected:
//Member variables
    CSplashThread* m_pSplashThread; //The GUI thread we are managing
};

#endif //__SPLASHER_H__
