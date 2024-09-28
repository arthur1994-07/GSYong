#if !defined(AFX_STCREATEPOS_H__57420D7F_EE4D_445D_84F6_5D665F5FDBB9__INCLUDED_)
#define AFX_STCREATEPOS_H__57420D7F_EE4D_445D_84F6_5D665F5FDBB9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StCreatePos.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStCreatePos window
#include "../../MfcExLib/ExLibs/MemDc.h"

#define _CT_POS		(0x0001)
#define _CT_DIR		(0x0002)

class CStCreatePos : public CStatic
{
// Construction
public:
	CStCreatePos();

// Attributes
public:
	float		MAX_SIZE;
	float		m_fScale;
	DWORD		m_dwType;
	D3DVECTOR	m_vtCreatePos;

public:
	void UpdateContrl ( float fSize );

protected:
	CPoint Projection ( float x, float y, float z, float AngX, float AngY );

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStCreatePos)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStCreatePos();

	// Generated message map functions
protected:
	//{{AFX_MSG(CStCreatePos)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STCREATEPOS_H__57420D7F_EE4D_445D_84F6_5D665F5FDBB9__INCLUDED_)
