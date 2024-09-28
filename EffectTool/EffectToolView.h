// EffectToolView.h : interface of the CEffectToolView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_EFFECTTOOLVIEW_H__623F2D42_84EA_4C6E_B050_D84E17B33736__INCLUDED_)
#define AFX_EFFECTTOOLVIEW_H__623F2D42_84EA_4C6E_B050_D84E17B33736__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../EngineLib/DxCommon/d3dapp.h"

#include "../EngineLib/DxLand/DxLandMan.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinChar.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinObject.h"
#include "../EngineLib/DxTools/DxEditBox.h"
#include "../EngineLib/DxTools/DxObjectMRS.h"

class DxEditMat;
class DxFrameMesh;
class DxGlobalStage;
class GLCharacter;
class EngineDeviceMan;

enum _CUR_TYPE
{
	_CUR_NONE = ID_VIEW_CURNONE,
	_CUR_MOVE = ID_VIEW_MOVE,
	_CUR_ROTATE = ID_VIEW_ROTATE,
	_CUR_SCALE = ID_VIEW_SCALE,
	_CUR_EDIT_BOX = ID_VIEW_EDITBOX,
};

extern BOOL				g_bBGTex;


class CEffectToolView : public CView, public CD3DApplication
{
protected:
	boost::shared_ptr<DxLandMan>		m_spDxLandMan;
    
private:
	BOOL				m_bCreated;    

public:
	static BOOL			m_bCharView;
	static BOOL			m_bMapView;
	static int			m_iSheetTap;	// Now Tap ?
	static _CUR_TYPE	m_curType;
	DxObjectMRS			m_sObjectMRS;
    DxGlobalStage*      m_pGlobalStage;
	GLCharacter*		m_pCharacter;

	LPDIRECT3DTEXTUREQ	m_pBackGroundTex;

	LPDIRECT3DDEVICEQ	GetD3dDevice ()	{ return m_pd3dDevice; }

public:
	void				SetCurType ( _CUR_TYPE cType );

public:
	void		ActiveEditBox ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );
	void		DeActiveEditBox ();
	void		ActiveEditMatrix( D3DXMATRIX* matWorld );
	void		DeActiveEditMatrix();
	void		SetCurClick( BOOL bUse );

	void		LoadBGTexture( const char *pszFileName );
	void		RenderBackGround();

	D3DXMATRIX	matParent;
	void		SetParentMatrix( const D3DXMATRIX& matWorld ) { matParent = matWorld; }

private:
	HRESULT ConfirmDevice( D3DCAPSQ*,DWORD,D3DFORMAT );
	HRESULT OneTimeSceneInit();
	HRESULT InitDeviceObjects();
	HRESULT RestoreDeviceObjects();
	HRESULT FrameMove( BOOL bNotRendering );
	HRESULT Render();
	HRESULT InvalidateDeviceObjects();
	
	HRESULT DeleteDeviceObjects();
	HRESULT FinalCleanup();

	HRESULT RenderText();
	HRESULT ReSizeWindow ( int cx, int cy )
	{
		if ( cx < 10 )	cx = 10;
		if ( cy < 10 )	cy = 10;

		m_d3dpp.BackBufferWidth  = cx;
		m_d3dpp.BackBufferHeight = cy;

		return Resize3DEnvironment();
	}

protected: // create from serialization only
	CEffectToolView();
	DECLARE_DYNCREATE(CEffectToolView)

// Attributes
public:
	CEffectToolDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEffectToolView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEffectToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CEffectToolView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnArchive();
	afx_msg void OnArchiveMobile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
};

#ifndef _DEBUG  // debug version in EffectToolView.cpp
inline CEffectToolDoc* CEffectToolView::GetDocument()
   { return (CEffectToolDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFFECTTOOLVIEW_H__623F2D42_84EA_4C6E_B050_D84E17B33736__INCLUDED_)
