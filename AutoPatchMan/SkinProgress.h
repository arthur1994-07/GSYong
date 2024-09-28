
#if !defined(AFX_SKINPROGRESS_H__1DF43017_611B_44BD_822A_FAA0E01B2257__INCLUDED_)
#define AFX_SKINPROGRESS_H__1DF43017_611B_44BD_822A_FAA0E01B2257__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// SkinProgress.h : header file
//

/*** eSkinProgressBitmap ******************************************************/
/* Purpose : Used in 'OnPaint' for internal purposes, in order to index the   */
/*           m_BarImgLst's elements                                           */
/* Unit    : None                                                             */
/* Range   : enum LIST                                                        */
/* List    :  0 = cSPB_START               : image for the start              */
/*            1 = cSPB_BEFORE              : image for before <m_nLeft>       */
/*            2 = cSPB_LEFT  = cSPB_TOP    : image for <m_nLeft>/<m_nLower>   */
/*            3 = cSPB_CENTER              : image for the center of the bar  */
/*            4 = cSPB_BAR                 : image for the bar body           */
/*            5 = cSPB_RIGHT = cSPB_BOTTOM : image for <m_nRight>             */
/*            6 = cSPB_AFTER               : image for after <m_nRight>       */
/*            7 = cSPB_BACKGROUND          : image for the bar background     */
/*            8 = cSPB_END                 : image for the end                */
/*            9 = cSPB_EnumElements        : current number of elements       */
/* Example : None                                                             */
/******************************************************************************/
/*
typedef enum eSkinProgressBitmap
{
  cSPB_START = 0,
  cSPB_BEFORE,
  cSPB_LEFT,
  cSPB_CENTER,
  cSPB_BAR,
  cSPB_RIGHT,
  cSPB_AFTER,
  cSPB_BACKGROUND,
  cSPB_END,
  cSPB_EnumElements,
  cSPB_TOP    = cSPB_LEFT,
  cSPB_BOTTOM = cSPB_RIGHT,
};
*/



typedef enum eSkinProgressBitmap
{
  cSPB_START = 0,
  cSPB_BACKGROUND,
  cSPB_END,
  cSPB_LEFT,
  cSPB_CENTER,
  cSPB_RIGHT,
  cSPB_EnumElements,
};

/*** eProgressSkinText ********************************************************/
/* Purpose : TRUE condition flag                                              */
/* Unit    : None                                                             */
/* Range   : enum LIST                                                        */
/* List    :  0 = cSPT_NONE         : no text information                     */
/*            1 = cSPT_PERCENT      : simple percent completion               */
/*            2 = cSPT_TIMED        : complete timed completion               */
/*            3 = cSPT_AUTOSIZE     : switch between cSPT_PERCENT and         */
/*                                    cSPT_TIMED according to the remaining   */
/*                                    place available                         */
/*            4 = cSPT_EnumElements : current number of elements              */
/* Example : None                                                             */
/******************************************************************************/
typedef enum eProgressSkinText
{
  cSPT_NONE = 0,
  cSPT_PERCENT,
  cSPT_TIMED,
  cSPT_AUTOSIZE,
  cSPT_EnumElements
};

/*** dCSP_DIALOG_PROGRESS *****************************************************/
/* Purpose : Activate the usage of CWnd object as anchor for the progress bar */
/* Unit    : None                                                             */
/* Range   : None                                                             */
/* List    : None                                                             */
/* Example : Lately implemented, but when activated, allows the use of CWnd   */
/*           objects to put the progress bar everywhere you want, in dialog   */
/*           box for instance...                                              */
/******************************************************************************/
#define dCSP_DIALOG_PROGRESS

/////////////////////////////////////////////////////////////////////////////
// CSkinProgress window

class CSkinProgress : public CStatic
{
	public: 

	CSkinProgress();
    virtual ~CSkinProgress();

	CSkinProgress
	( 
		LPCTSTR  i_poStrMessage,
		int      i_nUpper       = 100,
		int      i_nProgress    = cSPT_PERCENT,
		int      i_nPane        = 0,
		int      i_nSize        = 200,
		CBitmap* i_poBitmap     = NULL
	);

#ifdef dCSP_DIALOG_PROGRESS
    // Default constructor with parameters
	CSkinProgress
	(
		CWnd*    i_poWndProgress,
		int      i_nUpper       = 100, 
		CBitmap* i_poBitmap     = NULL,
		CWnd*    i_poWndMessage = NULL,
		LPCTSTR  i_poStrMessage = NULL,
		int      i_nProgress    = cSPT_NONE
	);
#endif // dCSP_DIALOG_PROGRESS

// *** BITMAP PROCESS ***

	BOOL SetBitmap ( CBitmap* i_poBitmap = NULL );

    // Copy a bitmap
	BOOL CopyBitmap ( CBitmap* o_poBitmap, CBitmap* i_poBitmap );

    // Convert a bitmap to a specified device context
	BOOL ConvBitmap ( CBitmap* o_poBitmap, CBitmap* i_poBitmap, CDC* i_poDC = NULL );

// *** PROGRESS INTERFACE ***

    // Set the new range
	BOOL SetRange( int i_nLower, int i_nUpper, int i_nStep = 1, BOOL i_bResamble = false );

    // Set <m_nRight> value
	int SetPos ( int i_nPos );

    // Forward of <i_nOffset> value
	int OffsetPos( int i_nOffset );

    // Set <m_nStep> value
	int SetStep( int i_nStep );

    // Forward of <m_nStep> value
	int StepIt ();

// *** EXTENDED PROGRESS INTERFACE ***

    // Set size of the progress bar
	BOOL SetSize ( int i_nSize );

    // Get width or height of the progress bar in pixel, mostly used in CSkinSlider
	int GetSize ();

    // Get <m_nRight> value
	int GetPos ( BOOL i_bPercent = false );

    // Get <m_nStep> value
	int GetStep ();

    // Get <m_nLower> value
	int GetLower ();

    // Get <m_nUpper> value
	int GetUpper ();

     // Restart the progress bar
    void Reset ();

// *** TEXT INTERFACE ***

    // Set the new text
	BOOL SetText ( LPCTSTR  i_poStrMessage );

    // Set progress text
	BOOL SetProgress ( int i_nProgress );

    // Get <m_nProgressText> value
	int GetProgress ();

// *** UPDATE PROCESS ***

    // Recalc the layout of the panes and refresh them, must be used in [OnSizing(...)] of the parent function
    BOOL RefreshPanes ();

    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(CSkinProgress)
	    //}}AFX_VIRTUAL

    // Implementation

protected: // --- PROTECTED -----------------------------------------------
    // Attributes

	int        m_nSize;              // Percentage size of control (if <0, autosize with the text if in pane 0)
	int        m_nPane;              // ID of status bar pane progress bar is to appear in

	CString    m_oStrMessage;        // Message to display in pane 0
	CString    m_oStrPrevMessage;    // Message to display in pane 0
	CString    m_oStrPrevText;       // Previous text in pane m_nPane

	int        m_nProgressText;      // Settings of the Progress text

	int        m_nLower;             // Min value
	int        m_nUpper;             // Max value
	int        m_nStep;              // Step increment

	int        m_nPrevRightAff;      // For 'OnPaint'
	int        m_nRightAff;          // For 'OnPaint'
	int        m_nPrevRight;         // Previous position (m_nLower <= m_nPrevRight <= m_nUpper)
	int        m_nRight;             // Current position  (m_nLower <= m_nRight <= m_nUpper)

	int        m_nPrevEndAff;        // For 'OnPaint'
	int        m_nEndAff;            // For 'OnPaint'

	int        m_nBmpWidth;          // Width of each m_BarImgLst element	
	int        m_nBmpHeight;         // Height of each m_BarImgLst element

	CRect      m_oRectPane;          // Position  of the progress bar used in [RefreshPanes()]
	CRect      m_oRectPaint;         // Dimension of the progress bar used in [OnPaint()]

	CBitmap*   m_poBaseBmp;          // The base bitmap
	CBitmap*   m_poCompatibleBmp;    // The client DC compatible base bitmap
	CBitmap*   m_poStretchBmp;		 // The stretched picture to adapt it to the bar size

	CImageList m_oBarImgLst;         // The ImageList that contains all the progress bar picture elements
	CBitmap*   m_poProgressBmp;      // The progress bar, created, modified and displayed in [OnPaint()]
	CBitmap*   m_poBackProgressBmp;      // The progress bar, created, modified and displayed in [OnPaint()]

#ifdef dCSP_DIALOG_PROGRESS
	CWnd*      m_poWndProgress;      // The base CWnd used to create the progress bar
	CWnd*      m_poWndMessage;       // The base CWnd used to create the text pane
#endif // dCSP_DIALOG_PROGRESS

	SYSTEMTIME m_sStartTimeSystem;   // Creation time of the SkinProgress
	LONGLONG   m_nStartTimeLongLong; // Same for 'GetTimed'

    // Operations

// *** CLEANER ***

    // Deletion of the progress bar, restoration of the context
	void Clear ();

// *** SYSTEM INTERFACE ***

    // Get the window's status bar handler
	CStatusBar* GetStatusBar ();

    // Get the target CRect of the progress bar
	BOOL GetTargetRect ( CRect* o_poProgressRect );

// *** INITIALIZATORS ***

    // I have put here 'Create...', to avoid to call them twice on an existing CSkinProgress !
    BOOL CreateCommon ( LPCTSTR i_poStrMessage, int i_nUpper = 100, int i_nProgress = cSPT_PERCENT, 
						CBitmap* i_poBitmap = NULL );

    // Specific Create for Status Bar
	BOOL ProgressInStatusBar ( LPCTSTR i_poStrMessage, int i_nUpper = 100, int i_nProgress = cSPT_PERCENT,
							   int i_nPane = 0, int i_nSize = 200, CBitmap* i_poBitmap = NULL );

#ifdef dCSP_DIALOG_PROGRESS
    // Specific Create for Dialog
	BOOL ProgressInDialog ( CWnd* i_poWndProgress, int i_nUpper = 100, CBitmap* i_poBitmap = NULL, 
							CWnd* i_poWndMessage = NULL, LPCTSTR i_poStrMessage = NULL, int i_nProgress = cSPT_NONE );
#endif // dCSP_DIALOG_PROGRESS

// *** DATA PROCESS ***

    // Calculation of the '...' text that fits in the (i_nMaxWidth + i_nAddWidth) place
	int CompactText ( CString* io_pString, CDC* i_pDC, int i_nMaxWidth, int i_nAddWidth = 0 );

    // Get the complete timed message (64 bits calculation inside)
    BOOL GetTimed ( CString* o_poTimed );

// *** UPDATE PROCESS ***

    // Update of the progress bar values
    BOOL UpdateProgress ( CRect* o_poRectPaint = NULL );	

// *** WINDOWS MAPPED PROCESSING ***

	  // Generated message map functions
	    //{{AFX_MSG(CSkinProgress)
	    afx_msg BOOL OnEraseBkgnd(CDC* i_pDC);
	    afx_msg void OnPaint();
	    afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	    afx_msg void OnSize(UINT nType, int cx, int cy);
	    afx_msg void OnTimer(UINT nIDEvent);
	  //}}AFX_MSG

	  DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnClose();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SKINPROGRESS_H__1DF43017_611B_44BD_822A_FAA0E01B2257__INCLUDED_)
