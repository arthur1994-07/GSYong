// CharEditView.h : interface of the CCharEditView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHAREDITVIEW_H_)
#define AFX_CHAREDITVIEW_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "../EngineLib/DxMeshs/SkinMesh/DxSkinMeshManager.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinChar.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinCharData.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxAttBone.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxAttBoneData.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinObject.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxVehicle.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxVehicleData.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxAttBoneLink.h"
#include "../EngineLib/DxEffect/EffKeep/DxEffKeep.h"
#include "../EngineLib/DxCommon/d3dapp.h"
#include "./CharEditDoc.h"
#include "../EngineLib/DxTools/DxObjectMRS.h"

class DxLandMan;
class CD3DFontPar;
class EngineDeviceMan;
class BILLBOARD_CHF_DATA;

enum ANIMATE
{
		AM_STOP				= 1,
		AM_PLAY				= 2,
		AM_STARTKEY			= 3,
		AM_ENDKEY			= 4,
		AM_PREVKEY			= 5,
		AM_NEXTKEY			= 6,
		AM_MULTIPLAY		= 7,
};

enum EDITOBJTYPE
{
		EOBJ_SKINOBJ		= 1,
		EOBJ_SKINCHAR		= 2,
		EOBJ_SKINPIECE		= 3,
		EOBJ_ATTBONE		= 4,
		EOBJ_ATTLINK		= 5,
		EOBJ_VEHICLE		= 6,
		EOBJ_FLYCAMERA		= 7,
};

struct DXRENDSTATE
{
		BOOL 				bWireFrame;
		BOOL 				bFlane;
		BOOL 				bBGTex;
		BOOL				bMovePiece;
		BOOL				bBillboard;

		DXRENDSTATE () :
			bWireFrame(FALSE),
			bFlane(TRUE),
			bBGTex(FALSE)
			, bMovePiece(FALSE)
			, bBillboard(FALSE)
		{
		}
};

enum RENDERTYPE
{
		RT_DEFAULT			= 0,
		RT_BONE				= 1,
		RT_WIREFRAME		= 2,
};

extern	DXRENDSTATE			g_RendState;
extern	ANIMATE				g_Animate;

class CSPINEX
{
public:
		int					FrameMove( BOOL& bUse, BOOL bDefWin );
		int					FrameMoveColor( int& iUse, BOOL bDefWin );
};

typedef std::vector<DxSkinPiece*> SkinPieceContainer;

/**
	캐릭터 툴의 뷰 클래스
	뷰를 통해 보여지는 모든 변수들을 제어 한다.

	현재 m_SkinObject 는 사용되지 않는다는 것에 주의 한다.	
*/
class CCharEditView : public CView, public CD3DApplication
{
public:
	static DWORD		m_dwRenderPSType;

public:
virtual ~CCharEditView();

virtual void 				OnDraw(CDC* pDC);
virtual BOOL 				PreCreateWindow(CREATESTRUCT& cs);
virtual void 				OnInitialUpdate();

		CCharEditDoc*		GetDocument();

#ifdef _DEBUG
virtual void				AssertValid() const;
virtual void				Dump(CDumpContext& dc) const;
#endif

		void				ActiveEditMatrix( D3DXMATRIX* matWorld );
		void				DeActiveEditMatrix();
		BOOL				IsActiveEditMatrix();						// m_sObjectMRS 의 커서 기능이 활성화인가~? 비활성화인가~?

		LPDIRECT3DDEVICEQ	GetD3dDevice ()										{ return m_pd3dDevice; }
		D3DCAPSQ			GetD3dCaps ()										{ return m_d3dCaps; }
		
		DxObjectMRS*		GetObjectMRS ()										{ return &m_sObjectMRS; }
		DxSkinChar* 		GetSkinChar ()			    					    { return &m_SkinChar; }
		DxSkinChar* 		GetSkinPassenger ( const int nPassenger )			{ return &m_SkinPassenger[nPassenger]; }
		DxSkinCharData*		GetSkinCharData ()	    					        { return &m_SkinCharData; }
		BILLBOARD_CHF_DATA*	GetBillboardData ()	    					        { return &m_sBillboardData; }
		DxAttBone*			GetAttBone ()			    					    { return &m_AttBone; }
		DxAttBoneData*		GetAttBoneData ()	    					        { return &m_AttBoneData; }
		DxVehicle*			GetVehicle ()			    					    { return &m_Vehicle; }
		DxVehicleData* 		GetVehicleData ()	    					        { return &m_VehicleData; }
		DxAttBoneLink* 		GetAttBoneLink ()									{ return &m_AttBoneLink; }
		DxSkeleton*	   		GetAttBoneLinkSkeleton()							{ return m_pAttBoneLinkSkeleton; }
		VOID		   		SetAttBoneLinkSkeleton( DxSkeleton *pSkeleton)		{ m_pAttBoneLinkSkeleton = NULL; m_pAttBoneLinkSkeleton = pSkeleton; }
		DxSkinObject*		GetSkinObject ()		    					    { return &m_SkinObject; }
        DxSkinPiece*		GetSkinPiece (int nParts = -1);
        D3DXMATRIX          GetSkinMatrix ()                                    { return m_SkinMat; }        
		boost::shared_ptr<DxLandMan>	GetLandMan(void) { return m_spLandMan; }

		EngineDeviceMan*	GetEngineDevice()									{ return m_pEngineDevice; }

		SkinPieceContainer	GetSkinPieceList()									{ return m_SkinPieceList; }
		void			 	SetSkinPieceList( SkinPieceContainer inputVec )		{ m_SkinPieceList = inputVec; }
		void				ClearSkinPieceList();

		void				SetSkinCharNonAtkMode(BOOL b)						{ m_bSkinCharNonAtkMode = b; }
		void				SetAttTIMER(float fattTIMER)						{ m_fattTIMER = fattTIMER; }
		void				SetAttSTEP(int nattSTEP)							{ m_nattSTEP = nattSTEP; }

		void				LoadBGTexture( const char *pszFileName );
		void				RenderBackGround();

		void				ResetSkinPos();

    	BOOL				Pick();

		void 				SetRenderTypeDefault();
		void 				SetRenderTypeBone()									{ m_emRenderType = RT_BONE; }
		void				SetRenderTypeWireframe()							{ m_emRenderType = RT_WIREFRAME; }

afx_msg void				OnViewfullsize();
afx_msg void				OnReload();
afx_msg void                OnArchive();

static	EDITOBJTYPE			m_emObjType;
static 	BOOL				m_bSkinTraceVertDraw;
static 	CString			    m_strSelectedTracVert;
static 	SVERTEXINFLU		m_VertexInflu;
static	DxEffKeepDataMain	g_sEffKeepMain;

protected:
		CCharEditView();
		DECLARE_DYNCREATE(CCharEditView)
		DECLARE_MESSAGE_MAP()

virtual BOOL 				OnPreparePrinting(CPrintInfo* pInfo);
virtual void 				OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
virtual void 				OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
virtual void 				PostNcDestroy();
virtual void 				OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);

afx_msg void 				OnSize(UINT nType, int cx, int cy);

		D3DXVECTOR3			m_vCameraLookAt;

private:
		HRESULT 			ConfirmDevice( D3DCAPSQ*,DWORD,D3DFORMAT );
		HRESULT 			OneTimeSceneInit();
		HRESULT 			InitDeviceObjects();
		HRESULT 			RestoreDeviceObjects();
		HRESULT 			FrameMove( BOOL bNotRendering );
		void				MoveUpdateSkinChar();
		void				ChangeAnimation();
		HRESULT 			Render();
		HRESULT 			InvalidateDeviceObjects();
		HRESULT 			DeleteDeviceObjects();
		HRESULT 			FinalCleanup();
		bool				PickingPos( D3DXVECTOR3& vPos );

		BOOL				AttackProc ( float fElapsedTime );
		BOOL				AttackVehicleProc ( float fElapsedTime );

		HRESULT 			RenderText();

		HRESULT				OnSize ( int cx, int cy );

		BOOL				m_bCreated;
		D3DXMATRIXA16		m_SkinMat;

		bool				m_bRunMove;

		float				m_fSkinRotate;
		D3DXVECTOR3			m_vSkinPos;
		D3DXVECTOR3			m_vTargetPos;
		D3DXVECTOR3			m_vSkinMeshPickPos;
		D3DXVECTOR3			m_vSkinBonePickPos;

		BOOL				m_bSkinCharNonAtkMode;

		LPDIRECT3DTEXTUREQ	m_pBackGroundTex;

		boost::shared_ptr<DxLandMan>	m_spLandMan;

		DxSkinObject		m_SkinObject;
		DxSkinChar			m_SkinChar;
		DxSkinCharData		m_SkinCharData;
		BILLBOARD_CHF_DATA	m_sBillboardData;
		DxVehicle			m_Vehicle;
		DxVehicleData		m_VehicleData;
		DxAttBone			m_AttBone;
		DxAttBoneData		m_AttBoneData;
		DxAttBoneLink		m_AttBoneLink;
		DxSkeleton*			m_pAttBoneLinkSkeleton;
        DxSkinPiece			m_SkinPiece;

		SkinPieceContainer	m_SkinPieceList;

		DxSkinChar			m_SkinPassenger[MAX_PASSENGER_COUNT];

static 	D3DXVECTOR3		    m_vCollision;
static 	RENDERTYPE		    m_emRenderType;

		float				m_fattTIMER;
		int					m_nattSTEP;

		DxObjectMRS			m_sObjectMRS;

		CSPINEX				m_sSpinRadius;
		CSPINEX				m_sSpinHeight;
		CSPINEX				m_sSpin3;
		CSPINEX				m_sSpin4;
		CSPINEX				m_sSpinColor;

		EngineDeviceMan*	m_pEngineDevice;    

public:
	void LoadLandFile( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strLandName );
	void EraseLand();

	void ClearSkinPassenger();

	D3DSURFACE_DESC   GetBackBufferSurface(void) { return m_d3dsdBackBuffer; }
	void SetSkinTranslationInfo(D3DXVECTOR3& vSkinPos, float fRotate);
	void GetSkinTranslationInfo(D3DXVECTOR3& IN OUT vOutput, float& IN OUT fRotate ) const;
};

#ifndef _DEBUG  // debug version in CharEditView.cpp
inline CCharEditDoc* CCharEditView::GetDocument()								{ return (CCharEditDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHAREDITVIEW_H_)
