/*
Module : SPLASHER.CPP
Purpose: A splash screen component for MFC which uses a DIB bitmap instead of a resource. Palette management 
         code is also included so that the bitmap will display using its own optimized palette. Also most
         importantly, the splash screen is shown in its own thread, meaning that it will redraw correctly
         while you app is performing it's startup code and not necessarily pumping the message queue.
Created: PJN / 15-11-1996
History: PJN / 11-12-1997 1) Incoporation of new DIB code provided by the authors own CDibImage class. A static library 
                          version is used by splasher here. If you want to use CDIbImage for anything other than 
                          CSplashThread, then you need to download CDibImage on its own.
                          2) Can now load from resource or bitmap
                          3) General tidy up of the code.
         PJN / 22-3-1998  1) Palette is now correctly initialised on startup
                          2) Code now protects itself if m_SplashScreen cannot be created
         PJN / 22-12-1998 1) Now no longer dependent on CDibImage.
                          2) Unicode Enabled the code,
                          3) General tidy up of the code
                          4) Removed the unnecessary variable m_bCreated
                          5) Fixed a potential race condition in CSplashThread::HideSplash()
         PJN / 01-03-2000 1) Fixed a problem with bitmaps which do not have a palette
                          2) Fixed a problem in Win 98 and Win2000 when the splash screen is closed but the main window 
                          of your app fails to activate. The code now uses AttachThreadInput to synchronise the UI 
                          activities of the main GUI thread and the splash screen thread.
         PJN / 01-01-2001 1) Now includes copyright message in the source code and documentation.
                          2) Modified the way the sample app waits for the thread to exit. It now synchronises with the 
                          exit of the splash screen thread
                          3) Now allows the icon of the splash screen to be specified
                          4) Now allows the splash screen window to be customized at run time through a virtual function 
                          "CSplashThread::CreateSplash". Thanks to Yury Goltsman for suggesting these enhancements.
         PJN / 27-08-2001 1) Splash screen now by default includes the window style WS_EX_TOPMOST
         PJN / 31-08-2001 1) Fixed an assert which was occuring when you brought up the splash window and closed it 
                          really quickly. Thanks to Wanner Robert for spotting this problem.
                          2) Removed a busy loop in HideSplash and used a Win32 event for synchronisation instead.
                          3) Implemented a class factory design pattern to further simplify the use of the splash screen 
                          class. All client code now needs to do is derive a class from CSplashWnd (making sure it's 
                          declared DYNCREATE) and at a minimum just call some of it's methods in the constructor to 
                          determine what it will display at runtime. Then just use CSplashFactory::Create with the name 
                          of your derived class. When you want to close the splash screen just call CSplashFactory::Close. 
                          What could be simpler!!!
         PJN / 03-10-2002 1) Now allows you to specify a transparent color to draw the splash screen with. This allows you 
                          to realize shapes other than the standard rectangle. Thanks to Gero Gerber for this update. Please 
                          note that this support is only available on Windows 2000 or later.
                          2) Made the destructor of CSplashWnd virtual
         PJN / 11-10-2002 1) Fixed up some coding issues reported by BoundsChecker in CSplashWnd::OnPaint.
                          2) Now supports a drop shadow effect on the splash screen (assuming that the client OS is 
                          Windows XP or later).
         PJN / 28-08-2003 1) Removed a number of level 4 warnings.
         PJN / 06-09-2003 1) Fixed a problem where you get a crash in the close method if the window to gain focus was NULL. 
                          Thanks to Darius Thabit for reporting this problem.
         PJN / 09-05-2004 1) Updated the copyright details
                          2) Included a new sample app in the download to demonstrate the correct usage of the splash screen in 
                          a dialog based app.
                          3) Fixed a bug where sometimes the splash screen component would get stuck while waiting for the splash 
                          screen thread to exit. The fix is to request the parent window to exit in CSplashWnd::OnClose. Thanks 
                          to Frederic Metrich for reporting this issue.
         PJN / 09-07-2006 1. Updated the code to clean compile on VC 2005.
                          2. Updated copyright details.  
                          3. Addition of CSPLASHER_EXT_CLASS macro to allow the classes to be easily added to an extension dll.  
                          4. Code now uses newer C++ style casts instead of C style casts.    
                          5. Optimized CSplashWnd constructor code  
                          6. Optimized CSplashThread constructor code  
                          7. Reviewed all TRACE statements for correctness.    
                          8. UI thread which is created by the code is now created at idle priority  
                          9. Updated the documentation to use the same style as the web site.       
         PJN / 22-07-2007 1. Updated copyright details.
                          2. Optimized CSplashFactory constructor code
                          3. CSplashWnd code now uses WM_NCHITTEST logic to support a draggable splash screen
                          4. Made Create and Close methods in CSplashFactory virtual
                          5. CSplashFactory::Create now waits for the worker thread's InitInstance to complete
                          6. Removed unnecessary VERIFY call from CSplashWnd::Create
                          7. CSplashWnd::CreatePaletteFromBitmap now returns a BOOL
                          8. Removed the unnecessary CSplashWnd destructor
                          9. Removed the unnecessary CSplashThread destructor.
                          10. Reviewed all TRACE statements for correctness
                          11. Removed unnecessary use of friend classes
                          12. Updated sample apps to clean compile on VC2005
         PJN / 13-10-2007 1. CSplashFactory::Create now by default creates the splash screen thread at normal priority instead of 
                          idle priority. This helps avoid thread starvation issues when the main thread is busy performing its 
                          startup code. Please be aware of this issue if you modify your applications main thread priority to be 
                          anything other than THREAD_PRIORITY_NORMAL. If you do please make sure that the nPriority value to 
                          CSplashFactory::Create is >= the value you use for your main thread, or otherwise you will starve the 
                          splash screen of CPU cycles on single core processor. Thanks to Ralf Magnussen for reporting this issue.
         PJN / 17-11-2007 1. Fixed a memory leak where the CMySplashWnd instance was not being deleted in the worker thread. 
                          Thanks to Laurent Lhermin?for reporting this bug.
         PJN / 17-05-2009 1. Updated copyright details
                          2. Removed VC 6 style classwizard comments from the code.
                          3. Updated code to compile correctly using _ATL_CSTRING_EXPLICIT_CONSTRUCTORS define
                          4. Updated CSplashWnd::SetTransparent to allow the alpha transparency level as well as the flags passed 
                          to SetLayeredWindowAttributes to be completed specified
                          5. Updated code to clean compile on VC 2008.
                          6. Code now compiles cleanly using Code Analysis (/analyze)
                          7. The code now only supports VC 2005 or later.

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

//////////////////// Includes /////////////////////////////////////////////////

#include "stdafx.h"
#include "Splasher.h"

//////////////////// Macros / Defines /////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//Define the various constants we need if not available
#ifndef WS_EX_LAYERED
#define WS_EX_LAYERED 0x00080000
#endif
#ifndef CS_DROPSHADOW
#define CS_DROPSHADOW 0x00020000
#endif


//////////////////// Implementation ///////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSplashWnd, CWnd)
  ON_WM_CREATE()
  ON_WM_PAINT()
  ON_WM_CLOSE()
	ON_WM_NCHITTEST()
    ON_WM_DESTROY()
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CSplashWnd, CWnd);

BOOL CALLBACK DialogProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
    /*case WM_COMMAND:*/
    case WM_CLOSE:
        EndDialog(hWnd, 0);
        break;
    }
    return FALSE;
}

CSplashWnd::CSplashWnd() 
    : m_bOKToClose(FALSE)
    , m_nHeight(60)
    , m_nWidth(200)
    , m_bUseFile(FALSE)
    , m_pszResourceName(NULL)
    , m_hIcon(NULL)
    , m_bTransparent(FALSE)
    , m_clrTransparent(RGB(255, 0, 255))
    , m_bDraggable(TRUE)
    , m_bDropShadow(FALSE)
    , m_pStatic(NULL)
    , m_pProgress(NULL)
{
  //Determine if the OS supports layered windows
  HINSTANCE hUser32 = GetModuleHandle(_T("USER32.DLL"));
  if (hUser32)
    m_lpfnSetLayeredWindowAttributes = reinterpret_cast<lpfnSetLayeredWindowAttributes>(GetProcAddress(hUser32, "SetLayeredWindowAttributes"));
  else 
    m_lpfnSetLayeredWindowAttributes = NULL;
}


void CSplashWnd::DisplayMessage(const std::string Message){	
    if(m_pStatic != NULL)
        m_pStatic->SetWindowText( Message.c_str() );	
}


void CSplashWnd::DisplayProgress(const unsigned int _iPercentage){
    if(m_pProgress != NULL)
    m_pProgress->SetPos(_iPercentage);
}


BOOL CSplashWnd::SetTransparent(COLORREF clrTransparent, DWORD dwFlags, BYTE bAlpha)
{
  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

  if (m_lpfnSetLayeredWindowAttributes)
  {
    m_bTransparent = TRUE;
    m_clrTransparent = clrTransparent;
    m_dwSLWAFlags = dwFlags;
    m_bSLWAAlpha = bAlpha;
    bSuccess = TRUE;
  }

  return bSuccess;
}


void CSplashWnd::SetDraggable(BOOL bDraggable)
{
  m_bDraggable = bDraggable;
}


void CSplashWnd::SetDropShadow(BOOL bDropShadow)
{
  m_bDropShadow = bDropShadow;
}


BOOL CSplashWnd::Create()
{                   
  //Load up the bitmap from file or from resource
//   if (!LoadBitmap())
//     return FALSE;

  //Modify the owner window of the splash screen to be an invisible WS_POPUP 
  //window so that the splash screen does not appear in the task bar
  LPCTSTR pszWndClassOwner = AfxRegisterWndClass(0, 0, 0, m_hIcon);
  if (!m_wndOwner.CreateEx(0, pszWndClassOwner, _T(""), WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, 0))
  {
    TRACE(_T("CSplashWnd::Create, Failed to create owner window\n"));  
    return FALSE;
  }

  //Create the appropiate window class to use for the splash window
  LPCTSTR pszWndClass = NULL;
  if (m_bDropShadow)
  {
    try
    {
      pszWndClass = AfxRegisterWndClass(CS_DROPSHADOW, AfxGetApp()->LoadStandardCursor(IDC_ARROW), 0, m_hIcon);
    }
    catch(CResourceException* pEx)
    {
      //Avoid the memory leak
      pEx->Delete();

      //Probably because CS_DROPSHADOW is not supported on this OS, try again without it
      pszWndClass = AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW), 0, m_hIcon);
    }
  }
  else
    pszWndClass = AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW), 0, m_hIcon);

  //Create this window
  if (!CreateEx(WS_EX_TOPMOST, pszWndClass, _T(""), WS_POPUP | WS_VISIBLE, 0, 0, m_nWidth, m_nHeight, m_wndOwner.GetSafeHwnd(), NULL))
  {
    TRACE(_T("CSplashWnd::Create, Failed to create the splash screen\n"));
    return FALSE;
  }

  //Make the window transparent if required to
  if (m_bTransparent && m_lpfnSetLayeredWindowAttributes)
  {
    //Add the transparent window style
    LONG nStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);
    SetWindowLong(m_hWnd, GWL_EXSTYLE, nStyle | WS_EX_LAYERED);

    //Make the Window transparent using the specifed color
    m_lpfnSetLayeredWindowAttributes(m_hWnd, m_clrTransparent, m_bSLWAAlpha, m_dwSLWAFlags);
  }

  //Create the palette, We need to do this after the window is created because
  //we may need to access the DC associated with it
//   if (!CreatePaletteFromBitmap())
//     return FALSE;

  //realize the bitmap's palette into the DC
  OnQueryNewPalette();


  m_pStatic = new CStatic;
  m_pStatic->Create(_T("Please Wait..."), WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0,0,200,50), this);   
  

  //m_pSatic->SetWindowText("Puck!!!!1");
  DisplayMessage("Please Wait...");


  m_pProgress = new CProgressCtrl;
  m_pProgress->Create(WS_VISIBLE|WS_CHILD|PBS_SMOOTH, CRect(0,50,200,60), this, 1);
  m_pProgress->SetRange(0, 100);
  m_pProgress->SetPos(0);
  //m_pProgress->ShowWindow()

  return TRUE;
}

int CSplashWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  //Let the parent class do its thing
  if (CWnd::OnCreate(lpCreateStruct) == -1)
    return -1;

  //Center the splash window on the screen
  CenterWindow();

  return 0;
}

void CSplashWnd::OnPaint()
{
    //CPaintDC dc(this);
    
//   //select the palette and bitmap into the memory DC and then bitblit it to the screen DC
//   CDC memDC;
//   memDC.CreateCompatibleDC(&dc);
//   //CBitmap* pOldBitmap = memDC.SelectObject(&m_Bitmap);
//   CBitmap* pOldBitmap = memDC.SelectObject(&m_Bitmap);
//   CPalette* pOldPalette = NULL;
//   if (m_Palette.m_hObject)
//     pOldPalette = dc.SelectPalette(&m_Palette, FALSE);
//   dc.RealizePalette();
//   dc.BitBlt(0, 0, m_nWidth, m_nHeight, &memDC, 0, 0, SRCCOPY);
// 
//   //Restore the DC


//   memDC.SelectObject(pOldBitmap);         
//   if (pOldPalette)
//     dc.SelectPalette(pOldPalette, FALSE);
}

void CSplashWnd::OnClose() 
{
  if (m_bOKToClose) 
  {
    //Just request the parent window to close which will cause this window to be destroyed
    m_wndOwner.PostMessage(WM_CLOSE);
  }
}

BOOL CSplashWnd::SelRelPal(BOOL bForceBkgnd)
{
  //We are going active, so realize our palette.
  CDC* pDC = GetDC();

  CPalette* pOldPal = pDC->SelectPalette(&m_Palette, bForceBkgnd);
  UINT nRemappedColors = pDC->RealizePalette();
  pDC->SelectPalette(pOldPal, TRUE);
  pDC->RealizePalette();

  ReleaseDC(pDC);

  //If any colors have changed or we are in the
  //background, repaint the lot.
  if (nRemappedColors || bForceBkgnd) 
    InvalidateRect(NULL, TRUE);
  
  return (nRemappedColors != 0); //TRUE if some colors changed.
}

void CSplashWnd::OnPaletteChanged(CWnd* pFocusWnd)
{
  //See if the change was caused by us and ignore it if not
  if (pFocusWnd != this) 
    SelRelPal(TRUE); //Realize in the background. 
}

BOOL CSplashWnd::OnQueryNewPalette()
{
  return SelRelPal(FALSE); //Realize in the foreground.
}

void CSplashWnd::SetBitmapToUse(const CString& sFilename)
{
  m_bUseFile = TRUE;
  m_sFilename = sFilename;
}

void CSplashWnd::SetBitmapToUse(UINT nResourceID)
{
  m_bUseFile = FALSE;
  m_pszResourceName = MAKEINTRESOURCE(nResourceID);
}

void CSplashWnd::SetBitmapToUse(LPCTSTR pszResourceName)
{
  m_bUseFile = FALSE;
  m_pszResourceName = pszResourceName;
}

void CSplashWnd::SetIcon(HICON hIcon)
{
  //Destroy the icon if we have one already loaded
  if (m_hIcon)
    DestroyIcon(m_hIcon);
     
  m_hIcon = hIcon; 
}

LRESULT CSplashWnd::OnNcHitTest(CPoint point) 
{
  if (m_bDraggable)
    return HTCAPTION; 
  else
  	return CWnd::OnNcHitTest(point);
}


BEGIN_MESSAGE_MAP(CSplashThread, CWinThread)
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CSplashThread, CWinThread)

CSplashThread::CSplashThread() : m_pRuntimeClassSplashWnd(NULL),
                                 m_pSplashScreen(NULL),
                                 m_bInitOK(FALSE)
{
}

BOOL CSplashThread::InitInstance()
{
  //Validate our parameters
  AFXASSUME(m_pRuntimeClassSplashWnd);

  //Create the splash screen window object
  m_pSplashScreen = static_cast<CSplashWnd*>(m_pRuntimeClassSplashWnd->CreateObject());
  m_pMainWnd = m_pSplashScreen;
  if (m_pSplashScreen == NULL)
  {
    TRACE(_T("CSplashThread::InitInstance, Failed to instantiate CSplashWnd object\n"));
    m_evtInitInstanceRun.SetEvent();
    return FALSE;
  }
  
  //Validate that it is derived from CSplashWnd
  ASSERT(m_pSplashScreen->IsKindOf(RUNTIME_CLASS(CSplashWnd)));

  //Create the HWND of it
  if (!m_pSplashScreen->Create())
  {
    TRACE(_T("CSplashThread::InitInstance, Failed to create CSplashWnd object\n"));

    //Tidy up the heap memory we have allocated
    delete m_pSplashScreen;
    m_pSplashScreen = NULL;

    m_evtInitInstanceRun.SetEvent();
    return FALSE;
  }

  //Signal the event which indicates that this InitInstance function has run
  InterlockedExchange(&m_bInitOK, TRUE);
  m_evtInitInstanceRun.SetEvent();

	return TRUE;
}

int CSplashThread::ExitInstance()
{
  //Free up our splash screen instance if created
  if (m_pSplashScreen)
  {
    delete m_pSplashScreen;
    m_pSplashScreen = NULL;
  }

  //Let the base class do its thing
  return CWinThread::ExitInstance();
}

CSplashFactory::CSplashFactory() : m_pSplashThread(NULL)
{
}

CSplashFactory::~CSplashFactory()
{
  //Just incase client code forgot to call Close, we do it anyway
  Close();
}

BOOL CSplashFactory::Create(CRuntimeClass* pRuntimeClassSplashWnd, int nPriority)
{
  //Validate our parameters
  ASSERT(m_pSplashThread == NULL);
  ASSERT(pRuntimeClassSplashWnd);

  //Create the GUI worker thread
  m_pSplashThread = static_cast<CSplashThread*>(AfxBeginThread(RUNTIME_CLASS(CSplashThread), nPriority, 0, CREATE_SUSPENDED));
  if (m_pSplashThread == NULL)
  {
    TRACE(_T("CSplashFactory::Create, Failed to create splash screen thread\n"));
    return FALSE;
  }

  ASSERT(m_pSplashThread->IsKindOf(RUNTIME_CLASS(CSplashThread)));
  m_pSplashThread->m_pRuntimeClassSplashWnd = pRuntimeClassSplashWnd;
  m_pSplashThread->m_bAutoDelete = FALSE; //We are in charge of the deletion of the thread
  m_pSplashThread->ResumeThread();        //Resume the thread now that we have set it up 

  //Wait until the thread's InitInstance code has completed. Note this is important because code in CSplashFactory::Close
  //can execute after Create returns and we want to ensure that "m_pSplashThread->m_pSplashScreen" is not modified from
  //the worker thread while we access it from the main UI thread
  WaitForSingleObject(m_pSplashThread->m_evtInitInstanceRun, INFINITE);
  
  //Get the return value from InitInstance
  LONG bSuccess = FALSE;
  InterlockedExchange(&bSuccess, m_pSplashThread->m_bInitOK);
  return bSuccess;
}

BOOL CSplashFactory::Close(CWnd* pWndToGainFocus)
{
  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

  if (m_pSplashThread)
  {
    //Attach this threads UI state to the worker threads one, This will ensure that 
    //the activation state is managed consistenly across the two threads
    ASSERT(AfxGetApp());
    AttachThreadInput(AfxGetApp()->m_nThreadID, m_pSplashThread->m_nThreadID, TRUE);

    //Set the focus back to the correct window
    CWnd* pWndFocus = pWndToGainFocus;
    if (pWndFocus == NULL)
      pWndFocus = AfxGetMainWnd();
    if (pWndFocus)
    {
      pWndFocus->SetFocus();
      pWndFocus->SetForegroundWindow();
    }

    //If the worker thread has a splash screen then ask it to close before trying to close it. See comment in 
    //CSplashFactory::Create about why can can safely dot into a pointer set in another thread
    if (m_pSplashThread->m_pSplashScreen)
    {
      m_pSplashThread->m_pSplashScreen->SetOKToClose();
      m_pSplashThread->m_pSplashScreen->PostMessage(WM_CLOSE);
    }

    //Wait for the worker thread to exit and tidy up its memory
    WaitForSingleObject(m_pSplashThread->m_hThread, INFINITE);
    delete m_pSplashThread;
    m_pSplashThread = NULL;

    bSuccess = TRUE;
  }

  return bSuccess;
}

void CSplashFactory::DisplayMessage( std::string Message_ )
{
    if( m_pSplashThread )
    {
          m_pSplashThread->m_pSplashScreen->DisplayMessage(Message_);
    }
}

void CSplashFactory::DisplayProgress( const unsigned int _iPercentage_ )
{
    if( m_pSplashThread )
    {
        m_pSplashThread->m_pSplashScreen->DisplayProgress(_iPercentage_);
    }
}

void CSplashWnd::OnDestroy()
{
    CWnd::OnDestroy();

    // TODO: 여기에 메시지 처리기 코드를 추가합니다.    
    delete m_pProgress;
    delete m_pStatic;
}
