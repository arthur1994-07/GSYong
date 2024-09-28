#pragma once

#include "./_new/DxSkinMesh9.h"
#include "./DxSkinObject.h"
#include "./DxSkinAnimationManager.h"
#include "./DxSkinPieceContainer.h"
#include "./DxPieceDefine.h"
#include "./DxAttBoneData.h"
#include "./DxCharPart.h"
#include "./DxSkinEffResult.h"
#include "./DxSkinCharBase.h"

#include "../../DxEffect/EffAni/DxEffAniPlayer.h"
#include "../../DxEffect/EffAni/DxEffAni.h"
#include "../../DxEffect/EffKeep/DxEffKeepPlayer.h"

class DxClothColl;
class DxEffKeepDataMain;
struct DxAttBoneLinkThreadNeedData;


// Note : 어태치 본
class DxAttBone : public DxSkinCharEffAniBase
{
private:
	enum		{ FILE_LENGTH = 64 };

	BOOL			m_bWorldObj;
	float			m_fScale;

	//DxEffAniPlayer	m_EffAniPlayer;

	//------------------------------------------------------------
	//				E	f	f		K	e	e	p
private:
	DxEffKeepPlayer	m_EffKeepPlayer;

public:
	SKINEFFDATA		m_sSkinEffData;

public: 
	void CreateEff( DxEffKeepDataMain* pEff );
	void DeleteEff( DxEffKeepDataMain* pEff );
	void Reset();

public:
	static DWORD				g_dwLOD;
	static DWORD				g_dwHIGHDRAW_NUM;

public:
	TCHAR			m_szFileName[FILE_LENGTH];
	float			m_fHeight;
	D3DXVECTOR3		m_vMax, m_vMin;
	DxCharPart		m_AttBoneArray[ATTBONE_SIZE];

private:
	DWORD			m_dwVertexNUM;

private:
	std::tr1::shared_ptr<DxSkinAniControl>	m_spSkinAniControlThread;	// 생성되는 위치는 여기지만, 주요 계산은 Thread 에서 일어나기 때문에 사용시 조심해야 한다.
																		// 현재는 부분 Thread. 만약 전구간 Thread가 되면 RefCount 가 쓰일 가능성이 있으므로 상속대신 이렇게 유지한다.

public:
	HRESULT ClearAll ();

public:
	
	PDXCHARPART GetAttBonePiece ( int i )		{ return &m_AttBoneArray[i]; }
	void ResetAttBonePiece ( int i );

	void ResetAttBone ();

	const TCHAR* GetFileName() const	OVERRIDE		{ return m_szFileName; }
	const TCHAR* GetBoneName() const	OVERRIDE;

	void SetWorldObj( BOOL bUse )		{ m_bWorldObj = bUse; }
	BOOL GetWorldObj()					{ return m_bWorldObj; }

	void SetAttBoneData ( DxAttBoneData* pAttBoneData, LPDIRECT3DDEVICEQ pd3dDevice, bool bAmbientUP = FALSE, int nLevel=0, EMSCD_FLAGS emFlags=EMSCD_ALL_THREAD, const char* pSelfBodyEffect=NULL );
	void SetRadiusHeight( DxAttBoneData* pAttBoneData );


	DWORD GetVertexNUM()				{ return m_dwVertexNUM; }

	void SetWorldBattleFlag( const TCHAR* pWorldBattleFlag );



	//////////////////////////////////////////////////////////////////////////
	// DxSkinAniControl
public:

	// Only AttBone
	void SETCURTIME_ALL_FOR_EDIT( float fCurTime ) const			
	{ 
		return m_spSkinAniControlThread->SETCURTIME_ALL_FOR_EDIT( fCurTime ); 
	}

	
	// const
	virtual const DxSkeletonMaintain* GetSkeletonMaintain() const override
	{ 
		return m_spSkinAniControlThread->GetSkeletonMaintain(); 
	}

	const PANIMCONTNODE GETANI( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType=AN_SUB_NONE ) const
	{
		return m_spSkinAniControlThread->GETANI( MType, SType );
	}

	const ANIMCONTNODE*	findanicontnode(const std::string& AnimNode) const
	{
		return m_spSkinAniControlThread->findanicontnode( AnimNode );
	}

	const PANIMCONTNODE	GETCURANIMNODE() const
	{
		return m_spSkinAniControlThread->GETCURANIMNODE();
	}

	const DxSkinAnimation*	GETCURANIM() const
	{
		return m_spSkinAniControlThread->GETCURANIM();
	}

	DWORD GETANIAMOUNT ( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType ) const
	{
		return m_spSkinAniControlThread->GETANIAMOUNT( MType, SType );
	}

	float GETENDTIME () const
	{
		return m_spSkinAniControlThread->GETENDTIME();
	}

	float GETCURTIME() const
	{ 
		return m_spSkinAniControlThread->GETCURTIME(); 
	}

	float GETCURANITIME () const
	{
		return m_spSkinAniControlThread->GETCURANITIME();
	}

	BOOL ISENDANIM () const
	{
		return m_spSkinAniControlThread->ISENDANIM();
	}

	virtual DWORD GETUNITTIME () const override
	{
		return m_spSkinAniControlThread->GETUNITTIME();
	}

	BOOL IsCurAniConInfo(const EMANICONINFO emAniConInfo) const
	{
		return m_spSkinAniControlThread->IsCurAniConInfo( emAniConInfo );
	}

	EMANI_MAINTYPE	GETCURMTYPE () const
	{
		return m_spSkinAniControlThread->GETCURMTYPE();
	}

	EMANI_SUBTYPE	GETCURSTYPE () const
	{
		return m_spSkinAniControlThread->GETCURSTYPE();
	}

	bool IsAni(EMANI_MAINTYPE MType, EMANI_SUBTYPE SType=AN_SUB_NONE) const
	{
		return m_spSkinAniControlThread->IsAni( MType, SType );
	}


	// None const
	BOOL IsANI_SELECTANI ( const TCHAR* szAnim, DWORD dwFlags=NULL )
	{
		if( findanicontnode( std::string(szAnim) ) )
		{
			m_spSkinAniControlThread->SELECTANI( szAnim, dwFlags );
			return TRUE;
		}
		return FALSE;
	}

	void SELECTANI ( const TCHAR* szAnim, DWORD dwFlags=NULL )
	{
		m_spSkinAniControlThread->SELECTANI( szAnim, dwFlags );
	}

	BOOL IsANI_SELECTANI( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType=AN_SUB_NONE, DWORD dwFlags=NULL, DWORD dwSelect=0 )
	{
		if( GETANI( MType, SType ) )
		{
			m_spSkinAniControlThread->SELECTANI( MType, SType, dwFlags, dwSelect );
			return TRUE;
		}
		return FALSE;
	}

	void SELECTANI( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType=AN_SUB_NONE, DWORD dwFlags=NULL, DWORD dwSelect=0 )
	{
		m_spSkinAniControlThread->SELECTANI( MType, SType, dwFlags, dwSelect );
	}

	void TOSTARTTIME ()
	{
		m_spSkinAniControlThread->TOSTARTTIME();
	}

	void TOENDTIME ()
	{
		m_spSkinAniControlThread->TOENDTIME();
	}

	void SETCURTIME( float fCurTime ) 
	{ 
		m_spSkinAniControlThread->SETCURTIME( fCurTime ); 
	}

	void SetPosition ( const D3DXVECTOR3& vPos )
	{	
		m_spSkinAniControlThread->SetPosition( vPos );
	}

	void DOSHOCKMIX ()
	{
		m_spSkinAniControlThread->DOSHOCKMIX();
	}

public:
	friend class CFramePage;
	friend class CEditState;
	friend class CAttBonePage;
	friend class CAttachBoneState;
	friend class CAttachBoneLinkState;
	friend class CSoundPage;
	friend class CAnimPage;
	friend class CAniDivPage;
	friend class CAnimSetPage;
	friend class CStrikePage;
	friend class CCharEditView;
	friend class CClothCollPage;
	friend class CAttachLinkPage;
	friend class CsheetWithTabChar;
	friend class CAniBoneScalePage;
	friend class CAniJiggleBlendPage;
	friend class CEffAniPage;
	friend class CEffAniSinglePage;
	friend class CEffAniGhostingPage;
	friend class CJiggleBonePage;
	friend class CJiggleBoneCollPage;
	friend class CEffectNamePage;
	friend class CsheetWithTabAnim;



public:
	HRESULT FrameMove( const float fTime, 
						const float fElapsedTime, 
						std::tr1::shared_ptr<DxSkinAniControl>& spLinkSkinAniControl,
						std::tr1::shared_ptr<DxAttBoneLinkThreadNeedData>& spThreadNeedData, 
						BOOL bNoneOverFrame,
						float fKeyJiggleWeight,
						const BOOL bContinue=TRUE, 
						BOOL const bFreeze=FALSE );

	HRESULT FrameMoveForTool( const float fTime, const float fElapsedTime, const D3DXMATRIX& matWorld );

	// 렌더시 필수 요소가 있는가~?
	BOOL IsValidForRender();

	// const BOOL bShadow=FALSE, const BOOL bEff=TRUE, const BOOL bReverse = FALSE
	HRESULT RenderFixed( const LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha, const D3DXMATRIX &matRot, const BOOL bShadow, const BOOL bEff, const BOOL bReverse );

	HRESULT RenderShadow( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX &matRot, const D3DXMATRIX& matViewProj, const BOOL bDay );

public:
	void SetPosOffset( const D3DXVECTOR3& vPosOffset );

	HRESULT RenderShadowForMaterialSystem( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matViewProj );

	virtual BOOL RenderMaterialGhosting( LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha, size_t unGhostingIndex ) override;

	virtual void GhostingON( BOOL bGhosting, float fLifeTime ) override;

	void GetAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin ) const
	{
		vMax = m_vMax;
		vMin = m_vMin;
	}

	DWORD GETCURKEYTIME () const
	{
		return m_spSkinAniControlThread->GETCURKEYTIME();
	}

	//template<class TYPE>
 //   HRESULT RenderEX( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX &matRot, const std::string& BoneName, const TYPE& BONEROT )
	//{
	//	D3DXMATRIXA16	matLocalRot, matScale;
	//	D3DXMatrixIdentity( &matScale );
	//	matScale._11 = m_fScale;
	//	matScale._22 = m_fScale;
	//	matScale._33 = m_fScale;
	//	D3DXMatrixMultiply( &matLocalRot, &matScale, &matRot );

	//	//	Note : Skin Animation.
	//	DxSkinAniControl::Render( matLocalRot, BoneName, BONEROT );

	//	RenderSKIN( pd3dDevice, matLocalRot );

	//	return S_OK;
	//}
	//HRESULT RenderSKIN( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matLocalRot );
	void RenderBone( LPDIRECT3DDEVICEQ pd3dDevice );	

    void UpdateLOD(const D3DXVECTOR3& vPos, LPDIRECT3DDEVICEQ pd3dDevice);

	// const BOOL bShadow=FALSE, const BOOL bEff=TRUE, const BOOL bReverse = FALSE
	void RenderMaterialOpaque( const LPDIRECT3DDEVICEQ pd3dDevice,
								float fAlpha, 
								const BOOL bShadow, 
								const BOOL bEff, 
								const DxClothColl* pClothColl,
								const std::vector<physx::PxClothCollisionSphere>* pvecClothCollSpheres,
								float fAttParentBoneScale );

	void RenderMaterialHardAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha );

	void RenderMaterialSoftAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha );

	void RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha );

	void RenderMaterialAdd( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderMaterialGlow( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	HRESULT InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects ();


	//////////////////////////////////////////////////////////////////////////
	// 외부 옵션에 따른 ON/OFF 로 인한 내부 Data 변화시 호출 함수 및 변수
public:
	BOOL SetPhysX_Cloth( BOOL bOnOff );


public:
	DxAttBone(void);
	~DxAttBone(void);
};

typedef DxAttBone*	PDXATTBONE;




