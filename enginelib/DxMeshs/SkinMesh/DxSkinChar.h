#pragma once

#include "./_new/DxSkinMesh9.h"
#include "DxSkinObject.h"
#include "DxSkinAnimationManager.h"
#include "../../DxMeshs/SkinMesh/DxSkinPieceContainer.h"
#include "DxSkinCharData.h"
#include "DxAttBone.h"
#include "DxAttBoneLink.h"
#include "DxAttBoneData.h"
#include "DxCharPart.h"
#include "../../DxMeshs/SkinMesh/DxSkinEffResult.h"

#include "../../DxEffect/EffAni/DxEffAniPlayer.h"
#include "../../DxEffect/EffAni/DxEffAni.h"
#include "../../DxEffect/EffKeep/DxEffKeepPlayer.h"
#include "./DxSkinCharBase.h"
#include "./DxSkinDefine.h"
#include "./DxBillboard.h"

class DxVehicle;
class DxLandMan;
class DxEffKeepDataMain;
class DxSkinAniControlThreadNeedData;
struct DxVehicleThreadNeedData;



// Note : ĳ����
class DxSkinChar : public DxSkinCharBase, DxSkinCharEffAniBase
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
	DxCharPart		m_PartArray[PIECE_SIZE];
	DxAttBoneLink*	m_pAttBoneArray[PIECE_SIZE];		// �޸� ���� ������ Pointer�� ������. �� �κп��� 1M �� ����ϰ� �־���.
	float			m_fHeightYPos;
    BOOL            m_bCharAmbientUP;
	DWORD			m_dwFlags;			// EMSCD_FLAGS
	BOOL			m_bAmbientColor;		// AmbientColor �� ����ϴ°�~?

private:
	DWORD			m_dwVertexNUM;
	BOOL			m_bCompleted_PartLoad_FirstCreate;	// �� DxCharPart�� �ε��۾��̿Ϸᰡ �Ǿ���.	[ ó���������� ���� Ȯ�εȴ�. ���� Part �� �����ϴ� �Ͱ��� ��������. ]
	D3DXMATRIX		m_matWorld;

	// ���� scale
	float			m_arrayfWeaponScale[EMPEACE_WEAPON_SIZE];

	boost::shared_ptr<BILLBOARD_CHF>		m_spBillboard;			// Billboard (NPC, MOB )

private:	
	SKIN_SPEC_SSS_INFO	m_sSkinInfo;

private:
	std::tr1::shared_ptr< DxFootColl >		m_smpFootColl;			// ������ �Ѵ�.

private:
	std::tr1::shared_ptr<DxSkinAniControl>	m_spSkinAniControlThread;	// �����Ǵ� ��ġ�� ��������, �ֿ� ����� Thread ���� �Ͼ�� ������ ���� �����ؾ� �Ѵ�.
																		// ����� �κ� Thread. ���� ������ Thread�� �Ǹ� RefCount �� ���� ���ɼ��� �����Ƿ� ��Ӵ�� �̷��� �����Ѵ�.

	//float			m_afKeyJiggleWeight[PIECE_SIZE+1];	// KeyFrame �� JiggleBone �� ����ġ ��.	 PIECE_SIZE �� �⺻ Skin Bone. 0 ���ʹ� Parts~
	std::tr1::shared_ptr<DxSkinAniControlThreadNeedData>	m_spThreadNeedData;	// Thread �󿡼� ���Ǳ� ������ shared_ptr �� ��.

public:
	HRESULT ClearAll ();

public:
	HRESULT SetPiece( 
		const TCHAR* szFile,
		LPDIRECT3DDEVICEQ pd3dDevice,
		DWORD dwFlag, 
		int nLevel, 
		EMSCD_FLAGS emFlags,
		BOOL bAttachSub=FALSE,
		EMPIECECHAR* pOutPiecetype=NULL,
		const char* pSelfBodyEffect=NULL );

	HRESULT SetPiece( 
		const std::string& strFile,
		LPDIRECT3DDEVICEQ pd3dDevice,
		DWORD dwFlag, 
		int nLevel, 
		EMSCD_FLAGS emFlags,
		BOOL bAttachSub=FALSE,
		EMPIECECHAR* pOutPiecetype=NULL,
		const char* pSelfBodyEffect=NULL,
		const EMPIECECHAR* pCheckPieceType=NULL );

	HRESULT SetPieceHiddenWeapon( 
		const std::string& strFile,
		LPDIRECT3DDEVICEQ pd3dDevice,
		int nLevel, 
		EMSCD_FLAGS emFlags,
		OUT EMPIECECHAR& emType,
		const char* pSelfBodyEffect=NULL );

	//HRESULT SetPiece ( const TCHAR* szFile, LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwFlag = 0x0, int nLevel=0, bool bCharacterData = FALSE, BOOL bAttachSub = FALSE, EMPIECECHAR* pOutPiecetype = NULL );
	HRESULT SetPieceGrindLevel ( int nIndex, int nLevel );
	void SetHairColor( WORD wColor );
	WORD GetHairColor();

	PDXATTBONELINK  GetAttachBone ( int i );
	PDXCHARPART GetPiece ( int i );
	void ResetPiece ( int i );
	void SetPartRend ( EMPIECECHAR emPIECE, BOOL bRender );

	const TCHAR* GetFileName() const	OVERRIDE		{ return m_szFileName; }
	const TCHAR* GetBoneName() const	OVERRIDE;

	void SetWorldObj( BOOL bUse )		{ m_bWorldObj = bUse; }
	BOOL GetWorldObj()					{ return m_bWorldObj; }
	const DxFootColl*	GetFootColl()	{ return m_smpFootColl.get(); }

	void SetCharData ( DxSkinCharData* pCharData, LPDIRECT3DDEVICEQ pd3dDevice, bool bAmbientUP, DWORD dwFlags, BOOL bMaintainAnim = FALSE );	// dwFlags - EMFLAGS_SetCharData
	void SetRadiusHeight( DxSkinCharData* pCharData );
    void ReloadVMFX( LPDIRECT3DDEVICEQ pd3dDevice );

    /**
        ��ü�� �������� ���� �Ѵ�.
        �⺻������ ĳ���� ������ ������ �����Ͽ� ����� ���� �Ѵ�.
        �ܼ��� �����ϸ� �ø��� ���� �ƴ϶� �ø� ������ ��ŭ
        �����Ͽ� ����� �����͵� AABB, Height, HeightPos �� ���� �ø���.
        ���� �����Ͽ� ���� ���� �÷���� �Ҳ� �ִٸ� �� ���� �־� �ش�.

     */
    void SetInteractiveScale(const float fScale);

	BOOL IsUseWorldColor();

	const D3DXMATRIX&	GetSkeletonRootMatrix() const;

	BOOL IsExistBillboard()		{ return m_spBillboard.get() ? TRUE : FALSE; }

	//////////////////////////////////////////////////////////////////////////
	// DxSkinAniControl
public:
	void DEFAULTANI() 										{ m_spSkinAniControlThread->DEFAULTANI(); }
	void StopLoopSound() 									{ m_spSkinAniControlThread->StopLoopSound(); }
	void SELECTANI_EDIT( const TCHAR* szAnim
		, BOOL bSameAniRefresh = TRUE
		, DWORD dwFlag = 0)				
	{ m_spSkinAniControlThread->SELECTANI_EDIT( szAnim, bSameAniRefresh, dwFlag ); }


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

	BOOL IsANI_ISENDANIM ( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType ) const
	{
		if ( m_spSkinAniControlThread->GETCURMTYPE() != MType )
			return FALSE;

		if ( m_spSkinAniControlThread->GETCURSTYPE() != SType )
			return FALSE;

		return m_spSkinAniControlThread->ISENDANIM();
	}

	BOOL IsANI_ISENDANIM ( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType, DWORD dwSelect ) const
	{
		if ( m_spSkinAniControlThread->GETCURANIMNODE() != m_spSkinAniControlThread->GETANI_none_const( MType, SType, dwSelect ) )
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

	void SELECTSKILLANI ( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType=AN_SUB_NONE )
	{
		m_spSkinAniControlThread->SELECTANI( MType, SType );
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

	void SETCURTIME_ALL_FOR_EDIT(float fTime)
	{
		m_spSkinAniControlThread->SETCURTIME_ALL_FOR_EDIT(fTime);
	}

public:
	friend class CFramePage;
	friend class CEditState;
	friend class CCharPage;
	friend class CBoneEdit;
	friend class CSoundPage;
	friend class CAnimPage;
	friend class CAniDivPage;
	friend class CAnimSetPage;
	friend class CFlyCameraEditPage;
	friend class CStrikePage;
	friend class CBoardEditDlg;
	friend class CCharEditView;
	friend class CCharFileState;
	friend class CClothCollPage;
	friend class CsheetWithTabChar;
	friend class CAniBoneScalePage;
	friend class CAniJiggleBlendPage;
	friend class CEffAniPage;
	friend class CEffAniSinglePage;
	friend class CEffAniGhostingPage;
	friend class CJiggleBonePage;
	friend class CJiggleBoneCollPage;
	friend class CEffGroupPage;
	friend class CEffectNamePage;
	friend class CsheetWithTabAnim;
	friend class CMultianimPage;



public:
	void	ResetStrikeAttachBone();
	void	UpdateStrikeAttachBone( const DWORD dwThisKey, const int nattSTEP );
	void	UpdateNonStrikeAttachBone();

	void	SET_ATTBONE_CURTIME_ALL_FOR_EDIT( float fCurTime );


private:
	HRESULT FrameMoveCore( const float fTime, 
							const float fElapsedTime,
							const BOOL bContinue, 
							BOOL const bFreeze, 
							const BOOL bAttackMode, 
							const BOOL bBoardRender,
							BOOL bNoneOverFrame );

public:
	// ĳ���Ͱ� Vehicle �� ������ ���.
	HRESULT FrameMove( const float fTime, 
						const float fElapsedTime,
						BOOL bNoneOverFrame,
						const D3DXMATRIX &matRot, 
						const BOOL bContinue=TRUE, 
						BOOL const bFreeze=FALSE, 
						const BOOL bAttackMode=TRUE, 
						const BOOL bBoardRender = FALSE );

	// ĳ���Ͱ� Vehicle �� Ÿ�� ���� ���.
	HRESULT FrameMoveCharOnVehicle( const float fTime, 
									const float fElapsedTime,
									const BOOL bContinue,
									BOOL const bFreeze,
									const BOOL bAttackMode,
									DxVehicle* pVehicle,
									DWORD dwVehiclePassengerID,
									BOOL bNoneOverFrame );

	// // [shhan][2014.12.18] GLCharClient ���� FrameMove �� ȣ�� ���� �����鼭 PlayReserveFuction �ҷ����� �ʾ� ClearReserveFuction �� ������ ȣ���ϵ��� ��.
	void ClearReserveAction();

	void SetDiffuseColor_NSMaterialSkinManager();
	void SetLookatLength_NSMaterialSkinManager();
	void SetPosition_NSMaterialSkinManager();

	// ������ �ʼ� ��Ұ� �ִ°�~?
	BOOL IsValidForRender();

	void SetPosOffset( const D3DXVECTOR3& vPosOffset );

	// const BOOL bShadow=FALSE, const BOOL bEff=TRUE, const BOOL bReverse = FALSE, const BOOL bBoardRender = FALSE, BOOL bFogON = FALSE
	virtual HRESULT RenderFixed( const LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPointLightRT, float fAlpha, const BOOL bShadow=FALSE, const BOOL bEff=TRUE, 
						const BOOL bReverse = FALSE, const BOOL bBoardRender = FALSE, BOOL bFogON = FALSE ) override;

	HRESULT RenderShadow( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matViewProj, const BOOL bDay );
	virtual HRESULT RenderReflect( const LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha ) override;

	virtual void RenderMaterialOpaque( const LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha ) override;
	virtual void RenderMaterialHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha ) override;
	virtual void RenderMaterialSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha ) override;
	virtual void RenderMaterialEffect( LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha ) override;

	//template<class TYPE>
 //   HRESULT RenderEX( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX &matRot, const std::string& BoneName, const TYPE& BONEROT )
	//{
	//	D3DXMATRIXA16	matLocalRot, matScale;
	//	D3DXMatrixIdentity( &matScale );
	//	matScale._11 = m_fScale;
	//	matScale._22 = m_fScale;
	//	matScale._33 = m_fScale;
	//	D3DXMatrixMultiply( &matLocalRot, &matScale, &matRot );
	//
	//	//	Note : Skin Animation.
	//	DxSkinAniControl::Render( matLocalRot, BoneName, BONEROT );

	//	RenderSKIN( pd3dDevice, matLocalRot );

	//	return S_OK;
	//}
	//HRESULT RenderSKIN( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matLocalRot );
	void RenderBone( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderAniEff ( const LPDIRECT3DDEVICEQ pd3dDevice );
	BOOL RenderBillboard( const D3DXMATRIX& matTrans, float fBillboardAlpha );

	void SetPart_Detail();

	void GetStaticVertexData( std::vector<VERTEXNORCOLORTEX4>& vecVertices, const D3DXMATRIX& matWorld );

    void UpdateLOD( const D3DXVECTOR3& vPos, LPDIRECT3DDEVICEQ pd3dDevice );

	void SetWorldBattleFlag( const TCHAR* pWorldBattleFlag );

	// chf Billboard
	// szNameCHF_bSaveBillboard - ���� �Ѿ���� Billboard ����. chf Name �� dds �� �����Ͽ�. billboard texture ����.
	void CreateChfBillboard( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bScreenRender, const TCHAR* szNameCHF_bSaveBillboard );





	//////////////////////////////////////////////////////////////////////////
	//						AnimationMove
	//////////////////////////////////////////////////////////////////////////

public:
	BOOL IsAnimationMove();								// �ִϸ��̼� �������� ĳ���Ϳ� ������ �޴� ������ Ȯ��
	const D3DXVECTOR3& GetAnimationMovePos();			// ĳ���Ͱ� �ִϸ��̼����� �󸶸�ŭ ���������� Ȯ��
	//const D3DXMATRIX& GetAnimationMoveStartMatrix();	// AnimationMove �� ���� Matrix

public:
	D3DXVECTOR3 GetAnimationMoveOffsetFull_EDIT();

	//////////////////////////////////////////////////////////////////////////	





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

	//WorldBattle
public:
	//! �¸��� Ŭ���� ���⸦ �ʿ� �����Ѵ�.
	//void SetFlagWinnerClubCountry( const TSTRING& stTextureName, const D3DXVECTOR4& vFlagConvert );

public:
	HRESULT InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects ();



	//////////////////////////////////////////////////////////////////////////
	// �ܺ� �ɼǿ� ���� ON/OFF �� ���� ���� Data ��ȭ�� ȣ�� �Լ� �� ����
private:
	BOOL m_bPhysX_Cloth_OnOff;		// m_bCompleted_PhysX_Cloth �� TRUE �� ���õǾ��� ��.
	BOOL m_bCompleted_PhysX_Cloth;	// ���ϴ� ������� �� ���õǾ��ٸ� ��ġ�� TRUE �� �Ѵ�.
									// ó�� �����ų� Part �� ������ �� FALSE �� �ٲ��ش�.

public:
	// �̰��� �� ������ �������� �Ѵ�.
	virtual void SetPhysX_Cloth( BOOL bOnOff ) override;
	// �ܺ� �ɼǿ� ���� ON/OFF �� ���� ���� Data ��ȭ�� ȣ�� �Լ� �� ����
	//////////////////////////////////////////////////////////////////////////



public:
	friend class CSkinInfoPage;

public:
	DxSkinChar(void);
	virtual ~DxSkinChar(void);
};




