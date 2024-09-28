#pragma once

#include "./_new/DxSkinMesh9.h"
#include "DxSkinObject.h"
#include "DxSkinAnimationManager.h"
#include "../../DxMeshs/SkinMesh/DxSkinPieceContainer.h"
#include "DxVehicleData.h"
#include "DxAttBone.h"
#include "DxAttBoneData.h"
#include "DxAttBoneLink.h"
#include "DxCharPart.h"
#include "../../DxMeshs/SkinMesh/DxSkinEffResult.h"

#include "../../DxEffect/EffAni/DxEffAniPlayer.h"
#include "../../DxEffect/EffAni/DxEffAni.h"
#include "../../DxEffect/EffKeep/DxEffKeepPlayer.h"
#include "./DxSkinCharBase.h"

class DxLandMan;
class DxEffKeepDataMain;
class DxFootColl;
class DxSkinAniControlThreadNeedData;
struct DxVehicleThreadNeedData;

// 탈것 관련 포멧
class DxVehicle : public DxSkinCharBase, DxSkinCharEffAniBase
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
	std::tr1::shared_ptr< DxFootColl >	m_smpFootColl;			// 참조만 한다.
	D3DXMATRIX m_matWorld;

public:
	SKINEFFDATA		m_sSkinEffData;

	//SPassengerData	m_sPassengerData[MAX_PASSENGER_COUNT];
	std::tr1::shared_ptr<DxVehicleThreadNeedData>	m_spVehicleThreadNeedData;

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
	DxCharPart		m_V_PartsArray[PART_V_SIZE];
	DxAttBoneLink*	m_p_V_AttBoneArray[PART_V_SIZE];	// 메모리 문제 때문에 Pointer로 변경함. 이 부분에서 600kb 정도는 사용하고 있었다.
	BOOL			m_bCharAmbientUP;
	BOOL			m_bAmbientColor;	// AmbientColor 를 사용하는가~?
	D3DXVECTOR3		m_vDiffuseColor;	// AmbientColor 값은~?

private:
	DWORD			m_dwVertexNUM;

private:
	std::tr1::shared_ptr<DxSkinAniControl>	m_spSkinAniControlThread;		// 생성되는 위치는 여기지만, 주요 계산은 Thread 에서 일어나기 때문에 사용시 조심해야 한다.

private:
	//float			m_afKeyJiggleWeight[PART_V_SIZE+1];	// KeyFrame 과 JiggleBone 의 가중치 값.	 PART_V_SIZE 은 기본 Skin Bone. 0 부터는 Parts~
	std::tr1::shared_ptr<DxSkinAniControlThreadNeedData>	m_spThreadNeedData;	// Thread 상에서 사용되기 때문에 shared_ptr 로 됨.

public:
	HRESULT ClearAll ();

private:
	BOOL			m_bCompleted_PartLoad_FirstCreate;	// 각 DxCharPart의 로딩작업이완료가 되었다.	[ 처음생성했을 때만 확인된다. 추후 Part 를 변경하는 것과는 연관없다. ]

private:
	void SetDiffuseColor_NSMaterialSkinManager();
	void SetLookatLength_NSMaterialSkinManager();
	void SetPosition_NSMaterialSkinManager();

public:
	const DxFootColl*	GetFootColl()	{ return m_smpFootColl.get(); }

	const SPassengerData* GetPassengerData( int i ) const;
	BOOL IsValidPassengerMatrixData() const;

	PDXCHARPART     GetVehiclePart(int i);
	PDXATTBONELINK  GetAttachBone(int i);

	void ResetVehiclePart ( int i );
	void ResetVehicle ();

	const TCHAR* GetFileName() const	OVERRIDE		{ return m_szFileName; }
	const TCHAR* GetBoneName() const	OVERRIDE;

	//DxEffAniPlayer* GetEffAniPlayer()	{ return &m_EffAniPlayer; }
	void SetWorldObj( BOOL bUse )		{ m_bWorldObj = bUse; }
	BOOL GetWorldObj()					{ return m_bWorldObj; }

	HRESULT SetPart ( const TCHAR* szFile, LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwFlag = 0x0, int nLevel=0, EMSCD_FLAGS emFlags=EMSCD_ZERO, bool bCharacterData = FALSE, BOOL bAttachSub = FALSE  );
	void SetVehiclePartData ( DxVehicleData* pVehiclePartData, LPDIRECT3DDEVICEQ pd3dDevice, bool bAmbientUP = FALSE, EMSCD_FLAGS emFlags = EMSCD_ZERO );
	void SetRadiusHeight( DxVehicleData* pVehiclePartData );


	DWORD GetVertexNUM()				{ return m_dwVertexNUM; }



	//////////////////////////////////////////////////////////////////////////
	// DxSkinAniControl
public:

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

	BOOL IsANI_ISENDANIM ( const TCHAR* szAnim ) const
	{
		if ( !m_spSkinAniControlThread->IsAnimName( szAnim ) )
			return FALSE;

		return m_spSkinAniControlThread->ISENDANIM();
	}

	BOOL IsANI_ISENDANIM ( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType=AN_SUB_NONE ) const
	{
		if ( m_spSkinAniControlThread->GETCURMTYPE() != MType )
			return FALSE;

		if ( m_spSkinAniControlThread->GETCURSTYPE() != SType )
			return FALSE;

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
	friend class CVehiclePage;
	friend class CEditState;
	friend class CVehicleState;
	friend class CSoundPage;
	friend class CAnimPage;
	friend class CAniDivPage;
	friend class CAnimSetPage;
	friend class CStrikePage;
	friend class CBoardEditDlg;
	friend class CCharEditView;
	friend class CClothCollPage;
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
	//void	UpdatePassengerPos();	
	void	ResetStrikeAttachBone();
	void	UpdateStrikeAttachBone( const DWORD dwThisKey, const int nattSTEP );
	void	UpdateNonStrikeAttachBone();

	void	SET_ATTBONE_CURTIME_ALL_FOR_EDIT( float fCurTime );

	HRESULT FrameMove( const float fTime, const float fElapsedTime, BOOL bNoneOverFrame, const D3DXMATRIX &matRot, const BOOL bContinue=TRUE, BOOL const bFreeze=FALSE );

	// 렌더시 필수 요소가 있는가~?
	BOOL IsValidForRender();

	virtual HRESULT RenderFixed( const LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPointLightRT, float fAlpha, const BOOL bShadow=FALSE, const BOOL bEff=TRUE, 
								const BOOL bReverse = FALSE, const BOOL bBoardRender = FALSE, BOOL bFogON = FALSE ) override;
	HRESULT RenderShadow( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matViewProj, const BOOL bDay );
	virtual HRESULT RenderReflect( const LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha ) override;

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

	/*
	virtual HRESULT RenderFixed( const LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPointLightRT, float fAlpha, const D3DXMATRIX &matRot, const BOOL bShadow=FALSE, const BOOL bEff=TRUE, 
	const BOOL bReverse = FALSE, const BOOL bBoardRender = FALSE, BOOL bFogON = FALSE ) override;
	*/

	// , const BOOL bShadow=FALSE, const BOOL bEff=TRUE
	virtual void RenderMaterialOpaque( const LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha ) override;
	virtual void RenderMaterialHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha ) override;
	virtual void RenderMaterialSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha ) override;
	virtual void RenderMaterialEffect( LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha ) override;


	const float GetScale() { return m_fScale; }

public:
	virtual HRESULT RenderShadowForMaterialSystem( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matViewProj ) override;

	virtual BOOL RenderMaterialGhosting( LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha, size_t unGhostingIndex ) override;

	virtual void GhostingON( BOOL bGhosting, float fLifeTime ) override;

	virtual void GetAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin ) const override
	{
		vMax = m_vMax;
		vMin = m_vMin;
	}

	virtual DWORD GETCURKEYTIME () const override
	{
		return m_spSkinAniControlThread->GETCURKEYTIME();
	}

public:
	HRESULT InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects ();



	//////////////////////////////////////////////////////////////////////////
	// 외부 옵션에 따른 ON/OFF 로 인한 내부 Data 변화시 호출 함수 및 변수
private:
	BOOL m_bPhysX_Cloth_OnOff;		// m_bCompleted_PhysX_Cloth 가 TRUE 때 셋팅되어진 값.
	BOOL m_bCompleted_PhysX_Cloth;	// 원하는 정보대로 다 셋팅되었다면 수치를 TRUE 로 한다.
									// 처음 들어오거나 Part 를 변경할 시 FALSE 로 바꿔준다.

public:
	// 이곳은 매 프레임 들어오도록 한다.
	virtual void SetPhysX_Cloth( BOOL bOnOff ) override;
	// 외부 옵션에 따른 ON/OFF 로 인한 내부 Data 변화시 호출 함수 및 변수
	//////////////////////////////////////////////////////////////////////////



public:
	DxVehicle(void);
	virtual ~DxVehicle(void);
};

typedef DxVehicle*	PDXVEHICLE;



