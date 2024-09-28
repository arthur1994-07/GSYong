// WorldEditView.h : interface of the CWorldEditView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_WORLDEDITVIEW_H__51B51C01_AE3A_4651_92A5_E5B56D3DC67A__INCLUDED_)
#define AFX_WORLDEDITVIEW_H__51B51C01_AE3A_4651_92A5_E5B56D3DC67A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

#include "../EngineLib/DxEffect/MapEdit/DxMapEditMan.h"
#include "../EngineLib/DxMeshs/FrameMesh/dxreplacecontainer.h"
#include "../EngineLib/DxTools/DxMethods.h"
#include "../EngineLib/DxTools/Light/DxLightMan.h"
#include "../EngineLib/DxTools/DxEditBox.h"
#include "../EngineLib/DxTools/DxObjectMRS.h"
#include "../EngineLib/DxLand/DxPiece/DxPieceManagerPSF.h"
#include "../EngineLib/DxLand/DxLandMan.h"
#include "../EngineLib/DxLand/DxSetLandMan.h"
#include "../EngineLib/DxLand/DxPiece/DxPieceEdit.h"
#include "../EngineLib/DxCommon/d3dapp.h"

class DxEffectBase;
class DxEditMat;
class DxGlobalStage;
class GLCharacter;
class EngineDeviceMan;
class DxPieceView;
class DxPrefab;

enum _VIEWTYPE
{
	FRAME_VIEW		    = ID_VIEW_FRAME,
	OCTREE_VIEW		    = ID_VIEW_OCTREE,
	NAVI_VIEW		    = ID_VIEW_NAVIMESH,
	PIECE_VIEW		    = ID_VIEW_PIECE,
	TILE_GAME_VIEW	    = 0,
	TILE_EDIT_VIEW	    = 1,
	FRAMEMESH_VIEW	    = 2,
	STATICMESH_VIEW	    = 3,
	CAMCOLL_VIEW        = 4,
    PIECE_REPLACE_VIEW	= 5,	// Replace Piece. < PIE File >
    PIECE_SETPOS_VIEW   = 6,
    EFFECT_SET_VIEW		= 7,	// Effect 설정.
    EFFECT_TILE_VIEW    = 8,	// Effect TileBlend 설정
    EFFECT_GRASS_VIEW   = 9,	// Effect Grass 설정
	WATER_SURF_VIEW		= 10,	// WaterSurf 설정
	PREFAB_VIEW			= 11,	// Prefab
};

enum _SET_TYPE
{
	BASE_VIEW	= 0,
	HEIGHT_VIEW = 1,
	TILE_COLOR_VIEW	= 2,
	MAP_PIECE_VIEW = 3,
};

enum RENDER_TYPE
{
	RENDER_TEXTURE_LIGHTMAP	= ID_CHECK_RENDER_TEXTURE_LIGHTMAP,
	RENDER_LIGHTMAP			= ID_CHECK_RENDER_LIGHTMAP,
	RENDER_LIGHTMAP_POINT	= ID_CHECK_RENDER_LIGHTMAP_POINT,
	RENDER_LIGHTMAP_CHECKER	= ID_CHECK_RENDER_LIGHTMAP_CHECKER,
};

enum WATER_TYPE
{
	RENDER_WATER_RT_REFLECTION		= ID_CHECK_WATER_RT_REFLECTION,
	RENDER_WATER_CUBEMAP			= ID_CHECK_WATER_CUBEMAP,
	RENDER_WATER_DIFFUSE_TEXTURE	= ID_CHECK_WATER_DIFFUSE_TEXTURE,
};

enum _CUR_TYPE
{
	_CUR_NONE = ID_VIEW_CURNONE,
	_CUR_SELECT = ID_VIEW_SELECT,
	_CUR_MOVE = ID_VIEW_MOVE,
	_CUR_ROTATE = ID_VIEW_ROTATE,
	_CUR_SCALE = ID_VIEW_SCALE,
	_CUR_SELECT_POINT = ID_VIEW_SELECTPOINT,
	_CUR_EDIT_BOX = ID_VIEW_EDITBOX,
	_CUR_CHAR_MOVE = ID_CHAR_MOVE,
	_CUR_GAME_TEST = ID_GAME_TEST,
	_CUR_TILE_DRAW = ID_VIEW_TILE_DRAW,
	_CUR_TILE_GET = ID_VIEW_TILE_GET,
	_CUR_TILE_DEL = ID_VIEW_TILE_DELETE,
    _CUR_TILE_SELECT_TEX = 40000,
};

class CWorldEditView : public CView, public CD3DApplication
{
public:
	static _VIEWTYPE	m_ViewType;
	static _SET_TYPE	m_SetType;
	static _CUR_TYPE	m_CurType;
	static RENDER_TYPE	m_RenderType;
	static WATER_TYPE	m_WaterType;
	static DWORD		m_dwRenderPSType;
	static DWORD		m_dwRenderCharType;
	static BOOL	        g_bMiniMap;
	static BOOL	        g_bOrthoView;
	static BOOL			g_bOcclsionPointRender;
	static BOOL			g_bOcclsionOctreeRender;
	static BOOL			g_bCubeMapPointRender;
	static BOOL			g_bCubeMapOctreeRender;
	static BOOL			g_bSpecCamaraMode;

public:
	static void SetCurType ( _CUR_TYPE cType ) { m_CurType = cType; }

public:
    static DxPiece*			g_pPIECE;
    static DxPiece*			g_pDYNAMIC_PIECE;
    static DxPieceEdit*		g_pPIECEEDIT;
    static TSTRING			g_strPIECENAME;
	static DxSetLandMan*	g_rSetLandMan;	// 참조만함
	static DxPrefab*		g_pPrefab;

private:
	BOOL			m_bCreated;
	HCURSOR			m_hCursorDefault;
    EngineDeviceMan* m_pEngineDevice;

	DxSetLandMan	m_SetLandMan;		// 기존 psf
	DxSetLandMan	m_SetLandManPrefab;	// Prefab용
	boost::shared_ptr<DxLandMan>		m_spDxLandMan;
	DXREPLACEPIECE	m_ReplacePiece;

	DxMapEditMan	m_DxMapEditMan;
	DxPieceEdit		m_DxPieceEdit;
	
	DxFrameMesh*	m_p3dPosMesh;
	DxEditMat*		m_pDxEditmat;
	DxEditBox		m_EditBox;

	D3DXVECTOR3		m_vCollisionPos;

    DxGlobalStage*  m_pGlobalStage;
	GLCharacter*	m_pCharacter;

    BOOL            m_bFrameMeshWireFrame;  // FrameMesh + WireFrame Rendering 되도록 함.

	DWORD			m_dwDelayGPU;
	DWORD			m_dwTimeGetTimePrev;

public:
	LPDIRECT3DSWAPCHAIN9 m_pSwapChainMain;
	LPDIRECT3DSWAPCHAIN9 m_pSwapChainPreView;

public:
	DxObjectMRS		m_sObjectMRS;

public:
	void ResetObjects ();
	void SetObjRotate90();

public:
	void TempCreateChar ();
	void MakeMiniMap( D3DXVECTOR2& vMin, 
					D3DXVECTOR2& vMax, 
					DWORD dwTexSize, 
					const TCHAR* pName );

	void SkinLoadForTool( const TCHAR* szName ); 

public:
	void SetNSLightMapFX_SetRenderMode();

public:
	void			SetCurEditType ( DWORD EditType );
	D3DXVECTOR3&	GetCollisionPos () {	return m_vCollisionPos; }
	void			SetCollisionPos ( D3DXVECTOR3 vPos ) { m_vCollisionPos = vPos; }

private:
	DxEffectBase*	m_pActiveEffect;

public:
	void			SetActiveEffect ( DxEffectBase* pActiveEffect );
	void			ResetActiveEffect ();

	void			ActiveLight ( DXLIGHT *pLight );
	void			ActiveSpotLight( LPD3DXMATRIX pMatrix );
	void			DeActiveLight ();

	void		ActiveEditMatrix2( D3DXMATRIX* matWorld );
	void		ActiveEditMatrix( DXAFFINEMATRIX *pAffineMatrix );
	void		DeActiveEditMatrix ();
	void		SetCurClick( BOOL bUse );

	void			ActiveEditBox ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );
	void			DeActiveEditBox ();

private:
	BOOL			m_bHeightBaseCamera;

protected:
	void			CurSelect ();
    void			CurPoint_Dynamic();						// 현 마우스 포인트의 위치.
	void			CurSelectPoint ();
    BOOL            CurSelectPoint_Frame( const D3DXVECTOR3& vFromPt, D3DXVECTOR3& vTargetPt );
	void			CurMove ();
	void			CurCharMove ();

public:
	BOOL			Pick();

	void			SetLandMan();
	void			SetLandManPrefab();
	void			RefreshPrefab( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
    void FrameMoveCurNext( float fElapsedTime );

public:
	LPDIRECT3DDEVICEQ	GetD3DDevice ()			{ return m_pd3dDevice; }
	DxLandMan*			GetLandMan ()			{ return m_spDxLandMan.get(); }
	boost::shared_ptr<DxLandMan> GetspLandMan()	{ return m_spDxLandMan; }
	DxFrameMesh*		GetFrameMesh ()			{ return g_rSetLandMan->GetFrameMesh(); }
	DxSetLandMan*		GetSetLandMan ()		{ return g_rSetLandMan; }
	DxMapEditMan*		GetMapEditMan ()		{ return &m_DxMapEditMan; }
	DxPieceEdit*		GetPieceEdit()			{ return &m_DxPieceEdit; }
	DXREPLACEPIECE*		GetReplacePiece ()		{ return &m_ReplacePiece; }
	DxPieceView*		GetPieceView()			{ return g_rSetLandMan->GetPieceView(); }
	DxPieceManagerPSF*	GetPieceManager()		{ return g_rSetLandMan->GetPieceManager(); }

	CStaticSoundMan*	GetStaticSoundMan ()	{ return g_rSetLandMan->GetStaticSoundMan(); }
	GLCharacter*		GetCharacter ()			{ return m_pCharacter; }
	DxEditBox*			GetEditBox ()			{ return &m_EditBox; }

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

	void OnOcclusionQuery( IDirect3DDevice9* pd3dDevice );

	void RenderDefferedOpaqueForPSF( IDirect3DDevice9* pd3dDevice, const D3DCOLOR& colorClear );
	void RenderDefferedSoftAlphaForPSF( IDirect3DDevice9* pd3dDevice );

	void RenderDefferedOpaqueForWLD( IDirect3DDevice9* pd3dDevice, const D3DCOLOR& colorClear );
	void RenderDefferedSoftAlphaForWLD( IDirect3DDevice9* pd3dDevice );

	HRESULT RenderText();

public:
	HRESULT OpenXFile ( CString FileName );

	HRESULT OnSize ( int cx, int cy )
	{
		m_d3dpp.BackBufferWidth  = cx;
		m_d3dpp.BackBufferHeight = cy;

		return Resize3DEnvironment();
	}

protected: // create from serialization only
	CWorldEditView();
	DECLARE_DYNCREATE(CWorldEditView)

// Attributes
public:
	CWorldEditDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorldEditView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void PostNcDestroy();
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWorldEditView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CWorldEditView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
protected:
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnFilePieceLoad();
	afx_msg void OnReload();
    afx_msg void OnArchive();
    afx_msg void OnPsfSave();
    afx_msg void OnEnter();
	afx_msg void OnDropFiles(HDROP hDropInfo);
};

#ifndef _DEBUG  // debug version in WorldEditView.cpp
inline CWorldEditDoc* CWorldEditView::GetDocument()
   { return (CWorldEditDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORLDEDITVIEW_H__51B51C01_AE3A_4651_92A5_E5B56D3DC67A__INCLUDED_)
