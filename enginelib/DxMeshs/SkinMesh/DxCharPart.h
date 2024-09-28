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

// 옵션에서 ON 되어 있다면 모든 DxCharPart 에 DxPhysXCloth 이 다 만들어져야한다.
//
// 캐릭터 창에 있는 DxSkinChar 같은 경우 혹 옵션에서 OFF 가 되어 잇는 경우는 PhysX 가 OFF 되도 상관이 없기 때문에
// 그 밑에 DxCharPart 가 들고 있는 DxPhysXCloth 가 NULL 로 셋팅되어진다.
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
	DWORD				m_dwFlags;			// 현재 파트의 효과의 Flag 들을 더하고 빼고 함.
	DWORD				m_dwFlag;			// Shadow 를 뿌릴지 안 뿌릴 지를 정하는 옵션
	BOOL				m_bCompletedLINK;	// Link가 완료되었는지 확인



	//////////////////////////////////////////////////////////////////////////
	// Part 가 바뀌었는데 Thread Loading이라 렌더링을 이전정보가 유지되야 하는 경우 
	//		이전 Data가 뿐만 아니라 새로운 Part 의 정보가 될 수 있는 Data
	//////////////////////////////////////////////////////////////////////////
private:
	struct LoadingChangeData
	{
		// 연결된 SkinPiece
		DxSkinPiece*		m_rSkinPiece;

		// Note : Ambient 를 켠다. 캐릭터 밝기를 밝게한다.
		BOOL				m_bAmbientON;

		// 추가 Effect
		const char*			m_rSelfBodyEffect;	// ItemEdit 에서 셋팅한 효과	// 참조를 해도 좋을 Data	// Item 정보가 사라지지 않는한 문제 없다.

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
	// m_pBackUpData은 참조만한다; 데이터 로드 과정에서 이펙트 갱신/추가 요청이 오면 백업해둔후;
	// 로드가 끝나면 갱신해주기 위함; 로드후 널로 처리함;
	DxEffCharData*		m_pBackUpData;

	TSTRING				m_strNameCPS;
	//char				m_szFileName[FILE_LENGTH];

	const DxSkinCharEffAniBase*	m_rSkinCharEffAniBase;

	DxSkinMesh9*		m_rSkinMesh;

	
	EMPEACEZONEWEAPON	m_emWeaponWhereBack;

	PSMESHCONTAINER		m_rMeshContainer;			// 렌더링의 기준은 이 데이터로 하자. 있으면 렌더 없으면 렌더안함.

	DxSkinPiece*		m_rSkinPiece;			// 참조만 한다.

	DWORD				m_dwMaterialNum;
	//SMATERIAL_PIECE*	m_rarrayMaterialPiece;	// 참조만 하도록 하자.

    DWORD               m_cEffAmbientType;  // Effect에서 Ambient Type가 셋팅되면 저장된다.
    D3DXCOLOR           m_cEffAmbientColor; // Effect에서 Ambient Color가 셋팅되면 저장된다.

private:
	BOOL						m_bRender;
	BOOL						m_bAttackMode;
	const DxBoneTransMaintain*	m_pHandHeld;	// 무기위치 손..
	const DxBoneTransMaintain*	m_pSlotHeld;	// 무기위치 슬롯..

	//	Note : 추적 위치값 들.
	VECTRACE			m_vecTrace;

	//	Note : Trace Bone
	DxTraceBone*		m_rTraceBone;		// 참조값

	//	Note : 스킨 조각에 부착된 효과들.
	GLEFFCHAR_VEC		m_vecEFFECT;

	//	Note : 개조 레벨.
	int					m_nGrindLevel;

	// Note : 헤어컬러 // 메인 파츠 칼라
	DWORD				m_dwHairColor;
	DWORD				m_dwSubColor;

	//// Note : 일반 메쉬일 경우 최종 Matrix	- Skin 메쉬일 경우는 필요 없는 정보이다.
	//D3DXMATRIX			m_matCombine;
	
	// WorldBattle
	TSTRING			m_stTextureName;	// 사용되야하는 국기 텍스쳐이름
	D3DXVECTOR4		m_vFlagConvert;		// UV 변경을 위해 사용되는 수치

	// Note : 유저 Back Slot
	TSTRING				m_strUserBackSlot;
	D3DXMATRIX			m_matUserBackSlot;
	BOOL				m_bKeepUserBackSlot;

	// Note : MaterialSystem 에서 쓰이는 데이터
	boost::scoped_ptr<EffectMaterialData>	m_scpEffectMaterialData;

	// Overlay or UserColor 가 쓰인다면 원본 텍스쳐 대신 이 텍스쳐를 쓰도록 하자.
	// MakeOverlayTex 와 CreateEffRenderTarget 에서 쓰인다.
	LPDIRECT3DTEXTUREQ*	m_apOverlayTex;		// OverlayTexture

	// Thread 상에서 작업된다.
	std::tr1::shared_ptr<CharPartBoneMatrices>	m_spCharPartBoneMatrices;

	// StaticBone 일 경우의 Matrix
	D3DXMATRIX m_matStaticLinkBone;

	// SafeMode 시 무기가 EMPEACE_WEAPON_WHIP_SLOT, EMPEACE_WEAPON_BOX_SLOT, EMPEACE_WEAPON_UMBRELLA_SLOT 경우 미리 본의 위치를 가지고 있도록 한다.
	WORDWORD m_wwMoveParentBoneIDForSafeModeWeaponSlot;

	// 모자와 헤어와의 관계. 헤어를 어떻게 할 것인가~?
	HAT_HAIR_OPTION m_emHairHairOption;
	TSTRING			m_strAddHairCps;

	//// Bone Matrices 를 저장해 놓고 여러군데에서 사용하도록 한다.
	//// Opaque, Hard Alpha, Soft Alplha, Shadow, Glow
	//// 배열값 => m_pmcMesh->m_dwNumAttributeGroups * m_pmcMesh->m_dwNumPaletteEntries
	////
	//// Thread Loading 때문에 m_arrayBoneMatrices 이 NULL 인 상태로 Render 나 이곳 저곳 들어올 수 있다. 사용시 NULL 체크 후 사용하자.
	////
	//D3DXMATRIX*	m_arrayBoneMatrices;

	//// 반사쪽에서 쓰임.
	////
	//// Thread Loading 때문에 m_arrayBoneMatricesReflect 이 NULL 인 상태로 Render 나 이곳 저곳 들어올 수 있다. 사용시 NULL 체크 후 사용하자.
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

	// [shhan][2015.04.01] 현재는 World Battle 의 텍스쳐를 위해서 사용되고 있다.
	//						추후 추가 될 가능성이 있다.
	//						m_apOverlayTex 를 MakeOverlayTex 과 공통으로 사용한다.
	//void CreateEffRenderTarget();

	const D3DXMATRIXA16*	GetBoneMatrices() const;//	{ return m_arrayBoneMatrices; }

	const DxSkinPiece* GetSkinPiece() const		{ return m_rSkinPiece; }

	// 
	void GetMoveParentBoneIDForSafeModeWeaponSlot( WORDWORD& wwOut );

	// 모자와 헤어와의 관계. 헤어를 어떻게 할 것인가~?
	HAT_HAIR_OPTION GetHatHairOption() const							{ return m_emHairHairOption; }
	BOOL			IsHatHairOption( HAT_HAIR_OPTION emType ) const		{ return m_emHairHairOption==emType ? TRUE : FALSE; }

	// 모자 사용시 추가로 cps 를 렌더링 할 수 있도록 한다.
	// 모자의 일부머리카락을 표현하기 위한 cps 로 쓰인다.
	// m_bHairRenderOff 가 TRUE 일 경우만 m_strAddHairCps 이 동작되도록 한다.
	const TSTRING& GetAddHairCps() const	{ return m_strAddHairCps; }

	//WorldBattle
public:
	// 이건 사용되어지진 않는다.
	//! 승리한 클럽의 국기를 맵에 셋팅한다.
	//void SetFlagWinnerClubCountry( const TSTRING& stTextureName, const D3DXVECTOR4& vFlagConvert );

	// 난사군도
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
	void SetPart_Detail();		// SetPart 된 상태이고 ThreadLoading 으로 인하여 지연로딩을 위함.
    void SetLODPart_Detail();    
	void ReSetPart_Detail();	// SetPart 된 상태이고 다시 한번 m_bCompletedLINK 로 재 로딩 하고 싶을때 쓰임.
	void ReSetPart ();			// 진짜로 Part 를 뺄때 쓰인다.

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

	//	pBoneMatrice -> (주의) 영향을 받는 본의 갯수가 여러개일 경우에 문제가 생김.
	//	랜더시 미리 계산되어 있으므로 값을 꺼내서 반환.
	HRESULT CalculateVertexInflu ( std::string strTrace, D3DXVECTOR3 &vVert, D3DXVECTOR3 &vNormal, LPD3DXMATRIX pBoneMatrice=NULL );

	void GetStaticVertexData( std::vector<VERTEXNORCOLORTEX4>& vecVertices, const DxSkeleton* pSkeleton );

	// Material 이전 셋팅 DxEffCharNone 을 썼을 경우, 새로운 Material Rendering 쪽에서 기본렌더를 안해야 한다고 알아차려야 한다.
	void SetBaseRenderOff();



	//////////////////////////////////////////////////////////////////////////
	// 외부 옵션에 따른 ON/OFF 로 인한 내부 Data 변화시 호출 함수 및 변수
private:
	// PhysX Cloth 는 공유가 아닌 실제 Data를 가지고 있어야 한다.
	// SetPhysX_Cloth( BOOL bOnOff ) 에서 TRUE 일 경우 값이 있고, FALSE 경우는 값이 없다.
	boost::shared_ptr<DxPhysXCloth>		m_spPhysXCloth;

public:
	BOOL SetPhysX_Cloth( BOOL bOnOff );
	// 외부 옵션에 따른 ON/OFF 로 인한 내부 Data 변화시 호출 함수 및 변수
	//////////////////////////////////////////////////////////////////////////

public:
	DxCharPart ();
	~DxCharPart ();
};
typedef DxCharPart* PDXCHARPART;





