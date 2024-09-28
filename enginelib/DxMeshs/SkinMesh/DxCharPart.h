#pragma once

#include "./_new/DxSkinMesh9.h"

#include "./_new/DxSkinMesh9.h"
#include "DxSkinObject.h"
#include "DxSkinAnimationManager.h"
#include "../../DxMeshs/SkinMesh/DxSkinPieceContainer.h"

struct DxSkinPiece;
class DxSkinAniControl;
class DxTraceBone;
class CharPartBoneMatrices;
struct SKINEFFDATA;
struct SKIN_SPEC_SSS_INFO;
struct EffectMaterialData;
class DxBoneTransMaintain;
class MaterialSkinMeshForCloth;
class DxSkinCharEffAniBase;

class DxEffCharData;

// �ɼǿ��� ON �Ǿ� �ִٸ� ��� DxCharPart �� DxPhysXCloth �� �� ����������Ѵ�.
//
// ĳ���� â�� �ִ� DxSkinChar ���� ��� Ȥ �ɼǿ��� OFF �� �Ǿ� �մ� ���� PhysX �� OFF �ǵ� ����� ���� ������
// �� �ؿ� DxCharPart �� ��� �ִ� DxPhysXCloth �� NULL �� ���õǾ�����.
//
class DxPhysXCloth
{
private:
	std::vector<boost::shared_ptr<MaterialSkinMeshForCloth>>	m_vecSkinMeshForCloth;

public:
	void Import( const DxSkinPiece* pSkinPiece );

	MaterialSkinMeshForCloth* GetMaterialSkinMeshForCloth( DWORD dwIndex ) const;

	const MaterialSkinMeshForCloth* GetMaterialSkinMeshForCloth_const( DWORD dwIndex ) const
	{
		return GetMaterialSkinMeshForCloth( dwIndex );
	}

	BOOL IsValid_PxCloth( DWORD dwIndex ) const;

	void Cleanup();

public:
	DxPhysXCloth();
	~DxPhysXCloth();
};


struct DxCharPart
{
private:
	enum		{ FILE_LENGTH = 64 };

    LPDIRECT3DDEVICEQ   m_pd3dDevice;
	DxSkeleton*			m_rSkeleton;

	BOOL                m_bMyChar;
	DWORD				m_dwFlags;			// ���� ��Ʈ�� ȿ���� Flag ���� ���ϰ� ���� ��.
	DWORD				m_dwFlag;			// Shadow �� �Ѹ��� �� �Ѹ� ���� ���ϴ� �ɼ�
	BOOL				m_bCompletedLINK;	// Link�� �Ϸ�Ǿ����� Ȯ��



	//////////////////////////////////////////////////////////////////////////
	// Part �� �ٲ���µ� Thread Loading�̶� �������� ���������� �����Ǿ� �ϴ� ��� 
	//		���� Data�� �Ӹ� �ƴ϶� ���ο� Part �� ������ �� �� �ִ� Data
	//////////////////////////////////////////////////////////////////////////
private:
	struct LoadingChangeData
	{
		// ����� SkinPiece
		DxSkinPiece*		m_rSkinPiece;

		// Note : Ambient �� �Ҵ�. ĳ���� ��⸦ ����Ѵ�.
		BOOL				m_bAmbientON;

		// �߰� Effect
		const char*			m_rSelfBodyEffect;	// ItemEdit ���� ������ ȿ��	// ������ �ص� ���� Data	// Item ������ ������� �ʴ��� ���� ����.

		LoadingChangeData()
			: m_rSkinPiece(NULL)
			, m_bAmbientON(FALSE)
			, m_rSelfBodyEffect(NULL)
		{
		}

		void CleanUp()
		{
			m_rSkinPiece = NULL;
			m_rSelfBodyEffect = NULL;
		}

		void Import( DxSkinPiece* rSkinPiece, BOOL bAmbientON, const char* rSelfBodyEffect )
		{
			m_rSkinPiece = rSkinPiece;
			m_bAmbientON = bAmbientON;
			m_rSelfBodyEffect = rSelfBodyEffect;
		}

		BOOL IsSame( DxSkinPiece* rSkinPiece, BOOL bAmbientON, const char* rSelfBodyEffect )
		{
			if ( m_rSkinPiece == rSkinPiece && m_bAmbientON == bAmbientON && m_rSelfBodyEffect == rSelfBodyEffect )
				return TRUE;

			return FALSE;
		}
	};

	LoadingChangeData	m_sUsedLoadingChangeData;
	LoadingChangeData	m_sBackUpLoadingChangeData;

public:
	// m_pBackUpData�� �������Ѵ�; ������ �ε� �������� ����Ʈ ����/�߰� ��û�� ���� ����ص���;
	// �ε尡 ������ �������ֱ� ����; �ε��� �η� ó����;
	DxEffCharData*		m_pBackUpData;

	TSTRING				m_strNameCPS;
	//char				m_szFileName[FILE_LENGTH];

	const DxSkinCharEffAniBase*	m_rSkinCharEffAniBase;

	DxSkinMesh9*		m_rSkinMesh;

	
	EMPEACEZONEWEAPON	m_emWeaponWhereBack;

	PSMESHCONTAINER		m_rMeshContainer;			// �������� ������ �� �����ͷ� ����. ������ ���� ������ ��������.

	DxSkinPiece*		m_rSkinPiece;			// ������ �Ѵ�.

	DWORD				m_dwMaterialNum;
	//SMATERIAL_PIECE*	m_rarrayMaterialPiece;	// ������ �ϵ��� ����.

    DWORD               m_cEffAmbientType;  // Effect���� Ambient Type�� ���õǸ� ����ȴ�.
    D3DXCOLOR           m_cEffAmbientColor; // Effect���� Ambient Color�� ���õǸ� ����ȴ�.

private:
	BOOL						m_bRender;
	BOOL						m_bAttackMode;
	const DxBoneTransMaintain*	m_pHandHeld;	// ������ġ ��..
	const DxBoneTransMaintain*	m_pSlotHeld;	// ������ġ ����..

	//	Note : ���� ��ġ�� ��.
	VECTRACE			m_vecTrace;

	//	Note : Trace Bone
	DxTraceBone*		m_rTraceBone;		// ������

	//	Note : ��Ų ������ ������ ȿ����.
	GLEFFCHAR_VEC		m_vecEFFECT;

	//	Note : ���� ����.
	int					m_nGrindLevel;

	// Note : ����÷� // ���� ���� Į��
	DWORD				m_dwHairColor;
	DWORD				m_dwSubColor;

	//// Note : �Ϲ� �޽��� ��� ���� Matrix	- Skin �޽��� ���� �ʿ� ���� �����̴�.
	//D3DXMATRIX			m_matCombine;
	
	// WorldBattle
	TSTRING			m_stTextureName;	// ���Ǿ��ϴ� ���� �ؽ����̸�
	D3DXVECTOR4		m_vFlagConvert;		// UV ������ ���� ���Ǵ� ��ġ

	// Note : ���� Back Slot
	TSTRING				m_strUserBackSlot;
	D3DXMATRIX			m_matUserBackSlot;
	BOOL				m_bKeepUserBackSlot;

	// Note : MaterialSystem ���� ���̴� ������
	boost::scoped_ptr<EffectMaterialData>	m_scpEffectMaterialData;

	// Overlay or UserColor �� ���δٸ� ���� �ؽ��� ��� �� �ؽ��ĸ� ������ ����.
	// MakeOverlayTex �� CreateEffRenderTarget ���� ���δ�.
	LPDIRECT3DTEXTUREQ*	m_apOverlayTex;		// OverlayTexture

	// Thread �󿡼� �۾��ȴ�.
	std::tr1::shared_ptr<CharPartBoneMatrices>	m_spCharPartBoneMatrices;

	// StaticBone �� ����� Matrix
	D3DXMATRIX m_matStaticLinkBone;

	// SafeMode �� ���Ⱑ EMPEACE_WEAPON_WHIP_SLOT, EMPEACE_WEAPON_BOX_SLOT, EMPEACE_WEAPON_UMBRELLA_SLOT ��� �̸� ���� ��ġ�� ������ �ֵ��� �Ѵ�.
	WORDWORD m_wwMoveParentBoneIDForSafeModeWeaponSlot;

	// ���ڿ� ������ ����. �� ��� �� ���ΰ�~?
	HAT_HAIR_OPTION m_emHairHairOption;
	TSTRING			m_strAddHairCps;

	//// Bone Matrices �� ������ ���� ������������ ����ϵ��� �Ѵ�.
	//// Opaque, Hard Alpha, Soft Alplha, Shadow, Glow
	//// �迭�� => m_pmcMesh->m_dwNumAttributeGroups * m_pmcMesh->m_dwNumPaletteEntries
	////
	//// Thread Loading ������ m_arrayBoneMatrices �� NULL �� ���·� Render �� �̰� ���� ���� �� �ִ�. ���� NULL üũ �� �������.
	////
	//D3DXMATRIX*	m_arrayBoneMatrices;

	//// �ݻ��ʿ��� ����.
	////
	//// Thread Loading ������ m_arrayBoneMatricesReflect �� NULL �� ���·� Render �� �̰� ���� ���� �� �ִ�. ���� NULL üũ �� �������.
	////
	//D3DXMATRIX*	m_arrayBoneMatricesReflect;


public:
	const D3DXMATRIX& GetStaticLinkBone() const		{ return m_matStaticLinkBone; }

	void SetGrindLevel ( int nLevel )		{ m_nGrindLevel = nLevel; }
	int GetGrindLevel () const				{ return m_nGrindLevel; }

	void SetHairColor( DWORD dwColor, BOOL bMakeOverlayTex=TRUE );
	DWORD GetHairColor() const				{ return m_dwHairColor; }

	void SetSubColor( DWORD dwColor, BOOL bMakeOverlayTex=TRUE );
	DWORD GetSubColor() const				{ return m_dwSubColor; }

	inline BOOL IsCompletedLINK() const				{ return m_bCompletedLINK; }

	void MakeOverlayTex();
	void GetDefaultOverlayColor( DWORD& dwMainColor, DWORD& dwSubColor );

	// [shhan][2015.04.01] ����� World Battle �� �ؽ��ĸ� ���ؼ� ���ǰ� �ִ�.
	//						���� �߰� �� ���ɼ��� �ִ�.
	//						m_apOverlayTex �� MakeOverlayTex �� �������� ����Ѵ�.
	//void CreateEffRenderTarget();

	const D3DXMATRIXA16*	GetBoneMatrices() const;//	{ return m_arrayBoneMatrices; }

	const DxSkinPiece* GetSkinPiece() const		{ return m_rSkinPiece; }

	// 
	void GetMoveParentBoneIDForSafeModeWeaponSlot( WORDWORD& wwOut );

	// ���ڿ� ������ ����. �� ��� �� ���ΰ�~?
	HAT_HAIR_OPTION GetHatHairOption() const							{ return m_emHairHairOption; }
	BOOL			IsHatHairOption( HAT_HAIR_OPTION emType ) const		{ return m_emHairHairOption==emType ? TRUE : FALSE; }

	// ���� ���� �߰��� cps �� ������ �� �� �ֵ��� �Ѵ�.
	// ������ �ϺθӸ�ī���� ǥ���ϱ� ���� cps �� ���δ�.
	// m_bHairRenderOff �� TRUE �� ��츸 m_strAddHairCps �� ���۵ǵ��� �Ѵ�.
	const TSTRING& GetAddHairCps() const	{ return m_strAddHairCps; }

	//WorldBattle
public:
	// �̰� ���Ǿ����� �ʴ´�.
	//! �¸��� Ŭ���� ���⸦ �ʿ� �����Ѵ�.
	//void SetFlagWinnerClubCountry( const TSTRING& stTextureName, const D3DXVECTOR4& vFlagConvert );

	// ���籺��
public:
	void SetWorldBattleFlag( const TCHAR* pWorldBattleFlag );

public:
	const char* GetFileName()	{ return m_strNameCPS.c_str(); }
	BOOL IsMeshContainer();
	BOOL IsActiveCharPart()		{ return m_sUsedLoadingChangeData.m_rSkinPiece ? TRUE : FALSE; }

	//D3DXMATRIX&	GetMatCombine()				{ return m_matCombine; }

protected:
	void SumEffFlag ();
	void AddDefaultAmbient();

public:
	void AddEffList ( DxEffChar* pEffChar );
	void DelEffList ( LPDIRECT3DDEVICEQ pd3dDevice, DxEffChar* pEffChar );
	void DelEffList ( const char* szEffFile );

	BOOL FindEffList ( const char* szEffFile );

public:
	BOOL SetPart ( const DxSkinCharEffAniBase* pSkinCharEffAniBase, DxSkinPiece* pSkinPiece, LPDIRECT3DDEVICEQ pd3dDevice, bool bAmbientUP, const char* pSelfBodyEffect );	
	DxSkinPiece* GetPart () { return m_sUsedLoadingChangeData.m_rSkinPiece; }
	void SetPart_Detail();		// SetPart �� �����̰� ThreadLoading ���� ���Ͽ� �����ε��� ����.
    void SetLODPart_Detail();    
	void ReSetPart_Detail();	// SetPart �� �����̰� �ٽ� �ѹ� m_bCompletedLINK �� �� �ε� �ϰ� ������ ����.
	void ReSetPart ();			// ��¥�� Part �� ���� ���δ�.

private:
	void ClearEffList ();

public:
	void SetRender ( BOOL bRender )			{ m_bRender = bRender; }
	void SetAttackMode ( BOOL bMode )		{ m_bAttackMode = bMode; }

    void ReloadVMFX( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	// BOOL bOriginDraw=TRUE, BOOL bAlpha=FALSE, BOOL bShadow=FALSE, BOOL bAlphaTex=TRUE, float fHeight=0.f
	HRESULT DrawMeshContainer ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bOriginDraw, BOOL bAlpha, BOOL bShadow, BOOL bAlphaTex, float fHeight, const DxSkeletonMaintain* pSkeleton );

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects();

	HRESULT FrameMove ( float fTime, float fETime, std::tr1::shared_ptr<DxSkinAniControl>& spSkinAniControlThread, BOOL bNoneOverFrame );

	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, 
					const DxSkeletonMaintain* pSkeleton,
					float* parrayfWeaponScale,
					SKINEFFDATA& sSKINEFFDATA, 
					const D3DXMATRIX& matWorld, 
					const SKIN_SPEC_SSS_INFO& sSkinInfo,
					BOOL bShadow=FALSE, 
					BOOL bEff=TRUE );

	HRESULT RenderShadow ( LPDIRECT3DDEVICEQ pd3dDevice, const DxSkeletonMaintain* pSkeleton, float fHeight );
	void RenderGlow ( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, const float fScale, const D3DXMATRIX* pAttBone );

	HRESULT RenderShadowForMaterial( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matViewProj, BOOL bMaterialSystemWLD );

    bool UpdateLOD(const D3DXVECTOR3& vPos, LPDIRECT3DDEVICEQ pd3dDevice);

	void GhostingON( BOOL bGhosting, float fLifeTime );

public:
	void SetPosOffset( const D3DXVECTOR3& vPosOffset );

	void RenderMaterialOpaque( LPDIRECT3DDEVICEQ pd3dDevice, 
								const DxSkeletonMaintain& sSkeleton,
								float* parrayfWeaponScale,
								SKINEFFDATA& sSKINEFFDATA, 
								float fAlpha, 
								const SKIN_SPEC_SSS_INFO& sSkinInfo, 
								const D3DXMATRIX* pmatAttParentBone,
								const D3DXMATRIX& matWorld,
								const float fScale,
								const DxClothColl* pClothColl,
								const std::vector<physx::PxClothCollisionSphere>* pvecClothCollSpheres,
								float fAttParentBoneScale,
								BOOL bEffect );

	void RenderMaterialHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha, const SKIN_SPEC_SSS_INFO& sSkinInfo );
	void RenderMaterialSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha, const SKIN_SPEC_SSS_INFO& sSkinInfo );

	void RenderMaterialAdd ( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderMaterialGlow ( LPDIRECT3DDEVICEQ pd3dDevice );

	BOOL RenderMaterialGhosting( LPDIRECT3DDEVICEQ pd3dDevice, SKINEFFDATA& sSKINEFFDATA, const SKIN_SPEC_SSS_INFO& sSkinInfo, float fAlpha, size_t unGhostingIndex );

private:
	void RenderFixed( LPDIRECT3DDEVICEQ pd3dDevice, 
						const DxSkeletonMaintain* pSkeleton, 
						float* parrayfWeaponScale, 
						SKINEFFDATA& sSKINEFFDATA, 
						const D3DXMATRIX& matWorld, 
						BOOL bShadow=FALSE, 
						BOOL bEff=TRUE );

	BOOL DrawMeshContainerMaterialOpaque( LPDIRECT3DDEVICEQ pd3dDevice, 
											const SKIN_SPEC_SSS_INFO& sSkinInfo, 
											const D3DXMATRIX* pmatAttParentBone, 
											const DxClothColl* pClothColl,
											const DxSkeletonMaintain& sSkeleton,
											float* parrayfWeaponScale,
											const std::vector<physx::PxClothCollisionSphere>* pvecClothCollSpheres,
											float fAttParentBoneScale,
											float fAlpha );

	BOOL IsEnableRender();

public:
	SVERTEXINFLU* GetTracePos(const std::string& strTrace);
	STRACOR* GetTracor(const std::string& strTrace);

	//	pBoneMatrice -> (����) ������ �޴� ���� ������ �������� ��쿡 ������ ����.
	//	������ �̸� ���Ǿ� �����Ƿ� ���� ������ ��ȯ.
	HRESULT CalculateVertexInflu ( std::string strTrace, D3DXVECTOR3 &vVert, D3DXVECTOR3 &vNormal, LPD3DXMATRIX pBoneMatrice=NULL );

	void GetStaticVertexData( std::vector<VERTEXNORCOLORTEX4>& vecVertices, const DxSkeleton* pSkeleton );

	// Material ���� ���� DxEffCharNone �� ���� ���, ���ο� Material Rendering �ʿ��� �⺻������ ���ؾ� �Ѵٰ� �˾������� �Ѵ�.
	void SetBaseRenderOff();



	//////////////////////////////////////////////////////////////////////////
	// �ܺ� �ɼǿ� ���� ON/OFF �� ���� ���� Data ��ȭ�� ȣ�� �Լ� �� ����
private:
	// PhysX Cloth �� ������ �ƴ� ���� Data�� ������ �־�� �Ѵ�.
	// SetPhysX_Cloth( BOOL bOnOff ) ���� TRUE �� ��� ���� �ְ�, FALSE ���� ���� ����.
	boost::shared_ptr<DxPhysXCloth>		m_spPhysXCloth;

public:
	BOOL SetPhysX_Cloth( BOOL bOnOff );
	// �ܺ� �ɼǿ� ���� ON/OFF �� ���� ���� Data ��ȭ�� ȣ�� �Լ� �� ����
	//////////////////////////////////////////////////////////////////////////

public:
	DxCharPart ();
	~DxCharPart ();
};
typedef DxCharPart* PDXCHARPART;





