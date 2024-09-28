// LevelEditView.h : iCLevelEditView 클래스의 인터페이스
//
#pragma once

#include "resource.h"
#include "../RanLogicServer/Land/GLLandMan.h"
#include "../EngineLib/DxLand/DxLandMan.h"
#include "../RanLogicServer/FieldServer/GLGaeaServer.h"
#include "../EngineLib/DxTools/DxEditBox.h"
#include "../EngineLib/DxTools/DxObjectMRS.h"
#include "../EngineLib/DxCommon/d3dapp.h"

enum _CUR_TYPE
{
	_CUR_NONE = ID_VIEW_CURNONE,
	_CUR_SELECT = ID_VIEW_SELECT,
	_CUR_MOVE = ID_VIEW_MOVE,
	_CUR_ROTATE = ID_VIEW_ROTATE,
	_CUR_SCALE = ID_VIEW_SCALE,
	_CUR_EDIT_BOX = ID_VIEW_BOXEDIT,

	_CUR_SEL_MOBSCH = ID_VIEW_SELECT_MOBSCH,
};

class DxFrameMesh;
class DxEditMat;
class EngineDeviceMan;

class CLevelEditView : public CView, public CD3DApplication
{
public:
	BOOL					m_bCreated;

public:
	static BOOL						m_bRENDQUAD;
	boost::shared_ptr<DxLandMan>	m_spDxLandMan;

	BOOL ImportGLLandMan ( const char* szFile, BOOL bExceptEffect=FALSE );
	BOOL LoadGLLandMan ( const char* szFile );
	BOOL SaveGLLandMan ( const char* szFile );

	HRESULT SaveCsvFile ( const char* szFile ); 
	HRESULT LoadCsvFile ( const char* szFile );


protected:
    EngineDeviceMan* m_pEngineDevice;
	static _CUR_TYPE		m_CurType;

	DxFrameMesh*			m_p3dPosMesh;
	DxEditMat*				m_pDxEditmat;
	DxEditBox				m_EditBox;
	DxObjectMRS				m_sObjectMRS;

	D3DXVECTOR3				m_vCollisionPos;

	void					CurSelect ();
	void					CurMove ();

public:
	void					ActiveEditMatrix2( D3DXMATRIX* matWorld );
	void					ActiveEditMatrix ( DXAFFINEMATRIX *pAffineMatrix );
	void					DeActiveEditMatrix ();

public:
	static void				SetCurType ( _CUR_TYPE cType )		{ m_CurType = cType; }

	void							SetCharMoveTo ();
	void							SetCurEditType ( DWORD EditType );
	D3DXVECTOR3&					GetCollisionPos ()					{	return m_vCollisionPos; }
	DxLandMan*						GetLandMan ()						{	return m_spDxLandMan.get(); }
	boost::shared_ptr<DxLandMan>	GetspLandMan ()						{	return m_spDxLandMan; }

public:
	LPDIRECT3DDEVICEQ		GetD3DDevice ()			{ return m_pd3dDevice; }
	DxEditBox*				GetEditBox ()			{ return &m_EditBox; }
	void					ActiveEditBox ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );
	void					DeActiveEditBox ();

protected: // serialization에서만 만들어집니다.
	CLevelEditView();
	DECLARE_DYNCREATE(CLevelEditView)

// 특성
public:
	CLevelEditDoc* GetDocument() const;

// 작업
public:
private:
	HRESULT ConfirmDevice( D3DCAPSQ*,DWORD,D3DFORMAT );
	HRESULT OneTimeSceneInit();
	HRESULT InitDeviceObjects();
	HRESULT RestoreDeviceObjects();
	HRESULT FrameMove( BOOL bNotRendering );
	HRESULT Render();
	VOID	RenderLandEffectState();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
	HRESULT FinalCleanup();

	void RenderDefferedOpaqueForWLD( IDirect3DDevice9* pd3dDevice, const D3DCOLOR& colorClear );
	void RenderDefferedSoftAlphaForWLD( IDirect3DDevice9* pd3dDevice );

	HRESULT RenderText();

	HRESULT OnSize ( int cx, int cy )
	{
		if ( cx < 10 )	cx = 10;
		if ( cy < 10 )	cy = 10;

		m_d3dpp.BackBufferWidth  = cx;
		m_d3dpp.BackBufferHeight = cy;

		return Resize3DEnvironment();
	}

public:
	LPDIRECT3DDEVICEQ GetD3dDevice ()	{ return m_pd3dDevice; }
	D3DCAPSQ GetD3dCaps ()				{ return m_d3dCaps; }

// 재정의
	public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 구현
public:
	virtual ~CLevelEditView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 메시지 맵 함수를 생성했습니다.
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
protected:
	virtual void PostNcDestroy();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
public:
//	afx_msg void OnClose();
};

#ifndef _DEBUG  // LevelEditView.cpp의 디버그 버전
inline CLevelEditDoc* CLevelEditView::GetDocument() const
   { return reinterpret_cast<CLevelEditDoc*>(m_pDocument); }
#endif

